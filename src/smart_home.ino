#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <Servo.h>

// ---------- PIN MAP (change to match your diagram.json / circuit.md) ----------
#define DHT_PIN     2
#define PIR_PIN     3
#define RELAY_LIGHT 4
#define RELAY_FAN   5
#define BUZZER_PIN  6
#define BUTTON_PIN  7
#define LDR_PIN     A0
#define SERVO_PIN   9      // servo acts as the rotating fan

#define DHT_TYPE    DHT22      // use DHT11 if your diagram uses DHT11

// ---------- SETTINGS ----------
#define LCD_ADDR        0x27   // try 0x3F if the LCD stays blank
#define TEMP_THRESHOLD  30.0   // fan ON above this (deg C)
#define LDR_THRESHOLD   500    // light level split between dark and bright
#define LDR_DARK_IS_HIGH 1    // set to 1 if your LDR reads HIGH in the dark
#define RELAY_ON        HIGH
#define RELAY_OFF       LOW
#define LIGHT_HOLD_MS   5000   // keep light on this long after last motion

enum Mode { HOME, AWAY, ALARM };

DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);
Servo fanServo;
int fanAngle = 0;
int fanStep = 6;
unsigned long lastFanMove = 0;

Mode mode = HOME;
float temperature = 0;
bool motion = false;
bool lightOn = false;
bool fanOn = false;

unsigned long lastMotion = 0;
unsigned long lastDht = 0;
unsigned long lastLcd = 0;
unsigned long lastBtn = 0;
bool lastBtnState = HIGH;

bool isDark() {
  int v = analogRead(LDR_PIN);
#if LDR_DARK_IS_HIGH
  return v > LDR_THRESHOLD;
#else
  return v < LDR_THRESHOLD;
#endif
}

void setLight(bool on) {
  lightOn = on;
  digitalWrite(RELAY_LIGHT, on ? RELAY_ON : RELAY_OFF);
}

void setFan(bool on) {
  fanOn = on;
  digitalWrite(RELAY_FAN, on ? RELAY_ON : RELAY_OFF);
}

// Sweeps the servo back and forth while the fan is ON (visual "rotation")
void animateFan() {
  if (!fanOn) return;
  if (millis() - lastFanMove < 20) return;
  lastFanMove = millis();
  fanAngle += fanStep;
  if (fanAngle >= 180 || fanAngle <= 0) {
    fanStep = -fanStep;
    fanAngle = constrain(fanAngle, 0, 180);
  }
  fanServo.write(fanAngle);
}

void handleButton() {
  bool state = digitalRead(BUTTON_PIN);
  if (state == LOW && lastBtnState == HIGH && millis() - lastBtn > 250) {
    lastBtn = millis();
    if (mode == HOME)       mode = AWAY;
    else                    mode = HOME;   // AWAY or ALARM -> HOME
    noTone(BUZZER_PIN);
    if (mode == AWAY) setLight(false);
    lcd.clear();
    Serial.print("Mode: ");
    Serial.println(mode == HOME ? "HOME" : "AWAY");
  }
  lastBtnState = state;
}

void updateLcd() {
  lcd.setCursor(0, 0);
  lcd.print("T:");
  if (isnan(temperature)) lcd.print("--.-");
  else lcd.print(temperature, 1);
  lcd.print("C M:");
  lcd.print(motion ? "Y" : "N");
  lcd.print(" ");

  lcd.setCursor(0, 1);
  if (mode == HOME)       lcd.print("HOME ");
  else if (mode == AWAY)  lcd.print("AWAY ");
  else                    lcd.print("ALARM");
  lcd.print(" L:");
  lcd.print(lightOn ? "ON " : "OFF");
  lcd.print(" F:");
  lcd.print(fanOn ? "1" : "0");
}

void setup() {
  Serial.begin(9600);

  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RELAY_LIGHT, OUTPUT);
  pinMode(RELAY_FAN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  setLight(false);
  setFan(false);

  fanServo.attach(SERVO_PIN);
  fanServo.write(0);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Home");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(1500);
  lcd.clear();

  Serial.println("Smart Home Automation started");
}

void loop() {
  handleButton();

  motion = digitalRead(PIR_PIN) == HIGH;
  if (motion) lastMotion = millis();

  // Read temperature every 2 seconds
  if (millis() - lastDht > 2000) {
    lastDht = millis();
    float t = dht.readTemperature();
    if (!isnan(t)) temperature = t;
    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.print(" C | Motion: ");
    Serial.print(motion);
    Serial.print(" | Dark: ");
    Serial.println(isDark());
  }

  switch (mode) {
    case HOME:
      // Light: motion recently detected AND dark
      if (isDark() && (millis() - lastMotion < LIGHT_HOLD_MS) && lastMotion != 0)
        setLight(true);
      else
        setLight(false);
      // Fan: temperature based
      setFan(temperature > TEMP_THRESHOLD);
      noTone(BUZZER_PIN);
      break;

    case AWAY:
      setLight(false);
      setFan(false);
      if (motion) {
        mode = ALARM;
        lcd.clear();
        Serial.println("INTRUSION! ALARM");
      }
      break;

    case ALARM:
      setLight(true);
      tone(BUZZER_PIN, 1000);
      break;
  }

  animateFan();

  if (millis() - lastLcd > 300) {
    lastLcd = millis();
    updateLcd();
  }
}


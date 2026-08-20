/*
 * Smart Home Automation System
 * ------------------------------
 * Subsystems:
 *   1. Lighting control  - LDR + PIR -> auto light ON/OFF
 *   2. Temperature control - DHT sensor -> fan relay
 *   3. Security           - PIR + buzzer -> intrusion alert (Away mode)
 *
 * Board: Arduino Uno / Mega
 * Also runs unmodified in Wokwi (https://wokwi.com) simulation.
 *
 * License: MIT
 */

#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// ---------- Pin definitions ----------
#define DHTPIN        2
#define DHTTYPE       DHT22      // change to DHT11 if that's what you have
#define LDR_PIN       A0
#define PIR_PIN       3
#define LIGHT_RELAY   4
#define FAN_RELAY     5
#define BUZZER        6
#define MODE_BUTTON   7          // toggles HOME / AWAY mode

// ---------- Thresholds (tune to your hardware) ----------
const int   TEMP_THRESHOLD = 30;    // deg C, fan ON above this
const int   LDR_THRESHOLD  = 400;   // analogRead value, lower = darker

// ---------- State machine ----------
enum SystemMode { HOME, AWAY, ALARM };
SystemMode currentMode = HOME;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);   // change address to 0x3F if LCD is blank

bool lastButtonState = HIGH;

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();

  pinMode(PIR_PIN, INPUT);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  pinMode(LIGHT_RELAY, OUTPUT);
  pinMode(FAN_RELAY, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(LIGHT_RELAY, LOW);
  digitalWrite(FAN_RELAY, LOW);
  digitalWrite(BUZZER, LOW);

  lcd.setCursor(0, 0);
  lcd.print("Smart Home Sys");
  delay(1500);
  lcd.clear();
}

void loop() {
  handleModeButton();

  float temp        = dht.readTemperature();
  int   lightLevel  = analogRead(LDR_PIN);
  int   motion      = digitalRead(PIR_PIN);

  handleLighting(motion, lightLevel);
  handleTemperature(temp);
  handleSecurity(motion);
  updateDisplay(temp, motion);

  delay(500);
}

// Debounced button press toggles HOME <-> AWAY
void handleModeButton() {
  bool reading = digitalRead(MODE_BUTTON);
  if (reading == LOW && lastButtonState == HIGH) {
    currentMode = (currentMode == HOME) ? AWAY : HOME;
    digitalWrite(BUZZER, LOW); // clear any alarm on mode change
    delay(200); // simple debounce
  }
  lastButtonState = reading;
}

void handleLighting(int motion, int lightLevel) {
  if (currentMode == AWAY) {
    digitalWrite(LIGHT_RELAY, LOW); // lights off while away
    return;
  }
  if (motion == HIGH && lightLevel < LDR_THRESHOLD) {
    digitalWrite(LIGHT_RELAY, HIGH);
  } else {
    digitalWrite(LIGHT_RELAY, LOW);
  }
}

void handleTemperature(float temp) {
  if (!isnan(temp) && temp > TEMP_THRESHOLD) {
    digitalWrite(FAN_RELAY, HIGH);
  } else {
    digitalWrite(FAN_RELAY, LOW);
  }
}

void handleSecurity(int motion) {
  if (currentMode == AWAY && motion == HIGH) {
    currentMode = ALARM;
  }
  digitalWrite(BUZZER, currentMode == ALARM ? HIGH : LOW);
}

void updateDisplay(float temp, int motion) {
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(isnan(temp) ? 0 : temp, 1);
  lcd.print("C M:");
  lcd.print(motion ? "Y" : "N");
  lcd.print("   ");

  lcd.setCursor(0, 1);
  switch (currentMode) {
    case HOME:  lcd.print("Mode: HOME    "); break;
    case AWAY:  lcd.print("Mode: AWAY    "); break;
    case ALARM: lcd.print("** INTRUDER **"); break;
  }
}

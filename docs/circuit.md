# Circuit & Wiring Guide

## Pin Mapping

| Component           | Arduino Pin | Notes                                  |
|----------------------|-------------|------------------------------------------|
| DHT22 / DHT11 data   | D2          | Add 10k pull-up if using bare sensor      |
| PIR sensor OUT        | D3          | HIGH = motion detected                    |
| Light relay IN        | D4          | Controls room light                       |
| Fan relay IN           | D5          | Controls fan/cooling                      |
| Buzzer +               | D6          | Active buzzer, HIGH = sound               |
| Mode button             | D7          | INPUT_PULLUP, press toggles HOME/AWAY    |
| LDR (via divider)       | A0          | Voltage divider with 10k resistor        |
| LCD SDA                 | A4 (SDA)    | I2C LCD, address usually 0x27 or 0x3F    |
| LCD SCL                 | A5 (SCL)    | I2C LCD                                   |

## Power
- All sensors/modules run on Arduino 5V/GND.
- If driving a real mains bulb/fan through the relay, use an
  opto-isolated relay module and route mains wiring separately from
  the low-voltage logic side. For a demo, a 12V LED strip or small
  DC fan is safer and just as effective.

## LDR Voltage Divider
```
5V ---[LDR]---+---[10k resistor]--- GND
              |
              A0
```
Reading is LOW in bright light, HIGH in darkness (or vice versa
depending on which leg you tap — calibrate `LDR_THRESHOLD` in code
after testing with a serial print).

## State Machine
```
        button press           motion detected (AWAY only)
  HOME ----------------> AWAY ---------------------------> ALARM
   ^                       ^                                  |
   |_______________________|__________________________________|
              button press clears alarm / returns to HOME
```

## Simulating in Wokwi (no hardware required)
1. Go to https://wokwi.com/projects/new/arduino-uno
2. Open the sketch editor and paste in `src/smart_home.ino`
3. Either rebuild the diagram using the pin map above, or import
   `wokwi/diagram.json` directly (the Wokwi VS Code extension supports
   opening a project folder that has `wokwi.toml` + `diagram.json`).
4. Press the green Play button. Click the PIR sensor to simulate
   motion, drag the DHT slider for temperature, and drag the LDR
   slider for ambient light.

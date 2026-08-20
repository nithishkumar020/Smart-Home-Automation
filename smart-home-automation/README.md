# Smart Home Automation with Sensors (Arduino + Embedded C)

A microcontroller-based smart home system built on Arduino that
automates **lighting**, **temperature control**, and **security**
using sensor input and a simple state machine — no cloud, no app
required, though hooks for adding both are included.

Runs on real Arduino hardware **or** entirely in the free
[Wokwi](https://wokwi.com) simulator, so no physical components are
required to build, test, or demo it.

## Features

- Automatic lighting — turns lights on only when motion is
  detected *and* ambient light is low, and off otherwise
- Temperature-based fan control — DHT sensor drives a relay to
  cool the room above a threshold
- Security / intrusion alert — PIR sensor triggers a buzzer
  alarm when the system is armed in AWAY mode
- Live LCD status display — temperature, motion, and current
  mode (HOME / AWAY / ALARM)
- Clean state machine (`HOME -> AWAY -> ALARM`) instead of flat
  if/else logic — easy to extend and explain in an interview

## Repository structure

```
smart-home-automation/
├── src/
│   └── smart_home.ino        # Main Arduino sketch
├── wokwi/
│   ├── diagram.json          # Wokwi simulation circuit
│   └── wokwi.toml            # Wokwi project config
├── docs/
│   ├── circuit.md            # Pin mapping, wiring, state diagram
│   └── bill_of_materials.md  # Parts list + prices
├── LICENSE
└── README.md
```

## Hardware

| Component            | Purpose                        |
|------------------------|-----------------------------------|
| Arduino Uno             | Main controller                  |
| DHT11 / DHT22           | Temperature & humidity sensing   |
| LDR + 10kΩ resistor     | Ambient light sensing            |
| PIR motion sensor        | Motion detection                 |
| 2-channel relay module   | Switch light + fan                |
| 16x2 I2C LCD             | Status display                   |
| Active buzzer             | Security alarm                   |
| Push button                | HOME / AWAY mode toggle          |

Full parts list with pricing: [`docs/bill_of_materials.md`](docs/bill_of_materials.md)

## Wiring

See [`docs/circuit.md`](docs/circuit.md) for the full pin map and a
labeled state-machine diagram.

## Getting started

### Option A — Real hardware
1. Wire components per [`docs/circuit.md`](docs/circuit.md).
2. Install libraries in Arduino IDE: **DHT sensor library** (Adafruit)
   and **LiquidCrystal_I2C**.
3. Open `src/smart_home.ino`, select your board/port, and upload.

### Option B — No hardware (Wokwi simulation)
1. Go to [wokwi.com](https://wokwi.com/projects/new/arduino-uno).
2. Paste `src/smart_home.ino` into the sketch editor.
3. Import `wokwi/diagram.json` (or the Wokwi VS Code extension can
   open this whole folder directly, since it already has a
   `wokwi.toml`).
4. Hit Play. Click the PIR sensor to simulate motion, and drag the
   DHT/LDR sliders to change temperature and light level live.

## How it works

```
HOME  --(button press)-->  AWAY  --(motion detected)-->  ALARM
 ^                                                          |
 |__________________(button press)________________________|
```

- **HOME**: lights respond to motion + darkness, fan responds to
  temperature, no alarm.
- **AWAY**: lights stay off, but motion triggers the alarm.
- **ALARM**: buzzer sounds until the mode button is pressed again.

## Possible extensions

- Add an **ESP8266** for a WiFi dashboard or phone app control
- **Datalog** sensor readings to an SD card or cloud endpoint
- Swap the push button for an **RFID reader** or keypad PIN
- Replace polling with `attachInterrupt()` on the PIR pin

## License

MIT — see [LICENSE](LICENSE).

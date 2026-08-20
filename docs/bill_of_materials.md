# Bill of Materials (BOM)

| Component                          | Qty | Approx. Price (INR) | Notes |
|--------------------------------------|-----|------------------------|--------|
| Arduino Uno R3 (or clone)            | 1   | 450–600                | Any Uno-compatible board works |
| DHT11 or DHT22 sensor                | 1   | 80–350                 | DHT22 is more accurate |
| LDR (5mm) + 10kΩ resistor             | 1   | 10                      | For light sensing |
| PIR motion sensor (HC-SR501)         | 1   | 90–150                  | Has sensitivity/delay trimmers |
| 2-channel relay module (5V)          | 1   | 100–150                 | Opto-isolated preferred |
| Active buzzer (5V)                   | 1   | 15–30                   | |
| 16x2 LCD with I2C backpack           | 1   | 150–250                 | Saves wiring vs raw 16-pin LCD |
| Push button                          | 1   | 5                        | Home/Away toggle |
| Breadboard (830 point)               | 1   | 100–150                 | |
| Jumper wire set (M-M, M-F, F-F)      | 1   | 100–150                 | |
| USB cable (A to B)                   | 1   | 60–100                   | |

**Estimated total: ₹1,200 – ₹1,800**

## Optional upgrades
| Component            | Purpose                                |
|------------------------|------------------------------------------|
| ESP8266 (NodeMCU)      | WiFi dashboard / remote control          |
| HC-05 Bluetooth module | Phone app control                        |
| RC522 RFID module      | Card-based security instead of button    |
| SG90 servo             | Simulated door lock                      |
| SD card module          | Data logging                             |

## No hardware yet?
See [`docs/circuit.md`](circuit.md) for how to simulate the entire
project in [Wokwi](https://wokwi.com) with no physical components.

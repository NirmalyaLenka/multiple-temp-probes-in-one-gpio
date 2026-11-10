# ESP32 Multi-Sensor Temperature Bus (Single GPIO)

Connect any number of temperature sensors to one ESP32 GPIO pin using the
1-Wire (OneWire) protocol. Each sensor has a unique factory-burned 64-bit
ROM address, so many sensors can share a single data line — no I2C address
conflicts, no extra GPIOs per sensor.

## Use case

Any scenario where you need distributed temperature readings but want to
save GPIOs and wiring: multi-zone greenhouse/incubator monitoring, battery
pack cell temps, server rack thermal mapping, HVAC duct sensing, 3D printer
bed zones, soil temperature arrays. Add or remove sensors without touching
firmware — the bus is enumerated at runtime.

Limits: 1-Wire is a shared bus, not a parallel one. More sensors = longer
enumeration + conversion time per polling cycle (roughly 750 ms per
conversion at 12-bit resolution, regardless of sensor count, since
conversions happen in parallel on the bus — but each device's data must be
read back sequentially, ~a few ms each). Bus length over ~30-50 m or heavy
EMI environments need shielded cable and parasite-power care.

## Repo layout

```
esp32-multi-ds18b20/
├── README.md                  this file
├── docs/
│   ├── WIRING.md               wiring diagram + pull-up resistor notes
│   └── SENSORS.md               compatible sensor list + selection notes
├── src/
│   └── main.cpp                 Arduino/PlatformIO firmware (scan + read all)
├── micropython/
│   └── main.py                  MicroPython equivalent
└── platformio.ini               PlatformIO build config
```

## Quick start (Arduino/PlatformIO)

1. Wire sensors per `docs/WIRING.md`.
2. `pio run -t upload -t monitor`
3. Console prints each sensor's ROM address and temperature every cycle.

## Quick start (MicroPython)

1. Wire sensors per `docs/WIRING.md`.
2. Flash MicroPython firmware to the ESP32.
3. Copy `micropython/main.py` to the board as `main.py` (use `ampy`,
   `mpremote`, or Thonny).
4. Reset the board. REPL prints each sensor's ROM address and temperature.

## How the addressing works

Every 1-Wire device (DS18B20 etc.) ships with a unique 64-bit ROM code
burned in at the factory (8-bit family code + 48-bit serial + 8-bit CRC).
On power-up, the master (ESP32) runs a "search ROM" algorithm over the bus
to discover every connected device's address without any manual
configuration or address pins. Firmware stores the discovered address list
and reads each device by address on every cycle.


---

## Skills & technologies

`Arduino/C++` · `Python`


## Repository structure

Key files and folders (not exhaustive):

- `LICENSE`
- `README.md`
- `SENSORS.md`
- `WIRING.md`
- `download`
- `main.cpp`
- `main.py`
- `platformio.ini`


## How it works

Python source is present, including `main.py`. Review configuration and dependencies before running; use a virtual environment and follow the project's setup instructions.


## Maintenance & Audit

- **Audit date:** 10 November 2026
- **Documentation status:** README reviewed and audit record added on this date.
- **Scope and evidence:** This is a documentation-level maintenance pass. It does not establish that source code compiles, dependencies are current, security has been tested, or runtime behavior has been verified.
- **Code and tests:** No new functional code changes or test results are claimed by this README update. Run project-specific build, test, and security checks before relying on the project.
- **Maintenance baseline:** Documentation status recorded through 10 November 2026; revisit setup instructions, dependencies, security, known limitations, and tests when the project changes.

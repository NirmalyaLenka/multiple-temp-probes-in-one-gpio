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

## Repository layout

```text
.
├── LICENSE
├── README.md
├── SENSORS.md
├── WIRING.md
├── download              # ignore-pattern text; not a download script
├── main.cpp              # Arduino/PlatformIO implementation
├── main.py               # MicroPython alternative
└── platformio.ini        # PlatformIO environment: esp32dev
```

The current checkout stores these files at the repository root. It does not contain the `docs/`, `src/`, or `micropython/` directories shown in some older examples. The Arduino and MicroPython programs are alternatives; use one implementation at a time.

## Wiring

Use [WIRING.md](WIRING.md) for the bus diagram and electrical notes. For a typical DS18B20 bus:

- Connect every sensor's DQ pin to **ESP32 GPIO4**.
- Connect all sensor VDD pins to **3.3 V** and all GND pins to ESP32 GND.
- Fit one **4.7 kΩ pull-up** between DQ and 3.3 V for the shared bus, not one resistor per sensor.
- Do not pull the data line up to 5 V; ESP32 GPIOs are 3.3 V logic.

See [SENSORS.md](SENSORS.md) for compatible 1-Wire sensor families and limitations.

## Quick start: Arduino / PlatformIO

The checked-in `platformio.ini` selects the `esp32dev` board, Arduino framework, 115200-baud serial monitor, and the OneWire and DallasTemperature libraries.

**Known layout issue:** PlatformIO's default source directory is `src/`, but the current `main.cpp` is at the repository root and `platformio.ini` does not override `src_dir`. A standard build may therefore omit the sketch. This pass intentionally did not move source files or change build configuration. Resolve that layout mismatch separately (for example, place a copy at `src/main.cpp` or configure `src_dir = .`) before relying on the commands below.

After resolving the layout, run from the repository root:

```bash
pio run
pio run -t upload
pio device monitor -b 115200
```

Install PlatformIO Core or use the PlatformIO IDE integration first. If multiple serial devices are connected, select the correct upload/monitor port using your local PlatformIO configuration.

## Quick start: MicroPython

1. Wire the sensors as described in [WIRING.md](WIRING.md).
2. Flash an ESP32-compatible MicroPython firmware image.
3. Copy the root-level `main.py` to the board's filesystem as `main.py` (for example, with Thonny or `mpremote`).
4. Confirm the firmware provides the `onewire` and `ds18x20` modules, then reset the board and open the serial REPL.

The script uses GPIO4, scans for sensor ROM addresses, waits 750 ms for a 12-bit conversion, prints each reading, and waits another five seconds before the next cycle.

## Runtime behavior and limits

- Both implementations use **GPIO4** for the 1-Wire data line.
- The Arduino sketch prints at **115200 baud**, scans at startup, retries discovery every five seconds if no devices are found, and reports a read error when a previously discovered device is disconnected.
- The Arduino sketch stores at most **32 sensor addresses**. It silently caps a larger discovered count at 32; it is not an unlimited-address implementation.
- The MicroPython script scans at startup and retries every five seconds only when no devices were found. It reads each discovered ROM address after a 750 ms conversion wait.
- The five-second delay is added after conversion/read work, so a 12-bit polling cycle takes roughly 5.75 seconds plus small overhead; it is not a strict five-second start-to-start interval.
- Practical bus capacity depends on wiring, pull-up strength, cable length, noise, and sensor power. The wiring notes are guidance, not a guarantee for every installation.

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


## Troubleshooting

| Symptom | Checks |
|---|---|
| No devices detected | Confirm DQ is on GPIO4, the shared 4.7 kΩ pull-up reaches 3.3 V, and every sensor shares GND. |
| A sensor reports a read error | Check that sensor's wiring and power; inspect the printed ROM address and connector continuity. |
| PlatformIO builds without the sketch or fails to find source | Resolve the documented root-level `main.cpp` versus default `src/` layout mismatch first. |
| MicroPython reports an import error | Verify the installed firmware includes `onewire` and `ds18x20`; module availability can vary by firmware build. |
| Readings are unstable on long wiring | Review pull-up, bus topology, grounding, cable length, and interference guidance in [WIRING.md](WIRING.md). |

## Verification status

The source files, PlatformIO configuration, sensor notes, and wiring notes were inspected for this documentation update. No PlatformIO build, MicroPython runtime check, or physical sensor test was performed. The PlatformIO source-directory mismatch remains unresolved; the upload commands above are conditional on fixing it.

## Maintenance & Audit

- **Audit date:** 11 November 2026
- **Documentation status:** README reviewed and audit record added on this date.
- **Scope and evidence:** This is a documentation-level maintenance pass. It does not establish that source code compiles, dependencies are current, security has been tested, or runtime behavior has been verified.
- **Code and tests:** No new functional code changes or test results are claimed by this README update. Run project-specific build, test, and security checks before relying on the project.
- **Maintenance baseline:** Documentation status recorded through 11 November 2026; revisit setup instructions, dependencies, security, known limitations, and tests when the project changes.

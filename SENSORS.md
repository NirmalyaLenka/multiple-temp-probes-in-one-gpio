# Compatible Sensors

Single-GPIO multi-sensor buses require the sensor to speak the 1-Wire
protocol with a unique factory ROM address. I2C temperature sensors are
NOT compatible with this approach unless each one supports a hardware
address pin AND you're willing to use two GPIOs (SDA+SCL) instead of one —
out of scope for this repo, which is specifically the 1-Wire single-line
method.

## Recommended

| Sensor  | Range          | Accuracy      | Resolution     | Notes |
|---------|----------------|---------------|----------------|-------|
| DS18B20 | -55°C to 125°C | ±0.5°C (-10 to 85°C) | 9-12 bit configurable | Most common, cheap, widely stocked, waterproof probe versions available |
| DS1822  | -55°C to 125°C | ±2°C          | 9-12 bit configurable | Cheaper DS18B20 variant, lower accuracy |
| MAX31820 | -55°C to 125°C | ±0.5°C (-10 to 85°C) | 9-12 bit configurable | Maxim's version, pin/protocol compatible with DS18B20 |
| DS18S20 | -55°C to 125°C | ±0.5°C (-10 to 85°C) | fixed 9-bit (extended via algorithm) | Older part, still supported by DallasTemperature library |

DS18B20 is the default recommendation: cheapest, most library support,
available as bare TO-92 chips or pre-potted waterproof probes (common for
liquid/soil/outdoor use).

## Not compatible with a single-GPIO 1-Wire bus

- DHT11 / DHT22 / AM2302 — single-drop protocol, only one device per pin,
  no addressing.
- LM35, TMP36 — analog output, one ADC pin per sensor, not a bus at all.
- BME280, SHT31, MCP9808, TMP117 — I2C or SPI, need shared clock line and
  distinct addresses; not part of this repo's approach.
- Thermocouples (MAX6675, MAX31855) — SPI, one CS pin per sensor typically.

## Mixing sensor counts

The DallasTemperature/OneWire library (and MicroPython's `onewire` +
`ds18x20` modules) both enumerate devices at boot with no code changes
needed when you add or remove sensors — only the physical wiring changes.
Practical bus limits are in the hundreds of devices; real-world projects
are usually capacity-limited by cable length and pull-up strength long
before hitting a device-count ceiling.

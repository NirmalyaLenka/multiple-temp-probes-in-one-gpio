import time
from machine import Pin
import onewire
import ds18x20

ONE_WIRE_PIN = 4
READ_INTERVAL_S = 5

def scan_bus(ds):
    roms = ds.scan()
    print("Devices found:", len(roms))
    for rom in roms:
        print("  address:", rom.hex())
    return roms

def main():
    bus = onewire.OneWire(Pin(ONE_WIRE_PIN))
    ds = ds18x20.DS18X20(bus)

    roms = scan_bus(ds)

    while True:
        if not roms:
            time.sleep(READ_INTERVAL_S)
            roms = scan_bus(ds)
            continue

        ds.convert_temp()
        time.sleep_ms(750)  # conversion time at 12-bit resolution

        for rom in roms:
            try:
                temp_c = ds.read_temp(rom)
                print("Sensor", rom.hex(), ":", round(temp_c, 2), "C")
            except Exception as e:
                print("Sensor", rom.hex(), ": read error -", e)

        time.sleep(READ_INTERVAL_S)

if __name__ == "__main__":
    main()

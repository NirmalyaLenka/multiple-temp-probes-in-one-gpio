# Wiring

## Diagram

```
                      3V3 (ESP32)
                       │
                      ┌┴┐
                      │ │ 4.7kΩ pull-up
                      └┬┘
                       │
  ESP32 GPIO4 ─────────┼──────────────┬──────────────┬─────────── ...
     (DQ / data)       │              │              │
                     ┌──┴──┐        ┌──┴──┐        ┌──┴──┐
                     │DQ   │        │DQ   │        │DQ   │
                     │DS18 │        │DS18 │        │DS18 │
                     │B20  │        │B20  │        │B20  │
                     │ #1  │        │ #2  │        │ #3  │
                     └┬───┬┘        └┬───┬┘        └┬───┬┘
                      │   │          │   │          │   │
                 VDD──┘   └──GND VDD─┘   └──GND VDD─┘   └──GND
                      │              │              │
   3V3 ───────────────┴──────────────┴──────────────┴─────────── (all VDD)
   GND ───────────────┴──────────────┴──────────────┴─────────── (all GND)
```

All sensors tie into the same three rails: 3V3, GND, and the single DQ line
back to one ESP32 GPIO. The bus topology can be a straight line, a star, or
short stubs off a trunk — 1-Wire tolerates all of these at moderate cable
lengths; a single trunk with short stubs is best practice for long runs.

## Pull-up resistor

- One 4.7 kΩ resistor between DQ and 3V3, placed once, anywhere on the bus
  (near the ESP32 is easiest). Do not add one resistor per sensor.
- Drop to 2.2 kΩ–3.3 kΩ if you have many sensors (10+) or long cable runs,
  since bus capacitance increases and needs a stronger pull-up to recover
  the line fast enough.
- ESP32 GPIOs are 3.3 V logic — do not run this bus at 5 V unless every
  sensor and the pull-up are also referenced to a level that's safe for the
  GPIO (DS18B20 works fine at 3.3 V, so just power the whole bus at 3.3 V).

## Pin choice on ESP32

- Any GPIO capable of input/output works: GPIO4, GPIO5, GPIO15, GPIO18,
  GPIO19, GPIO21, GPIO22, GPIO23 are common safe choices.
- Avoid strapping pins (GPIO0, GPIO2, GPIO12, GPIO15 has caveats) if you
  need reliable cold boot; GPIO4 or GPIO5 are trouble-free defaults.
- Avoid input-only pins (GPIO34–39) — 1-Wire needs the pin to also drive
  output (the master pulls the line low to initiate resets/writes).

## Power mode

- **Normal (recommended):** 3 wires per sensor (VDD, GND, DQ). Simple,
  reliable, works at any sensor count.
- **Parasite power (2 wires, VDD tied to GND):** possible but not
  recommended for multi-sensor buses — timing gets tight when several
  sensors draw current from the data line during conversion. Only use it
  if you are wiring-constrained and have fewer than ~5 sensors on a short
  bus.

## Physical connectors

For anything beyond a breadboard prototype, use a screw-terminal or JST
trunk with short stub drops to each sensor, and twisted-pair or shielded
cable if runs exceed a couple of meters or share conduit with noisy loads
(motors, relays, PSUs).

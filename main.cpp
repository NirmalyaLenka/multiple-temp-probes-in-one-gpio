#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_PIN 4
#define READ_INTERVAL_MS 5000

OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

DeviceAddress deviceAddresses[32];
uint8_t deviceCount = 0;

void printAddress(const DeviceAddress addr) {
  for (uint8_t i = 0; i < 8; i++) {
    if (addr[i] < 0x10) Serial.print("0");
    Serial.print(addr[i], HEX);
  }
}

void scanBus() {
  sensors.begin();
  deviceCount = sensors.getDeviceCount();
  if (deviceCount > 32) deviceCount = 32;

  Serial.print("Devices found: ");
  Serial.println(deviceCount);

  for (uint8_t i = 0; i < deviceCount; i++) {
    if (sensors.getAddress(deviceAddresses[i], i)) {
      Serial.print("  #");
      Serial.print(i);
      Serial.print(" address: ");
      printAddress(deviceAddresses[i]);
      Serial.print(" resolution: ");
      Serial.println(sensors.getResolution(deviceAddresses[i]));
    } else {
      Serial.print("  #");
      Serial.print(i);
      Serial.println(" address read failed");
    }
  }

  if (deviceCount == 0) {
    Serial.println("No devices detected. Check wiring and pull-up resistor.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("ESP32 1-Wire multi-sensor bus scan");
  scanBus();
}

void loop() {
  if (deviceCount == 0) {
    // Retry discovery periodically in case sensors were connected after boot.
    delay(READ_INTERVAL_MS);
    scanBus();
    return;
  }

  sensors.requestTemperatures(); // triggers conversion on all devices in parallel

  for (uint8_t i = 0; i < deviceCount; i++) {
    float tempC = sensors.getTempC(deviceAddresses[i]);
    Serial.print("Sensor ");
    printAddress(deviceAddresses[i]);
    Serial.print(": ");
    if (tempC == DEVICE_DISCONNECTED_C) {
      Serial.println("read error (disconnected?)");
    } else {
      Serial.print(tempC, 2);
      Serial.println(" C");
    }
  }

  delay(READ_INTERVAL_MS);
}

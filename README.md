# Weather Station

Embedded weather station using:
- Arduino Mega 2560
- DHT22
- BMP280
- OLED Display (SH1106)
- Press Button
- PlatformIO

## Features

- Temperature measurement and display
- Display Min/Max values
- Humidity measurement
- Air pressure measurement (BMP280)
- OLED UI
- Cooperative scheduler
- Data logging to SD

## Hardware

| Component | Connection |
|...........|------------|
| DHT22     | Pin 2      |
| OLED SDA  | SDA        |
| OLED SCL  | SCL        |

## Project Structure

src/
include/
lib/

## Learning Goals

- Embedded architecture
- I2C/SPI communication
- Scheduling
- State machines
- Sensor integration
- Data logging
- Time stamps
- EEPROM
- ESP32 + WLAN + Webdashboard

## Scheduler

The application uses a cooperative scheduler
based on millis() timing.

Tasks:
- Sensor update every 2 seconds
- Display of Min/Max values
- Display refresh every 200 ms
- LED heartbeat every 500 ms (conditional activation, warning of high humidity)
- History with history plot frequency every 6 min -> 12 h window on display


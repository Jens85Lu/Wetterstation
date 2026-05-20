# Weather Station

Embedded weather station using:
- Arduino Mega 2560
- DHT22
- OLED Display (SH1106)
- PlatformIO

## Features

- Temperature measurement
- Humidity measurement
- OLED UI
- Cooperative scheduler
- Data logging to SD

## Hardware

| Component | Connection |
|---|---|
| DHT22 | Pin 2 |
| OLED SDA | SDA |
| OLED SCL | SCL |

## Project Structure

src/
include/
lib/

## Learning Goals

- Embedded architecture
- I2C communication
- Scheduling
- State machines
- Sensor integration
- Data logging

## Scheduler

The application uses a cooperative scheduler
based on millis() timing.

Tasks:
- Sensor update every 2 seconds
- Display refresh every 200 ms
- LED heartbeat every 500 ms

# 🌦️ Weather Station

Embedded weather station project using Arduino Mega 2560 and multiple sensors for environmental monitoring.

---

## 🔧 Hardware

- Arduino Mega 2560
- DHT22 (Temperature & Humidity)
- BMP280 (Air Pressure)
- SH1106 OLED Display (I2C)
- Push Button
- PlatformIO (VSCode)

---

## ✨ Features

- Temperature & humidity measurement (DHT22)
- Air pressure measurement (BMP280)
- OLED display with UI screens
- Min / Max tracking
- Historical data logging
- Graph visualization (temperature & humidity)
- Cooperative scheduler (non-blocking millis timing)
- Button-based screen navigation
- LED status indicator

---

## 📊 Wiring

### Arduino Mega 2560 Connections

| Component | Signal | Arduino Pin |
|----------|--------|-------------|
| DHT22    | Data   | 2           |
| BMP280   | SDA    | SDA (20)    |
| BMP280   | SCL    | SCL (21)    |
| OLED     | SDA    | SDA (20)    |
| OLED     | SCL    | SCL (21)    |
| Button   | Signal | 3           |
| LED      | Output | 13          |

> Note: SDA/SCL are shared via I2C bus

---

## 📁 Project Structure

src/
include/
lib/
platformio.ini

## 🎯 Learning Goals

- Embedded system architecture
- Sensor integration (I2C / digital)
- Non-blocking scheduler design
- State machine UI design
- Ring buffer data handling
- Embedded graph rendering
- Modular C++ design

---

## ⏱️ Scheduler

- Sensor update: 2s
- Display update: 200ms
- LED blink: 500ms
- History update: ~6 min intervals


## 📌 Future

- SD card logging
- ESP32 web dashboard
- Sensor calibration layer





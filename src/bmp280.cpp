#include "bmp280.h"
#include <Wire.h>

#include <Arduino.h>

#include <Adafruit_BMP280.h>

static Adafruit_BMP280 bmp;

bool bmp280_init() {
    
    return bmp.begin(0x76); // I2C-Adresse des BMP280, je nach Sensor kann es auch 0x77 sein
}

float bmp280_getPressure() {
    
    return bmp.readPressure() / 100.0f;
}

float seaLevelPressure(float pressure, float altitude) {
    return pressure / pow(1.0f - (altitude / 44330.0f), 5.255f);
}

float bmp280_getTemperature() {
    
    return bmp.readTemperature();
}

float bmp280_getAltitude(float seaLevelhPa) {
    
    return bmp.readAltitude(seaLevelhPa);
}
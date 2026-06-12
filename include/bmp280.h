#ifndef BMP280_H
#define BMP280_H

bool bmp280_init();

float bmp280_getPressure();
float bmp280_getTemperature();
float bmp280_getAltitude(float seaLevelhPa = 1013.25f);
float seaLevelPressure(float pressure, float altitude);

#endif // BMP280_H
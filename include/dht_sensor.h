// Sensor lesen und Fehler behandeln
#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

void dht_init();

float dht_getTemperature();

float dht_getHumidity(); 

#endif
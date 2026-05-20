#include "dht_sensor.h"
#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void dht_init() {
    dht.begin();
}

float dht_getTemperature() {
    return dht.readTemperature();
}

float dht_getHumidity() {
    return dht.readHumidity();
}
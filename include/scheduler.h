#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

extern float tempHistory[64];
extern float humidityHistory[64];
extern int historyIndex;
extern int validSamples;

void scheduler_run();

#endif
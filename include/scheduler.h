#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

extern float tempHistory[128];
extern float humidityHistory[128];
extern int historyIndex;
extern int validSamples;
extern float tempStep;
extern float humidityStep;

void scheduler_run();

#endif
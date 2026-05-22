#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

extern float tempHistory[120];
extern float humidityHistory[120];
extern int historyIndex;
extern int validSamples;
extern float tempStep;
extern float humidityStep;

void scheduler_run();

#endif
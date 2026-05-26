#ifndef APP_STATE_H
#define APP_STATE_H

enum uiScreen {
  SCREEN_MAIN,
  SCREEN_MINMAX,
  SCREEN_GRAPH_TEMP,
  SCREEN_GRAPH_HUM
};

struct AppData
{
    // Sensordaten
    float temp;
    float humidity;

    // Mittelwerte
    float meanTemp;
    float meanHumidity;

    // Trends
    float tempStep;
    float humidityStep;

    // Historie
    float tempHistory[128];
    float humidityHistory[128];

    int historyIndex;
    int validSamples;

    // UI
    uiScreen currentScreen;

    // Status
    bool sensorValid;
};

extern uiScreen currentScreen;
extern AppData app;

#endif // APP_STATE_H
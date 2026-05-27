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

    float minTemp;
    float maxTemp;
    float minHumidity;
    float maxHumidity;

    // Historie
    float tempHistory[128];
    float humidityHistory[128];
    int historyIndex;
    int validSamples;

    /* // Mittelwerte für history
    float meanTemp;
    float meanHumidity; */

    // Trends
    float tempStep; // Difference of mean after 10 measurements
    float humidityStep;

    bool sensorValid;    

    // UI
    uiScreen currentScreen;

};

extern AppData app;

extern uiScreen currentScreen;


#endif // APP_STATE_H


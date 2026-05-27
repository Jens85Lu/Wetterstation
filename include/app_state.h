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

    float minTemp = 999.0f;
    float maxTemp = -999.0f ;
    float minHumidity = 999.0f;
    float maxHumidity = -999.0f;

    // Historie
    float tempHistory[128];
    float humidityHistory[128];
    int historyIndex = -1;
    int validSamples;

    /* // Mittelwerte für history
    float meanTemp;
    float meanHumidity; */

    // Trends
    float tempStep; // Difference of mean after 10 measurements
    float humidityStep;

    bool sensorValid = false;   
    bool ledState; 

    // UI
    uiScreen currentScreen;

};

extern AppData app;

extern uiScreen currentScreen;


#endif // APP_STATE_H


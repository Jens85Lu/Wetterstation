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

    /* // Mittelwerte für history vielleicht später mal nötig
    float meanTemp;
    float meanHumidity; */

    bool sensorValid = false;   
    bool ledState; 

    // UI
    uiScreen currentScreen = SCREEN_MAIN;

};

extern AppData app;


#endif // APP_STATE_H


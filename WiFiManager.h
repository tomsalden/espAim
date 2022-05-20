#include "Arduino.h"

#include <WiFi.h>

class WiFiManager{
    public:
        void update(WiFiServer server);


        WiFiManager();
    private:
    unsigned long currentTime = millis();
    unsigned long previousTime = 0;
    const long timeoutTime = 2000;
    String header;
    String output26State = "off";
    String output27State = "off";
    const int output26 = 26;
    const int output27 = 27;
};
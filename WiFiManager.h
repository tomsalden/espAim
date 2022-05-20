#include "Arduino.h"
#include "SPIFFS.h"
#include "FS.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "credentials.h"

class WiFiManager{
    public:
        void setup();
        bool WiFiManagerBegin(const char *ap_ssid, const char *ap_pw);
        void setupWMWebpages();
        void setupWebPages();

        WiFiManager();

    private:
        void readCredentials(char *ssid, char *pw);
        void writeCredentials(const char *ssid, const char *pw);
        void clearWMCredentials();
        bool WiFiConnect(const char *ssid, const char *pw);
        int scanWifiNetworks();
        void listNetworks(int n);
};
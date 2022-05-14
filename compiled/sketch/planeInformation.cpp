#include "planeInformation.h"

extern void setDirectionAltitude(double curLat, double curLon, double curAlt, double newLat, double newLon, double newAlt);

planeInformation::planeInformation(){
}

void planeInformation::init(float curlat, float curlon){

    currentLocation.latitude = curlat;
    currentLocation.longitude = curlon;
    currentLocation.altitude = 0;

    areaMaxLat = 55.5006;
    areaMinLat = 49.6228;
    areaMaxLon = 7.6836;
    areaMinLon = 0.6271;

    updateURL();
}

void planeInformation::update(){
    Serial.println("Updating plane location");
    planeLocation.latitude = 52.05110410396898;
    planeLocation.longitude = 4.468350219726553;
    planeLocation.altitude = 1000;

    setDirectionAltitude(currentLocation.latitude, currentLocation.longitude, currentLocation.altitude, planeLocation.latitude, planeLocation.longitude, planeLocation.altitude);
}

void planeInformation::updateURL(){
    String Bounds = String(areaMaxLat) + "," + String(areaMinLat) + "," + String(areaMinLon) + "," + String(areaMaxLon);
    URL = "http://data-live.flightradar24.com/zones/fcgi/feed.js?bounds=" + Bounds + "&faa=1&satellite=1&mlat=1&flarm=1&adsb=1&gnd=1&air=1&vehicles=1&estimated=1&maxage=14400&gliders=1&stats=0";
}
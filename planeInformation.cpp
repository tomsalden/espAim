#include "planeInformation.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>


extern void setDirectionAltitude(double curLat, double curLon, double curAlt, double newLat, double newLon, double newAlt);

planeInformation::planeInformation(){
}

void planeInformation::init(double curlat, double curlon){

    curLat = curlat;
    curLon = curlon;
    curAlt = 0;

    areaMaxLat = 55.5006;
    areaMinLat = 49.6228;
    areaMaxLon = 7.6836;
    areaMinLon = 0.6271;

    planeLat = 52.05110410396898;
    planeLon = 4.468350219726553;
    planeAlt = 1000;

    updateURL();
    updateSec = 2;
    identifier = "2bdc3018";
}

void planeInformation::update(){
    static int updateTime = millis();
    if (millis() < (updateTime + updateSec*1000)){
        return;
    }
    Serial.println("Updating plane location");


    HTTPClient client;
    client.begin(URL);
    const auto httpResultCode = client.GET();

    if (httpResultCode != HTTP_CODE_OK)
    {
        client.end();
        Serial.print("Error received!! Code = ");
        Serial.println(httpResultCode);
        return;
    }
    
    auto response = client.getString();
    Serial.print("Message received! Retrieving plane data");
    //Serial.println(response);
    DynamicJsonDocument doc_flight_data(32*1024);
    StaticJsonDocument<64> filter;
    filter[identifier] = true;
    const auto error = deserializeJson(doc_flight_data, response, DeserializationOption::Filter(filter));
    if (error != DeserializationError::Ok)
    {
        client.end();
        Serial.print("Deserialise error! Code = ");
        Serial.println(error.c_str());
        return;
    }

    serializeJson(doc_flight_data, Serial);
    Serial.println(" ");

    client.end();

    ICAO_Addr = doc_flight_data[identifier][0];
    planeLat = doc_flight_data[identifier][1];
    planeLon = doc_flight_data[identifier][2];
    planeTrack = doc_flight_data[identifier][3];
    planeAlt = doc_flight_data[identifier][4];
    planeSpeed = doc_flight_data[identifier][5];
    //planeType = String(doc_flight_data[identifier][7]);
    //planeRegistration = String(doc_flight_data[identifier][8]);
    //Departure = 

    Serial.print("ICO Address: ");
    Serial.print(ICAO_Addr);
    Serial.print(", Latitude: ");
    Serial.print(planeLat);
    Serial.print(", Longitude: ");
    Serial.print(planeLon);

    Serial.print(", Altitude: ");
    Serial.print(planeAlt);
    Serial.print(", Speed: ");
    Serial.print(planeSpeed);


    setDirectionAltitude(curLat, curLon, curAlt, planeLat, planeLon, planeAlt);
    updateTime = millis();
}

void planeInformation::updateURL(){
    String Bounds = String(areaMaxLat) + "," + String(areaMinLat) + "," + String(areaMinLon) + "," + String(areaMaxLon);
    URL = "http://data-live.flightradar24.com/zones/fcgi/feed.js?bounds=" + Bounds + "&faa=1&satellite=1&mlat=1&flarm=1&adsb=1&gnd=1&air=1&vehicles=1&estimated=1&maxage=14400&gliders=1&stats=0";
}
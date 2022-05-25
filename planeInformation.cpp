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

    areaMaxLat = 55.5007;
    areaMinLat = 49.6228;
    areaMaxLon = 7.6836;
    areaMinLon = 0.6271;

    planeLat = 52.05110410396898;
    planeLon = 4.468350219726553;
    planeAlt = 1000;

    updateURL();
    updateSec = 2;
    identifier = "2bdc3018";
    configTime(0, 0, "pool.ntp.org");
    prevUpdate = millis();
}

void planeInformation::update(){
    static int updateTime = millis();

    if (millis() < (updateTime + updateSec*1000)){
        planeInterpolation();
        setDirectionAltitude(curLat, curLon, curAlt, planeLat, planeLon, planeAlt);
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
    Serial.println("Message received! Retrieving plane data. HTML code:");
    Serial.println(httpResultCode);
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
        Serial.println(response);
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
    updateEpoch = doc_flight_data[identifier][10];
    //planeType = String(doc_flight_data[identifier][7]);
    //planeRegistration = String(doc_flight_data[identifier][8]);
    //Departure = 


    Serial.print("Latitude: ");
    Serial.print(planeLat);
    Serial.print(", Longitude: ");
    Serial.print(planeLon);

    Serial.print(", Altitude: ");
    Serial.print(planeAlt);
    Serial.print(", Speed: ");
    Serial.print(planeSpeed);
    Serial.print(", Heading: ");
    Serial.print(planeTrack);
    Serial.println("");

    planeupdatedLat = planeLat;
    planeupdatedLon = planeLon;

    planeInterpolation();
    setDirectionAltitude(curLat, curLon, curAlt, planeLat, planeLon, planeAlt);

    updateTime = millis();
}

void planeInformation::planeInterpolation(){
    //Interpolate the speed and direction to find out where the plane is now

    //Get the current epoch and compare it to the updated time. Use this to get the distance travelled since updated values
    getTime();
    if (prevEpoch != currentEpoch){
        prevUpdate = millis();
    }
    prevEpoch = currentEpoch;
    float timeDifference = currentEpoch - updateEpoch + (millis()-prevUpdate)*1e-3;
    int distanceTravelled = planeSpeed * 0.51444 * timeDifference;
    // Serial.print("epoch: ");
    // Serial.println(currentEpoch);
    // Serial.print("Distance since last update: ");
    // Serial.println(distanceTravelled);

    //Now calculate the interpolated GPS coordinate from the heading, distance and old GPS coordinates

    //Set the updated lat and long to rads
    double updateLatRad = (planeupdatedLat * 71) / 4068;
    double updateLonRad = (planeupdatedLon * 71) / 4068;
    double headingRad = (planeTrack * 71) /4068;
    float Ad = distanceTravelled*1.5696E-7;

    //Calculate new coordinates in radians
    double newlatRad = asin(sin(updateLatRad)*cos(Ad) + cos(updateLatRad) * sin(Ad) * cos(headingRad));
    double newlonRad = updateLonRad + atan2(sin(headingRad) * sin(Ad) * cos(updateLatRad), cos(Ad) - sin(updateLatRad) * sin(newlatRad));

    //Convert back to degrees
    planeLat = (newlatRad * 4068) / 71;
    planeLon = (newlonRad * 4068) / 71;

    // Serial.print("NewLat : ");
    // Serial.print(planeLat, 6);
    // Serial.print(", NewLon: ");
    // Serial.println(planeLon, 6);
}

void planeInformation::getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return;
  }
  time(&now);
  currentEpoch = now;
  return;
}

void planeInformation::updateURL(){
    String Bounds = String(areaMaxLat) + "," + String(areaMinLat) + "," + String(areaMinLon) + "," + String(areaMaxLon);
    URL = "http://data-live.flightradar24.com/zones/fcgi/feed.js?bounds=" + Bounds + "&faa=1&satellite=1&mlat=1&flarm=1&adsb=1&gnd=1&air=1&vehicles=0&estimated=1&maxage=14400&gliders=0&stats=0";
}
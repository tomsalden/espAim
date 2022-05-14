#pragma once

#include "Arduino.h"

class planeInformation{
  public:
    float areaMinLat;
    float areaMaxLat;
    float areaMinLon;
    float areaMaxLon;

    String identifier;
    int updateSec;

    double curLat;
    double curLon;
    double curAlt;

    long ICAO_Addr;
    double planeLat;
    double planeLon;
    int planeTrack;
    double planeAlt;
    int planeSpeed;
    String planeType;
    String planeRegistration;
    String Departure;
    String Destination;
    String FlightNumber;
    String FlightNumber2;


    planeInformation();
    void init(double curlat, double curlon);
    void update();
    void updateURL();

  private:
    String URL;
};

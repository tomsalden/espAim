#pragma once

#include "Arduino.h"
#include "time.h"

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
    
    unsigned long updateEpoch;

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
    double planeupdatedLat;
    double planeupdatedLon;
    void planeInterpolation();
    unsigned long currentEpoch;
    unsigned long prevEpoch;
    void getTime();
    int prevUpdate;
};

/*
Information about the link:
To get all data in the Netherlands:
https://data-live.flightradar24.com/zones/fcgi/feed.js?bounds=55.5006,49.6228,0.6271,7.6836&faa=1&satellite=1&mlat=1&flarm=1&adsb=1&gnd=0&air=1&vehicles=0&estimated=1&maxage=14400&gliders=0&stats=0

flight = KL1847, filters for flight number, only displays fligth KL1847
type = B739, filters for airplane type, only displays planes with type B739
callsign = DHK2121, filters for callsign in url


*/
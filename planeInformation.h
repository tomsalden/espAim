#pragma once

#include "Arduino.h"

class planeInformation{
  public:
    float areaMinLat;
    float areaMaxLat;
    float areaMinLon;
    float areaMaxLon;

    char identifier;
    int updateSec;

    double planeLat;
    double planeLon;
    double planeAlt;

    double curLat;
    double curLon;
    double curAlt;

    planeInformation();
    void init(float curlat, float curlon);
    void update();
    void updateURL();

  private:
    String URL;
};

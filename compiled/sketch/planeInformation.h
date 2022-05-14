#include "Arduino.h"
#include "gpsClass.h"

class planeInformation{
  public:
    float areaMinLat;
    float areaMaxLat;
    float areaMinLon;
    float areaMaxLon;

    char identifier;
    int updateSec;

    gpsClass planeLocation;
    gpsClass currentLocation;

    planeInformation();
    void init(float curlat, float curlon);
    void update();
    void updateURL();

  private:
    String URL;
};

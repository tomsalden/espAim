#pragma once

#include "Arduino.h"
#include <ESP32Servo.h>

class trackingMotors{
  public:
    int servoPin;
    int Min;
    int Max;
    int Center;
    int Location;
    int NewLocation;
    
    bool objectFront;
    int millisperStep;

    trackingMotors();
    void init(int Pin, int maxLoc, int minLoc, int centerLoc);
    void update();

  private:
    Servo Servomotor;
    void initMoving();
};

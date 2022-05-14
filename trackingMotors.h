#include "Arduino.h"
#include <ESP32Servo.h>

class trackingMotors{
  public:
    int directionMin;
    int directionMax;
    int directionCenter;
    int directionLocation;
    int directionNewLocation;

    int altitudeMin;
    int altitudeMax;
    int altitudeCenter;
    int altitudeLocation;
    int altitudeNewLocation;
    
    bool objectFront;
    int millisperStep;

    trackingMotors();
    void init();
    void print();
    void update();

    Servo directionServo;
    Servo altitudeServo;
    

  private:

    void initMoving(Servo movedServo, int min, int max);
};

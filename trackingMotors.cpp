#include "trackingMotors.h"

trackingMotors::trackingMotors(){

}

void trackingMotors::init(int Pin, int maxLoc, int minLoc, int centerLoc){
    millisperStep = 5;

    servoPin = Pin;
    Min = minLoc;
    Max = maxLoc;
    Center = centerLoc;

    Servomotor.setPeriodHertz(50);
    Servomotor.attach(servoPin, Min, Max);

    Location = Center;
    NewLocation = Center;

    delay(10);

    Servomotor.write(Center);
    delay(1000);

    //Move servos to their extremes for startup
    initMoving();
    Serial.println("Initialisation is done!");
}

void trackingMotors::initMoving(){
    float initPos = 0.5;
    Serial.println("Moving towards one extreme");
    while (1)
    {
        initPos = initPos + 0.005;
        if (initPos > 1)
        {
            break;
        }
        Servomotor.write(Min + (Max-Min)*initPos);
        delay(10);
    }
    Serial.println("Moving to the other extreme");
    while (1)
    {
        initPos = initPos - 0.005;
        if (initPos < 0)
        {
            break;
        }
        Servomotor.write(Min + (Max-Min)*initPos);
        delay(10);
    }
    Serial.println("Moving back to the center");
    while (1)
    {
        initPos = initPos + 0.005;
        if (initPos > 0.5)
        {
            break;
        }
        Servomotor.write(Min + (Max-Min)*initPos);
        delay(10);
    }
    delay(1000);
}

void trackingMotors::update(){
    static int servoTime = millis();
    if (millis() < (servoTime + millisperStep)){
        return;
    }
    if (NewLocation == Location){
        return;
    }
    if (NewLocation > Location){
        Location++;
    }
    if (NewLocation < Location){
        Location--;
    }
    Servomotor.write(Location);
    servoTime = millis();
    return;
}

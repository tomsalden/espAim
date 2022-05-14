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

// void trackingMotors::update(){
//     static int servoTime = millis();
    
//     //Don't do anything if it is too early
//     if (millis() < (servoTime + millisperStep)){
//         return;
//     }

//     //directionServo.attach(DIRECTION_PIN, directionMin, directionMax);
//     //altitudeServo.attach(ALTITUDE_PIN, altitudeMin, altitudeMax);

//     bool directionUpdate = false;
//     bool altitudeUpdate = false;

//     //If the new location is bigger than the current location, increase the current location
//     if (directionNewLocation > directionLocation){
//         directionLocation++;
//         directionUpdate = true;
//     }

//     //If the new loation is smaller than the current location, decrease the current location
//     if (directionNewLocation < directionLocation){
//         directionLocation--;
//         directionUpdate = true;
//     }

//     //If the new location is bigger than the current location, increase the current location
//     if ((altitudeNewLocation > altitudeLocation) && (directionUpdate == false)){
//         altitudeLocation++;
//         altitudeUpdate = true;
//     }

//     //If the new loation is smaller than the current location, decrease the current location
//     if ((altitudeNewLocation < altitudeLocation) && (directionUpdate == false)){
//         altitudeLocation--;
//         altitudeUpdate = true;
//     }

//     //Update direction if necessary
//     if (directionUpdate == true){
//         Serial.print("Direction updated: ");
//         Serial.println(directionLocation);
//         directionServo.write(directionLocation);
//         delay(20);
//     }

//     if (altitudeUpdate == true){
//         Serial.print("Altitude updated: ");
//         Serial.println(altitudeLocation);
//         altitudeServo.write(altitudeLocation);
//         delay(20);
//     }

//     Serial.print("Servo Status");
//     Serial.println(directionServo.attached());
//     Serial.println(servoTime);

//     servoTime = millis();
//     return;
// }


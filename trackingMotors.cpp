#include "trackingMotors.h"

#define DIRECTION_PIN 13
#define ALTITUDE_PIN 12

trackingMotors::trackingMotors(){

}

void trackingMotors::init(){
    millisperStep = 5;

    directionMin = 500;
    directionMax = 2500;
    directionCenter = (directionMax + directionMin) / 2;
    
    altitudeMin = 500;
    altitudeMax = 2500;
    altitudeCenter = (altitudeMax + altitudeMin) / 2;

    directionServo.setPeriodHertz(50);
    altitudeServo.setPeriodHertz(50);

    directionServo.attach(DIRECTION_PIN, directionMin, directionMax);
    altitudeServo.attach(ALTITUDE_PIN, altitudeMin, altitudeMax);

    directionLocation = directionCenter;
    directionNewLocation = directionCenter;

    altitudeLocation = altitudeCenter;
    altitudeNewLocation = altitudeCenter;

    delay(10);

    directionServo.write(directionCenter);
    delay(1000);
    altitudeServo.write(altitudeCenter);

    delay(1000);

    //Move servos to their extremes for startup
    initMoving(directionServo,directionMin, directionMax);
    initMoving(altitudeServo, altitudeMin, altitudeMax);
    Serial.println("Initialisation is done!");
}

void trackingMotors::initMoving(Servo movedServo, int min, int max){
    float initPos = 0.5;
    Serial.println("Moving towards one extreme");
    while (1)
    {
        initPos = initPos + 0.005;
        if (initPos > 1)
        {
            break;
        }
        movedServo.write(min + (max-min)*initPos);
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
        movedServo.write(min + (max-min)*initPos);
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
        movedServo.write(min + (max-min)*initPos);
        delay(10);
    }
    delay(1000);
}

void trackingMotors::update(){
    static int servoTime = millis();
    if (millis() < (servoTime + millisperStep)){
        return;
    }
    if (directionNewLocation == directionLocation){
        return;
    }
    if (directionNewLocation > directionLocation){
        directionLocation++;
    }
    if (directionNewLocation < directionLocation){
        directionLocation--;
    }
    directionServo.write(directionLocation);
    Serial.print("CurrentPosition: ");
    Serial.print(directionLocation);
    Serial.print(", NextPosition: ");
    Serial.print(directionNewLocation);
    Serial.print(", time: ");
    Serial.println(millis());
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


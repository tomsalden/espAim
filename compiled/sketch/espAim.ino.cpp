#include <Arduino.h>
#line 1 "e:\\Overig\\espAim\\espAim.ino"

#include "trackingMotors.h"

trackingMotors trackingServos; 

struct GPSLocation {
    float latitude;
    float longitude;
    float altitude;
};

int servoTime;

#line 14 "e:\\Overig\\espAim\\espAim.ino"
void setup();
#line 39 "e:\\Overig\\espAim\\espAim.ino"
float deg2rad(float deg);
#line 42 "e:\\Overig\\espAim\\espAim.ino"
float rad2deg(float rad);
#line 46 "e:\\Overig\\espAim\\espAim.ino"
float heading(GPSLocation curLoc, GPSLocation newLoc);
#line 61 "e:\\Overig\\espAim\\espAim.ino"
void setDirection(GPSLocation curLoc, GPSLocation newLoc);
#line 87 "e:\\Overig\\espAim\\espAim.ino"
void ServoUpdate();
#line 146 "e:\\Overig\\espAim\\espAim.ino"
void loop();
#line 14 "e:\\Overig\\espAim\\espAim.ino"
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  
  Serial.println("Initialising motors");
  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
  trackingServos.init();


  GPSLocation currentLocation = {51.997842, 4.374279, 0};
  GPSLocation planeLocation = {52.05110410396898, 4.468350219726553, 1000};

    
  setDirection(currentLocation, planeLocation);
  Serial.print("Servo set to number:");
  Serial.println(trackingServos.directionNewLocation);
  trackingServos.altitudeNewLocation = 1995;

  int servoTime = millis();
}

float deg2rad(float deg) {
  return (deg * 71) / 4068;
}
float rad2deg(float rad) {
  return (rad * 4068) / 71;
}

float heading(GPSLocation curLoc, GPSLocation newLoc) { //Function to determine the heading of the object
  //Convert all coordinates to radians
  float lat1 = deg2rad(curLoc.latitude);
  float lon1 = deg2rad(curLoc.longitude);
  float lat2 = deg2rad(newLoc.latitude);
  float lon2 = deg2rad(newLoc.longitude);

  float x = cos(lat2) * sin(lon2-lon1);
  float y = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(lon2-lon1);
  float angle = rad2deg(atan2(x,y));
  Serial.print("Heading: ");
  Serial.println(angle);
  return angle; //Angle relative to North
}

void setDirection(GPSLocation curLoc, GPSLocation newLoc){
  //Determine heading
  float direction = heading(curLoc, newLoc);

  //Figure out if it is to the front or to the back
  if (abs(direction) < 90){
    Serial.println("Heading is in front of the device");
    trackingServos.objectFront = true;
    trackingServos.directionNewLocation = map(direction, -90, 90, trackingServos.directionMin, trackingServos.directionMax);
    return;
  }

  Serial.println("Heading is behind the device");
  trackingServos.objectFront = false;

  if(direction > 0){
      Serial.println("Heading is to the right");
      trackingServos.directionNewLocation = map(direction, 91, 180, trackingServos.directionCenter, trackingServos.directionMin);
      return;
  }

  Serial.println("Heading is to the left");
  trackingServos.directionNewLocation = map(direction, -91, -180, trackingServos.directionCenter, trackingServos.directionMax);
  return;
}

void ServoUpdate(){
    //Don't do anything if it is too early
    if (millis() < (servoTime + trackingServos.millisperStep)){
        return;
    }

    //directionServo.attach(DIRECTION_PIN, directionMin, directionMax);
    //altitudeServo.attach(ALTITUDE_PIN, altitudeMin, altitudeMax);

    bool directionUpdate = false;
    bool altitudeUpdate = false;

    //If the new location is bigger than the current location, increase the current location
    if (trackingServos.directionNewLocation > trackingServos.directionLocation){
        trackingServos.directionLocation++;
        directionUpdate = true;
    }

    //If the new loation is smaller than the current location, decrease the current location
    if (trackingServos.directionNewLocation < trackingServos.directionLocation){
        trackingServos.directionLocation--;
        directionUpdate = true;
    }

    //If the new location is bigger than the current location, increase the current location
    if ((trackingServos.altitudeNewLocation > trackingServos.altitudeLocation) && (directionUpdate == false)){
        trackingServos.altitudeLocation++;
        altitudeUpdate = true;
    }

    //If the new loation is smaller than the current location, decrease the current location
    if ((trackingServos.altitudeNewLocation < trackingServos.altitudeLocation) && (directionUpdate == false)){
        trackingServos.altitudeLocation--;
        altitudeUpdate = true;
    }

    //Update direction if necessary
    if (directionUpdate == true){
        Serial.print("Direction updated: ");
        Serial.println(trackingServos.directionLocation);
        trackingServos.directionServo.write(trackingServos.directionLocation);
        delay(20);
    }

    if (altitudeUpdate == true){
        Serial.print("Altitude updated: ");
        Serial.println(trackingServos.altitudeLocation);
        trackingServos.altitudeServo.write(trackingServos.altitudeLocation);
        delay(20);
    }

    Serial.print("Servo Status");
    Serial.println(trackingServos.directionServo.attached());
    Serial.println(servoTime);

    servoTime = millis();
    return;
}

void loop() {
  // put your main code here, to run repeatedly:
  ServoUpdate();
  delay(10);

  //URL for Planes in NL: https://data-live.flightradar24.com/zones/fcgi/feed.js?bounds=55.5006,49.6228,0.6271,7.6836&faa=1&satellite=1&mlat=1&flarm=1&adsb=1&gnd=1&air=1&vehicles=1&estimated=1&maxage=14400&gliders=1&stats=0
}


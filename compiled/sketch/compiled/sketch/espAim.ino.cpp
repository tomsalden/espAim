#include <Arduino.h>
#line 1 "e:\\Overig\\espAim\\espAim.ino"

#include "trackingMotors.h"

trackingMotors trackingDirection; 
trackingMotors trackingAltitude;

struct GPSLocation {
    float latitude;
    float longitude;
    float altitude;
};

int servoTime;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  
  Serial.println("Initialising motors");
  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);

  trackingDirection.init(13, 2500, 500, 1500);
  trackingAltitude.init(12, 2500, 500, 1500);


  GPSLocation currentLocation = {51.997842, 4.374279, 0};
  GPSLocation planeLocation = {52.05110410396898, 4.468350219726553, 1000};

    
  setDirection(currentLocation, planeLocation);
  Serial.print("Servo set to number:");
  Serial.println(trackingDirection.NewLocation);
  trackingAltitude.NewLocation = 1995;
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
    trackingDirection.objectFront = true;
    trackingDirection.NewLocation = map(direction, -90, 90, trackingDirection.Min, trackingDirection.Max);
    return;
  }

  Serial.println("Heading is behind the device");
  trackingDirection.objectFront = false;

  if(direction > 0){
      Serial.println("Heading is to the right");
      trackingDirection.NewLocation = map(direction, 91, 180, trackingDirection.Center, trackingDirection.Min);
      return;
  }

  Serial.println("Heading is to the left");
  trackingDirection.NewLocation = map(direction, -91, -180, trackingDirection.Center, trackingDirection.Max);
  return;
}



void loop() {
  // put your main code here, to run repeatedly:
  trackingDirection.update();
  trackingAltitude.update();
  delay(10);

  //URL for Planes in NL: https://data-live.flightradar24.com/zones/fcgi/feed.js?bounds=55.5006,49.6228,0.6271,7.6836&faa=1&satellite=1&mlat=1&flarm=1&adsb=1&gnd=1&air=1&vehicles=1&estimated=1&maxage=14400&gliders=1&stats=0
}


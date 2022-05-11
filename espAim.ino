#include <ESP32Servo.h>

Servo headingservo;
int servoPin = 13;
int servoMin = 500;
int servoMax = 2500;
int servoCenter = (servoMax + servoMin) / 2;

struct GPSLocation {
    float latitude;
    float longitude;
    float altitude;
};

struct trackerServoData {
    int directionPin;
    int directionMin;
    int directionMax;
    int directionCenter;
    int directionLocation;
    int altitudePin;
    int altitudeMin;
    int altitudeMax;
    int altitudeCenter;
    int altitudeLocation;
    bool objectFront;
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  GPSLocation currentLocation = {51.997842, 4.374279, 0};
  GPSLocation planeLocation = {52.05110410396898, 4.468350219726553, 1000};

  trackerServoData servoData = {servoPin, servoMin, servoMax, servoCenter, servoCenter, 0, 0, 0, 0, 0, true};

  headingservo.setPeriodHertz(50);
  headingservo.attach(servoPin, servoMin, servoMax);
  headingservo.write(servoMin);
  delay(1000);
  headingservo.write(servoMax);
  delay(1000);  
  servoData = setDirection(servoData, currentLocation, planeLocation);
  Serial.print("Servo set to number:");
  Serial.println(servoData.directionLocation);
  headingservo.write(servoData.directionLocation);
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

trackerServoData setDirection(trackerServoData servoData, GPSLocation curLoc, GPSLocation newLoc){
  //Determine heading
  float direction = heading(curLoc, newLoc);

  //Figure out if it is to the front or to the back
  if (abs(direction) < 90){
    Serial.println("Heading is in front of the device");
    servoData.objectFront = true;
    servoData.directionLocation = map(direction, -90, 90, servoData.directionMin, servoData.directionMax);
    return servoData;
  }

  Serial.println("Heading is behind the device");
  servoData.objectFront = false;

  if(direction > 0){
      Serial.println("Heading is to the right");
      servoData.directionLocation = map(direction, 91, 180, servoData.directionCenter, servoData.directionMin);
      return servoData;
  }

  Serial.println("Heading is to the left");
  servoData.directionLocation = map(direction, -91, -180, servoData.directionCenter, servoData.directionMax);
  return servoData;
}
void loop() {
  // put your main code here, to run repeatedly:
  delay(10); // this speeds up the simulation
}

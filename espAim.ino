#include <ESP32Servo.h>

Servo headingservo;
int servoPin = 13;
int servoMin = 500;
int servoMax = 2500;
int servoCenter = (servoMax + servoMin) / 2;
int servoTime;

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
    int directionNewLocation;
    int altitudePin;
    int altitudeMin;
    int altitudeMax;
    int altitudeCenter;
    int altitudeLocation;
    int altitudeNewLocation;
    bool objectFront;
    int millisperStep;
};

trackerServoData servoData = {servoPin, servoMin, servoMax, servoCenter, servoCenter, servoCenter, 0, 0, 0, 0, 0, 0, true, 5};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  GPSLocation currentLocation = {51.997842, 4.374279, 0};
  GPSLocation planeLocation = {52.05110410396898, 4.468350219726553, 1000};

  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
  headingservo.setPeriodHertz(50);
  headingservo.attach(servoPin, servoMin, servoMax);

  // Move to extremes (slowly)
  Serial.println("StartingServos");
  headingservo.write(servoCenter);
  delay(1000);
  moveServosInit();
    
  setDirection(currentLocation, planeLocation);
  Serial.print("Servo set to number:");
  Serial.println(servoData.directionNewLocation);
  delay(10);
  servoTime = millis();
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
    servoData.objectFront = true;
    servoData.directionNewLocation = map(direction, -90, 90, servoData.directionMin, servoData.directionMax);
    return;
  }

  Serial.println("Heading is behind the device");
  servoData.objectFront = false;

  if(direction > 0){
      Serial.println("Heading is to the right");
      servoData.directionNewLocation = map(direction, 91, 180, servoData.directionCenter, servoData.directionMin);
      return;
  }

  Serial.println("Heading is to the left");
  servoData.directionNewLocation = map(direction, -91, -180, servoData.directionCenter, servoData.directionMax);
  return;
}

void moveServosInit(){
  int posHeading = servoCenter;
  while (1){
    posHeading = posHeading + 10;
    if (posHeading > servoMax){
      break;
    }
    headingservo.write(posHeading);
    delay(10);
  }

  while (1){
    posHeading = posHeading - 10;
    if (posHeading < servoMin){
      break;
    }
    headingservo.write(posHeading);
    delay(10);
  }

  while(1){
    posHeading = posHeading + 10;
    if (posHeading > servoCenter){
      break;
    }
    headingservo.write(posHeading);
    delay(10);
  }
}

void moveServosStep(){
  if (millis() < (servoTime + servoData.millisperStep)){
    return;
  }
  if (servoData.directionNewLocation == servoData.directionLocation){
    return;
  }
  if (servoData.directionNewLocation > servoData.directionLocation){
    servoData.directionLocation++;
  }
  if (servoData.directionNewLocation < servoData.directionLocation){
    servoData.directionLocation--;
  }
  headingservo.write(servoData.directionLocation);
  Serial.print("CurrentPosition: ");
  Serial.print(servoData.directionLocation);
  Serial.print(", NextPosition: ");
  Serial.print(servoData.directionNewLocation);
  Serial.print(", time: ");
  Serial.println(millis());
  servoTime = millis();
  return;
}
void loop() {
  // put your main code here, to run repeatedly:
  delay(10); // this speeds up the simulation
  moveServosStep();

  //URL for Planes in NL: https://data-live.flightradar24.com/zones/fcgi/feed.js?bounds=55.5006,49.6228,0.6271,7.6836&faa=1&satellite=1&mlat=1&flarm=1&adsb=1&gnd=1&air=1&vehicles=1&estimated=1&maxage=14400&gliders=1&stats=0
}

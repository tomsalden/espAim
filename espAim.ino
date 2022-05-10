#include <ESP32Servo.h>

Servo headingservo;
int servoPin = 13;
int servoMin = 500;
int servoMax = 2500;
int servoCenter = (servoMax + servoMin) / 2;
bool objectFront = true

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  
  float lat1 = 51.997842;
  float lon1 = 4.374279;

  float lat2 = 52.05110410396898;
  float lon2 = 4.468350219726553;

  headingservo.setPeriodHertz(50);
  headingservo.attach(servoPin, servoMin, servoMax);
  headingservo.write(servoMin);
  delay(1000);
  headingservo.write(servoMax);
  delay(1000);  
  //Serial.println(heading(lat1,lon1,lat2,lon2));
  setServo(headingservo, lat1, lon1, lat2, lon2);
}

float deg2rad(float deg) {
  return (deg * 71) / 4068;
}
float rad2deg(float rad) {
  return (rad * 4068) / 71;
}

float heading(float lat1,float lon1,float lat2,float lon2) { //Function to determine the heading of the object
  //Convert all coordinates to radians
  lat1 = deg2rad(lat1);
  lon1 = deg2rad(lon1);
  lat2 = deg2rad(lat2);
  lon2 = deg2rad(lon2);

  float x = cos(lat2) * sin(lon2-lon1);
  float y = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(lon2-lon1);
  float angle = rad2deg(atan2(x,y));
  Serial.print("Heading: ");
  Serial.println(angle);
  return angle; //Angle relative to North
}

void setServo(Servo directionservo, float lat1,float lon1,float lat2,float lon2){
  //Determine heading
  float direction = heading(lat1,lon1,lat2,lon2);
  int serialPosition = 1000;

  //Figure out if it is to the front or to the back
  if (abs(direction) < 90){
    Serial.println("Heading is in front of the device");
    objectFront = true;
    serialPosition = map(direction, -90, 90, servoMin, servoMax);
  }
  else{
    Serial.println("Heading is behind the device");
    objectFront = false;
    if(direction > 0){
      Serial.println("Heading is to the right");
      serialPosition = map(direction, 91, 180, servoCenter, servoMin);
    }
    else{
      Serial.println("Heading is to the left");
      serialPosition = map(direction, -91, -180, servoCenter, servoMax);
    }
  }
  Serial.print("Servo set to number:");
  Serial.println(serialPosition);
  directionservo.write(serialPosition);
  delay(1000);
}
void loop() {
  // put your main code here, to run repeatedly:
  delay(10); // this speeds up the simulation
}

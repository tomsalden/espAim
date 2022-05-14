# 1 "e:\\Overig\\espAim\\espAim.ino"
# 2 "e:\\Overig\\espAim\\espAim.ino" 2
# 3 "e:\\Overig\\espAim\\espAim.ino" 2


trackingMotors trackingDirection;
trackingMotors trackingAltitude;
planeInformation plane;

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

  plane.init(51.997842,4.374279);


  //setDirectionAltitude(currentLocation, planeLocation);
  Serial.print("Servo set to number:");
  Serial.println(trackingDirection.NewLocation);
  trackingAltitude.NewLocation = 1995;
}


void loop() {
  // put your main code here, to run repeatedly:
  plane.update();

  trackingDirection.update();
  trackingAltitude.update();
  delay(10);

  //URL for Planes in NL: https://data-live.flightradar24.com/zones/fcgi/feed.js?bounds=55.5006,49.6228,0.6271,7.6836&faa=1&satellite=1&mlat=1&flarm=1&adsb=1&gnd=1&air=1&vehicles=1&estimated=1&maxage=14400&gliders=1&stats=0
}


//Some general functions (radians to degrees and back)
float deg2rad(float deg) {
  return (deg * 71) / 4068;
}
float rad2deg(float rad) {
  return (rad * 4068) / 71;
}

//Function to get a heading from 2 GPS coordinates
float getHeading(GPSLocation curLoc, GPSLocation newLoc) {
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

//Function to set the direction and altitude of the servos
void setDirectionAltitude(double curLat, double curLon, double curAlt, double newLat, double newLon, double newAlt){
  //Create GPS struct
  GPSLocation curLoc = {curLat, curLon, curAlt};
  GPSLocation newLoc = {newLat, newLon, newAlt};

  //Determine heading
  float direction = getHeading(curLoc, newLoc);

  //Set the altitude, irrespective of if it is in the front or the back (from 0 to 90 degrees)
  // Need distance...

  //Figure out if it is to the front or to the back
  if (abs(direction) < 90){
    Serial.println("Heading is in front of the device");
    trackingAltitude.objectFront = true;
    trackingDirection.NewLocation = map(direction, -90, 90, trackingDirection.Min, trackingDirection.Max);
    return;
  }

  Serial.println("Heading is behind the device");
  trackingAltitude.objectFront = false;

  if(direction > 0){
      Serial.println("Heading is to the right");
      trackingDirection.NewLocation = map(direction, 91, 180, trackingDirection.Center, trackingDirection.Min);
      return;
  }

  Serial.println("Heading is to the left");
  trackingDirection.NewLocation = map(direction, -91, -180, trackingDirection.Center, trackingDirection.Max);
  return;
}

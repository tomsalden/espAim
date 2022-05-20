#include "trackingMotors.h"
#include "planeInformation.h"
#include "WiFiManager.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include "credentials.h"


//#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include "FS.h"

#ifndef CREDENTIALS_H
#pragma error "!!!!!!!!"
#pragma error "PLEASE COPY credentials.h.template TO credentials.h"
#pragma error "AND CONFIGURE YOUR CREDENTIALS"
#pragma error "!!!!!!!!"
#endif

#define WM_CRED_FILE "/wm_cred.dat"

trackingMotors trackingDirection; 
trackingMotors trackingAltitude;
planeInformation plane;
WiFiManager WiFiMgr;

struct GPSLocation {
    float latitude;
    float longitude;
    float altitude;
};

int servoTime;
#define PAR_FILE "/par.dat"

//Define webserver on port 80
AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting espAim!");

  delay(2000);
  
  Serial.println("Initialising motors");
  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);

  trackingDirection.init(13, 2500, 500, 1500);
  trackingAltitude.init(12, 2500, 500, 1500);

  plane.init(51.997842,4.374279);

  if (!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    delay(1000);
    ESP.restart();
  }

  listDir(SPIFFS, "/", 1);

  WiFi.hostname(HOSTNAME);

  Serial.println("Setting up WiFi");
  //WiFi.encryptionType(3);
  //WiFi.begin(AP_SSID,AP_PASS);
  
  if (!WiFiMgr.WiFiManagerBegin(AP_SSID, AP_PASS)){
    // setup wifimanager config portal
    WiFiMgr.setupWMWebpages();
  }
  WiFiMgr.setupWebPages();

  server.onNotFound(notFound);
  Serial.println("starting server");
  server.begin();

  

  trackingAltitude.NewLocation = 1995;

  Serial.print("Connected with ip: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting Webserver");



}


void loop() {
  // put your main code here, to run repeatedly:
  plane.update();
  
  trackingDirection.update();
  trackingAltitude.update();
  delay(10);

  if (Serial.available() > 0){
    String identifier = Serial.readString();
    identifier.trim();
    plane.identifier = identifier;
    Serial.println(plane.identifier);
    
  }


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

  //Figure out if the direction is left or right
  if (direction > 0){
    Serial.println("Heading is to the right");
    trackingAltitude.objectFront = true;
    trackingDirection.NewLocation = map(direction, 0, 180, trackingDirection.Max, trackingDirection.Min);
    return;
  }

  Serial.println("Heading is behind the device");
  trackingAltitude.objectFront = false;
  trackingDirection.NewLocation = map(direction, 0, -180, trackingDirection.Min, trackingDirection.Max);

  // if(direction > 0){
  //     Serial.println("Heading is to the right");
  //     trackingDirection.NewLocation = map(direction, 91, 180, trackingDirection.Center, trackingDirection.Min);
  //     return;
  // }

  // Serial.println("Heading is to the left");
  // trackingDirection.NewLocation = map(direction, -91, -180, trackingDirection.Center, trackingDirection.Max);
  return;
}

// void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
//   Serial.printf("Listing directory: %s\r\n", dirname);

//   File root = fs.open(dirname);
//   if (!root) {
//     Serial.println("- failed to open directory");
//     return;
//   }
//   if (!root.isDirectory()) {
//     Serial.println(" - not a directory");
//     return;
//   }

//   File file = root.openNextFile();
//   while (file) {
//     if (file.isDirectory()) {
//       Serial.print("  DIR : ");
//       Serial.println(file.name());
//       if (levels) {
//         listDir(fs, file.name(), levels - 1);
//       }
//     } else {
//       Serial.print("  FILE: ");
//       Serial.print(file.name());
//       Serial.print("\tSIZE: ");
//       Serial.println(file.size());
//     }
//     file = root.openNextFile();
//   }
// }

void getParam(char* p) {
  Serial.printf("Reading file: %s\r\n", PAR_FILE);
  File file = SPIFFS.open(PAR_FILE, "r");
  if (!file || file.isDirectory())
  {
    Serial.println("ERROR: failed to open file for reading");
    int a = 4;
    //p[0] = rgb[0]; p[1] = rgb[1]; p[2] = rgb[2]; p[3] = brightness;
    return;
  }
  uint8_t i = 0;
  while (file.available()) {
    p[i] = file.read();
    i++;
  }
  file.close();
}

void saveParam() {
  Serial.printf("Writing to file: %s\r\n", PAR_FILE);
  File file = SPIFFS.open(PAR_FILE, "w");
  if (!file)
  {
    Serial.println("ERROR: failed to open file for writing");
    return;
  }
  if ((true == true)
//    file.write(rgb[0]) &&
//    file.write(rgb[1]) &&
//    file.write(rgb[2]) &&
//    file.write(brightness) &&
//    file.write(fxState) &&
//    file.write(fxIndex)
  ) Serial.println("- file written");
  else Serial.println("- write failed");
  file.close();
}

void readCredentials(char *ssid, char *pw)
{
  Serial.printf("Reading file: %s\r\n", WM_CRED_FILE);

  File file = SPIFFS.open(WM_CRED_FILE, "r");
  if (!file || file.isDirectory())
  {
    Serial.println("ERROR: failed to open file for reading");
    return;
  }

  uint8_t row = 0;
  uint8_t col = 0;
  while (file.available())
  {
    char c = file.read();
    //        Serial.write(c);
    if (c != '\n')
    {
      if (row == 0)
        ssid[col] = c;
      else
        pw[col] = c;
      col++;
    }
    else
    {
      col = 0;
      row++;
    }
  }
  file.close();
}
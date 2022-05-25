#include <Arduino.h>
#line 1 "e:\\Overig\\espAim\\espAim.ino"
#include "trackingMotors.h"
#include "planeInformation.h"
#include "WiFiManager.h"

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <HTTPClient.h>
#include "credentials.h"

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

struct GPSLocation {
    float latitude;
    float longitude;
    float altitude;
};

int servoTime;
bool motorInitialisation = false;
float altangle;
float dirangle;

const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>espAim</h2>
  <h3>The aiming arduino</h3><br>
  <h3>Current flight information</h3><br>
  <h4>Plane ID:<span id="IDPlane">0</span> </h4> <h4><label class="textfield"><input type="text" id="identifier" onchange="sendInformation(this)"</label></h4><br>
  <h4>Latitude: <span id="LatPlane">0</span> </h4>
  <h4>Longitude: <span id="LonPlane">0</span> </h4>
  <h4>Altitude: <span id="AltPlane">0</span> </h4>
  <h4>Speed: <span id="SpdPlane">0</span> </h4>
  <h4>Heading: <span id="HeadPlane">0</span> </h4><br>
  <h4>Direction angle from Servo: <span id="AngleDirServo">0</span> </h4>
  <h4>Altitude angle from Servo: <span id="AngleAltServo">0</span> </h4>
  <h5>This ESP32 is currently aiming at planes. Later on, other implementations can be added as wel, such as planets/stars, the ISS, satelites or other things :)</h5>
  %BUTTONPLACEHOLDER%
<script>
    function sendInformation(element){
        const planeID = document.getElementById("identifier")
        console.log("Plane id: " + element.value);
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/update?output="+element.id+"&state="+element.value, true);
        xhr.send();
    }

function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
  xhr.send();
}

setInterval(function() {
  // Call a function repetatively with 2 Second interval
  getIdentifier();
  getLatitude();
  getLongitude();
  getAltitude();
  getSpeed();
  getHeading();
  getdirAngle();
  getaltAngle();
}, 2000); //2000mSeconds update rate

function getIdentifier() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("IDPlane").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "readID", true);
  xhttp.send();
}

function getLatitude() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LatPlane").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "readLat", true);
  xhttp.send();
}

function getLongitude() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LonPlane").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "readLon", true);
  xhttp.send();
}

function getAltitude() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("AltPlane").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "readAlt", true);
  xhttp.send();
}

function getSpeed() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("SpdPlane").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "readSpeed", true);
  xhttp.send();
}

function getHeading() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("HeadPlane").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "readHead", true);
  xhttp.send();
}

function getdirAngle() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("AngleDirServo").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "readDirAngle", true);
  xhttp.send();
}

function getaltAngle() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("AngleAltServo").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "readAltAngle", true);
  xhttp.send();
}
</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = "";
    buttons += "<h4>Initialise Motors</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"2\" " + outputState(2) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Output - GPIO 4</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"4\" " + outputState(4) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Output - GPIO 33</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"33\" " + outputState(33) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}

String outputState(int output){
  if(digitalRead(output)){
    return "checked";
  }
  else {
    return "";
  }
}

void handleADC() {

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

  plane.init(52.197136,4.754039);

  WiFi.hostname(HOSTNAME);

  Serial.println("Setting up WiFi");
  WiFi.encryptionType(3);
  WiFi.begin(AP_SSID,AP_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected with ip: ");
  Serial.println(WiFi.localIP());

  Serial.println("starting server");
    // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/readID", HTTP_GET, [](AsyncWebServerRequest *request){
    String info = String(plane.identifier);
     request->send(200, "text/plane", info); //Send ADC value only to client ajax request
  });

  server.on("/readLat", HTTP_GET, [](AsyncWebServerRequest *request){
    String info = String(plane.planeLat);
     request->send(200, "text/plane", info); //Send ADC value only to client ajax request
  });
  
  server.on("/readLon", HTTP_GET, [](AsyncWebServerRequest *request){
    String info = String(plane.planeLon);
     request->send(200, "text/plane", info); //Send ADC value only to client ajax request
  });

  server.on("/readAlt", HTTP_GET, [](AsyncWebServerRequest *request){
    String info = String(plane.planeAlt);
     request->send(200, "text/plane", info); //Send ADC value only to client ajax request
  });

  server.on("/readSpeed", HTTP_GET, [](AsyncWebServerRequest *request){
    String info = String(plane.planeSpeed);
     request->send(200, "text/plane", info); //Send ADC value only to client ajax request
  });
  
  server.on("/readHead", HTTP_GET, [](AsyncWebServerRequest *request){
    String info = String("Unknown yet");
     request->send(200, "text/plane", info); //Send ADC value only to client ajax request
  });

  server.on("/readDirAngle", HTTP_GET, [](AsyncWebServerRequest *request){
    String info = String(dirangle);
     request->send(200, "text/plane", info); //Send ADC value only to client ajax request
  });

  server.on("/readAltAngle", HTTP_GET, [](AsyncWebServerRequest *request){
    String info = String(altangle);
     request->send(200, "text/plane", info); //Send ADC value only to client ajax request
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage1;
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();

      Serial.print("Recieved message. Message1: ");
      Serial.print(inputMessage1);
      Serial.print(". Message 2: ");
      Serial.println(inputMessage2);
      
      if(inputMessage1 == "identifier"){
        plane.identifier = inputMessage2;
        Serial.println("Updated plane!");
      }
      // if(inputMessage1 == "2"){
      //   //Initialise the motors
      //   motorInitialisation = true;
      // }
      else{
      digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
      }
    }
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }
    Serial.print("GPIO: ");
    Serial.print(inputMessage1);
    Serial.print(" - Set to: ");
    Serial.println(inputMessage2);
    request->send(200, "text/plain", "OK");
  });

  server.begin();
  

  trackingAltitude.NewLocation = 1995;

  Serial.println("Starting Webserver");



}


void loop() {
  // put your main code here, to run repeatedly:
  plane.update();
  
  trackingAltitude.update();
  trackingDirection.update();
  //delay(100);

  // if (motorInitialisation == true){
  //   delay(1000);
  //   trackingDirection.initMoving();
  //   delay(1000);
  //   trackingAltitude.initMoving();
  //   delay(5000);
  //   motorInitialisation = false;
  // }

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
  dirangle = rad2deg(atan2(x,y));
  // Serial.print("Heading: ");
  // Serial.println(dirangle);
  return dirangle; //Angle relative to North
}

//Get current altitude reading from the 2 GPS coordinates
float getAltitude(GPSLocation curloc, GPSLocation newloc){
  //First determine the distance between the two coordinates on the ground
  float lat1 = deg2rad(curloc.latitude);
  float lon1 = deg2rad(curloc.longitude);
  float lat2 = deg2rad(newloc.latitude);
  float lon2 = deg2rad(newloc.longitude);
  
  float dlat = lat2-lat1;
  float dlon = lon2-lon1;

  float a = sin(dlat/2)*sin(dlat/2) + cos(lat1) * cos(lat2) * sin(dlon/2) * sin(dlon/2);
  float c = 2 * atan2(sqrt(a),sqrt(1-a));
  float d = 6371*1000*c;
  // Serial.print("Distance: ");
  // Serial.println(d);

  // Serial.print("Altitude (m): ");
  // Serial.println(newloc.altitude*0.3048);

  //Now use the altitude and distance to determine the altitude angle
  altangle = atan((newloc.altitude*0.3048-curloc.altitude)/d);
  altangle = rad2deg(altangle);

  // Serial.print("Angle from 0: ");
  // Serial.println(altangle);
  return altangle;
}

//Function to set the direction and altitude of the servos
void setDirectionAltitude(double curLat, double curLon, double curAlt, double newLat, double newLon, double newAlt){
  //Create GPS struct
  GPSLocation curLoc = {curLat, curLon, curAlt};
  GPSLocation newLoc = {newLat, newLon, newAlt};

  //Determine heading and altitude
  float direction = getHeading(curLoc, newLoc);
  float altitude = getAltitude(curLoc, newLoc);

  //Figure out if the direction is left or right
  if (direction > 0){
    // Serial.println("Heading is to the right of the device");
    trackingAltitude.objectFront = true;
    trackingDirection.NewLocation = map(direction, 0, 180, trackingDirection.Max, trackingDirection.Min);
    trackingAltitude.NewLocation = map(altitude, 0, 90, trackingAltitude.Min, trackingAltitude.Center);
    return;
  }

  // Serial.println("Heading is to the left of the device");
  trackingAltitude.objectFront = false;
  trackingDirection.NewLocation = map(direction, 0, -180, trackingDirection.Min, trackingDirection.Max);
  trackingAltitude.NewLocation = map(altitude, 0, 90, trackingAltitude.Max, trackingAltitude.Center);
  return;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  float lat2 = 51.8242;
  float lon2 = 4.8;

  float lat1 = 51.997842;
  float lon1 = 4.374279;

  
  Serial.println(heading(lat1,lon1,lat2,lon2));
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
  return rad2deg(atan2(x,y)); //Angle relative to North
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10); // this speeds up the simulation
}

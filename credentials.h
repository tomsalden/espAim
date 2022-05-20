#ifndef CREDENTIALS_H
#define CREDENTIALS_H

// AP Settings define how you connect to the wifiManager 
// AP_SSID is the network name, AP_PASS is the password used to connect to it.
const char* AP_SSID = "Fam.Salden2";   // DEFAULTVALUE: Schrodingers Lantern
const char* AP_PASS = "tomenluc";

// HOSTNAME is the name of the device. 
// The easiest method to connect to the webUI is to visit http://HOSTNAME.local
#define HOSTNAME "espAim"                      // DEFAULTVALUE: lantern


// The OTA_PASS is used to verify OTA uploads and prevent anyone on your network 
// from uploading firmware to the device
#define OTA_PASS "espAim"

#endif
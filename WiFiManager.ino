#define WM_CRED_FILE "/wm_cred.dat"

/*
   Retrieve credentials from file in SPIFFS
   Writes to the ssid and pw pointers which must be
   created in the scope of the function calling this function
*/
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

/*
   Write credentials to the credential file in SPIFFS
*/
void writeCredentials(const char *ssid, const char *pw)
{
  Serial.printf("Writing to file: %s\r\n", WM_CRED_FILE);
  File file = SPIFFS.open(WM_CRED_FILE, "w");
  if (!file)
  {
    Serial.println("ERROR: failed to open file for writing");
    return;
  }

  if (file.print(ssid) && file.print("\n") && file.print(pw))
  {
    Serial.println("- file written");
  }
  else
  {
    Serial.println("- write failed");
  }
  file.close();
}

/*
   Start wifi manager
   This function reads stored credentials and tries to connect to WiFi.
   If this action fails, the wifimanager portal is deployed.
   returns true if connection succesful,
   returns false if portal is deployed
*/
bool WiFiManagerBegin(const char *ap_ssid, const char *ap_pw)
{
  // buffers for the ssid and password
  char ssid[30] = "";
  char pw[30] = "";

  // open credentials file and extract ssid and password
  readCredentials(ssid, pw);

  if (WiFiConnect(ssid, pw)) return true;
  else
  {
    // scan for available wifi networks
    int n = scanWifiNetworks();
    if (n > 0) listNetworks(n);

    // switch to AP mode to host login portal
    WiFi.mode(WIFI_AP);
    delay(100);
    WiFi.softAP(AP_SSID, AP_PASS);
    return false;
  }
}

/*
   Attempts to connect to wifi given the credentials
   from the arguments.
   Returns true if succesful, false if connection failed
*/
bool WiFiConnect(const char *ssid, const char *pw)
{
  WiFi.mode(WIFI_STA);
  delay(200);
  Serial.println("Attempting connection:");
  Serial.printf(" - ssid:     %s\n", ssid);
  Serial.printf(" - password: %s\n", pw);

  // Attempt to connect to network
  WiFi.begin(ssid, pw);
  Serial.print("Connecting");
  uint8_t timeout = 50;
  while (WiFi.status() != WL_CONNECTED)
  {
    yield();
    delay(100);
    Serial.print(".");
    timeout--;
    if (timeout == 0)
      break;
  }

  // verify connection status
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi Connected!");
    return true;
  }

  // print newline in case connection failed
  Serial.println();
  return false;
}

int scanWifiNetworks()
{
  WiFi.disconnect();
  delay(200);
  int n = WiFi.scanNetworks(); // number of found networks
  if (n == WIFI_SCAN_FAILED)
  {
    Serial.println("scan failed");
    return 0;
  }
  else if (n == WIFI_SCAN_RUNNING)
  {
    Serial.print("Scanning for networks");
    while (WiFi.scanComplete() == WIFI_SCAN_RUNNING)
    {
      Serial.print(".");
      delay(100);
    }
    Serial.println();
  }
  Serial.print("Number of networks: ");
  Serial.println(n);
  return n;
}

void listNetworks(int n)
{
  Serial.println("-----------------------------");
  for (int i = 0; i < n; i++)
  {
    Serial.print((String)WiFi.SSID(i));
    Serial.print(", RSSI: ");
    Serial.println((String)WiFi.RSSI(i));
  }
  Serial.println("-----------------------------\n");
}

void setupWMWebpages()
{
  server.reset();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    //Send index.htm with default content type
    request->send(SPIFFS, "/wm/index.html");
  });

  // make css available
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send(SPIFFS, "/wm/style.css");
  });

  // make js script available
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send(SPIFFS, "/wm/script.js");
  });

  // make klushok logo available
  server.on("/klushok.svg", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send(SPIFFS, "/klushok.svg");
  });

  // make icon available
  server.on("/favicon.svg", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon.svg");
  });
  server.on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon.png");
  });

  // Return scan results as JSON.
  // This function doesn't actually run the scan, it only returns
  // results from the previous scan!
  server.on("/scan", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    String json = "[";
    int n = WiFi.scanComplete();
    //   if (n == WIFI_SCAN_FAILED) scanWifiNetworks();
    if (n > 0)
    {
      for (int i = 0; i < n; i++)
      {
        json += "{";
        json += "\"rssi\":" + String(WiFi.RSSI(i));
        json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
        json += ",\"secure\":" + String(WiFi.encryptionType(i));
        json += "}";
        if (i < n - 1)
          json += ",";
      }
    }
    json += "]";
    request->send(200, "application/json", json);
  });

  server.on("/set", HTTP_POST, [](AsyncWebServerRequest * request)
  {
    if (request->hasParam("ssid", true) &&
        request->hasParam("pw", true))
    {
      //      request->send(200, "application/json", "Attempting to connect...");
      char buf[100] = "http://";
      strcat(buf, HOSTNAME);
      strcat(buf, ".local");
      request->redirect(buf);
      Serial.println("Credentials received");
      AsyncWebParameter *ssid = request->getParam("ssid", true);
      AsyncWebParameter *pw = request->getParam("pw", true);

      writeCredentials(
        ssid->value().c_str(),
        pw->value().c_str());
      ESP.restart();
    }
    else
    {
      request->send(404, "application/json", "Something went wrong");
    }
  });
}

void clearWMCredentials()
{
  Serial.println("Clearing credentials");
  writeCredentials("-", "-");
}

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoOTA.h>
#include <FS.h>

const char* ssid = "API-HOTSPOT";
const char* password = "nevergiveup";
const char* versionUrl = "https://raw.githubusercontent.com/asaddun/MyStorage/main/githubOTA/version.txt";
String currentVersion = "1.0";  // Change this to the version of your current sketch

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  // Initialize OTA
  ArduinoOTA.begin();
}

void loop() {
  // Handle OTA update
  ArduinoOTA.handle();

  // Check for new sketch if there is a change in version
  if (checkForUpdate()) {
    downloadSketch();
  }

  // Your main code goes here
}

bool checkForUpdate() {
  Serial.println("Checking for update...");

  // Make HTTP request to download version file
  HTTPClient http;
  http.begin(versionUrl);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String latestVersion = http.getString();
    Serial.println("Latest version: " + latestVersion);

    // Compare versions
    if (latestVersion != currentVersion) {
      Serial.println("New version available");
      return true;
    } else {
      Serial.println("No update available");
      return false;
    }
  } else {
    Serial.println("Failed to get version file");
    return false;
  }
  http.end();
}

void downloadSketch() {
  Serial.println("Downloading new sketch...");

  // Make HTTP request to download new sketch
  HTTPClient http;
  http.begin("https://raw.githubusercontent.com/asaddun/MyStorage/main/githubOTA/sketch.ino");
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    // Save downloaded sketch to SPIFFS
    File file = SPIFFS.open("/sketch.ino", "w");
    if (file) {
      file.write(http.getString().c_str());
      file.close();
      Serial.println("New sketch downloaded");

      // Restart ESP8266 to apply the update
      ESP.restart();
    } else {
      Serial.println("Failed to save sketch");
    }
  } else {
    Serial.println("Failed to download sketch");
  }
  http.end();
}

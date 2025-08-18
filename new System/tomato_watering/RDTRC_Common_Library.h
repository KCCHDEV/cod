/*
 * RDTRC Common Library - Shared Functions and Utilities
 * Version: 4.0
 * Firmware made by: RDTRC
 * Updated: 2024
 * 
 * This library contains common functions and utilities that can be shared
 * between different RDTRC growing systems (mushroom, cilantro, etc.)
 * 
 * Usage:
 * #include "RDTRC_Common_Library.h"
 */

#ifndef RDTRC_COMMON_LIBRARY_H
#define RDTRC_COMMON_LIBRARY_H

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <BlynkSimpleEsp32.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoOTA.h>
#include <DHT.h>

// Common System Configuration
#define RDTRC_FIRMWARE_VERSION "4.0"
#define RDTRC_FIRMWARE_MAKER "RDTRC"

// Common Pin Definitions (can be overridden in main sketch)
#ifndef DHT_PIN
#define DHT_PIN 22
#endif

#ifndef DHT_TYPE
#define DHT_TYPE DHT22
#endif

#ifndef STATUS_LED_PIN
#define STATUS_LED_PIN 2
#endif

#ifndef BUZZER_PIN
#define BUZZER_PIN 4
#endif

#ifndef RESET_BUTTON_PIN
#define RESET_BUTTON_PIN 0
#endif

#ifndef MANUAL_BUTTON_PIN
#define MANUAL_BUTTON_PIN 27
#endif

// Common thresholds
#define RDTRC_LOW_WATER_THRESHOLD 8
#define RDTRC_HIGH_CO2_THRESHOLD 1000
#define RDTRC_OPTIMAL_PH_MIN 6.0
#define RDTRC_OPTIMAL_PH_MAX 7.0

// Common structures
struct RDTRCEnvironmentalData {
  float temperature;
  float humidity;
  int co2Level;
  float phLevel;
  int lightLevel;
  float waterLevel;
  bool isDaylight;
  unsigned long timestamp;
};

struct RDTRCSystemStatus {
  bool wifiConnected;
  bool maintenanceMode;
  unsigned long uptime;
  int freeMemory;
  int wifiSignal;
  String systemName;
  String deviceId;
  String firmwareVersion;
};

// Common utility functions
class RDTRCCommon {
  public:
    // System initialization
    static void initializePins();
    static void playBootMelody();
    static void displayBootScreen(String systemName);
    
    // WiFi management
    static bool connectWiFi(const char* ssid, const char* password, int timeout = 30);
    static void setupHotspot(const char* hotspotSSID, const char* hotspotPassword);
    static void setupMDNS(const char* hostname);
    
    // OTA updates
    static void setupOTA(const char* hostname, const char* password = "rdtrc2024");
    
    // Sensor reading utilities
    static RDTRCEnvironmentalData readEnvironmentalSensors(DHT& dht, int co2Pin, int phPin, int lightPin, int waterTrigPin, int waterEchoPin, int tankHeight);
    static int readSoilMoisture(int pin);
    static float readWaterLevel(int trigPin, int echoPin, int tankHeight);
    
    // Communication utilities
    static void sendLineNotification(const char* token, String message);
    static void sendBlynkUpdate(int pin, float value);
    
    // File system utilities
    static bool initializeSPIFFS();
    static void rotateLogFile(const char* filename, int maxSize = 150000);
    static void saveJSONConfig(const char* filename, JsonDocument& doc);
    static bool loadJSONConfig(const char* filename, JsonDocument& doc);
    
    // System maintenance
    static void checkSystemHealth();
    static void handleResetButton();
    static void handleEmergencyStop();
    
    // Alert management
    static void checkCommonAlerts(RDTRCEnvironmentalData& data, void (*alertCallback)(String));
    
    // Time utilities
    static String getFormattedDate(NTPClient& timeClient);
    static String getFormattedTime(NTPClient& timeClient);
    static bool isTimeInRange(NTPClient& timeClient, int startHour, int startMinute, int durationMinutes);
    
    // Web interface utilities
    static String generateStatusJSON(RDTRCSystemStatus& status, RDTRCEnvironmentalData& envData);
    static String generateCommonCSS();
    static String generateCommonJS();
    
    // Mathematical utilities
    static float mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh);
    static int constrainInt(int value, int min, int max);
    static float constrainFloat(float value, float min, float max);
    
    // String utilities
    static String repeatString(String str, int count);
    static String formatUptime(unsigned long milliseconds);
    static String formatMemory(int bytes);
};

// Implementation of static methods
void RDTRCCommon::initializePins() {
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MANUAL_BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize LED to OFF
  digitalWrite(STATUS_LED_PIN, LOW);
}

void RDTRCCommon::playBootMelody() {
  // Boot sequence beeps
  for (int i = 0; i < 3; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    tone(BUZZER_PIN, 1200 + (i * 200), 300);
    delay(400);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(200);
  }
  
  // Welcome melody
  int melody[] = {1500, 1800, 2100, 2400, 2100, 1800, 1500};
  for (int i = 0; i < 7; i++) {
    tone(BUZZER_PIN, melody[i], 200);
    delay(250);
  }
}

void RDTRCCommon::displayBootScreen(String systemName) {
  Serial.println("\n" + repeatString("=", 60));
  Serial.println("🌱 RDTRC " + systemName);
  Serial.println("");
  Serial.println("Firmware made by: " + String(RDTRC_FIRMWARE_MAKER));
  Serial.println("Version: " + String(RDTRC_FIRMWARE_VERSION));
  Serial.println("");
  Serial.println("Features:");
  Serial.println("✓ Advanced Environmental Monitoring");
  Serial.println("✓ Automated Control Systems");
  Serial.println("✓ Web Interface");
  Serial.println("✓ Mobile App Control");
  Serial.println("✓ Smart Notifications");
  Serial.println("✓ Data Logging & Analytics");
  Serial.println("✓ OTA Updates");
  Serial.println("");
  Serial.println("Initializing Growing Environment...");
  Serial.println(repeatString("=", 60));
  delay(3000);
}

bool RDTRCCommon::connectWiFi(const char* ssid, const char* password, int timeout) {
  Serial.println("🔗 Connecting to WiFi: " + String(ssid));
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < timeout) {
    delay(1000);
    Serial.print(".");
    attempts++;
    
    // Blink LED while connecting
    digitalWrite(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected!");
    Serial.println("📍 IP Address: " + WiFi.localIP().toString());
    Serial.println("📶 Signal Strength: " + String(WiFi.RSSI()) + " dBm");
    return true;
  } else {
    Serial.println("\n❌ WiFi connection failed");
    return false;
  }
}

void RDTRCCommon::setupHotspot(const char* hotspotSSID, const char* hotspotPassword) {
  WiFi.softAP(hotspotSSID, hotspotPassword);
  Serial.println("🔥 Hotspot started: " + String(hotspotSSID));
  Serial.println("📍 Hotspot IP: " + WiFi.softAPIP().toString());
  Serial.println("🔑 Password: " + String(hotspotPassword));
}

void RDTRCCommon::setupMDNS(const char* hostname) {
  if (MDNS.begin(hostname)) {
    Serial.println("✅ mDNS responder started: " + String(hostname) + ".local");
  }
}

void RDTRCCommon::setupOTA(const char* hostname, const char* password) {
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.setPassword(password);
  
  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    Serial.println("🔄 OTA Update started: " + type);
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\n✅ OTA Update completed");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("📊 OTA Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("❌ OTA Error[%u]: ", error);
  });
  
  ArduinoOTA.begin();
  Serial.println("✅ OTA updates enabled");
}

RDTRCEnvironmentalData RDTRCCommon::readEnvironmentalSensors(DHT& dht, int co2Pin, int phPin, int lightPin, int waterTrigPin, int waterEchoPin, int tankHeight) {
  RDTRCEnvironmentalData data;
  
  // Read DHT sensor
  data.temperature = dht.readTemperature();
  data.humidity = dht.readHumidity();
  
  // Check for sensor errors
  if (isnan(data.temperature) || isnan(data.humidity)) {
    Serial.println("❌ DHT sensor error");
    data.temperature = 0;
    data.humidity = 0;
  }
  
  // Read CO2 sensor (analog approximation)
  int co2Raw = analogRead(co2Pin);
  data.co2Level = map(co2Raw, 0, 4095, 400, 2000);
  
  // Read pH sensor (analog approximation)
  int phRaw = analogRead(phPin);
  data.phLevel = mapFloat(phRaw, 0, 4095, 4.0, 10.0);
  
  // Read light sensor
  data.lightLevel = analogRead(lightPin);
  data.isDaylight = data.lightLevel > 500;
  
  // Read water level
  data.waterLevel = readWaterLevel(waterTrigPin, waterEchoPin, tankHeight);
  
  data.timestamp = millis();
  
  return data;
}

int RDTRCCommon::readSoilMoisture(int pin) {
  int raw = analogRead(pin);
  int moisture = map(raw, 4095, 0, 0, 100); // Invert for dry=low, wet=high
  return constrainInt(moisture, 0, 100);
}

float RDTRCCommon::readWaterLevel(int trigPin, int echoPin, int tankHeight) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration > 0) {
    float level = tankHeight - (duration * 0.034 / 2);
    return constrainFloat(level, 0, tankHeight);
  }
  return 0;
}

void RDTRCCommon::sendLineNotification(const char* token, String message) {
  if (WiFi.status() != WL_CONNECTED || strlen(token) < 10) return;
  
  HTTPClient http;
  http.begin("https://notify-api.line.me/api/notify");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", "Bearer " + String(token));
  
  String payload = "message=" + message;
  int httpResponseCode = http.POST(payload);
  
  if (httpResponseCode == 200) {
    Serial.println("📱 LINE notification sent");
  } else {
    Serial.println("❌ LINE notification failed: " + String(httpResponseCode));
  }
  
  http.end();
}

bool RDTRCCommon::initializeSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ SPIFFS initialization failed");
    return false;
  } else {
    Serial.println("✅ SPIFFS initialized");
    return true;
  }
}

void RDTRCCommon::rotateLogFile(const char* filename, int maxSize) {
  File logFile = SPIFFS.open(filename, "r");
  if (logFile && logFile.size() > maxSize) {
    logFile.close();
    String oldFilename = String(filename) + ".old";
    SPIFFS.remove(oldFilename.c_str());
    SPIFFS.rename(filename, oldFilename.c_str());
    Serial.println("🔄 Log file rotated: " + String(filename));
  }
  if (logFile) logFile.close();
}

void RDTRCCommon::saveJSONConfig(const char* filename, JsonDocument& doc) {
  File configFile = SPIFFS.open(filename, "w");
  if (configFile) {
    serializeJson(doc, configFile);
    configFile.close();
    Serial.println("💾 Configuration saved: " + String(filename));
  }
}

bool RDTRCCommon::loadJSONConfig(const char* filename, JsonDocument& doc) {
  File configFile = SPIFFS.open(filename, "r");
  if (configFile) {
    bool success = deserializeJson(doc, configFile) == DeserializationError::Ok;
    configFile.close();
    if (success) {
      Serial.println("📖 Configuration loaded: " + String(filename));
    }
    return success;
  }
  return false;
}

void RDTRCCommon::checkSystemHealth() {
  if (ESP.getFreeHeap() < 20000) {
    Serial.println("⚠️ Low memory warning: " + String(ESP.getFreeHeap()) + " bytes");
  }
}

void RDTRCCommon::checkCommonAlerts(RDTRCEnvironmentalData& data, void (*alertCallback)(String)) {
  if (!alertCallback) return;
  
  // Low water alert
  if (data.waterLevel < RDTRC_LOW_WATER_THRESHOLD) {
    String alertMsg = "⚠️ Low Water Level Alert!\n" +
                     "Current Level: " + String(data.waterLevel) + "cm\n" +
                     "Please refill the water tank.";
    alertCallback(alertMsg);
  }
  
  // High CO2 alert
  if (data.co2Level > RDTRC_HIGH_CO2_THRESHOLD) {
    String alertMsg = "⚠️ High CO2 Level Alert!\n" +
                     "Current Level: " + String(data.co2Level) + "ppm\n" +
                     "Ventilation activated.";
    alertCallback(alertMsg);
  }
  
  // pH out of range alert
  if (data.phLevel < RDTRC_OPTIMAL_PH_MIN || data.phLevel > RDTRC_OPTIMAL_PH_MAX) {
    String alertMsg = "⚠️ pH Level Alert!\n" +
                     "Current pH: " + String(data.phLevel) + "\n" +
                     "Optimal range: " + String(RDTRC_OPTIMAL_PH_MIN) + "-" + String(RDTRC_OPTIMAL_PH_MAX);
    alertCallback(alertMsg);
  }
  
  // Temperature alerts
  if (data.temperature > 35.0) {
    String alertMsg = "🌡️ High Temperature Alert!\n" +
                     "Current: " + String(data.temperature) + "°C\n" +
                     "Check cooling systems.";
    alertCallback(alertMsg);
  } else if (data.temperature < 10.0) {
    String alertMsg = "🌡️ Low Temperature Alert!\n" +
                     "Current: " + String(data.temperature) + "°C\n" +
                     "Check heating systems.";
    alertCallback(alertMsg);
  }
}

String RDTRCCommon::getFormattedDate(NTPClient& timeClient) {
  return String(timeClient.getDay()) + "/" + String(timeClient.getMonth()) + "/" + String(timeClient.getYear());
}

String RDTRCCommon::getFormattedTime(NTPClient& timeClient) {
  return String(timeClient.getHours()) + ":" + String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds());
}

bool RDTRCCommon::isTimeInRange(NTPClient& timeClient, int startHour, int startMinute, int durationMinutes) {
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentTimeMinutes = currentHour * 60 + currentMinute;
  
  int startTime = startHour * 60 + startMinute;
  int endTime = startTime + durationMinutes;
  
  return (currentTimeMinutes >= startTime && currentTimeMinutes < endTime);
}

float RDTRCCommon::mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
  return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

int RDTRCCommon::constrainInt(int value, int min, int max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}

float RDTRCCommon::constrainFloat(float value, float min, float max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}

String RDTRCCommon::repeatString(String str, int count) {
  String result = "";
  for (int i = 0; i < count; i++) {
    result += str;
  }
  return result;
}

String RDTRCCommon::formatUptime(unsigned long milliseconds) {
  unsigned long seconds = milliseconds / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;
  
  if (days > 0) {
    return String(days) + "d " + String(hours % 24) + "h " + String(minutes % 60) + "m";
  } else if (hours > 0) {
    return String(hours) + "h " + String(minutes % 60) + "m " + String(seconds % 60) + "s";
  } else if (minutes > 0) {
    return String(minutes) + "m " + String(seconds % 60) + "s";
  } else {
    return String(seconds) + "s";
  }
}

String RDTRCCommon::formatMemory(int bytes) {
  if (bytes > 1024 * 1024) {
    return String(bytes / 1024 / 1024) + " MB";
  } else if (bytes > 1024) {
    return String(bytes / 1024) + " KB";
  } else {
    return String(bytes) + " B";
  }
}

String RDTRCCommon::generateCommonCSS() {
  return R"(
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #0f1419; color: #e6e6e6; }
    .header { padding: 20px; text-align: center; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }
    .header h1 { color: white; margin-bottom: 10px; font-size: 28px; }
    .header p { color: #f0f0f0; opacity: 0.9; }
    .container { max-width: 1600px; margin: 20px auto; padding: 0 20px; }
    .status-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(140px, 1fr)); gap: 15px; margin-bottom: 30px; }
    .status-card { background: #1a1f2e; border-radius: 10px; padding: 15px; text-align: center; border: 1px solid #2d3748; transition: transform 0.2s; }
    .status-card:hover { transform: translateY(-2px); }
    .status-card h3 { margin-bottom: 8px; font-size: 11px; text-transform: uppercase; }
    .status-card .value { font-size: 16px; font-weight: bold; }
    .btn { padding: 8px 16px; border: none; border-radius: 5px; cursor: pointer; font-size: 12px; font-weight: 600; transition: all 0.2s; margin: 2px; }
    .btn-primary { background: #8e44ad; color: white; }
    .btn-success { background: #2ecc71; color: white; }
    .btn-danger { background: #e74c3c; color: white; }
    .btn-secondary { background: #4a5568; color: white; }
    .btn-warning { background: #f39c12; color: white; }
    .btn:hover { opacity: 0.8; transform: translateY(-1px); }
    .btn:disabled { opacity: 0.5; cursor: not-allowed; transform: none; }
    .alert { padding: 12px; border-radius: 5px; margin-bottom: 15px; font-size: 14px; }
    .alert-warning { background: #f39c12; color: white; }
    .alert-info { background: #3498db; color: white; }
    .alert-danger { background: #e74c3c; color: white; }
  )";
}

String RDTRCCommon::generateCommonJS() {
  return R"(
    function formatUptime(milliseconds) {
      const seconds = Math.floor(milliseconds / 1000);
      const minutes = Math.floor(seconds / 60);
      const hours = Math.floor(minutes / 60);
      const days = Math.floor(hours / 24);
      
      if (days > 0) {
        return days + 'd ' + (hours % 24) + 'h ' + (minutes % 60) + 'm';
      } else if (hours > 0) {
        return hours + 'h ' + (minutes % 60) + 'm ' + (seconds % 60) + 's';
      } else if (minutes > 0) {
        return minutes + 'm ' + (seconds % 60) + 's';
      } else {
        return seconds + 's';
      }
    }
    
    function formatMemory(bytes) {
      if (bytes > 1024 * 1024) {
        return Math.floor(bytes / 1024 / 1024) + ' MB';
      } else if (bytes > 1024) {
        return Math.floor(bytes / 1024) + ' KB';
      } else {
        return bytes + ' B';
      }
    }
    
    function showAlert(message, type = 'info') {
      const alertDiv = document.createElement('div');
      alertDiv.className = 'alert alert-' + type;
      alertDiv.textContent = message;
      
      const alertsContainer = document.getElementById('alerts');
      if (alertsContainer) {
        alertsContainer.appendChild(alertDiv);
        setTimeout(() => alertDiv.remove(), 5000);
      }
    }
  )";
}

#endif // RDTRC_COMMON_LIBRARY_H
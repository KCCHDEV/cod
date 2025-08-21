/*
 * RDTRC Complete Mushroom Growing System with LCD - Standalone Version
 * Version: 4.0 - Independent System with LCD I2C 16x2 Support
 * Firmware made by: RDTRC
 * Updated: 2024
 * 
 * Features:
 * - Complete standalone mushroom growing system
 * - LCD I2C 16x2 display with auto address detection
 * - Advanced environmental control
 * - Soil moisture monitoring
 * - Temperature, humidity, CO2, and pH monitoring
 * - Automated misting system for mushrooms
 * - Ventilation fan control
 * - Heating system for temperature control
 * - Built-in web interface
 * - Blynk integration for mobile control
 * - Hotspot mode for direct access
 * - Data logging to SPIFFS
 * - EMAIL/LINE notifications
 * - OTA updates
 * - Growth phase management
 * - LCD debug and status display
 */

// Blynk Configuration - MUST be defined BEFORE includes
#define BLYNK_TEMPLATE_ID "TMPL61Zdwsx9r"
#define BLYNK_TEMPLATE_NAME "Mushroom_Growing_System"
#define BLYNK_AUTH_TOKEN "H4AnMNnYtDTRBl1qssnraGZbVmnKoC8e"

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
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RDTRC_LCD_Library.h"

// System Configuration
#define FIRMWARE_VERSION "4.0"
#define FIRMWARE_MAKER "RDTRC"
#define SYSTEM_NAME "Mushroom System"
#define DEVICE_ID "RDTRC_MUSHROOM"

// Network Configuration
const char* ssid = "WIN-D61OOTVOG2V 2791";
const char* password = "%476g05A";
const char* hotspot_ssid = "RDTRC_Mushroom";
const char* hotspot_password = "rdtrc123";

// LINE Notify Configuration
const char* lineToken = "YOUR_LINE_NOTIFY_TOKEN";

// Pin Definitions
#define DHT_PIN 22
#define DHT_TYPE DHT22
#define CO2_SENSOR_PIN 23
#define PH_SENSOR_PIN 32
#define LIGHT_SENSOR_PIN 33
#define EC_SENSOR_PIN 25
#define AIR_QUALITY_SENSOR_PIN 26
#define WATER_LEVEL_SENSOR_PIN 27
#define FLOW_SENSOR_PIN 35

// Mushroom Zone Pins
#define MUSHROOM_SOIL_PIN 34
#define MUSHROOM_MISTING_PIN 18
#define MUSHROOM_HEATER_PIN 19
#define MUSHROOM_FAN_PIN 21

// System Control Pins
#define STATUS_LED_PIN 2
#define BUZZER_PIN 4
#define RESET_BUTTON_PIN 0
#define MANUAL_BUTTON_PIN 27
#define LCD_NEXT_BUTTON_PIN 26  // Button to navigate LCD pages

// Water System Pins
#define WATER_LEVEL_TRIG_PIN 25
#define WATER_LEVEL_ECHO_PIN 14

// I2C Pins (ESP32 default: SDA=21, SCL=22, but we can define custom)
#define I2C_SDA 21
#define I2C_SCL 22

// Growing Configuration
#define MUSHROOM_OPTIMAL_TEMP 22.0      // °C
#define MUSHROOM_OPTIMAL_HUMIDITY 85.0   // %
#define MUSHROOM_OPTIMAL_MOISTURE 70     // %

#define WATER_TANK_HEIGHT 40             // cm
#define LOW_WATER_THRESHOLD 8            // cm
#define HIGH_CO2_THRESHOLD 1000          // ppm
#define OPTIMAL_PH_MIN 6.0
#define OPTIMAL_PH_MAX 7.0

// Environmental thresholds for mushroom growing
#define TEMP_MIN 15.0
#define TEMP_MAX 35.0
#define HUMIDITY_MIN 30.0
#define HUMIDITY_MAX 95.0
#define EC_MIN 0.5
#define EC_MAX 3.0
#define AIR_QUALITY_MIN 50.0

// Offline detection constants
#define SENSOR_TIMEOUT 30000      // 30 seconds timeout
#define SENSOR_RETRY_INTERVAL 60000 // 1 minute retry interval

// System Objects
WebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); // UTC+7 Thailand
DHT dht(DHT_PIN, DHT_TYPE);
RDTRC_LCD systemLCD;

// Sensor Status Structure
struct SensorStatus {
  bool isOnline;
  unsigned long lastReading;
  float lastValue;
  int errorCount;
  String sensorName;
};

// Sensor Status Instances
SensorStatus dhtSensor;
SensorStatus co2Sensor;
SensorStatus phSensor;
SensorStatus ecSensor;
SensorStatus lightSensor;
SensorStatus airQualitySensor;
SensorStatus waterLevelSensor;
SensorStatus flowSensor;
SensorStatus soilSensor;
SensorStatus lcdSensor;

// System Variables
bool isWiFiConnected = false;
bool isHotspotMode = false;
unsigned long lastHeartbeat = 0;
unsigned long lastDataLog = 0;
unsigned long lastStatusCheck = 0;
unsigned long lastLCDUpdate = 0;
unsigned long bootTime = 0;

// Growing Zone
struct GrowingZone {
  String name;
  int soilPin;
  int waterPin;
  float targetTemp;
  float targetHumidity;
  int targetMoisture;
  float currentTemp;
  float currentHumidity;
  int currentMoisture;
  bool wateringActive;
  bool heatingActive;
  bool fanActive;
  unsigned long lastWatered;
  unsigned long totalWateringTime;
  String growthPhase;
  int daysInPhase;
  bool enabled;
};

GrowingZone mushroom = {
  "Mushroom Zone", MUSHROOM_SOIL_PIN, MUSHROOM_MISTING_PIN,
  MUSHROOM_OPTIMAL_TEMP, MUSHROOM_OPTIMAL_HUMIDITY, MUSHROOM_OPTIMAL_MOISTURE,
  0, 0, 0, false, false, false, 0, 0, "Inoculation", 0, true
};

// Environmental Variables
float ambientTemperature = 0;
float ambientHumidity = 0;
int co2Level = 0;
float phLevel = 7.0;
float ecLevel = 0;
int lightLevel = 0;
float airQualityLevel = 0;
float waterLevel = 0;
float flowRate = 0;
bool isDaylight = true;
bool systemMaintenanceMode = false;

// Growth Phase Schedules
struct GrowthPhase {
  String name;
  int duration; // days
  float tempRange[2]; // min, max
  float humidityRange[2]; // min, max
  int moistureRange[2]; // min, max
  bool needsVentilation;
  String description;
};

// Mushroom Growth Phases
GrowthPhase mushroomPhases[4] = {
  {"Inoculation", 14, {20, 24}, {80, 90}, {65, 75}, false, "Spore inoculation phase"},
  {"Colonization", 21, {22, 25}, {75, 85}, {70, 80}, true, "Mycelium colonization"},
  {"Pinning", 7, {18, 22}, {85, 95}, {75, 85}, true, "Pin formation"},
  {"Fruiting", 14, {16, 20}, {80, 90}, {70, 80}, true, "Mushroom fruiting"}
};

// Statistics
struct DailyStats {
  String date;
  float avgTemperature;
  float avgHumidity;
  int avgCO2;
  float avgPH;
  int wateringCycles;
  unsigned long totalWateringTime;
  int alerts;
};

DailyStats todayStats;

// Function Declarations
void setupSystem();
void setupLCD();
void setupWiFi();
void setupWebServer();
void setupBlynk();
void setupOTA();
void displayBootScreen();
void handleSystemLoop();
void controlEnvironment();
void readSensors();
void controlMushrooms();
void logData();
void handleWebInterface();
void sendLineNotification(String message);
void handleManualControls();
void handleLCDControls();
void updateLCDDisplay();
void saveSettings();
void loadSettings();
void performSystemMaintenance();
void updateGrowthPhases();
void checkAlerts();
void initializeSensors();
void checkSensorStatus();
void updateSensorStatus(int sensorId, bool success, float value);
void handleSensorError(int sensorId, String sensorName);
void gracefulDegradation();
bool canOperateWithOfflineSensors();
String getSensorStatusString();

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  bootTime = millis();
  displayBootScreen();
  setupSystem();
  
  Serial.println("RDTRC Mushroom Growing System with LCD Ready!");
  Serial.println("Web Interface: http://mushroom-system.local");
  Serial.println("Blynk App: Connected");
  Serial.println("LINE Notifications: Enabled");
  if (systemLCD.isLCDConnected()) {
    Serial.println("LCD Display: Connected at 0x" + String(systemLCD.getLCDAddress(), HEX));
  } else {
    Serial.println("LCD Display: Not found");
  }
}

void loop() {
  handleSystemLoop();
  
  // Handle web server
  server.handleClient();
  
  // Handle Blynk
  if (isWiFiConnected) {
    Blynk.run();
  }
  
  // Handle OTA updates
  ArduinoOTA.handle();
  
  // Environmental control
  if (!systemMaintenanceMode) {
    controlEnvironment();
  }
  
  // Handle manual controls
  handleManualControls();
  
  // Handle LCD controls
  handleLCDControls();
  
  // Update LCD display
  updateLCDDisplay();
  
  // Read sensors every 45 seconds
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead > 45000) {
    readSensors();
    lastSensorRead = millis();
  }
  
  // Log data every 15 minutes
  if (millis() - lastDataLog > 900000) {
    logData();
    lastDataLog = millis();
  }
  
  // System status check every 5 minutes
  if (millis() - lastStatusCheck > 300000) {
    performSystemMaintenance();
    updateGrowthPhases();
    checkAlerts();
    lastStatusCheck = millis();
  }
  
  delay(100);
}

void displayBootScreen() {
  Serial.println("\n============================================================");
  Serial.println("RDTRC Complete Mushroom Growing System with LCD");
  Serial.println("");
  Serial.println("Firmware made by: " + String(FIRMWARE_MAKER));
  Serial.println("Version: " + String(FIRMWARE_VERSION));
  Serial.println("System: " + String(SYSTEM_NAME));
  Serial.println("");
  Serial.println("Features:");
  Serial.println("* LCD I2C 16x2 Display with Auto Detection");
  Serial.println("* Advanced Environmental Monitoring");
  Serial.println("* Automated Misting System");
  Serial.println("* Temperature & Humidity Control");
  Serial.println("* CO2 & pH Monitoring");
  Serial.println("* Growth Phase Management");
  Serial.println("* Web Interface");
  Serial.println("* Mobile App Control");
  Serial.println("* Smart Notifications");
  Serial.println("* Data Logging & Analytics");
  Serial.println("* OTA Updates");
  Serial.println("");
  Serial.println("Initializing Mushroom Growing Environment...");
  Serial.println("============================================================");
  delay(3000);
}

void setupSystem() {
  // Initialize pins
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MANUAL_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LCD_NEXT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DHT_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(PH_SENSOR_PIN, INPUT);
  pinMode(EC_SENSOR_PIN, INPUT);
  pinMode(CO2_SENSOR_PIN, INPUT);
  pinMode(AIR_QUALITY_SENSOR_PIN, INPUT);
  pinMode(WATER_LEVEL_SENSOR_PIN, INPUT);
  pinMode(FLOW_SENSOR_PIN, INPUT);
  pinMode(WATER_LEVEL_TRIG_PIN, OUTPUT);
  pinMode(WATER_LEVEL_ECHO_PIN, INPUT);
  
  // Mushroom zone pins
  pinMode(MUSHROOM_MISTING_PIN, OUTPUT);
  pinMode(MUSHROOM_HEATER_PIN, OUTPUT);
  pinMode(MUSHROOM_FAN_PIN, OUTPUT);
  
  // Initialize all outputs to OFF
  digitalWrite(MUSHROOM_MISTING_PIN, LOW);
  digitalWrite(MUSHROOM_HEATER_PIN, LOW);
  digitalWrite(MUSHROOM_FAN_PIN, LOW);
  
  // Setup LCD first
  setupLCD();
  
  // Initialize DHT sensor
  dht.begin();
  systemLCD.showDebug("DHT22 Init", "Sensor Ready");
  
  // Initialize sensors
  initializeSensors();
  systemLCD.showDebug("Sensors", "Initialized");
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialization failed");
    systemLCD.showDebug("SPIFFS Failed", "Check Storage");
  } else {
    Serial.println("SPIFFS initialized");
    systemLCD.showDebug("SPIFFS OK", "Storage Ready");
  }
  
  // Load saved settings
  loadSettings();
  systemLCD.showDebug("Settings", "Loaded");
  
  // Setup network and services
  setupWiFi();
  setupWebServer();
  setupBlynk();
  setupOTA();
  
  // Initialize NTP
  timeClient.begin();
  timeClient.update();
  
  // Initialize today's stats
  todayStats.date = "01/01/2024"; // Simple date format
  todayStats.avgTemperature = 0;
  todayStats.avgHumidity = 0;
  todayStats.avgCO2 = 0;
  todayStats.avgPH = 0;
  todayStats.wateringCycles = 0;
  todayStats.totalWateringTime = 0;
  todayStats.alerts = 0;
  
  // Boot sequence
  systemLCD.showDebug("Boot Sequence", "Starting...");
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
  
  // Initial sensor reading
  readSensors();
  
  // Send startup notification
  String startupMsg = "RDTRC Mushroom Growing System Started!\n";
  startupMsg += "System: " + String(SYSTEM_NAME) + "\n";
  startupMsg += "Version: " + String(FIRMWARE_VERSION) + "\n";
  startupMsg += String("LCD: ") + (systemLCD.isLCDConnected() ? "Connected" : "Not Found") + "\n";
  startupMsg += "Water Level: " + String(waterLevel) + "cm\n";
  startupMsg += "Temperature: " + String(ambientTemperature) + "C\n";
  startupMsg += "Humidity: " + String(ambientHumidity) + "%\n";
  startupMsg += "Mushroom Phase: " + mushroom.growthPhase + "\n";
  startupMsg += "Status: Growing environment ready!";
  sendLineNotification(startupMsg);
  
  systemLCD.showDebug("System Ready!", "All OK");
  Serial.println("System initialization complete!");
}

void setupLCD() {
  Serial.println("Setting up LCD I2C 16x2...");
  
  // Initialize I2C with custom pins if needed
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Scan for I2C devices first
  systemLCD.scanI2C();
  
  // Initialize LCD with auto detection
  if (systemLCD.begin()) {
    Serial.println("LCD initialized successfully");
    systemLCD.setAutoScroll(true, 4000); // Auto scroll every 4 seconds
  } else {
    Serial.println("LCD initialization failed - continuing without LCD");
  }
}

void setupWiFi() {
  Serial.println("Connecting to WiFi: " + String(ssid));
  systemLCD.showDebug("WiFi Connect", String(ssid));
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(1000);
    Serial.print(".");
    attempts++;
    
    // Show progress on LCD
    systemLCD.showDebug("WiFi Connect", "Attempt " + String(attempts));
    
    // Blink LED while connecting
    digitalWrite(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    isWiFiConnected = true;
    Serial.println("\nWiFi Connected!");
    Serial.println("IP Address: " + WiFi.localIP().toString());
    Serial.println("Signal Strength: " + String(WiFi.RSSI()) + " dBm");
    
    systemLCD.showDebug("WiFi Connected", WiFi.localIP().toString());
    
    // Setup mDNS
    if (MDNS.begin("mushroom-system")) {
      Serial.println("mDNS responder started: mushroom-system.local");
    }
  } else {
    Serial.println("\nWiFi connection failed. Starting hotspot...");
    systemLCD.showDebug("WiFi Failed", "Starting Hotspot");
    setupHotspot();
  }
}

void setupHotspot() {
  WiFi.softAP(hotspot_ssid, hotspot_password);
  isHotspotMode = true;
  Serial.println("Hotspot started: " + String(hotspot_ssid));
  Serial.println("Hotspot IP: " + WiFi.softAPIP().toString());
  Serial.println("Password: " + String(hotspot_password));
  
  systemLCD.showDebug("Hotspot Mode", WiFi.softAPIP().toString());
  
  // Setup mDNS for hotspot
  if (MDNS.begin("mushroom-system")) {
    Serial.println("mDNS responder started for hotspot");
  }
}

void setupWebServer() {
  systemLCD.showDebug("Web Server", "Starting...");
  
  // Main dashboard
  server.on("/", handleWebInterface);
  
  // API Endpoints
  server.on("/api/status", HTTP_GET, []() {
    JsonDocument doc;
    doc["system_name"] = SYSTEM_NAME;
    doc["version"] = FIRMWARE_VERSION;
    doc["device_id"] = DEVICE_ID;
    doc["uptime"] = millis() - bootTime;
    doc["wifi_connected"] = isWiFiConnected;
    doc["wifi_signal"] = WiFi.RSSI();
    doc["ambient_temperature"] = ambientTemperature;
    doc["ambient_humidity"] = ambientHumidity;
    doc["co2_level"] = co2Level;
    doc["ph_level"] = phLevel;
    doc["light_level"] = lightLevel;
    doc["is_daylight"] = isDaylight;
    doc["water_level"] = waterLevel;
    doc["maintenance_mode"] = systemMaintenanceMode;
    doc["timestamp"] = timeClient.getEpochTime();
    doc["lcd_connected"] = systemLCD.isLCDConnected();
    doc["lcd_address"] = "0x" + String(systemLCD.getLCDAddress(), HEX);
    
    // Mushroom zone data
    JsonObject mushroomObj = doc.createNestedObject("mushroom");
    mushroomObj["name"] = mushroom.name;
    mushroomObj["moisture"] = mushroom.currentMoisture;
    mushroomObj["target_temp"] = mushroom.targetTemp;
    mushroomObj["target_humidity"] = mushroom.targetHumidity;
    mushroomObj["target_moisture"] = mushroom.targetMoisture;
    mushroomObj["watering_active"] = mushroom.wateringActive;
    mushroomObj["heating_active"] = mushroom.heatingActive;
    mushroomObj["fan_active"] = mushroom.fanActive;
    mushroomObj["growth_phase"] = mushroom.growthPhase;
    mushroomObj["days_in_phase"] = mushroom.daysInPhase;
    mushroomObj["enabled"] = mushroom.enabled;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/control", HTTP_POST, []() {
    if (systemMaintenanceMode) {
      server.send(423, "application/json", "{\"error\":\"system_in_maintenance\"}");
      return;
    }
    
    String zone = server.arg("zone");
    String action = server.arg("action");
    String value = server.arg("value");
    
    if (zone == "mushroom") {
      String lcdAction = "";
      if (action == "misting") {
        mushroom.wateringActive = value == "true";
        digitalWrite(MUSHROOM_MISTING_PIN, mushroom.wateringActive ? HIGH : LOW);
        lcdAction = mushroom.wateringActive ? "Misting ON" : "Misting OFF";
      } else if (action == "heating") {
        mushroom.heatingActive = value == "true";
        digitalWrite(MUSHROOM_HEATER_PIN, mushroom.heatingActive ? HIGH : LOW);
        lcdAction = mushroom.heatingActive ? "Heater ON" : "Heater OFF";
      } else if (action == "fan") {
        mushroom.fanActive = value == "true";
        digitalWrite(MUSHROOM_FAN_PIN, mushroom.fanActive ? HIGH : LOW);
        lcdAction = mushroom.fanActive ? "Fan ON" : "Fan OFF";
      }
      
      if (lcdAction != "") {
        systemLCD.showDebug("Control", lcdAction);
        systemLCD.updateStatus(SYSTEM_NAME, ambientTemperature, ambientHumidity, 
                              mushroom.currentMoisture, mushroom.growthPhase, 
                              isWiFiConnected, systemMaintenanceMode, todayStats.alerts, lcdAction);
      }
    }
    
    server.send(200, "application/json", "{\"status\":\"control_updated\"}");
  });
  
  server.on("/api/phase", HTTP_POST, []() {
    String zone = server.arg("zone");
    String phase = server.arg("phase");
    
    if (zone == "mushroom" && phase.length() > 0) {
      mushroom.growthPhase = phase;
      mushroom.daysInPhase = 0;
      saveSettings();
      
      systemLCD.showDebug("Phase Change", phase);
      systemLCD.updateStatus(SYSTEM_NAME, ambientTemperature, ambientHumidity, 
                            mushroom.currentMoisture, mushroom.growthPhase, 
                            isWiFiConnected, systemMaintenanceMode, todayStats.alerts, "Phase: " + phase);
      
      server.send(200, "application/json", "{\"status\":\"mushroom_phase_updated\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"invalid_parameters\"}");
    }
  });
  
  server.on("/api/settings", HTTP_POST, []() {
    bool updated = false;
    
    if (server.hasArg("mushroom_temp")) {
      mushroom.targetTemp = server.arg("mushroom_temp").toFloat();
      updated = true;
    }
    if (server.hasArg("mushroom_humidity")) {
      mushroom.targetHumidity = server.arg("mushroom_humidity").toFloat();
      updated = true;
    }
    if (server.hasArg("mushroom_moisture")) {
      mushroom.targetMoisture = server.arg("mushroom_moisture").toInt();
      updated = true;
    }
    
    if (updated) {
      saveSettings();
      systemLCD.showDebug("Settings", "Updated");
      server.send(200, "application/json", "{\"status\":\"settings_updated\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"no_valid_parameters\"}");
    }
  });
  
  server.on("/api/maintenance", HTTP_POST, []() {
    if (server.hasArg("mode")) {
      systemMaintenanceMode = server.arg("mode") == "true";
      if (systemMaintenanceMode) {
        // Turn off all systems
        digitalWrite(MUSHROOM_MISTING_PIN, LOW);
        digitalWrite(MUSHROOM_HEATER_PIN, LOW);
        digitalWrite(MUSHROOM_FAN_PIN, LOW);
        systemLCD.showAlert("MAINTENANCE", 3000);
      } else {
        systemLCD.showDebug("Maintenance", "Disabled");
      }
      String status = systemMaintenanceMode ? "enabled" : "disabled";
      server.send(200, "application/json", "{\"status\":\"maintenance_" + status + "\"}");
    }
  });
  
  server.on("/api/stats", HTTP_GET, []() {
    JsonDocument doc;
    doc["today"] = todayStats.date;
    doc["avg_temperature"] = todayStats.avgTemperature;
    doc["avg_humidity"] = todayStats.avgHumidity;
    doc["avg_co2"] = todayStats.avgCO2;
    doc["avg_ph"] = todayStats.avgPH;
    doc["watering_cycles"] = todayStats.wateringCycles;
    doc["total_watering_time"] = todayStats.totalWateringTime;
    doc["alerts"] = todayStats.alerts;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.begin();
  Serial.println("Web server started on port 80");
  systemLCD.showDebug("Web Server", "Port 80 Ready");
}

void setupBlynk() {
  if (isWiFiConnected) {
    systemLCD.showDebug("Blynk", "Connecting...");
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
    Serial.println("Blynk connected");
    systemLCD.showDebug("Blynk", "Connected");
    
    // Send initial data to Blynk
    Blynk.virtualWrite(V1, ambientTemperature);
    Blynk.virtualWrite(V2, ambientHumidity);
    Blynk.virtualWrite(V3, co2Level);
    Blynk.virtualWrite(V4, phLevel);
    Blynk.virtualWrite(V5, waterLevel);
    Blynk.virtualWrite(V10, mushroom.currentMoisture);
  }
}

void setupOTA() {
  if (isWiFiConnected) {
    ArduinoOTA.setHostname("mushroom-system");
    ArduinoOTA.setPassword("rdtrc2024");
    
    ArduinoOTA.onStart([]() {
      String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
      Serial.println("OTA Update started: " + type);
      systemMaintenanceMode = true;
      systemLCD.showAlert("OTA UPDATE", 2000);
    });
    
    ArduinoOTA.onEnd([]() {
      Serial.println("\nOTA Update completed");
      systemMaintenanceMode = false;
      systemLCD.showDebug("OTA Complete", "Restarting...");
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      int percent = (progress / (total / 100));
      Serial.printf("OTA Progress: %u%%\r", percent);
      if (percent % 10 == 0) { // Update LCD every 10%
        systemLCD.showDebug("OTA Update", String(percent) + "%");
      }
    });
    
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("OTA Error[%u]: ", error);
      systemMaintenanceMode = false;
      systemLCD.showAlert("OTA ERROR", 3000);
    });
    
    ArduinoOTA.begin();
    Serial.println("OTA updates enabled");
    systemLCD.showDebug("OTA", "Enabled");
  }
}

void handleSystemLoop() {
  // Status LED heartbeat
  static bool ledState = false;
  if (millis() - lastHeartbeat > (systemMaintenanceMode ? 200 : 2000)) {
    ledState = !ledState;
    digitalWrite(STATUS_LED_PIN, ledState);
    lastHeartbeat = millis();
  }
  
  // Check WiFi connection
  if (!isWiFiConnected && WiFi.status() == WL_CONNECTED) {
    isWiFiConnected = true;
    Serial.println("WiFi reconnected");
    systemLCD.showDebug("WiFi", "Reconnected");
    setupBlynk();
  } else if (isWiFiConnected && WiFi.status() != WL_CONNECTED) {
    isWiFiConnected = false;
    Serial.println("WiFi disconnected");
    systemLCD.showDebug("WiFi", "Disconnected");
  }
  
  // Update time
  if (isWiFiConnected) {
    timeClient.update();
  }
}

void handleLCDControls() {
  // Handle LCD navigation button
  static bool lastLCDButtonState = HIGH;
  static unsigned long lastLCDButtonPress = 0;
  
  bool currentLCDButtonState = digitalRead(LCD_NEXT_BUTTON_PIN);
  
  if (currentLCDButtonState == LOW && lastLCDButtonState == HIGH) {
    if (millis() - lastLCDButtonPress > 200) { // Debounce
      systemLCD.nextPage();
      lastLCDButtonPress = millis();
    }
  }
  
  lastLCDButtonState = currentLCDButtonState;
}

void updateLCDDisplay() {
  // Update LCD status every 2 seconds
  if (millis() - lastLCDUpdate > 2000) {
    // Only update LCD if it's online
    if (lcdSensor.isOnline && systemLCD.isLCDConnected()) {
      systemLCD.updateStatus(
        SYSTEM_NAME,
        ambientTemperature,
        ambientHumidity,
        mushroom.currentMoisture,
        mushroom.growthPhase,
        isWiFiConnected,
        systemMaintenanceMode,
        todayStats.alerts
      );
      
      // Update LCD display
      systemLCD.update();
    } else {
      // LCD is offline - skip display updates
      Serial.println("LCD offline - skipping display updates");
    }
    
    lastLCDUpdate = millis();
  }
}

void controlEnvironment() {
  // Control mushroom environment
  if (mushroom.enabled) {
    controlMushrooms();
  }
}

void controlMushrooms() {
  // Temperature control
  if (ambientTemperature < mushroom.targetTemp - 1.0) {
    if (!mushroom.heatingActive) {
      mushroom.heatingActive = true;
      digitalWrite(MUSHROOM_HEATER_PIN, HIGH);
      Serial.println("Mushroom heater ON");
      systemLCD.showDebug("Control", "Heater ON");
    }
  } else if (ambientTemperature > mushroom.targetTemp + 1.0) {
    if (mushroom.heatingActive) {
      mushroom.heatingActive = false;
      digitalWrite(MUSHROOM_HEATER_PIN, LOW);
      Serial.println("Mushroom heater OFF");
      systemLCD.showDebug("Control", "Heater OFF");
    }
  }
  
  // Humidity control (misting)
  if (ambientHumidity < mushroom.targetHumidity - 5.0) {
    if (!mushroom.wateringActive) {
      mushroom.wateringActive = true;
      digitalWrite(MUSHROOM_MISTING_PIN, HIGH);
      Serial.println("Mushroom misting ON");
      systemLCD.showDebug("Control", "Misting ON");
      todayStats.wateringCycles++;
    }
  } else if (ambientHumidity > mushroom.targetHumidity + 5.0) {
    if (mushroom.wateringActive) {
      mushroom.wateringActive = false;
      digitalWrite(MUSHROOM_MISTING_PIN, LOW);
      Serial.println("Mushroom misting OFF");
      systemLCD.showDebug("Control", "Misting OFF");
    }
  }
  
  // Ventilation control
  bool needsVentilation = false;
  if (mushroom.growthPhase == "Colonization" || 
      mushroom.growthPhase == "Pinning" || 
      mushroom.growthPhase == "Fruiting") {
    needsVentilation = true;
  }
  
  if (co2Level > HIGH_CO2_THRESHOLD) {
    needsVentilation = true;
  }
  
  if (needsVentilation && !mushroom.fanActive) {
    mushroom.fanActive = true;
    digitalWrite(MUSHROOM_FAN_PIN, HIGH);
    Serial.println("Mushroom fan ON");
    systemLCD.showDebug("Control", "Fan ON");
  } else if (!needsVentilation && mushroom.fanActive) {
    mushroom.fanActive = false;
    digitalWrite(MUSHROOM_FAN_PIN, LOW);
    Serial.println("Mushroom fan OFF");
    systemLCD.showDebug("Control", "Fan OFF");
  }
}

void readSensors() {
  // Read DHT sensor
  if (dhtSensor.isOnline) {
    ambientTemperature = dht.readTemperature();
    ambientHumidity = dht.readHumidity();
    
    if (isnan(ambientTemperature) || isnan(ambientHumidity)) {
      handleSensorError(-1, dhtSensor.sensorName);
      ambientTemperature = 0;
      ambientHumidity = 0;
    } else {
      updateSensorStatus(-1, true, ambientTemperature);
    }
  }
  
  // Read CO2 sensor
  if (co2Sensor.isOnline) {
    int co2Raw = analogRead(CO2_SENSOR_PIN);
    co2Level = map(co2Raw, 0, 4095, 400, 2000); // Map to ppm range
    updateSensorStatus(1, true, co2Level);
  }
  
  // Read pH sensor
  if (phSensor.isOnline) {
    int phRaw = analogRead(PH_SENSOR_PIN);
    phLevel = map(phRaw, 0, 4095, 4.0 * 100, 10.0 * 100) / 100.0; // Map to pH range
    updateSensorStatus(2, true, phLevel);
  }
  
  // Read EC sensor
  if (ecSensor.isOnline) {
    int ecRaw = analogRead(EC_SENSOR_PIN);
    ecLevel = map(ecRaw, 0, 4095, 0, 5); // Convert to mS/cm
    updateSensorStatus(3, true, ecLevel);
  }
  
  // Read light sensor
  if (lightSensor.isOnline) {
    lightLevel = analogRead(LIGHT_SENSOR_PIN);
    isDaylight = lightLevel > 500;
    updateSensorStatus(4, true, lightLevel);
  }
  
  // Read air quality sensor
  if (airQualitySensor.isOnline) {
    int aqRaw = analogRead(AIR_QUALITY_SENSOR_PIN);
    airQualityLevel = map(aqRaw, 0, 4095, 0, 100); // Convert to percentage
    updateSensorStatus(5, true, airQualityLevel);
  }
  
  // Read water level sensor
  if (waterLevelSensor.isOnline) {
    int wlRaw = analogRead(WATER_LEVEL_SENSOR_PIN);
    waterLevel = map(wlRaw, 0, 4095, 0, 100); // Convert to percentage
    updateSensorStatus(6, true, waterLevel);
  }
  
  // Read flow sensor
  if (flowSensor.isOnline) {
    int flowRaw = analogRead(FLOW_SENSOR_PIN);
    flowRate = map(flowRaw, 0, 4095, 0, 10); // Convert to L/min
    updateSensorStatus(7, true, flowRate);
  }
  
  // Read soil moisture sensor
  if (soilSensor.isOnline) {
    int mushroomRaw = analogRead(mushroom.soilPin);
    mushroom.currentMoisture = map(mushroomRaw, 4095, 0, 0, 100); // Invert for dry=low, wet=high
    if (mushroom.currentMoisture < 0) mushroom.currentMoisture = 0;
    if (mushroom.currentMoisture > 100) mushroom.currentMoisture = 100;
    updateSensorStatus(8, true, mushroom.currentMoisture);
  }
  
  // Update zone temperature and humidity (assuming ambient for now)
  mushroom.currentTemp = ambientTemperature;
  mushroom.currentHumidity = ambientHumidity;
  
  // Check sensor status and apply graceful degradation
  checkSensorStatus();
  gracefulDegradation();
  
  // Update Blynk with sensor data (only if sensors are online)
  if (isWiFiConnected) {
    if (dhtSensor.isOnline) {
      Blynk.virtualWrite(V1, ambientTemperature);
      Blynk.virtualWrite(V2, ambientHumidity);
    }
    if (co2Sensor.isOnline) {
      Blynk.virtualWrite(V3, co2Level);
    }
    if (phSensor.isOnline) {
      Blynk.virtualWrite(V4, phLevel);
    }
    if (ecSensor.isOnline) {
      Blynk.virtualWrite(V5, ecLevel);
    }
    if (waterLevelSensor.isOnline) {
      Blynk.virtualWrite(V6, waterLevel);
    }
    if (lightSensor.isOnline) {
      Blynk.virtualWrite(V7, lightLevel);
    }
    if (airQualitySensor.isOnline) {
      Blynk.virtualWrite(V8, airQualityLevel);
    }
    if (flowSensor.isOnline) {
      Blynk.virtualWrite(V9, flowRate);
    }
    Blynk.virtualWrite(V10, mushroom.currentMoisture);
  }
}

void checkAlerts() {
  // Low water alert
  if (waterLevel < LOW_WATER_THRESHOLD) {
    String alertMsg = "Low Water Level Alert!\n";
    alertMsg += "Current Level: " + String(waterLevel) + "cm\n";
    alertMsg += "Please refill the water tank.";
    sendLineNotification(alertMsg);
    systemLCD.showAlert("LOW WATER");
    todayStats.alerts++;
  }
  
  // High CO2 alert
  if (co2Level > HIGH_CO2_THRESHOLD) {
    String alertMsg = "High CO2 Level Alert!\n";
    alertMsg += "Current Level: " + String(co2Level) + "ppm\n";
    alertMsg += "Ventilation activated.";
    sendLineNotification(alertMsg);
    systemLCD.showAlert("HIGH CO2");
    todayStats.alerts++;
  }
  
  // pH out of range alert
  if (phLevel < OPTIMAL_PH_MIN || phLevel > OPTIMAL_PH_MAX) {
    String alertMsg = "pH Level Alert!\n";
    alertMsg += "Current pH: " + String(phLevel) + "\n";
    alertMsg += "Optimal range: " + String(OPTIMAL_PH_MIN) + "-" + String(OPTIMAL_PH_MAX);
    sendLineNotification(alertMsg);
    systemLCD.showAlert("pH ALERT");
    todayStats.alerts++;
  }
  
  // Temperature alerts
  if (ambientTemperature > 35.0) {
    String alertMsg = "High Temperature Alert!\n";
    alertMsg += "Current: " + String(ambientTemperature) + "C\n";
    alertMsg += "Check cooling systems.";
    sendLineNotification(alertMsg);
    systemLCD.showAlert("HIGH TEMP");
    todayStats.alerts++;
  } else if (ambientTemperature < 10.0) {
    String alertMsg = "Low Temperature Alert!\n";
    alertMsg += "Current: " + String(ambientTemperature) + "C\n";
    alertMsg += "Check heating systems.";
    sendLineNotification(alertMsg);
    systemLCD.showAlert("LOW TEMP");
    todayStats.alerts++;
  }
}

void updateGrowthPhases() {
  // Update mushroom growth phase
  for (int i = 0; i < 4; i++) {
    if (mushroomPhases[i].name == mushroom.growthPhase) {
      mushroom.daysInPhase++;
      if (mushroom.daysInPhase >= mushroomPhases[i].duration && i < 3) {
        mushroom.growthPhase = mushroomPhases[i + 1].name;
        mushroom.daysInPhase = 0;
        
        // Update targets based on new phase
        mushroom.targetTemp = (mushroomPhases[i + 1].tempRange[0] + mushroomPhases[i + 1].tempRange[1]) / 2;
        mushroom.targetHumidity = (mushroomPhases[i + 1].humidityRange[0] + mushroomPhases[i + 1].humidityRange[1]) / 2;
        mushroom.targetMoisture = (mushroomPhases[i + 1].moistureRange[0] + mushroomPhases[i + 1].moistureRange[1]) / 2;
        
        String phaseMsg = "Mushroom Growth Phase Updated!\n";
        phaseMsg += "New Phase: " + mushroom.growthPhase + "\n";
        phaseMsg += "Target Temp: " + String(mushroom.targetTemp) + "C\n";
        phaseMsg += "Target Humidity: " + String(mushroom.targetHumidity) + "%\n";
        phaseMsg += "Description: " + mushroomPhases[i + 1].description;
        sendLineNotification(phaseMsg);
        
        systemLCD.showMessage("Phase Update", mushroom.growthPhase, 5000);
        
        saveSettings();
      }
      break;
    }
  }
}

void handleManualControls() {
  // Manual button for emergency stop
  static bool lastButtonState = HIGH;
  static unsigned long lastButtonPress = 0;
  
  bool currentButtonState = digitalRead(MANUAL_BUTTON_PIN);
  
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    if (millis() - lastButtonPress > 1000) { // Debounce
      Serial.println("Manual emergency stop button pressed");
      
      // Turn off all systems
      digitalWrite(MUSHROOM_MISTING_PIN, LOW);
      digitalWrite(MUSHROOM_HEATER_PIN, LOW);
      digitalWrite(MUSHROOM_FAN_PIN, LOW);
      
      systemMaintenanceMode = true;
      systemLCD.showAlert("EMERGENCY STOP", 5000);
      
      sendLineNotification("Emergency Stop Activated!\nAll mushroom growing systems have been stopped.");
      
      lastButtonPress = millis();
    }
  }
  
  lastButtonState = currentButtonState;
  
  // Reset button (hold for 5 seconds)
  static unsigned long resetButtonPressTime = 0;
  static bool resetButtonPressed = false;
  
  if (digitalRead(RESET_BUTTON_PIN) == LOW) {
    if (!resetButtonPressed) {
      resetButtonPressTime = millis();
      resetButtonPressed = true;
      systemLCD.showDebug("Reset", "Hold 5 sec...");
    } else if (millis() - resetButtonPressTime > 5000) {
      Serial.println("System reset initiated...");
      systemLCD.showMessage("RESTARTING", "Please wait...", 3000);
      sendLineNotification("Mushroom growing system reset initiated");
      delay(1000);
      ESP.restart();
    } else if (millis() - resetButtonPressTime > 3000) {
      systemLCD.showDebug("Reset", "2 more sec...");
    }
  } else {
    resetButtonPressed = false;
  }
}

void logData() {
  JsonDocument doc;
  doc["timestamp"] = timeClient.getEpochTime();
  doc["uptime"] = millis() - bootTime;
  doc["ambient_temperature"] = ambientTemperature;
  doc["ambient_humidity"] = ambientHumidity;
  doc["co2_level"] = co2Level;
  doc["ph_level"] = phLevel;
  doc["light_level"] = lightLevel;
  doc["water_level"] = waterLevel;
  doc["wifi_signal"] = WiFi.RSSI();
  doc["free_memory"] = ESP.getFreeHeap();
  doc["lcd_connected"] = systemLCD.isLCDConnected();
  
  // Zone data
  JsonObject mushroomObj = doc.createNestedObject("mushroom");
  mushroomObj["moisture"] = mushroom.currentMoisture;
  mushroomObj["growth_phase"] = mushroom.growthPhase;
  mushroomObj["days_in_phase"] = mushroom.daysInPhase;
  mushroomObj["watering_active"] = mushroom.wateringActive;
  mushroomObj["heating_active"] = mushroom.heatingActive;
  mushroomObj["fan_active"] = mushroom.fanActive;
  
  String logData;
  serializeJson(doc, logData);
  
  File logFile = SPIFFS.open("/system.log", "a");
  if (logFile) {
    logFile.println(logData);
    logFile.close();
  }
  
  // Update daily averages
  static int readingCount = 0;
  readingCount++;
  
  todayStats.avgTemperature = ((todayStats.avgTemperature * (readingCount - 1)) + ambientTemperature) / readingCount;
  todayStats.avgHumidity = ((todayStats.avgHumidity * (readingCount - 1)) + ambientHumidity) / readingCount;
  todayStats.avgCO2 = ((todayStats.avgCO2 * (readingCount - 1)) + co2Level) / readingCount;
  todayStats.avgPH = ((todayStats.avgPH * (readingCount - 1)) + phLevel) / readingCount;
}

void saveSettings() {
  JsonDocument doc;
  
  // Mushroom settings
  JsonObject mushroomObj = doc.createNestedObject("mushroom");
  mushroomObj["target_temp"] = mushroom.targetTemp;
  mushroomObj["target_humidity"] = mushroom.targetHumidity;
  mushroomObj["target_moisture"] = mushroom.targetMoisture;
  mushroomObj["growth_phase"] = mushroom.growthPhase;
  mushroomObj["days_in_phase"] = mushroom.daysInPhase;
  mushroomObj["enabled"] = mushroom.enabled;
  
  File configFile = SPIFFS.open("/config.json", "w");
  if (configFile) {
    serializeJson(doc, configFile);
    configFile.close();
    Serial.println("Settings saved");
  }
}

void loadSettings() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (configFile) {
    JsonDocument doc;
    if (deserializeJson(doc, configFile) == DeserializationError::Ok) {
      // Load mushroom settings
      if (doc.containsKey("mushroom")) {
        JsonObject mushroomObj = doc["mushroom"];
        mushroom.targetTemp = mushroomObj["target_temp"] | MUSHROOM_OPTIMAL_TEMP;
        mushroom.targetHumidity = mushroomObj["target_humidity"] | MUSHROOM_OPTIMAL_HUMIDITY;
        mushroom.targetMoisture = mushroomObj["target_moisture"] | MUSHROOM_OPTIMAL_MOISTURE;
        mushroom.growthPhase = mushroomObj["growth_phase"] | "Inoculation";
        mushroom.daysInPhase = mushroomObj["days_in_phase"] | 0;
        mushroom.enabled = mushroomObj["enabled"] | true;
      }
      
      Serial.println("Settings loaded");
    }
    configFile.close();
  }
}

void performSystemMaintenance() {
  // Check system health
  if (ESP.getFreeHeap() < 20000) {
    Serial.println("Low memory warning: " + String(ESP.getFreeHeap()) + " bytes");
    systemLCD.showDebug("Low Memory", String(ESP.getFreeHeap()) + "B");
  }
  
  // Rotate log files if they get too large
  File logFile = SPIFFS.open("/system.log", "r");
  if (logFile && logFile.size() > 150000) { // 150KB limit
    logFile.close();
    SPIFFS.remove("/system.log.old");
    SPIFFS.rename("/system.log", "/system.log.old");
    Serial.println("Log file rotated");
    systemLCD.showDebug("Log Rotate", "Files rotated");
  }
  if (logFile) logFile.close();
  
  // Simple date update (you can enhance this with proper NTP date parsing)
  static unsigned long lastDateUpdate = 0;
  if (millis() - lastDateUpdate > 86400000) { // 24 hours
    lastDateUpdate = millis();
    Serial.println("New day started - stats reset");
    systemLCD.showDebug("New Day", "Stats Reset");
    
    // Reset daily stats
    todayStats.avgTemperature = 0;
    todayStats.avgHumidity = 0;
    todayStats.avgCO2 = 0;
    todayStats.avgPH = 0;
    todayStats.wateringCycles = 0;
    todayStats.totalWateringTime = 0;
    todayStats.alerts = 0;
  }
}

void sendLineNotification(String message) {
  if (!isWiFiConnected || strlen(lineToken) < 10) return;
  
  HTTPClient http;
  http.begin("https://notify-api.line.me/api/notify");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", "Bearer " + String(lineToken));
  
  String payload = "message=" + message;
  int httpResponseCode = http.POST(payload);
  
  if (httpResponseCode == 200) {
    Serial.println("LINE notification sent");
  } else {
    Serial.println("LINE notification failed: " + String(httpResponseCode));
  }
  
  http.end();
}

void handleWebInterface() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>RDTRC Mushroom Growing System with LCD</title>";
  html += "<meta charset=\"UTF-8\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background: #1a1a1a; color: #fff; margin: 0; padding: 20px; }";
  html += ".header { background: linear-gradient(135deg, #8e44ad, #3498db); padding: 20px; text-align: center; border-radius: 10px; margin-bottom: 20px; }";
  html += ".container { max-width: 1200px; margin: 0 auto; }";
  html += ".status-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; margin-bottom: 20px; }";
  html += ".status-card { background: #2d2d2d; padding: 15px; border-radius: 8px; text-align: center; }";
  html += ".btn { padding: 10px 20px; margin: 5px; border: none; border-radius: 5px; cursor: pointer; font-size: 14px; }";
  html += ".btn-success { background: #27ae60; color: white; }";
  html += ".btn-secondary { background: #555; color: white; }";
  html += ".btn-danger { background: #e74c3c; color: white; }";
  html += ".zone-card { background: #2d2d2d; padding: 20px; border-radius: 10px; margin-bottom: 20px; }";
  html += ".lcd-info { background: #2c3e50; padding: 15px; border-radius: 8px; margin-bottom: 20px; }";
  html += "</style>";
  html += "<script>";
  html += "function refreshStatus() {";
  html += "  fetch('/api/status').then(response => response.json()).then(data => {";
  html += "    document.getElementById('temperature').textContent = data.ambient_temperature.toFixed(1) + 'C';";
  html += "    document.getElementById('humidity').textContent = data.ambient_humidity.toFixed(1) + '%';";
  html += "    document.getElementById('moisture').textContent = data.mushroom.moisture + '%';";
  html += "    document.getElementById('phase').textContent = data.mushroom.growth_phase;";
  html += "    document.getElementById('lcd-status').textContent = data.lcd_connected ? 'Connected at ' + data.lcd_address : 'Not Connected';";
  html += "  });";
  html += "}";
  html += "function toggleControl(action) {";
  html += "  fetch('/api/control', {";
  html += "    method: 'POST',";
  html += "    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },";
  html += "    body: 'zone=mushroom&action=' + action + '&value=true'";
  html += "  }).then(() => setTimeout(refreshStatus, 1000));";
  html += "}";
  html += "setInterval(refreshStatus, 30000);";
  html += "window.onload = refreshStatus;";
  html += "</script>";
  html += "</head><body>";
  html += "<div class=\"container\">";
  html += "<div class=\"header\">";
  html += "<h1>RDTRC Mushroom Growing System</h1>";
  html += "<p>Complete Standalone System with LCD Display - v4.0</p>";
  html += "</div>";
  html += "<div class=\"lcd-info\">";
  html += "<h3>LCD Display Status</h3>";
  html += "<p id=\"lcd-status\">Loading...</p>";
  html += "<p>LCD shows: System info, Environmental data, Growth phase, Status & alerts</p>";
  html += "<p>Use the LCD button (Pin 26) to navigate between pages manually</p>";
  html += "</div>";
  html += "<div class=\"status-grid\">";
  html += "<div class=\"status-card\"><h3>Temperature</h3><div id=\"temperature\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Humidity</h3><div id=\"humidity\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Soil Moisture</h3><div id=\"moisture\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Growth Phase</h3><div id=\"phase\">Loading...</div></div>";
  html += "</div>";
  html += "<div class=\"zone-card\">";
  html += "<h2>Mushroom Zone Controls</h2>";
  html += "<button class=\"btn btn-success\" onclick=\"toggleControl('misting')\">Toggle Misting</button>";
  html += "<button class=\"btn btn-danger\" onclick=\"toggleControl('heating')\">Toggle Heating</button>";
  html += "<button class=\"btn btn-secondary\" onclick=\"toggleControl('fan')\">Toggle Fan</button>";
  html += "</div>";
  html += "</div>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// Blynk Virtual Pin Handlers
BLYNK_WRITE(V20) { // Mushroom misting control
  if (!systemMaintenanceMode) {
    mushroom.wateringActive = param.asInt() == 1;
    digitalWrite(MUSHROOM_MISTING_PIN, mushroom.wateringActive ? HIGH : LOW);
    systemLCD.showDebug("Blynk", mushroom.wateringActive ? "Mist ON" : "Mist OFF");
  }
}

BLYNK_WRITE(V21) { // Mushroom heater control
  if (!systemMaintenanceMode) {
    mushroom.heatingActive = param.asInt() == 1;
    digitalWrite(MUSHROOM_HEATER_PIN, mushroom.heatingActive ? HIGH : LOW);
    systemLCD.showDebug("Blynk", mushroom.heatingActive ? "Heat ON" : "Heat OFF");
  }
}

BLYNK_WRITE(V22) { // Mushroom fan control
  if (!systemMaintenanceMode) {
    mushroom.fanActive = param.asInt() == 1;
    digitalWrite(MUSHROOM_FAN_PIN, mushroom.fanActive ? HIGH : LOW);
    systemLCD.showDebug("Blynk", mushroom.fanActive ? "Fan ON" : "Fan OFF");
  }
}

BLYNK_WRITE(V30) { // Emergency stop
  if (param.asInt() == 1) {
    systemMaintenanceMode = true;
    // Turn off all systems
    digitalWrite(MUSHROOM_MISTING_PIN, LOW);
    digitalWrite(MUSHROOM_HEATER_PIN, LOW);
    digitalWrite(MUSHROOM_FAN_PIN, LOW);
    systemLCD.showAlert("BLYNK STOP", 3000);
  }
}

// Read-only pins for Blynk dashboard
BLYNK_READ(V1) { Blynk.virtualWrite(V1, ambientTemperature); }
BLYNK_READ(V2) { Blynk.virtualWrite(V2, ambientHumidity); }
BLYNK_READ(V3) { Blynk.virtualWrite(V3, co2Level); }
BLYNK_READ(V4) { Blynk.virtualWrite(V4, phLevel); }
BLYNK_READ(V5) { Blynk.virtualWrite(V5, ecLevel); }
BLYNK_READ(V6) { Blynk.virtualWrite(V6, waterLevel); }
BLYNK_READ(V7) { Blynk.virtualWrite(V7, lightLevel); }
BLYNK_READ(V8) { Blynk.virtualWrite(V8, airQualityLevel); }
BLYNK_READ(V9) { Blynk.virtualWrite(V9, flowRate); }
BLYNK_READ(V10) { Blynk.virtualWrite(V10, mushroom.currentMoisture); }

// Sensor Management Functions
void initializeSensors() {
  // Initialize DHT sensor
  dhtSensor.isOnline = true;
  dhtSensor.lastReading = millis();
  dhtSensor.lastValue = 0;
  dhtSensor.errorCount = 0;
  dhtSensor.sensorName = "DHT";
  
  // Initialize CO2 sensor
  co2Sensor.isOnline = true;
  co2Sensor.lastReading = millis();
  co2Sensor.lastValue = 0;
  co2Sensor.errorCount = 0;
  co2Sensor.sensorName = "CO2";
  
  // Initialize pH sensor
  phSensor.isOnline = true;
  phSensor.lastReading = millis();
  phSensor.lastValue = 0;
  phSensor.errorCount = 0;
  phSensor.sensorName = "pH";
  
  // Initialize EC sensor
  ecSensor.isOnline = true;
  ecSensor.lastReading = millis();
  ecSensor.lastValue = 0;
  ecSensor.errorCount = 0;
  ecSensor.sensorName = "EC";
  
  // Initialize light sensor
  lightSensor.isOnline = true;
  lightSensor.lastReading = millis();
  lightSensor.lastValue = 0;
  lightSensor.errorCount = 0;
  lightSensor.sensorName = "Light";
  
  // Initialize air quality sensor
  airQualitySensor.isOnline = true;
  airQualitySensor.lastReading = millis();
  airQualitySensor.lastValue = 0;
  airQualitySensor.errorCount = 0;
  airQualitySensor.sensorName = "AirQuality";
  
  // Initialize water level sensor
  waterLevelSensor.isOnline = true;
  waterLevelSensor.lastReading = millis();
  waterLevelSensor.lastValue = 0;
  waterLevelSensor.errorCount = 0;
  waterLevelSensor.sensorName = "WaterLevel";
  
  // Initialize flow sensor
  flowSensor.isOnline = true;
  flowSensor.lastReading = millis();
  flowSensor.lastValue = 0;
  flowSensor.errorCount = 0;
  flowSensor.sensorName = "Flow";
  
  // Initialize soil moisture sensor
  soilSensor.isOnline = true;
  soilSensor.lastReading = millis();
  soilSensor.lastValue = 0;
  soilSensor.errorCount = 0;
  soilSensor.sensorName = "Soil";
  
  // Initialize LCD sensor
  lcdSensor.isOnline = true;
  lcdSensor.lastReading = millis();
  lcdSensor.lastValue = 0;
  lcdSensor.errorCount = 0;
  lcdSensor.sensorName = "LCD";
  
  Serial.println("All sensors initialized");
}

void checkSensorStatus() {
  unsigned long currentTime = millis();
  
  // Check DHT sensor
  if (dhtSensor.isOnline && (currentTime - dhtSensor.lastReading > SENSOR_TIMEOUT)) {
    dhtSensor.isOnline = false;
    Serial.println("DHT sensor offline");
  }
  
  // Check CO2 sensor
  if (co2Sensor.isOnline && (currentTime - co2Sensor.lastReading > SENSOR_TIMEOUT)) {
    co2Sensor.isOnline = false;
    Serial.println("CO2 sensor offline");
  }
  
  // Check pH sensor
  if (phSensor.isOnline && (currentTime - phSensor.lastReading > SENSOR_TIMEOUT)) {
    phSensor.isOnline = false;
    Serial.println("pH sensor offline");
  }
  
  // Check EC sensor
  if (ecSensor.isOnline && (currentTime - ecSensor.lastReading > SENSOR_TIMEOUT)) {
    ecSensor.isOnline = false;
    Serial.println("EC sensor offline");
  }
  
  // Check light sensor
  if (lightSensor.isOnline && (currentTime - lightSensor.lastReading > SENSOR_TIMEOUT)) {
    lightSensor.isOnline = false;
    Serial.println("Light sensor offline");
  }
  
  // Check air quality sensor
  if (airQualitySensor.isOnline && (currentTime - airQualitySensor.lastReading > SENSOR_TIMEOUT)) {
    airQualitySensor.isOnline = false;
    Serial.println("Air quality sensor offline");
  }
  
  // Check water level sensor
  if (waterLevelSensor.isOnline && (currentTime - waterLevelSensor.lastReading > SENSOR_TIMEOUT)) {
    waterLevelSensor.isOnline = false;
    Serial.println("Water level sensor offline");
  }
  
  // Check flow sensor
  if (flowSensor.isOnline && (currentTime - flowSensor.lastReading > SENSOR_TIMEOUT)) {
    flowSensor.isOnline = false;
    Serial.println("Flow sensor offline");
  }
  
  // Check soil moisture sensor
  if (soilSensor.isOnline && (currentTime - soilSensor.lastReading > SENSOR_TIMEOUT)) {
    soilSensor.isOnline = false;
    Serial.println("Soil moisture sensor offline");
  }
  
  // Check LCD sensor
  if (lcdSensor.isOnline && !systemLCD.isLCDConnected()) {
    lcdSensor.isOnline = false;
    Serial.println("LCD sensor offline");
  }
}

void updateSensorStatus(int sensorId, bool success, float value) {
  SensorStatus* sensor = nullptr;
  
  switch (sensorId) {
    case -1: sensor = &dhtSensor; break;
    case 1: sensor = &co2Sensor; break;
    case 2: sensor = &phSensor; break;
    case 3: sensor = &ecSensor; break;
    case 4: sensor = &lightSensor; break;
    case 5: sensor = &airQualitySensor; break;
    case 6: sensor = &waterLevelSensor; break;
    case 7: sensor = &flowSensor; break;
    case 8: sensor = &soilSensor; break;
  }
  
  if (sensor) {
    if (success) {
      sensor->lastReading = millis();
      sensor->lastValue = value;
      sensor->errorCount = 0;
      if (!sensor->isOnline) {
        sensor->isOnline = true;
        Serial.println(sensor->sensorName + " sensor back online");
      }
    } else {
      sensor->errorCount++;
      if (sensor->errorCount >= 3) {
        sensor->isOnline = false;
        Serial.println(sensor->sensorName + " sensor marked offline");
      }
    }
  }
}

void handleSensorError(int sensorId, String sensorName) {
  Serial.println("Sensor error: " + sensorName);
  updateSensorStatus(sensorId, false, 0);
}

void gracefulDegradation() {
  // Check if critical sensors are offline
  bool criticalSensorsOffline = !dhtSensor.isOnline || !soilSensor.isOnline;
  
  if (criticalSensorsOffline) {
    Serial.println("Critical sensors offline - entering degraded mode");
    systemLCD.showDebug("Degraded Mode", "Critical Sensors Offline");
  }
  
  // Check if system can still operate
  if (!canOperateWithOfflineSensors()) {
    Serial.println("Too many sensors offline - system may not function properly");
    systemLCD.showAlert("SENSOR ERROR", 3000);
  }
}

bool canOperateWithOfflineSensors() {
  // Count online sensors
  int onlineSensors = 0;
  if (dhtSensor.isOnline) onlineSensors++;
  if (co2Sensor.isOnline) onlineSensors++;
  if (phSensor.isOnline) onlineSensors++;
  if (ecSensor.isOnline) onlineSensors++;
  if (lightSensor.isOnline) onlineSensors++;
  if (airQualitySensor.isOnline) onlineSensors++;
  if (waterLevelSensor.isOnline) onlineSensors++;
  if (flowSensor.isOnline) onlineSensors++;
  if (soilSensor.isOnline) onlineSensors++;
  if (lcdSensor.isOnline) onlineSensors++;
  
  // System can operate if at least 3 sensors are online
  return onlineSensors >= 3;
}

String getSensorStatusString() {
  String status = "Sensors: ";
  status += dhtSensor.isOnline ? "D1" : "D0";
  status += co2Sensor.isOnline ? " CO21" : " CO20";
  status += phSensor.isOnline ? " pH1" : " pH0";
  status += ecSensor.isOnline ? " EC1" : " EC0";
  status += lightSensor.isOnline ? " L1" : " L0";
  status += airQualitySensor.isOnline ? " AQ1" : " AQ0";
  status += waterLevelSensor.isOnline ? " WL1" : " WL0";
  status += flowSensor.isOnline ? " F1" : " F0";
  status += soilSensor.isOnline ? " S1" : " S0";
  status += lcdSensor.isOnline ? " LCD1" : " LCD0";
  return status;
}
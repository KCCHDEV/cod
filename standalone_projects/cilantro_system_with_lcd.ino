/*
 * RDTRC Complete Cilantro Growing System with LCD - Standalone Version
 * Version: 4.0 - Independent System with LCD I2C 16x2 Support
 * Firmware made by: RDTRC
 * Updated: 2024
 * 
 * Features:
 * - Complete standalone cilantro growing system
 * - LCD I2C 16x2 display with auto address detection
 * - Advanced environmental control
 * - Soil moisture monitoring
 * - Temperature, humidity, CO2, and pH monitoring
 * - LED grow lights with timer control
 * - Automated watering system
 * - Ventilation fan control
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
#define BLYNK_TEMPLATE_NAME "Cilantro_Growing_System"
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
#define SYSTEM_NAME "Cilantro System"
#define DEVICE_ID "RDTRC_CILANTRO"

// Network Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* hotspot_ssid = "RDTRC_Cilantro";
const char* hotspot_password = "rdtrc123";

// LINE Notify Configuration
const char* lineToken = "YOUR_LINE_NOTIFY_TOKEN";

// Pin Definitions
#define DHT_PIN 22
#define DHT_TYPE DHT22
#define CO2_SENSOR_PIN 23
#define PH_SENSOR_PIN 32
#define LIGHT_SENSOR_PIN 33

// Cilantro Zone Pins
#define CILANTRO_SOIL_PIN 35
#define CILANTRO_WATER_PUMP_PIN 5
#define CILANTRO_GROW_LIGHT_PIN 17
#define CILANTRO_FAN_PIN 16

// System Control Pins
#define STATUS_LED_PIN 2
#define BUZZER_PIN 4
#define RESET_BUTTON_PIN 0
#define MANUAL_BUTTON_PIN 27
#define LCD_NEXT_BUTTON_PIN 26  // Button to navigate LCD pages

// Water System Pins
#define WATER_LEVEL_TRIG_PIN 25
#define WATER_LEVEL_ECHO_PIN 14

// I2C Pins (ESP32 default: SDA=21, SCL=22)
#define I2C_SDA 21
#define I2C_SCL 22

// Growing Configuration
#define CILANTRO_OPTIMAL_TEMP 20.0       // °C
#define CILANTRO_OPTIMAL_HUMIDITY 60.0   // %
#define CILANTRO_OPTIMAL_MOISTURE 50     // %

#define WATER_TANK_HEIGHT 40             // cm
#define LOW_WATER_THRESHOLD 8            // cm
#define HIGH_CO2_THRESHOLD 1000          // ppm
#define OPTIMAL_PH_MIN 6.0
#define OPTIMAL_PH_MAX 7.0

// System Objects
WebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); // UTC+7 Thailand
DHT dht(DHT_PIN, DHT_TYPE);
RDTRC_LCD systemLCD;

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
  bool fanActive;
  bool lightActive;
  unsigned long lastWatered;
  unsigned long totalWateringTime;
  String growthPhase;
  int daysInPhase;
  bool enabled;
};

GrowingZone cilantro = {
  "Cilantro Zone", CILANTRO_SOIL_PIN, CILANTRO_WATER_PUMP_PIN,
  CILANTRO_OPTIMAL_TEMP, CILANTRO_OPTIMAL_HUMIDITY, CILANTRO_OPTIMAL_MOISTURE,
  0, 0, 0, false, false, false, 0, 0, "Germination", 0, true
};

// Environmental Variables
float ambientTemperature = 0;
float ambientHumidity = 0;
int co2Level = 0;
float phLevel = 7.0;
int lightLevel = 0;
float waterLevel = 0;
bool isDaylight = true;
bool systemMaintenanceMode = false;

// Growth Phase Schedules
struct GrowthPhase {
  String name;
  int duration; // days
  float tempRange[2]; // min, max
  float humidityRange[2]; // min, max
  int moistureRange[2]; // min, max
  bool needsLight;
  int lightHours; // hours per day
  bool needsVentilation;
  String description;
};

// Cilantro Growth Phases  
GrowthPhase cilantroPhases[4] = {
  {"Germination", 7, {18, 22}, {70, 80}, {60, 70}, true, 12, false, "Seed germination"},
  {"Seedling", 14, {20, 24}, {60, 70}, {50, 60}, true, 14, true, "Seedling development"},
  {"Vegetative", 21, {18, 25}, {50, 65}, {45, 55}, true, 16, true, "Leaf growth phase"},
  {"Harvest", 30, {16, 22}, {50, 60}, {40, 50}, true, 14, true, "Harvest ready"}
};

// Lighting Schedule
struct LightSchedule {
  int startHour;
  int startMinute;
  int duration; // minutes
  bool enabled;
};

LightSchedule cilantroLights[3] = {
  {6, 0, 360, true},   // 6:00 AM - 6 hours
  {14, 0, 300, true},  // 2:00 PM - 5 hours  
  {20, 0, 180, true}   // 8:00 PM - 3 hours
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
  int lightHours;
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
void controlCilantro();
void controlLighting();
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

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  bootTime = millis();
  displayBootScreen();
  setupSystem();
  
  Serial.println("RDTRC Cilantro Growing System with LCD Ready!");
  Serial.println("Web Interface: http://cilantro-system.local");
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
  Serial.println("RDTRC Complete Cilantro Growing System with LCD");
  Serial.println("");
  Serial.println("Firmware made by: " + String(FIRMWARE_MAKER));
  Serial.println("Version: " + String(FIRMWARE_VERSION));
  Serial.println("System: " + String(SYSTEM_NAME));
  Serial.println("");
  Serial.println("Features:");
  Serial.println("* LCD I2C 16x2 Display with Auto Detection");
  Serial.println("* Advanced Environmental Monitoring");
  Serial.println("* LED Grow Light Control");
  Serial.println("* Automated Watering System");
  Serial.println("* Temperature & Humidity Control");
  Serial.println("* CO2 & pH Monitoring");
  Serial.println("* Growth Phase Management");
  Serial.println("* Web Interface");
  Serial.println("* Mobile App Control");
  Serial.println("* Smart Notifications");
  Serial.println("* Data Logging & Analytics");
  Serial.println("* OTA Updates");
  Serial.println("");
  Serial.println("Initializing Cilantro Growing Environment...");
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
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(WATER_LEVEL_TRIG_PIN, OUTPUT);
  pinMode(WATER_LEVEL_ECHO_PIN, INPUT);
  
  // Cilantro zone pins
  pinMode(CILANTRO_WATER_PUMP_PIN, OUTPUT);
  pinMode(CILANTRO_GROW_LIGHT_PIN, OUTPUT);
  pinMode(CILANTRO_FAN_PIN, OUTPUT);
  
  // Initialize all outputs to OFF
  digitalWrite(CILANTRO_WATER_PUMP_PIN, LOW);
  digitalWrite(CILANTRO_GROW_LIGHT_PIN, LOW);
  digitalWrite(CILANTRO_FAN_PIN, LOW);
  
  // Setup LCD first
  setupLCD();
  
  // Initialize DHT sensor
  dht.begin();
  systemLCD.showDebug("DHT22 Init", "Sensor Ready");
  
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
  todayStats.date = "01/01/2024";
  todayStats.avgTemperature = 0;
  todayStats.avgHumidity = 0;
  todayStats.avgCO2 = 0;
  todayStats.avgPH = 0;
  todayStats.wateringCycles = 0;
  todayStats.totalWateringTime = 0;
  todayStats.lightHours = 0;
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
  String startupMsg = "RDTRC Cilantro Growing System Started!\n";
  startupMsg += "System: " + String(SYSTEM_NAME) + "\n";
  startupMsg += "Version: " + String(FIRMWARE_VERSION) + "\n";
  startupMsg += "LCD: " + (systemLCD.isLCDConnected() ? "Connected" : "Not Found") + "\n";
  startupMsg += "Water Level: " + String(waterLevel) + "cm\n";
  startupMsg += "Temperature: " + String(ambientTemperature) + "C\n";
  startupMsg += "Humidity: " + String(ambientHumidity) + "%\n";
  startupMsg += "Cilantro Phase: " + cilantro.growthPhase + "\n";
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
    if (MDNS.begin("cilantro-system")) {
      Serial.println("mDNS responder started: cilantro-system.local");
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
  if (MDNS.begin("cilantro-system")) {
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
    
    // Cilantro zone data
    JsonObject cilantroObj = doc.createNestedObject("cilantro");
    cilantroObj["name"] = cilantro.name;
    cilantroObj["moisture"] = cilantro.currentMoisture;
    cilantroObj["target_temp"] = cilantro.targetTemp;
    cilantroObj["target_humidity"] = cilantro.targetHumidity;
    cilantroObj["target_moisture"] = cilantro.targetMoisture;
    cilantroObj["watering_active"] = cilantro.wateringActive;
    cilantroObj["light_active"] = cilantro.lightActive;
    cilantroObj["fan_active"] = cilantro.fanActive;
    cilantroObj["growth_phase"] = cilantro.growthPhase;
    cilantroObj["days_in_phase"] = cilantro.daysInPhase;
    cilantroObj["enabled"] = cilantro.enabled;
    
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
    
    if (zone == "cilantro") {
      String lcdAction = "";
      if (action == "watering") {
        cilantro.wateringActive = value == "true";
        digitalWrite(CILANTRO_WATER_PUMP_PIN, cilantro.wateringActive ? HIGH : LOW);
        lcdAction = cilantro.wateringActive ? "Watering ON" : "Watering OFF";
      } else if (action == "light") {
        cilantro.lightActive = value == "true";
        digitalWrite(CILANTRO_GROW_LIGHT_PIN, cilantro.lightActive ? HIGH : LOW);
        lcdAction = cilantro.lightActive ? "Light ON" : "Light OFF";
      } else if (action == "fan") {
        cilantro.fanActive = value == "true";
        digitalWrite(CILANTRO_FAN_PIN, cilantro.fanActive ? HIGH : LOW);
        lcdAction = cilantro.fanActive ? "Fan ON" : "Fan OFF";
      }
      
      if (lcdAction != "") {
        systemLCD.showDebug("Control", lcdAction);
        systemLCD.updateStatus(SYSTEM_NAME, ambientTemperature, ambientHumidity, 
                              cilantro.currentMoisture, cilantro.growthPhase, 
                              isWiFiConnected, systemMaintenanceMode, todayStats.alerts, lcdAction);
      }
    }
    
    server.send(200, "application/json", "{\"status\":\"control_updated\"}");
  });
  
  server.on("/api/phase", HTTP_POST, []() {
    String zone = server.arg("zone");
    String phase = server.arg("phase");
    
    if (zone == "cilantro" && phase.length() > 0) {
      cilantro.growthPhase = phase;
      cilantro.daysInPhase = 0;
      saveSettings();
      
      systemLCD.showDebug("Phase Change", phase);
      systemLCD.updateStatus(SYSTEM_NAME, ambientTemperature, ambientHumidity, 
                            cilantro.currentMoisture, cilantro.growthPhase, 
                            isWiFiConnected, systemMaintenanceMode, todayStats.alerts, "Phase: " + phase);
      
      server.send(200, "application/json", "{\"status\":\"cilantro_phase_updated\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"invalid_parameters\"}");
    }
  });
  
  server.on("/api/settings", HTTP_POST, []() {
    bool updated = false;
    
    if (server.hasArg("cilantro_temp")) {
      cilantro.targetTemp = server.arg("cilantro_temp").toFloat();
      updated = true;
    }
    if (server.hasArg("cilantro_humidity")) {
      cilantro.targetHumidity = server.arg("cilantro_humidity").toFloat();
      updated = true;
    }
    if (server.hasArg("cilantro_moisture")) {
      cilantro.targetMoisture = server.arg("cilantro_moisture").toInt();
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
        digitalWrite(CILANTRO_WATER_PUMP_PIN, LOW);
        digitalWrite(CILANTRO_GROW_LIGHT_PIN, LOW);
        digitalWrite(CILANTRO_FAN_PIN, LOW);
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
    doc["light_hours"] = todayStats.lightHours;
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
    Blynk.virtualWrite(V11, cilantro.currentMoisture);
  }
}

void setupOTA() {
  if (isWiFiConnected) {
    ArduinoOTA.setHostname("cilantro-system");
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
    systemLCD.updateStatus(
      SYSTEM_NAME,
      ambientTemperature,
      ambientHumidity,
      cilantro.currentMoisture,
      cilantro.growthPhase,
      isWiFiConnected,
      systemMaintenanceMode,
      todayStats.alerts
    );
    
    // Update LCD display
    systemLCD.update();
    
    lastLCDUpdate = millis();
  }
}

void controlEnvironment() {
  // Control cilantro environment
  if (cilantro.enabled) {
    controlCilantro();
  }
  
  // Control lighting
  controlLighting();
}

void controlCilantro() {
  // Soil moisture control
  if (cilantro.currentMoisture < cilantro.targetMoisture - 5) {
    if (!cilantro.wateringActive && waterLevel > LOW_WATER_THRESHOLD) {
      cilantro.wateringActive = true;
      digitalWrite(CILANTRO_WATER_PUMP_PIN, HIGH);
      cilantro.lastWatered = millis();
      Serial.println("Cilantro watering ON");
      systemLCD.showDebug("Control", "Water ON");
      todayStats.wateringCycles++;
    }
  } else if (cilantro.currentMoisture > cilantro.targetMoisture + 5) {
    if (cilantro.wateringActive) {
      cilantro.wateringActive = false;
      digitalWrite(CILANTRO_WATER_PUMP_PIN, LOW);
      cilantro.totalWateringTime += millis() - cilantro.lastWatered;
      Serial.println("Cilantro watering OFF");
      systemLCD.showDebug("Control", "Water OFF");
    }
  }
  
  // Automatic watering timeout (safety)
  if (cilantro.wateringActive && (millis() - cilantro.lastWatered > 60000)) {
    cilantro.wateringActive = false;
    digitalWrite(CILANTRO_WATER_PUMP_PIN, LOW);
    Serial.println("Cilantro watering timeout - safety stop");
    systemLCD.showDebug("Safety", "Water Timeout");
  }
  
  // Ventilation control
  if (ambientTemperature > cilantro.targetTemp + 2.0 || co2Level > HIGH_CO2_THRESHOLD) {
    if (!cilantro.fanActive) {
      cilantro.fanActive = true;
      digitalWrite(CILANTRO_FAN_PIN, HIGH);
      Serial.println("Cilantro fan ON");
      systemLCD.showDebug("Control", "Fan ON");
    }
  } else if (ambientTemperature < cilantro.targetTemp - 2.0 && co2Level < HIGH_CO2_THRESHOLD) {
    if (cilantro.fanActive) {
      cilantro.fanActive = false;
      digitalWrite(CILANTRO_FAN_PIN, LOW);
      Serial.println("Cilantro fan OFF");
      systemLCD.showDebug("Control", "Fan OFF");
    }
  }
}

void controlLighting() {
  if (!cilantro.enabled) return;
  
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentTimeMinutes = currentHour * 60 + currentMinute;
  
  bool shouldLightBeOn = false;
  
  // Check each light schedule
  for (int i = 0; i < 3; i++) {
    if (cilantroLights[i].enabled) {
      int startTime = cilantroLights[i].startHour * 60 + cilantroLights[i].startMinute;
      int endTime = startTime + cilantroLights[i].duration;
      
      if (currentTimeMinutes >= startTime && currentTimeMinutes < endTime) {
        shouldLightBeOn = true;
        break;
      }
    }
  }
  
  // Only turn on light if cilantro needs light in current growth phase
  GrowthPhase* currentPhase = nullptr;
  for (int i = 0; i < 4; i++) {
    if (cilantroPhases[i].name == cilantro.growthPhase) {
      currentPhase = &cilantroPhases[i];
      break;
    }
  }
  
  if (currentPhase && !currentPhase->needsLight) {
    shouldLightBeOn = false;
  }
  
  if (shouldLightBeOn && !cilantro.lightActive) {
    cilantro.lightActive = true;
    digitalWrite(CILANTRO_GROW_LIGHT_PIN, HIGH);
    Serial.println("Cilantro grow light ON");
    systemLCD.showDebug("Control", "Light ON");
  } else if (!shouldLightBeOn && cilantro.lightActive) {
    cilantro.lightActive = false;
    digitalWrite(CILANTRO_GROW_LIGHT_PIN, LOW);
    Serial.println("Cilantro grow light OFF");
    systemLCD.showDebug("Control", "Light OFF");
  }
}

void readSensors() {
  // Read DHT sensor
  ambientTemperature = dht.readTemperature();
  ambientHumidity = dht.readHumidity();
  
  // Check for sensor errors
  if (isnan(ambientTemperature) || isnan(ambientHumidity)) {
    Serial.println("DHT sensor error");
    systemLCD.showDebug("DHT Error", "Check Sensor");
    ambientTemperature = 0;
    ambientHumidity = 0;
  }
  
  // Read CO2 sensor (analog approximation)
  int co2Raw = analogRead(CO2_SENSOR_PIN);
  co2Level = map(co2Raw, 0, 4095, 400, 2000); // Map to ppm range
  
  // Read pH sensor (analog approximation)
  int phRaw = analogRead(PH_SENSOR_PIN);
  phLevel = map(phRaw, 0, 4095, 4.0 * 100, 10.0 * 100) / 100.0; // Map to pH range
  
  // Read light sensor
  lightLevel = analogRead(LIGHT_SENSOR_PIN);
  isDaylight = lightLevel > 500;
  
  // Read water level using ultrasonic sensor
  digitalWrite(WATER_LEVEL_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(WATER_LEVEL_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(WATER_LEVEL_TRIG_PIN, LOW);
  
  long duration = pulseIn(WATER_LEVEL_ECHO_PIN, HIGH, 30000);
  if (duration > 0) {
    waterLevel = WATER_TANK_HEIGHT - (duration * 0.034 / 2);
    if (waterLevel < 0) waterLevel = 0;
    if (waterLevel > WATER_TANK_HEIGHT) waterLevel = WATER_TANK_HEIGHT;
  }
  
  // Read soil moisture sensor
  int cilantroRaw = analogRead(cilantro.soilPin);
  cilantro.currentMoisture = map(cilantroRaw, 4095, 0, 0, 100); // Invert for dry=low, wet=high
  if (cilantro.currentMoisture < 0) cilantro.currentMoisture = 0;
  if (cilantro.currentMoisture > 100) cilantro.currentMoisture = 100;
  
  // Update zone temperature and humidity (assuming ambient for now)
  cilantro.currentTemp = ambientTemperature;
  cilantro.currentHumidity = ambientHumidity;
  
  // Update Blynk with sensor data
  if (isWiFiConnected) {
    Blynk.virtualWrite(V1, ambientTemperature);
    Blynk.virtualWrite(V2, ambientHumidity);
    Blynk.virtualWrite(V3, co2Level);
    Blynk.virtualWrite(V4, phLevel);
    Blynk.virtualWrite(V5, waterLevel);
    Blynk.virtualWrite(V6, lightLevel);
    Blynk.virtualWrite(V11, cilantro.currentMoisture);
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
  // Update cilantro growth phase
  for (int i = 0; i < 4; i++) {
    if (cilantroPhases[i].name == cilantro.growthPhase) {
      cilantro.daysInPhase++;
      if (cilantro.daysInPhase >= cilantroPhases[i].duration && i < 3) {
        cilantro.growthPhase = cilantroPhases[i + 1].name;
        cilantro.daysInPhase = 0;
        
        // Update targets based on new phase
        cilantro.targetTemp = (cilantroPhases[i + 1].tempRange[0] + cilantroPhases[i + 1].tempRange[1]) / 2;
        cilantro.targetHumidity = (cilantroPhases[i + 1].humidityRange[0] + cilantroPhases[i + 1].humidityRange[1]) / 2;
        cilantro.targetMoisture = (cilantroPhases[i + 1].moistureRange[0] + cilantroPhases[i + 1].moistureRange[1]) / 2;
        
        String phaseMsg = "Cilantro Growth Phase Updated!\n";
        phaseMsg += "New Phase: " + cilantro.growthPhase + "\n";
        phaseMsg += "Target Temp: " + String(cilantro.targetTemp) + "C\n";
        phaseMsg += "Target Humidity: " + String(cilantro.targetHumidity) + "%\n";
        phaseMsg += "Light Hours: " + String(cilantroPhases[i + 1].lightHours) + "h\n";
        phaseMsg += "Description: " + cilantroPhases[i + 1].description;
        sendLineNotification(phaseMsg);
        
        systemLCD.showMessage("Phase Update", cilantro.growthPhase, 5000);
        
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
      digitalWrite(CILANTRO_WATER_PUMP_PIN, LOW);
      digitalWrite(CILANTRO_GROW_LIGHT_PIN, LOW);
      digitalWrite(CILANTRO_FAN_PIN, LOW);
      
      systemMaintenanceMode = true;
      systemLCD.showAlert("EMERGENCY STOP", 5000);
      
      sendLineNotification("Emergency Stop Activated!\nAll cilantro growing systems have been stopped.");
      
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
      sendLineNotification("Cilantro growing system reset initiated");
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
  JsonObject cilantroObj = doc.createNestedObject("cilantro");
  cilantroObj["moisture"] = cilantro.currentMoisture;
  cilantroObj["growth_phase"] = cilantro.growthPhase;
  cilantroObj["days_in_phase"] = cilantro.daysInPhase;
  cilantroObj["watering_active"] = cilantro.wateringActive;
  cilantroObj["light_active"] = cilantro.lightActive;
  cilantroObj["fan_active"] = cilantro.fanActive;
  
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
  
  // Cilantro settings
  JsonObject cilantroObj = doc.createNestedObject("cilantro");
  cilantroObj["target_temp"] = cilantro.targetTemp;
  cilantroObj["target_humidity"] = cilantro.targetHumidity;
  cilantroObj["target_moisture"] = cilantro.targetMoisture;
  cilantroObj["growth_phase"] = cilantro.growthPhase;
  cilantroObj["days_in_phase"] = cilantro.daysInPhase;
  cilantroObj["enabled"] = cilantro.enabled;
  
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
      // Load cilantro settings
      if (doc.containsKey("cilantro")) {
        JsonObject cilantroObj = doc["cilantro"];
        cilantro.targetTemp = cilantroObj["target_temp"] | CILANTRO_OPTIMAL_TEMP;
        cilantro.targetHumidity = cilantroObj["target_humidity"] | CILANTRO_OPTIMAL_HUMIDITY;
        cilantro.targetMoisture = cilantroObj["target_moisture"] | CILANTRO_OPTIMAL_MOISTURE;
        cilantro.growthPhase = cilantroObj["growth_phase"] | "Germination";
        cilantro.daysInPhase = cilantroObj["days_in_phase"] | 0;
        cilantro.enabled = cilantroObj["enabled"] | true;
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
  
  // Simple date update
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
    todayStats.lightHours = 0;
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
  html += "<title>RDTRC Cilantro Growing System with LCD</title>";
  html += "<meta charset=\"UTF-8\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background: #1a1a1a; color: #fff; margin: 0; padding: 20px; }";
  html += ".header { background: linear-gradient(135deg, #2ecc71, #3498db); padding: 20px; text-align: center; border-radius: 10px; margin-bottom: 20px; }";
  html += ".container { max-width: 1200px; margin: 0 auto; }";
  html += ".status-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; margin-bottom: 20px; }";
  html += ".status-card { background: #2d2d2d; padding: 15px; border-radius: 8px; text-align: center; }";
  html += ".btn { padding: 10px 20px; margin: 5px; border: none; border-radius: 5px; cursor: pointer; font-size: 14px; }";
  html += ".btn-success { background: #27ae60; color: white; }";
  html += ".btn-warning { background: #f39c12; color: white; }";
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
  html += "    document.getElementById('moisture').textContent = data.cilantro.moisture + '%';";
  html += "    document.getElementById('phase').textContent = data.cilantro.growth_phase;";
  html += "    document.getElementById('lcd-status').textContent = data.lcd_connected ? 'Connected at ' + data.lcd_address : 'Not Connected';";
  html += "  });";
  html += "}";
  html += "function toggleControl(action) {";
  html += "  fetch('/api/control', {";
  html += "    method: 'POST',";
  html += "    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },";
  html += "    body: 'zone=cilantro&action=' + action + '&value=true'";
  html += "  }).then(() => setTimeout(refreshStatus, 1000));";
  html += "}";
  html += "setInterval(refreshStatus, 30000);";
  html += "window.onload = refreshStatus;";
  html += "</script>";
  html += "</head><body>";
  html += "<div class=\"container\">";
  html += "<div class=\"header\">";
  html += "<h1>RDTRC Cilantro Growing System</h1>";
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
  html += "<h2>Cilantro Zone Controls</h2>";
  html += "<button class=\"btn btn-success\" onclick=\"toggleControl('watering')\">Toggle Watering</button>";
  html += "<button class=\"btn btn-warning\" onclick=\"toggleControl('light')\">Toggle Grow Light</button>";
  html += "<button class=\"btn btn-secondary\" onclick=\"toggleControl('fan')\">Toggle Fan</button>";
  html += "</div>";
  html += "</div>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// Blynk Virtual Pin Handlers
BLYNK_WRITE(V22) { // Cilantro watering control
  if (!systemMaintenanceMode) {
    cilantro.wateringActive = param.asInt() == 1;
    digitalWrite(CILANTRO_WATER_PUMP_PIN, cilantro.wateringActive ? HIGH : LOW);
    systemLCD.showDebug("Blynk", cilantro.wateringActive ? "Water ON" : "Water OFF");
  }
}

BLYNK_WRITE(V23) { // Cilantro grow light control
  if (!systemMaintenanceMode) {
    cilantro.lightActive = param.asInt() == 1;
    digitalWrite(CILANTRO_GROW_LIGHT_PIN, cilantro.lightActive ? HIGH : LOW);
    systemLCD.showDebug("Blynk", cilantro.lightActive ? "Light ON" : "Light OFF");
  }
}

BLYNK_WRITE(V24) { // Cilantro fan control
  if (!systemMaintenanceMode) {
    cilantro.fanActive = param.asInt() == 1;
    digitalWrite(CILANTRO_FAN_PIN, cilantro.fanActive ? HIGH : LOW);
    systemLCD.showDebug("Blynk", cilantro.fanActive ? "Fan ON" : "Fan OFF");
  }
}

BLYNK_WRITE(V30) { // Emergency stop
  if (param.asInt() == 1) {
    systemMaintenanceMode = true;
    // Turn off all systems
    digitalWrite(CILANTRO_WATER_PUMP_PIN, LOW);
    digitalWrite(CILANTRO_GROW_LIGHT_PIN, LOW);
    digitalWrite(CILANTRO_FAN_PIN, LOW);
    systemLCD.showAlert("BLYNK STOP", 3000);
  }
}

// Read-only pins for Blynk dashboard
BLYNK_READ(V1) { Blynk.virtualWrite(V1, ambientTemperature); }
BLYNK_READ(V2) { Blynk.virtualWrite(V2, ambientHumidity); }
BLYNK_READ(V3) { Blynk.virtualWrite(V3, co2Level); }
BLYNK_READ(V4) { Blynk.virtualWrite(V4, phLevel); }
BLYNK_READ(V5) { Blynk.virtualWrite(V5, waterLevel); }
BLYNK_READ(V6) { Blynk.virtualWrite(V6, lightLevel); }
BLYNK_READ(V11) { Blynk.virtualWrite(V11, cilantro.currentMoisture); }
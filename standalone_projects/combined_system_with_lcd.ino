/*
 * RDTRC Complete Mushroom & Cilantro Growing System with LCD - Standalone Version
 * Version: 4.0 - Independent System with LCD I2C 16x2 Support
 * Firmware made by: RDTRC
 * Updated: 2024
 * 
 * Features:
 * - Complete standalone mushroom and cilantro growing system
 * - LCD I2C 16x2 display with auto address detection
 * - Dual-zone growing (Mushroom zone + Cilantro zone)
 * - Advanced environmental control
 * - Soil moisture monitoring for both zones
 * - Temperature, humidity, CO2, and pH monitoring
 * - Automated misting system for mushrooms
 * - LED grow lights with timer control
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
#define BLYNK_TEMPLATE_NAME "Mushroom_Cilantro_System"
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
#define SYSTEM_NAME "Dual Growing System"
#define DEVICE_ID "RDTRC_DUAL_SYSTEM"

// Network Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* hotspot_ssid = "RDTRC_Growing";
const char* hotspot_password = "rdtrc123";

// LINE Notify Configuration
const char* lineToken = "YOUR_LINE_NOTIFY_TOKEN";

// Pin Definitions
#define DHT_PIN 22
#define DHT_TYPE DHT22
#define CO2_SENSOR_PIN 23
#define PH_SENSOR_PIN 32
#define LIGHT_SENSOR_PIN 33

// Mushroom Zone Pins
#define MUSHROOM_SOIL_PIN 34
#define MUSHROOM_MISTING_PIN 18
#define MUSHROOM_HEATER_PIN 19
#define MUSHROOM_FAN_PIN 21

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

// I2C Pins
#define I2C_SDA 21
#define I2C_SCL 22

// Growing Configuration
#define MUSHROOM_OPTIMAL_TEMP 22.0      // °C
#define MUSHROOM_OPTIMAL_HUMIDITY 85.0   // %
#define MUSHROOM_OPTIMAL_MOISTURE 70     // %
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
int currentLCDZone = 0; // 0 = mushroom, 1 = cilantro

// Growing Zones
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
  bool lightActive;
  unsigned long lastWatered;
  unsigned long totalWateringTime;
  String growthPhase;
  int daysInPhase;
  bool enabled;
};

GrowingZone mushroom = {
  "Mushroom Zone", MUSHROOM_SOIL_PIN, MUSHROOM_MISTING_PIN,
  MUSHROOM_OPTIMAL_TEMP, MUSHROOM_OPTIMAL_HUMIDITY, MUSHROOM_OPTIMAL_MOISTURE,
  0, 0, 0, false, false, false, false, 0, 0, "Inoculation", 0, true
};

GrowingZone cilantro = {
  "Cilantro Zone", CILANTRO_SOIL_PIN, CILANTRO_WATER_PUMP_PIN,
  CILANTRO_OPTIMAL_TEMP, CILANTRO_OPTIMAL_HUMIDITY, CILANTRO_OPTIMAL_MOISTURE,
  0, 0, 0, false, false, false, false, 0, 0, "Germination", 0, true
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

// Mushroom Growth Phases
GrowthPhase mushroomPhases[4] = {
  {"Inoculation", 14, {20, 24}, {80, 90}, {65, 75}, false, 0, false, "Spore inoculation phase"},
  {"Colonization", 21, {22, 25}, {75, 85}, {70, 80}, false, 0, true, "Mycelium colonization"},
  {"Pinning", 7, {18, 22}, {85, 95}, {75, 85}, false, 0, true, "Pin formation"},
  {"Fruiting", 14, {16, 20}, {80, 90}, {70, 80}, false, 0, true, "Mushroom fruiting"}
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

// Enhanced LCD Display Class for Dual Zone
class DualZoneLCD {
  private:
    RDTRC_LCD* baseLCD;
    int currentZone; // 0 = mushroom, 1 = cilantro
    unsigned long lastZoneSwitch;
    unsigned long zoneSwitchInterval;
    
  public:
    DualZoneLCD() {
      baseLCD = &systemLCD;
      currentZone = 0;
      lastZoneSwitch = 0;
      zoneSwitchInterval = 6000; // Switch zone every 6 seconds
    }
    
    void updateDualZoneStatus() {
      if (!baseLCD->isLCDConnected()) return;
      
      // Auto switch between zones
      if (millis() - lastZoneSwitch > zoneSwitchInterval) {
        currentZone = (currentZone + 1) % 2;
        lastZoneSwitch = millis();
      }
      
      if (currentZone == 0) {
        // Show mushroom data
        baseLCD->updateStatus(
          "Mushroom Zone",
          ambientTemperature,
          ambientHumidity,
          mushroom.currentMoisture,
          mushroom.growthPhase,
          isWiFiConnected,
          systemMaintenanceMode,
          todayStats.alerts
        );
      } else {
        // Show cilantro data
        baseLCD->updateStatus(
          "Cilantro Zone",
          ambientTemperature,
          ambientHumidity,
          cilantro.currentMoisture,
          cilantro.growthPhase,
          isWiFiConnected,
          systemMaintenanceMode,
          todayStats.alerts
        );
      }
    }
    
    void switchZone() {
      currentZone = (currentZone + 1) % 2;
      lastZoneSwitch = millis();
    }
    
    int getCurrentZone() {
      return currentZone;
    }
};

DualZoneLCD dualLCD;

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
  
  Serial.println("RDTRC Dual Growing System with LCD Ready!");
  Serial.println("Web Interface: http://growing-system.local");
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
  Serial.println("RDTRC Complete Dual Growing System with LCD");
  Serial.println("");
  Serial.println("Firmware made by: " + String(FIRMWARE_MAKER));
  Serial.println("Version: " + String(FIRMWARE_VERSION));
  Serial.println("System: " + String(SYSTEM_NAME));
  Serial.println("");
  Serial.println("Features:");
  Serial.println("* LCD I2C 16x2 Display with Auto Detection");
  Serial.println("* Dual-Zone Growing Control");
  Serial.println("* Advanced Environmental Monitoring");
  Serial.println("* Automated Misting System");
  Serial.println("* LED Grow Light Control");
  Serial.println("* Temperature & Humidity Control");
  Serial.println("* CO2 & pH Monitoring");
  Serial.println("* Growth Phase Management");
  Serial.println("* Web Interface");
  Serial.println("* Mobile App Control");
  Serial.println("* Smart Notifications");
  Serial.println("* Data Logging & Analytics");
  Serial.println("* OTA Updates");
  Serial.println("");
  Serial.println("Initializing Dual Growing Environment...");
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
  
  // Mushroom zone pins
  pinMode(MUSHROOM_MISTING_PIN, OUTPUT);
  pinMode(MUSHROOM_HEATER_PIN, OUTPUT);
  pinMode(MUSHROOM_FAN_PIN, OUTPUT);
  
  // Cilantro zone pins
  pinMode(CILANTRO_WATER_PUMP_PIN, OUTPUT);
  pinMode(CILANTRO_GROW_LIGHT_PIN, OUTPUT);
  pinMode(CILANTRO_FAN_PIN, OUTPUT);
  
  // Initialize all outputs to OFF
  digitalWrite(MUSHROOM_MISTING_PIN, LOW);
  digitalWrite(MUSHROOM_HEATER_PIN, LOW);
  digitalWrite(MUSHROOM_FAN_PIN, LOW);
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
  String startupMsg = "RDTRC Dual Growing System Started!\n";
  startupMsg += "System: " + String(SYSTEM_NAME) + "\n";
  startupMsg += "Version: " + String(FIRMWARE_VERSION) + "\n";
  startupMsg += "LCD: " + (systemLCD.isLCDConnected() ? "Connected" : "Not Found") + "\n";
  startupMsg += "Water Level: " + String(waterLevel) + "cm\n";
  startupMsg += "Temperature: " + String(ambientTemperature) + "C\n";
  startupMsg += "Humidity: " + String(ambientHumidity) + "%\n";
  startupMsg += "Mushroom Phase: " + mushroom.growthPhase + "\n";
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
    if (MDNS.begin("growing-system")) {
      Serial.println("mDNS responder started: growing-system.local");
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
  if (MDNS.begin("growing-system")) {
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
    doc["lcd_current_zone"] = dualLCD.getCurrentZone();
    
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
  
  // Control endpoint for both zones
  server.on("/api/control", HTTP_POST, []() {
    if (systemMaintenanceMode) {
      server.send(423, "application/json", "{\"error\":\"system_in_maintenance\"}");
      return;
    }
    
    String zone = server.arg("zone");
    String action = server.arg("action");
    String value = server.arg("value");
    String lcdAction = "";
    
    if (zone == "mushroom") {
      if (action == "misting") {
        mushroom.wateringActive = value == "true";
        digitalWrite(MUSHROOM_MISTING_PIN, mushroom.wateringActive ? HIGH : LOW);
        lcdAction = "M:" + String(mushroom.wateringActive ? "Mist ON" : "Mist OFF");
      } else if (action == "heating") {
        mushroom.heatingActive = value == "true";
        digitalWrite(MUSHROOM_HEATER_PIN, mushroom.heatingActive ? HIGH : LOW);
        lcdAction = "M:" + String(mushroom.heatingActive ? "Heat ON" : "Heat OFF");
      } else if (action == "fan") {
        mushroom.fanActive = value == "true";
        digitalWrite(MUSHROOM_FAN_PIN, mushroom.fanActive ? HIGH : LOW);
        lcdAction = "M:" + String(mushroom.fanActive ? "Fan ON" : "Fan OFF");
      }
    } else if (zone == "cilantro") {
      if (action == "watering") {
        cilantro.wateringActive = value == "true";
        digitalWrite(CILANTRO_WATER_PUMP_PIN, cilantro.wateringActive ? HIGH : LOW);
        lcdAction = "C:" + String(cilantro.wateringActive ? "Water ON" : "Water OFF");
      } else if (action == "light") {
        cilantro.lightActive = value == "true";
        digitalWrite(CILANTRO_GROW_LIGHT_PIN, cilantro.lightActive ? HIGH : LOW);
        lcdAction = "C:" + String(cilantro.lightActive ? "Light ON" : "Light OFF");
      } else if (action == "fan") {
        cilantro.fanActive = value == "true";
        digitalWrite(CILANTRO_FAN_PIN, cilantro.fanActive ? HIGH : LOW);
        lcdAction = "C:" + String(cilantro.fanActive ? "Fan ON" : "Fan OFF");
      }
    }
    
    if (lcdAction != "") {
      systemLCD.showDebug("Control", lcdAction);
    }
    
    server.send(200, "application/json", "{\"status\":\"control_updated\"}");
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
    Blynk.virtualWrite(V11, cilantro.currentMoisture);
  }
}

void setupOTA() {
  if (isWiFiConnected) {
    ArduinoOTA.setHostname("growing-system");
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
      // Switch between zones manually
      dualLCD.switchZone();
      systemLCD.showDebug("LCD Switch", dualLCD.getCurrentZone() == 0 ? "Mushroom" : "Cilantro");
      lastLCDButtonPress = millis();
    }
  }
  
  lastLCDButtonState = currentLCDButtonState;
}

void updateLCDDisplay() {
  // Update LCD status every 2 seconds
  if (millis() - lastLCDUpdate > 2000) {
    // Update dual zone LCD display
    dualLCD.updateDualZoneStatus();
    
    // Update LCD display
    systemLCD.update();
    
    lastLCDUpdate = millis();
  }
}

void controlEnvironment() {
  // Control mushroom environment
  if (mushroom.enabled) {
    controlMushrooms();
  }
  
  // Control cilantro environment
  if (cilantro.enabled) {
    controlCilantro();
  }
  
  // Control lighting
  controlLighting();
}

void controlMushrooms() {
  // Temperature control
  if (ambientTemperature < mushroom.targetTemp - 1.0) {
    if (!mushroom.heatingActive) {
      mushroom.heatingActive = true;
      digitalWrite(MUSHROOM_HEATER_PIN, HIGH);
      Serial.println("Mushroom heater ON");
      systemLCD.showDebug("M: Control", "Heater ON");
    }
  } else if (ambientTemperature > mushroom.targetTemp + 1.0) {
    if (mushroom.heatingActive) {
      mushroom.heatingActive = false;
      digitalWrite(MUSHROOM_HEATER_PIN, LOW);
      Serial.println("Mushroom heater OFF");
      systemLCD.showDebug("M: Control", "Heater OFF");
    }
  }
  
  // Humidity control (misting)
  if (ambientHumidity < mushroom.targetHumidity - 5.0) {
    if (!mushroom.wateringActive) {
      mushroom.wateringActive = true;
      digitalWrite(MUSHROOM_MISTING_PIN, HIGH);
      Serial.println("Mushroom misting ON");
      systemLCD.showDebug("M: Control", "Misting ON");
      todayStats.wateringCycles++;
    }
  } else if (ambientHumidity > mushroom.targetHumidity + 5.0) {
    if (mushroom.wateringActive) {
      mushroom.wateringActive = false;
      digitalWrite(MUSHROOM_MISTING_PIN, LOW);
      Serial.println("Mushroom misting OFF");
      systemLCD.showDebug("M: Control", "Misting OFF");
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
    systemLCD.showDebug("M: Control", "Fan ON");
  } else if (!needsVentilation && mushroom.fanActive) {
    mushroom.fanActive = false;
    digitalWrite(MUSHROOM_FAN_PIN, LOW);
    Serial.println("Mushroom fan OFF");
    systemLCD.showDebug("M: Control", "Fan OFF");
  }
}

void controlCilantro() {
  // Soil moisture control
  if (cilantro.currentMoisture < cilantro.targetMoisture - 5) {
    if (!cilantro.wateringActive && waterLevel > LOW_WATER_THRESHOLD) {
      cilantro.wateringActive = true;
      digitalWrite(CILANTRO_WATER_PUMP_PIN, HIGH);
      cilantro.lastWatered = millis();
      Serial.println("Cilantro watering ON");
      systemLCD.showDebug("C: Control", "Water ON");
      todayStats.wateringCycles++;
    }
  } else if (cilantro.currentMoisture > cilantro.targetMoisture + 5) {
    if (cilantro.wateringActive) {
      cilantro.wateringActive = false;
      digitalWrite(CILANTRO_WATER_PUMP_PIN, LOW);
      cilantro.totalWateringTime += millis() - cilantro.lastWatered;
      Serial.println("Cilantro watering OFF");
      systemLCD.showDebug("C: Control", "Water OFF");
    }
  }
  
  // Automatic watering timeout (safety)
  if (cilantro.wateringActive && (millis() - cilantro.lastWatered > 60000)) {
    cilantro.wateringActive = false;
    digitalWrite(CILANTRO_WATER_PUMP_PIN, LOW);
    Serial.println("Cilantro watering timeout - safety stop");
    systemLCD.showDebug("C: Safety", "Water Timeout");
  }
  
  // Ventilation control
  if (ambientTemperature > cilantro.targetTemp + 2.0 || co2Level > HIGH_CO2_THRESHOLD) {
    if (!cilantro.fanActive) {
      cilantro.fanActive = true;
      digitalWrite(CILANTRO_FAN_PIN, HIGH);
      Serial.println("Cilantro fan ON");
      systemLCD.showDebug("C: Control", "Fan ON");
    }
  } else if (ambientTemperature < cilantro.targetTemp - 2.0 && co2Level < HIGH_CO2_THRESHOLD) {
    if (cilantro.fanActive) {
      cilantro.fanActive = false;
      digitalWrite(CILANTRO_FAN_PIN, LOW);
      Serial.println("Cilantro fan OFF");
      systemLCD.showDebug("C: Control", "Fan OFF");
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
    systemLCD.showDebug("C: Control", "Light ON");
  } else if (!shouldLightBeOn && cilantro.lightActive) {
    cilantro.lightActive = false;
    digitalWrite(CILANTRO_GROW_LIGHT_PIN, LOW);
    Serial.println("Cilantro grow light OFF");
    systemLCD.showDebug("C: Control", "Light OFF");
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
  
  // Read soil moisture sensors
  int mushroomRaw = analogRead(mushroom.soilPin);
  mushroom.currentMoisture = map(mushroomRaw, 4095, 0, 0, 100);
  if (mushroom.currentMoisture < 0) mushroom.currentMoisture = 0;
  if (mushroom.currentMoisture > 100) mushroom.currentMoisture = 100;
  
  int cilantroRaw = analogRead(cilantro.soilPin);
  cilantro.currentMoisture = map(cilantroRaw, 4095, 0, 0, 100);
  if (cilantro.currentMoisture < 0) cilantro.currentMoisture = 0;
  if (cilantro.currentMoisture > 100) cilantro.currentMoisture = 100;
  
  // Update zone temperature and humidity
  mushroom.currentTemp = ambientTemperature;
  mushroom.currentHumidity = ambientHumidity;
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
    Blynk.virtualWrite(V10, mushroom.currentMoisture);
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
        
        systemLCD.showMessage("M: Phase Update", mushroom.growthPhase, 5000);
        
        saveSettings();
      }
      break;
    }
  }
  
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
        
        systemLCD.showMessage("C: Phase Update", cilantro.growthPhase, 5000);
        
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
      digitalWrite(CILANTRO_WATER_PUMP_PIN, LOW);
      digitalWrite(CILANTRO_GROW_LIGHT_PIN, LOW);
      digitalWrite(CILANTRO_FAN_PIN, LOW);
      
      systemMaintenanceMode = true;
      systemLCD.showAlert("EMERGENCY STOP", 5000);
      
      sendLineNotification("Emergency Stop Activated!\nAll growing systems have been stopped.");
      
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
      sendLineNotification("Dual growing system reset initiated");
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
  
  // Mushroom settings
  JsonObject mushroomObj = doc.createNestedObject("mushroom");
  mushroomObj["target_temp"] = mushroom.targetTemp;
  mushroomObj["target_humidity"] = mushroom.targetHumidity;
  mushroomObj["target_moisture"] = mushroom.targetMoisture;
  mushroomObj["growth_phase"] = mushroom.growthPhase;
  mushroomObj["days_in_phase"] = mushroom.daysInPhase;
  mushroomObj["enabled"] = mushroom.enabled;
  
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
  html += "<title>RDTRC Dual Growing System with LCD</title>";
  html += "<meta charset=\"UTF-8\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background: #1a1a1a; color: #fff; margin: 0; padding: 20px; }";
  html += ".header { background: linear-gradient(135deg, #8e44ad, #3498db, #2ecc71); padding: 20px; text-align: center; border-radius: 10px; margin-bottom: 20px; }";
  html += ".container { max-width: 1200px; margin: 0 auto; }";
  html += ".status-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 15px; margin-bottom: 20px; }";
  html += ".status-card { background: #2d2d2d; padding: 15px; border-radius: 8px; text-align: center; }";
  html += ".btn { padding: 10px 20px; margin: 5px; border: none; border-radius: 5px; cursor: pointer; font-size: 14px; }";
  html += ".btn-success { background: #27ae60; color: white; }";
  html += ".btn-warning { background: #f39c12; color: white; }";
  html += ".btn-secondary { background: #555; color: white; }";
  html += ".btn-danger { background: #e74c3c; color: white; }";
  html += ".zone-card { background: #2d2d2d; padding: 20px; border-radius: 10px; margin-bottom: 20px; }";
  html += ".mushroom-zone { border-left: 4px solid #8e44ad; }";
  html += ".cilantro-zone { border-left: 4px solid #2ecc71; }";
  html += ".lcd-info { background: #2c3e50; padding: 15px; border-radius: 8px; margin-bottom: 20px; }";
  html += ".zones-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(400px, 1fr)); gap: 20px; }";
  html += "</style>";
  html += "<script>";
  html += "function refreshStatus() {";
  html += "  fetch('/api/status').then(response => response.json()).then(data => {";
  html += "    document.getElementById('temperature').textContent = data.ambient_temperature.toFixed(1) + 'C';";
  html += "    document.getElementById('humidity').textContent = data.ambient_humidity.toFixed(1) + '%';";
  html += "    document.getElementById('co2').textContent = data.co2_level + ' ppm';";
  html += "    document.getElementById('ph').textContent = data.ph_level.toFixed(1);";
  html += "    document.getElementById('water-level').textContent = data.water_level.toFixed(1) + 'cm';";
  html += "    document.getElementById('mushroom-moisture').textContent = data.mushroom.moisture + '%';";
  html += "    document.getElementById('cilantro-moisture').textContent = data.cilantro.moisture + '%';";
  html += "    document.getElementById('mushroom-phase').textContent = data.mushroom.growth_phase;";
  html += "    document.getElementById('cilantro-phase').textContent = data.cilantro.growth_phase;";
  html += "    document.getElementById('lcd-status').textContent = data.lcd_connected ? 'Connected at ' + data.lcd_address : 'Not Connected';";
  html += "    document.getElementById('lcd-zone').textContent = data.lcd_current_zone == 0 ? 'Mushroom' : 'Cilantro';";
  html += "  });";
  html += "}";
  html += "function toggleControl(zone, action) {";
  html += "  fetch('/api/control', {";
  html += "    method: 'POST',";
  html += "    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },";
  html += "    body: 'zone=' + zone + '&action=' + action + '&value=true'";
  html += "  }).then(() => setTimeout(refreshStatus, 1000));";
  html += "}";
  html += "setInterval(refreshStatus, 30000);";
  html += "window.onload = refreshStatus;";
  html += "</script>";
  html += "</head><body>";
  html += "<div class=\"container\">";
  html += "<div class=\"header\">";
  html += "<h1>RDTRC Dual Growing System</h1>";
  html += "<p>Complete Dual-Zone System with LCD Display - v4.0</p>";
  html += "</div>";
  html += "<div class=\"lcd-info\">";
  html += "<h3>LCD Display Status</h3>";
  html += "<p>LCD Status: <span id=\"lcd-status\">Loading...</span></p>";
  html += "<p>Current Zone: <span id=\"lcd-zone\">Loading...</span></p>";
  html += "<p>LCD auto-switches between zones every 6 seconds</p>";
  html += "<p>Use the LCD button (Pin 26) to manually switch zones</p>";
  html += "</div>";
  html += "<div class=\"status-grid\">";
  html += "<div class=\"status-card\"><h3>Temperature</h3><div id=\"temperature\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Humidity</h3><div id=\"humidity\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>CO2 Level</h3><div id=\"co2\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>pH Level</h3><div id=\"ph\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Water Level</h3><div id=\"water-level\">Loading...</div></div>";
  html += "</div>";
  html += "<div class=\"zones-grid\">";
  html += "<div class=\"zone-card mushroom-zone\">";
  html += "<h2>Mushroom Zone</h2>";
  html += "<p>Moisture: <span id=\"mushroom-moisture\">Loading...</span></p>";
  html += "<p>Phase: <span id=\"mushroom-phase\">Loading...</span></p>";
  html += "<button class=\"btn btn-success\" onclick=\"toggleControl('mushroom', 'misting')\">Toggle Misting</button>";
  html += "<button class=\"btn btn-danger\" onclick=\"toggleControl('mushroom', 'heating')\">Toggle Heating</button>";
  html += "<button class=\"btn btn-secondary\" onclick=\"toggleControl('mushroom', 'fan')\">Toggle Fan</button>";
  html += "</div>";
  html += "<div class=\"zone-card cilantro-zone\">";
  html += "<h2>Cilantro Zone</h2>";
  html += "<p>Moisture: <span id=\"cilantro-moisture\">Loading...</span></p>";
  html += "<p>Phase: <span id=\"cilantro-phase\">Loading...</span></p>";
  html += "<button class=\"btn btn-success\" onclick=\"toggleControl('cilantro', 'watering')\">Toggle Watering</button>";
  html += "<button class=\"btn btn-warning\" onclick=\"toggleControl('cilantro', 'light')\">Toggle Light</button>";
  html += "<button class=\"btn btn-secondary\" onclick=\"toggleControl('cilantro', 'fan')\">Toggle Fan</button>";
  html += "</div>";
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
    systemLCD.showDebug("Blynk M", mushroom.wateringActive ? "Mist ON" : "Mist OFF");
  }
}

BLYNK_WRITE(V21) { // Mushroom heater control
  if (!systemMaintenanceMode) {
    mushroom.heatingActive = param.asInt() == 1;
    digitalWrite(MUSHROOM_HEATER_PIN, mushroom.heatingActive ? HIGH : LOW);
    systemLCD.showDebug("Blynk M", mushroom.heatingActive ? "Heat ON" : "Heat OFF");
  }
}

BLYNK_WRITE(V22) { // Cilantro watering control
  if (!systemMaintenanceMode) {
    cilantro.wateringActive = param.asInt() == 1;
    digitalWrite(CILANTRO_WATER_PUMP_PIN, cilantro.wateringActive ? HIGH : LOW);
    systemLCD.showDebug("Blynk C", cilantro.wateringActive ? "Water ON" : "Water OFF");
  }
}

BLYNK_WRITE(V23) { // Cilantro grow light control
  if (!systemMaintenanceMode) {
    cilantro.lightActive = param.asInt() == 1;
    digitalWrite(CILANTRO_GROW_LIGHT_PIN, cilantro.lightActive ? HIGH : LOW);
    systemLCD.showDebug("Blynk C", cilantro.lightActive ? "Light ON" : "Light OFF");
  }
}

BLYNK_WRITE(V30) { // Emergency stop
  if (param.asInt() == 1) {
    systemMaintenanceMode = true;
    // Turn off all systems
    digitalWrite(MUSHROOM_MISTING_PIN, LOW);
    digitalWrite(MUSHROOM_HEATER_PIN, LOW);
    digitalWrite(MUSHROOM_FAN_PIN, LOW);
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
BLYNK_READ(V10) { Blynk.virtualWrite(V10, mushroom.currentMoisture); }
BLYNK_READ(V11) { Blynk.virtualWrite(V11, cilantro.currentMoisture); }
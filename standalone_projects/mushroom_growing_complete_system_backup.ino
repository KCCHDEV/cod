/*
 * RDTRC Complete Mushroom Growing System - Standalone Version
 * Version: 4.0 - Independent System
 * Firmware made by: RDTRC
 * Updated: 2024
 * 
 * Features:
 * - Complete standalone mushroom growing system
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
 */

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

// System Configuration
#define FIRMWARE_VERSION "4.0"
#define FIRMWARE_MAKER "RDTRC"
#define SYSTEM_NAME "Mushroom Growing System"
#define DEVICE_ID "RDTRC_MUSHROOM"

// Network Configuration - แก้ไขตามเครือข่ายของคุณ
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* hotspot_ssid = "RDTRC_Mushroom";
const char* hotspot_password = "rdtrc123";

// Blynk Configuration - ใส่ Token ของคุณ
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_DEVICE_NAME "RDTRC Mushroom System"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"

// LINE Notify Configuration - ใส่ Token ของคุณ
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

// System Control Pins
#define STATUS_LED_PIN 2
#define BUZZER_PIN 4
#define RESET_BUTTON_PIN 0
#define MANUAL_BUTTON_PIN 27

// Water System Pins
#define WATER_LEVEL_TRIG_PIN 25
#define WATER_LEVEL_ECHO_PIN 26

// Growing Configuration
#define MUSHROOM_OPTIMAL_TEMP 22.0      // °C
#define MUSHROOM_OPTIMAL_HUMIDITY 85.0   // %
#define MUSHROOM_OPTIMAL_MOISTURE 70     // %

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

// System Variables
bool isWiFiConnected = false;
bool isHotspotMode = false;
unsigned long lastHeartbeat = 0;
unsigned long lastDataLog = 0;
unsigned long lastStatusCheck = 0;
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
  
  Serial.println("✅ RDTRC Mushroom Growing System Ready!");
  Serial.println("🌐 Web Interface: http://mushroom-system.local");
  Serial.println("📱 Blynk App: Connected");
  Serial.println("🔔 LINE Notifications: Enabled");
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
  Serial.println("\n" + String("=").repeat(60));
  Serial.println("🍄 RDTRC Complete Mushroom Growing System");
  Serial.println("");
  Serial.println("Firmware made by: " + String(FIRMWARE_MAKER));
  Serial.println("Version: " + String(FIRMWARE_VERSION));
  Serial.println("System: " + String(SYSTEM_NAME));
  Serial.println("");
  Serial.println("Features:");
  Serial.println("✓ Advanced Environmental Monitoring");
  Serial.println("✓ Automated Misting System");
  Serial.println("✓ Temperature & Humidity Control");
  Serial.println("✓ CO2 & pH Monitoring");
  Serial.println("✓ Growth Phase Management");
  Serial.println("✓ Web Interface");
  Serial.println("✓ Mobile App Control");
  Serial.println("✓ Smart Notifications");
  Serial.println("✓ Data Logging & Analytics");
  Serial.println("✓ OTA Updates");
  Serial.println("");
  Serial.println("Initializing Mushroom Growing Environment...");
  Serial.println("=".repeat(60));
  delay(3000);
}

void setupSystem() {
  // Initialize pins
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MANUAL_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
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
  
  // Initialize DHT sensor
  dht.begin();
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ SPIFFS initialization failed");
  } else {
    Serial.println("✅ SPIFFS initialized");
  }
  
  // Load saved settings
  loadSettings();
  
  // Setup network and services
  setupWiFi();
  setupWebServer();
  setupBlynk();
  setupOTA();
  
  // Initialize NTP
  timeClient.begin();
  timeClient.update();
  
  // Initialize today's stats
  todayStats.date = String(timeClient.getDay()) + "/" + String(timeClient.getMonth()) + "/" + String(timeClient.getYear());
  todayStats.avgTemperature = 0;
  todayStats.avgHumidity = 0;
  todayStats.avgCO2 = 0;
  todayStats.avgPH = 0;
  todayStats.wateringCycles = 0;
  todayStats.totalWateringTime = 0;
  todayStats.alerts = 0;
  
  // Boot sequence
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
  sendLineNotification("🍄 RDTRC Mushroom Growing System Started!\n"
                      "System: " + String(SYSTEM_NAME) + "\n" +
                      "Version: " + String(FIRMWARE_VERSION) + "\n" +
                      "Water Level: " + String(waterLevel) + "cm\n" +
                      "Temperature: " + String(ambientTemperature) + "°C\n" +
                      "Humidity: " + String(ambientHumidity) + "%\n" +
                      "Mushroom Phase: " + mushroom.growthPhase + "\n" +
                      "Status: Growing environment ready!");
  
  Serial.println("🚀 System initialization complete!");
}

void setupWiFi() {
  Serial.println("🔗 Connecting to WiFi: " + String(ssid));
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(1000);
    Serial.print(".");
    attempts++;
    
    // Blink LED while connecting
    digitalWrite(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    isWiFiConnected = true;
    Serial.println("\n✅ WiFi Connected!");
    Serial.println("📍 IP Address: " + WiFi.localIP().toString());
    Serial.println("📶 Signal Strength: " + String(WiFi.RSSI()) + " dBm");
    
    // Setup mDNS
    if (MDNS.begin("mushroom-system")) {
      Serial.println("✅ mDNS responder started: mushroom-system.local");
    }
  } else {
    Serial.println("\n❌ WiFi connection failed. Starting hotspot...");
    setupHotspot();
  }
}

void setupHotspot() {
  WiFi.softAP(hotspot_ssid, hotspot_password);
  isHotspotMode = true;
  Serial.println("🔥 Hotspot started: " + String(hotspot_ssid));
  Serial.println("📍 Hotspot IP: " + WiFi.softAPIP().toString());
  Serial.println("🔑 Password: " + String(hotspot_password));
  
  // Setup mDNS for hotspot
  if (MDNS.begin("mushroom-system")) {
    Serial.println("✅ mDNS responder started for hotspot");
  }
}

void setupWebServer() {
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
      if (action == "misting") {
        mushroom.wateringActive = value == "true";
        digitalWrite(MUSHROOM_MISTING_PIN, mushroom.wateringActive ? HIGH : LOW);
      } else if (action == "heating") {
        mushroom.heatingActive = value == "true";
        digitalWrite(MUSHROOM_HEATER_PIN, mushroom.heatingActive ? HIGH : LOW);
      } else if (action == "fan") {
        mushroom.fanActive = value == "true";
        digitalWrite(MUSHROOM_FAN_PIN, mushroom.fanActive ? HIGH : LOW);
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
  Serial.println("✅ Web server started on port 80");
}

void setupBlynk() {
  if (isWiFiConnected) {
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
    Serial.println("✅ Blynk connected");
    
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
      Serial.println("🔄 OTA Update started: " + type);
      systemMaintenanceMode = true;
    });
    
    ArduinoOTA.onEnd([]() {
      Serial.println("\n✅ OTA Update completed");
      systemMaintenanceMode = false;
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("📊 OTA Progress: %u%%\r", (progress / (total / 100)));
    });
    
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("❌ OTA Error[%u]: ", error);
      systemMaintenanceMode = false;
    });
    
    ArduinoOTA.begin();
    Serial.println("✅ OTA updates enabled");
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
    Serial.println("✅ WiFi reconnected");
    setupBlynk();
  } else if (isWiFiConnected && WiFi.status() != WL_CONNECTED) {
    isWiFiConnected = false;
    Serial.println("❌ WiFi disconnected");
  }
  
  // Update time
  if (isWiFiConnected) {
    timeClient.update();
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
      Serial.println("🍄 Mushroom heater ON");
    }
  } else if (ambientTemperature > mushroom.targetTemp + 1.0) {
    if (mushroom.heatingActive) {
      mushroom.heatingActive = false;
      digitalWrite(MUSHROOM_HEATER_PIN, LOW);
      Serial.println("🍄 Mushroom heater OFF");
    }
  }
  
  // Humidity control (misting)
  if (ambientHumidity < mushroom.targetHumidity - 5.0) {
    if (!mushroom.wateringActive) {
      mushroom.wateringActive = true;
      digitalWrite(MUSHROOM_MISTING_PIN, HIGH);
      Serial.println("🍄 Mushroom misting ON");
      todayStats.wateringCycles++;
    }
  } else if (ambientHumidity > mushroom.targetHumidity + 5.0) {
    if (mushroom.wateringActive) {
      mushroom.wateringActive = false;
      digitalWrite(MUSHROOM_MISTING_PIN, LOW);
      Serial.println("🍄 Mushroom misting OFF");
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
    Serial.println("🍄 Mushroom fan ON");
  } else if (!needsVentilation && mushroom.fanActive) {
    mushroom.fanActive = false;
    digitalWrite(MUSHROOM_FAN_PIN, LOW);
    Serial.println("🍄 Mushroom fan OFF");
  }
}

void readSensors() {
  // Read DHT sensor
  ambientTemperature = dht.readTemperature();
  ambientHumidity = dht.readHumidity();
  
  // Check for sensor errors
  if (isnan(ambientTemperature) || isnan(ambientHumidity)) {
    Serial.println("❌ DHT sensor error");
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
  int mushroomRaw = analogRead(mushroom.soilPin);
  mushroom.currentMoisture = map(mushroomRaw, 4095, 0, 0, 100); // Invert for dry=low, wet=high
  if (mushroom.currentMoisture < 0) mushroom.currentMoisture = 0;
  if (mushroom.currentMoisture > 100) mushroom.currentMoisture = 100;
  
  // Update zone temperature and humidity (assuming ambient for now)
  mushroom.currentTemp = ambientTemperature;
  mushroom.currentHumidity = ambientHumidity;
  
  // Update Blynk with sensor data
  if (isWiFiConnected) {
    Blynk.virtualWrite(V1, ambientTemperature);
    Blynk.virtualWrite(V2, ambientHumidity);
    Blynk.virtualWrite(V3, co2Level);
    Blynk.virtualWrite(V4, phLevel);
    Blynk.virtualWrite(V5, waterLevel);
    Blynk.virtualWrite(V6, lightLevel);
    Blynk.virtualWrite(V10, mushroom.currentMoisture);
  }
}

void checkAlerts() {
  // Low water alert
  if (waterLevel < LOW_WATER_THRESHOLD) {
    String alertMsg = "⚠️ Low Water Level Alert!\n" +
                     "Current Level: " + String(waterLevel) + "cm\n" +
                     "Please refill the water tank.";
    sendLineNotification(alertMsg);
    todayStats.alerts++;
  }
  
  // High CO2 alert
  if (co2Level > HIGH_CO2_THRESHOLD) {
    String alertMsg = "⚠️ High CO2 Level Alert!\n" +
                     "Current Level: " + String(co2Level) + "ppm\n" +
                     "Ventilation activated.";
    sendLineNotification(alertMsg);
    todayStats.alerts++;
  }
  
  // pH out of range alert
  if (phLevel < OPTIMAL_PH_MIN || phLevel > OPTIMAL_PH_MAX) {
    String alertMsg = "⚠️ pH Level Alert!\n" +
                     "Current pH: " + String(phLevel) + "\n" +
                     "Optimal range: " + String(OPTIMAL_PH_MIN) + "-" + String(OPTIMAL_PH_MAX);
    sendLineNotification(alertMsg);
    todayStats.alerts++;
  }
  
  // Temperature alerts
  if (ambientTemperature > 35.0) {
    String alertMsg = "🌡️ High Temperature Alert!\n" +
                     "Current: " + String(ambientTemperature) + "°C\n" +
                     "Check cooling systems.";
    sendLineNotification(alertMsg);
    todayStats.alerts++;
  } else if (ambientTemperature < 10.0) {
    String alertMsg = "🌡️ Low Temperature Alert!\n" +
                     "Current: " + String(ambientTemperature) + "°C\n" +
                     "Check heating systems.";
    sendLineNotification(alertMsg);
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
        
        String phaseMsg = "🍄 Mushroom Growth Phase Updated!\n" +
                         "New Phase: " + mushroom.growthPhase + "\n" +
                         "Target Temp: " + String(mushroom.targetTemp) + "°C\n" +
                         "Target Humidity: " + String(mushroom.targetHumidity) + "%\n" +
                         "Description: " + mushroomPhases[i + 1].description;
        sendLineNotification(phaseMsg);
        
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
      Serial.println("🔘 Manual emergency stop button pressed");
      
      // Turn off all systems
      digitalWrite(MUSHROOM_MISTING_PIN, LOW);
      digitalWrite(MUSHROOM_HEATER_PIN, LOW);
      digitalWrite(MUSHROOM_FAN_PIN, LOW);
      
      systemMaintenanceMode = true;
      
      sendLineNotification("🛑 Emergency Stop Activated!\nAll mushroom growing systems have been stopped.");
      
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
    } else if (millis() - resetButtonPressTime > 5000) {
      Serial.println("🔄 System reset initiated...");
      sendLineNotification("🔄 Mushroom growing system reset initiated");
      delay(1000);
      ESP.restart();
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
    Serial.println("💾 Settings saved");
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
      
      Serial.println("📖 Settings loaded");
    }
    configFile.close();
  }
}

void performSystemMaintenance() {
  // Check system health
  if (ESP.getFreeHeap() < 20000) {
    Serial.println("⚠️ Low memory warning: " + String(ESP.getFreeHeap()) + " bytes");
  }
  
  // Rotate log files if they get too large
  File logFile = SPIFFS.open("/system.log", "r");
  if (logFile && logFile.size() > 150000) { // 150KB limit
    logFile.close();
    SPIFFS.remove("/system.log.old");
    SPIFFS.rename("/system.log", "/system.log.old");
    Serial.println("🔄 Log file rotated");
  }
  if (logFile) logFile.close();
  
  // Reset daily stats at midnight
  String currentDate = String(timeClient.getDay()) + "/" + String(timeClient.getMonth()) + "/" + String(timeClient.getYear());
  if (currentDate != todayStats.date) {
    // Save yesterday's stats
    JsonDocument statsDoc;
    statsDoc["date"] = todayStats.date;
    statsDoc["avg_temperature"] = todayStats.avgTemperature;
    statsDoc["avg_humidity"] = todayStats.avgHumidity;
    statsDoc["avg_co2"] = todayStats.avgCO2;
    statsDoc["avg_ph"] = todayStats.avgPH;
    statsDoc["watering_cycles"] = todayStats.wateringCycles;
    statsDoc["total_watering_time"] = todayStats.totalWateringTime;
    statsDoc["alerts"] = todayStats.alerts;
    
    File statsFile = SPIFFS.open("/daily_stats.log", "a");
    if (statsFile) {
      serializeJson(statsDoc, statsFile);
      statsFile.println();
      statsFile.close();
    }
    
    // Reset for new day
    todayStats.date = currentDate;
    todayStats.avgTemperature = 0;
    todayStats.avgHumidity = 0;
    todayStats.avgCO2 = 0;
    todayStats.avgPH = 0;
    todayStats.wateringCycles = 0;
    todayStats.totalWateringTime = 0;
    todayStats.alerts = 0;
    
    Serial.println("🌅 New day started - stats reset");
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
    Serial.println("📱 LINE notification sent");
  } else {
    Serial.println("❌ LINE notification failed: " + String(httpResponseCode));
  }
  
  http.end();
}

void handleWebInterface() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>RDTRC Mushroom Growing System</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #0f1419; color: #e6e6e6; }
        .header { background: linear-gradient(135deg, #8e44ad 0%, #3498db 50%, #2ecc71 100%); padding: 20px; text-align: center; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }
        .header h1 { color: white; margin-bottom: 10px; font-size: 28px; }
        .header p { color: #f0f0f0; opacity: 0.9; }
        .container { max-width: 1600px; margin: 20px auto; padding: 0 20px; }
        .status-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(140px, 1fr)); gap: 15px; margin-bottom: 30px; }
        .status-card { background: #1a1f2e; border-radius: 10px; padding: 15px; text-align: center; border: 1px solid #2d3748; transition: transform 0.2s; }
        .status-card:hover { transform: translateY(-2px); }
        .status-card h3 { color: #8e44ad; margin-bottom: 8px; font-size: 11px; text-transform: uppercase; }
        .status-card .value { font-size: 16px; font-weight: bold; }
        .zone-card { background: #1a1f2e; border-radius: 10px; padding: 20px; border: 1px solid #2d3748; margin-bottom: 20px; border-left: 4px solid #8e44ad; }
        .zone-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 15px; }
        .zone-status { display: inline-block; padding: 4px 8px; border-radius: 4px; font-size: 11px; font-weight: bold; }
        .status-active { background: #48bb78; color: white; }
        .status-inactive { background: #4a5568; color: white; }
        .controls { background: #1a1f2e; border-radius: 10px; padding: 20px; margin-bottom: 20px; border: 1px solid #2d3748; }
        .control-section { margin-bottom: 25px; }
        .control-section h3 { color: #8e44ad; margin-bottom: 15px; }
        .btn { padding: 8px 16px; border: none; border-radius: 5px; cursor: pointer; font-size: 12px; font-weight: 600; transition: all 0.2s; margin: 2px; }
        .btn-primary { background: #8e44ad; color: white; }
        .btn-success { background: #2ecc71; color: white; }
        .btn-danger { background: #e74c3c; color: white; }
        .btn-secondary { background: #4a5568; color: white; }
        .btn:hover { opacity: 0.8; transform: translateY(-1px); }
        .btn:disabled { opacity: 0.5; cursor: not-allowed; transform: none; }
        .alert { padding: 12px; border-radius: 5px; margin-bottom: 15px; font-size: 14px; }
        .alert-warning { background: #f39c12; color: white; }
        .alert-info { background: #3498db; color: white; }
        .alert-danger { background: #e74c3c; color: white; }
        .maintenance-mode { background: #e74c3c; color: white; padding: 10px; text-align: center; margin-bottom: 20px; border-radius: 5px; }
        .phase-info { background: #2d3748; padding: 15px; border-radius: 8px; margin: 10px 0; }
        .phase-info h4 { color: #3498db; margin-bottom: 8px; }
        .environmental-controls { display: grid; grid-template-columns: repeat(auto-fit, minmax(120px, 1fr)); gap: 10px; margin-top: 15px; }
    </style>
    <script>
        let maintenanceMode = false;
        
        function refreshStatus() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    // Update environmental data
                    document.getElementById('temperature').textContent = data.ambient_temperature.toFixed(1) + '°C';
                    document.getElementById('humidity').textContent = data.ambient_humidity.toFixed(1) + '%';
                    document.getElementById('co2').textContent = data.co2_level + ' ppm';
                    document.getElementById('ph').textContent = data.ph_level.toFixed(1);
                    document.getElementById('water-level').textContent = data.water_level.toFixed(1) + 'cm';
                    document.getElementById('light-level').textContent = data.light_level;
                    document.getElementById('uptime').textContent = Math.floor(data.uptime / 1000 / 60) + ' min';
                    document.getElementById('wifi-signal').textContent = data.wifi_signal + ' dBm';
                    
                    maintenanceMode = data.maintenance_mode;
                    document.getElementById('maintenance-alert').style.display = maintenanceMode ? 'block' : 'none';
                    
                    // Update mushroom zone
                    const mushroomCard = document.getElementById('mushroom-card');
                    mushroomCard.innerHTML = createZoneContent(data.mushroom, 'mushroom');
                    
                    // Show alerts
                    const alertsDiv = document.getElementById('alerts');
                    alertsDiv.innerHTML = '';
                    
                    if (data.water_level < 8) {
                        alertsDiv.innerHTML += '<div class="alert alert-danger">⚠️ Low water level: ' + data.water_level.toFixed(1) + 'cm</div>';
                    }
                    
                    if (data.co2_level > 1000) {
                        alertsDiv.innerHTML += '<div class="alert alert-warning">⚠️ High CO2 level: ' + data.co2_level + 'ppm</div>';
                    }
                    
                    if (data.ph_level < 6.0 || data.ph_level > 7.0) {
                        alertsDiv.innerHTML += '<div class="alert alert-info">⚠️ pH out of range: ' + data.ph_level.toFixed(1) + '</div>';
                    }
                    
                    if (data.ambient_temperature > 35 || data.ambient_temperature < 10) {
                        alertsDiv.innerHTML += '<div class="alert alert-warning">🌡️ Temperature alert: ' + data.ambient_temperature.toFixed(1) + '°C</div>';
                    }
                });
            
            // Refresh stats
            fetch('/api/stats')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('watering-cycles').textContent = data.watering_cycles;
                    document.getElementById('alerts-count').textContent = data.alerts;
                });
        }
        
        function createZoneContent(zone, type) {
            return `
                <div class="zone-header">
                    <h3>🍄 ${zone.name}</h3>
                    <span class="zone-status ${zone.enabled ? 'status-active' : 'status-inactive'}">
                        ${zone.enabled ? 'ENABLED' : 'DISABLED'}
                    </span>
                </div>
                <div class="phase-info">
                    <h4>Growth Phase: ${zone.growth_phase}</h4>
                    <div>Days in Phase: ${zone.days_in_phase}</div>
                </div>
                <div style="margin: 15px 0;">
                    <div><strong>Soil Moisture:</strong> ${zone.moisture}% (Target: ${zone.target_moisture}%)</div>
                    <div><strong>Target Temperature:</strong> ${zone.target_temp}°C</div>
                    <div><strong>Target Humidity:</strong> ${zone.target_humidity}%</div>
                </div>
                <div class="environmental-controls">
                    <button class="btn ${zone.watering_active ? 'btn-success' : 'btn-secondary'}" onclick="toggleControl('mushroom', 'misting')">
                        ${zone.watering_active ? '💧 Misting ON' : '💧 Misting OFF'}
                    </button>
                    <button class="btn ${zone.heating_active ? 'btn-danger' : 'btn-secondary'}" onclick="toggleControl('mushroom', 'heating')">
                        ${zone.heating_active ? '🔥 Heat ON' : '🔥 Heat OFF'}
                    </button>
                    <button class="btn ${zone.fan_active ? 'btn-primary' : 'btn-secondary'}" onclick="toggleControl('mushroom', 'fan')">
                        ${zone.fan_active ? '💨 Fan ON' : '💨 Fan OFF'}
                    </button>
                </div>
                <div style="margin-top: 15px;">
                    <button class="btn btn-secondary" onclick="configureZone('mushroom')">⚙️ Configure</button>
                    <button class="btn btn-secondary" onclick="changePhase('mushroom')">🔄 Change Phase</button>
                </div>
            `;
        }
        
        function toggleControl(zone, action) {
            if (maintenanceMode) {
                alert('System is in maintenance mode');
                return;
            }
            
            fetch('/api/control', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `zone=${zone}&action=${action}&value=true`
            })
            .then(response => response.json())
            .then(data => {
                setTimeout(refreshStatus, 1000);
            });
        }
        
        function configureZone(zone) {
            const temp = prompt('Enter target temperature for mushroom (°C):', '22');
            const humidity = prompt('Enter target humidity for mushroom (%):', '85');
            const moisture = prompt('Enter target moisture for mushroom (%):', '70');
            
            if (temp && humidity && moisture) {
                fetch('/api/settings', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: `mushroom_temp=${temp}&mushroom_humidity=${humidity}&mushroom_moisture=${moisture}`
                })
                .then(response => response.json())
                .then(data => {
                    alert('Zone configuration updated');
                    refreshStatus();
                });
            }
        }
        
        function changePhase(zone) {
            const phases = ['Inoculation', 'Colonization', 'Pinning', 'Fruiting'];
            const phase = prompt(`Select growth phase for mushroom:\\n${phases.join('\\n')}`, phases[0]);
            
            if (phase && phases.includes(phase)) {
                fetch('/api/phase', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: `zone=mushroom&phase=${phase}`
                })
                .then(response => response.json())
                .then(data => {
                    alert('Growth phase updated to: ' + phase);
                    refreshStatus();
                });
            }
        }
        
        function toggleMaintenance() {
            const newMode = !maintenanceMode;
            fetch('/api/maintenance', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `mode=${newMode}`
            })
            .then(response => response.json())
            .then(data => {
                alert('Maintenance mode ' + (newMode ? 'enabled' : 'disabled'));
                refreshStatus();
            });
        }
        
        // Auto refresh every 45 seconds
        setInterval(refreshStatus, 45000);
        
        // Initial load
        window.onload = refreshStatus;
    </script>
</head>
<body>
    <div id="maintenance-alert" class="maintenance-mode" style="display: none;">
        🔧 MAINTENANCE MODE ACTIVE - All growing systems disabled
    </div>
    
    <div class="header">
        <h1>🍄 RDTRC Mushroom Growing System</h1>
        <p>Complete Standalone Mushroom Growing System - Firmware by RDTRC v4.0</p>
    </div>
    
    <div class="container">
        <div id="alerts"></div>
        
        <div class="status-grid">
            <div class="status-card">
                <h3>Temperature</h3>
                <div class="value" id="temperature">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Humidity</h3>
                <div class="value" id="humidity">Loading...</div>
            </div>
            <div class="status-card">
                <h3>CO2 Level</h3>
                <div class="value" id="co2">Loading...</div>
            </div>
            <div class="status-card">
                <h3>pH Level</h3>
                <div class="value" id="ph">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Water Level</h3>
                <div class="value" id="water-level">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Light Level</h3>
                <div class="value" id="light-level">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Misting Cycles</h3>
                <div class="value" id="watering-cycles">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Alerts Today</h3>
                <div class="value" id="alerts-count">Loading...</div>
            </div>
            <div class="status-card">
                <h3>System Uptime</h3>
                <div class="value" id="uptime">Loading...</div>
            </div>
            <div class="status-card">
                <h3>WiFi Signal</h3>
                <div class="value" id="wifi-signal">Loading...</div>
            </div>
        </div>
        
        <h2 style="margin-bottom: 20px; color: #8e44ad;">🍄 Mushroom Growing Zone</h2>
        <div class="zone-card" id="mushroom-card">
            <!-- Mushroom zone content will be loaded here -->
        </div>
        
        <div class="controls">
            <div class="control-section">
                <h3>🔧 System Controls</h3>
                <button class="btn btn-danger" onclick="toggleMaintenance()">🔧 Toggle Maintenance Mode</button>
            </div>
        </div>
        
        <div class="controls">
            <h3 style="color: #8e44ad; margin-bottom: 15px;">📊 System Information</h3>
            <div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px;">
                <div>
                    <p><strong>Device ID:</strong> RDTRC_MUSHROOM</p>
                    <p><strong>Firmware Version:</strong> 4.0</p>
                    <p><strong>System Type:</strong> Mushroom Growing System</p>
                    <p><strong>Water Tank:</strong> 40cm height monitoring</p>
                </div>
                <div>
                    <p><strong>🍄 Mushroom Features:</strong></p>
                    <p>• Automated misting system</p>
                    <p>• Temperature control with heater</p>
                    <p>• Ventilation control</p>
                    <p>• Growth phase management</p>
                </div>
                <div>
                    <p><strong>🔬 Monitoring:</strong></p>
                    <p>• Temperature & Humidity (DHT22)</p>
                    <p>• CO2 level monitoring</p>
                    <p>• pH level monitoring</p>
                    <p>• Light level detection</p>
                    <p>• Soil moisture monitoring</p>
                </div>
                <div>
                    <p><strong>🍄 Growth Phases:</strong></p>
                    <p>• Inoculation (14 days)</p>
                    <p>• Colonization (21 days)</p>
                    <p>• Pinning (7 days)</p>
                    <p>• Fruiting (14 days)</p>
                </div>
            </div>
        </div>
    </div>
</body>
</html>
  )";
  
  server.send(200, "text/html", html);
}

// Blynk Virtual Pin Handlers
BLYNK_WRITE(V20) { // Mushroom misting control
  if (!systemMaintenanceMode) {
    mushroom.wateringActive = param.asInt() == 1;
    digitalWrite(MUSHROOM_MISTING_PIN, mushroom.wateringActive ? HIGH : LOW);
  }
}

BLYNK_WRITE(V21) { // Mushroom heater control
  if (!systemMaintenanceMode) {
    mushroom.heatingActive = param.asInt() == 1;
    digitalWrite(MUSHROOM_HEATER_PIN, mushroom.heatingActive ? HIGH : LOW);
  }
}

BLYNK_WRITE(V22) { // Mushroom fan control
  if (!systemMaintenanceMode) {
    mushroom.fanActive = param.asInt() == 1;
    digitalWrite(MUSHROOM_FAN_PIN, mushroom.fanActive ? HIGH : LOW);
  }
}

BLYNK_WRITE(V30) { // Emergency stop
  if (param.asInt() == 1) {
    systemMaintenanceMode = true;
    // Turn off all systems
    digitalWrite(MUSHROOM_MISTING_PIN, LOW);
    digitalWrite(MUSHROOM_HEATER_PIN, LOW);
    digitalWrite(MUSHROOM_FAN_PIN, LOW);
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
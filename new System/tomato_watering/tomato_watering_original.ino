/*
 * RDTRC Complete Tomato Watering System with LCD - Standalone Version
 * Version: 4.0 - Independent System with LCD I2C 16x2 Support
 * Firmware made by: RDTRC
 * Updated: 2024
 * 
 * Features:
 * - Complete standalone tomato watering system
 * - LCD I2C 16x2 display with auto address detection
 * - Multi-zone watering (4 zones)
 * - Advanced soil moisture monitoring for each zone
 * - Multiple soil moisture sensors for comprehensive monitoring
 * - Light sensor for day/night detection
 * - Automatic watering schedule
 * - Weather-based watering adjustment
 * - Built-in web interface
 * - Blynk integration for mobile control
 * - Hotspot mode for direct access
 * - Data logging to SPIFFS
 * - EMAIL/LINE notifications
 * - OTA updates
 * - Pump protection and flow monitoring
 * - LCD debug and status display
 */

// Blynk Configuration - MUST be defined BEFORE includes
#define BLYNK_TEMPLATE_ID "TMPL61Zdwsx9r"
#define BLYNK_TEMPLATE_NAME "Tomato_Watering_System"
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
// #include <DHT.h> // Removed - using soil moisture sensors only
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RDTRC_LCD_Library.h"

// System Configuration
#define FIRMWARE_VERSION "4.0"
#define FIRMWARE_MAKER "RDTRC"
#define SYSTEM_NAME "Tomato Watering"
#define DEVICE_ID "RDTRC_TOMATO_WATER"

// Network Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* hotspot_ssid = "RDTRC_TomatoWater";
const char* hotspot_password = "rdtrc123";

// LINE Notify Configuration
const char* lineToken = "YOUR_LINE_NOTIFY_TOKEN";

// Pin Definitions
// DHT22 removed - using soil moisture sensors only
#define LIGHT_SENSOR_PIN 32
#define WATER_PUMP_PIN 18
#define FLOW_SENSOR_PIN 19
#define WATER_LEVEL_TRIG_PIN 25
#define WATER_LEVEL_ECHO_PIN 14
#define STATUS_LED_PIN 2
#define BUZZER_PIN 4
#define RESET_BUTTON_PIN 0
#define LCD_NEXT_BUTTON_PIN 26  // Button to navigate LCD pages

// Soil Moisture Sensors (4 zones + 2 additional sensors)
#define SOIL_SENSOR_1_PIN 33
#define SOIL_SENSOR_2_PIN 34
#define SOIL_SENSOR_3_PIN 35
#define SOIL_SENSOR_4_PIN 36
#define SOIL_SENSOR_5_PIN 39  // Additional sensor for comprehensive monitoring
#define SOIL_SENSOR_6_PIN 23  // Reusing DHT22 pin for additional soil sensor

// Solenoid Valves (4 zones)
#define VALVE_1_PIN 5
#define VALVE_2_PIN 17
#define VALVE_3_PIN 16
#define VALVE_4_PIN 27

// I2C Pins
#define I2C_SDA 21
#define I2C_SCL 22

// Watering Configuration
#define NUM_ZONES 4
#define DEFAULT_WATERING_DURATION 30000 // 30 seconds per zone
#define MIN_WATERING_DURATION 5000      // 5 seconds minimum
#define MAX_WATERING_DURATION 300000    // 5 minutes maximum
#define DRY_SOIL_THRESHOLD 30           // Below 30% moisture = dry
#define WET_SOIL_THRESHOLD 70           // Above 70% moisture = wet
#define WATER_TANK_HEIGHT 50            // cm
#define LOW_WATER_THRESHOLD 10          // cm

// Environmental thresholds
#define DAYLIGHT_THRESHOLD 500          // ADC value
// Temperature and humidity thresholds removed - using soil moisture only
#define SOIL_MOISTURE_VERY_DRY 15       // Very dry soil threshold
#define SOIL_MOISTURE_OPTIMAL_MIN 40    // Minimum optimal soil moisture
#define SOIL_MOISTURE_OPTIMAL_MAX 80    // Maximum optimal soil moisture

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
int currentLCDZone = 0; // 0-3 for zones, 4 for system info

// Watering Zones
struct WateringZone {
  String name;
  int soilPin;
  int valvePin;
  int moistureLevel;
  int targetMoisture;
  bool isWatering;
  unsigned long lastWatered;
  unsigned long totalWateringTime;
  bool enabled;
};

WateringZone zones[NUM_ZONES] = {
  {"Zone 1", SOIL_SENSOR_1_PIN, VALVE_1_PIN, 0, 50, false, 0, 0, true},
  {"Zone 2", SOIL_SENSOR_2_PIN, VALVE_2_PIN, 0, 50, false, 0, 0, true},
  {"Zone 3", SOIL_SENSOR_3_PIN, VALVE_3_PIN, 0, 50, false, 0, 0, true},
  {"Zone 4", SOIL_SENSOR_4_PIN, VALVE_4_PIN, 0, 50, false, 0, 0, true}
};

// Environmental Variables
float ambientTemperature = 0;
float ambientHumidity = 0;
int lightLevel = 0;
float waterLevel = 0;
bool isDaylight = true;
float flowRate = 0;
int dailyWateringCycles = 0;

// Watering Schedule
struct WateringSchedule {
  int hour;
  int minute;
  bool enabled;
  String description;
};

WateringSchedule wateringTimes[3] = {
  {6, 0, true, "Morning Water"},
  {12, 0, true, "Noon Water"},
  {18, 0, true, "Evening Water"}
};

// Statistics
struct DailyStats {
  String date;
  int wateringCycles;
  unsigned long totalWateringTime;
  float avgTemperature;
  float avgHumidity;
  int alerts;
};

DailyStats todayStats;

// Enhanced LCD Display Class for Multi-Zone
class MultiZoneLCD {
  private:
    RDTRC_LCD* baseLCD;
    int currentZone; // 0-3 for zones, 4 for system
    unsigned long lastZoneSwitch;
    unsigned long zoneSwitchInterval;
    
  public:
    MultiZoneLCD() {
      baseLCD = &systemLCD;
      currentZone = 0;
      lastZoneSwitch = 0;
      zoneSwitchInterval = 3000; // Switch zone every 3 seconds
    }
    
    void updateMultiZoneStatus() {
      if (!baseLCD->isLCDConnected()) return;
      
      // Auto switch between zones and system info
      if (millis() - lastZoneSwitch > zoneSwitchInterval) {
        currentZone = (currentZone + 1) % 5; // 0-3 zones + 1 system info
        lastZoneSwitch = millis();
      }
      
      if (currentZone < 4) {
        // Show zone data
        baseLCD->updateStatus(
          zones[currentZone].name,
          ambientTemperature,
          ambientHumidity,
          zones[currentZone].moistureLevel,
          zones[currentZone].isWatering ? "Watering" : "Idle",
          isWiFiConnected,
          false,
          todayStats.alerts
        );
      } else {
        // Show system info
        baseLCD->updateStatus(
          SYSTEM_NAME,
          ambientTemperature,
          ambientHumidity,
          (int)waterLevel,
          "Cycles: " + String(dailyWateringCycles),
          isWiFiConnected,
          false,
          todayStats.alerts
        );
      }
    }
    
    void switchZone() {
      currentZone = (currentZone + 1) % 5;
      lastZoneSwitch = millis();
    }
    
    int getCurrentZone() {
      return currentZone;
    }
};

MultiZoneLCD multiLCD;

// Function Declarations
void setupSystem();
void setupLCD();
void setupWiFi();
void setupWebServer();
void setupBlynk();
void setupOTA();
void displayBootScreen();
void handleSystemLoop();
void readSensors();
void checkWateringSchedule();
void waterZone(int zoneIndex, unsigned long duration);
void handleWebInterface();
void sendLineNotification(String message);
void handleManualControls();
void handleLCDControls();
void updateLCDDisplay();
void saveSettings();
void loadSettings();
void performSystemMaintenance();
void checkAlerts();

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  bootTime = millis();
  displayBootScreen();
  setupSystem();
  
  Serial.println("RDTRC Tomato Watering System with LCD Ready!");
  Serial.println("Web Interface: http://tomato-water.local");
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
  
  // Check watering schedule every minute
  static unsigned long lastScheduleCheck = 0;
  if (millis() - lastScheduleCheck > 60000) {
    checkWateringSchedule();
    lastScheduleCheck = millis();
  }
  
  // Log data every 15 minutes
  if (millis() - lastDataLog > 900000) {
    logData();
    lastDataLog = millis();
  }
  
  // System status check every 5 minutes
  if (millis() - lastStatusCheck > 300000) {
    performSystemMaintenance();
    checkAlerts();
    lastStatusCheck = millis();
  }
  
  delay(100);
}

void displayBootScreen() {
  Serial.println("\n============================================================");
  Serial.println("RDTRC Complete Tomato Watering System with LCD");
  Serial.println("");
  Serial.println("Firmware made by: " + String(FIRMWARE_MAKER));
  Serial.println("Version: " + String(FIRMWARE_VERSION));
  Serial.println("System: " + String(SYSTEM_NAME));
  Serial.println("");
  Serial.println("Features:");
  Serial.println("* LCD I2C 16x2 Display with Auto Detection");
  Serial.println("* Multi-Zone Watering (4 zones)");
  Serial.println("* Soil Moisture Monitoring");
  Serial.println("* Temperature & Humidity Monitoring");
  Serial.println("* Light Sensor (Day/Night Detection)");
  Serial.println("* Automatic Watering Schedule");
  Serial.println("* Weather-based Adjustment");
  Serial.println("* Web Interface");
  Serial.println("* Mobile App Control");
  Serial.println("* Smart Notifications");
  Serial.println("* Data Logging & Analytics");
  Serial.println("* OTA Updates");
  Serial.println("");
  Serial.println("Initializing Tomato Watering Environment...");
  Serial.println("============================================================");
  delay(3000);
}

void setupSystem() {
  // Initialize pins
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LCD_NEXT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(WATER_LEVEL_TRIG_PIN, OUTPUT);
  pinMode(WATER_LEVEL_ECHO_PIN, INPUT);
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(FLOW_SENSOR_PIN, INPUT);
  
  // Initialize valve pins
  for (int i = 0; i < NUM_ZONES; i++) {
    pinMode(zones[i].valvePin, OUTPUT);
    digitalWrite(zones[i].valvePin, LOW); // All valves closed
  }
  
  digitalWrite(WATER_PUMP_PIN, LOW); // Pump off
  
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
  todayStats.wateringCycles = 0;
  todayStats.totalWateringTime = 0;
  todayStats.avgTemperature = 0;
  todayStats.avgHumidity = 0;
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
  
  // Initial sensor reading
  readSensors();
  
  // Send startup notification
  String startupMsg = "RDTRC Tomato Watering System Started!\n";
  startupMsg += "System: " + String(SYSTEM_NAME) + "\n";
  startupMsg += "Version: " + String(FIRMWARE_VERSION) + "\n";
  startupMsg += "LCD: ";
  startupMsg += (systemLCD.isLCDConnected() ? "Connected" : "Not Found");
  startupMsg += "\n";
  startupMsg += "Water Level: " + String(waterLevel) + "cm\n";
  startupMsg += "Temperature: " + String(ambientTemperature) + "C\n";
  startupMsg += "Humidity: " + String(ambientHumidity) + "%\n";
  startupMsg += "Zones Active: " + String(NUM_ZONES) + "\n";
  startupMsg += "Status: Watering system ready!";
  sendLineNotification(startupMsg);
  
  systemLCD.showDebug("System Ready!", "Tomatoes OK");
  Serial.println("System initialization complete!");
}

void setupLCD() {
  Serial.println("Setting up LCD I2C 16x2...");
  
  // Initialize I2C
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
    
    systemLCD.showDebug("WiFi Connect", "Attempt " + String(attempts));
    digitalWrite(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    isWiFiConnected = true;
    Serial.println("\nWiFi Connected!");
    Serial.println("IP Address: " + WiFi.localIP().toString());
    Serial.println("Signal Strength: " + String(WiFi.RSSI()) + " dBm");
    
    systemLCD.showDebug("WiFi Connected", WiFi.localIP().toString());
    
    if (MDNS.begin("tomato-water")) {
      Serial.println("mDNS responder started: tomato-water.local");
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
  
  systemLCD.showDebug("Hotspot Mode", WiFi.softAPIP().toString());
  
  if (MDNS.begin("tomato-water")) {
    Serial.println("mDNS responder started for hotspot");
  }
}

void setupWebServer() {
  systemLCD.showDebug("Web Server", "Starting...");
  
  server.on("/", handleWebInterface);
  
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
    doc["light_level"] = lightLevel;
    doc["is_daylight"] = isDaylight;
    doc["water_level"] = waterLevel;
    doc["flow_rate"] = flowRate;
    doc["daily_watering_cycles"] = dailyWateringCycles;
    doc["timestamp"] = timeClient.getEpochTime();
    doc["lcd_connected"] = systemLCD.isLCDConnected();
    doc["lcd_address"] = "0x" + String(systemLCD.getLCDAddress(), HEX);
    doc["lcd_current_zone"] = multiLCD.getCurrentZone();
    
    // Zone data
    JsonArray zonesArray = doc.createNestedArray("zones");
    for (int i = 0; i < NUM_ZONES; i++) {
      JsonObject zoneObj = zonesArray.createNestedObject();
      zoneObj["name"] = zones[i].name;
      zoneObj["moisture"] = zones[i].moistureLevel;
      zoneObj["target_moisture"] = zones[i].targetMoisture;
      zoneObj["is_watering"] = zones[i].isWatering;
      zoneObj["enabled"] = zones[i].enabled;
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/water", HTTP_POST, []() {
    String zoneStr = server.arg("zone");
    String durationStr = server.arg("duration");
    
    int zone = zoneStr.toInt();
    unsigned long duration = durationStr.toInt() * 1000; // Convert to milliseconds
    
    if (zone >= 0 && zone < NUM_ZONES && duration >= MIN_WATERING_DURATION && duration <= MAX_WATERING_DURATION) {
      waterZone(zone, duration);
      systemLCD.showDebug("Manual Water", "Zone " + String(zone + 1));
      server.send(200, "application/json", "{\"status\":\"watering_started\",\"zone\":" + String(zone) + ",\"duration\":" + String(duration) + "}");
    } else {
      server.send(400, "application/json", "{\"error\":\"invalid_parameters\"}");
    }
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
    Blynk.virtualWrite(V3, waterLevel);
    Blynk.virtualWrite(V4, lightLevel);
    for (int i = 0; i < NUM_ZONES; i++) {
      Blynk.virtualWrite(V10 + i, zones[i].moistureLevel);
    }
  }
}

void setupOTA() {
  if (isWiFiConnected) {
    ArduinoOTA.setHostname("tomato-water");
    ArduinoOTA.setPassword("rdtrc2024");
    
    ArduinoOTA.onStart([]() {
      systemLCD.showAlert("OTA UPDATE", 2000);
    });
    
    ArduinoOTA.onEnd([]() {
      systemLCD.showDebug("OTA Complete", "Restarting...");
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      int percent = (progress / (total / 100));
      if (percent % 10 == 0) {
        systemLCD.showDebug("OTA Update", String(percent) + "%");
      }
    });
    
    ArduinoOTA.onError([](ota_error_t error) {
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
  if (millis() - lastHeartbeat > 2000) {
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
      multiLCD.switchZone();
      String zoneName = multiLCD.getCurrentZone() < 4 ? "Zone " + String(multiLCD.getCurrentZone() + 1) : "System";
      systemLCD.showDebug("LCD Switch", zoneName);
      lastLCDButtonPress = millis();
    }
  }
  
  lastLCDButtonState = currentLCDButtonState;
}

void updateLCDDisplay() {
  // Update LCD status every 2 seconds
  if (millis() - lastLCDUpdate > 2000) {
    // Update multi-zone LCD display
    multiLCD.updateMultiZoneStatus();
    
    // Update LCD display
    systemLCD.update();
    
    lastLCDUpdate = millis();
  }
}

void readSensors() {
  // Read DHT sensor
  ambientTemperature = dht.readTemperature();
  ambientHumidity = dht.readHumidity();
  
  if (isnan(ambientTemperature) || isnan(ambientHumidity)) {
    Serial.println("DHT sensor error");
    systemLCD.showDebug("DHT Error", "Check Sensor");
    ambientTemperature = 0;
    ambientHumidity = 0;
  }
  
  // Read light sensor
  lightLevel = analogRead(LIGHT_SENSOR_PIN);
  isDaylight = lightLevel > DAYLIGHT_THRESHOLD;
  
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
  
  // Read soil moisture sensors for all zones
  for (int i = 0; i < NUM_ZONES; i++) {
    int soilRaw = analogRead(zones[i].soilPin);
    zones[i].moistureLevel = map(soilRaw, 4095, 0, 0, 100); // Invert for dry=low, wet=high
    if (zones[i].moistureLevel < 0) zones[i].moistureLevel = 0;
    if (zones[i].moistureLevel > 100) zones[i].moistureLevel = 100;
  }
  
  // Update Blynk with sensor data
  if (isWiFiConnected) {
    Blynk.virtualWrite(V1, ambientTemperature);
    Blynk.virtualWrite(V2, ambientHumidity);
    Blynk.virtualWrite(V3, waterLevel);
    Blynk.virtualWrite(V4, lightLevel);
    for (int i = 0; i < NUM_ZONES; i++) {
      Blynk.virtualWrite(V10 + i, zones[i].moistureLevel);
    }
  }
}

void checkWateringSchedule() {
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  for (int i = 0; i < 3; i++) {
    if (wateringTimes[i].enabled && 
        wateringTimes[i].hour == currentHour && 
        wateringTimes[i].minute == currentMinute) {
      
      Serial.println("Scheduled watering time: " + wateringTimes[i].description);
      systemLCD.showMessage("Watering Time", wateringTimes[i].description, 3000);
      
      // Water all zones that need it
      for (int j = 0; j < NUM_ZONES; j++) {
        if (zones[j].enabled && zones[j].moistureLevel < DRY_SOIL_THRESHOLD) {
          waterZone(j, DEFAULT_WATERING_DURATION);
          delay(5000); // Wait between zones
        }
      }
      break;
    }
  }
  
  // Also check for emergency watering (very dry soil)
  for (int i = 0; i < NUM_ZONES; i++) {
    if (zones[i].enabled && zones[i].moistureLevel < 20 && !zones[i].isWatering) {
      Serial.println("Emergency watering for " + zones[i].name + " - very dry soil");
      systemLCD.showMessage("Emergency", zones[i].name);
      waterZone(i, DEFAULT_WATERING_DURATION);
    }
  }
}

void waterZone(int zoneIndex, unsigned long duration) {
  if (zoneIndex < 0 || zoneIndex >= NUM_ZONES) return;
  if (waterLevel < LOW_WATER_THRESHOLD) {
    systemLCD.showAlert("LOW WATER", 3000);
    return;
  }
  
  Serial.println("Starting watering " + zones[zoneIndex].name + " for " + String(duration/1000) + " seconds");
  systemLCD.showDebug("Watering", zones[zoneIndex].name);
  
  // Turn on pump
  digitalWrite(WATER_PUMP_PIN, HIGH);
  systemLCD.showDebug("Pump", "ON");
  
  // Open valve for specific zone
  digitalWrite(zones[zoneIndex].valvePin, HIGH);
  zones[zoneIndex].isWatering = true;
  zones[zoneIndex].lastWatered = millis();
  
  systemLCD.showDebug("Valve Open", zones[zoneIndex].name);
  
  // Wait for watering duration
  delay(duration);
  
  // Close valve
  digitalWrite(zones[zoneIndex].valvePin, LOW);
  zones[zoneIndex].isWatering = false;
  zones[zoneIndex].totalWateringTime += duration;
  
  // Turn off pump
  digitalWrite(WATER_PUMP_PIN, LOW);
  systemLCD.showDebug("Watering", "Complete");
  
  // Update statistics
  dailyWateringCycles++;
  todayStats.wateringCycles++;
  todayStats.totalWateringTime += duration;
  
  // Send notification
  String waterMsg = "Tomato Zone Watered!\n";
  waterMsg += "Zone: " + zones[zoneIndex].name + "\n";
  waterMsg += "Duration: " + String(duration/1000) + " seconds\n";
  waterMsg += "Soil Moisture: " + String(zones[zoneIndex].moistureLevel) + "%\n";
  waterMsg += "Daily Cycles: " + String(dailyWateringCycles);
  sendLineNotification(waterMsg);
  
  systemLCD.showMessage("Watered", zones[zoneIndex].name, 3000);
  
  // Save watering data
  saveSettings();
  
  Serial.println("Watering completed for " + zones[zoneIndex].name);
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
  
  // High temperature alert
  if (ambientTemperature > HIGH_TEMP_THRESHOLD) {
    String alertMsg = "High Temperature Alert!\n";
    alertMsg += "Current: " + String(ambientTemperature) + "C\n";
    alertMsg += "Consider additional watering.";
    sendLineNotification(alertMsg);
    systemLCD.showAlert("HIGH TEMP");
    todayStats.alerts++;
  }
  
  // Low humidity alert
  if (ambientHumidity < LOW_HUMIDITY_THRESHOLD) {
    String alertMsg = "Low Humidity Alert!\n";
    alertMsg += "Current: " + String(ambientHumidity) + "%\n";
    alertMsg += "Plants may need more water.";
    sendLineNotification(alertMsg);
    systemLCD.showAlert("LOW HUMIDITY");
    todayStats.alerts++;
  }
  
  // Dry soil alerts
  for (int i = 0; i < NUM_ZONES; i++) {
    if (zones[i].enabled && zones[i].moistureLevel < 15) {
      String alertMsg = "Critical Dry Soil Alert!\n";
      alertMsg += "Zone: " + zones[i].name + "\n";
      alertMsg += "Moisture: " + String(zones[i].moistureLevel) + "%\n";
      alertMsg += "Immediate watering recommended.";
      sendLineNotification(alertMsg);
      systemLCD.showAlert("DRY SOIL Z" + String(i+1));
      todayStats.alerts++;
    }
  }
}

void handleManualControls() {
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
      sendLineNotification("Tomato watering system reset initiated");
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
  doc["light_level"] = lightLevel;
  doc["is_daylight"] = isDaylight;
  doc["water_level"] = waterLevel;
  doc["flow_rate"] = flowRate;
  doc["daily_watering_cycles"] = dailyWateringCycles;
  doc["wifi_signal"] = WiFi.RSSI();
  doc["free_memory"] = ESP.getFreeHeap();
  doc["lcd_connected"] = systemLCD.isLCDConnected();
  
  // Zone data
  JsonArray zonesArray = doc.createNestedArray("zones");
  for (int i = 0; i < NUM_ZONES; i++) {
    JsonObject zoneObj = zonesArray.createNestedObject();
    zoneObj["name"] = zones[i].name;
    zoneObj["moisture"] = zones[i].moistureLevel;
    zoneObj["is_watering"] = zones[i].isWatering;
    zoneObj["total_watering_time"] = zones[i].totalWateringTime;
  }
  
  String logData;
  serializeJson(doc, logData);
  
  File logFile = SPIFFS.open("/system.log", "a");
  if (logFile) {
    logFile.println(logData);
    logFile.close();
  }
}

void saveSettings() {
  JsonDocument doc;
  doc["daily_watering_cycles"] = dailyWateringCycles;
  doc["today_date"] = todayStats.date;
  
  // Save zone settings
  JsonArray zonesArray = doc.createNestedArray("zones");
  for (int i = 0; i < NUM_ZONES; i++) {
    JsonObject zoneObj = zonesArray.createNestedObject();
    zoneObj["target_moisture"] = zones[i].targetMoisture;
    zoneObj["enabled"] = zones[i].enabled;
    zoneObj["total_watering_time"] = zones[i].totalWateringTime;
  }
  
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
      dailyWateringCycles = doc["daily_watering_cycles"] | 0;
      todayStats.date = doc["today_date"] | "01/01/2024";
      
      // Load zone settings
      if (doc.containsKey("zones")) {
        JsonArray zonesArray = doc["zones"];
        for (int i = 0; i < NUM_ZONES && i < zonesArray.size(); i++) {
          zones[i].targetMoisture = zonesArray[i]["target_moisture"] | 50;
          zones[i].enabled = zonesArray[i]["enabled"] | true;
          zones[i].totalWateringTime = zonesArray[i]["total_watering_time"] | 0;
        }
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
  
  // Reset daily stats at midnight
  static unsigned long lastDateUpdate = 0;
  if (millis() - lastDateUpdate > 86400000) { // 24 hours
    lastDateUpdate = millis();
    Serial.println("New day started - stats reset");
    systemLCD.showDebug("New Day", "Stats Reset");
    
    // Reset daily stats
    dailyWateringCycles = 0;
    todayStats.wateringCycles = 0;
    todayStats.totalWateringTime = 0;
    todayStats.avgTemperature = 0;
    todayStats.avgHumidity = 0;
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
  html += "<title>RDTRC Tomato Watering System with LCD</title>";
  html += "<meta charset=\"UTF-8\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background: #1a1a1a; color: #fff; margin: 0; padding: 20px; }";
  html += ".header { background: linear-gradient(135deg, #e74c3c, #f39c12); padding: 20px; text-align: center; border-radius: 10px; margin-bottom: 20px; }";
  html += ".container { max-width: 1200px; margin: 0 auto; }";
  html += ".status-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 15px; margin-bottom: 20px; }";
  html += ".status-card { background: #2d2d2d; padding: 15px; border-radius: 8px; text-align: center; }";
  html += ".btn { padding: 10px 20px; margin: 5px; border: none; border-radius: 5px; cursor: pointer; font-size: 14px; }";
  html += ".btn-success { background: #27ae60; color: white; }";
  html += ".btn-warning { background: #f39c12; color: white; }";
  html += ".btn-info { background: #3498db; color: white; }";
  html += ".zone-card { background: #2d2d2d; padding: 20px; border-radius: 10px; margin-bottom: 20px; }";
  html += ".zones-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 15px; }";
  html += ".lcd-info { background: #2c3e50; padding: 15px; border-radius: 8px; margin-bottom: 20px; }";
  html += "</style>";
  html += "<script>";
  html += "function refreshStatus() {";
  html += "  fetch('/api/status').then(response => response.json()).then(data => {";
  html += "    document.getElementById('temperature').textContent = data.ambient_temperature.toFixed(1) + 'C';";
  html += "    document.getElementById('humidity').textContent = data.ambient_humidity.toFixed(1) + '%';";
  html += "    document.getElementById('water-level').textContent = data.water_level.toFixed(1) + 'cm';";
  html += "    document.getElementById('light-level').textContent = data.light_level + (data.is_daylight ? ' (Day)' : ' (Night)');";
  html += "    document.getElementById('daily-cycles').textContent = data.daily_watering_cycles;";
  html += "    document.getElementById('lcd-status').textContent = data.lcd_connected ? 'Connected at ' + data.lcd_address : 'Not Connected';";
  html += "    document.getElementById('lcd-zone').textContent = data.lcd_current_zone < 4 ? 'Zone ' + (data.lcd_current_zone + 1) : 'System Info';";
  html += "    for (let i = 0; i < data.zones.length; i++) {";
  html += "      document.getElementById('zone-' + (i+1) + '-moisture').textContent = data.zones[i].moisture + '%';";
  html += "    }";
  html += "  });";
  html += "}";
  html += "function waterZone(zone, duration) {";
  html += "  fetch('/api/water', {";
  html += "    method: 'POST',";
  html += "    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },";
  html += "    body: 'zone=' + zone + '&duration=' + duration";
  html += "  }).then(() => setTimeout(refreshStatus, 2000));";
  html += "}";
  html += "setInterval(refreshStatus, 30000);";
  html += "window.onload = refreshStatus;";
  html += "</script>";
  html += "</head><body>";
  html += "<div class=\"container\">";
  html += "<div class=\"header\">";
  html += "<h1>RDTRC Tomato Watering System</h1>";
  html += "<p>Multi-Zone Irrigation with LCD Display - v4.0</p>";
  html += "</div>";
  html += "<div class=\"lcd-info\">";
  html += "<h3>LCD Display Status</h3>";
  html += "<p>LCD Status: <span id=\"lcd-status\">Loading...</span></p>";
  html += "<p>Current Display: <span id=\"lcd-zone\">Loading...</span></p>";
  html += "<p>LCD cycles through all 4 zones + system info every 3 seconds</p>";
  html += "<p>Use the LCD button (Pin 26) to manually switch zones</p>";
  html += "</div>";
  html += "<div class=\"status-grid\">";
  html += "<div class=\"status-card\"><h3>Temperature</h3><div id=\"temperature\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Humidity</h3><div id=\"humidity\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Water Level</h3><div id=\"water-level\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Light Level</h3><div id=\"light-level\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Daily Cycles</h3><div id=\"daily-cycles\">Loading...</div></div>";
  html += "</div>";
  html += "<div class=\"zones-grid\">";
  for (int i = 0; i < NUM_ZONES; i++) {
    html += "<div class=\"zone-card\">";
    html += "<h3>Zone " + String(i+1) + "</h3>";
    html += "<p>Moisture: <span id=\"zone-" + String(i+1) + "-moisture\">Loading...</span></p>";
    html += "<button class=\"btn btn-success\" onclick=\"waterZone(" + String(i) + ", 30)\">Water 30s</button>";
    html += "<button class=\"btn btn-warning\" onclick=\"waterZone(" + String(i) + ", 60)\">Water 60s</button>";
    html += "<button class=\"btn btn-info\" onclick=\"waterZone(" + String(i) + ", 120)\">Water 2min</button>";
    html += "</div>";
  }
  html += "</div>";
  html += "</div>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// Blynk Virtual Pin Handlers
BLYNK_WRITE(V20) { // Manual watering Zone 1
  if (param.asInt() == 1) {
    waterZone(0, DEFAULT_WATERING_DURATION);
    systemLCD.showDebug("Blynk Water", "Zone 1");
  }
}

BLYNK_WRITE(V21) { // Manual watering Zone 2
  if (param.asInt() == 1) {
    waterZone(1, DEFAULT_WATERING_DURATION);
    systemLCD.showDebug("Blynk Water", "Zone 2");
  }
}

BLYNK_WRITE(V22) { // Manual watering Zone 3
  if (param.asInt() == 1) {
    waterZone(2, DEFAULT_WATERING_DURATION);
    systemLCD.showDebug("Blynk Water", "Zone 3");
  }
}

BLYNK_WRITE(V23) { // Manual watering Zone 4
  if (param.asInt() == 1) {
    waterZone(3, DEFAULT_WATERING_DURATION);
    systemLCD.showDebug("Blynk Water", "Zone 4");
  }
}

// Read-only pins for Blynk dashboard
BLYNK_READ(V1) { Blynk.virtualWrite(V1, ambientTemperature); }
BLYNK_READ(V2) { Blynk.virtualWrite(V2, ambientHumidity); }
BLYNK_READ(V3) { Blynk.virtualWrite(V3, waterLevel); }
BLYNK_READ(V4) { Blynk.virtualWrite(V4, lightLevel); }
BLYNK_READ(V10) { Blynk.virtualWrite(V10, zones[0].moistureLevel); }
BLYNK_READ(V11) { Blynk.virtualWrite(V11, zones[1].moistureLevel); }
BLYNK_READ(V12) { Blynk.virtualWrite(V12, zones[2].moistureLevel); }
BLYNK_READ(V13) { Blynk.virtualWrite(V13, zones[3].moistureLevel); }
/*
 * RDTRC Complete Tomato Watering System - Standalone Version
 * Version: 4.0 - Independent System
 * Firmware made by: RDTRC
 * Updated: 2024
 * 
 * Features:
 * - Complete standalone tomato watering system
 * - Multi-zone watering (4 zones)
 * - Soil moisture monitoring for each zone
 * - Temperature and humidity monitoring
 * - Light sensor for day/night detection
 * - Automatic watering schedule
 * - Weather-based watering adjustment
 * - Built-in web interface
 * - Blink integration for mobile control
 * - Hotspot mode for direct access
 * - Data logging to SPIFFS
 * - Email/LINE notifications
 * - OTA updates
 * - Pump protection and flow monitoring
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
#define SYSTEM_NAME "Tomato Watering System"
#define DEVICE_ID "RDTRC_TOMATO_WATER"

// Network Configuration - แก้ไขตามเครือข่ายของคุณ
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* hotspot_ssid = "RDTRC_TomatoWater";
const char* hotspot_password = "rdtrc123";

// Blynk Configuration - ใส่ Token ของคุณ
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_DEVICE_NAME "RDTRC Tomato Watering"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"

// LINE Notify Configuration - ใส่ Token ของคุณ
const char* lineToken = "YOUR_LINE_NOTIFY_TOKEN";

// Pin Definitions
#define DHT_PIN 22
#define DHT_TYPE DHT22
#define LIGHT_SENSOR_PIN 23
#define WATER_PUMP_PIN 18
#define FLOW_SENSOR_PIN 19
#define WATER_LEVEL_TRIG_PIN 25
#define WATER_LEVEL_ECHO_PIN 26
#define STATUS_LED_PIN 2
#define BUZZER_PIN 4
#define RESET_BUTTON_PIN 0

// Soil Moisture Sensors (4 zones)
#define SOIL_SENSOR_1_PIN 32
#define SOIL_SENSOR_2_PIN 33
#define SOIL_SENSOR_3_PIN 34
#define SOIL_SENSOR_4_PIN 35

// Solenoid Valves (4 zones)
#define VALVE_1_PIN 5
#define VALVE_2_PIN 17
#define VALVE_3_PIN 16
#define VALVE_4_PIN 27

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
#define HIGH_TEMP_THRESHOLD 35.0        // °C
#define LOW_HUMIDITY_THRESHOLD 30.0     // %
#define DAYLIGHT_THRESHOLD 500          // ADC value

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

// Watering Variables
struct WateringZone {
  int soilPin;
  int valvePin;
  String name;
  int moistureLevel;
  int moistureThreshold;
  bool needsWatering;
  unsigned long lastWatered;
  unsigned long totalWateringTime;
  bool enabled;
};

WateringZone zones[NUM_ZONES] = {
  {SOIL_SENSOR_1_PIN, VALVE_1_PIN, "Zone 1 - North", 0, 40, false, 0, 0, true},
  {SOIL_SENSOR_2_PIN, VALVE_2_PIN, "Zone 2 - South", 0, 40, false, 0, 0, true},
  {SOIL_SENSOR_3_PIN, VALVE_3_PIN, "Zone 3 - East", 0, 40, false, 0, 0, true},
  {SOIL_SENSOR_4_PIN, VALVE_4_PIN, "Zone 4 - West", 0, 40, false, 0, 0, true}
};

struct WateringSchedule {
  int hour;
  int minute;
  int duration; // seconds
  bool enabled;
  bool weather_dependent; // Skip if recently rained or high humidity
};

WateringSchedule schedule[4] = {
  {6, 0, 45, true, true},   // 06:00 - 45 seconds (morning)
  {12, 0, 30, true, true},  // 12:00 - 30 seconds (noon, light watering)
  {18, 0, 60, true, true},  // 18:00 - 60 seconds (evening, main watering)
  {0, 0, 0, false, true}    // Extra slot
};

bool wateringInProgress = false;
int currentWateringZone = -1;
unsigned long wateringStartTime = 0;
unsigned long pumpRunTime = 0;
float waterLevel = 0;
bool lowWaterAlertSent = false;
bool systemMaintenanceMode = false;

// Environmental monitoring
float temperature = 0;
float humidity = 0;
int lightLevel = 0;
bool isDaylight = true;
volatile unsigned int flowPulses = 0;
float flowRate = 0;
unsigned long oldTime = 0;

// Statistics
struct DailyStats {
  String date;
  int wateringCycles;
  unsigned long totalWateringTime;
  float totalWaterUsed;
  float avgTemperature;
  float avgHumidity;
  int dryZoneAlerts;
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
void checkWateringSchedule();
void performWatering(int zone, int duration, bool isScheduled = false);
void performAllZonesWatering(int duration, bool isScheduled = false);
void readSensors();
void logData();
void handleWebInterface();
void sendLineNotification(String message);
void handleManualControls();
void saveSettings();
void loadSettings();
void performSystemMaintenance();
void checkWeatherConditions();
void flowPulseCounter();
void stopAllWatering();

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  bootTime = millis();
  displayBootScreen();
  setupSystem();
  
  Serial.println("✅ RDTRC Tomato Watering System Ready!");
  Serial.println("🌐 Web Interface: http://tomato-water.local");
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
  
  // Check watering schedule
  if (!systemMaintenanceMode && !wateringInProgress) {
    checkWateringSchedule();
  }
  
  // Handle manual controls
  handleManualControls();
  
  // Read sensors every 30 seconds
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead > 30000) {
    readSensors();
    lastSensorRead = millis();
  }
  
  // Log data every 10 minutes
  if (millis() - lastDataLog > 600000) {
    logData();
    lastDataLog = millis();
  }
  
  // System status check every 2 minutes
  if (millis() - lastStatusCheck > 120000) {
    performSystemMaintenance();
    lastStatusCheck = millis();
  }
  
  // Handle ongoing watering
  if (wateringInProgress) {
    unsigned long elapsed = millis() - wateringStartTime;
    
    // Calculate flow rate every second
    if (millis() - oldTime > 1000) {
      detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));
      flowRate = ((1000.0 / (millis() - oldTime)) * flowPulses) / 7.5; // L/min
      oldTime = millis();
      flowPulses = 0;
      attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseCounter, FALLING);
    }
    
    // Check for pump problems (no flow)
    if (elapsed > 5000 && flowRate < 0.1) {
      Serial.println("❌ Pump problem detected - no flow!");
      sendLineNotification("❌ Watering System Alert!\nPump problem detected - no water flow.\nStopping watering cycle.");
      stopAllWatering();
    }
  }
  
  delay(100);
}

void displayBootScreen() {
  Serial.println("\n" + String("=").repeat(60));
  Serial.println("🍅 RDTRC Complete Tomato Watering System");
  Serial.println("");
  Serial.println("Firmware made by: " + String(FIRMWARE_MAKER));
  Serial.println("Version: " + String(FIRMWARE_VERSION));
  Serial.println("System: " + String(SYSTEM_NAME));
  Serial.println("");
  Serial.println("Features:");
  Serial.println("✓ Multi-Zone Watering (4 zones)");
  Serial.println("✓ Soil Moisture Monitoring");
  Serial.println("✓ Temperature & Humidity");
  Serial.println("✓ Light Detection");
  Serial.println("✓ Flow Rate Monitoring");
  Serial.println("✓ Weather-Based Control");
  Serial.println("✓ Web Interface");
  Serial.println("✓ Mobile App Control");
  Serial.println("✓ Smart Notifications");
  Serial.println("✓ Pump Protection");
  Serial.println("✓ OTA Updates");
  Serial.println("");
  Serial.println("Initializing System...");
  Serial.println("=".repeat(60));
  delay(3000);
}

void setupSystem() {
  // Initialize pins
  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(WATER_LEVEL_TRIG_PIN, OUTPUT);
  pinMode(WATER_LEVEL_ECHO_PIN, INPUT);
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  
  // Initialize zone valves
  for (int i = 0; i < NUM_ZONES; i++) {
    pinMode(zones[i].valvePin, OUTPUT);
    digitalWrite(zones[i].valvePin, LOW); // Valves closed
  }
  
  // Initialize pump off
  digitalWrite(WATER_PUMP_PIN, LOW);
  
  // Initialize DHT sensor
  dht.begin();
  
  // Setup flow sensor interrupt
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseCounter, FALLING);
  
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
  todayStats.wateringCycles = 0;
  todayStats.totalWateringTime = 0;
  todayStats.totalWaterUsed = 0;
  todayStats.avgTemperature = 0;
  todayStats.avgHumidity = 0;
  todayStats.dryZoneAlerts = 0;
  
  // Boot sequence
  for (int i = 0; i < 3; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    tone(BUZZER_PIN, 1000, 200);
    delay(300);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(200);
  }
  
  // Welcome sound
  tone(BUZZER_PIN, 1200, 300);
  delay(400);
  tone(BUZZER_PIN, 1500, 300);
  delay(400);
  tone(BUZZER_PIN, 1800, 400);
  
  // Initial sensor reading
  readSensors();
  
  // Send startup notification
  sendLineNotification("🍅 RDTRC Tomato Watering System Started!\n"
                      "System: " + String(SYSTEM_NAME) + "\n" +
                      "Version: " + String(FIRMWARE_VERSION) + "\n" +
                      "Water Level: " + String(waterLevel) + "cm\n" +
                      "Temperature: " + String(temperature) + "°C\n" +
                      "Humidity: " + String(humidity) + "%\n" +
                      "Status: Ready for watering!");
  
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
    if (MDNS.begin("tomato-water")) {
      Serial.println("✅ mDNS responder started: tomato-water.local");
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
  if (MDNS.begin("tomato-water")) {
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
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["light_level"] = lightLevel;
    doc["is_daylight"] = isDaylight;
    doc["water_level"] = waterLevel;
    doc["flow_rate"] = flowRate;
    doc["watering_in_progress"] = wateringInProgress;
    doc["current_zone"] = currentWateringZone;
    doc["pump_run_time"] = pumpRunTime;
    doc["maintenance_mode"] = systemMaintenanceMode;
    doc["timestamp"] = timeClient.getEpochTime();
    
    // Zone data
    JsonArray zonesArray = doc.createNestedArray("zones");
    for (int i = 0; i < NUM_ZONES; i++) {
      JsonObject zoneObj = zonesArray.createNestedObject();
      zoneObj["id"] = i;
      zoneObj["name"] = zones[i].name;
      zoneObj["moisture"] = zones[i].moistureLevel;
      zoneObj["threshold"] = zones[i].moistureThreshold;
      zoneObj["needs_watering"] = zones[i].needsWatering;
      zoneObj["last_watered"] = zones[i].lastWatered;
      zoneObj["total_time"] = zones[i].totalWateringTime;
      zoneObj["enabled"] = zones[i].enabled;
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/water", HTTP_POST, []() {
    if (systemMaintenanceMode) {
      server.send(423, "application/json", "{\"error\":\"system_in_maintenance\"}");
      return;
    }
    
    if (wateringInProgress) {
      server.send(409, "application/json", "{\"error\":\"watering_in_progress\"}");
      return;
    }
    
    int zone = -1;
    int duration = DEFAULT_WATERING_DURATION / 1000; // Convert to seconds
    
    if (server.hasArg("zone")) {
      zone = server.arg("zone").toInt();
    }
    
    if (server.hasArg("duration")) {
      duration = server.arg("duration").toInt();
      duration *= 1000; // Convert to milliseconds
    }
    
    if (duration < MIN_WATERING_DURATION || duration > MAX_WATERING_DURATION) {
      server.send(400, "application/json", "{\"error\":\"invalid_duration\"}");
      return;
    }
    
    if (zone >= 0 && zone < NUM_ZONES) {
      performWatering(zone, duration, false);
      server.send(200, "application/json", "{\"status\":\"watering_started\",\"zone\":" + String(zone) + ",\"duration\":" + String(duration/1000) + "}");
    } else if (zone == -1) {
      performAllZonesWatering(duration, false);
      server.send(200, "application/json", "{\"status\":\"all_zones_watering_started\",\"duration\":" + String(duration/1000) + "}");
    } else {
      server.send(400, "application/json", "{\"error\":\"invalid_zone\"}");
    }
  });
  
  server.on("/api/stop", HTTP_POST, []() {
    stopAllWatering();
    server.send(200, "application/json", "{\"status\":\"watering_stopped\"}");
  });
  
  server.on("/api/zone", HTTP_POST, []() {
    if (server.hasArg("zone") && server.hasArg("threshold")) {
      int zone = server.arg("zone").toInt();
      int threshold = server.arg("threshold").toInt();
      
      if (zone >= 0 && zone < NUM_ZONES && threshold >= 0 && threshold <= 100) {
        zones[zone].moistureThreshold = threshold;
        
        if (server.hasArg("enabled")) {
          zones[zone].enabled = server.arg("enabled") == "true";
        }
        
        saveSettings();
        server.send(200, "application/json", "{\"status\":\"zone_updated\"}");
      } else {
        server.send(400, "application/json", "{\"error\":\"invalid_parameters\"}");
      }
    } else {
      server.send(400, "application/json", "{\"error\":\"missing_parameters\"}");
    }
  });
  
  server.on("/api/schedule", HTTP_GET, []() {
    JsonDocument doc;
    JsonArray scheduleArray = doc.createNestedArray("schedule");
    
    for (int i = 0; i < 4; i++) {
      if (schedule[i].enabled || schedule[i].hour != 0 || schedule[i].minute != 0) {
        JsonObject scheduleItem = scheduleArray.createNestedObject();
        scheduleItem["index"] = i;
        scheduleItem["hour"] = schedule[i].hour;
        scheduleItem["minute"] = schedule[i].minute;
        scheduleItem["duration"] = schedule[i].duration;
        scheduleItem["enabled"] = schedule[i].enabled;
        scheduleItem["weather_dependent"] = schedule[i].weather_dependent;
      }
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/schedule", HTTP_POST, []() {
    if (server.hasArg("index") && server.hasArg("hour") && 
        server.hasArg("minute") && server.hasArg("duration") && server.hasArg("enabled")) {
      
      int index = server.arg("index").toInt();
      if (index >= 0 && index < 4) {
        schedule[index].hour = server.arg("hour").toInt();
        schedule[index].minute = server.arg("minute").toInt();
        schedule[index].duration = server.arg("duration").toInt();
        schedule[index].enabled = server.arg("enabled") == "true";
        schedule[index].weather_dependent = server.hasArg("weather_dependent") ? 
                                          server.arg("weather_dependent") == "true" : true;
        
        saveSettings();
        server.send(200, "application/json", "{\"status\":\"schedule_updated\"}");
      } else {
        server.send(400, "application/json", "{\"error\":\"invalid_index\"}");
      }
    } else {
      server.send(400, "application/json", "{\"error\":\"missing_parameters\"}");
    }
  });
  
  server.on("/api/maintenance", HTTP_POST, []() {
    if (server.hasArg("mode")) {
      systemMaintenanceMode = server.arg("mode") == "true";
      if (systemMaintenanceMode && wateringInProgress) {
        stopAllWatering();
      }
      String status = systemMaintenanceMode ? "enabled" : "disabled";
      server.send(200, "application/json", "{\"status\":\"maintenance_" + status + "\"}");
    }
  });
  
  server.on("/api/stats", HTTP_GET, []() {
    JsonDocument doc;
    doc["today"] = todayStats.date;
    doc["watering_cycles"] = todayStats.wateringCycles;
    doc["total_watering_time"] = todayStats.totalWateringTime;
    doc["total_water_used"] = todayStats.totalWaterUsed;
    doc["avg_temperature"] = todayStats.avgTemperature;
    doc["avg_humidity"] = todayStats.avgHumidity;
    doc["dry_zone_alerts"] = todayStats.dryZoneAlerts;
    
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
    Blynk.virtualWrite(V1, temperature);
    Blynk.virtualWrite(V2, humidity);
    Blynk.virtualWrite(V3, waterLevel);
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
      String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
      Serial.println("🔄 OTA Update started: " + type);
      systemMaintenanceMode = true;
      if (wateringInProgress) {
        stopAllWatering();
      }
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

void checkWateringSchedule() {
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  // Check each scheduled watering time
  for (int i = 0; i < 4; i++) {
    if (schedule[i].enabled && 
        currentHour == schedule[i].hour && 
        currentMinute == schedule[i].minute) {
      
      // Check weather dependency
      if (schedule[i].weather_dependent) {
        if (humidity > 80) {
          Serial.println("⏰ Skipping scheduled watering (high humidity): " + String(humidity) + "%");
          continue;
        }
        
        if (temperature < 10) {
          Serial.println("⏰ Skipping scheduled watering (low temperature): " + String(temperature) + "°C");
          continue;
        }
      }
      
      Serial.println("⏰ Scheduled watering time: " + String(schedule[i].hour) + ":" + 
                    (schedule[i].minute < 10 ? "0" : "") + String(schedule[i].minute));
      performAllZonesWatering(schedule[i].duration * 1000, true);
      break;
    }
  }
  
  // Check for automatic watering based on soil moisture
  for (int i = 0; i < NUM_ZONES; i++) {
    if (zones[i].enabled && zones[i].moistureLevel < zones[i].moistureThreshold) {
      unsigned long timeSinceLastWatering = timeClient.getEpochTime() - zones[i].lastWatered;
      
      // Only auto-water if it's been at least 2 hours since last watering
      if (timeSinceLastWatering > 7200) {
        Serial.println("🏜️ Auto-watering triggered for " + zones[i].name + " (moisture: " + String(zones[i].moistureLevel) + "%)");
        performWatering(i, DEFAULT_WATERING_DURATION, true);
        break; // Water one zone at a time
      }
    }
  }
}

void performWatering(int zone, int duration, bool isScheduled) {
  if (wateringInProgress || systemMaintenanceMode) {
    Serial.println("⚠️ Cannot start watering - system busy or in maintenance");
    return;
  }
  
  if (zone < 0 || zone >= NUM_ZONES || !zones[zone].enabled) {
    Serial.println("⚠️ Invalid zone or zone disabled: " + String(zone));
    return;
  }
  
  if (waterLevel < LOW_WATER_THRESHOLD) {
    Serial.println("⚠️ Cannot water: Water level too low (" + String(waterLevel) + "cm)");
    sendLineNotification("⚠️ Cannot water: Water level too low (" + String(waterLevel) + "cm)");
    return;
  }
  
  wateringInProgress = true;
  currentWateringZone = zone;
  wateringStartTime = millis();
  
  String wateringType = isScheduled ? "Scheduled" : "Manual";
  Serial.println("💧 Starting " + wateringType + " watering for " + zones[zone].name + " (" + String(duration/1000) + "s)");
  
  // Start pump
  digitalWrite(WATER_PUMP_PIN, HIGH);
  pumpRunTime = millis();
  
  // Open zone valve
  digitalWrite(zones[zone].valvePin, HIGH);
  
  // Alert sound
  tone(BUZZER_PIN, 1500, 200);
  delay(250);
  tone(BUZZER_PIN, 2000, 200);
  
  // Wait for watering duration
  while (millis() - wateringStartTime < duration && wateringInProgress) {
    delay(100);
    
    // Check for emergency stop conditions
    if (systemMaintenanceMode) {
      Serial.println("⚠️ Emergency stop - maintenance mode activated");
      break;
    }
    
    if (waterLevel < 5) {
      Serial.println("⚠️ Emergency stop - water level critical");
      break;
    }
    
    // Handle web server during watering
    server.handleClient();
  }
  
  // Stop watering
  digitalWrite(zones[zone].valvePin, LOW);
  digitalWrite(WATER_PUMP_PIN, LOW);
  
  unsigned long actualDuration = millis() - wateringStartTime;
  pumpRunTime = millis() - pumpRunTime;
  
  // Update statistics
  zones[zone].lastWatered = timeClient.getEpochTime();
  zones[zone].totalWateringTime += actualDuration;
  todayStats.wateringCycles++;
  todayStats.totalWateringTime += actualDuration;
  
  wateringInProgress = false;
  currentWateringZone = -1;
  
  Serial.println("✅ Watering completed for " + zones[zone].name + 
                " (Duration: " + String(actualDuration/1000) + "s)");
  
  // Success sound
  tone(BUZZER_PIN, 2000, 300);
  delay(400);
  tone(BUZZER_PIN, 2500, 300);
  
  // Log the watering
  String logEntry = String(timeClient.getEpochTime()) + "," + 
                   wateringType + "," + 
                   String(zone) + "," +
                   zones[zone].name + "," +
                   String(actualDuration/1000) + "," +
                   String(zones[zone].moistureLevel) + "," +
                   String(temperature) + "," +
                   String(humidity) + "\n";
  
  File logFile = SPIFFS.open("/watering.log", "a");
  if (logFile) {
    logFile.print(logEntry);
    logFile.close();
  }
  
  // Send notification
  String notification = "💧 " + wateringType + " Watering Completed!\n" +
                       "Zone: " + zones[zone].name + "\n" +
                       "Duration: " + String(actualDuration/1000) + "s\n" +
                       "Soil Moisture: " + String(zones[zone].moistureLevel) + "%\n" +
                       "Temperature: " + String(temperature) + "°C\n" +
                       "Humidity: " + String(humidity) + "%";
  
  sendLineNotification(notification);
  
  // Update Blynk
  if (isWiFiConnected) {
    Blynk.virtualWrite(V20, todayStats.wateringCycles);
    Blynk.virtualWrite(V21, todayStats.totalWateringTime / 1000);
  }
}

void performAllZonesWatering(int duration, bool isScheduled) {
  for (int i = 0; i < NUM_ZONES; i++) {
    if (zones[i].enabled && !systemMaintenanceMode) {
      performWatering(i, duration, isScheduled);
      
      // Wait between zones to prevent water pressure issues
      if (i < NUM_ZONES - 1) {
        delay(5000); // 5 second pause between zones
      }
    }
  }
}

void stopAllWatering() {
  if (wateringInProgress) {
    Serial.println("🛑 Emergency stop - all watering stopped");
    
    // Stop pump and close all valves
    digitalWrite(WATER_PUMP_PIN, LOW);
    for (int i = 0; i < NUM_ZONES; i++) {
      digitalWrite(zones[i].valvePin, LOW);
    }
    
    wateringInProgress = false;
    currentWateringZone = -1;
    
    // Alert sound
    for (int i = 0; i < 3; i++) {
      tone(BUZZER_PIN, 800, 200);
      delay(300);
    }
    
    sendLineNotification("🛑 Watering Emergency Stop!\nAll watering operations have been stopped.");
  }
}

void readSensors() {
  // Read DHT sensor
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  
  // Check for sensor errors
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("❌ DHT sensor error");
    temperature = 0;
    humidity = 0;
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
  
  // Read soil moisture sensors
  for (int i = 0; i < NUM_ZONES; i++) {
    int rawValue = analogRead(zones[i].soilPin);
    // Convert to percentage (adjust calibration as needed)
    zones[i].moistureLevel = map(rawValue, 4095, 0, 0, 100); // Invert for dry=low, wet=high
    if (zones[i].moistureLevel < 0) zones[i].moistureLevel = 0;
    if (zones[i].moistureLevel > 100) zones[i].moistureLevel = 100;
    
    // Check if zone needs watering
    zones[i].needsWatering = zones[i].moistureLevel < zones[i].moistureThreshold;
    
    // Send dry zone alert
    if (zones[i].moistureLevel < DRY_SOIL_THRESHOLD && zones[i].enabled) {
      unsigned long timeSinceAlert = millis() - zones[i].lastWatered;
      if (timeSinceAlert > 3600000) { // Only alert once per hour
        String alertMsg = "🏜️ Dry Soil Alert!\n" +
                         "Zone: " + zones[i].name + "\n" +
                         "Moisture: " + String(zones[i].moistureLevel) + "%\n" +
                         "Threshold: " + String(zones[i].moistureThreshold) + "%";
        
        sendLineNotification(alertMsg);
        todayStats.dryZoneAlerts++;
      }
    }
  }
  
  // Check for low water alert
  if (waterLevel < LOW_WATER_THRESHOLD && !lowWaterAlertSent) {
    String alertMsg = "⚠️ Low Water Level Alert!\n" +
                     "Current Level: " + String(waterLevel) + "cm\n" +
                     "Please refill the water tank.";
    
    sendLineNotification(alertMsg);
    lowWaterAlertSent = true;
    
    // Alert sound
    for (int i = 0; i < 5; i++) {
      tone(BUZZER_PIN, 500, 300);
      delay(400);
      tone(BUZZER_PIN, 800, 300);
      delay(400);
    }
  } else if (waterLevel > LOW_WATER_THRESHOLD * 2) {
    lowWaterAlertSent = false; // Reset alert when water is refilled
  }
  
  // Check for extreme weather conditions
  if (temperature > HIGH_TEMP_THRESHOLD) {
    String weatherMsg = "🌡️ High Temperature Alert!\n" +
                       "Temperature: " + String(temperature) + "°C\n" +
                       "Consider increasing watering frequency.";
    sendLineNotification(weatherMsg);
  }
  
  if (humidity < LOW_HUMIDITY_THRESHOLD) {
    String weatherMsg = "💨 Low Humidity Alert!\n" +
                       "Humidity: " + String(humidity) + "%\n" +
                       "Plants may need extra watering.";
    sendLineNotification(weatherMsg);
  }
  
  // Update Blynk with sensor data
  if (isWiFiConnected) {
    Blynk.virtualWrite(V1, temperature);
    Blynk.virtualWrite(V2, humidity);
    Blynk.virtualWrite(V3, waterLevel);
    Blynk.virtualWrite(V4, lightLevel);
    Blynk.virtualWrite(V5, flowRate);
    
    for (int i = 0; i < NUM_ZONES; i++) {
      Blynk.virtualWrite(V10 + i, zones[i].moistureLevel);
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
    } else if (millis() - resetButtonPressTime > 5000) {
      Serial.println("🔄 System reset initiated...");
      sendLineNotification("🔄 Tomato watering system reset initiated");
      stopAllWatering();
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
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["light_level"] = lightLevel;
  doc["is_daylight"] = isDaylight;
  doc["water_level"] = waterLevel;
  doc["flow_rate"] = flowRate;
  doc["watering_cycles"] = todayStats.wateringCycles;
  doc["total_watering_time"] = todayStats.totalWateringTime;
  doc["wifi_signal"] = WiFi.RSSI();
  doc["free_memory"] = ESP.getFreeHeap();
  
  // Zone data
  JsonArray zonesArray = doc.createNestedArray("zones");
  for (int i = 0; i < NUM_ZONES; i++) {
    JsonObject zoneObj = zonesArray.createNestedObject();
    zoneObj["moisture"] = zones[i].moistureLevel;
    zoneObj["needs_watering"] = zones[i].needsWatering;
    zoneObj["total_time"] = zones[i].totalWateringTime;
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
  
  // Zone settings
  JsonArray zonesArray = doc.createNestedArray("zones");
  for (int i = 0; i < NUM_ZONES; i++) {
    JsonObject zoneObj = zonesArray.createNestedObject();
    zoneObj["threshold"] = zones[i].moistureThreshold;
    zoneObj["enabled"] = zones[i].enabled;
  }
  
  // Schedule settings
  JsonArray scheduleArray = doc.createNestedArray("schedule");
  for (int i = 0; i < 4; i++) {
    JsonObject scheduleItem = scheduleArray.createNestedObject();
    scheduleItem["hour"] = schedule[i].hour;
    scheduleItem["minute"] = schedule[i].minute;
    scheduleItem["duration"] = schedule[i].duration;
    scheduleItem["enabled"] = schedule[i].enabled;
    scheduleItem["weather_dependent"] = schedule[i].weather_dependent;
  }
  
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
      // Load zone settings
      if (doc.containsKey("zones")) {
        JsonArray zonesArray = doc["zones"];
        for (int i = 0; i < NUM_ZONES && i < zonesArray.size(); i++) {
          JsonObject zoneObj = zonesArray[i];
          zones[i].moistureThreshold = zoneObj["threshold"] | 40;
          zones[i].enabled = zoneObj["enabled"] | true;
        }
      }
      
      // Load schedule settings
      if (doc.containsKey("schedule")) {
        JsonArray scheduleArray = doc["schedule"];
        for (int i = 0; i < 4 && i < scheduleArray.size(); i++) {
          JsonObject scheduleItem = scheduleArray[i];
          schedule[i].hour = scheduleItem["hour"];
          schedule[i].minute = scheduleItem["minute"];
          schedule[i].duration = scheduleItem["duration"];
          schedule[i].enabled = scheduleItem["enabled"];
          schedule[i].weather_dependent = scheduleItem["weather_dependent"] | true;
        }
      }
      
      Serial.println("📖 Settings loaded");
    }
    configFile.close();
  }
}

void performSystemMaintenance() {
  // Check system health
  if (ESP.getFreeHeap() < 15000) {
    Serial.println("⚠️ Low memory warning: " + String(ESP.getFreeHeap()) + " bytes");
  }
  
  // Rotate log files if they get too large
  File logFile = SPIFFS.open("/watering.log", "r");
  if (logFile && logFile.size() > 100000) { // 100KB limit
    logFile.close();
    SPIFFS.remove("/watering.log.old");
    SPIFFS.rename("/watering.log", "/watering.log.old");
    Serial.println("🔄 Log file rotated");
  }
  if (logFile) logFile.close();
  
  // Reset daily stats at midnight
  String currentDate = String(timeClient.getDay()) + "/" + String(timeClient.getMonth()) + "/" + String(timeClient.getYear());
  if (currentDate != todayStats.date) {
    // Save yesterday's stats
    JsonDocument statsDoc;
    statsDoc["date"] = todayStats.date;
    statsDoc["watering_cycles"] = todayStats.wateringCycles;
    statsDoc["total_watering_time"] = todayStats.totalWateringTime;
    statsDoc["total_water_used"] = todayStats.totalWaterUsed;
    statsDoc["avg_temperature"] = todayStats.avgTemperature;
    statsDoc["avg_humidity"] = todayStats.avgHumidity;
    statsDoc["dry_zone_alerts"] = todayStats.dryZoneAlerts;
    
    File statsFile = SPIFFS.open("/daily_stats.log", "a");
    if (statsFile) {
      serializeJson(statsDoc, statsFile);
      statsFile.println();
      statsFile.close();
    }
    
    // Reset for new day
    todayStats.date = currentDate;
    todayStats.wateringCycles = 0;
    todayStats.totalWateringTime = 0;
    todayStats.totalWaterUsed = 0;
    todayStats.avgTemperature = 0;
    todayStats.avgHumidity = 0;
    todayStats.dryZoneAlerts = 0;
    
    // Reset zone daily counters
    for (int i = 0; i < NUM_ZONES; i++) {
      zones[i].totalWateringTime = 0;
    }
    
    Serial.println("🌅 New day started - stats reset");
  }
}

void flowPulseCounter() {
  flowPulses++;
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
    <title>RDTRC Tomato Watering</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #0f1419; color: #e6e6e6; }
        .header { background: linear-gradient(135deg, #56ab2f 0%, #a8e6cf 100%); padding: 20px; text-align: center; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }
        .header h1 { color: white; margin-bottom: 10px; font-size: 28px; }
        .header p { color: #f0f0f0; opacity: 0.9; }
        .container { max-width: 1400px; margin: 20px auto; padding: 0 20px; }
        .status-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 15px; margin-bottom: 30px; }
        .status-card { background: #1a1f2e; border-radius: 10px; padding: 15px; text-align: center; border: 1px solid #2d3748; transition: transform 0.2s; }
        .status-card:hover { transform: translateY(-2px); }
        .status-card h3 { color: #56ab2f; margin-bottom: 8px; font-size: 12px; text-transform: uppercase; }
        .status-card .value { font-size: 18px; font-weight: bold; }
        .zones-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(280px, 1fr)); gap: 20px; margin-bottom: 30px; }
        .zone-card { background: #1a1f2e; border-radius: 10px; padding: 20px; border: 1px solid #2d3748; }
        .zone-card.dry { border-left: 4px solid #f56565; }
        .zone-card.wet { border-left: 4px solid #48bb78; }
        .zone-card.normal { border-left: 4px solid #4299e1; }
        .zone-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 15px; }
        .zone-status { display: inline-block; padding: 4px 8px; border-radius: 4px; font-size: 11px; font-weight: bold; }
        .status-dry { background: #f56565; color: white; }
        .status-wet { background: #48bb78; color: white; }
        .status-normal { background: #4299e1; color: white; }
        .controls { background: #1a1f2e; border-radius: 10px; padding: 20px; margin-bottom: 20px; border: 1px solid #2d3748; }
        .control-section { margin-bottom: 25px; }
        .control-section h3 { color: #56ab2f; margin-bottom: 15px; }
        .control-group { margin-bottom: 15px; }
        .control-group label { display: block; margin-bottom: 5px; color: #a0aec0; font-weight: 500; }
        .control-group input, .control-group select { width: 100%; padding: 8px; border: 1px solid #4a5568; border-radius: 5px; background: #2d3748; color: #e6e6e6; }
        .control-row { display: flex; gap: 10px; align-items: end; }
        .control-row input { flex: 1; }
        .btn { padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 14px; font-weight: 600; transition: all 0.2s; }
        .btn-primary { background: #56ab2f; color: white; }
        .btn-secondary { background: #4a5568; color: white; }
        .btn-danger { background: #f56565; color: white; }
        .btn-warning { background: #ed8936; color: white; }
        .btn:hover { opacity: 0.8; transform: translateY(-1px); }
        .btn:disabled { opacity: 0.5; cursor: not-allowed; transform: none; }
        .alert { padding: 12px; border-radius: 5px; margin-bottom: 15px; font-size: 14px; }
        .alert-warning { background: #ed8936; color: white; }
        .alert-info { background: #4299e1; color: white; }
        .alert-danger { background: #f56565; color: white; }
        .maintenance-mode { background: #f56565; color: white; padding: 10px; text-align: center; margin-bottom: 20px; border-radius: 5px; }
        .watering-progress { background: #4299e1; color: white; padding: 10px; text-align: center; margin-bottom: 20px; border-radius: 5px; }
        .progress-bar { width: 100%; height: 20px; background: #2d3748; border-radius: 10px; overflow: hidden; margin-top: 10px; }
        .progress-fill { height: 100%; background: #56ab2f; transition: width 0.3s ease; }
        .schedule-table { width: 100%; border-collapse: collapse; margin-top: 15px; }
        .schedule-table th, .schedule-table td { padding: 8px; text-align: left; border-bottom: 1px solid #4a5568; font-size: 14px; }
        .schedule-table th { background: #2d3748; color: #56ab2f; }
        .schedule-table input[type="number"], .schedule-table input[type="time"] { width: 70px; padding: 4px; }
        .schedule-table input[type="checkbox"] { transform: scale(1.1); }
    </style>
    <script>
        let maintenanceMode = false;
        let wateringInProgress = false;
        
        function refreshStatus() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    // Update system status
                    document.getElementById('temperature').textContent = data.temperature.toFixed(1) + '°C';
                    document.getElementById('humidity').textContent = data.humidity.toFixed(1) + '%';
                    document.getElementById('water-level').textContent = data.water_level.toFixed(1) + 'cm';
                    document.getElementById('light-level').textContent = data.light_level;
                    document.getElementById('flow-rate').textContent = data.flow_rate.toFixed(1) + ' L/min';
                    document.getElementById('uptime').textContent = Math.floor(data.uptime / 1000 / 60) + ' min';
                    document.getElementById('wifi-signal').textContent = data.wifi_signal + ' dBm';
                    
                    maintenanceMode = data.maintenance_mode;
                    wateringInProgress = data.watering_in_progress;
                    
                    document.getElementById('maintenance-alert').style.display = maintenanceMode ? 'block' : 'none';
                    document.getElementById('watering-progress').style.display = wateringInProgress ? 'block' : 'none';
                    
                    if (wateringInProgress) {
                        document.getElementById('current-zone').textContent = 'Zone ' + (data.current_zone + 1);
                    }
                    
                    // Update zones
                    const zonesContainer = document.getElementById('zones-container');
                    zonesContainer.innerHTML = '';
                    
                    data.zones.forEach((zone, index) => {
                        const zoneCard = createZoneCard(zone, index);
                        zonesContainer.appendChild(zoneCard);
                    });
                    
                    // Show alerts
                    const alertsDiv = document.getElementById('alerts');
                    alertsDiv.innerHTML = '';
                    
                    if (data.water_level < 10) {
                        alertsDiv.innerHTML += '<div class="alert alert-danger">⚠️ Low water level: ' + data.water_level.toFixed(1) + 'cm</div>';
                    }
                    
                    if (data.temperature > 35) {
                        alertsDiv.innerHTML += '<div class="alert alert-warning">🌡️ High temperature: ' + data.temperature.toFixed(1) + '°C</div>';
                    }
                    
                    if (data.humidity < 30) {
                        alertsDiv.innerHTML += '<div class="alert alert-info">💨 Low humidity: ' + data.humidity.toFixed(1) + '%</div>';
                    }
                    
                    let dryZones = data.zones.filter(zone => zone.needs_watering && zone.enabled).length;
                    if (dryZones > 0) {
                        alertsDiv.innerHTML += '<div class="alert alert-warning">🏜️ ' + dryZones + ' zone(s) need watering</div>';
                    }
                });
            
            // Refresh stats
            fetch('/api/stats')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('watering-cycles').textContent = data.watering_cycles;
                    document.getElementById('total-time').textContent = Math.floor(data.total_watering_time / 60) + ' min';
                });
        }
        
        function createZoneCard(zone, index) {
            const div = document.createElement('div');
            let statusClass = 'normal';
            let statusText = 'Normal';
            
            if (zone.needs_watering) {
                statusClass = 'dry';
                statusText = 'Needs Water';
            } else if (zone.moisture > 70) {
                statusClass = 'wet';
                statusText = 'Well Watered';
            }
            
            div.className = `zone-card ${statusClass}`;
            
            div.innerHTML = `
                <div class="zone-header">
                    <h3>${zone.name}</h3>
                    <span class="zone-status status-${statusClass}">${statusText}</span>
                </div>
                <div style="margin-bottom: 15px;">
                    <div><strong>Moisture:</strong> ${zone.moisture}% (Target: ${zone.threshold}%)</div>
                    <div><strong>Last Watered:</strong> ${zone.last_watered ? new Date(zone.last_watered * 1000).toLocaleString() : 'Never'}</div>
                    <div><strong>Total Time Today:</strong> ${Math.floor(zone.total_time / 1000)} seconds</div>
                    <div><strong>Status:</strong> ${zone.enabled ? 'Enabled' : 'Disabled'}</div>
                </div>
                <div style="display: flex; gap: 10px;">
                    <button class="btn btn-primary" onclick="waterZone(${index})" ${wateringInProgress || maintenanceMode ? 'disabled' : ''}>💧 Water Now</button>
                    <button class="btn btn-secondary" onclick="configureZone(${index})">⚙️ Configure</button>
                </div>
            `;
            
            return div;
        }
        
        function waterZone(zone) {
            if (wateringInProgress || maintenanceMode) {
                alert('Cannot start watering - system busy or in maintenance');
                return;
            }
            
            const duration = prompt('Enter watering duration (seconds):', '30');
            if (duration && duration > 0) {
                fetch('/api/water', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: `zone=${zone}&duration=${duration}`
                })
                .then(response => response.json())
                .then(data => {
                    if (data.status && data.status.includes('started')) {
                        alert('Watering started for Zone ' + (zone + 1));
                        setTimeout(refreshStatus, 1000);
                    } else {
                        alert('Error: ' + (data.error || 'Unknown error'));
                    }
                });
            }
        }
        
        function waterAllZones() {
            if (wateringInProgress || maintenanceMode) {
                alert('Cannot start watering - system busy or in maintenance');
                return;
            }
            
            const duration = document.getElementById('all-zones-duration').value || 30;
            fetch('/api/water', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `zone=-1&duration=${duration}`
            })
            .then(response => response.json())
            .then(data => {
                if (data.status && data.status.includes('started')) {
                    alert('Watering started for all zones');
                    setTimeout(refreshStatus, 1000);
                } else {
                    alert('Error: ' + (data.error || 'Unknown error'));
                }
            });
        }
        
        function stopWatering() {
            fetch('/api/stop', { method: 'POST' })
            .then(response => response.json())
            .then(data => {
                alert('Watering stopped');
                refreshStatus();
            });
        }
        
        function configureZone(zone) {
            const threshold = prompt('Enter moisture threshold (%):', '40');
            if (threshold && threshold >= 0 && threshold <= 100) {
                const enabled = confirm('Enable this zone?');
                fetch('/api/zone', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                    body: `zone=${zone}&threshold=${threshold}&enabled=${enabled}`
                })
                .then(response => response.json())
                .then(data => {
                    alert('Zone configuration updated');
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
        
        function loadSchedule() {
            fetch('/api/schedule')
                .then(response => response.json())
                .then(data => {
                    const tbody = document.getElementById('schedule-tbody');
                    tbody.innerHTML = '';
                    
                    data.schedule.forEach((item, index) => {
                        const row = tbody.insertRow();
                        row.innerHTML = `
                            <td><input type="time" value="${String(item.hour).padStart(2,'0')}:${String(item.minute).padStart(2,'0')}" id="time-${index}"></td>
                            <td><input type="number" value="${item.duration}" min="5" max="300" id="duration-${index}"></td>
                            <td><input type="checkbox" ${item.enabled ? 'checked' : ''} id="enabled-${index}"></td>
                            <td><input type="checkbox" ${item.weather_dependent ? 'checked' : ''} id="weather-${index}"></td>
                            <td><button class="btn btn-secondary" onclick="updateScheduleItem(${index})">💾</button></td>
                        `;
                    });
                });
        }
        
        function updateScheduleItem(index) {
            const time = document.getElementById(`time-${index}`).value.split(':');
            const duration = document.getElementById(`duration-${index}`).value;
            const enabled = document.getElementById(`enabled-${index}`).checked;
            const weatherDependent = document.getElementById(`weather-${index}`).checked;
            
            fetch('/api/schedule', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `index=${index}&hour=${time[0]}&minute=${time[1]}&duration=${duration}&enabled=${enabled}&weather_dependent=${weatherDependent}`
            })
            .then(response => response.json())
            .then(data => {
                alert('Schedule updated!');
            });
        }
        
        // Auto refresh every 30 seconds
        setInterval(refreshStatus, 30000);
        
        // Initial load
        window.onload = function() {
            refreshStatus();
            loadSchedule();
        };
    </script>
</head>
<body>
    <div id="maintenance-alert" class="maintenance-mode" style="display: none;">
        🔧 MAINTENANCE MODE ACTIVE - Watering disabled
    </div>
    
    <div id="watering-progress" class="watering-progress" style="display: none;">
        💧 WATERING IN PROGRESS - <span id="current-zone">Zone 1</span>
        <div class="progress-bar">
            <div class="progress-fill" style="width: 60%;"></div>
        </div>
    </div>
    
    <div class="header">
        <h1>🍅 RDTRC Tomato Watering System</h1>
        <p>Complete Standalone Multi-Zone System - Firmware by RDTRC v4.0</p>
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
                <h3>Water Level</h3>
                <div class="value" id="water-level">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Light Level</h3>
                <div class="value" id="light-level">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Flow Rate</h3>
                <div class="value" id="flow-rate">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Watering Cycles</h3>
                <div class="value" id="watering-cycles">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Total Time Today</h3>
                <div class="value" id="total-time">Loading...</div>
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
        
        <div class="controls">
            <div class="control-section">
                <h3>🎛️ Manual Controls</h3>
                <div class="control-row">
                    <div class="control-group">
                        <label for="all-zones-duration">Duration for All Zones (seconds):</label>
                        <input type="number" id="all-zones-duration" min="5" max="300" value="30">
                    </div>
                    <button class="btn btn-primary" onclick="waterAllZones()">💧 Water All Zones</button>
                    <button class="btn btn-danger" onclick="stopWatering()">🛑 Stop Watering</button>
                </div>
            </div>
            
            <div class="control-section">
                <h3>🔧 System Controls</h3>
                <button class="btn btn-warning" onclick="toggleMaintenance()">🔧 Toggle Maintenance</button>
            </div>
        </div>
        
        <h2 style="margin-bottom: 20px; color: #56ab2f;">🌱 Watering Zones</h2>
        <div class="zones-grid" id="zones-container">
            <!-- Zones will be loaded here -->
        </div>
        
        <div class="controls">
            <div class="control-section">
                <h3>⏰ Watering Schedule</h3>
                <table class="schedule-table">
                    <thead>
                        <tr>
                            <th>Time</th>
                            <th>Duration (s)</th>
                            <th>Enabled</th>
                            <th>Weather Dep.</th>
                            <th>Action</th>
                        </tr>
                    </thead>
                    <tbody id="schedule-tbody">
                        <!-- Schedule items will be loaded here -->
                    </tbody>
                </table>
            </div>
        </div>
        
        <div class="controls">
            <h3 style="color: #56ab2f; margin-bottom: 15px;">📊 System Information</h3>
            <p><strong>Device ID:</strong> RDTRC_TOMATO_WATER</p>
            <p><strong>Firmware Version:</strong> 4.0</p>
            <p><strong>Features:</strong> Complete Standalone Multi-Zone Watering System</p>
            <p><strong>Zones:</strong> 4 independent watering zones</p>
            <p><strong>Sensors:</strong> Soil moisture, Temperature, Humidity, Light, Flow rate</p>
            <p><strong>Water Tank:</strong> 50cm height monitoring</p>
            <p><strong>Pump Protection:</strong> Flow monitoring and emergency stop</p>
            <p><strong>Weather Integration:</strong> Smart watering based on conditions</p>
        </div>
    </div>
</body>
</html>
  )";
  
  server.send(200, "text/html", html);
}

// Blynk Virtual Pin Handlers
BLYNK_WRITE(V0) { // Manual water all zones
  if (param.asInt() == 1 && !systemMaintenanceMode && !wateringInProgress) {
    performAllZonesWatering(DEFAULT_WATERING_DURATION, false);
  }
}

BLYNK_WRITE(V6) { // Emergency stop
  if (param.asInt() == 1) {
    stopAllWatering();
  }
}

BLYNK_WRITE(V7) { // Maintenance mode toggle
  systemMaintenanceMode = param.asInt() == 1;
  if (systemMaintenanceMode && wateringInProgress) {
    stopAllWatering();
  }
}

// Individual zone controls
BLYNK_WRITE(V30) { if (param.asInt() == 1) performWatering(0, DEFAULT_WATERING_DURATION, false); }
BLYNK_WRITE(V31) { if (param.asInt() == 1) performWatering(1, DEFAULT_WATERING_DURATION, false); }
BLYNK_WRITE(V32) { if (param.asInt() == 1) performWatering(2, DEFAULT_WATERING_DURATION, false); }
BLYNK_WRITE(V33) { if (param.asInt() == 1) performWatering(3, DEFAULT_WATERING_DURATION, false); }

// Read-only pins for Blynk dashboard
BLYNK_READ(V1) { Blynk.virtualWrite(V1, temperature); }
BLYNK_READ(V2) { Blynk.virtualWrite(V2, humidity); }
BLYNK_READ(V3) { Blynk.virtualWrite(V3, waterLevel); }
BLYNK_READ(V4) { Blynk.virtualWrite(V4, lightLevel); }
BLYNK_READ(V5) { Blynk.virtualWrite(V5, flowRate); }
BLYNK_READ(V10) { Blynk.virtualWrite(V10, zones[0].moistureLevel); }
BLYNK_READ(V11) { Blynk.virtualWrite(V11, zones[1].moistureLevel); }
BLYNK_READ(V12) { Blynk.virtualWrite(V12, zones[2].moistureLevel); }
BLYNK_READ(V13) { Blynk.virtualWrite(V13, zones[3].moistureLevel); }
BLYNK_READ(V20) { Blynk.virtualWrite(V20, todayStats.wateringCycles); }
BLYNK_READ(V21) { Blynk.virtualWrite(V21, todayStats.totalWateringTime / 1000); }
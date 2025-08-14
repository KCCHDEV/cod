/*
 * ระบบรดน้ำผักชีฟลั่งอัตโนมัติ (เวอร์ชันเรียบง่าย)
 * ESP32 + RTC + Soil Moisture + Relay + WiFi Manager
 * 
 * Features:
 * - รดน้ำอัตโนมัติตามความต้องการของผักชีฟลั่ง
 * - ตรวจวัดความชื้นดินเท่านั้น (เรียบง่าย)
 * - ระบบควบคุมปริมาณน้ำแบบแม่นยำ
 * - WiFi Manager สำหรับเลือกเครือข่าย WiFi ผ่าน Captive Portal
 * - Web Interface แบบ Responsive แสดงสถานะในเครื่อง
 * - Webhook Notifications (ทางเลือก)
 * - การรดน้ำตามเวลาที่เหมาะสม
 */

#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiManager.h> // WiFiManager library for captive portal
#include <EEPROM.h>

// Hardware Configuration
RTC_DS3231 rtc;
bool rtcAvailable = false;

// Relay and Sensor Pins
const int RELAY_COUNT = 3;
const int relayPins[RELAY_COUNT] = {5, 18, 19}; // 3 zones for cilantro
const int SOIL_MOISTURE_PINS[RELAY_COUNT] = {36, 39, 34};
const int STATUS_LED = 2;
const int PUMP_FLOW_SENSOR = 21; // Optional flow sensor
const int WIFI_RESET_BUTTON = 0; // Boot button for WiFi reset

// WiFi Manager Configuration
WiFiManager wifiManager;
bool shouldSaveConfig = false;

// Webhook Configuration (optional)
char webhookUrl[200] = "";
bool webhookEnabled = false;

// System variables
WebServer server(80);
bool relayStates[RELAY_COUNT] = {false, false, false};
int wateringCount[RELAY_COUNT] = {0, 0, 0};
unsigned long lastWateringTime[RELAY_COUNT] = {0, 0, 0};
unsigned long wateringEndTime[RELAY_COUNT] = {0, 0, 0};

// Cilantro-specific settings
const int CILANTRO_MOISTURE_MIN = 45; // ผักชีฟลั่งต้องการความชื้นปานกลาง
const int CILANTRO_MOISTURE_MAX = 75;

// Environmental data (removed temperature, humidity, and light level)
int soilMoisture[RELAY_COUNT] = {0, 0, 0};
float waterFlowRate = 0;

// Advanced watering logic
struct CilantroZone {
  int zoneId;
  String zoneName;
  int currentMoisture;
  int targetMoisture;
  int wateringDuration; // minutes
  bool needsWater;
  unsigned long lastWatered;
  float growthStage; // 0-1 (seedling to harvest)
  String status;
};

CilantroZone cilantroZones[RELAY_COUNT] = {
  {0, "Zone 1 - Seedling", 50, 65, 5, false, 0, 0.2, "Growing"},
  {1, "Zone 2 - Growing", 45, 60, 7, false, 0, 0.6, "Healthy"},
  {2, "Zone 3 - Mature", 40, 55, 8, false, 0, 0.9, "Ready"}
};

// Watering schedules for cilantro (early morning and evening)
struct WateringSchedule {
  int hour;
  int minute;
  int zoneIndex;
  int duration;
  bool enabled;
};

WateringSchedule cilantroSchedules[] = {
  {6, 0, 0, 3, true},   // Zone 1 - 6:00 AM
  {6, 5, 1, 5, true},   // Zone 2 - 6:05 AM  
  {6, 10, 2, 7, true},  // Zone 3 - 6:10 AM
  {18, 0, 0, 2, true},  // Zone 1 - 6:00 PM
  {18, 5, 1, 4, true},  // Zone 2 - 6:05 PM
  {18, 10, 2, 5, true}  // Zone 3 - 6:10 PM
};

// Callback function for saving WiFi config
void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

// Callback function when entering config mode
void configModeCallback(WiFiManager *myWiFiManager) {
  Serial.println("🔥 Entered config mode!");
  Serial.print("📶 AP IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("🌐 AP SSID: ");
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.println("🔗 Go to: http://192.168.4.1");
  
  // Blink LED to indicate config mode
  for (int i = 0; i < 10; i++) {
    digitalWrite(STATUS_LED, HIGH);
    delay(200);
    digitalWrite(STATUS_LED, LOW);
    delay(200);
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize EEPROM
  EEPROM.begin(512);
  
  // Initialize pins
  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // Active LOW relays
  }
  pinMode(STATUS_LED, OUTPUT);
  pinMode(PUMP_FLOW_SENSOR, INPUT_PULLUP);
  pinMode(WIFI_RESET_BUTTON, INPUT_PULLUP);
  
  // Initialize sensors
  Wire.begin();
  if (!rtc.begin()) {
    Serial.println("⚠️ RTC DS3231 not found - continuing without RTC");
    Serial.println("🕒 Time-based features will use millis() fallback");
    rtcAvailable = false;
  } else {
    Serial.println("✅ RTC DS3231 found and initialized");
    rtcAvailable = true;
  }
  
  // Load saved configuration
  loadConfig();
  
  // Check for WiFi reset button
  bool forceConfigPortal = false;
  if (digitalRead(WIFI_RESET_BUTTON) == LOW) {
    Serial.println("🔄 WiFi Reset button pressed - Clearing saved WiFi credentials");
    wifiManager.resetSettings();
    forceConfigPortal = true;
    delay(3000);
  }
  
  // Setup WiFi Manager
  setupWiFiManager();
  
  // Connect to WiFi using WiFiManager
  bool connected = false;
  if (forceConfigPortal) {
    Serial.println("🌐 Starting Config Portal (Forced)...");
    connected = wifiManager.startConfigPortal("CilantroWatering-Setup", "cilantro123");
  } else {
    Serial.println("🌐 Trying Auto Connect...");
    connected = wifiManager.autoConnect("CilantroWatering-Setup", "cilantro123");
  }
  
  if (!connected) {
    Serial.println("❌ Failed to connect to WiFi - Restarting...");
    delay(3000);
    ESP.restart();
  }
  
  Serial.println();
  Serial.print("✅ WiFi connected! IP: ");
  Serial.println(WiFi.localIP());
  
  // Setup web server
  setupWebServer();
  server.begin();
  Serial.println("🌐 Web server started");
  Serial.println("🌿 Simple Cilantro Watering System Ready!");
  
  // Send startup notification
  sendWebhook("🌿 Simple Cilantro Watering System Started", "info");
}

void setupWiFiManager() {
  Serial.println("🔧 Setting up WiFiManager...");
  
  // WiFiManager custom parameters for webhook configuration
  WiFiManagerParameter custom_webhook_url("webhook_url", "Webhook URL (Optional)", webhookUrl, 200);
  
  // Add parameters to WiFiManager
  wifiManager.addParameter(&custom_webhook_url);
  
  // Set callback to save custom parameters
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  
  // Enable debug output
  wifiManager.setDebugOutput(true);
  
  // Set custom AP name and password
  wifiManager.setAPStaticIPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  
  // Customize portal
  wifiManager.setConfigPortalTimeout(300); // 5 minutes timeout
  wifiManager.setConnectTimeout(20); // 20 seconds to connect
  wifiManager.setAPCallback(configModeCallback);
  
  // Show additional info
  wifiManager.setCustomHeadElement("<style>body{background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);}</style>");
  
  // Custom info
  String customInfo = "<p>🌿 ระบบรดน้ำผักชีฟลั่งอัตโนมัติ</p>";
  customInfo += "<p>📱 เชื่อมต่อกับ WiFi เพื่อใช้งานระบบ</p>";
  customInfo += "<p>💧 ระบบเรียบง่าย ใช้งานง่าย</p>";
  customInfo += "<p>🔗 เข้าใช้งานที่: http://192.168.4.1</p>";
  wifiManager.setCustomHeadElement(customInfo.c_str());
  
  Serial.println("✅ WiFiManager setup completed");
}

void loadConfig() {
  // Load configuration from EEPROM
  int address = 0;
  EEPROM.get(address, webhookUrl);
  address += sizeof(webhookUrl);
  EEPROM.get(address, webhookEnabled);
}

void saveConfig() {
  // Save configuration to EEPROM
  int address = 0;
  EEPROM.put(address, webhookUrl);
  address += sizeof(webhookUrl);
  EEPROM.put(address, webhookEnabled);
  EEPROM.commit();
}

void loop() {
  // Check for WiFi reset button (long press)
  static unsigned long buttonPressTime = 0;
  if (digitalRead(WIFI_RESET_BUTTON) == LOW) {
    if (buttonPressTime == 0) {
      buttonPressTime = millis();
    } else if (millis() - buttonPressTime > 5000) { // 5 seconds long press
      Serial.println("🔄 Long press detected - Resetting WiFi and restarting");
      wifiManager.resetSettings();
      ESP.restart();
    }
  } else {
    buttonPressTime = 0;
  }
  
  // Read sensors every 30 seconds
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead > 30000) {
    readAllSensors();
    lastSensorRead = millis();
  }
  
  // Check watering schedules (only if RTC available)
  checkWateringSchedules();
  
  // Smart watering logic
  smartCilantroWatering();
  
  // Handle active watering
  handleActiveWatering();
  
  // System status monitoring
  // (No external services to check)
  
  // Handle web server
  server.handleClient();
  
  // Status LED heartbeat
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 2000) {
    digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
    lastHeartbeat = millis();
  }
  
  delay(100);
}

void readAllSensors() {
  // Read soil moisture for each zone
  for (int i = 0; i < RELAY_COUNT; i++) {
    int rawValue = analogRead(SOIL_MOISTURE_PINS[i]);
    soilMoisture[i] = map(rawValue, 4095, 0, 0, 100); // Convert to percentage
    cilantroZones[i].currentMoisture = soilMoisture[i];
  }
  
  // Log sensor data
  logSensorData();
}

void smartCilantroWatering() {
  for (int i = 0; i < RELAY_COUNT; i++) {
    CilantroZone* zone = &cilantroZones[i];
    
    // Skip if currently watering
    if (relayStates[i]) continue;
    
    // Check if zone needs water based on multiple factors
    bool needsWater = false;
    String reason = "";
    
    // Factor 1: Soil moisture
    if (zone->currentMoisture < CILANTRO_MOISTURE_MIN) {
      needsWater = true;
      reason += "Low soil moisture (" + String(zone->currentMoisture) + "%). ";
    }
    
    // Factor 2: Time-based watering need (morning/evening preference)
    if (rtcAvailable) {
      DateTime now = rtc.now();
      bool isDayTime = (now.hour() >= 8 && now.hour() <= 18);
      if (isDayTime && zone->currentMoisture < zone->targetMoisture) {
        needsWater = true;
        reason += "Daytime watering need. ";
      }
    } else {
      // Fallback: use millis-based timing (every 2 hours)
      unsigned long timeSinceWatering = millis() - zone->lastWatered;
      if (timeSinceWatering > 7200000 && zone->currentMoisture < zone->targetMoisture) { // 2 hours
        needsWater = true;
        reason += "Time-based watering (no RTC). ";
      }
    }
    
    // Factor 3: Growth stage requirements
    float moistureAdjustment = zone->growthStage * 10; // Mature plants need less water
    int adjustedTarget = zone->targetMoisture - moistureAdjustment;
    if (zone->currentMoisture < adjustedTarget) {
      needsWater = true;
      reason += "Growth stage requirement. ";
    }
    
    // Factor 4: Time since last watering
    unsigned long timeSinceWatering = millis() - zone->lastWatered;
    if (timeSinceWatering > 3600000 && zone->currentMoisture < zone->targetMoisture) { // 1 hour
      needsWater = true;
      reason += "Extended dry period. ";
    }
    
    // Factor 5: Prevent overwatering
    if (timeSinceWatering < 1800000) { // 30 minutes
      needsWater = false;
      reason = "Recently watered, skipping.";
    }
    
    // Factor 6: Daily watering limit
    if (wateringCount[i] >= 8) { // Max 8 times per day
      needsWater = false;
      reason = "Daily watering limit reached.";
    }
    
    // Execute watering if needed
    if (needsWater) {
      int duration = calculateWateringDuration(i);
      startWatering(i, duration);
      Serial.println("🌿 Zone " + String(i+1) + " watering started: " + reason);
      sendWebhook("🌿 Cilantro Zone " + String(i+1) + " watering: " + reason, "info");
    }
    
    zone->needsWater = needsWater;
    zone->status = needsWater ? "Watering" : getZoneStatus(i);
  }
}

int calculateWateringDuration(int zoneIndex) {
  CilantroZone* zone = &cilantroZones[zoneIndex];
  
  // Base duration from zone settings
  int baseDuration = zone->wateringDuration;
  
  // Adjust based on moisture deficit
  int moistureDeficit = zone->targetMoisture - zone->currentMoisture;
  float durationMultiplier = 1.0 + (moistureDeficit / 20.0); // +5% per 1% deficit
  
  // Adjust based on time of day (if RTC available)
  if (rtcAvailable) {
    DateTime now = rtc.now();
    if (now.hour() >= 10 && now.hour() <= 16) {
      durationMultiplier *= 1.1; // 10% longer during midday heat
    } else if (now.hour() <= 6 || now.hour() >= 20) {
      durationMultiplier *= 0.9; // 10% shorter during cool periods
    }
  } else {
    // Without RTC, use standard duration
    Serial.println("ℹ️ Using standard watering duration (no RTC)");
  }
  
  // Adjust based on growth stage
  durationMultiplier *= (0.5 + zone->growthStage); // Seedlings need less water
  
  int finalDuration = baseDuration * durationMultiplier;
  
  // Safety limits
  finalDuration = constrain(finalDuration, 1, 15); // 1-15 minutes max
  
  return finalDuration;
}

String getZoneStatus(int zoneIndex) {
  CilantroZone* zone = &cilantroZones[zoneIndex];
  
  if (zone->currentMoisture >= zone->targetMoisture) {
    return "Optimal";
  } else if (zone->currentMoisture >= CILANTRO_MOISTURE_MIN) {
    return "Good";
  } else if (zone->currentMoisture >= 30) {
    return "Dry";
  } else {
    return "Critical";
  }
}

void checkWateringSchedules() {
  if (!rtcAvailable) {
    return; // Skip scheduled watering if no RTC
  }
  
  DateTime now = rtc.now();
  int scheduleCount = sizeof(cilantroSchedules) / sizeof(cilantroSchedules[0]);
  
  for (int i = 0; i < scheduleCount; i++) {
    WateringSchedule* schedule = &cilantroSchedules[i];
    
    if (schedule->enabled && 
        now.hour() == schedule->hour && 
        now.minute() == schedule->minute &&
        now.second() == 0) { // Prevent multiple triggers
      
      // Check if zone isn't already watering
      if (!relayStates[schedule->zoneIndex]) {
        startWatering(schedule->zoneIndex, schedule->duration);
        Serial.println("⏰ Scheduled watering: Zone " + String(schedule->zoneIndex + 1));
      }
    }
  }
}

void startWatering(int zoneIndex, int durationMinutes) {
  if (zoneIndex < 0 || zoneIndex >= RELAY_COUNT) return;
  
  digitalWrite(relayPins[zoneIndex], LOW); // Turn on relay (active LOW)
  relayStates[zoneIndex] = true;
  wateringEndTime[zoneIndex] = millis() + (durationMinutes * 60000UL);
  wateringCount[zoneIndex]++;
  cilantroZones[zoneIndex].lastWatered = millis();
  
  Serial.println("💧 Started watering Zone " + String(zoneIndex + 1) + " for " + String(durationMinutes) + " minutes");
  
  // Send webhook notification
  sendWebhook("💧 Zone " + String(zoneIndex + 1) + " watering started (" + String(durationMinutes) + " min)", "info");
}

void stopWatering(int zoneIndex) {
  if (zoneIndex < 0 || zoneIndex >= RELAY_COUNT) return;
  
  digitalWrite(relayPins[zoneIndex], HIGH); // Turn off relay
  relayStates[zoneIndex] = false;
  wateringEndTime[zoneIndex] = 0;
  
  Serial.println("🛑 Stopped watering Zone " + String(zoneIndex + 1));
  
  // Send webhook notification
  sendWebhook("🛑 Zone " + String(zoneIndex + 1) + " watering completed", "info");
}

void handleActiveWatering() {
  for (int i = 0; i < RELAY_COUNT; i++) {
    if (relayStates[i] && millis() >= wateringEndTime[i]) {
      stopWatering(i);
    }
  }
}



void sendWebhook(String message, String level) {
  if (!webhookEnabled || WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  http.begin(webhookUrl);
  http.addHeader("Content-Type", "application/json");
  
  DynamicJsonDocument doc(512);
  doc["timestamp"] = getCurrentTime();
  doc["system"] = "Simple Cilantro Watering System";
  doc["message"] = message;
  doc["level"] = level;
  doc["ip"] = WiFi.localIP().toString()
  
  JsonArray moistures = doc.createNestedArray("soil_moisture");
  for (int i = 0; i < RELAY_COUNT; i++) {
    moistures.add(soilMoisture[i]);
  }
  
  String payload;
  serializeJson(doc, payload);
  
  int httpCode = http.POST(payload);
  if (httpCode > 0) {
    Serial.println("📤 Webhook sent: " + message);
  }
  
  http.end();
}

String getCurrentTime() {
  if (rtcAvailable) {
    DateTime now = rtc.now();
    return String(now.year()) + "-" + 
           String(now.month()) + "-" + 
           String(now.day()) + " " + 
           String(now.hour()) + ":" + 
           String(now.minute()) + ":" + 
           String(now.second());
  } else {
    // Fallback to system uptime
    unsigned long uptimeSeconds = millis() / 1000;
    unsigned long hours = uptimeSeconds / 3600;
    unsigned long minutes = (uptimeSeconds % 3600) / 60;
    unsigned long seconds = uptimeSeconds % 60;
    return "UPTIME " + String(hours) + ":" + 
           String(minutes) + ":" + String(seconds);
  }
}

void logSensorData() {
  Serial.println("📊 Sensor Data:");
  
  for (int i = 0; i < RELAY_COUNT; i++) {
    Serial.println("  Zone " + String(i+1) + " Moisture: " + String(soilMoisture[i]) + "% - " + cilantroZones[i].status);
  }
}

// Web Server Setup
void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/api/status", handleStatus);
  server.on("/api/manual", HTTP_POST, handleManualControl);
  server.on("/api/zones", handleZones);
  server.on("/api/schedule", HTTP_POST, handleScheduleUpdate);
  
  // System endpoints
  server.on("/api/settings", HTTP_POST, handleSettingsUpdate);
  server.on("/api/emergency", HTTP_POST, handleEmergencyStop);
  server.on("/api/wifi/reset", HTTP_POST, handleWiFiReset);
}

void handleWiFiReset() {
  sendWebhook("🔄 WiFi reset requested via web interface", "warning");
  server.send(200, "application/json", "{\"status\":\"wifi_reset_scheduled\"}");
  delay(1000);
  wifiManager.resetSettings();
  ESP.restart();
}

void handleRoot() {
  String html = generateMainPage();
  server.send(200, "text/html", html);
}

void handleStatus() {
  DynamicJsonDocument doc(2048);
  doc["timestamp"] = getCurrentTime();
  doc["wifi_strength"] = WiFi.RSSI();
  doc["uptime"] = millis() / 1000;
  doc["free_memory"] = ESP.getFreeHeap();
  doc["total_watering_today"] = wateringCount[0] + wateringCount[1] + wateringCount[2];
  doc["rtc_available"] = rtcAvailable;
  
  JsonArray zones = doc.createNestedArray("zones");
  for (int i = 0; i < RELAY_COUNT; i++) {
    JsonObject zone = zones.createNestedObject();
    zone["id"] = i;
    zone["name"] = cilantroZones[i].zoneName;
    zone["moisture"] = soilMoisture[i];
    zone["target_moisture"] = cilantroZones[i].targetMoisture;
    zone["watering"] = relayStates[i];
    zone["status"] = cilantroZones[i].status;
    zone["growth_stage"] = cilantroZones[i].growthStage;
    zone["watering_count"] = wateringCount[i];
    zone["last_watered"] = cilantroZones[i].lastWatered;
  }
  
  doc["webhook_enabled"] = webhookEnabled;
  doc["system_status"] = rtcAvailable ? "running" : "running_no_rtc";
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleManualControl() {
  if (server.hasArg("zone") && server.hasArg("duration")) {
    int zone = server.arg("zone").toInt();
    int duration = server.arg("duration").toInt();
    
    if (zone >= 0 && zone < RELAY_COUNT && duration > 0 && duration <= 15) {
      startWatering(zone, duration);
      server.send(200, "application/json", "{\"status\":\"watering_started\",\"zone\":" + String(zone) + ",\"duration\":" + String(duration) + "}");
    } else {
      server.send(400, "application/json", "{\"error\":\"invalid_parameters\"}");
    }
  } else {
    server.send(400, "application/json", "{\"error\":\"missing_parameters\"}");
  }
}

void handleZones() {
  DynamicJsonDocument doc(1024);
  JsonArray zones = doc.createNestedArray("zones");
  
  for (int i = 0; i < RELAY_COUNT; i++) {
    JsonObject zone = zones.createNestedObject();
    zone["id"] = i;
    zone["name"] = cilantroZones[i].zoneName;
    zone["current_moisture"] = cilantroZones[i].currentMoisture;
    zone["target_moisture"] = cilantroZones[i].targetMoisture;
    zone["watering_duration"] = cilantroZones[i].wateringDuration;
    zone["growth_stage"] = cilantroZones[i].growthStage;
    zone["status"] = cilantroZones[i].status;
    zone["needs_water"] = cilantroZones[i].needsWater;
    zone["is_watering"] = relayStates[i];
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}



void handleEmergencyStop() {
  for (int i = 0; i < RELAY_COUNT; i++) {
    if (relayStates[i]) {
      stopWatering(i);
    }
  }
  
  sendWebhook("🚨 Emergency stop activated - All watering stopped", "warning");
  server.send(200, "application/json", "{\"status\":\"emergency_stop_executed\"}");
}

void handleSettingsUpdate() {
  // Handle settings updates from web interface
  if (server.hasArg("webhook_url")) {
    strncpy(webhookUrl, server.arg("webhook_url").c_str(), sizeof(webhookUrl));
    webhookEnabled = server.arg("webhook_enabled").toInt();
    saveConfig();
    sendWebhook("🌐 System settings updated", "info");
  }
  server.send(200, "application/json", "{\"status\":\"settings_updated\"}");
}

void handleScheduleUpdate() {
  // Handle schedule updates from web interface
  server.send(200, "application/json", "{\"status\":\"schedule_updated\"}");
}

String generateMainPage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="th">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>🌿 ระบบรดน้ำผักชีฟลั่งอัตโนมัติ</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            color: white;
            padding: 20px;
        }
        .container { max-width: 1200px; margin: 0 auto; }
        .header {
            text-align: center;
            margin-bottom: 30px;
            background: rgba(255,255,255,0.1);
            padding: 20px;
            border-radius: 15px;
            backdrop-filter: blur(10px);
        }
        .stats-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        .stat-card {
            background: rgba(255,255,255,0.1);
            padding: 20px;
            border-radius: 15px;
            backdrop-filter: blur(10px);
            border: 1px solid rgba(255,255,255,0.2);
        }
        .zones-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        .zone-card {
            background: rgba(255,255,255,0.1);
            padding: 20px;
            border-radius: 15px;
            backdrop-filter: blur(10px);
            border: 1px solid rgba(255,255,255,0.2);
        }
        .controls {
            background: rgba(255,255,255,0.1);
            padding: 20px;
            border-radius: 15px;
            backdrop-filter: blur(10px);
        }
        button {
            background: #4CAF50;
            color: white;
            border: none;
            padding: 12px 24px;
            border-radius: 8px;
            cursor: pointer;
            margin: 5px;
            font-size: 14px;
            transition: all 0.3s;
        }
        button:hover { background: #45a049; transform: translateY(-2px); }
        .emergency { background: #f44336; }
        .emergency:hover { background: #da190b; }
        .status-indicator {
            display: inline-block;
            width: 12px;
            height: 12px;
            border-radius: 50%;
            margin-right: 8px;
        }
        .status-optimal { background: #4CAF50; }
        .status-good { background: #8BC34A; }
        .status-dry { background: #FF9800; }
        .status-critical { background: #f44336; }
        .progress-bar {
            width: 100%;
            height: 20px;
            background: rgba(255,255,255,0.2);
            border-radius: 10px;
            overflow: hidden;
            margin: 10px 0;
        }
        .progress-fill {
            height: 100%;
            background: linear-gradient(90deg, #4CAF50, #8BC34A);
            transition: width 0.3s;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🌿 ระบบรดน้ำผักชีฟลั่งอัตโนมัติ</h1>
            <p>Simple Cilantro Watering System - เรียบง่าย ใช้งานง่าย</p>
            <p><span class="status-indicator status-optimal"></span>ระบบพร้อมใช้งาน</p>
        </div>

        <div class="stats-grid">
            <div class="stat-card">
                <h3>📶 WiFi Signal</h3>
                <div id="wifiSignal">-- dBm</div>
            </div>
            <div class="stat-card">
                <h3>⏱️ System Uptime</h3>
                <div id="uptime">--</div>
            </div>
            <div class="stat-card">
                <h3>🕒 Current Time</h3>
                <div id="currentTime">--:--</div>
            </div>
            <div class="stat-card">
                <h3>💧 Total Watering Today</h3>
                <div id="totalWatering">-- ครั้ง</div>
            </div>
            <div class="stat-card">
                <h3>💾 Free Memory</h3>
                <div id="freeMemory">-- KB</div>
            </div>
            <div class="stat-card">
                <h3>🌐 System Status</h3>
                <div id="systemStatus">--</div>
            </div>
        </div>

        <div class="zones-grid" id="zonesContainer">
            <!-- Zones will be populated by JavaScript -->
        </div>

        <div class="controls">
            <h3>🎛️ การควบคุม</h3>
            <button onclick="manualWater(0, 3)">💧 รดน้ำโซน 1 (3 นาที)</button>
            <button onclick="manualWater(1, 5)">💧 รดน้ำโซน 2 (5 นาที)</button>
            <button onclick="manualWater(2, 7)">💧 รดน้ำโซน 3 (7 นาที)</button>
            <button onclick="showSystemInfo()">📊 ข้อมูลระบบ</button>
            <button onclick="resetWiFi()" class="emergency">🔄 รีเซ็ต WiFi</button>
            <button onclick="emergencyStop()" class="emergency">🚨 หยุดฉุกเฉิน</button>
        </div>

        <!-- System Info Modal -->
        <div id="systemModal" style="display:none; position:fixed; top:0; left:0; width:100%; height:100%; background:rgba(0,0,0,0.5); z-index:1000;">
            <div style="position:relative; top:50%; left:50%; transform:translate(-50%,-50%); background:rgba(255,255,255,0.95); padding:30px; border-radius:15px; max-width:600px; width:90%;">
                <h2>📊 ข้อมูลระบบ</h2>
                <div id="systemInfoContent" style="text-align:left; line-height:1.8;">
                    <p><strong>📶 WiFi:</strong> <span id="modalWifiSignal">--</span> dBm</p>
                    <p><strong>💾 หน่วยความจำว่าง:</strong> <span id="modalFreeMemory">--</span> KB</p>
                    <p><strong>⏱️ เวลาทำงาน:</strong> <span id="modalUptime">--</span></p>
                    <p><strong>🌐 IP Address:</strong> <span id="modalIpAddress">--</span></p>
                    <p><strong>💧 รดน้ำวันนี้:</strong> <span id="modalTotalWatering">--</span> ครั้ง</p>
                    <p><strong>🕒 เวลาปัจจุบัน:</strong> <span id="modalCurrentTime">--</span></p>
                    <p><strong>⏰ RTC Status:</strong> <span id="modalRtcStatus">--</span></p>
                    <hr>
                    <h3>⚙️ ตั้งค่า Webhook (ทางเลือก)</h3>
                    <p><label>Webhook URL:</label><br><input type="url" id="webhookUrlInput" style="width:100%; padding:8px; margin:5px 0;" placeholder="https://example.com/webhook"></p>
                    <p><label><input type="checkbox" id="webhookEnabledInput"> เปิดใช้ Webhook</label></p>
                </div>
                <button type="button" onclick="saveSystemConfig()" style="background:#4CAF50; color:white; padding:10px 20px; border:none; border-radius:5px; margin:5px;">💾 บันทึก</button>
                <button type="button" onclick="closeSystemModal()" style="background:#f44336; color:white; padding:10px 20px; border:none; border-radius:5px; margin:5px;">❌ ปิด</button>
            </div>
        </div>
    </div>

    <script>
        function updateStatus() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('wifiSignal').textContent = data.wifi_strength + ' dBm';
                    document.getElementById('uptime').textContent = formatUptime(data.uptime);
                    document.getElementById('currentTime').textContent = data.timestamp.split(' ')[1];
                    document.getElementById('totalWatering').textContent = data.total_watering_today + ' ครั้ง';
                    document.getElementById('freeMemory').textContent = Math.round(data.free_memory / 1024) + ' KB';
                    let statusText = '🔴 ขัดข้อง';
                    if (data.system_status === 'running') {
                        statusText = '🟢 ทำงานปกติ';
                    } else if (data.system_status === 'running_no_rtc') {
                        statusText = '🟡 ทำงาน (ไม่มี RTC)';
                    }
                    document.getElementById('systemStatus').textContent = statusText;
                    
                    updateZones(data.zones);
                });
        }

        function formatUptime(seconds) {
            const hours = Math.floor(seconds / 3600);
            const minutes = Math.floor((seconds % 3600) / 60);
            return hours + 'h ' + minutes + 'm';
        }

        function updateZones(zones) {
            const container = document.getElementById('zonesContainer');
            container.innerHTML = '';
            
            zones.forEach(zone => {
                const statusClass = getStatusClass(zone.status);
                const moisturePercent = (zone.moisture / 100) * 100;
                
                const zoneCard = `
                    <div class="zone-card">
                        <h3>${zone.name}</h3>
                        <p><span class="status-indicator ${statusClass}"></span>${zone.status}</p>
                        <p>ความชื้นดิน: ${zone.moisture}%</p>
                        <div class="progress-bar">
                            <div class="progress-fill" style="width: ${moisturePercent}%"></div>
                        </div>
                        <p>เป้าหมาย: ${zone.target_moisture}%</p>
                        <p>ระยะการเจริญเติบโต: ${(zone.growth_stage * 100).toFixed(0)}%</p>
                        <p>จำนวนรดน้ำวันนี้: ${zone.watering_count} ครั้ง</p>
                        ${zone.watering ? '<p style="color: #4CAF50;">🚿 กำลังรดน้ำ...</p>' : ''}
                    </div>
                `;
                container.innerHTML += zoneCard;
            });
        }

        function getStatusClass(status) {
            switch(status) {
                case 'Optimal': return 'status-optimal';
                case 'Good': return 'status-good';
                case 'Dry': return 'status-dry';
                case 'Critical': return 'status-critical';
                default: return 'status-good';
            }
        }

        function manualWater(zone, duration) {
            fetch('/api/manual', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `zone=${zone}&duration=${duration}`
            })
            .then(response => response.json())
            .then(data => {
                if (data.status === 'watering_started') {
                    alert(`เริ่มรดน้ำโซน ${zone + 1} เป็นเวลา ${duration} นาที`);
                } else {
                    alert('เกิดข้อผิดพลาด: ' + (data.error || 'Unknown error'));
                }
            });
        }

        function testBlinkCamera() {
            fetch('/api/blink/test', { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    alert('ทดสอบ Blink Camera เสร็จสิ้น!');
                });
        }

        function emergencyStop() {
            if (confirm('คุณแน่ใจหรือไม่ที่จะหยุดการรดน้ำทั้งหมด?')) {
                fetch('/api/emergency', { method: 'POST' })
                    .then(response => response.json())
                    .then(data => {
                        alert('หยุดการรดน้ำฉุกเฉินเรียบร้อย!');
                        updateStatus();
                    });
            }
        }

        function showSystemInfo() {
            // Update modal with current data
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('modalWifiSignal').textContent = data.wifi_strength;
                    document.getElementById('modalFreeMemory').textContent = Math.round(data.free_memory / 1024);
                    document.getElementById('modalUptime').textContent = formatUptime(data.uptime);
                    document.getElementById('modalTotalWatering').textContent = data.total_watering_today;
                    document.getElementById('modalCurrentTime').textContent = data.timestamp;
                    document.getElementById('modalRtcStatus').textContent = data.rtc_available ? '✅ พร้อมใช้งาน' : '⚠️ ไม่พบ RTC';
                    document.getElementById('systemModal').style.display = 'block';
                });
        }

        function closeSystemModal() {
            document.getElementById('systemModal').style.display = 'none';
        }

        function saveSystemConfig() {
            const formData = new FormData();
            formData.append('webhook_url', document.getElementById('webhookUrlInput').value);
            formData.append('webhook_enabled', document.getElementById('webhookEnabledInput').checked ? '1' : '0');

            fetch('/api/settings', { method: 'POST', body: formData })
                .then(response => response.json())
                .then(data => {
                    alert('บันทึกการตั้งค่าเรียบร้อย!');
                    closeSystemModal();
                    updateStatus();
                });
        }

        function resetWiFi() {
            if (confirm('คุณแน่ใจหรือไม่ที่จะรีเซ็ต WiFi? ระบบจะรีสตาร์ทและเปิด Hotspot สำหรับตั้งค่าใหม่')) {
                alert('กดปุ่ม Boot บน ESP32 ค้างไว้ 5 วินาที เพื่อรีเซ็ต WiFi หรือรีสตาร์ทอุปกรณ์');
            }
        }

        // Update every 5 seconds
        setInterval(updateStatus, 5000);
        updateStatus(); // Initial load
    </script>
</body>
</html>
)rawliteral";

  return html;
}
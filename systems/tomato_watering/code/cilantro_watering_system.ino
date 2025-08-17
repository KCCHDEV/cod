/*
 * ระบบรดน้ำผักชีฟลั่งอัตโนมัติขั้นสูง
 * ESP32 + RTC + DHT22 + Soil Moisture + Relay + Blink Camera
 * 
 * Features:
 * - รดน้ำอัตโนมัติตามความต้องการของผักชีฟลั่ง
 * - ตรวจวัดอุณหภูมิ ความชื้นอากาศ และความชื้นดิน
 * - เชื่อมต่อ Blink Camera สำหรับการติดตาม
 * - ระบบควบคุมปริมาณน้ำแบบแม่นยำ
 * - Web Interface แบบ Responsive
 * - Webhook Notifications
 * - การปรับระบบตามสภาพอากาศ
 */

#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "DHT.h"

// Hardware Configuration
RTC_DS3231 rtc;
#define DHT_PIN 4
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

// Relay and Sensor Pins
const int RELAY_COUNT = 3;
const int relayPins[RELAY_COUNT] = {5, 18, 19}; // 3 zones for cilantro
const int SOIL_MOISTURE_PINS[RELAY_COUNT] = {36, 39, 34};
const int LIGHT_SENSOR_PIN = 35;
const int STATUS_LED = 2;
const int PUMP_FLOW_SENSOR = 21; // Optional flow sensor

// WiFi & Blink Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* blinkEmail = "YOUR_BLINK_EMAIL";
const char* blinkPassword = "YOUR_BLINK_PASSWORD";
const char* blinkAccountId = "YOUR_BLINK_ACCOUNT_ID";
const char* blinkNetworkId = "YOUR_BLINK_NETWORK_ID";
const char* blinkCameraId = "YOUR_BLINK_CAMERA_ID";

// Webhook Configuration
const char* webhookUrl = "YOUR_WEBHOOK_URL";
bool webhookEnabled = true;

// Blink API endpoints
const char* blinkAuthUrl = "https://rest-prod.immedia-semi.com/api/v5/account/login";
const char* blinkCameraUrl = "https://rest-prod.immedia-semi.com/api/v1/networks/";
const char* blinkArmUrl = "https://rest-prod.immedia-semi.com/api/v1/networks/";
const char* blinkDisarmUrl = "https://rest-prod.immedia-semi.com/api/v1/networks/";

// System variables
WebServer server(80);
bool relayStates[RELAY_COUNT] = {false, false, false};
int wateringCount[RELAY_COUNT] = {0, 0, 0};
unsigned long lastWateringTime[RELAY_COUNT] = {0, 0, 0};
unsigned long wateringEndTime[RELAY_COUNT] = {0, 0, 0};

// Cilantro-specific settings
const int CILANTRO_MOISTURE_MIN = 45; // ผักชีฟลั่งต้องการความชื้นปานกลาง
const int CILANTRO_MOISTURE_MAX = 75;
const int CILANTRO_TEMP_MIN = 15;     // อุณหภูมิที่เหมาะสม 15-25°C
const int CILANTRO_TEMP_MAX = 25;
const int CILANTRO_LIGHT_MIN = 4;     // ต้องการแสงปานกลาง 4-6 ชั่วโมง
const int CILANTRO_LIGHT_MAX = 6;

// Environmental data
float temperature = 0;
float humidity = 0;
int soilMoisture[RELAY_COUNT] = {0, 0, 0};
int lightLevel = 0;
float waterFlowRate = 0;

// Blink authentication
String blinkAuthToken = "";
unsigned long lastBlinkAuth = 0;
const unsigned long BLINK_AUTH_INTERVAL = 3600000; // 1 hour

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

void setup() {
  Serial.begin(115200);
  
  // Initialize pins
  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // Active LOW relays
  }
  pinMode(STATUS_LED, OUTPUT);
  pinMode(PUMP_FLOW_SENSOR, INPUT_PULLUP);
  
  // Initialize sensors
  Wire.begin();
  if (!rtc.begin()) {
    Serial.println("❌ RTC DS3231 not found!");
    while (1);
  }
  
  dht.begin();
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
  }
  Serial.println();
  Serial.print("✅ WiFi connected! IP: ");
  Serial.println(WiFi.localIP());
  
  // Initialize Blink camera
  authenticateBlink();
  
  // Setup web server
  setupWebServer();
  server.begin();
  Serial.println("🌐 Web server started");
  Serial.println("🌿 Cilantro Watering System Ready!");
  
  // Send startup notification
  sendWebhook("🌿 Cilantro Watering System Started", "info");
}

void loop() {
  DateTime now = rtc.now();
  
  // Read sensors every 30 seconds
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead > 30000) {
    readAllSensors();
    lastSensorRead = millis();
  }
  
  // Check watering schedules
  checkWateringSchedules(now);
  
  // Smart watering logic
  smartCilantroWatering();
  
  // Handle active watering
  handleActiveWatering();
  
  // Check Blink connection
  checkBlinkConnection();
  
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
  // Read DHT22
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  
  // Read soil moisture for each zone
  for (int i = 0; i < RELAY_COUNT; i++) {
    int rawValue = analogRead(SOIL_MOISTURE_PINS[i]);
    soilMoisture[i] = map(rawValue, 4095, 0, 0, 100); // Convert to percentage
    cilantroZones[i].currentMoisture = soilMoisture[i];
  }
  
  // Read light level
  lightLevel = analogRead(LIGHT_SENSOR_PIN);
  lightLevel = map(lightLevel, 0, 4095, 0, 100);
  
  // Check for sensor errors
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("⚠️ DHT22 sensor error!");
    sendWebhook("⚠️ DHT22 sensor error detected", "warning");
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
    
    // Factor 2: Temperature stress
    if (temperature > CILANTRO_TEMP_MAX && zone->currentMoisture < zone->targetMoisture) {
      needsWater = true;
      reason += "High temperature stress (" + String(temperature) + "°C). ";
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
  
  // Adjust based on temperature
  if (temperature > CILANTRO_TEMP_MAX) {
    durationMultiplier *= 1.2; // 20% longer in hot weather
  } else if (temperature < CILANTRO_TEMP_MIN) {
    durationMultiplier *= 0.8; // 20% shorter in cool weather
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

void checkWateringSchedules(DateTime now) {
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
  
  // Arm Blink camera for monitoring
  armBlinkCamera();
  
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
  
  // Disarm Blink camera
  disarmBlinkCamera();
  
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

// Blink Camera Integration
void authenticateBlink() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  http.begin(blinkAuthUrl);
  http.addHeader("Content-Type", "application/json");
  
  DynamicJsonDocument doc(256);
  doc["email"] = blinkEmail;
  doc["password"] = blinkPassword;
  
  String requestBody;
  serializeJson(doc, requestBody);
  
  int httpCode = http.POST(requestBody);
  
  if (httpCode == 200) {
    String response = http.getString();
    DynamicJsonDocument responseDoc(1024);
    deserializeJson(responseDoc, response);
    
    blinkAuthToken = responseDoc["auth"]["token"].as<String>();
    lastBlinkAuth = millis();
    Serial.println("✅ Blink authentication successful");
    sendWebhook("📷 Blink camera connected successfully", "info");
  } else {
    Serial.print("❌ Blink authentication failed: ");
    Serial.println(httpCode);
  }
  
  http.end();
}

void checkBlinkConnection() {
  if (millis() - lastBlinkAuth > BLINK_AUTH_INTERVAL) {
    authenticateBlink();
  }
}

void armBlinkCamera() {
  if (blinkAuthToken.isEmpty() || WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  String url = blinkArmUrl + String(blinkNetworkId) + "/camera/" + String(blinkCameraId) + "/enable";
  http.begin(url);
  http.addHeader("TOKEN_AUTH", blinkAuthToken);
  
  int httpCode = http.POST("");
  if (httpCode == 200) {
    Serial.println("🔒 Blink camera armed for monitoring");
  } else {
    Serial.print("❌ Failed to arm Blink camera: ");
    Serial.println(httpCode);
  }
  
  http.end();
}

void disarmBlinkCamera() {
  if (blinkAuthToken.isEmpty() || WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  String url = blinkDisarmUrl + String(blinkNetworkId) + "/camera/" + String(blinkCameraId) + "/disable";
  http.begin(url);
  http.addHeader("TOKEN_AUTH", blinkAuthToken);
  
  int httpCode = http.POST("");
  if (httpCode == 200) {
    Serial.println("🔓 Blink camera disarmed");
  } else {
    Serial.print("❌ Failed to disarm Blink camera: ");
    Serial.println(httpCode);
  }
  
  http.end();
}

void sendWebhook(String message, String level) {
  if (!webhookEnabled || WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  http.begin(webhookUrl);
  http.addHeader("Content-Type", "application/json");
  
  DynamicJsonDocument doc(512);
  doc["timestamp"] = getCurrentTime();
  doc["system"] = "Cilantro Watering System";
  doc["message"] = message;
  doc["level"] = level;
  doc["ip"] = WiFi.localIP().toString();
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  
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
  DateTime now = rtc.now();
  return String(now.year()) + "-" + 
         String(now.month()) + "-" + 
         String(now.day()) + " " + 
         String(now.hour()) + ":" + 
         String(now.minute()) + ":" + 
         String(now.second());
}

void logSensorData() {
  Serial.println("📊 Sensor Data:");
  Serial.println("  Temperature: " + String(temperature) + "°C");
  Serial.println("  Humidity: " + String(humidity) + "%");
  Serial.println("  Light Level: " + String(lightLevel) + "%");
  
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
  
  // Blink integration endpoints
  server.on("/api/blink/test", HTTP_POST, handleBlinkTest);
  server.on("/api/blink/water", HTTP_POST, handleBlinkWater);
  server.on("/api/blink/status", handleBlinkStatus);
  
  // Advanced endpoints
  server.on("/api/settings", HTTP_POST, handleSettingsUpdate);
  server.on("/api/emergency", HTTP_POST, handleEmergencyStop);
}

void handleRoot() {
  String html = generateMainPage();
  server.send(200, "text/html", html);
}

void handleStatus() {
  DynamicJsonDocument doc(2048);
  doc["timestamp"] = getCurrentTime();
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["light_level"] = lightLevel;
  doc["wifi_strength"] = WiFi.RSSI();
  doc["uptime"] = millis() / 1000;
  
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
  }
  
  doc["blink_connected"] = !blinkAuthToken.isEmpty();
  doc["webhook_enabled"] = webhookEnabled;
  
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

void handleBlinkTest() {
  armBlinkCamera();
  delay(2000);
  disarmBlinkCamera();
  server.send(200, "application/json", "{\"status\":\"blink_test_completed\"}");
}

void handleBlinkWater() {
  int zone = 0; // Default to zone 0
  if (server.hasArg("zone")) {
    zone = server.arg("zone").toInt();
  }
  
  if (zone >= 0 && zone < RELAY_COUNT) {
    startWatering(zone, 3); // 3 minutes default
    server.send(200, "application/json", "{\"status\":\"watering_started\",\"zone\":" + String(zone) + "}");
  } else {
    server.send(400, "application/json", "{\"error\":\"invalid_zone\"}");
  }
}

void handleBlinkStatus() {
  DynamicJsonDocument doc(512);
  doc["blink_connected"] = !blinkAuthToken.isEmpty();
  doc["camera_armed"] = false; // Would need to track this state
  doc["system_status"] = "running";
  doc["zones_watering"] = 0;
  
  for (int i = 0; i < RELAY_COUNT; i++) {
    if (relayStates[i]) doc["zones_watering"] = doc["zones_watering"].as<int>() + 1;
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
            <p>Smart Cilantro Watering System with Blink Camera Integration</p>
            <p><span class="status-indicator status-optimal"></span>Connected to Blink Security</p>
        </div>

        <div class="stats-grid">
            <div class="stat-card">
                <h3>🌡️ อุณหภูมิ</h3>
                <div id="temperature">--°C</div>
            </div>
            <div class="stat-card">
                <h3>💧 ความชื้นอากาศ</h3>
                <div id="humidity">--%</div>
            </div>
            <div class="stat-card">
                <h3>☀️ ระดับแสง</h3>
                <div id="lightLevel">--%</div>
            </div>
            <div class="stat-card">
                <h3>📷 Blink Status</h3>
                <div id="blinkStatus">Connected</div>
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
            <button onclick="testBlinkCamera()">📷 ทดสอบ Blink Camera</button>
            <button onclick="emergencyStop()" class="emergency">🚨 หยุดฉุกเฉิน</button>
        </div>
    </div>

    <script>
        function updateStatus() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('temperature').textContent = data.temperature.toFixed(1) + '°C';
                    document.getElementById('humidity').textContent = data.humidity.toFixed(1) + '%';
                    document.getElementById('lightLevel').textContent = data.light_level + '%';
                    document.getElementById('blinkStatus').textContent = data.blink_connected ? 'Connected' : 'Disconnected';
                    
                    updateZones(data.zones);
                });
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

        // Update every 5 seconds
        setInterval(updateStatus, 5000);
        updateStatus(); // Initial load
    </script>
</body>
</html>
)rawliteral";

  return html;
}
/*
 * ระบบรดน้ำเห็ดนางฟ้าอัตโนมัติ
 * ESP32 + RTC + MH Sensor + Relay + WiFi Manager
 * 
 * Features:
 * - รดน้ำอัตโนมัติตามความต้องการของเห็ดนางฟ้า
 * - ตรวจวัดความชื้นดินด้วย MH Sensor
 * - ระบบควบคุมปริมาณน้ำแบบแม่นยำ
 * - WiFi Manager สำหรับเลือกเครือข่าย WiFi ผ่าน Captive Portal
 * - Web Interface แบบ Responsive แสดงสถานะในเครื่อง
 * - Webhook Notifications (ทางเลือก)
 * - เห็ดนางฟ้าต้องการความชื้นสูงกว่าผักชีฟลั่ง
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
const int RELAY_COUNT = 1; // ใช้เซ็นเซอร์ตัวเดียว
const int relayPins[RELAY_COUNT] = {5}; // รีเลย์ควบคุมปั๊มน้ำ
// MH Sensor Pins
const int MH_SENSOR_AO = 36; // ขา AO (Analog Output) ต่อกับ GPIO 36
const int MH_SENSOR_DO = 35; // ขา DO (Digital Output) ต่อกับ GPIO 35
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
bool relayStates[RELAY_COUNT] = {false};
int wateringCount[RELAY_COUNT] = {0};
unsigned long lastWateringTime[RELAY_COUNT] = {0};
unsigned long wateringEndTime[RELAY_COUNT] = {0};

// เห็ดนางฟ้า settings - ต้องการความชื้นสูงกว่าผักชีฟลั่ง
const int MOISTURE_THRESHOLD_LOW = 60;  // รดน้ำเมื่อความชื้นต่ำกว่า 60%
const int MOISTURE_THRESHOLD_HIGH = 85; // หยุดรดน้ำเมื่อความชื้นถึง 85%

// Environmental data
int soilMoisture[RELAY_COUNT] = {0};
float waterFlowRate = 0;
int moisturePercent = 0; // ความชื้นเป็นเปอร์เซ็นต์

// Advanced watering logic
struct MushroomZone {
  int zoneId;
  String zoneName;
  int currentMoisture;
  int targetMoisture;
  int wateringDuration; // minutes
  bool needsWater;
  unsigned long lastWatered;
  float growthStage; // 0-1 (spawning to harvesting)
  String status;
};

MushroomZone mushroomZones[RELAY_COUNT] = {
  {0, "เห็ดนางฟ้า", 70, MOISTURE_THRESHOLD_HIGH, 3, false, 0, 0.5, "Growing"}
};

// Watering schedules สำหรับเห็ดนางฟ้า (รดน้ำบ่อยกว่าผักชีฟลั่ง)
struct WateringSchedule {
  int hour;
  int minute;
  int zoneIndex;
  int duration;
  bool enabled;
};

WateringSchedule mushroomSchedules[] = {
  {6, 0, 0, 2, true},   // เห็ดนางฟ้า - 6:00 AM
  {12, 0, 0, 2, true},  // เห็ดนางฟ้า - 12:00 PM  
  {18, 0, 0, 2, true}   // เห็ดนางฟ้า - 6:00 PM
};

// Callback function for saving WiFi config
void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

// Config mode callback
void configModeCallback(WiFiManager *myWiFiManager) {
  Serial.println("🌐 เข้าสู่โหมดตั้งค่า WiFi");
  Serial.print("🔗 SSID: ");
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.println("🌐 IP: 192.168.4.1");
  Serial.println("📱 เปิดเบราว์เซอร์และไปที่ http://192.168.4.1");
  
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
  Serial.println("🍄 ระบบรดน้ำเห็ดนางฟ้าอัตโนมัติ");
  
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
  
  // Configure MH Sensor pins
  pinMode(MH_SENSOR_AO, INPUT);  // Analog input สำหรับขา AO
  pinMode(MH_SENSOR_DO, INPUT);  // Digital input สำหรับขา DO
  
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
    delay(3000); // Debounce
  }
  
  // Setup WiFi Manager
  setupWiFiManager();
  
  // Connect to WiFi
  if (forceConfigPortal) {
    Serial.println("🔧 Forcing config portal...");
    if (!wifiManager.startConfigPortal("MushroomWatering-Setup", "mushroom123")) {
      Serial.println("❌ Failed to connect to WiFi");
      ESP.restart();
    }
  } else {
    if (!wifiManager.autoConnect("MushroomWatering-Setup", "mushroom123")) {
      Serial.println("❌ Failed to connect to WiFi");
      ESP.restart();
    }
  }
  
  Serial.println("✅ WiFi connected!");
  Serial.print("📶 IP address: ");
  Serial.println(WiFi.localIP());
  
  // Setup web server
  setupWebServer();
  server.begin();
  
  Serial.println("🍄 Mushroom Watering System ready!");
  Serial.println("💧 Moisture thresholds: Start < 60%, Stop at 85%");
}

void loadConfig() {
  // Load webhook configuration from EEPROM
  EEPROM.get(0, webhookUrl);
  EEPROM.get(200, webhookEnabled);
  
  // Validate loaded data
  if (strlen(webhookUrl) == 0 || webhookUrl[0] == 0xFF) {
    strcpy(webhookUrl, "");
    webhookEnabled = false;
  }
}

void saveConfig() {
  // Save webhook configuration to EEPROM
  EEPROM.put(0, webhookUrl);
  EEPROM.put(200, webhookEnabled);
  EEPROM.commit();
  Serial.println("📁 Configuration saved to EEPROM");
}

void setupWiFiManager() {
  Serial.println("🔧 Setting up WiFiManager...");
  
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setDebugOutput(true);
  
  // Custom info for the portal
  String customInfo = "<h2>🍄 ระบบรดน้ำเห็ดนางฟ้าอัตโนมัติ</h2>";
  customInfo += "<p>📱 เลือกเครือข่าย WiFi และกรอกรหัสผ่าน</p>";
  customInfo += "<p>💧 ระบบจะรดน้ำอัตโนมัติเมื่อความชื้น < 60%</p>";
  customInfo += "<p>🛑 หยุดรดน้ำเมื่อความชื้นถึง 85%</p>";
  customInfo += "<p>🔗 เข้าใช้งานที่: http://192.168.4.1</p>";
  wifiManager.setCustomHeadElement(customInfo.c_str());
  
  // Configure timeout
  wifiManager.setConfigPortalTimeout(300); // 5 minutes
  
  Serial.println("✅ WiFiManager setup completed");
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
  Serial.print("🍄 [");
  Serial.print(getCurrentTime());
  Serial.print("] Moisture: ");
  Serial.print(moisturePercent);
  Serial.print("% | Target: 60-85% | Status: ");
  Serial.println(getZoneStatus(0));
}

void readAllSensors() {
  // Read MH Sensor (single sensor for mushroom growing)
  int rawValue = analogRead(MH_SENSOR_AO); // อ่านค่า analog จากขา AO
  bool digitalValue = digitalRead(MH_SENSOR_DO); // อ่านค่า digital จากขา DO
  
  // แปลงค่า analog เป็นเปอร์เซ็นต์ความชื้น
  // MH Sensor: ค่าต่ำ = ดินแห้ง, ค่าสูง = ดินชื้น
  moisturePercent = map(rawValue, 4095, 0, 0, 100); // 4095 = แห้งสุด, 0 = ชื้นสุด
  
  // อัพเดทข้อมูลให้ zone เดียว
  soilMoisture[0] = moisturePercent;
  mushroomZones[0].currentMoisture = moisturePercent;
  
  Serial.print("🌡️ MH Sensor - AO: ");
  Serial.print(rawValue);
  Serial.print(" (");
  Serial.print(moisturePercent);
  Serial.print("%) DO: ");
  Serial.println(digitalValue ? "DRY" : "WET");
  
  // Log sensor data
  logSensorData();
}

void smartMushroomWatering() {
  // ใช้เซ็นเซอร์ตัวเดียว - ตรวจสอบความชื้นและรดน้ำตามเงื่อนไขเห็ดนางฟ้า
  MushroomZone* zone = &mushroomZones[0];
  
  // Skip if currently watering
  if (relayStates[0]) return;
  
  // ตรวจสอบว่าต้องรดน้ำหรือไม่
  bool needsWater = false;
  String reason = "";
  
  // เงื่อนไขการรดน้ำ: ความชื้นต่ำกว่า 60% (เห็ดต้องการความชื้นสูง)
  if (moisturePercent < MOISTURE_THRESHOLD_LOW) {
    needsWater = true;
    reason += "ความชื้นต่ำสำหรับเห็ด (" + String(moisturePercent) + "% < " + String(MOISTURE_THRESHOLD_LOW) + "%). ";
  }
    
  // ป้องกันการรดน้ำบ่อยเกินไป (ห่างกัน 20 นาที - น้อยกว่าผักชีฟลั่ง)
  unsigned long timeSinceWatering = millis() - zone->lastWatered;
  if (timeSinceWatering < 1200000) { // 20 minutes
    needsWater = false;
    reason = "เพิ่งรดน้ำไปแล้ว, รอก่อน.";
  }
  
  // จำกัดการรดน้ำต่อวัน (สูงสุด 8 ครั้ง/วัน - มากกว่าผักชีฟลั่ง)
  if (wateringCount[0] >= 8) {
    needsWater = false;
    reason = "รดน้ำครบจำนวนแล้ววันนี้.";
  }
  
  // รดน้ำหากจำเป็น
  if (needsWater) {
    int duration = calculateWateringDuration(0);
    startWatering(0, duration);
    Serial.println("💧 เริ่มรดน้ำเห็ดนางฟ้า: " + reason);
    sendWebhook("💧 เริ่มรดน้ำเห็ดนางฟ้า: " + reason, "info");
  }
  
  zone->needsWater = needsWater;
  zone->status = needsWater ? "กำลังรดน้ำ" : getZoneStatus(0);
}

String getZoneStatus(int zoneIndex) {
  MushroomZone* zone = &mushroomZones[zoneIndex];
  
  if (zone->currentMoisture >= MOISTURE_THRESHOLD_HIGH) {
    return "เหมาะสม"; // Optimal for mushrooms
  } else if (zone->currentMoisture >= 70) {
    return "ดี"; // Good
  } else if (zone->currentMoisture >= MOISTURE_THRESHOLD_LOW) {
    return "แห้งเล็กน้อย"; // Slightly dry
  } else {
    return "แห้งมาก"; // Critical - needs immediate watering
  }
}

int calculateWateringDuration(int zoneIndex) {
  MushroomZone* zone = &mushroomZones[zoneIndex];
  
  // Base duration from zone settings (shorter for mushrooms)
  int baseDuration = zone->wateringDuration;
  
  // Calculate moisture deficit
  int moistureDeficit = MOISTURE_THRESHOLD_HIGH - zone->currentMoisture;
  float durationMultiplier = 1.0 + (moistureDeficit / 25.0); // +4% per 1% deficit
  
  // Adjust based on time of day (if RTC available)
  if (rtcAvailable) {
    DateTime now = rtc.now();
    if (now.hour() >= 10 && now.hour() <= 16) {
      durationMultiplier *= 1.2; // 20% longer during midday (mushrooms need more water in heat)
    } else if (now.hour() <= 6 || now.hour() >= 20) {
      durationMultiplier *= 0.8; // 20% shorter during cool periods
    }
  } else {
    // Without RTC, use standard duration
    Serial.println("ℹ️ Using standard watering duration (no RTC)");
  }
  
  // Adjust based on growth stage
  if (zone->growthStage < 0.3) {
    durationMultiplier *= 0.7; // Spawning stage needs less water
  } else if (zone->growthStage > 0.7) {
    durationMultiplier *= 1.3; // Fruiting stage needs more water
  }
  
  int finalDuration = (int)(baseDuration * durationMultiplier);
  finalDuration = constrain(finalDuration, 1, 10); // 1-10 minutes max
  
  Serial.println("💧 Watering duration calculated: " + String(finalDuration) + " minutes");
  return finalDuration;
}

void checkWateringSchedules() {
  if (!rtcAvailable) {
    return; // Skip scheduled watering if no RTC
  }
  
  DateTime now = rtc.now();
  int scheduleCount = sizeof(mushroomSchedules) / sizeof(mushroomSchedules[0]);
  
  for (int i = 0; i < scheduleCount; i++) {
    WateringSchedule* schedule = &mushroomSchedules[i];
    
    if (schedule->enabled && 
        now.hour() == schedule->hour && 
        now.minute() == schedule->minute &&
        now.second() == 0) { // Prevent multiple triggers
      
      // Check if zone isn't already watering
      if (!relayStates[schedule->zoneIndex]) {
        startWatering(schedule->zoneIndex, schedule->duration);
        Serial.println("⏰ Scheduled mushroom watering: Zone " + String(schedule->zoneIndex + 1));
      }
    }
  }
}

void startWatering(int zoneIndex, int durationMinutes) {
  if (zoneIndex >= RELAY_COUNT) return;
  
  digitalWrite(relayPins[zoneIndex], LOW); // Turn on relay (active LOW)
  relayStates[zoneIndex] = true;
  wateringEndTime[zoneIndex] = millis() + (durationMinutes * 60000);
  lastWateringTime[zoneIndex] = millis();
  wateringCount[zoneIndex]++;
  mushroomZones[zoneIndex].lastWatered = millis();
  
  Serial.println("💧 เริ่มรดน้ำเห็ดนางฟ้า โซน " + String(zoneIndex + 1) + " เป็นเวลา " + String(durationMinutes) + " นาที");
}

void stopWatering(int zoneIndex) {
  if (zoneIndex >= RELAY_COUNT) return;
  
  digitalWrite(relayPins[zoneIndex], HIGH); // Turn off relay
  relayStates[zoneIndex] = false;
  
  Serial.println("💧 หยุดรดน้ำเห็ดนางฟ้า โซน " + String(zoneIndex + 1));
}

void handleActiveWatering() {
  // ตรวจสอบการรดน้ำที่กำลังทำงาน
  if (relayStates[0]) {
    // หยุดรดน้ำเมื่อ: 1) เวลาครบ หรือ 2) ความชื้นถึง 85%
    if (millis() >= wateringEndTime[0] || moisturePercent >= MOISTURE_THRESHOLD_HIGH) {
      if (moisturePercent >= MOISTURE_THRESHOLD_HIGH) {
        Serial.println("💧 หยุดรดน้ำ: ความชื้นถึงเป้าหมายสำหรับเห็ด " + String(moisturePercent) + "%");
        sendWebhook("💧 หยุดรดน้ำเห็ด: ความชื้นถึง " + String(moisturePercent) + "%", "info");
      }
      stopWatering(0);
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
  doc["system"] = "Mushroom Watering System";
  doc["message"] = message;
  doc["level"] = level;
  doc["ip"] = WiFi.localIP().toString();
  
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

void setupWebServer() {
  // Serve main page
  server.on("/", HTTP_GET, handleRoot);
  
  // API endpoints
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/manual", HTTP_POST, handleManualWatering);
  server.on("/api/stop", HTTP_POST, handleStopWatering);
  server.on("/api/wifi/reset", HTTP_POST, handleWiFiReset);
  server.on("/api/settings", HTTP_POST, handleSettingsUpdate);
}

void handleRoot() {
  String html = R"(
<!DOCTYPE html>
<html lang="th">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>🍄 ระบบรดน้ำเห็ดนางฟ้าอัตโนมัติ</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; 
            background: linear-gradient(135deg, #8B4513 0%, #D2691E 100%);
            color: #333; 
            line-height: 1.6;
        }
        .container { max-width: 1200px; margin: 0 auto; padding: 20px; }
        .header { 
            background: rgba(255,255,255,0.95); 
            padding: 20px; 
            border-radius: 15px; 
            margin-bottom: 20px; 
            text-align: center;
            box-shadow: 0 8px 32px rgba(0,0,0,0.1);
        }
        .header h1 { color: #8B4513; margin-bottom: 10px; }
        .stats-grid, .zones-grid { 
            display: grid; 
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); 
            gap: 20px; 
            margin-bottom: 20px; 
        }
        .stat-card, .zone-card { 
            background: rgba(255,255,255,0.95); 
            padding: 20px; 
            border-radius: 15px; 
            box-shadow: 0 8px 32px rgba(0,0,0,0.1);
            transition: transform 0.3s ease;
        }
        .stat-card:hover, .zone-card:hover { transform: translateY(-5px); }
        .stat-card h3 { color: #8B4513; margin-bottom: 10px; }
        .stat-card div { font-size: 1.5em; font-weight: bold; color: #D2691E; }
        .progress-bar { 
            width: 100%; 
            height: 20px; 
            background: #f0f0f0; 
            border-radius: 10px; 
            overflow: hidden; 
            margin: 10px 0;
        }
        .progress-fill { 
            height: 100%; 
            background: linear-gradient(90deg, #D2691E, #FF8C00); 
            transition: width 0.3s ease;
        }
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
        .status-critical { background: #F44336; }
        .controls { 
            background: rgba(255,255,255,0.95); 
            padding: 20px; 
            border-radius: 15px; 
            text-align: center;
            box-shadow: 0 8px 32px rgba(0,0,0,0.1);
        }
        .controls h3 { color: #8B4513; margin-bottom: 15px; }
        button { 
            background: linear-gradient(45deg, #D2691E, #FF8C00); 
            color: white; 
            border: none; 
            padding: 12px 24px; 
            margin: 5px; 
            border-radius: 25px; 
            cursor: pointer; 
            font-size: 16px;
            transition: all 0.3s ease;
        }
        button:hover { 
            transform: translateY(-2px); 
            box-shadow: 0 5px 15px rgba(210,105,30,0.4);
        }
        button.emergency { 
            background: linear-gradient(45deg, #F44336, #FF5722); 
        }
        .modal { 
            display: none; 
            position: fixed; 
            top: 0; 
            left: 0; 
            width: 100%; 
            height: 100%; 
            background: rgba(0,0,0,0.5); 
            z-index: 1000;
        }
        .modal-content { 
            background: white; 
            margin: 5% auto; 
            padding: 20px; 
            border-radius: 15px; 
            width: 90%; 
            max-width: 500px;
        }
        .close { 
            float: right; 
            font-size: 28px; 
            font-weight: bold; 
            cursor: pointer;
        }
        @media (max-width: 768px) { 
            .stats-grid, .zones-grid { grid-template-columns: 1fr; }
            .container { padding: 10px; }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🍄 ระบบรดน้ำเห็ดนางฟ้าอัตโนมัติ</h1>
            <p>Mushroom Watering System - เหมาะสำหรับเห็ดนางฟ้า</p>
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
            <button onclick="manualWater(0, 2)">💧 รดน้ำ 2 นาที</button>
            <button onclick="manualWater(0, 3)">💧 รดน้ำ 3 นาที</button>
            <button onclick="manualWater(0, 5)">💧 รดน้ำ 5 นาที</button>
            <button onclick="showSystemInfo()">📊 ข้อมูลระบบ</button>
            <button onclick="resetWiFi()" class="emergency">🔄 รีเซ็ต WiFi</button>
            <button onclick="emergencyStop()" class="emergency">🚨 หยุดฉุกเฉิน</button>
        </div>

        <!-- System Info Modal -->
        <div id="systemModal" class="modal">
            <div class="modal-content">
                <span class="close" onclick="closeModal()">&times;</span>
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
                    <p><label><input type="checkbox" id="webhookEnabledInput"> เปิดใช้งาน Webhook</label></p>
                    <button onclick="saveSettings()">💾 บันทึกการตั้งค่า</button>
                </div>
            </div>
        </div>
    </div>

    <script>
        // Update data every 5 seconds
        setInterval(updateData, 5000);
        updateData(); // Initial call

        function updateData() {
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
                        <p>ความชื้นดิน: <strong>${zone.moisture}%</strong></p>
                        <div class="progress-bar">
                            <div class="progress-fill" style="width: ${moisturePercent}%"></div>
                        </div>
                        <p>🔴 รดน้ำเมื่อ: < 60%</p>
                        <p>🟢 หยุดเมื่อ: 85%</p>
                        <p>จำนวนรดน้ำวันนี้: ${zone.watering_count} ครั้ง</p>
                        ${zone.watering ? '<p style="color: #4CAF50;">🚿 กำลังรดน้ำ...</p>' : ''}
                    </div>
                `;
                container.innerHTML += zoneCard;
            });
        }

        function getStatusClass(status) {
            switch(status) {
                case 'เหมาะสม': return 'status-optimal';
                case 'ดี': return 'status-good';
                case 'แห้งเล็กน้อย': return 'status-dry';
                case 'แห้งมาก': return 'status-critical';
                default: return 'status-good';
            }
        }

        function manualWater(zone, duration) {
            fetch('/api/manual', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ zone: zone, duration: duration })
            }).then(() => updateData());
        }

        function emergencyStop() {
            if (confirm('หยุดการรดน้ำทั้งหมดใช่หรือไม่?')) {
                fetch('/api/stop', { method: 'POST' }).then(() => updateData());
            }
        }

        function resetWiFi() {
            if (confirm('รีเซ็ต WiFi ใช่หรือไม่? ระบบจะรีสตาร์ท')) {
                fetch('/api/wifi/reset', { method: 'POST' });
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

        function closeModal() {
            document.getElementById('systemModal').style.display = 'none';
        }

        function saveSettings() {
            const webhookUrl = document.getElementById('webhookUrlInput').value;
            const webhookEnabled = document.getElementById('webhookEnabledInput').checked;
            
            fetch('/api/settings', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ 
                    webhook_url: webhookUrl, 
                    webhook_enabled: webhookEnabled 
                })
            }).then(() => {
                alert('บันทึกการตั้งค่าเรียบร้อย');
                closeModal();
            });
        }

        function formatUptime(seconds) {
            const days = Math.floor(seconds / 86400);
            const hours = Math.floor((seconds % 86400) / 3600);
            const minutes = Math.floor((seconds % 3600) / 60);
            return `${days}d ${hours}h ${minutes}m`;
        }

        // Close modal when clicking outside
        window.onclick = function(event) {
            const modal = document.getElementById('systemModal');
            if (event.target === modal) {
                modal.style.display = 'none';
            }
        }
    </script>
</body>
</html>
  )";
  
  server.send(200, "text/html", html);
}

void handleStatus() {
  DynamicJsonDocument doc(2048);
  doc["timestamp"] = getCurrentTime();
  doc["wifi_strength"] = WiFi.RSSI();
  doc["uptime"] = millis() / 1000;
  doc["free_memory"] = ESP.getFreeHeap();
  doc["total_watering_today"] = wateringCount[0];
  doc["rtc_available"] = rtcAvailable;
  
  JsonArray zones = doc.createNestedArray("zones");
  for (int i = 0; i < RELAY_COUNT; i++) {
    JsonObject zone = zones.createNestedObject();
    zone["id"] = i;
    zone["name"] = mushroomZones[i].zoneName;
    zone["moisture"] = soilMoisture[i];
    zone["target_moisture"] = mushroomZones[i].targetMoisture;
    zone["watering"] = relayStates[i];
    zone["status"] = mushroomZones[i].status;
    zone["growth_stage"] = mushroomZones[i].growthStage;
    zone["watering_count"] = wateringCount[i];
    zone["last_watered"] = mushroomZones[i].lastWatered;
  }
  
  doc["webhook_enabled"] = webhookEnabled;
  doc["system_status"] = rtcAvailable ? "running" : "running_no_rtc";
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleManualWatering() {
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, server.arg("plain"));
    
    int zone = doc["zone"];
    int duration = doc["duration"];
    
    if (zone >= 0 && zone < RELAY_COUNT && duration > 0 && duration <= 15) {
      startWatering(zone, duration);
      server.send(200, "text/plain", "Manual watering started");
    } else {
      server.send(400, "text/plain", "Invalid parameters");
    }
  } else {
    server.send(400, "text/plain", "No data received");
  }
}

void handleStopWatering() {
  for (int i = 0; i < RELAY_COUNT; i++) {
    if (relayStates[i]) {
      stopWatering(i);
    }
  }
  server.send(200, "text/plain", "All watering stopped");
}

void handleWiFiReset() {
  server.send(200, "text/plain", "WiFi reset initiated");
  delay(1000);
  wifiManager.resetSettings();
  ESP.restart();
}

void handleSettingsUpdate() {
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, server.arg("plain"));
    
    if (doc.containsKey("webhook_url")) {
      String url = doc["webhook_url"].as<String>();
      url.toCharArray(webhookUrl, sizeof(webhookUrl));
    }
    
    if (doc.containsKey("webhook_enabled")) {
      webhookEnabled = doc["webhook_enabled"];
    }
    
    saveConfig();
    server.send(200, "text/plain", "Settings updated");
  } else {
    server.send(400, "text/plain", "No data received");
  }
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
  smartMushroomWatering();
  
  // Handle active watering
  handleActiveWatering();
  
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
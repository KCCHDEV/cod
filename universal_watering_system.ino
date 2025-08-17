/*
 * ระบบรดน้ำอัตโนมัติแบบ Universal Template
 * ESP32 + WiFi Manager + Blink + Web Interface + LCD Auto-detect
 * 
 * Features:
 * - รองรับทุกชนิดพืช (ปรับค่าได้)
 * - WiFi Manager สำหรับตั้งค่า WiFi
 * - Blink Camera Integration สำหรับแสดงสถานะ
 * - Web Interface แบบ Responsive
 * - LCD 16x2 Auto Detection
 * - MH Sensor สำหรับวัดความชื้น
 * - ระบบแจ้งเตือน Webhook
 * - RTC สำหรับเวลาที่แม่นยำ
 * 
 * ใช้ได้กับ: ผักชีฟลั่ง, เห็ดนางฟ้า, ผักสลัด, มะเขือเทศ, พริก, ฯลฯ
 */

#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiManager.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h> // LCD library

// ======================== Plant Configuration ========================
// ตั้งค่าพืชที่ต้องการ (เปลี่ยนตรงนี้เท่านั้น)
#define PLANT_TYPE "CILANTRO"  // Options: CILANTRO, MUSHROOM, LETTUCE, TOMATO, PEPPER
#define SYSTEM_NAME "ผักชีฟลั่ง"
#define SYSTEM_EMOJI "🌿"

// Plant-specific settings (will be set based on PLANT_TYPE)
struct PlantConfig {
  int moistureThresholdLow;   // รดน้ำเมื่อต่ำกว่า
  int moistureThresholdHigh;  // หยุดรดน้ำเมื่อถึง
  int wateringInterval;       // ระยะห่างการรดน้ำ (นาที)
  int maxWateringPerDay;      // จำนวนครั้งสูงสุดต่อวัน
  int baseDuration;           // เวลารดน้ำพื้นฐาน (นาที)
  int scheduleCount;          // จำนวนตารางรดน้ำต่อวัน
  String statusLevels[4];     // ชื่อสถานะ 4 ระดับ
  String wifiSSID;           // ชื่อ WiFi Hotspot
  String wifiPassword;       // รหัส WiFi Hotspot
} plantConfig;

// ======================== Hardware Configuration ========================
RTC_DS3231 rtc;
bool rtcAvailable = false;

// LCD Configuration (Auto-detect)
LiquidCrystal_I2C lcd(0x27, 16, 2); // Default I2C address
bool lcdAvailable = false;
unsigned long lastLCDUpdate = 0;

// Pins
const int RELAY_COUNT = 1;
const int relayPins[RELAY_COUNT] = {5};
const int MH_SENSOR_AO = 36;
const int MH_SENSOR_DO = 35;
const int STATUS_LED = 2;
const int PUMP_FLOW_SENSOR = 21;
const int WIFI_RESET_BUTTON = 0;

// WiFi Manager
WiFiManager wifiManager;
bool shouldSaveConfig = false;

// Configuration Storage
char webhookUrl[200] = "";
bool webhookEnabled = false;
char blinkEmail[100] = "";
char blinkPassword[100] = "";
char blinkAccountId[50] = "";
char blinkNetworkId[50] = "";
char blinkCameraId[50] = "";
String blinkAuthToken = "";
bool blinkConnected = false;

// System Variables
WebServer server(80);
bool relayStates[RELAY_COUNT] = {false};
int wateringCount[RELAY_COUNT] = {0};
unsigned long lastWateringTime[RELAY_COUNT] = {0};
unsigned long wateringEndTime[RELAY_COUNT] = {0};

// Sensor Data
int soilMoisture[RELAY_COUNT] = {0};
int moisturePercent = 0;
float temperature = 0;
float humidity = 0;

// Zone Structure
struct WateringZone {
  int zoneId;
  String zoneName;
  int currentMoisture;
  int targetMoisture;
  int wateringDuration;
  bool needsWater;
  unsigned long lastWatered;
  float growthStage;
  String status;
};

WateringZone zones[RELAY_COUNT];

// Watering Schedule
struct WateringSchedule {
  int hour;
  int minute;
  int zoneIndex;
  int duration;
  bool enabled;
};

WateringSchedule schedules[6]; // Max 6 schedules per day

// ======================== Plant Configuration Setup ========================
void setupPlantConfig() {
  // Default values
  plantConfig.moistureThresholdLow = 30;
  plantConfig.moistureThresholdHigh = 70;
  plantConfig.wateringInterval = 30;
  plantConfig.maxWateringPerDay = 6;
  plantConfig.baseDuration = 3;
  plantConfig.scheduleCount = 2;
  
  // Plant-specific configurations
  if (PLANT_TYPE == "CILANTRO") {
    plantConfig.moistureThresholdLow = 30;
    plantConfig.moistureThresholdHigh = 70;
    plantConfig.wateringInterval = 30;
    plantConfig.maxWateringPerDay = 6;
    plantConfig.baseDuration = 3;
    plantConfig.scheduleCount = 2;
    plantConfig.statusLevels[0] = "เหมาะสม";
    plantConfig.statusLevels[1] = "ดี";
    plantConfig.statusLevels[2] = "แห้งเล็กน้อย";
    plantConfig.statusLevels[3] = "แห้งมาก";
    plantConfig.wifiSSID = "CilantroWatering-Setup";
    plantConfig.wifiPassword = "cilantro123";
    
    // Cilantro schedules: 6:00 AM, 6:00 PM
    schedules[0] = {6, 0, 0, 3, true};
    schedules[1] = {18, 0, 0, 3, true};
    
  } else if (PLANT_TYPE == "MUSHROOM") {
    plantConfig.moistureThresholdLow = 60;
    plantConfig.moistureThresholdHigh = 85;
    plantConfig.wateringInterval = 20;
    plantConfig.maxWateringPerDay = 8;
    plantConfig.baseDuration = 2;
    plantConfig.scheduleCount = 3;
    plantConfig.statusLevels[0] = "เหมาะสำหรับเห็ด";
    plantConfig.statusLevels[1] = "ดี";
    plantConfig.statusLevels[2] = "แห้งเล็กน้อย";
    plantConfig.statusLevels[3] = "แห้งมาก";
    plantConfig.wifiSSID = "MushroomWatering-Setup";
    plantConfig.wifiPassword = "mushroom123";
    
    // Mushroom schedules: 6:00 AM, 12:00 PM, 6:00 PM
    schedules[0] = {6, 0, 0, 2, true};
    schedules[1] = {12, 0, 0, 2, true};
    schedules[2] = {18, 0, 0, 2, true};
    
  } else if (PLANT_TYPE == "LETTUCE") {
    plantConfig.moistureThresholdLow = 40;
    plantConfig.moistureThresholdHigh = 75;
    plantConfig.wateringInterval = 25;
    plantConfig.maxWateringPerDay = 5;
    plantConfig.baseDuration = 4;
    plantConfig.scheduleCount = 2;
    plantConfig.statusLevels[0] = "เหมาะสำหรับสลัด";
    plantConfig.statusLevels[1] = "ดี";
    plantConfig.statusLevels[2] = "แห้งเล็กน้อย";
    plantConfig.statusLevels[3] = "แห้งมาก";
    plantConfig.wifiSSID = "LettuceWatering-Setup";
    plantConfig.wifiPassword = "lettuce123";
    
    // Lettuce schedules: 7:00 AM, 5:00 PM
    schedules[0] = {7, 0, 0, 4, true};
    schedules[1] = {17, 0, 0, 4, true};
    
  } else if (PLANT_TYPE == "TOMATO") {
    plantConfig.moistureThresholdLow = 35;
    plantConfig.moistureThresholdHigh = 65;
    plantConfig.wateringInterval = 40;
    plantConfig.maxWateringPerDay = 4;
    plantConfig.baseDuration = 5;
    plantConfig.scheduleCount = 2;
    plantConfig.statusLevels[0] = "เหมาะสำหรับมะเขือเทศ";
    plantConfig.statusLevels[1] = "ดี";
    plantConfig.statusLevels[2] = "แห้งเล็กน้อย";
    plantConfig.statusLevels[3] = "แห้งมาก";
    plantConfig.wifiSSID = "TomatoWatering-Setup";
    plantConfig.wifiPassword = "tomato123";
    
    // Tomato schedules: 6:30 AM, 6:30 PM
    schedules[0] = {6, 30, 0, 5, true};
    schedules[1] = {18, 30, 0, 5, true};
    
  } else if (PLANT_TYPE == "PEPPER") {
    plantConfig.moistureThresholdLow = 25;
    plantConfig.moistureThresholdHigh = 60;
    plantConfig.wateringInterval = 45;
    plantConfig.maxWateringPerDay = 4;
    plantConfig.baseDuration = 4;
    plantConfig.scheduleCount = 2;
    plantConfig.statusLevels[0] = "เหมาะสำหรับพริก";
    plantConfig.statusLevels[1] = "ดี";
    plantConfig.statusLevels[2] = "แห้งเล็กน้อย";
    plantConfig.statusLevels[3] = "แห้งมาก";
    plantConfig.wifiSSID = "PepperWatering-Setup";
    plantConfig.wifiPassword = "pepper123";
    
    // Pepper schedules: 6:00 AM, 7:00 PM
    schedules[0] = {6, 0, 0, 4, true};
    schedules[1] = {19, 0, 0, 4, true};
  }
  
  // Initialize zones
  zones[0] = {0, SYSTEM_NAME, 50, plantConfig.moistureThresholdHigh, plantConfig.baseDuration, false, 0, 0.5, "กำลังเริ่มต้น"};
}

// ======================== LCD Functions ========================
void initLCD() {
  Serial.println("🖥️ Checking for LCD 16x2...");
  
  // Try different I2C addresses
  byte addresses[] = {0x27, 0x3F, 0x20, 0x38};
  
  for (int i = 0; i < 4; i++) {
    Wire.beginTransmission(addresses[i]);
    if (Wire.endTransmission() == 0) {
      Serial.print("📱 LCD found at address: 0x");
      Serial.println(addresses[i], HEX);
      
      lcd = LiquidCrystal_I2C(addresses[i], 16, 2);
      lcd.init();
      lcd.backlight();
      lcd.clear();
      
      // Test display
      lcd.setCursor(0, 0);
      lcd.print(SYSTEM_EMOJI + " " + SYSTEM_NAME);
      lcd.setCursor(0, 1);
      lcd.print("System Starting..");
      
      lcdAvailable = true;
      Serial.println("✅ LCD initialized successfully");
      return;
    }
  }
  
  Serial.println("⚠️ LCD not found - continuing without LCD");
  lcdAvailable = false;
}

void updateLCD() {
  if (!lcdAvailable || millis() - lastLCDUpdate < 2000) return;
  
  lcd.clear();
  
  // Line 1: Plant name + moisture
  lcd.setCursor(0, 0);
  lcd.print(SYSTEM_EMOJI + " " + String(moisturePercent) + "%");
  
  // Show WiFi status
  if (WiFi.status() == WL_CONNECTED) {
    lcd.print(" WiFi:OK");
  } else {
    lcd.print(" WiFi:--");
  }
  
  // Line 2: Status + time
  lcd.setCursor(0, 1);
  String status = getZoneStatus(0);
  if (status.length() > 8) status = status.substring(0, 8);
  lcd.print(status);
  
  // Show current time or uptime
  if (rtcAvailable) {
    DateTime now = rtc.now();
    lcd.print(" ");
    if (now.hour() < 10) lcd.print("0");
    lcd.print(now.hour());
    lcd.print(":");
    if (now.minute() < 10) lcd.print("0");
    lcd.print(now.minute());
  } else {
    unsigned long uptime = millis() / 1000 / 60; // minutes
    lcd.print(" " + String(uptime) + "m");
  }
  
  lastLCDUpdate = millis();
}

// ======================== Blink Integration ========================
void setupBlinkIntegration() {
  // Setup Blink parameters for WiFiManager
  WiFiManagerParameter custom_blink_email("blink_email", "Blink Email", blinkEmail, 100);
  WiFiManagerParameter custom_blink_password("blink_password", "Blink Password", blinkPassword, 100, "type=\"password\"");
  WiFiManagerParameter custom_blink_account("blink_account", "Blink Account ID", blinkAccountId, 50);
  WiFiManagerParameter custom_blink_network("blink_network", "Blink Network ID", blinkNetworkId, 50);
  WiFiManagerParameter custom_blink_camera("blink_camera", "Blink Camera ID", blinkCameraId, 50);
  
  wifiManager.addParameter(&custom_blink_email);
  wifiManager.addParameter(&custom_blink_password);
  wifiManager.addParameter(&custom_blink_account);
  wifiManager.addParameter(&custom_blink_network);
  wifiManager.addParameter(&custom_blink_camera);
}

bool authenticateBlink() {
  if (strlen(blinkEmail) == 0 || strlen(blinkPassword) == 0) {
    Serial.println("⚠️ Blink credentials not configured");
    return false;
  }
  
  HTTPClient http;
  http.begin("https://rest-prod.immedia-semi.com/api/v5/account/login");
  http.addHeader("Content-Type", "application/json");
  
  DynamicJsonDocument doc(512);
  doc["email"] = blinkEmail;
  doc["password"] = blinkPassword;
  
  String payload;
  serializeJson(doc, payload);
  
  int httpCode = http.POST(payload);
  
  if (httpCode == 200) {
    String response = http.getString();
    DynamicJsonDocument responseDoc(2048);
    deserializeJson(responseDoc, response);
    
    blinkAuthToken = responseDoc["auth"]["token"].as<String>();
    strcpy(blinkAccountId, responseDoc["account"]["account_id"].as<String>().c_str());
    
    Serial.println("✅ Blink authentication successful");
    blinkConnected = true;
    
    if (lcdAvailable) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Blink: Connected");
      delay(2000);
    }
    
    http.end();
    return true;
  } else {
    Serial.println("❌ Blink authentication failed: " + String(httpCode));
    blinkConnected = false;
    http.end();
    return false;
  }
}

void updateBlinkStatus(String status, String moisture) {
  if (!blinkConnected || blinkAuthToken.length() == 0) return;
  
  HTTPClient http;
  String url = "https://rest-prod.immedia-semi.com/api/v1/accounts/" + String(blinkAccountId) + "/networks/" + String(blinkNetworkId) + "/cameras/" + String(blinkCameraId) + "/thumbnail";
  
  http.begin(url);
  http.addHeader("TOKEN_AUTH", blinkAuthToken);
  http.addHeader("Content-Type", "application/json");
  
  DynamicJsonDocument doc(512);
  doc["status"] = SYSTEM_EMOJI + " " + SYSTEM_NAME + ": " + status;
  doc["moisture"] = moisture + "%";
  doc["timestamp"] = getCurrentTime();
  doc["system"] = "Universal Watering System";
  
  String payload;
  serializeJson(doc, payload);
  
  int httpCode = http.POST(payload);
  if (httpCode > 0) {
    Serial.println("📤 Blink status updated: " + status);
  } else {
    Serial.println("❌ Blink update failed: " + String(httpCode));
  }
  
  http.end();
}

// ======================== Core Functions ========================
void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void configModeCallback(WiFiManager *myWiFiManager) {
  Serial.println("🌐 เข้าสู่โหมดตั้งค่า WiFi");
  Serial.print("🔗 SSID: ");
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.println("🌐 IP: 192.168.4.1");
  Serial.println("📱 เปิดเบราว์เซอร์และไปที่ http://192.168.4.1");
  
  if (lcdAvailable) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Setup Mode");
    lcd.setCursor(0, 1);
    lcd.print("192.168.4.1");
  }
  
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
  Serial.println(SYSTEM_EMOJI + " ระบบรดน้ำ" + SYSTEM_NAME + "อัตโนมัติ");
  
  // Setup plant configuration
  setupPlantConfig();
  
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
  pinMode(MH_SENSOR_AO, INPUT);
  pinMode(MH_SENSOR_DO, INPUT);
  
  // Initialize I2C and check for devices
  Wire.begin();
  
  // Initialize LCD (auto-detect)
  initLCD();
  
  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("⚠️ RTC DS3231 not found - continuing without RTC");
    rtcAvailable = false;
    if (lcdAvailable) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("RTC: Not Found");
      delay(2000);
    }
  } else {
    Serial.println("✅ RTC DS3231 found and initialized");
    rtcAvailable = true;
    if (lcdAvailable) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("RTC: Connected");
      delay(2000);
    }
  }
  
  // Load saved configuration
  loadConfig();
  
  // Setup WiFi Manager with Blink integration
  setupWiFiManager();
  setupBlinkIntegration();
  
  // Check for WiFi reset button
  bool forceConfigPortal = false;
  if (digitalRead(WIFI_RESET_BUTTON) == LOW) {
    Serial.println("🔄 WiFi Reset button pressed");
    if (lcdAvailable) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WiFi Resetting..");
    }
    wifiManager.resetSettings();
    forceConfigPortal = true;
    delay(3000);
  }
  
  // Connect to WiFi
  if (forceConfigPortal) {
    Serial.println("🔧 Forcing config portal...");
    if (!wifiManager.startConfigPortal(plantConfig.wifiSSID.c_str(), plantConfig.wifiPassword.c_str())) {
      Serial.println("❌ Failed to connect to WiFi");
      ESP.restart();
    }
  } else {
    if (!wifiManager.autoConnect(plantConfig.wifiSSID.c_str(), plantConfig.wifiPassword.c_str())) {
      Serial.println("❌ Failed to connect to WiFi");
      ESP.restart();
    }
  }
  
  Serial.println("✅ WiFi connected!");
  Serial.print("📶 IP address: ");
  Serial.println(WiFi.localIP());
  
  if (lcdAvailable) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi: Connected");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP().toString());
    delay(3000);
  }
  
  // Authenticate with Blink
  if (strlen(blinkEmail) > 0) {
    Serial.println("🔗 Connecting to Blink...");
    authenticateBlink();
  }
  
  // Setup web server
  setupWebServer();
  server.begin();
  
  Serial.println(SYSTEM_EMOJI + " " + SYSTEM_NAME + " Watering System ready!");
  Serial.println("💧 Moisture thresholds: Start < " + String(plantConfig.moistureThresholdLow) + "%, Stop at " + String(plantConfig.moistureThresholdHigh) + "%");
  
  if (lcdAvailable) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(SYSTEM_EMOJI + " System Ready");
    lcd.setCursor(0, 1);
    lcd.print("Monitoring...");
  }
  
  // Initial Blink status update
  if (blinkConnected) {
    updateBlinkStatus("System Started", "0");
  }
}

void loadConfig() {
  // Load all configuration from EEPROM
  EEPROM.get(0, webhookUrl);
  EEPROM.get(200, webhookEnabled);
  EEPROM.get(250, blinkEmail);
  EEPROM.get(350, blinkPassword);
  EEPROM.get(450, blinkAccountId);
  EEPROM.get(500, blinkNetworkId);
  
  // Validate loaded data
  if (strlen(webhookUrl) == 0 || webhookUrl[0] == 0xFF) {
    strcpy(webhookUrl, "");
    webhookEnabled = false;
  }
  
  if (strlen(blinkEmail) == 0 || blinkEmail[0] == 0xFF) {
    strcpy(blinkEmail, "");
    strcpy(blinkPassword, "");
    strcpy(blinkAccountId, "");
    strcpy(blinkNetworkId, "");
    strcpy(blinkCameraId, "");
  }
}

void saveConfig() {
  // Save all configuration to EEPROM
  EEPROM.put(0, webhookUrl);
  EEPROM.put(200, webhookEnabled);
  EEPROM.put(250, blinkEmail);
  EEPROM.put(350, blinkPassword);
  EEPROM.put(450, blinkAccountId);
  EEPROM.put(500, blinkNetworkId);
  EEPROM.commit();
  Serial.println("📁 Configuration saved to EEPROM");
}

void setupWiFiManager() {
  Serial.println("🔧 Setting up WiFiManager...");
  
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setDebugOutput(true);
  
  // Custom info for the portal
  String customInfo = "<h2>" + SYSTEM_EMOJI + " ระบบรดน้ำ" + SYSTEM_NAME + "อัตโนมัติ</h2>";
  customInfo += "<p>📱 เลือกเครือข่าย WiFi และกรอกรหัสผ่าน</p>";
  customInfo += "<p>💧 ระบบจะรดน้ำอัตโนมัติเมื่อความชื้น < " + String(plantConfig.moistureThresholdLow) + "%</p>";
  customInfo += "<p>🛑 หยุดรดน้ำเมื่อความชื้นถึง " + String(plantConfig.moistureThresholdHigh) + "%</p>";
  customInfo += "<p>📷 ตั้งค่า Blink เพื่อแสดงสถานะ</p>";
  customInfo += "<p>🔗 เข้าใช้งานที่: http://192.168.4.1</p>";
  wifiManager.setCustomHeadElement(customInfo.c_str());
  
  // Add webhook parameter
  WiFiManagerParameter custom_webhook_url("webhook", "Webhook URL", webhookUrl, 200);
  wifiManager.addParameter(&custom_webhook_url);
  
  wifiManager.setConfigPortalTimeout(300);
  
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
    unsigned long uptimeSeconds = millis() / 1000;
    unsigned long hours = uptimeSeconds / 3600;
    unsigned long minutes = (uptimeSeconds % 3600) / 60;
    unsigned long seconds = uptimeSeconds % 60;
    return "UPTIME " + String(hours) + ":" + 
           String(minutes) + ":" + String(seconds);
  }
}

void logSensorData() {
  String logMessage = SYSTEM_EMOJI + " [" + getCurrentTime() + "] " + SYSTEM_NAME + " - Moisture: " + 
                     String(moisturePercent) + "% | Target: " + 
                     String(plantConfig.moistureThresholdLow) + "-" + 
                     String(plantConfig.moistureThresholdHigh) + "% | Status: " + 
                     getZoneStatus(0);
  Serial.println(logMessage);
}

void readAllSensors() {
  // Read MH Sensor
  int rawValue = analogRead(MH_SENSOR_AO);
  bool digitalValue = digitalRead(MH_SENSOR_DO);
  
  moisturePercent = map(rawValue, 4095, 0, 0, 100);
  
  soilMoisture[0] = moisturePercent;
  zones[0].currentMoisture = moisturePercent;
  
  Serial.print("🌡️ MH Sensor - AO: ");
  Serial.print(rawValue);
  Serial.print(" (");
  Serial.print(moisturePercent);
  Serial.print("%) DO: ");
  Serial.println(digitalValue ? "DRY" : "WET");
  
  logSensorData();
  
  // Update LCD
  updateLCD();
  
  // Update Blink status every 5 minutes
  static unsigned long lastBlinkUpdate = 0;
  if (blinkConnected && millis() - lastBlinkUpdate > 300000) { // 5 minutes
    updateBlinkStatus(getZoneStatus(0), String(moisturePercent));
    lastBlinkUpdate = millis();
  }
}

void smartWatering() {
  WateringZone* zone = &zones[0];
  
  if (relayStates[0]) return;
  
  bool needsWater = false;
  String reason = "";
  
  // Check moisture threshold
  if (moisturePercent < plantConfig.moistureThresholdLow) {
    needsWater = true;
    reason += "ความชื้นต่ำ (" + String(moisturePercent) + "% < " + String(plantConfig.moistureThresholdLow) + "%). ";
  }
  
  // Prevent frequent watering
  unsigned long timeSinceWatering = millis() - zone->lastWatered;
  if (timeSinceWatering < (plantConfig.wateringInterval * 60000)) {
    needsWater = false;
    reason = "เพิ่งรดน้ำไปแล้ว, รอก่อน.";
  }
  
  // Daily limit
  if (wateringCount[0] >= plantConfig.maxWateringPerDay) {
    needsWater = false;
    reason = "รดน้ำครบจำนวนแล้ววันนี้.";
  }
  
  // Execute watering if needed
  if (needsWater) {
    int duration = calculateWateringDuration(0);
    startWatering(0, duration);
    Serial.println("💧 เริ่มรดน้ำ" + SYSTEM_NAME + ": " + reason);
    sendWebhook("💧 เริ่มรดน้ำ" + SYSTEM_NAME + ": " + reason, "info");
    
    if (blinkConnected) {
      updateBlinkStatus("กำลังรดน้ำ", String(moisturePercent));
    }
    
    if (lcdAvailable) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Watering...");
      lcd.setCursor(0, 1);
      lcd.print(String(duration) + " minutes");
    }
  }
  
  zone->needsWater = needsWater;
  zone->status = needsWater ? "กำลังรดน้ำ" : getZoneStatus(0);
}

String getZoneStatus(int zoneIndex) {
  WateringZone* zone = &zones[zoneIndex];
  
  if (zone->currentMoisture >= plantConfig.moistureThresholdHigh) {
    return plantConfig.statusLevels[0]; // Optimal
  } else if (zone->currentMoisture >= (plantConfig.moistureThresholdHigh - 15)) {
    return plantConfig.statusLevels[1]; // Good
  } else if (zone->currentMoisture >= plantConfig.moistureThresholdLow) {
    return plantConfig.statusLevels[2]; // Slightly dry
  } else {
    return plantConfig.statusLevels[3]; // Critical
  }
}

int calculateWateringDuration(int zoneIndex) {
  WateringZone* zone = &zones[zoneIndex];
  
  int baseDuration = plantConfig.baseDuration;
  int moistureDeficit = plantConfig.moistureThresholdHigh - zone->currentMoisture;
  float durationMultiplier = 1.0 + (moistureDeficit / 25.0);
  
  // Adjust based on time of day (if RTC available)
  if (rtcAvailable) {
    DateTime now = rtc.now();
    if (now.hour() >= 10 && now.hour() <= 16) {
      durationMultiplier *= 1.2; // 20% longer during midday
    } else if (now.hour() <= 6 || now.hour() >= 20) {
      durationMultiplier *= 0.8; // 20% shorter during cool periods
    }
  }
  
  // Adjust based on growth stage
  if (zone->growthStage < 0.3) {
    durationMultiplier *= 0.8; // Young plants need less water
  } else if (zone->growthStage > 0.7) {
    durationMultiplier *= 1.2; // Mature plants need more water
  }
  
  int finalDuration = (int)(baseDuration * durationMultiplier);
  finalDuration = constrain(finalDuration, 1, 15); // 1-15 minutes max
  
  Serial.println("💧 Watering duration calculated: " + String(finalDuration) + " minutes");
  return finalDuration;
}

void checkWateringSchedules() {
  if (!rtcAvailable) return;
  
  DateTime now = rtc.now();
  
  for (int i = 0; i < plantConfig.scheduleCount; i++) {
    WateringSchedule* schedule = &schedules[i];
    
    if (schedule->enabled && 
        now.hour() == schedule->hour && 
        now.minute() == schedule->minute &&
        now.second() == 0) {
      
      if (!relayStates[schedule->zoneIndex]) {
        startWatering(schedule->zoneIndex, schedule->duration);
        Serial.println("⏰ Scheduled " + SYSTEM_NAME + " watering: Zone " + String(schedule->zoneIndex + 1));
        
        if (blinkConnected) {
          updateBlinkStatus("ตารางรดน้ำ", String(moisturePercent));
        }
      }
    }
  }
}

void startWatering(int zoneIndex, int durationMinutes) {
  if (zoneIndex >= RELAY_COUNT) return;
  
  digitalWrite(relayPins[zoneIndex], LOW);
  relayStates[zoneIndex] = true;
  wateringEndTime[zoneIndex] = millis() + (durationMinutes * 60000);
  lastWateringTime[zoneIndex] = millis();
  wateringCount[zoneIndex]++;
  zones[zoneIndex].lastWatered = millis();
  
  Serial.println("💧 เริ่มรดน้ำ" + SYSTEM_NAME + " โซน " + String(zoneIndex + 1) + " เป็นเวลา " + String(durationMinutes) + " นาที");
}

void stopWatering(int zoneIndex) {
  if (zoneIndex >= RELAY_COUNT) return;
  
  digitalWrite(relayPins[zoneIndex], HIGH);
  relayStates[zoneIndex] = false;
  
  Serial.println("💧 หยุดรดน้ำ" + SYSTEM_NAME + " โซน " + String(zoneIndex + 1));
  
  if (blinkConnected) {
    updateBlinkStatus("รดน้ำเสร็จแล้ว", String(moisturePercent));
  }
  
  if (lcdAvailable) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Watering Done");
    lcd.setCursor(0, 1);
    lcd.print("Moisture: " + String(moisturePercent) + "%");
    delay(2000);
  }
}

void handleActiveWatering() {
  if (relayStates[0]) {
    if (millis() >= wateringEndTime[0] || moisturePercent >= plantConfig.moistureThresholdHigh) {
      if (moisturePercent >= plantConfig.moistureThresholdHigh) {
        Serial.println("💧 หยุดรดน้ำ: ความชื้นถึงเป้าหมาย " + String(moisturePercent) + "%");
        sendWebhook("💧 หยุดรดน้ำ" + SYSTEM_NAME + ": ความชื้นถึง " + String(moisturePercent) + "%", "info");
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
  
  DynamicJsonDocument doc(1024);
  doc["timestamp"] = getCurrentTime();
  doc["system"] = "Universal " + SYSTEM_NAME + " Watering System";
  doc["plant_type"] = PLANT_TYPE;
  doc["message"] = message;
  doc["level"] = level;
  doc["ip"] = WiFi.localIP().toString();
  doc["moisture"] = moisturePercent;
  doc["threshold_low"] = plantConfig.moistureThresholdLow;
  doc["threshold_high"] = plantConfig.moistureThresholdHigh;
  doc["rtc_available"] = rtcAvailable;
  doc["lcd_available"] = lcdAvailable;
  doc["blink_connected"] = blinkConnected;
  
  String payload;
  serializeJson(doc, payload);
  
  int httpCode = http.POST(payload);
  if (httpCode > 0) {
    Serial.println("📤 Webhook sent: " + message);
  }
  
  http.end();
}

void setupWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/manual", HTTP_POST, handleManualWatering);
  server.on("/api/stop", HTTP_POST, handleStopWatering);
  server.on("/api/wifi/reset", HTTP_POST, handleWiFiReset);
  server.on("/api/settings", HTTP_POST, handleSettingsUpdate);
  server.on("/api/blink/test", HTTP_POST, handleBlinkTest);
}

void handleRoot() {
  String plantEmoji = SYSTEM_EMOJI;
  String plantName = SYSTEM_NAME;
  String thresholdLow = String(plantConfig.moistureThresholdLow);
  String thresholdHigh = String(plantConfig.moistureThresholdHigh);
  
  String html = R"(
<!DOCTYPE html>
<html lang="th">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>)" + plantEmoji + R"( ระบบรดน้ำ)" + plantName + R"(อัตโนมัติ</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
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
        .header h1 { color: #4a5568; margin-bottom: 10px; }
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
        .stat-card h3 { color: #4a5568; margin-bottom: 10px; }
        .stat-card div { font-size: 1.5em; font-weight: bold; color: #667eea; }
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
            background: linear-gradient(90deg, #667eea, #764ba2); 
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
        .controls h3 { color: #4a5568; margin-bottom: 15px; }
        button { 
            background: linear-gradient(45deg, #667eea, #764ba2); 
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
            box-shadow: 0 5px 15px rgba(102,126,234,0.4);
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
            max-width: 600px;
            max-height: 80vh;
            overflow-y: auto;
        }
        .close { 
            float: right; 
            font-size: 28px; 
            font-weight: bold; 
            cursor: pointer;
        }
        .device-status {
            background: #f8f9fa;
            padding: 15px;
            border-radius: 10px;
            margin: 10px 0;
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
            <h1>)" + plantEmoji + R"( ระบบรดน้ำ)" + plantName + R"(อัตโนมัติ</h1>
            <p>Universal Smart Watering System - )" + plantName + R"(</p>
            <p><span class="status-indicator status-optimal"></span>ระบบพร้อมใช้งาน | LCD: <span id="lcdStatus">--</span> | Blink: <span id="blinkStatus">--</span></p>
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
            <button onclick="manualWater(0, 10)">💧 รดน้ำ 10 นาที</button>
            <button onclick="showSystemInfo()">📊 ข้อมูลระบบ</button>
            <button onclick="testBlink()">📷 ทดสอบ Blink</button>
            <button onclick="resetWiFi()" class="emergency">🔄 รีเซ็ต WiFi</button>
            <button onclick="emergencyStop()" class="emergency">🚨 หยุดฉุกเฉิน</button>
        </div>

        <!-- System Info Modal -->
        <div id="systemModal" class="modal">
            <div class="modal-content">
                <span class="close" onclick="closeModal()">&times;</span>
                <h2>📊 ข้อมูลระบบ</h2>
                <div id="systemInfoContent" style="text-align:left; line-height:1.8;">
                    <div class="device-status">
                        <h3>🔧 อุปกรณ์ที่เชื่อมต่อ</h3>
                        <p><strong>📶 WiFi:</strong> <span id="modalWifiSignal">--</span> dBm</p>
                        <p><strong>⏰ RTC:</strong> <span id="modalRtcStatus">--</span></p>
                        <p><strong>📱 LCD:</strong> <span id="modalLcdStatus">--</span></p>
                        <p><strong>📷 Blink:</strong> <span id="modalBlinkStatus">--</span></p>
                    </div>
                    
                    <div class="device-status">
                        <h3>📊 ข้อมูลระบบ</h3>
                        <p><strong>💾 หน่วยความจำว่าง:</strong> <span id="modalFreeMemory">--</span> KB</p>
                        <p><strong>⏱️ เวลาทำงาน:</strong> <span id="modalUptime">--</span></p>
                        <p><strong>🌐 IP Address:</strong> <span id="modalIpAddress">--</span></p>
                        <p><strong>💧 รดน้ำวันนี้:</strong> <span id="modalTotalWatering">--</span> ครั้ง</p>
                        <p><strong>🕒 เวลาปัจจุบัน:</strong> <span id="modalCurrentTime">--</span></p>
                        <p><strong>🌱 ชนิดพืช:</strong> )" + plantName + R"(</p>
                        <p><strong>🎯 เกณฑ์ความชื้น:</strong> )" + thresholdLow + R"(% - )" + thresholdHigh + R"(%</p>
                    </div>
                    
                    <hr>
                    <h3>⚙️ ตั้งค่า</h3>
                    <p><label>Webhook URL:</label><br><input type="url" id="webhookUrlInput" style="width:100%; padding:8px; margin:5px 0;" placeholder="https://example.com/webhook"></p>
                    <p><label><input type="checkbox" id="webhookEnabledInput"> เปิดใช้งาน Webhook</label></p>
                    <button onclick="saveSettings()">💾 บันทึกการตั้งค่า</button>
                </div>
            </div>
        </div>
    </div>

    <script>
        setInterval(updateData, 5000);
        updateData();

        function updateData() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('wifiSignal').textContent = data.wifi_strength + ' dBm';
                    document.getElementById('uptime').textContent = formatUptime(data.uptime);
                    document.getElementById('currentTime').textContent = data.timestamp.split(' ')[1] || data.timestamp;
                    document.getElementById('totalWatering').textContent = data.total_watering_today + ' ครั้ง';
                    document.getElementById('freeMemory').textContent = Math.round(data.free_memory / 1024) + ' KB';
                    
                    let statusText = '🔴 ขัดข้อง';
                    if (data.system_status === 'running') {
                        statusText = '🟢 ทำงานปกติ';
                    } else if (data.system_status === 'running_no_rtc') {
                        statusText = '🟡 ทำงาน (ไม่มี RTC)';
                    }
                    document.getElementById('systemStatus').textContent = statusText;
                    
                    // Device status indicators
                    document.getElementById('lcdStatus').textContent = data.lcd_available ? '✅' : '❌';
                    document.getElementById('blinkStatus').textContent = data.blink_connected ? '✅' : '❌';
                    
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
                        <p>🔴 รดน้ำเมื่อ: < )" + thresholdLow + R"(%</p>
                        <p>🟢 หยุดเมื่อ: )" + thresholdHigh + R"(%</p>
                        <p>จำนวนรดน้ำวันนี้: ${zone.watering_count} ครั้ง</p>
                        ${zone.watering ? '<p style="color: #4CAF50;">🚿 กำลังรดน้ำ...</p>' : ''}
                    </div>
                `;
                container.innerHTML += zoneCard;
            });
        }

        function getStatusClass(status) {
            if (status.includes('เหมาะ')) return 'status-optimal';
            if (status.includes('ดี')) return 'status-good';
            if (status.includes('แห้งเล็กน้อย')) return 'status-dry';
            return 'status-critical';
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

        function testBlink() {
            fetch('/api/blink/test', { 
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ message: 'ทดสอบการเชื่อมต่อ Blink จากระบบ' })
            }).then(response => {
                if (response.ok) {
                    alert('ส่งทดสอบ Blink เรียบร้อย');
                } else {
                    alert('ไม่สามารถเชื่อมต่อ Blink ได้');
                }
            });
        }

        function showSystemInfo() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('modalWifiSignal').textContent = data.wifi_strength;
                    document.getElementById('modalFreeMemory').textContent = Math.round(data.free_memory / 1024);
                    document.getElementById('modalUptime').textContent = formatUptime(data.uptime);
                    document.getElementById('modalTotalWatering').textContent = data.total_watering_today;
                    document.getElementById('modalCurrentTime').textContent = data.timestamp;
                    document.getElementById('modalRtcStatus').textContent = data.rtc_available ? '✅ พร้อมใช้งาน' : '⚠️ ไม่พบ RTC';
                    document.getElementById('modalLcdStatus').textContent = data.lcd_available ? '✅ เชื่อมต่อแล้ว' : '⚠️ ไม่พบ LCD';
                    document.getElementById('modalBlinkStatus').textContent = data.blink_connected ? '✅ เชื่อมต่อแล้ว' : '⚠️ ไม่ได้เชื่อมต่อ';
                    document.getElementById('modalIpAddress').textContent = window.location.hostname;
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
  doc["lcd_available"] = lcdAvailable;
  doc["blink_connected"] = blinkConnected;
  doc["plant_type"] = PLANT_TYPE;
  doc["threshold_low"] = plantConfig.moistureThresholdLow;
  doc["threshold_high"] = plantConfig.moistureThresholdHigh;
  
  JsonArray zonesArray = doc.createNestedArray("zones");
  for (int i = 0; i < RELAY_COUNT; i++) {
    JsonObject zone = zonesArray.createNestedObject();
    zone["id"] = i;
    zone["name"] = zones[i].zoneName;
    zone["moisture"] = soilMoisture[i];
    zone["target_moisture"] = zones[i].targetMoisture;
    zone["watering"] = relayStates[i];
    zone["status"] = zones[i].status;
    zone["growth_stage"] = zones[i].growthStage;
    zone["watering_count"] = wateringCount[i];
    zone["last_watered"] = zones[i].lastWatered;
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
      
      if (blinkConnected) {
        updateBlinkStatus("รดน้ำแมนนวล", String(moisturePercent));
      }
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

void handleBlinkTest() {
  if (blinkConnected) {
    updateBlinkStatus("ทดสอบระบบ", String(moisturePercent));
    server.send(200, "text/plain", "Blink test sent");
  } else {
    server.send(400, "text/plain", "Blink not connected");
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
      if (lcdAvailable) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("WiFi Resetting..");
      }
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
  smartWatering();
  
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
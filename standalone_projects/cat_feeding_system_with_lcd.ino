/*
 * RDTRC Complete Cat Feeding System with LCD - Standalone Version
 * Version: 4.0 - Independent System with LCD I2C 16x2 Support
 * Firmware made by: RDTRC
 * Updated: 2024
 * 
 * Features:
 * - Complete standalone cat feeding system
 * - LCD I2C 16x2 display with auto address detection
 * - Automated feeding schedule (6 times/day)
 * - Load cell weight monitoring (HX711)
 * - PIR motion detection
 * - Ultrasonic food level sensor
 * - Built-in web interface
 * - Blynk integration for mobile control
 * - Hotspot mode for direct access
 * - Data logging to SPIFFS
 * - EMAIL/LINE notifications
 * - OTA updates
 * - Manual override controls
 * - LCD debug and status display
 */

// Blynk Configuration - MUST be defined BEFORE includes
#define BLYNK_TEMPLATE_ID "TMPL61Zdwsx9r"
#define BLYNK_TEMPLATE_NAME "Cat_Feeding_System"
#define BLYNK_AUTH_TOKEN "H4AnMNnYtDTRBl1qssnraGZbVmnKoC8e"

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <BlynkSimpleEsp32.h>
#include <HX711.h>
#include <ESP32Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoOTA.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RDTRC_LCD_Library.h"

// System Configuration
#define FIRMWARE_VERSION "4.0"
#define FIRMWARE_MAKER "RDTRC"
#define SYSTEM_NAME "Cat Feeder"
#define DEVICE_ID "RDTRC_CAT_FEEDER"

// Network Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* hotspot_ssid = "RDTRC_CatFeeder";
const char* hotspot_password = "rdtrc123";

// LINE Notify Configuration
const char* lineToken = "YOUR_LINE_NOTIFY_TOKEN";

// Pin Definitions
#define SERVO_PIN 18
#define LOAD_CELL_DOUT_PIN 19
#define LOAD_CELL_SCK_PIN 5    // Changed from 21 to avoid I2C conflict
#define PIR_SENSOR_PIN 22
#define ULTRASONIC_TRIG_PIN 23
#define ULTRASONIC_ECHO_PIN 25
#define BUZZER_PIN 4
#define STATUS_LED_PIN 2
#define RESET_BUTTON_PIN 0
#define MANUAL_FEED_BUTTON_PIN 27
#define LCD_NEXT_BUTTON_PIN 26  // Button to navigate LCD pages

// I2C Pins
#define I2C_SDA 21
#define I2C_SCL 22

// Feeding Configuration
#define DEFAULT_PORTION_SIZE 30  // grams
#define MIN_PORTION_SIZE 5
#define MAX_PORTION_SIZE 100
#define FOOD_CONTAINER_HEIGHT 20  // cm
#define LOW_FOOD_THRESHOLD 3      // cm
#define EMPTY_BOWL_THRESHOLD 5    // grams

// System Objects
WebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); // UTC+7 Thailand
HX711 scale;
Servo feedingServo;
RDTRC_LCD systemLCD;

// System Variables
bool isWiFiConnected = false;
bool isHotspotMode = false;
unsigned long lastHeartbeat = 0;
unsigned long lastDataLog = 0;
unsigned long lastStatusCheck = 0;
unsigned long lastLCDUpdate = 0;
unsigned long bootTime = 0;

// Feeding Variables
struct FeedingSchedule {
  int hour;
  int minute;
  int portion;
  bool enabled;
  String description;
};

// Default feeding schedule (6 times per day)
FeedingSchedule feedingTimes[6] = {
  {7, 0, 30, true, "Morning Feed"},
  {10, 0, 20, true, "Mid Morning"},
  {13, 0, 35, true, "Lunch Feed"},
  {16, 0, 25, true, "Afternoon"},
  {19, 0, 40, true, "Dinner Feed"},
  {22, 0, 20, true, "Night Snack"}
};

// Sensor Variables
float currentWeight = 0;
float foodLevel = 0;
bool motionDetected = false;
unsigned long lastMotionTime = 0;
int dailyFeedings = 0;
float totalFoodDispensed = 0;

// Statistics
struct DailyStats {
  String date;
  int feedingCount;
  float totalFood;
  int motionEvents;
  float avgBowlWeight;
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
void readSensors();
void checkFeedingSchedule();
void performFeeding(int portion);
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
  
  Serial.println("RDTRC Cat Feeding System with LCD Ready!");
  Serial.println("Web Interface: http://cat-feeder.local");
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
  
  // Read sensors every 30 seconds
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead > 30000) {
    readSensors();
    lastSensorRead = millis();
  }
  
  // Check feeding schedule every minute
  static unsigned long lastScheduleCheck = 0;
  if (millis() - lastScheduleCheck > 60000) {
    checkFeedingSchedule();
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
  Serial.println("RDTRC Complete Cat Feeding System with LCD");
  Serial.println("");
  Serial.println("Firmware made by: " + String(FIRMWARE_MAKER));
  Serial.println("Version: " + String(FIRMWARE_VERSION));
  Serial.println("System: " + String(SYSTEM_NAME));
  Serial.println("");
  Serial.println("Features:");
  Serial.println("* LCD I2C 16x2 Display with Auto Detection");
  Serial.println("* Automated Feeding Schedule (6 times/day)");
  Serial.println("* Weight Monitoring (HX711 Load Cell)");
  Serial.println("* Motion Detection (PIR Sensor)");
  Serial.println("* Food Level Monitoring");
  Serial.println("* Web Interface");
  Serial.println("* Mobile App Control");
  Serial.println("* Smart Notifications");
  Serial.println("* Data Logging & Analytics");
  Serial.println("* OTA Updates");
  Serial.println("");
  Serial.println("Initializing Cat Feeding Environment...");
  Serial.println("============================================================");
  delay(3000);
}

void setupSystem() {
  // Initialize pins
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MANUAL_FEED_BUTTON_PIN, INPUT_PULLUP);
  pinMode(LCD_NEXT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  
  // Setup LCD first
  setupLCD();
  
  // Initialize servo
  feedingServo.attach(SERVO_PIN);
  feedingServo.write(0); // Closed position
  systemLCD.showDebug("Servo Init", "Position: 0");
  
  // Initialize load cell
  scale.begin(LOAD_CELL_DOUT_PIN, LOAD_CELL_SCK_PIN);
  scale.set_scale(2280.f); // Calibration factor
  scale.tare(); // Reset scale to 0
  systemLCD.showDebug("Load Cell", "Calibrated");
  
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
  todayStats.feedingCount = 0;
  todayStats.totalFood = 0;
  todayStats.motionEvents = 0;
  todayStats.avgBowlWeight = 0;
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
  String startupMsg = "RDTRC Cat Feeding System Started!\n";
  startupMsg += "System: " + String(SYSTEM_NAME) + "\n";
  startupMsg += "Version: " + String(FIRMWARE_VERSION) + "\n";
  startupMsg += "LCD: ";
  startupMsg += (systemLCD.isLCDConnected() ? "Connected" : "Not Found");
  startupMsg += "\n";
  startupMsg += "Bowl Weight: " + String(currentWeight) + "g\n";
  startupMsg += "Food Level: " + String(foodLevel) + "cm\n";
  startupMsg += "Status: Feeding system ready!";
  sendLineNotification(startupMsg);
  
  systemLCD.showDebug("System Ready!", "Feeding OK");
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
    
    if (MDNS.begin("cat-feeder")) {
      Serial.println("mDNS responder started: cat-feeder.local");
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
  
  if (MDNS.begin("cat-feeder")) {
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
    doc["current_weight"] = currentWeight;
    doc["food_level"] = foodLevel;
    doc["motion_detected"] = motionDetected;
    doc["last_motion"] = lastMotionTime;
    doc["daily_feedings"] = dailyFeedings;
    doc["total_food_dispensed"] = totalFoodDispensed;
    doc["timestamp"] = timeClient.getEpochTime();
    doc["lcd_connected"] = systemLCD.isLCDConnected();
    doc["lcd_address"] = "0x" + String(systemLCD.getLCDAddress(), HEX);
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/feed", HTTP_POST, []() {
    String portionStr = server.arg("portion");
    int portion = portionStr.toInt();
    
    if (portion >= MIN_PORTION_SIZE && portion <= MAX_PORTION_SIZE) {
      performFeeding(portion);
      systemLCD.showDebug("Manual Feed", String(portion) + "g");
      server.send(200, "application/json", "{\"status\":\"feeding_started\",\"portion\":" + String(portion) + "}");
    } else {
      server.send(400, "application/json", "{\"error\":\"invalid_portion_size\"}");
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
    Blynk.virtualWrite(V1, currentWeight);
    Blynk.virtualWrite(V2, foodLevel);
    Blynk.virtualWrite(V3, dailyFeedings);
  }
}

void setupOTA() {
  if (isWiFiConnected) {
    ArduinoOTA.setHostname("cat-feeder");
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
      currentWeight,        // Use weight as "temperature" 
      foodLevel,           // Use food level as "humidity"
      (int)totalFoodDispensed, // Use total food as "moisture"
      "Fed: " + String(dailyFeedings), // Use feeding count as "phase"
      isWiFiConnected,
      false, // No maintenance mode for feeder
      todayStats.alerts
    );
    
    // Update LCD display
    systemLCD.update();
    
    lastLCDUpdate = millis();
  }
}

void readSensors() {
  // Read load cell (bowl weight)
  if (scale.is_ready()) {
    currentWeight = scale.get_units(3); // Average of 3 readings
    if (currentWeight < 0) currentWeight = 0;
  } else {
    Serial.println("Load cell not ready");
    systemLCD.showDebug("Scale Error", "Check HX711");
  }
  
  // Read ultrasonic sensor (food level)
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  
  long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH, 30000);
  if (duration > 0) {
    foodLevel = FOOD_CONTAINER_HEIGHT - (duration * 0.034 / 2);
    if (foodLevel < 0) foodLevel = 0;
    if (foodLevel > FOOD_CONTAINER_HEIGHT) foodLevel = FOOD_CONTAINER_HEIGHT;
  }
  
  // Read PIR sensor
  bool currentMotion = digitalRead(PIR_SENSOR_PIN);
  if (currentMotion && !motionDetected) {
    motionDetected = true;
    lastMotionTime = millis();
    todayStats.motionEvents++;
    systemLCD.showDebug("Motion", "Cat Detected!");
    Serial.println("Motion detected - cat is near!");
  } else if (!currentMotion && motionDetected) {
    motionDetected = false;
  }
  
  // Update Blynk with sensor data
  if (isWiFiConnected) {
    Blynk.virtualWrite(V1, currentWeight);
    Blynk.virtualWrite(V2, foodLevel);
    Blynk.virtualWrite(V3, dailyFeedings);
    Blynk.virtualWrite(V4, motionDetected ? 1 : 0);
  }
}

void checkFeedingSchedule() {
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  for (int i = 0; i < 6; i++) {
    if (feedingTimes[i].enabled && 
        feedingTimes[i].hour == currentHour && 
        feedingTimes[i].minute == currentMinute) {
      
      Serial.println("Scheduled feeding time: " + feedingTimes[i].description);
      systemLCD.showMessage("Feeding Time", feedingTimes[i].description, 3000);
      performFeeding(feedingTimes[i].portion);
      break;
    }
  }
}

void performFeeding(int portion) {
  Serial.println("Starting feeding: " + String(portion) + "g");
  systemLCD.showDebug("Feeding", String(portion) + "g");
  
  // Sound feeding alert
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 1500, 200);
    delay(300);
  }
  
  // Open servo to dispense food
  feedingServo.write(90); // Open position
  systemLCD.showDebug("Dispensing", "Servo Open");
  
  // Calculate dispensing time (rough approximation)
  int dispensingTime = portion * 100; // 100ms per gram
  delay(dispensingTime);
  
  // Close servo
  feedingServo.write(0); // Closed position
  systemLCD.showDebug("Dispensing", "Complete");
  
  // Update statistics
  dailyFeedings++;
  totalFoodDispensed += portion;
  todayStats.feedingCount++;
  todayStats.totalFood += portion;
  
  // Send notification
  String feedMsg = "Cat Fed Successfully!\n";
  feedMsg += "Portion: " + String(portion) + "g\n";
  feedMsg += "Daily Total: " + String(totalFoodDispensed) + "g\n";
  feedMsg += "Feedings Today: " + String(dailyFeedings);
  sendLineNotification(feedMsg);
  
  systemLCD.showMessage("Fed Cat", String(portion) + "g", 5000);
  
  // Save feeding data
  saveSettings();
  
  Serial.println("Feeding completed: " + String(portion) + "g");
}

void checkAlerts() {
  // Low food alert
  if (foodLevel < LOW_FOOD_THRESHOLD) {
    String alertMsg = "Low Food Level Alert!\n";
    alertMsg += "Current Level: " + String(foodLevel) + "cm\n";
    alertMsg += "Please refill the food container.";
    sendLineNotification(alertMsg);
    systemLCD.showAlert("LOW FOOD");
    todayStats.alerts++;
  }
  
  // Empty bowl alert (no food detected for long time)
  if (currentWeight < EMPTY_BOWL_THRESHOLD) {
    String alertMsg = "Empty Food Bowl Alert!\n";
    alertMsg += "Bowl Weight: " + String(currentWeight) + "g\n";
    alertMsg += "Cat may not be eating.";
    sendLineNotification(alertMsg);
    systemLCD.showAlert("EMPTY BOWL");
    todayStats.alerts++;
  }
}

void handleManualControls() {
  // Manual feeding button
  static bool lastFeedButtonState = HIGH;
  static unsigned long lastFeedButtonPress = 0;
  
  bool currentFeedButtonState = digitalRead(MANUAL_FEED_BUTTON_PIN);
  
  if (currentFeedButtonState == LOW && lastFeedButtonState == HIGH) {
    if (millis() - lastFeedButtonPress > 1000) { // Debounce
      Serial.println("Manual feeding button pressed");
      systemLCD.showDebug("Manual Feed", "Button Press");
      performFeeding(DEFAULT_PORTION_SIZE);
      lastFeedButtonPress = millis();
    }
  }
  
  lastFeedButtonState = currentFeedButtonState;
  
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
      sendLineNotification("Cat feeding system reset initiated");
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
  doc["current_weight"] = currentWeight;
  doc["food_level"] = foodLevel;
  doc["motion_detected"] = motionDetected;
  doc["daily_feedings"] = dailyFeedings;
  doc["total_food_dispensed"] = totalFoodDispensed;
  doc["wifi_signal"] = WiFi.RSSI();
  doc["free_memory"] = ESP.getFreeHeap();
  doc["lcd_connected"] = systemLCD.isLCDConnected();
  
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
  doc["daily_feedings"] = dailyFeedings;
  doc["total_food_dispensed"] = totalFoodDispensed;
  doc["today_date"] = todayStats.date;
  
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
      dailyFeedings = doc["daily_feedings"] | 0;
      totalFoodDispensed = doc["total_food_dispensed"] | 0.0;
      todayStats.date = doc["today_date"] | "01/01/2024";
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
    dailyFeedings = 0;
    totalFoodDispensed = 0;
    todayStats.feedingCount = 0;
    todayStats.totalFood = 0;
    todayStats.motionEvents = 0;
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
  html += "<title>RDTRC Cat Feeding System with LCD</title>";
  html += "<meta charset=\"UTF-8\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background: #1a1a1a; color: #fff; margin: 0; padding: 20px; }";
  html += ".header { background: linear-gradient(135deg, #e67e22, #f39c12); padding: 20px; text-align: center; border-radius: 10px; margin-bottom: 20px; }";
  html += ".container { max-width: 1200px; margin: 0 auto; }";
  html += ".status-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; margin-bottom: 20px; }";
  html += ".status-card { background: #2d2d2d; padding: 15px; border-radius: 8px; text-align: center; }";
  html += ".btn { padding: 10px 20px; margin: 5px; border: none; border-radius: 5px; cursor: pointer; font-size: 14px; }";
  html += ".btn-success { background: #27ae60; color: white; }";
  html += ".btn-warning { background: #f39c12; color: white; }";
  html += ".btn-danger { background: #e74c3c; color: white; }";
  html += ".feeding-card { background: #2d2d2d; padding: 20px; border-radius: 10px; margin-bottom: 20px; }";
  html += ".lcd-info { background: #2c3e50; padding: 15px; border-radius: 8px; margin-bottom: 20px; }";
  html += "</style>";
  html += "<script>";
  html += "function refreshStatus() {";
  html += "  fetch('/api/status').then(response => response.json()).then(data => {";
  html += "    document.getElementById('weight').textContent = data.current_weight.toFixed(1) + 'g';";
  html += "    document.getElementById('food-level').textContent = data.food_level.toFixed(1) + 'cm';";
  html += "    document.getElementById('daily-feedings').textContent = data.daily_feedings;";
  html += "    document.getElementById('total-food').textContent = data.total_food_dispensed.toFixed(1) + 'g';";
  html += "    document.getElementById('motion').textContent = data.motion_detected ? 'Detected' : 'None';";
  html += "    document.getElementById('lcd-status').textContent = data.lcd_connected ? 'Connected at ' + data.lcd_address : 'Not Connected';";
  html += "  });";
  html += "}";
  html += "function feedCat(portion) {";
  html += "  fetch('/api/feed', {";
  html += "    method: 'POST',";
  html += "    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },";
  html += "    body: 'portion=' + portion";
  html += "  }).then(() => setTimeout(refreshStatus, 2000));";
  html += "}";
  html += "setInterval(refreshStatus, 30000);";
  html += "window.onload = refreshStatus;";
  html += "</script>";
  html += "</head><body>";
  html += "<div class=\"container\">";
  html += "<div class=\"header\">";
  html += "<h1>RDTRC Cat Feeding System</h1>";
  html += "<p>Automated Cat Care with LCD Display - v4.0</p>";
  html += "</div>";
  html += "<div class=\"lcd-info\">";
  html += "<h3>LCD Display Status</h3>";
  html += "<p>LCD Status: <span id=\"lcd-status\">Loading...</span></p>";
  html += "<p>LCD shows: Bowl weight, Food level, Daily feedings, Motion detection</p>";
  html += "<p>Use the LCD button (Pin 26) to navigate between pages manually</p>";
  html += "</div>";
  html += "<div class=\"status-grid\">";
  html += "<div class=\"status-card\"><h3>Bowl Weight</h3><div id=\"weight\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Food Level</h3><div id=\"food-level\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Daily Feedings</h3><div id=\"daily-feedings\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Total Food</h3><div id=\"total-food\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Motion</h3><div id=\"motion\">Loading...</div></div>";
  html += "</div>";
  html += "<div class=\"feeding-card\">";
  html += "<h2>Manual Feeding Controls</h2>";
  html += "<button class=\"btn btn-success\" onclick=\"feedCat(20)\">Small (20g)</button>";
  html += "<button class=\"btn btn-warning\" onclick=\"feedCat(30)\">Medium (30g)</button>";
  html += "<button class=\"btn btn-danger\" onclick=\"feedCat(50)\">Large (50g)</button>";
  html += "</div>";
  html += "</div>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// Blynk Virtual Pin Handlers
BLYNK_WRITE(V10) { // Manual feeding from Blynk
  int portion = param.asInt();
  if (portion >= MIN_PORTION_SIZE && portion <= MAX_PORTION_SIZE) {
    performFeeding(portion);
    systemLCD.showDebug("Blynk Feed", String(portion) + "g");
  }
}

// Read-only pins for Blynk dashboard
BLYNK_READ(V1) { Blynk.virtualWrite(V1, currentWeight); }
BLYNK_READ(V2) { Blynk.virtualWrite(V2, foodLevel); }
BLYNK_READ(V3) { Blynk.virtualWrite(V3, dailyFeedings); }
BLYNK_READ(V4) { Blynk.virtualWrite(V4, motionDetected ? 1 : 0); }
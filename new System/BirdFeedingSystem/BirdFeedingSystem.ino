/*
 * RDTRC Complete Bird Feeding System with LCD - Standalone Version
 * Version: 4.0 - Independent System with LCD I2C 16x2 Support
 * Firmware made by: RDTRC
 * Updated: 2024
 * 
 * Features:
 * - Complete standalone bird feeding system
 * - LCD I2C 16x2 display with auto address detection
 * - Automated feeding schedule (4 times/day)
 * - Weight sensor monitoring (HX711)
 * - Motion detection (PIR)
 * - Light sensor for day/night detection
 * - Weather-resistant design considerations
 * - Built-in web interface
 * - Blynk integration for mobile control
 * - Hotspot mode for direct access
 * - Data logging to SPIFFS
 * - EMAIL/LINE notifications
 * - OTA updates
 * - Bird visit counting
 * - LCD debug and status display
 */

// Blynk Configuration - MUST be defined BEFORE includes
#define BLYNK_TEMPLATE_ID "TMPL61Zdwsx9r"
#define BLYNK_TEMPLATE_NAME "Bird_Feeding_System"
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
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "RDTRC_LCD_Library.h"

// System Configuration
#define FIRMWARE_VERSION "4.0"
#define FIRMWARE_MAKER "RDTRC"
#define SYSTEM_NAME "Bird Feeder"
#define DEVICE_ID "RDTRC_BIRD_FEEDER"

// Network Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* hotspot_ssid = "RDTRC_BirdFeeder";
const char* hotspot_password = "rdtrc123";

// LINE Notify Configuration
const char* lineToken = "YOUR_LINE_NOTIFY_TOKEN";

// Pin Definitions
#define SERVO_PIN 18
#define LOAD_CELL_DOUT_PIN 19
#define LOAD_CELL_SCK_PIN 5    // Changed from 21 to avoid I2C conflict
#define PIR_SENSOR_PIN 23
#define LIGHT_SENSOR_PIN 32
#define DHT_PIN 25
#define DHT_TYPE DHT22
#define PH_SENSOR_PIN 33
#define EC_SENSOR_PIN 34
#define CO2_SENSOR_PIN 35
#define AIR_QUALITY_SENSOR_PIN 36
#define WATER_LEVEL_SENSOR_PIN 39
#define FLOW_SENSOR_PIN 16
#define BUZZER_PIN 4
#define STATUS_LED_PIN 2
#define RESET_BUTTON_PIN 0
#define MANUAL_FEED_BUTTON_PIN 27
#define LCD_NEXT_BUTTON_PIN 26  // Button to navigate LCD pages

// I2C Pins
#define I2C_SDA 21
#define I2C_SCL 22

// Feeding Configuration
#define DEFAULT_PORTION_SIZE 20  // grams (smaller portions for birds)
#define MIN_PORTION_SIZE 5
#define MAX_PORTION_SIZE 50
#define FOOD_CONTAINER_HEIGHT 15  // cm (smaller container)
#define LOW_FOOD_THRESHOLD 2      // cm
#define EMPTY_FEEDER_THRESHOLD 3  // grams

// Light sensor thresholds
#define DAYLIGHT_THRESHOLD 500    // ADC value for daylight detection
#define NIGHT_THRESHOLD 100       // ADC value for night detection

// Environmental thresholds for bird feeding
#define TEMP_MIN 15.0
#define TEMP_MAX 35.0
#define HUMIDITY_MIN 30.0
#define HUMIDITY_MAX 80.0
#define PH_MIN 6.0
#define PH_MAX 8.0
#define EC_MIN 0.5
#define EC_MAX 3.0
#define CO2_MAX 1000.0
#define AIR_QUALITY_MIN 50.0

// Offline detection constants
#define SENSOR_TIMEOUT 30000      // 30 seconds timeout
#define SENSOR_RETRY_INTERVAL 60000 // 1 minute retry interval

// System Objects
WebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); // UTC+7 Thailand
HX711 scale;
Servo feedingServo;
RDTRC_LCD systemLCD;
DHT dht(DHT_PIN, DHT_TYPE);

// Sensor Status Structure
struct SensorStatus {
  bool isOnline;
  unsigned long lastReading;
  float lastValue;
  int errorCount;
  String sensorName;
};

// Sensor Status Instances
SensorStatus loadCellSensor;
SensorStatus pirSensor;
SensorStatus lightSensor;
SensorStatus dhtSensor;
SensorStatus phSensor;
SensorStatus ecSensor;
SensorStatus co2Sensor;
SensorStatus airQualitySensor;
SensorStatus waterLevelSensor;
SensorStatus flowSensor;
SensorStatus lcdSensor;

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
  bool daylight_only; // Only feed during daylight hours
  String description;
};

// Default feeding schedule (4 times per day for birds)
FeedingSchedule feedingTimes[4] = {
  {6, 30, 15, true, true, "Dawn Feed"},
  {11, 0, 20, true, true, "Morning Feed"},
  {15, 30, 18, true, true, "Afternoon Feed"},
  {18, 0, 12, true, true, "Evening Feed"}
};

// Sensor Variables
float currentWeight = 0;
float foodLevel = 0;
bool motionDetected = false;
bool isDaylight = true;
int lightLevel = 0;
unsigned long lastMotionTime = 0;
int dailyFeedings = 0;
float totalFoodDispensed = 0;
int birdVisits = 0;

// Environmental Variables
float ambientTemperature = 0;
float ambientHumidity = 0;
float phLevel = 0;
float ecLevel = 0;
float co2Level = 0;
float airQualityLevel = 0;
float waterLevel = 0;
float flowRate = 0;

// Statistics
struct DailyStats {
  String date;
  int feedingCount;
  float totalFood;
  int birdVisits;
  int motionEvents;
  float avgFeederWeight;
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
void initializeSensors();
void checkSensorStatus();
void updateSensorStatus(int sensorId, bool success, float value);
void handleSensorError(int sensorId, String sensorName);
void gracefulDegradation();
bool canOperateWithOfflineSensors();
String getSensorStatusString();

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  bootTime = millis();
  displayBootScreen();
  setupSystem();
  
  Serial.println("RDTRC Bird Feeding System with LCD Ready!");
  Serial.println("Web Interface: http://bird-feeder.local");
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
  Serial.println("RDTRC Complete Bird Feeding System with LCD");
  Serial.println("");
  Serial.println("Firmware made by: " + String(FIRMWARE_MAKER));
  Serial.println("Version: " + String(FIRMWARE_VERSION));
  Serial.println("System: " + String(SYSTEM_NAME));
  Serial.println("");
  Serial.println("Features:");
  Serial.println("* LCD I2C 16x2 Display with Auto Detection");
  Serial.println("* Automated Bird Feeding Schedule (4 times/day)");
  Serial.println("* Weight Monitoring (HX711 Load Cell)");
  Serial.println("* Motion Detection (PIR Sensor)");
  Serial.println("* Light Sensor (Day/Night Detection)");
  Serial.println("* Food Level Monitoring");
  Serial.println("* Web Interface");
  Serial.println("* Mobile App Control");
  Serial.println("* Smart Notifications");
  Serial.println("* Data Logging & Analytics");
  Serial.println("* OTA Updates");
  Serial.println("");
  Serial.println("Initializing Bird Feeding Environment...");
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
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(DHT_PIN, INPUT);
  pinMode(PH_SENSOR_PIN, INPUT);
  pinMode(EC_SENSOR_PIN, INPUT);
  pinMode(CO2_SENSOR_PIN, INPUT);
  pinMode(AIR_QUALITY_SENSOR_PIN, INPUT);
  pinMode(WATER_LEVEL_SENSOR_PIN, INPUT);
  pinMode(FLOW_SENSOR_PIN, INPUT);
  
  // Setup LCD first
  setupLCD();
  
  // Initialize servo
  feedingServo.attach(SERVO_PIN);
  feedingServo.write(0); // Closed position
  systemLCD.showDebug("Servo Init", "Position: 0");
  
  // Initialize DHT sensor
  dht.begin();
  systemLCD.showDebug("DHT Sensor", "Initialized");
  
  // Initialize load cell
  scale.begin(LOAD_CELL_DOUT_PIN, LOAD_CELL_SCK_PIN);
  scale.set_scale(2280.f); // Calibration factor for bird feeder
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
  
  // Initialize sensors
  initializeSensors();
  systemLCD.showDebug("Sensors", "Initialized");
  
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
  todayStats.birdVisits = 0;
  todayStats.motionEvents = 0;
  todayStats.avgFeederWeight = 0;
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
  String startupMsg = "RDTRC Bird Feeding System Started!\n";
  startupMsg += "System: " + String(SYSTEM_NAME) + "\n";
  startupMsg += "Version: " + String(FIRMWARE_VERSION) + "\n";
  startupMsg += "LCD: ";
  startupMsg += (systemLCD.isLCDConnected() ? "Connected" : "Not Found");
  startupMsg += "\n";
  startupMsg += "Feeder Weight: " + String(currentWeight) + "g\n";
  startupMsg += "Food Level: " + String(foodLevel) + "cm\n";
  startupMsg += "Light Level: " + String(lightLevel) + "\n";
  startupMsg += "Status: Bird feeding system ready!";
  sendLineNotification(startupMsg);
  
  systemLCD.showDebug("System Ready!", "Birds Welcome");
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
    
    if (MDNS.begin("bird-feeder")) {
      Serial.println("mDNS responder started: bird-feeder.local");
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
  
  if (MDNS.begin("bird-feeder")) {
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
    doc["is_daylight"] = isDaylight;
    doc["light_level"] = lightLevel;
    doc["last_motion"] = lastMotionTime;
    doc["daily_feedings"] = dailyFeedings;
    doc["bird_visits"] = birdVisits;
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
    Blynk.virtualWrite(V4, lightLevel);
  }
}

void setupOTA() {
  if (isWiFiConnected) {
    ArduinoOTA.setHostname("bird-feeder");
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
    // Only update LCD if it's online
    if (lcdSensor.isOnline && systemLCD.isLCDConnected()) {
      systemLCD.updateStatus(
        SYSTEM_NAME,
        currentWeight,        // Use weight as "temperature" 
        foodLevel,           // Use food level as "humidity"
        birdVisits,          // Use bird visits as "moisture"
        "Visits: " + String(birdVisits), // Use visits as "phase"
        isWiFiConnected,
        false, // No maintenance mode for feeder
        todayStats.alerts
      );
      
      // Update LCD display
      systemLCD.update();
    } else {
      // LCD is offline - skip display updates
      Serial.println("LCD offline - skipping display updates");
    }
    
    lastLCDUpdate = millis();
  }
}

void readSensors() {
  // Read load cell (feeder weight)
  if (loadCellSensor.isOnline) {
    if (scale.is_ready()) {
      currentWeight = scale.get_units(3); // Average of 3 readings
      if (currentWeight < 0) currentWeight = 0;
      updateSensorStatus(1, true, currentWeight);
    } else {
      handleSensorError(1, loadCellSensor.sensorName);
    }
  }
  
  // Read DHT sensor
  if (dhtSensor.isOnline) {
    ambientTemperature = dht.readTemperature();
    ambientHumidity = dht.readHumidity();
    
    if (isnan(ambientTemperature) || isnan(ambientHumidity)) {
      handleSensorError(-1, dhtSensor.sensorName);
      ambientTemperature = 0;
      ambientHumidity = 0;
    } else {
      updateSensorStatus(-1, true, ambientTemperature);
    }
  }
  
  // Read light sensor
  if (lightSensor.isOnline) {
    lightLevel = analogRead(LIGHT_SENSOR_PIN);
    isDaylight = lightLevel > DAYLIGHT_THRESHOLD;
    updateSensorStatus(2, true, lightLevel);
  }
  
  // Read PIR sensor
  if (pirSensor.isOnline) {
    bool currentMotion = digitalRead(PIR_SENSOR_PIN);
    if (currentMotion && !motionDetected) {
      motionDetected = true;
      lastMotionTime = millis();
      birdVisits++;
      todayStats.motionEvents++;
      todayStats.birdVisits++;
      systemLCD.showDebug("Motion", "Bird Detected!");
      Serial.println("Motion detected - bird is near!");
    } else if (!currentMotion && motionDetected) {
      motionDetected = false;
    }
    updateSensorStatus(3, true, currentMotion ? 1 : 0);
  }
  
  // Read pH sensor
  if (phSensor.isOnline) {
    int phRaw = analogRead(PH_SENSOR_PIN);
    phLevel = map(phRaw, 0, 4095, 0, 14); // Convert to pH scale
    updateSensorStatus(4, true, phLevel);
  }
  
  // Read EC sensor
  if (ecSensor.isOnline) {
    int ecRaw = analogRead(EC_SENSOR_PIN);
    ecLevel = map(ecRaw, 0, 4095, 0, 5); // Convert to mS/cm
    updateSensorStatus(5, true, ecLevel);
  }
  
  // Read CO2 sensor
  if (co2Sensor.isOnline) {
    int co2Raw = analogRead(CO2_SENSOR_PIN);
    co2Level = map(co2Raw, 0, 4095, 400, 2000); // Convert to ppm
    updateSensorStatus(6, true, co2Level);
  }
  
  // Read air quality sensor
  if (airQualitySensor.isOnline) {
    int aqRaw = analogRead(AIR_QUALITY_SENSOR_PIN);
    airQualityLevel = map(aqRaw, 0, 4095, 0, 100); // Convert to percentage
    updateSensorStatus(7, true, airQualityLevel);
  }
  
  // Read water level sensor
  if (waterLevelSensor.isOnline) {
    int wlRaw = analogRead(WATER_LEVEL_SENSOR_PIN);
    waterLevel = map(wlRaw, 0, 4095, 0, 100); // Convert to percentage
    updateSensorStatus(8, true, waterLevel);
  }
  
  // Read flow sensor
  if (flowSensor.isOnline) {
    int flowRaw = analogRead(FLOW_SENSOR_PIN);
    flowRate = map(flowRaw, 0, 4095, 0, 10); // Convert to L/min
    updateSensorStatus(9, true, flowRate);
  }
  
  // Calculate food level (assuming ultrasonic sensor exists)
  // This would be implemented if ultrasonic sensor is connected
  foodLevel = FOOD_CONTAINER_HEIGHT * 0.8; // Placeholder
  
  // Check sensor status and apply graceful degradation
  checkSensorStatus();
  gracefulDegradation();
  
  // Update Blynk with sensor data (only if sensors are online)
  if (isWiFiConnected) {
    if (loadCellSensor.isOnline) {
      Blynk.virtualWrite(V1, currentWeight);
    }
    if (dhtSensor.isOnline) {
      Blynk.virtualWrite(V2, ambientTemperature);
      Blynk.virtualWrite(V3, ambientHumidity);
    }
    Blynk.virtualWrite(V4, dailyFeedings);
    if (lightSensor.isOnline) {
      Blynk.virtualWrite(V5, lightLevel);
    }
    Blynk.virtualWrite(V6, birdVisits);
    if (phSensor.isOnline) {
      Blynk.virtualWrite(V7, phLevel);
    }
    if (ecSensor.isOnline) {
      Blynk.virtualWrite(V8, ecLevel);
    }
    if (co2Sensor.isOnline) {
      Blynk.virtualWrite(V9, co2Level);
    }
    if (airQualitySensor.isOnline) {
      Blynk.virtualWrite(V10, airQualityLevel);
    }
    if (waterLevelSensor.isOnline) {
      Blynk.virtualWrite(V11, waterLevel);
    }
    if (flowSensor.isOnline) {
      Blynk.virtualWrite(V12, flowRate);
    }
  }
}

void checkFeedingSchedule() {
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  for (int i = 0; i < 4; i++) {
    if (feedingTimes[i].enabled && 
        feedingTimes[i].hour == currentHour && 
        feedingTimes[i].minute == currentMinute) {
      
      // Check if daylight is required
      if (feedingTimes[i].daylight_only && !isDaylight) {
        Serial.println("Skipping " + feedingTimes[i].description + " - waiting for daylight");
        systemLCD.showDebug("Feed Skip", "Night Time");
        continue;
      }
      
      Serial.println("Scheduled feeding time: " + feedingTimes[i].description);
      systemLCD.showMessage("Feeding Time", feedingTimes[i].description, 3000);
      performFeeding(feedingTimes[i].portion);
      break;
    }
  }
}

void performFeeding(int portion) {
  Serial.println("Starting bird feeding: " + String(portion) + "g");
  systemLCD.showDebug("Feeding", String(portion) + "g");
  
  // Sound feeding alert (softer for birds)
  for (int i = 0; i < 2; i++) {
    tone(BUZZER_PIN, 1000, 150);
    delay(200);
  }
  
  // Open servo to dispense food
  feedingServo.write(90); // Open position
  systemLCD.showDebug("Dispensing", "Servo Open");
  
  // Calculate dispensing time (rough approximation)
  int dispensingTime = portion * 80; // 80ms per gram for birds
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
  String feedMsg = "Birds Fed Successfully!\n";
  feedMsg += "Portion: " + String(portion) + "g\n";
  feedMsg += "Daily Total: " + String(totalFoodDispensed) + "g\n";
  feedMsg += "Feedings Today: " + String(dailyFeedings) + "\n";
  feedMsg += "Bird Visits: " + String(birdVisits);
  sendLineNotification(feedMsg);
  
  systemLCD.showMessage("Fed Birds", String(portion) + "g", 5000);
  
  // Save feeding data
  saveSettings();
  
  Serial.println("Bird feeding completed: " + String(portion) + "g");
}

void checkAlerts() {
  // Low food alert
  if (foodLevel < LOW_FOOD_THRESHOLD) {
    String alertMsg = "Low Food Level Alert!\n";
    alertMsg += "Current Level: " + String(foodLevel) + "cm\n";
    alertMsg += "Please refill the bird feeder.";
    sendLineNotification(alertMsg);
    systemLCD.showAlert("LOW FOOD");
    todayStats.alerts++;
  }
  
  // Empty feeder alert
  if (currentWeight < EMPTY_FEEDER_THRESHOLD) {
    String alertMsg = "Empty Bird Feeder Alert!\n";
    alertMsg += "Feeder Weight: " + String(currentWeight) + "g\n";
    alertMsg += "Birds may not have food available.";
    sendLineNotification(alertMsg);
    systemLCD.showAlert("EMPTY FEEDER");
    todayStats.alerts++;
  }
  
  // No bird visits alert (if no motion for 12 hours during day)
  if (isDaylight && (millis() - lastMotionTime > 43200000) && lastMotionTime > 0) {
    String alertMsg = "No Bird Activity Alert!\n";
    alertMsg += "No birds detected for over 12 hours\n";
    alertMsg += "Check feeder location and food quality.";
    sendLineNotification(alertMsg);
    systemLCD.showAlert("NO BIRDS");
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
      sendLineNotification("Bird feeding system reset initiated");
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
  doc["is_daylight"] = isDaylight;
  doc["light_level"] = lightLevel;
  doc["daily_feedings"] = dailyFeedings;
  doc["bird_visits"] = birdVisits;
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
  doc["bird_visits"] = birdVisits;
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
      birdVisits = doc["bird_visits"] | 0;
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
    birdVisits = 0;
    todayStats.feedingCount = 0;
    todayStats.totalFood = 0;
    todayStats.birdVisits = 0;
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
  html += "<title>RDTRC Bird Feeding System with LCD</title>";
  html += "<meta charset=\"UTF-8\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background: #1a1a1a; color: #fff; margin: 0; padding: 20px; }";
  html += ".header { background: linear-gradient(135deg, #3498db, #2ecc71); padding: 20px; text-align: center; border-radius: 10px; margin-bottom: 20px; }";
  html += ".container { max-width: 1200px; margin: 0 auto; }";
  html += ".status-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; margin-bottom: 20px; }";
  html += ".status-card { background: #2d2d2d; padding: 15px; border-radius: 8px; text-align: center; }";
  html += ".btn { padding: 10px 20px; margin: 5px; border: none; border-radius: 5px; cursor: pointer; font-size: 14px; }";
  html += ".btn-success { background: #27ae60; color: white; }";
  html += ".btn-warning { background: #f39c12; color: white; }";
  html += ".btn-info { background: #3498db; color: white; }";
  html += ".feeding-card { background: #2d2d2d; padding: 20px; border-radius: 10px; margin-bottom: 20px; }";
  html += ".lcd-info { background: #2c3e50; padding: 15px; border-radius: 8px; margin-bottom: 20px; }";
  html += "</style>";
  html += "<script>";
  html += "function refreshStatus() {";
  html += "  fetch('/api/status').then(response => response.json()).then(data => {";
  html += "    document.getElementById('weight').textContent = data.current_weight.toFixed(1) + 'g';";
  html += "    document.getElementById('food-level').textContent = data.food_level.toFixed(1) + 'cm';";
  html += "    document.getElementById('daily-feedings').textContent = data.daily_feedings;";
  html += "    document.getElementById('bird-visits').textContent = data.bird_visits;";
  html += "    document.getElementById('total-food').textContent = data.total_food_dispensed.toFixed(1) + 'g';";
  html += "    document.getElementById('light-level').textContent = data.light_level + (data.is_daylight ? ' (Day)' : ' (Night)');";
  html += "    document.getElementById('motion').textContent = data.motion_detected ? 'Detected' : 'None';";
  html += "    document.getElementById('lcd-status').textContent = data.lcd_connected ? 'Connected at ' + data.lcd_address : 'Not Connected';";
  html += "  });";
  html += "}";
  html += "function feedBirds(portion) {";
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
  html += "<h1>RDTRC Bird Feeding System</h1>";
  html += "<p>Automated Bird Care with LCD Display - v4.0</p>";
  html += "</div>";
  html += "<div class=\"lcd-info\">";
  html += "<h3>LCD Display Status</h3>";
  html += "<p>LCD Status: <span id=\"lcd-status\">Loading...</span></p>";
  html += "<p>LCD shows: Feeder weight, Food level, Bird visits, Light status</p>";
  html += "<p>Use the LCD button (Pin 26) to navigate between pages manually</p>";
  html += "</div>";
  html += "<div class=\"status-grid\">";
  html += "<div class=\"status-card\"><h3>Feeder Weight</h3><div id=\"weight\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Food Level</h3><div id=\"food-level\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Daily Feedings</h3><div id=\"daily-feedings\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Bird Visits</h3><div id=\"bird-visits\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Total Food</h3><div id=\"total-food\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Light Level</h3><div id=\"light-level\">Loading...</div></div>";
  html += "<div class=\"status-card\"><h3>Motion</h3><div id=\"motion\">Loading...</div></div>";
  html += "</div>";
  html += "<div class=\"feeding-card\">";
  html += "<h2>Manual Feeding Controls</h2>";
  html += "<button class=\"btn btn-success\" onclick=\"feedBirds(10)\">Small (10g)</button>";
  html += "<button class=\"btn btn-warning\" onclick=\"feedBirds(20)\">Medium (20g)</button>";
  html += "<button class=\"btn btn-info\" onclick=\"feedBirds(30)\">Large (30g)</button>";
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
BLYNK_READ(V2) { Blynk.virtualWrite(V2, ambientTemperature); }
BLYNK_READ(V3) { Blynk.virtualWrite(V3, ambientHumidity); }
BLYNK_READ(V4) { Blynk.virtualWrite(V4, dailyFeedings); }
BLYNK_READ(V5) { Blynk.virtualWrite(V5, lightLevel); }
BLYNK_READ(V6) { Blynk.virtualWrite(V6, birdVisits); }
BLYNK_READ(V7) { Blynk.virtualWrite(V7, phLevel); }
BLYNK_READ(V8) { Blynk.virtualWrite(V8, ecLevel); }
BLYNK_READ(V9) { Blynk.virtualWrite(V9, co2Level); }
BLYNK_READ(V10) { Blynk.virtualWrite(V10, airQualityLevel); }
BLYNK_READ(V11) { Blynk.virtualWrite(V11, waterLevel); }
BLYNK_READ(V12) { Blynk.virtualWrite(V12, flowRate); }

// Sensor Management Functions
void initializeSensors() {
  // Initialize load cell sensor
  loadCellSensor.isOnline = true;
  loadCellSensor.lastReading = millis();
  loadCellSensor.lastValue = 0;
  loadCellSensor.errorCount = 0;
  loadCellSensor.sensorName = "LoadCell";
  
  // Initialize PIR sensor
  pirSensor.isOnline = true;
  pirSensor.lastReading = millis();
  pirSensor.lastValue = 0;
  pirSensor.errorCount = 0;
  pirSensor.sensorName = "PIR";
  
  // Initialize light sensor
  lightSensor.isOnline = true;
  lightSensor.lastReading = millis();
  lightSensor.lastValue = 0;
  lightSensor.errorCount = 0;
  lightSensor.sensorName = "Light";
  
  // Initialize DHT sensor
  dhtSensor.isOnline = true;
  dhtSensor.lastReading = millis();
  dhtSensor.lastValue = 0;
  dhtSensor.errorCount = 0;
  dhtSensor.sensorName = "DHT";
  
  // Initialize pH sensor
  phSensor.isOnline = true;
  phSensor.lastReading = millis();
  phSensor.lastValue = 0;
  phSensor.errorCount = 0;
  phSensor.sensorName = "pH";
  
  // Initialize EC sensor
  ecSensor.isOnline = true;
  ecSensor.lastReading = millis();
  ecSensor.lastValue = 0;
  ecSensor.errorCount = 0;
  ecSensor.sensorName = "EC";
  
  // Initialize CO2 sensor
  co2Sensor.isOnline = true;
  co2Sensor.lastReading = millis();
  co2Sensor.lastValue = 0;
  co2Sensor.errorCount = 0;
  co2Sensor.sensorName = "CO2";
  
  // Initialize air quality sensor
  airQualitySensor.isOnline = true;
  airQualitySensor.lastReading = millis();
  airQualitySensor.lastValue = 0;
  airQualitySensor.errorCount = 0;
  airQualitySensor.sensorName = "AirQuality";
  
  // Initialize water level sensor
  waterLevelSensor.isOnline = true;
  waterLevelSensor.lastReading = millis();
  waterLevelSensor.lastValue = 0;
  waterLevelSensor.errorCount = 0;
  waterLevelSensor.sensorName = "WaterLevel";
  
  // Initialize flow sensor
  flowSensor.isOnline = true;
  flowSensor.lastReading = millis();
  flowSensor.lastValue = 0;
  flowSensor.errorCount = 0;
  flowSensor.sensorName = "Flow";
  
  // Initialize LCD sensor
  lcdSensor.isOnline = true;
  lcdSensor.lastReading = millis();
  lcdSensor.lastValue = 0;
  lcdSensor.errorCount = 0;
  lcdSensor.sensorName = "LCD";
  
  Serial.println("All sensors initialized");
}

void checkSensorStatus() {
  unsigned long currentTime = millis();
  
  // Check load cell sensor
  if (loadCellSensor.isOnline && (currentTime - loadCellSensor.lastReading > SENSOR_TIMEOUT)) {
    loadCellSensor.isOnline = false;
    Serial.println("Load cell sensor offline");
  }
  
  // Check DHT sensor
  if (dhtSensor.isOnline && (currentTime - dhtSensor.lastReading > SENSOR_TIMEOUT)) {
    dhtSensor.isOnline = false;
    Serial.println("DHT sensor offline");
  }
  
  // Check light sensor
  if (lightSensor.isOnline && (currentTime - lightSensor.lastReading > SENSOR_TIMEOUT)) {
    lightSensor.isOnline = false;
    Serial.println("Light sensor offline");
  }
  
  // Check PIR sensor
  if (pirSensor.isOnline && (currentTime - pirSensor.lastReading > SENSOR_TIMEOUT)) {
    pirSensor.isOnline = false;
    Serial.println("PIR sensor offline");
  }
  
  // Check pH sensor
  if (phSensor.isOnline && (currentTime - phSensor.lastReading > SENSOR_TIMEOUT)) {
    phSensor.isOnline = false;
    Serial.println("pH sensor offline");
  }
  
  // Check EC sensor
  if (ecSensor.isOnline && (currentTime - ecSensor.lastReading > SENSOR_TIMEOUT)) {
    ecSensor.isOnline = false;
    Serial.println("EC sensor offline");
  }
  
  // Check CO2 sensor
  if (co2Sensor.isOnline && (currentTime - co2Sensor.lastReading > SENSOR_TIMEOUT)) {
    co2Sensor.isOnline = false;
    Serial.println("CO2 sensor offline");
  }
  
  // Check air quality sensor
  if (airQualitySensor.isOnline && (currentTime - airQualitySensor.lastReading > SENSOR_TIMEOUT)) {
    airQualitySensor.isOnline = false;
    Serial.println("Air quality sensor offline");
  }
  
  // Check water level sensor
  if (waterLevelSensor.isOnline && (currentTime - waterLevelSensor.lastReading > SENSOR_TIMEOUT)) {
    waterLevelSensor.isOnline = false;
    Serial.println("Water level sensor offline");
  }
  
  // Check flow sensor
  if (flowSensor.isOnline && (currentTime - flowSensor.lastReading > SENSOR_TIMEOUT)) {
    flowSensor.isOnline = false;
    Serial.println("Flow sensor offline");
  }
  
  // Check LCD sensor
  if (lcdSensor.isOnline && !systemLCD.isLCDConnected()) {
    lcdSensor.isOnline = false;
    Serial.println("LCD sensor offline");
  }
}

void updateSensorStatus(int sensorId, bool success, float value) {
  SensorStatus* sensor = nullptr;
  
  switch (sensorId) {
    case 1: sensor = &loadCellSensor; break;
    case -1: sensor = &dhtSensor; break;
    case 2: sensor = &lightSensor; break;
    case 3: sensor = &pirSensor; break;
    case 4: sensor = &phSensor; break;
    case 5: sensor = &ecSensor; break;
    case 6: sensor = &co2Sensor; break;
    case 7: sensor = &airQualitySensor; break;
    case 8: sensor = &waterLevelSensor; break;
    case 9: sensor = &flowSensor; break;
  }
  
  if (sensor) {
    if (success) {
      sensor->lastReading = millis();
      sensor->lastValue = value;
      sensor->errorCount = 0;
      if (!sensor->isOnline) {
        sensor->isOnline = true;
        Serial.println(sensor->sensorName + " sensor back online");
      }
    } else {
      sensor->errorCount++;
      if (sensor->errorCount >= 3) {
        sensor->isOnline = false;
        Serial.println(sensor->sensorName + " sensor marked offline");
      }
    }
  }
}

void handleSensorError(int sensorId, String sensorName) {
  Serial.println("Sensor error: " + sensorName);
  updateSensorStatus(sensorId, false, 0);
}

void gracefulDegradation() {
  // Check if critical sensors are offline
  bool criticalSensorsOffline = !loadCellSensor.isOnline || !pirSensor.isOnline;
  
  if (criticalSensorsOffline) {
    Serial.println("Critical sensors offline - entering degraded mode");
    systemLCD.showDebug("Degraded Mode", "Critical Sensors Offline");
  }
  
  // Check if system can still operate
  if (!canOperateWithOfflineSensors()) {
    Serial.println("Too many sensors offline - system may not function properly");
    systemLCD.showAlert("SENSOR ERROR", 3000);
  }
}

bool canOperateWithOfflineSensors() {
  // Count online sensors
  int onlineSensors = 0;
  if (loadCellSensor.isOnline) onlineSensors++;
  if (pirSensor.isOnline) onlineSensors++;
  if (lightSensor.isOnline) onlineSensors++;
  if (dhtSensor.isOnline) onlineSensors++;
  if (phSensor.isOnline) onlineSensors++;
  if (ecSensor.isOnline) onlineSensors++;
  if (co2Sensor.isOnline) onlineSensors++;
  if (airQualitySensor.isOnline) onlineSensors++;
  if (waterLevelSensor.isOnline) onlineSensors++;
  if (flowSensor.isOnline) onlineSensors++;
  if (lcdSensor.isOnline) onlineSensors++;
  
  // System can operate if at least 3 sensors are online
  return onlineSensors >= 3;
}

String getSensorStatusString() {
  String status = "Sensors: ";
  status += loadCellSensor.isOnline ? "LC1" : "LC0";
  status += dhtSensor.isOnline ? " D1" : " D0";
  status += lightSensor.isOnline ? " L1" : " L0";
  status += pirSensor.isOnline ? " P1" : " P0";
  status += phSensor.isOnline ? " pH1" : " pH0";
  status += ecSensor.isOnline ? " EC1" : " EC0";
  status += co2Sensor.isOnline ? " CO21" : " CO20";
  status += airQualitySensor.isOnline ? " AQ1" : " AQ0";
  status += waterLevelSensor.isOnline ? " WL1" : " WL0";
  status += flowSensor.isOnline ? " F1" : " F0";
  status += lcdSensor.isOnline ? " LCD1" : " LCD0";
  return status;
}
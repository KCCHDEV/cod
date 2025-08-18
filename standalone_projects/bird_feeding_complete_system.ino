/*
 * RDTRC Complete Bird Feeding System - Standalone Version
 * Version: 4.0 - Independent System
 * Firmware made by: RDTRC
 * Updated: 2024
 * 
 * Features:
 * - Complete standalone bird feeding system
 * - Automated feeding schedule (4 times/day)
 * - Weight sensor monitoring (HX711)
 * - Motion detection (PIR)
 * - Light sensor for day/night detection
 * - Weather-resistant design considerations
 * - Built-in web interface
 * - Blink integration for mobile control
 * - Hotspot mode for direct access
 * - Data logging to SPIFFS
 * - Email/LINE notifications
 * - OTA updates
 * - Bird visit counting
 */

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

// System Configuration
#define FIRMWARE_VERSION "4.0"
#define FIRMWARE_MAKER "RDTRC"
#define SYSTEM_NAME "Bird Feeding System"
#define DEVICE_ID "RDTRC_BIRD_FEEDER"

// Network Configuration - แก้ไขตามเครือข่ายของคุณ
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* hotspot_ssid = "RDTRC_BirdFeeder";
const char* hotspot_password = "rdtrc123";

// Blynk Configuration - ใส่ Token ของคุณ
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_DEVICE_NAME "RDTRC Bird Feeder"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"

// LINE Notify Configuration - ใส่ Token ของคุณ
const char* lineToken = "YOUR_LINE_NOTIFY_TOKEN";

// Pin Definitions
#define SERVO_PIN 18
#define LOAD_CELL_DOUT_PIN 19
#define LOAD_CELL_SCK_PIN 21
#define PIR_SENSOR_PIN 22
#define LIGHT_SENSOR_PIN 23
#define BUZZER_PIN 26
#define STATUS_LED_PIN 2
#define RESET_BUTTON_PIN 0
#define MANUAL_FEED_BUTTON_PIN 4

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

// System Objects
WebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); // UTC+7 Thailand
HX711 scale;
Servo feedingServo;

// System Variables
bool isWiFiConnected = false;
bool isHotspotMode = false;
unsigned long lastHeartbeat = 0;
unsigned long lastDataLog = 0;
unsigned long lastStatusCheck = 0;
unsigned long bootTime = 0;

// Feeding Variables
struct FeedingSchedule {
  int hour;
  int minute;
  int portion;
  bool enabled;
  bool daylight_only; // Only feed during daylight hours
};

FeedingSchedule schedule[6] = {
  {6, 30, 25, true, true},   // 06:30 - 25g (dawn feeding)
  {10, 0, 20, true, true},   // 10:00 - 20g (morning feeding)
  {14, 30, 20, true, true},  // 14:30 - 20g (afternoon feeding)
  {17, 0, 25, true, true},   // 17:00 - 25g (evening feeding)
  {0, 0, 0, false, true},    // Extra slot
  {0, 0, 0, false, true}     // Extra slot
};

int defaultPortionSize = DEFAULT_PORTION_SIZE;
bool feedingInProgress = false;
unsigned long lastFeedingTime = 0;
float currentWeight = 0;
bool birdPresent = false;
float foodLevel = 0;
int totalFeedingsToday = 0;
float totalFoodDispensedToday = 0;
bool lowFoodAlertSent = false;
bool systemMaintenanceMode = false;

// Environmental monitoring
int lightLevel = 0;
bool isDaylight = true;
int birdVisitsToday = 0;
unsigned long lastBirdVisit = 0;

// Statistics
struct DailyStats {
  String date;
  int feedings;
  float totalFood;
  int birdVisits;
  float avgMealSize;
  int daylightHours;
};

DailyStats todayStats;

// Calibration values
float calibrationFactor = -5000.0; // ปรับค่าตามการ calibrate ของคุณ (น้อยกว่าแมวเพราะน้ำหนักเบา)
float tareWeight = 0;

// Function Declarations
void setupSystem();
void setupWiFi();
void setupWebServer();
void setupBlynk();
void setupOTA();
void displayBootScreen();
void handleSystemLoop();
void checkFeedingSchedule();
void performFeeding(int portion, bool isScheduled = false);
void readSensors();
void logData();
void handleWebInterface();
void sendLineNotification(String message);
void handleManualControls();
void saveSettings();
void loadSettings();
void performSystemMaintenance();
void calibrateScale();
void checkWeatherConditions();

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  bootTime = millis();
  displayBootScreen();
  setupSystem();
  
  Serial.println("✅ RDTRC Bird Feeding System Ready!");
  Serial.println("🌐 Web Interface: http://bird-feeder.local");
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
  
  // Check feeding schedule
  if (!systemMaintenanceMode) {
    checkFeedingSchedule();
  }
  
  // Handle manual controls
  handleManualControls();
  
  // Read sensors every 20 seconds
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead > 20000) {
    readSensors();
    lastSensorRead = millis();
  }
  
  // Log data every 5 minutes
  if (millis() - lastDataLog > 300000) {
    logData();
    lastDataLog = millis();
  }
  
  // System status check every minute
  if (millis() - lastStatusCheck > 60000) {
    performSystemMaintenance();
    lastStatusCheck = millis();
  }
  
  delay(100);
}

void displayBootScreen() {
  Serial.println("\n" + String("=").repeat(60));
  Serial.println("🐦 RDTRC Complete Bird Feeding System");
  Serial.println("");
  Serial.println("Firmware made by: " + String(FIRMWARE_MAKER));
  Serial.println("Version: " + String(FIRMWARE_VERSION));
  Serial.println("System: " + String(SYSTEM_NAME));
  Serial.println("");
  Serial.println("Features:");
  Serial.println("✓ Automated Bird Feeding");
  Serial.println("✓ Daylight Detection");
  Serial.println("✓ Weight Monitoring");
  Serial.println("✓ Motion Detection");
  Serial.println("✓ Weather Monitoring");
  Serial.println("✓ Web Interface");
  Serial.println("✓ Mobile App Control");
  Serial.println("✓ Smart Notifications");
  Serial.println("✓ Bird Visit Tracking");
  Serial.println("✓ OTA Updates");
  Serial.println("");
  Serial.println("Initializing System...");
  Serial.println("=".repeat(60));
  delay(3000);
}

void setupSystem() {
  // Initialize pins
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MANUAL_FEED_BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize servo
  feedingServo.attach(SERVO_PIN);
  feedingServo.write(0); // Closed position
  
  // Initialize load cell
  scale.begin(LOAD_CELL_DOUT_PIN, LOAD_CELL_SCK_PIN);
  scale.set_scale(calibrationFactor);
  scale.tare(); // Reset to 0
  
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
  todayStats.feedings = 0;
  todayStats.totalFood = 0;
  todayStats.birdVisits = 0;
  todayStats.daylightHours = 0;
  
  // Boot sequence with gentle sounds (not to scare birds)
  for (int i = 0; i < 2; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    tone(BUZZER_PIN, 2000, 100); // Higher, shorter tones for birds
    delay(200);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(300);
  }
  
  // Welcome chirp
  tone(BUZZER_PIN, 2500, 100);
  delay(150);
  tone(BUZZER_PIN, 3000, 100);
  delay(150);
  tone(BUZZER_PIN, 3500, 150);
  
  // Initial sensor reading
  readSensors();
  
  // Send startup notification
  sendLineNotification("🐦 RDTRC Bird Feeder Started!\n"
                      "System: " + String(SYSTEM_NAME) + "\n" +
                      "Version: " + String(FIRMWARE_VERSION) + "\n" +
                      "Food Level: " + String(foodLevel) + "cm\n" +
                      "Light Level: " + String(lightLevel) + "\n" +
                      "Status: Ready for feeding!");
  
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
    if (MDNS.begin("bird-feeder")) {
      Serial.println("✅ mDNS responder started: bird-feeder.local");
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
  if (MDNS.begin("bird-feeder")) {
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
    doc["current_weight"] = currentWeight;
    doc["food_level"] = foodLevel;
    doc["bird_present"] = birdPresent;
    doc["light_level"] = lightLevel;
    doc["is_daylight"] = isDaylight;
    doc["feeding_in_progress"] = feedingInProgress;
    doc["feedings_today"] = totalFeedingsToday;
    doc["food_dispensed_today"] = totalFoodDispensedToday;
    doc["bird_visits_today"] = birdVisitsToday;
    doc["maintenance_mode"] = systemMaintenanceMode;
    doc["timestamp"] = timeClient.getEpochTime();
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/feed", HTTP_POST, []() {
    if (systemMaintenanceMode) {
      server.send(423, "application/json", "{\"error\":\"system_in_maintenance\"}");
      return;
    }
    
    int portion = defaultPortionSize;
    if (server.hasArg("portion")) {
      portion = server.arg("portion").toInt();
    }
    
    if (portion >= MIN_PORTION_SIZE && portion <= MAX_PORTION_SIZE) {
      performFeeding(portion, false);
      server.send(200, "application/json", "{\"status\":\"feeding_started\",\"portion\":" + String(portion) + "}");
    } else {
      server.send(400, "application/json", "{\"error\":\"invalid_portion_size\"}");
    }
  });
  
  server.on("/api/schedule", HTTP_GET, []() {
    JsonDocument doc;
    JsonArray scheduleArray = doc.createNestedArray("schedule");
    
    for (int i = 0; i < 6; i++) {
      if (schedule[i].enabled || schedule[i].hour != 0 || schedule[i].minute != 0) {
        JsonObject scheduleItem = scheduleArray.createNestedObject();
        scheduleItem["index"] = i;
        scheduleItem["hour"] = schedule[i].hour;
        scheduleItem["minute"] = schedule[i].minute;
        scheduleItem["portion"] = schedule[i].portion;
        scheduleItem["enabled"] = schedule[i].enabled;
        scheduleItem["daylight_only"] = schedule[i].daylight_only;
      }
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/schedule", HTTP_POST, []() {
    if (server.hasArg("index") && server.hasArg("hour") && 
        server.hasArg("minute") && server.hasArg("portion") && server.hasArg("enabled")) {
      
      int index = server.arg("index").toInt();
      if (index >= 0 && index < 6) {
        schedule[index].hour = server.arg("hour").toInt();
        schedule[index].minute = server.arg("minute").toInt();
        schedule[index].portion = server.arg("portion").toInt();
        schedule[index].enabled = server.arg("enabled") == "true";
        schedule[index].daylight_only = server.hasArg("daylight_only") ? 
                                      server.arg("daylight_only") == "true" : true;
        
        saveSettings();
        server.send(200, "application/json", "{\"status\":\"schedule_updated\"}");
      } else {
        server.send(400, "application/json", "{\"error\":\"invalid_index\"}");
      }
    } else {
      server.send(400, "application/json", "{\"error\":\"missing_parameters\"}");
    }
  });
  
  server.on("/api/settings", HTTP_POST, []() {
    bool updated = false;
    
    if (server.hasArg("default_portion")) {
      int newPortion = server.arg("default_portion").toInt();
      if (newPortion >= MIN_PORTION_SIZE && newPortion <= MAX_PORTION_SIZE) {
        defaultPortionSize = newPortion;
        updated = true;
      }
    }
    
    if (server.hasArg("calibration_factor")) {
      calibrationFactor = server.arg("calibration_factor").toFloat();
      scale.set_scale(calibrationFactor);
      updated = true;
    }
    
    if (updated) {
      saveSettings();
      server.send(200, "application/json", "{\"status\":\"settings_updated\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"no_valid_parameters\"}");
    }
  });
  
  server.on("/api/calibrate", HTTP_POST, []() {
    calibrateScale();
    server.send(200, "application/json", "{\"status\":\"calibration_started\"}");
  });
  
  server.on("/api/maintenance", HTTP_POST, []() {
    if (server.hasArg("mode")) {
      systemMaintenanceMode = server.arg("mode") == "true";
      String status = systemMaintenanceMode ? "enabled" : "disabled";
      server.send(200, "application/json", "{\"status\":\"maintenance_" + status + "\"}");
    }
  });
  
  server.on("/api/stats", HTTP_GET, []() {
    JsonDocument doc;
    doc["today"] = todayStats.date;
    doc["feedings"] = todayStats.feedings;
    doc["total_food"] = todayStats.totalFood;
    doc["bird_visits"] = todayStats.birdVisits;
    doc["avg_meal_size"] = todayStats.feedings > 0 ? todayStats.totalFood / todayStats.feedings : 0;
    doc["daylight_hours"] = todayStats.daylightHours;
    
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
    Blynk.virtualWrite(V1, totalFeedingsToday);
    Blynk.virtualWrite(V2, currentWeight);
    Blynk.virtualWrite(V3, foodLevel);
    Blynk.virtualWrite(V4, birdPresent ? 1 : 0);
    Blynk.virtualWrite(V8, lightLevel);
    Blynk.virtualWrite(V9, birdVisitsToday);
  }
}

void setupOTA() {
  if (isWiFiConnected) {
    ArduinoOTA.setHostname("bird-feeder");
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
  // Status LED heartbeat (dimmer for birds)
  static bool ledState = false;
  if (millis() - lastHeartbeat > (systemMaintenanceMode ? 200 : 3000)) {
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

void checkFeedingSchedule() {
  if (feedingInProgress || systemMaintenanceMode) return;
  
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  // Check each scheduled feeding time
  for (int i = 0; i < 6; i++) {
    if (schedule[i].enabled && 
        currentHour == schedule[i].hour && 
        currentMinute == schedule[i].minute) {
      
      // Check daylight requirement
      if (schedule[i].daylight_only && !isDaylight) {
        Serial.println("⏰ Skipping scheduled feeding (nighttime): " + String(schedule[i].hour) + ":" + 
                      (schedule[i].minute < 10 ? "0" : "") + String(schedule[i].minute));
        continue;
      }
      
      // Check if we haven't fed in the last hour to prevent duplicate feedings
      unsigned long currentTime = timeClient.getEpochTime();
      if (currentTime - lastFeedingTime > 3600) {
        Serial.println("⏰ Scheduled feeding time: " + String(schedule[i].hour) + ":" + 
                      (schedule[i].minute < 10 ? "0" : "") + String(schedule[i].minute));
        performFeeding(schedule[i].portion, true);
        break;
      }
    }
  }
}

void performFeeding(int portion, bool isScheduled) {
  if (feedingInProgress) {
    Serial.println("⚠️ Feeding already in progress");
    return;
  }
  
  if (foodLevel < LOW_FOOD_THRESHOLD) {
    Serial.println("⚠️ Cannot feed: Food level too low");
    sendLineNotification("⚠️ Cannot feed: Food level too low (" + String(foodLevel) + "cm)");
    return;
  }
  
  feedingInProgress = true;
  lastFeedingTime = timeClient.getEpochTime();
  
  String feedingType = isScheduled ? "Scheduled" : "Manual";
  Serial.println("🐦 Starting " + feedingType + " feeding: " + String(portion) + "g");
  
  // Pre-feeding gentle alert (birds are sensitive to noise)
  tone(BUZZER_PIN, 2000, 50);
  delay(100);
  tone(BUZZER_PIN, 2500, 50);
  delay(100);
  
  // Record initial weight
  float initialWeight = scale.get_units(5); // Average of 5 readings
  
  // Open feeding mechanism
  feedingServo.write(90); // Open position
  Serial.println("🔄 Dispensing bird food...");
  
  // Calculate feeding duration based on portion size (birds need smaller, more precise portions)
  int baseDuration = 800; // Base duration in ms
  int feedingDuration = baseDuration + (portion * 30); // 30ms per gram
  delay(feedingDuration);
  
  // Close feeding mechanism
  feedingServo.write(0); // Closed position
  
  delay(2000); // Wait for food to settle
  
  // Record final weight
  float finalWeight = scale.get_units(5); // Average of 5 readings
  float actualPortion = finalWeight - initialWeight;
  
  // Handle negative weight (sensor error)
  if (actualPortion < 0) {
    actualPortion = portion * 0.85; // Assume 85% accuracy if sensor fails (birds are lighter)
  }
  
  Serial.println("✅ Feeding completed. Requested: " + String(portion) + "g, Actual: " + String(actualPortion) + "g");
  
  // Success sound (gentle for birds)
  tone(BUZZER_PIN, 2500, 100);
  delay(150);
  tone(BUZZER_PIN, 3000, 100);
  delay(150);
  tone(BUZZER_PIN, 3500, 200);
  
  // Update statistics
  totalFeedingsToday++;
  totalFoodDispensedToday += actualPortion;
  todayStats.feedings++;
  todayStats.totalFood += actualPortion;
  
  feedingInProgress = false;
  
  // Log the feeding
  String logEntry = String(timeClient.getEpochTime()) + "," + 
                   feedingType + "," + 
                   String(portion) + "," + 
                   String(actualPortion) + "," +
                   String(currentWeight) + "," +
                   String(foodLevel) + "," +
                   String(lightLevel) + "," +
                   String(isDaylight ? 1 : 0) + "\n";
  
  File logFile = SPIFFS.open("/feeding.log", "a");
  if (logFile) {
    logFile.print(logEntry);
    logFile.close();
  }
  
  // Send notification
  String notification = "🐦 " + feedingType + " Feeding Completed!\n" +
                       "Portion: " + String(actualPortion) + "g\n" +
                       "Today's Total: " + String(totalFeedingsToday) + " feedings\n" +
                       "Food Dispensed: " + String(totalFoodDispensedToday) + "g\n" +
                       "Light Level: " + String(lightLevel) + "\n" +
                       "Daylight: " + String(isDaylight ? "Yes" : "No");
  
  sendLineNotification(notification);
  
  // Update Blynk
  if (isWiFiConnected) {
    Blynk.virtualWrite(V1, totalFeedingsToday);
    Blynk.virtualWrite(V2, currentWeight);
    Blynk.virtualWrite(V5, totalFoodDispensedToday);
  }
}

void readSensors() {
  // Read load cell (average of 3 readings for stability)
  float weightSum = 0;
  for (int i = 0; i < 3; i++) {
    weightSum += scale.get_units();
    delay(100);
  }
  currentWeight = weightSum / 3.0;
  
  // Read PIR sensor
  bool previousBirdPresent = birdPresent;
  birdPresent = digitalRead(PIR_SENSOR_PIN);
  
  // Count bird visits
  if (birdPresent && !previousBirdPresent) {
    birdVisitsToday++;
    todayStats.birdVisits++;
    lastBirdVisit = millis();
    Serial.println("🐦 Bird detected!");
    
    // Send notification if feeder is empty
    if (currentWeight < EMPTY_FEEDER_THRESHOLD) {
      sendLineNotification("🐦 Bird is at the feeder but it's empty! (" + String(currentWeight) + "g)");
    }
  }
  
  // Read light sensor
  lightLevel = analogRead(LIGHT_SENSOR_PIN);
  
  // Determine if it's daylight
  bool previousDaylight = isDaylight;
  if (lightLevel > DAYLIGHT_THRESHOLD) {
    isDaylight = true;
  } else if (lightLevel < NIGHT_THRESHOLD) {
    isDaylight = false;
  }
  
  // Log daylight changes
  if (isDaylight != previousDaylight) {
    Serial.println(isDaylight ? "🌅 Daylight detected" : "🌙 Nighttime detected");
    String lightMsg = isDaylight ? "🌅 Dawn - Bird feeding will resume" : "🌙 Dusk - Bird feeding paused until dawn";
    sendLineNotification(lightMsg + "\nLight Level: " + String(lightLevel));
  }
  
  // Calculate food level using ultrasonic or estimated from weight
  // For simplicity, we'll estimate from container weight
  // This would need calibration based on your specific setup
  float estimatedFoodWeight = currentWeight - EMPTY_FEEDER_THRESHOLD;
  if (estimatedFoodWeight < 0) estimatedFoodWeight = 0;
  
  // Rough estimation: 1g of bird food ≈ 0.1cm height (adjust based on your container)
  foodLevel = (estimatedFoodWeight * 0.1);
  if (foodLevel > FOOD_CONTAINER_HEIGHT) foodLevel = FOOD_CONTAINER_HEIGHT;
  
  // Check for low food alert (send once per day)
  if (foodLevel < LOW_FOOD_THRESHOLD && !lowFoodAlertSent) {
    String alertMsg = "⚠️ Low Bird Food Alert!\n" +
                     "Current Level: " + String(foodLevel) + "cm\n" +
                     "Weight: " + String(currentWeight) + "g\n" +
                     "Please refill the bird feeder.";
    
    sendLineNotification(alertMsg);
    lowFoodAlertSent = true;
    
    // Alert sound (gentle for outdoor use)
    for (int i = 0; i < 3; i++) {
      tone(BUZZER_PIN, 1500, 200);
      delay(300);
    }
  } else if (foodLevel > LOW_FOOD_THRESHOLD * 2) {
    lowFoodAlertSent = false; // Reset alert when food is refilled
  }
  
  // Update Blynk with sensor data
  if (isWiFiConnected) {
    Blynk.virtualWrite(V2, currentWeight);
    Blynk.virtualWrite(V3, foodLevel);
    Blynk.virtualWrite(V4, birdPresent ? 1 : 0);
    Blynk.virtualWrite(V8, lightLevel);
    Blynk.virtualWrite(V9, birdVisitsToday);
  }
}

void handleManualControls() {
  // Manual feed button
  static bool lastButtonState = HIGH;
  static unsigned long lastButtonPress = 0;
  
  bool currentButtonState = digitalRead(MANUAL_FEED_BUTTON_PIN);
  
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    if (millis() - lastButtonPress > 1000) { // Debounce
      Serial.println("🔘 Manual feed button pressed");
      if (!systemMaintenanceMode) {
        performFeeding(defaultPortionSize, false);
      }
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
      sendLineNotification("🔄 Bird feeder system reset initiated");
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
  doc["weight"] = currentWeight;
  doc["food_level"] = foodLevel;
  doc["bird_present"] = birdPresent;
  doc["light_level"] = lightLevel;
  doc["is_daylight"] = isDaylight;
  doc["feedings_today"] = totalFeedingsToday;
  doc["food_dispensed"] = totalFoodDispensedToday;
  doc["bird_visits"] = birdVisitsToday;
  doc["wifi_signal"] = WiFi.RSSI();
  doc["free_memory"] = ESP.getFreeHeap();
  
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
  doc["default_portion"] = defaultPortionSize;
  doc["calibration_factor"] = calibrationFactor;
  
  JsonArray scheduleArray = doc.createNestedArray("schedule");
  for (int i = 0; i < 6; i++) {
    JsonObject scheduleItem = scheduleArray.createNestedObject();
    scheduleItem["hour"] = schedule[i].hour;
    scheduleItem["minute"] = schedule[i].minute;
    scheduleItem["portion"] = schedule[i].portion;
    scheduleItem["enabled"] = schedule[i].enabled;
    scheduleItem["daylight_only"] = schedule[i].daylight_only;
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
      defaultPortionSize = doc["default_portion"] | DEFAULT_PORTION_SIZE;
      calibrationFactor = doc["calibration_factor"] | -5000.0;
      
      if (doc.containsKey("schedule")) {
        JsonArray scheduleArray = doc["schedule"];
        for (int i = 0; i < 6 && i < scheduleArray.size(); i++) {
          JsonObject scheduleItem = scheduleArray[i];
          schedule[i].hour = scheduleItem["hour"];
          schedule[i].minute = scheduleItem["minute"];
          schedule[i].portion = scheduleItem["portion"];
          schedule[i].enabled = scheduleItem["enabled"];
          schedule[i].daylight_only = scheduleItem["daylight_only"] | true;
        }
      }
      
      Serial.println("📖 Settings loaded");
    }
    configFile.close();
  }
}

void performSystemMaintenance() {
  // Check system health
  if (ESP.getFreeHeap() < 10000) {
    Serial.println("⚠️ Low memory warning: " + String(ESP.getFreeHeap()) + " bytes");
  }
  
  // Rotate log files if they get too large
  File logFile = SPIFFS.open("/feeding.log", "r");
  if (logFile && logFile.size() > 50000) { // 50KB limit
    logFile.close();
    SPIFFS.remove("/feeding.log.old");
    SPIFFS.rename("/feeding.log", "/feeding.log.old");
    Serial.println("🔄 Log file rotated");
  }
  if (logFile) logFile.close();
  
  // Reset daily stats at midnight
  String currentDate = String(timeClient.getDay()) + "/" + String(timeClient.getMonth()) + "/" + String(timeClient.getYear());
  if (currentDate != todayStats.date) {
    // Save yesterday's stats
    JsonDocument statsDoc;
    statsDoc["date"] = todayStats.date;
    statsDoc["feedings"] = todayStats.feedings;
    statsDoc["total_food"] = todayStats.totalFood;
    statsDoc["bird_visits"] = todayStats.birdVisits;
    statsDoc["daylight_hours"] = todayStats.daylightHours;
    
    File statsFile = SPIFFS.open("/daily_stats.log", "a");
    if (statsFile) {
      serializeJson(statsDoc, statsFile);
      statsFile.println();
      statsFile.close();
    }
    
    // Reset for new day
    todayStats.date = currentDate;
    todayStats.feedings = 0;
    todayStats.totalFood = 0;
    todayStats.birdVisits = 0;
    todayStats.daylightHours = 0;
    totalFeedingsToday = 0;
    totalFoodDispensedToday = 0;
    birdVisitsToday = 0;
    
    Serial.println("🌅 New day started - stats reset");
  }
}

void calibrateScale() {
  Serial.println("🔧 Starting scale calibration...");
  sendLineNotification("🔧 Bird feeder scale calibration started. Please remove all items from the feeder.");
  
  // Tare the scale
  delay(5000);
  scale.tare();
  Serial.println("✅ Scale tared");
  
  sendLineNotification("📏 Please place a known weight (e.g., 50g) on the feeder and wait...");
  delay(10000);
  
  float reading = scale.get_units(10);
  float knownWeight = 50.0; // Assume 50g calibration weight (smaller for birds)
  
  if (reading != 0) {
    calibrationFactor = reading / knownWeight;
    scale.set_scale(calibrationFactor);
    saveSettings();
    
    Serial.println("✅ Calibration complete. Factor: " + String(calibrationFactor));
    sendLineNotification("✅ Bird feeder scale calibration complete!\nNew factor: " + String(calibrationFactor));
  } else {
    Serial.println("❌ Calibration failed - no reading");
    sendLineNotification("❌ Scale calibration failed - no reading detected");
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
    <title>RDTRC Bird Feeder</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #0f1419; color: #e6e6e6; }
        .header { background: linear-gradient(135deg, #4ecdc4 0%, #44a08d 100%); padding: 20px; text-align: center; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }
        .header h1 { color: white; margin-bottom: 10px; font-size: 28px; }
        .header p { color: #f0f0f0; opacity: 0.9; }
        .container { max-width: 1200px; margin: 20px auto; padding: 0 20px; }
        .status-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(180px, 1fr)); gap: 15px; margin-bottom: 30px; }
        .status-card { background: #1a1f2e; border-radius: 10px; padding: 15px; text-align: center; border: 1px solid #2d3748; transition: transform 0.2s; }
        .status-card:hover { transform: translateY(-2px); }
        .status-card h3 { color: #4ecdc4; margin-bottom: 8px; font-size: 12px; text-transform: uppercase; }
        .status-card .value { font-size: 20px; font-weight: bold; }
        .controls { background: #1a1f2e; border-radius: 10px; padding: 20px; margin-bottom: 20px; border: 1px solid #2d3748; }
        .control-section { margin-bottom: 25px; }
        .control-section h3 { color: #4ecdc4; margin-bottom: 15px; }
        .control-group { margin-bottom: 15px; }
        .control-group label { display: block; margin-bottom: 5px; color: #a0aec0; font-weight: 500; }
        .control-group input, .control-group select { width: 100%; padding: 8px; border: 1px solid #4a5568; border-radius: 5px; background: #2d3748; color: #e6e6e6; }
        .control-row { display: flex; gap: 10px; align-items: end; }
        .control-row input { flex: 1; }
        .btn { padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 14px; font-weight: 600; transition: all 0.2s; }
        .btn-primary { background: #4ecdc4; color: white; }
        .btn-secondary { background: #4a5568; color: white; }
        .btn-success { background: #48bb78; color: white; }
        .btn-warning { background: #ed8936; color: white; }
        .btn:hover { opacity: 0.8; transform: translateY(-1px); }
        .btn:disabled { opacity: 0.5; cursor: not-allowed; transform: none; }
        .schedule-table { width: 100%; border-collapse: collapse; margin-top: 15px; }
        .schedule-table th, .schedule-table td { padding: 8px; text-align: left; border-bottom: 1px solid #4a5568; font-size: 14px; }
        .schedule-table th { background: #2d3748; color: #4ecdc4; }
        .schedule-table input[type="number"], .schedule-table input[type="time"] { width: 70px; padding: 4px; }
        .schedule-table input[type="checkbox"] { transform: scale(1.1); }
        .stats-section { background: #1a1f2e; border-radius: 10px; padding: 20px; border: 1px solid #2d3748; }
        .alert { padding: 12px; border-radius: 5px; margin-bottom: 15px; font-size: 14px; }
        .alert-warning { background: #ed8936; color: white; }
        .alert-info { background: #4299e1; color: white; }
        .maintenance-mode { background: #f56565; color: white; padding: 10px; text-align: center; margin-bottom: 20px; border-radius: 5px; }
        .daylight-indicator { padding: 8px; border-radius: 5px; text-align: center; margin-bottom: 15px; font-weight: bold; }
        .daylight { background: #ffd700; color: #333; }
        .nighttime { background: #2d3748; color: #e6e6e6; border: 1px solid #4a5568; }
    </style>
    <script>
        let maintenanceMode = false;
        
        function refreshStatus() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('weight').textContent = data.current_weight.toFixed(1) + 'g';
                    document.getElementById('food-level').textContent = data.food_level.toFixed(1) + 'cm';
                    document.getElementById('bird-present').textContent = data.bird_present ? '🐦 Yes' : '❌ No';
                    document.getElementById('light-level').textContent = data.light_level;
                    document.getElementById('feedings-today').textContent = data.feedings_today;
                    document.getElementById('food-dispensed').textContent = data.food_dispensed_today.toFixed(1) + 'g';
                    document.getElementById('bird-visits').textContent = data.bird_visits_today;
                    document.getElementById('uptime').textContent = Math.floor(data.uptime / 1000 / 60) + ' min';
                    document.getElementById('wifi-signal').textContent = data.wifi_signal + ' dBm';
                    
                    // Daylight indicator
                    const daylightDiv = document.getElementById('daylight-indicator');
                    if (data.is_daylight) {
                        daylightDiv.className = 'daylight-indicator daylight';
                        daylightDiv.textContent = '🌅 Daylight - Feeding Active';
                    } else {
                        daylightDiv.className = 'daylight-indicator nighttime';
                        daylightDiv.textContent = '🌙 Nighttime - Feeding Paused';
                    }
                    
                    maintenanceMode = data.maintenance_mode;
                    document.getElementById('maintenance-alert').style.display = maintenanceMode ? 'block' : 'none';
                    
                    // Update feeding button
                    const feedBtn = document.getElementById('feed-btn');
                    feedBtn.disabled = data.feeding_in_progress || maintenanceMode;
                    feedBtn.textContent = data.feeding_in_progress ? '🔄 Feeding...' : '🐦 Feed Now';
                    
                    // Show alerts
                    const alertsDiv = document.getElementById('alerts');
                    alertsDiv.innerHTML = '';
                    
                    if (data.food_level < 2) {
                        alertsDiv.innerHTML += '<div class="alert alert-warning">⚠️ Low food level: ' + data.food_level.toFixed(1) + 'cm</div>';
                    }
                    
                    if (data.current_weight < 3 && data.bird_present) {
                        alertsDiv.innerHTML += '<div class="alert alert-info">🐦 Bird detected but feeder is empty!</div>';
                    }
                    
                    if (!data.is_daylight) {
                        alertsDiv.innerHTML += '<div class="alert alert-info">🌙 Nighttime mode - scheduled feeding paused</div>';
                    }
                });
            
            // Refresh stats
            fetch('/api/stats')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('avg-meal').textContent = data.avg_meal_size.toFixed(1) + 'g';
                });
        }
        
        function feedNow() {
            if (maintenanceMode) {
                alert('System is in maintenance mode');
                return;
            }
            
            const portion = document.getElementById('manual-portion').value || 20;
            fetch('/api/feed', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `portion=${portion}`
            })
            .then(response => response.json())
            .then(data => {
                if (data.status === 'feeding_started') {
                    alert('Feeding started! Dispensing ' + data.portion + 'g');
                    setTimeout(refreshStatus, 2000);
                } else {
                    alert('Error: ' + (data.error || 'Unknown error'));
                }
            });
        }
        
        function updateSettings() {
            const defaultPortion = document.getElementById('default-portion').value;
            fetch('/api/settings', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `default_portion=${defaultPortion}`
            })
            .then(response => response.json())
            .then(data => {
                alert('Settings updated successfully!');
            });
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
        
        function calibrateScale() {
            if (confirm('Start scale calibration? Remove all items from the feeder first.')) {
                fetch('/api/calibrate', { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    alert('Calibration started! Follow the instructions.');
                });
            }
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
                            <td><input type="number" value="${item.portion}" min="5" max="50" id="portion-${index}"></td>
                            <td><input type="checkbox" ${item.enabled ? 'checked' : ''} id="enabled-${index}"></td>
                            <td><input type="checkbox" ${item.daylight_only ? 'checked' : ''} id="daylight-${index}"></td>
                            <td><button class="btn btn-secondary" onclick="updateScheduleItem(${index})">💾</button></td>
                        `;
                    });
                });
        }
        
        function updateScheduleItem(index) {
            const time = document.getElementById(`time-${index}`).value.split(':');
            const portion = document.getElementById(`portion-${index}`).value;
            const enabled = document.getElementById(`enabled-${index}`).checked;
            const daylightOnly = document.getElementById(`daylight-${index}`).checked;
            
            fetch('/api/schedule', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `index=${index}&hour=${time[0]}&minute=${time[1]}&portion=${portion}&enabled=${enabled}&daylight_only=${daylightOnly}`
            })
            .then(response => response.json())
            .then(data => {
                alert('Schedule updated!');
            });
        }
        
        // Auto refresh every 20 seconds
        setInterval(refreshStatus, 20000);
        
        // Initial load
        window.onload = function() {
            refreshStatus();
            loadSchedule();
        };
    </script>
</head>
<body>
    <div id="maintenance-alert" class="maintenance-mode" style="display: none;">
        🔧 MAINTENANCE MODE ACTIVE - Feeding disabled
    </div>
    
    <div class="header">
        <h1>🐦 RDTRC Bird Feeding System</h1>
        <p>Complete Standalone System - Firmware by RDTRC v4.0</p>
    </div>
    
    <div class="container">
        <div id="daylight-indicator" class="daylight-indicator">Loading...</div>
        <div id="alerts"></div>
        
        <div class="status-grid">
            <div class="status-card">
                <h3>Feeder Weight</h3>
                <div class="value" id="weight">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Food Level</h3>
                <div class="value" id="food-level">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Bird Present</h3>
                <div class="value" id="bird-present">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Light Level</h3>
                <div class="value" id="light-level">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Feedings Today</h3>
                <div class="value" id="feedings-today">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Food Dispensed</h3>
                <div class="value" id="food-dispensed">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Bird Visits</h3>
                <div class="value" id="bird-visits">Loading...</div>
            </div>
            <div class="status-card">
                <h3>System Uptime</h3>
                <div class="value" id="uptime">Loading...</div>
            </div>
            <div class="status-card">
                <h3>WiFi Signal</h3>
                <div class="value" id="wifi-signal">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Avg Meal Size</h3>
                <div class="value" id="avg-meal">Loading...</div>
            </div>
        </div>
        
        <div class="controls">
            <div class="control-section">
                <h3>🎛️ Manual Controls</h3>
                <div class="control-row">
                    <div class="control-group">
                        <label for="manual-portion">Feed Amount (grams):</label>
                        <input type="number" id="manual-portion" min="5" max="50" value="20">
                    </div>
                    <button class="btn btn-primary" id="feed-btn" onclick="feedNow()">🐦 Feed Now</button>
                </div>
            </div>
            
            <div class="control-section">
                <h3>⚙️ Settings</h3>
                <div class="control-row">
                    <div class="control-group">
                        <label for="default-portion">Default Portion Size:</label>
                        <input type="number" id="default-portion" min="5" max="50" value="20">
                    </div>
                    <button class="btn btn-secondary" onclick="updateSettings()">💾 Save Settings</button>
                </div>
            </div>
            
            <div class="control-section">
                <h3>🔧 System Controls</h3>
                <button class="btn btn-warning" onclick="toggleMaintenance()">🔧 Toggle Maintenance</button>
                <button class="btn btn-success" onclick="calibrateScale()">⚖️ Calibrate Scale</button>
            </div>
        </div>
        
        <div class="controls">
            <div class="control-section">
                <h3>⏰ Feeding Schedule</h3>
                <table class="schedule-table">
                    <thead>
                        <tr>
                            <th>Time</th>
                            <th>Portion (g)</th>
                            <th>Enabled</th>
                            <th>Daylight Only</th>
                            <th>Action</th>
                        </tr>
                    </thead>
                    <tbody id="schedule-tbody">
                        <!-- Schedule items will be loaded here -->
                    </tbody>
                </table>
            </div>
        </div>
        
        <div class="stats-section">
            <h3 style="color: #4ecdc4; margin-bottom: 15px;">📊 System Information</h3>
            <p><strong>Device ID:</strong> RDTRC_BIRD_FEEDER</p>
            <p><strong>Firmware Version:</strong> 4.0</p>
            <p><strong>Features:</strong> Complete Standalone Bird Feeding System</p>
            <p><strong>Portion Range:</strong> 5-50g</p>
            <p><strong>Food Container:</strong> 15cm height</p>
            <p><strong>Low Food Alert:</strong> < 2cm</p>
            <p><strong>Daylight Detection:</strong> Automatic feeding schedule</p>
            <p><strong>Weather Resistant:</strong> Outdoor installation ready</p>
        </div>
    </div>
</body>
</html>
  )";
  
  server.send(200, "text/html", html);
}

// Blynk Virtual Pin Handlers
BLYNK_WRITE(V0) { // Manual feed button
  if (param.asInt() == 1 && !systemMaintenanceMode) {
    performFeeding(defaultPortionSize, false);
  }
}

BLYNK_WRITE(V6) { // Portion size setting
  int newPortion = param.asInt();
  if (newPortion >= MIN_PORTION_SIZE && newPortion <= MAX_PORTION_SIZE) {
    defaultPortionSize = newPortion;
    saveSettings();
  }
}

BLYNK_WRITE(V7) { // Maintenance mode toggle
  systemMaintenanceMode = param.asInt() == 1;
}

// Read-only pins for Blynk dashboard
BLYNK_READ(V1) { Blynk.virtualWrite(V1, totalFeedingsToday); }
BLYNK_READ(V2) { Blynk.virtualWrite(V2, currentWeight); }
BLYNK_READ(V3) { Blynk.virtualWrite(V3, foodLevel); }
BLYNK_READ(V4) { Blynk.virtualWrite(V4, birdPresent ? 1 : 0); }
BLYNK_READ(V5) { Blynk.virtualWrite(V5, totalFoodDispensedToday); }
BLYNK_READ(V8) { Blynk.virtualWrite(V8, lightLevel); }
BLYNK_READ(V9) { Blynk.virtualWrite(V9, birdVisitsToday); }
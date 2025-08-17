/*
 * ระบบให้อาหารนกอัตโนมัติ (Automatic Bird Feeding System)
 * 
 * Firmware made by: RDTRC
 * Version: 2.0
 * Created: 2024
 * 
 * คุณสมบัติ (Features):
 * - ให้อาหารและน้ำตามเวลาที่กำหนด (Scheduled feeding & watering)
 * - ตรวจสอบระดับอาหารและน้ำ (Food & water level monitoring)
 * - เซ็นเซอร์ตรวจจับนก (Bird detection sensor)
 * - ควบคุมผ่านแอพมือถือ (Mobile app control)
 * - แจ้งเตือนเมื่อต้องเติมอาหาร/น้ำ (Refill alerts)
 * - Boot screen แสดง "FW make by RDTRC"
 * - ระบบปั๊มน้ำอัตโนมัติ (Automatic water pump)
 */

// Include the common base system
#include "../../common/base_code/system_base.h"
#include <ESP32Servo.h>

// System-specific pin definitions
#define SERVO_FOOD_PIN 18
#define SERVO_WATER_PIN 19
#define PUMP_PIN 5
#define ULTRASONIC_TRIG_PIN 16
#define ULTRASONIC_ECHO_PIN 17
#define WATER_LEVEL_PIN 34  // ADC pin
#define PIR_PIN 4
#define BUTTON_MANUAL_FEED_PIN 32
#define BUTTON_MANUAL_WATER_PIN 33
#define BUTTON_RESET_PIN 35

// Bird feeding specific configuration
#define MAX_SCHEDULES 8
#define MIN_FOOD_DISTANCE 5.0    // cm (ultrasonic)
#define MIN_WATER_LEVEL 20       // ADC value (0-4095)
#define DEFAULT_FEED_TIME 2000   // ms
#define DEFAULT_WATER_TIME 3000  // ms
#define MAX_DAILY_FEEDS 20
#define BIRD_DETECTION_TIMEOUT 60000  // 1 minute

// Feeding/watering schedule structure
struct BirdSchedule {
  int hour;
  int minute;
  bool feedEnabled;
  bool waterEnabled;
  int feedDuration;    // milliseconds
  int waterDuration;   // milliseconds
};

// Bird system status
struct BirdSystemStatus {
  float foodDistance;
  int waterLevel;
  bool birdDetected;
  int dailyFeeds;
  int dailyWaters;
  String lastFeedTime;
  String lastWaterTime;
  String nextScheduleTime;
  bool pumpActive;
  bool foodLow;
  bool waterLow;
};

// Bird Feeding System Class
class BirdFeedingSystem : public SystemBase {
private:
  Servo foodServo;
  Servo waterServo;
  
  BirdSchedule schedules[MAX_SCHEDULES];
  BirdSystemStatus birdStatus;
  
  unsigned long lastScheduleCheck;
  unsigned long lastSensorCheck;
  unsigned long lastPIRCheck;
  unsigned long lastMotionTime;
  bool pirState;
  bool lastPirState;
  
public:
  BirdFeedingSystem() : SystemBase("Bird Feeder") {
    lastScheduleCheck = 0;
    lastSensorCheck = 0;
    lastPIRCheck = 0;
    lastMotionTime = 0;
    pirState = false;
    lastPirState = false;
    
    // Initialize default schedules
    schedules[0] = {6, 0, true, true, 2000, 3000};   // เช้า 06:00
    schedules[1] = {12, 0, true, false, 2000, 0};    // เที่ยง 12:00 (อาหารอย่างเดียว)
    schedules[2] = {18, 0, true, true, 2000, 3000};  // เย็น 18:00
    
    // Initialize remaining schedules as disabled
    for (int i = 3; i < MAX_SCHEDULES; i++) {
      schedules[i] = {0, 0, false, false, 0, 0};
    }
    
    // Initialize bird status
    birdStatus.foodDistance = 0;
    birdStatus.waterLevel = 0;
    birdStatus.birdDetected = false;
    birdStatus.dailyFeeds = 0;
    birdStatus.dailyWaters = 0;
    birdStatus.lastFeedTime = "";
    birdStatus.lastWaterTime = "";
    birdStatus.nextScheduleTime = "";
    birdStatus.pumpActive = false;
    birdStatus.foodLow = false;
    birdStatus.waterLow = false;
  }
  
  void setupSystem() override {
    LOG_INFO("Setting up Bird Feeding System");
    
    // Initialize servos
    foodServo.attach(SERVO_FOOD_PIN);
    waterServo.attach(SERVO_WATER_PIN);
    foodServo.write(0);  // Close food gate
    waterServo.write(0); // Close water valve
    
    // Initialize pump
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW);
    
    // Initialize ultrasonic sensor
    pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
    pinMode(ULTRASONIC_ECHO_PIN, INPUT);
    
    // Initialize water level sensor (ADC)
    pinMode(WATER_LEVEL_PIN, INPUT);
    
    // Initialize PIR sensor
    pinMode(PIR_PIN, INPUT);
    
    // Initialize buttons
    pinMode(BUTTON_MANUAL_FEED_PIN, INPUT_PULLUP);
    pinMode(BUTTON_MANUAL_WATER_PIN, INPUT_PULLUP);
    pinMode(BUTTON_RESET_PIN, INPUT_PULLUP);
    
    // Load bird feeding specific settings
    loadBirdSettings();
    
    // Calculate next schedule time
    calculateNextScheduleTime();
    
    // Setup web endpoints
    setupBirdWebServer();
    
    // Setup Blynk virtual pins
    setupBlynkPins();
    
    LOG_INFO("Bird Feeding System setup complete");
    
    // Show initial status on LCD
    showStatus("Bird Feeder v2.0", "FW by RDTRC");
    delay(2000);
  }
  
  void runSystem() override {
    // Check sensors every 5 seconds
    if (millis() - lastSensorCheck > 5000) {
      checkSensors();
      lastSensorCheck = millis();
    }
    
    // Check PIR sensor every 500ms
    if (millis() - lastPIRCheck > 500) {
      checkBirdDetection();
      lastPIRCheck = millis();
    }
    
    // Check schedule every minute
    if (millis() - lastScheduleCheck > 60000) {
      checkSchedule();
      lastScheduleCheck = millis();
    }
    
    // Check manual buttons
    checkManualButtons();
    
    // Update Blynk
    updateBlynkData();
  }
  
  String getSystemStatus() override {
    DynamicJsonDocument doc(512);
    doc["food"] = String(birdStatus.foodDistance, 1) + "cm";
    doc["water"] = String(birdStatus.waterLevel);
    doc["bird"] = birdStatus.birdDetected ? "Yes" : "No";
    doc["feeds"] = String(birdStatus.dailyFeeds);
    
    String output;
    serializeJson(doc, output);
    return output;
  }
  
  void handleSystemCommand(const String& command) override {
    if (command == "feed") {
      dispenseFeed(DEFAULT_FEED_TIME);
    } else if (command == "water") {
      dispenseWater(DEFAULT_WATER_TIME);
    } else if (command == "reset_daily") {
      resetDailyStats();
    } else if (command == "pump_on") {
      activatePump(5000); // 5 seconds
    }
  }

private:
  void checkSensors() {
    // Check food level using ultrasonic sensor
    checkFoodLevel();
    
    // Check water level using ADC
    checkWaterLevel();
    
    // Update status flags
    birdStatus.foodLow = (birdStatus.foodDistance < MIN_FOOD_DISTANCE);
    birdStatus.waterLow = (birdStatus.waterLevel < MIN_WATER_LEVEL);
    
    // Send alerts if needed
    if (birdStatus.foodLow) {
      LOG_ERROR("Food level low: " + String(birdStatus.foodDistance) + "cm");
      if (status.blynk_connected) {
        Blynk.notify("อาหารนกเหลือน้อย! ระยะ " + String(birdStatus.foodDistance, 1) + " cm");
      }
      setLEDColor(255, 255, 0); // Yellow warning
    }
    
    if (birdStatus.waterLow) {
      LOG_ERROR("Water level low: " + String(birdStatus.waterLevel));
      if (status.blynk_connected) {
        Blynk.notify("น้ำสำหรับนกเหลือน้อย! ระดับ " + String(birdStatus.waterLevel));
      }
      setLEDColor(255, 165, 0); // Orange warning
    }
  }
  
  void checkFoodLevel() {
    // Send ultrasonic pulse
    digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
    
    // Read echo
    long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH, 30000); // 30ms timeout
    
    if (duration > 0) {
      // Calculate distance in cm
      birdStatus.foodDistance = (duration * 0.034) / 2;
      
      // Limit reasonable range
      if (birdStatus.foodDistance > 50) birdStatus.foodDistance = 50;
      if (birdStatus.foodDistance < 0) birdStatus.foodDistance = 0;
    } else {
      // Sensor error or out of range
      LOG_ERROR("Ultrasonic sensor timeout");
    }
  }
  
  void checkWaterLevel() {
    // Read ADC value (0-4095 for 12-bit ADC)
    birdStatus.waterLevel = analogRead(WATER_LEVEL_PIN);
    
    // Apply simple filtering
    static int lastReading = 0;
    birdStatus.waterLevel = (birdStatus.waterLevel + lastReading) / 2;
    lastReading = birdStatus.waterLevel;
  }
  
  void checkBirdDetection() {
    pirState = digitalRead(PIR_PIN);
    
    if (pirState != lastPirState) {
      if (pirState == HIGH) {
        birdStatus.birdDetected = true;
        lastMotionTime = millis();
        
        LOG_INFO("Bird detected!");
        
        // Send Blynk notification
        if (status.blynk_connected) {
          Blynk.notify("ตรวจพบนกที่จุดให้อาหาร!");
        }
        
        // Blue LED for bird detection
        setLEDColor(0, 0, 255);
        
        // Gentle chirp sound
        playBeep(2000, 100);
        delay(50);
        playBeep(2500, 100);
      }
      lastPirState = pirState;
    }
    
    // Reset bird detection after timeout
    if (millis() - lastMotionTime > BIRD_DETECTION_TIMEOUT) {
      birdStatus.birdDetected = false;
    }
  }
  
  void checkSchedule() {
    if (!status.rtc_connected) return;
    
    DateTime now = getCurrentTime();
    
    for (int i = 0; i < MAX_SCHEDULES; i++) {
      if (schedules[i].feedEnabled || schedules[i].waterEnabled) {
        if (now.hour() == schedules[i].hour && now.minute() == schedules[i].minute) {
          LOG_INFO("Scheduled time reached: " + String(schedules[i].hour) + ":" + String(schedules[i].minute));
          
          // Feed if enabled
          if (schedules[i].feedEnabled && birdStatus.dailyFeeds < MAX_DAILY_FEEDS) {
            dispenseFeed(schedules[i].feedDuration);
          }
          
          // Water if enabled
          if (schedules[i].waterEnabled) {
            dispenseWater(schedules[i].waterDuration);
          }
          
          calculateNextScheduleTime();
          break;
        }
      }
    }
  }
  
  void checkManualButtons() {
    // Manual feed button
    if (digitalRead(BUTTON_MANUAL_FEED_PIN) == LOW) {
      delay(50); // debounce
      if (digitalRead(BUTTON_MANUAL_FEED_PIN) == LOW) {
        LOG_INFO("Manual feeding triggered");
        dispenseFeed(DEFAULT_FEED_TIME);
        
        while (digitalRead(BUTTON_MANUAL_FEED_PIN) == LOW) {
          delay(10);
        }
      }
    }
    
    // Manual water button
    if (digitalRead(BUTTON_MANUAL_WATER_PIN) == LOW) {
      delay(50); // debounce
      if (digitalRead(BUTTON_MANUAL_WATER_PIN) == LOW) {
        LOG_INFO("Manual watering triggered");
        dispenseWater(DEFAULT_WATER_TIME);
        
        while (digitalRead(BUTTON_MANUAL_WATER_PIN) == LOW) {
          delay(10);
        }
      }
    }
    
    // Reset button
    if (digitalRead(BUTTON_RESET_PIN) == LOW) {
      delay(50); // debounce
      if (digitalRead(BUTTON_RESET_PIN) == LOW) {
        LOG_INFO("Manual reset triggered");
        resetDailyStats();
        
        while (digitalRead(BUTTON_RESET_PIN) == LOW) {
          delay(10);
        }
      }
    }
  }
  
  void dispenseFeed(int duration) {
    if (birdStatus.dailyFeeds >= MAX_DAILY_FEEDS) {
      LOG_ERROR("Daily feeding limit reached");
      showStatus("Feed Limit", "Reached!");
      playErrorSound();
      return;
    }
    
    if (birdStatus.foodLow) {
      LOG_ERROR("Food level too low to feed");
      showStatus("Food Empty", "Refill needed");
      playErrorSound();
      return;
    }
    
    LOG_INFO("Dispensing food for " + String(duration) + "ms");
    
    // Show feeding status on LCD
    showStatus("Feeding Birds...", String(duration/1000) + "s");
    
    // Pre-feeding sound
    playBeep(1500, 200);
    delay(200);
    
    // Orange LED during feeding
    setLEDColor(255, 165, 0);
    
    // Open food gate
    foodServo.write(90);
    delay(duration);
    
    // Close food gate
    foodServo.write(0);
    
    // Update statistics
    birdStatus.dailyFeeds++;
    
    if (status.rtc_connected) {
      DateTime now = getCurrentTime();
      birdStatus.lastFeedTime = String(now.hour()) + ":" + 
                               (now.minute() < 10 ? "0" : "") + String(now.minute());
    }
    
    // Save settings
    saveBirdSettings();
    
    // Success sound
    playSuccessSound();
    
    // Send Blynk notification
    if (status.blynk_connected) {
      Blynk.notify("ให้อาหารนกแล้ว " + String(duration/1000) + " วินาที");
    }
    
    LOG_INFO("Feeding completed successfully");
    
    // Reset LED color
    setLEDColor(0, 255, 0);
  }
  
  void dispenseWater(int duration) {
    if (birdStatus.waterLow) {
      LOG_ERROR("Water level too low");
      showStatus("Water Empty", "Refill needed");
      playErrorSound();
      return;
    }
    
    LOG_INFO("Dispensing water for " + String(duration) + "ms");
    
    // Show watering status on LCD
    showStatus("Watering...", String(duration/1000) + "s");
    
    // Pre-watering sound
    playBeep(1000, 200);
    delay(200);
    
    // Blue LED during watering
    setLEDColor(0, 100, 255);
    
    // Option 1: Use servo to open water valve
    waterServo.write(90);
    delay(duration);
    waterServo.write(0);
    
    // Option 2: Use pump (uncomment if using pump instead of servo)
    // activatePump(duration);
    
    // Update statistics
    birdStatus.dailyWaters++;
    
    if (status.rtc_connected) {
      DateTime now = getCurrentTime();
      birdStatus.lastWaterTime = String(now.hour()) + ":" + 
                                (now.minute() < 10 ? "0" : "") + String(now.minute());
    }
    
    // Save settings
    saveBirdSettings();
    
    // Success sound
    playSuccessSound();
    
    // Send Blynk notification
    if (status.blynk_connected) {
      Blynk.notify("ให้น้ำนกแล้ว " + String(duration/1000) + " วินาที");
    }
    
    LOG_INFO("Watering completed successfully");
    
    // Reset LED color
    setLEDColor(0, 255, 0);
  }
  
  void activatePump(int duration) {
    birdStatus.pumpActive = true;
    digitalWrite(PUMP_PIN, HIGH);
    delay(duration);
    digitalWrite(PUMP_PIN, LOW);
    birdStatus.pumpActive = false;
  }
  
  void calculateNextScheduleTime() {
    if (!status.rtc_connected) {
      birdStatus.nextScheduleTime = "No RTC";
      return;
    }
    
    DateTime now = getCurrentTime();
    int currentMinutes = now.hour() * 60 + now.minute();
    int nextMinutes = 24 * 60; // Start with tomorrow
    
    // Find next enabled schedule
    for (int i = 0; i < MAX_SCHEDULES; i++) {
      if (schedules[i].feedEnabled || schedules[i].waterEnabled) {
        int scheduleMinutes = schedules[i].hour * 60 + schedules[i].minute;
        if (scheduleMinutes > currentMinutes && scheduleMinutes < nextMinutes) {
          nextMinutes = scheduleMinutes;
        }
      }
    }
    
    // If no schedule today, find first schedule tomorrow
    if (nextMinutes == 24 * 60) {
      for (int i = 0; i < MAX_SCHEDULES; i++) {
        if (schedules[i].feedEnabled || schedules[i].waterEnabled) {
          int scheduleMinutes = schedules[i].hour * 60 + schedules[i].minute;
          if (scheduleMinutes < nextMinutes) {
            nextMinutes = scheduleMinutes;
          }
        }
      }
    }
    
    int nextHour = (nextMinutes / 60) % 24;
    int nextMin = nextMinutes % 60;
    
    birdStatus.nextScheduleTime = (nextHour < 10 ? "0" : "") + String(nextHour) + ":" + 
                                 (nextMin < 10 ? "0" : "") + String(nextMin);
  }
  
  void setupBirdWebServer() {
    // Bird feeding specific endpoints
    server.on("/api/bird/status", HTTP_GET, [this]() {
      DynamicJsonDocument doc(1024);
      doc["foodDistance"] = birdStatus.foodDistance;
      doc["waterLevel"] = birdStatus.waterLevel;
      doc["birdDetected"] = birdStatus.birdDetected;
      doc["dailyFeeds"] = birdStatus.dailyFeeds;
      doc["dailyWaters"] = birdStatus.dailyWaters;
      doc["lastFeedTime"] = birdStatus.lastFeedTime;
      doc["lastWaterTime"] = birdStatus.lastWaterTime;
      doc["nextScheduleTime"] = birdStatus.nextScheduleTime;
      doc["foodLow"] = birdStatus.foodLow;
      doc["waterLow"] = birdStatus.waterLow;
      
      String output;
      serializeJson(doc, output);
      server.send(200, "application/json", output);
    });
    
    server.on("/api/bird/feed", HTTP_POST, [this]() {
      int duration = DEFAULT_FEED_TIME;
      
      if (server.hasArg("plain")) {
        DynamicJsonDocument doc(256);
        deserializeJson(doc, server.arg("plain"));
        if (doc.containsKey("duration")) {
          duration = doc["duration"];
        }
      }
      
      if (duration > 0 && duration <= 10000) {
        dispenseFeed(duration);
        server.send(200, "application/json", "{\"message\":\"Feeding successful\",\"success\":true}");
      } else {
        server.send(400, "application/json", "{\"message\":\"Invalid duration\",\"success\":false}");
      }
    });
    
    server.on("/api/bird/water", HTTP_POST, [this]() {
      int duration = DEFAULT_WATER_TIME;
      
      if (server.hasArg("plain")) {
        DynamicJsonDocument doc(256);
        deserializeJson(doc, server.arg("plain"));
        if (doc.containsKey("duration")) {
          duration = doc["duration"];
        }
      }
      
      if (duration > 0 && duration <= 10000) {
        dispenseWater(duration);
        server.send(200, "application/json", "{\"message\":\"Watering successful\",\"success\":true}");
      } else {
        server.send(400, "application/json", "{\"message\":\"Invalid duration\",\"success\":false}");
      }
    });
  }
  
  void setupBlynkPins() {
    // Virtual pin assignments:
    // V0 - Food distance display
    // V1 - Water level display
    // V2 - Daily feeds counter
    // V3 - Bird detection LED
    // V4 - System status LED
    // V10 - Manual feed button
    // V11 - Manual water button
    // V12 - System enable switch
  }
  
  void updateBlynkData() {
    static unsigned long lastBlynkUpdate = 0;
    
    if (status.blynk_connected && millis() - lastBlynkUpdate > 15000) {
      Blynk.virtualWrite(V0, birdStatus.foodDistance);
      Blynk.virtualWrite(V1, birdStatus.waterLevel);
      Blynk.virtualWrite(V2, birdStatus.dailyFeeds);
      Blynk.virtualWrite(V3, birdStatus.birdDetected ? 1 : 0);
      Blynk.virtualWrite(V4, status.state == SYSTEM_RUNNING ? 1 : 0);
      
      lastBlynkUpdate = millis();
    }
  }
  
  void loadBirdSettings() {
    // Load bird-specific settings from EEPROM
    EEPROM.get(400, schedules);
    EEPROM.get(500, birdStatus.dailyFeeds);
    EEPROM.get(504, birdStatus.dailyWaters);
    
    // Validate loaded data
    if (birdStatus.dailyFeeds < 0 || birdStatus.dailyFeeds > 1000) {
      birdStatus.dailyFeeds = 0;
    }
    if (birdStatus.dailyWaters < 0 || birdStatus.dailyWaters > 1000) {
      birdStatus.dailyWaters = 0;
    }
  }
  
  void saveBirdSettings() {
    EEPROM.put(400, schedules);
    EEPROM.put(500, birdStatus.dailyFeeds);
    EEPROM.put(504, birdStatus.dailyWaters);
    EEPROM.commit();
  }
  
  void resetDailyStats() {
    birdStatus.dailyFeeds = 0;
    birdStatus.dailyWaters = 0;
    birdStatus.lastFeedTime = "";
    birdStatus.lastWaterTime = "";
    saveBirdSettings();
    
    showStatus("Daily Stats", "Reset!");
    playSuccessSound();
    delay(2000);
    
    LOG_INFO("Daily statistics reset");
  }
};

// Global system instance
BirdFeedingSystem birdFeeder;

// Blynk Virtual Pin Handlers
BLYNK_WRITE(V10) { // Manual feed button
  if (param.asInt() == 1) {
    birdFeeder.handleSystemCommand("feed");
  }
}

BLYNK_WRITE(V11) { // Manual water button
  if (param.asInt() == 1) {
    birdFeeder.handleSystemCommand("water");
  }
}

BLYNK_WRITE(V12) { // System enable/disable
  config.system_enabled = param.asInt();
  birdFeeder.saveConfig();
}

// Arduino setup and loop
void setup() {
  // Initialize the bird feeding system
  if (!birdFeeder.begin()) {
    // System failed to initialize
    Serial.println("Bird Feeding System initialization failed!");
    while (true) {
      delay(1000);
    }
  }
}

void loop() {
  // Run the main system loop
  birdFeeder.loop();
}
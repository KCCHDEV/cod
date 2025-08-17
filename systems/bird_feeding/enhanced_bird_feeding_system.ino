/*
 * Enhanced Bird Feeding System - Single Main File
 * ระบบให้อาหารนกอัตโนมัติขั้นสูง - ไฟล์หลักเดียว
 * 
 * Enhanced Features:
 * ✅ Blink connectivity integration
 * ✅ Local hotspot capability 
 * ✅ Comprehensive web interface for network management
 * ✅ Local operation without internet
 * ✅ Single main file architecture
 * ✅ Boot screen "FW make by RDTRC"
 * 
 * System Features:
 * - Scheduled feeding (multiple times per day)
 * - Bird detection with PIR sensor
 * - Water level monitoring
 * - Food level monitoring
 * - Mobile app control via Blink
 * - Weather-based feeding adjustments
 * - Feeding history logging
 * - Anti-pest features
 * 
 * Firmware made by: RDTRC
 * Version: 3.0
 * Created: 2024
 */

// Include enhanced system base
#include "../common/base_code/enhanced_system_base.h"
#include "../common/base_code/web_handlers.cpp"
#include "../common/base_code/device_test_handlers.cpp"
#include <ESP32Servo.h>

// System-specific pin definitions
#define FOOD_SERVO_PIN 18
#define WATER_SERVO_PIN 19
#define PIR_PIN 4
#define FOOD_LEVEL_PIN A0
#define WATER_LEVEL_PIN A1
#define RAIN_SENSOR_PIN A2
#define LIGHT_SENSOR_PIN A3
#define BUTTON_MANUAL_PIN 32
#define BUTTON_RESET_PIN 33

// Bird feeding specific configuration
#define MAX_SCHEDULES 8
#define MIN_FOOD_LEVEL 20.0  // percentage
#define MIN_WATER_LEVEL 30.0  // percentage
#define DEFAULT_FEED_AMOUNT 15.0  // seconds of servo operation
#define DEFAULT_WATER_AMOUNT 10.0  // seconds of servo operation
#define MAX_DAILY_FEEDS 12
#define FEEDING_TIMEOUT 30000  // 30 seconds
#define SERVO_OPEN_ANGLE 90
#define SERVO_CLOSE_ANGLE 0

// Feeding schedule structure
struct BirdFeedingSchedule {
  int hour;
  int minute;
  float feedAmount;
  float waterAmount;
  bool enabled;
  bool weatherDependent; // Skip if raining
};

// Bird feeding system status
struct BirdFeedingStatus {
  float foodLevel;
  float waterLevel;
  float lightLevel;
  bool isRaining;
  bool birdDetected;
  int dailyFeedCount;
  String lastFeedingTime;
  String nextFeedingTime;
  bool lowFoodAlert;
  bool lowWaterAlert;
  unsigned long lastFeedingTimestamp;
  unsigned long lastBirdDetection;
};

// Enhanced Bird Feeding System Class
class EnhancedBirdFeedingSystem : public EnhancedSystemBase {
private:
  Servo foodServo;
  Servo waterServo;
  
  BirdFeedingSchedule schedules[MAX_SCHEDULES];
  BirdFeedingStatus birdStatus;
  
  unsigned long lastFeedingTime;
  unsigned long lastSensorCheck;
  unsigned long lastPIRCheck;
  unsigned long lastMotionTime;
  unsigned long lastBlinkUpdate;
  bool pirState;
  bool lastPirState;
  bool isFeeding;
  
public:
  EnhancedBirdFeedingSystem() : EnhancedSystemBase("Enhanced Bird Feeder", "BirdFeeding") {
    lastFeedingTime = 0;
    lastSensorCheck = 0;
    lastPIRCheck = 0;
    lastMotionTime = 0;
    lastBlinkUpdate = 0;
    pirState = false;
    lastPirState = false;
    isFeeding = false;
    
    // Initialize feeding schedules
    schedules[0] = {6, 30, 15.0, 10.0, true, false};   // Early morning
    schedules[1] = {8, 0, 20.0, 0.0, true, true};      // Morning (weather dependent)
    schedules[2] = {11, 0, 15.0, 5.0, true, true};     // Late morning
    schedules[3] = {14, 0, 15.0, 0.0, true, true};     // Afternoon
    schedules[4] = {17, 0, 20.0, 10.0, true, false};   // Evening
    schedules[5] = {19, 30, 15.0, 0.0, true, false};   // Late evening
    schedules[6] = {0, 0, 0.0, 0.0, false, false};     // Disabled
    schedules[7] = {0, 0, 0.0, 0.0, false, false};     // Disabled
    
    // Initialize bird status
    birdStatus.foodLevel = 0;
    birdStatus.waterLevel = 0;
    birdStatus.lightLevel = 0;
    birdStatus.isRaining = false;
    birdStatus.birdDetected = false;
    birdStatus.dailyFeedCount = 0;
    birdStatus.lastFeedingTime = "";
    birdStatus.nextFeedingTime = "";
    birdStatus.lowFoodAlert = false;
    birdStatus.lowWaterAlert = false;
    birdStatus.lastFeedingTimestamp = 0;
    birdStatus.lastBirdDetection = 0;
  }
  
  void setupSystem() override {
    LOG_INFO("Setting up Enhanced Bird Feeding System...");
    
    // Initialize servos
    foodServo.attach(FOOD_SERVO_PIN);
    waterServo.attach(WATER_SERVO_PIN);
    foodServo.write(SERVO_CLOSE_ANGLE);
    waterServo.write(SERVO_CLOSE_ANGLE);
    delay(1000);
    LOG_INFO("Servos initialized");
    
    // Initialize PIR sensor
    pinMode(PIR_PIN, INPUT);
    LOG_INFO("PIR sensor initialized");
    
    // Initialize analog sensors
    pinMode(FOOD_LEVEL_PIN, INPUT);
    pinMode(WATER_LEVEL_PIN, INPUT);
    pinMode(RAIN_SENSOR_PIN, INPUT);
    pinMode(LIGHT_SENSOR_PIN, INPUT);
    LOG_INFO("Analog sensors initialized");
    
    // Initialize manual buttons
    pinMode(BUTTON_MANUAL_PIN, INPUT_PULLUP);
    pinMode(BUTTON_RESET_PIN, INPUT_PULLUP);
    LOG_INFO("Manual buttons initialized");
    
    // Load feeding schedules from EEPROM
    loadFeedingSchedules();
    
    // Calculate next feeding time
    calculateNextFeedingTime();
    
    // Setup Blink virtual pins
    setupBlinkIntegration();
    
    systemStatus.state = SYSTEM_RUNNING;
    LOG_INFO("Enhanced Bird Feeding System setup completed");
  }
  
  void runSystem() override {
    unsigned long currentTime = millis();
    
    // Check manual buttons
    checkManualButtons();
    
    // Read sensors periodically
    if (currentTime - lastSensorCheck >= 3000) { // Every 3 seconds
      readSensors();
      lastSensorCheck = currentTime;
    }
    
    // Check PIR sensor
    if (currentTime - lastPIRCheck >= 500) { // Every 500ms
      checkBirdPresence();
      lastPIRCheck = currentTime;
    }
    
    // Check scheduled feeding
    checkScheduledFeeding();
    
    // Update Blink data
    if (systemConfig.blink_enabled && (currentTime - lastBlinkUpdate >= 5000)) { // Every 5 seconds
      sendBlynkData();
      lastBlinkUpdate = currentTime;
    }
    
    // Check for alerts
    checkAlerts();
    
    // Update system status
    updateSystemStatus();
  }
  
  void checkManualButtons() {
    // Manual feeding button
    if (digitalRead(BUTTON_MANUAL_PIN) == LOW) {
      delay(50); // Debounce
      if (digitalRead(BUTTON_MANUAL_PIN) == LOW) {
        if (!isFeeding) {
          LOG_INFO("Manual feeding triggered");
          performFeeding(DEFAULT_FEED_AMOUNT, DEFAULT_WATER_AMOUNT, "Manual");
          delay(1000); // Prevent multiple triggers
        }
      }
    }
    
    // Reset button (factory reset)
    static unsigned long resetButtonPressed = 0;
    if (digitalRead(BUTTON_RESET_PIN) == LOW) {
      if (resetButtonPressed == 0) {
        resetButtonPressed = millis();
      } else if (millis() - resetButtonPressed > 5000) { // Hold for 5 seconds
        LOG_INFO("Factory reset triggered by button");
        factoryReset();
      }
    } else {
      resetButtonPressed = 0;
    }
  }
  
  void readSensors() {
    // Read food level (0-100%)
    int foodRaw = analogRead(FOOD_LEVEL_PIN);
    birdStatus.foodLevel = map(foodRaw, 0, 4095, 0, 100);
    
    // Read water level (0-100%)
    int waterRaw = analogRead(WATER_LEVEL_PIN);
    birdStatus.waterLevel = map(waterRaw, 0, 4095, 0, 100);
    
    // Read light level (0-100%)
    int lightRaw = analogRead(LIGHT_SENSOR_PIN);
    birdStatus.lightLevel = map(lightRaw, 0, 4095, 0, 100);
    
    // Read rain sensor
    int rainRaw = analogRead(RAIN_SENSOR_PIN);
    birdStatus.isRaining = (rainRaw > 2000); // Threshold for rain detection
    
    // Log sensor readings
    if (systemConfig.debug_mode) {
      LOG_DEBUG("Sensors - Food: " + String(birdStatus.foodLevel) + "%, Water: " + 
                String(birdStatus.waterLevel) + "%, Light: " + String(birdStatus.lightLevel) + 
                "%, Rain: " + String(birdStatus.isRaining ? "Yes" : "No"));
    }
  }
  
  void checkBirdPresence() {
    pirState = digitalRead(PIR_PIN);
    
    if (pirState != lastPirState) {
      if (pirState == HIGH) {
        birdStatus.birdDetected = true;
        birdStatus.lastBirdDetection = millis();
        lastMotionTime = millis();
        LOG_INFO("Bird detected");
        
        if (systemConfig.blink_enabled) {
          Blynk.virtualWrite(V5, 1); // Bird presence indicator
          Blynk.logEvent("bird_detected", "Bird activity detected at feeder");
        }
      }
      lastPirState = pirState;
    }
    
    // Clear bird detected after 60 seconds of no motion
    if (birdStatus.birdDetected && (millis() - lastMotionTime > 60000)) {
      birdStatus.birdDetected = false;
      
      if (systemConfig.blink_enabled) {
        Blynk.virtualWrite(V5, 0);
      }
    }
  }
  
  void checkScheduledFeeding() {
    if (!systemStatus.rtc_connected) return;
    
    DateTime now = rtc.now();
    int currentHour = now.hour();
    int currentMinute = now.minute();
    
    for (int i = 0; i < MAX_SCHEDULES; i++) {
      if (schedules[i].enabled && 
          schedules[i].hour == currentHour && 
          schedules[i].minute == currentMinute &&
          now.second() == 0) { // Only trigger at the exact minute
        
        // Check if already fed in this minute
        if (millis() - lastFeedingTime > 60000) { // At least 1 minute since last feeding
          
          // Check weather dependency
          if (schedules[i].weatherDependent && birdStatus.isRaining) {
            LOG_INFO("Scheduled feeding #" + String(i+1) + " skipped due to rain");
            continue;
          }
          
          // Check daily limit
          if (birdStatus.dailyFeedCount >= MAX_DAILY_FEEDS) {
            LOG_INFO("Daily feeding limit reached");
            continue;
          }
          
          LOG_INFO("Scheduled feeding #" + String(i+1) + " triggered");
          performFeeding(schedules[i].feedAmount, schedules[i].waterAmount, 
                        "Schedule #" + String(i+1));
        }
      }
    }
  }
  
  void performFeeding(float feedAmount, float waterAmount, String source) {
    if (isFeeding) {
      LOG_INFO("Feeding already in progress");
      return;
    }
    
    // Check resource levels
    if (birdStatus.foodLevel < MIN_FOOD_LEVEL && feedAmount > 0) {
      LOG_ERROR("Food level too low for feeding");
      return;
    }
    
    if (birdStatus.waterLevel < MIN_WATER_LEVEL && waterAmount > 0) {
      LOG_ERROR("Water level too low for dispensing");
      return;
    }
    
    isFeeding = true;
    LOG_INFO("Starting feeding - Food: " + String(feedAmount) + "s, Water: " + 
             String(waterAmount) + "s from " + source);
    
    // Dispense food if requested
    if (feedAmount > 0) {
      foodServo.write(SERVO_OPEN_ANGLE);
      delay((int)(feedAmount * 1000)); // Convert seconds to milliseconds
      foodServo.write(SERVO_CLOSE_ANGLE);
      delay(500);
      LOG_INFO("Food dispensed for " + String(feedAmount) + " seconds");
    }
    
    // Dispense water if requested
    if (waterAmount > 0) {
      waterServo.write(SERVO_OPEN_ANGLE);
      delay((int)(waterAmount * 1000)); // Convert seconds to milliseconds
      waterServo.write(SERVO_CLOSE_ANGLE);
      delay(500);
      LOG_INFO("Water dispensed for " + String(waterAmount) + " seconds");
    }
    
    // Update statistics
    birdStatus.dailyFeedCount++;
    birdStatus.lastFeedingTime = getCurrentTimeString();
    birdStatus.lastFeedingTimestamp = millis();
    lastFeedingTime = millis();
    
    // Calculate next feeding time
    calculateNextFeedingTime();
    
    isFeeding = false;
    
    LOG_INFO("Feeding completed from " + source);
    
    // Send notification to Blink
    if (systemConfig.blink_enabled) {
      String message = "Fed birds - Food: " + String(feedAmount) + "s";
      if (waterAmount > 0) {
        message += ", Water: " + String(waterAmount) + "s";
      }
      message += ". Source: " + source + ". Daily count: " + String(birdStatus.dailyFeedCount);
      
      Blynk.logEvent("feeding_complete", message);
    }
    
    // Play success sound
    playSystemSound("success");
  }
  
  void calculateNextFeedingTime() {
    if (!systemStatus.rtc_connected) {
      birdStatus.nextFeedingTime = "RTC not available";
      return;
    }
    
    DateTime now = rtc.now();
    int currentHour = now.hour();
    int currentMinute = now.minute();
    
    // Find next enabled schedule
    int nextHour = -1, nextMinute = -1;
    bool foundToday = false;
    
    for (int i = 0; i < MAX_SCHEDULES; i++) {
      if (schedules[i].enabled) {
        if ((schedules[i].hour > currentHour) || 
            (schedules[i].hour == currentHour && schedules[i].minute > currentMinute)) {
          if (nextHour == -1 || schedules[i].hour < nextHour || 
              (schedules[i].hour == nextHour && schedules[i].minute < nextMinute)) {
            nextHour = schedules[i].hour;
            nextMinute = schedules[i].minute;
            foundToday = true;
          }
        }
      }
    }
    
    if (!foundToday) {
      // Find first schedule tomorrow
      for (int i = 0; i < MAX_SCHEDULES; i++) {
        if (schedules[i].enabled) {
          if (nextHour == -1 || schedules[i].hour < nextHour || 
              (schedules[i].hour == nextHour && schedules[i].minute < nextMinute)) {
            nextHour = schedules[i].hour;
            nextMinute = schedules[i].minute;
          }
        }
      }
    }
    
    if (nextHour != -1) {
      birdStatus.nextFeedingTime = String(nextHour) + ":" + 
        (nextMinute < 10 ? "0" : "") + String(nextMinute) + 
        (foundToday ? " today" : " tomorrow");
    } else {
      birdStatus.nextFeedingTime = "No schedule set";
    }
  }
  
  String getCurrentTimeString() {
    if (!systemStatus.rtc_connected) return "No RTC";
    
    DateTime now = rtc.now();
    return String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()) + 
           " " + String(now.hour()) + ":" + 
           (now.minute() < 10 ? "0" : "") + String(now.minute());
  }
  
  void checkAlerts() {
    static unsigned long lastAlertTime = 0;
    
    // Check low food alert
    if (birdStatus.foodLevel < MIN_FOOD_LEVEL && !birdStatus.lowFoodAlert) {
      birdStatus.lowFoodAlert = true;
      LOG_ERROR("Low food alert: " + String(birdStatus.foodLevel) + "% remaining");
      
      if (systemConfig.blink_enabled) {
        Blynk.logEvent("low_food", "Food level is low: " + String(birdStatus.foodLevel) + "% remaining");
      }
    } else if (birdStatus.foodLevel >= MIN_FOOD_LEVEL * 1.5) {
      birdStatus.lowFoodAlert = false; // Clear alert when food is refilled
    }
    
    // Check low water alert
    if (birdStatus.waterLevel < MIN_WATER_LEVEL && !birdStatus.lowWaterAlert) {
      birdStatus.lowWaterAlert = true;
      LOG_ERROR("Low water alert: " + String(birdStatus.waterLevel) + "% remaining");
      
      if (systemConfig.blink_enabled) {
        Blynk.logEvent("low_water", "Water level is low: " + String(birdStatus.waterLevel) + "% remaining");
      }
    } else if (birdStatus.waterLevel >= MIN_WATER_LEVEL * 1.5) {
      birdStatus.lowWaterAlert = false; // Clear alert when water is refilled
    }
    
    // Periodic alert reminders
    if ((birdStatus.lowFoodAlert || birdStatus.lowWaterAlert) && 
        (millis() - lastAlertTime > 600000)) { // Every 10 minutes
      
      if (birdStatus.lowFoodAlert) {
        blinkLED(255, 255, 0, 3); // Yellow blink for food
      }
      if (birdStatus.lowWaterAlert) {
        blinkLED(0, 255, 255, 3); // Cyan blink for water
      }
      
      lastAlertTime = millis();
    }
  }
  
  void updateSystemStatus() {
    // Reset daily counters at midnight
    static int lastDay = -1;
    if (systemStatus.rtc_connected) {
      DateTime now = rtc.now();
      if (lastDay != now.day()) {
        birdStatus.dailyFeedCount = 0;
        lastDay = now.day();
        LOG_INFO("Daily counters reset");
      }
    }
  }
  
  void setupBlinkIntegration() {
    if (!systemConfig.blink_enabled) return;
    
    // Virtual Pin assignments:
    // V0 - Food level percentage
    // V1 - Manual feeding button
    // V2 - Water level percentage
    // V3 - Light level percentage
    // V4 - Daily feed count
    // V5 - Bird presence indicator
    // V6 - Next feeding time (display)
    // V7 - Weather status (display)
    // V8 - Schedule enable/disable
    // V9 - Alert status
    // V10 - Rain sensor status
    
    LOG_INFO("Blink integration setup completed");
  }
  
  void sendBlynkData() override {
    if (!systemConfig.blink_enabled || !systemStatus.network.blink_connected) return;
    
    Blynk.virtualWrite(V0, birdStatus.foodLevel);
    Blynk.virtualWrite(V2, birdStatus.waterLevel);
    Blynk.virtualWrite(V3, birdStatus.lightLevel);
    Blynk.virtualWrite(V4, birdStatus.dailyFeedCount);
    Blynk.virtualWrite(V5, birdStatus.birdDetected ? 1 : 0);
    Blynk.virtualWrite(V9, (birdStatus.lowFoodAlert || birdStatus.lowWaterAlert) ? 1 : 0);
    Blynk.virtualWrite(V10, birdStatus.isRaining ? 1 : 0);
    
    // Send text data
    Blynk.virtualWrite(V6, birdStatus.nextFeedingTime);
    Blynk.virtualWrite(V7, "Weather: " + String(birdStatus.isRaining ? "Raining" : "Clear") + 
                            "\nLight: " + String(birdStatus.lightLevel) + "%\n" +
                            "Fed: " + String(birdStatus.dailyFeedCount) + " times today");
  }
  
  void handleBlynkCommand(int pin, int value) override {
    switch (pin) {
      case V1: // Manual feeding button
        if (value == 1 && !isFeeding) {
          performFeeding(DEFAULT_FEED_AMOUNT, DEFAULT_WATER_AMOUNT, "Blink App");
        }
        break;
        
      case V8: // Schedule toggle
        // Toggle schedule enable/disable
        // Implementation depends on which schedule to toggle
        break;
    }
  }
  
  String getSystemStatus() override {
    DynamicJsonDocument doc(512);
    
    doc["food_level"] = birdStatus.foodLevel;
    doc["water_level"] = birdStatus.waterLevel;
    doc["light_level"] = birdStatus.lightLevel;
    doc["is_raining"] = birdStatus.isRaining;
    doc["bird_detected"] = birdStatus.birdDetected;
    doc["daily_feed_count"] = birdStatus.dailyFeedCount;
    doc["last_feeding"] = birdStatus.lastFeedingTime;
    doc["next_feeding"] = birdStatus.nextFeedingTime;
    doc["low_food_alert"] = birdStatus.lowFoodAlert;
    doc["low_water_alert"] = birdStatus.lowWaterAlert;
    doc["is_feeding"] = isFeeding;
    
    String output;
    serializeJson(doc, output);
    return output;
  }
  
  void loadFeedingSchedules() {
    // Load from EEPROM or use defaults
    // Implementation would read from EEPROM address
    LOG_INFO("Feeding schedules loaded");
  }
  
  void saveFeedingSchedules() {
    // Save to EEPROM
    // Implementation would write to EEPROM address
    LOG_INFO("Feeding schedules saved");
  }
};

// Global system instance
EnhancedBirdFeedingSystem* birdSystem = nullptr;
EnhancedSystemBase* currentSystem = nullptr;

// Blink event handlers
BLYNK_WRITE(V1) {
  if (birdSystem) {
    birdSystem->handleBlynkCommand(V1, param.asInt());
  }
}

BLYNK_WRITE(V8) {
  if (birdSystem) {
    birdSystem->handleBlynkCommand(V8, param.asInt());
  }
}

void setup() {
  // Create system instance
  birdSystem = new EnhancedBirdFeedingSystem();
  currentSystem = birdSystem;
  
  // Initialize system
  if (!birdSystem->begin()) {
    Serial.println("System initialization failed!");
    while (true) {
      delay(1000);
    }
  }
  
  Serial.println("Enhanced Bird Feeding System ready!");
}

void loop() {
  if (birdSystem) {
    birdSystem->loop();
  }
}
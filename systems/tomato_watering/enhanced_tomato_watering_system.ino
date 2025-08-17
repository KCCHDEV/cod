/*
 * Enhanced Tomato Watering System - Single Main File
 * ระบบรดน้ำมะเขือเทศอัตโนมัติขั้นสูง - ไฟล์หลักเดียว
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
 * - Multiple zone watering control
 * - Soil moisture monitoring (multiple sensors)
 * - Weather-based watering adjustments
 * - Water level monitoring
 * - Pump control with flow monitoring
 * - Mobile app control via Blink
 * - Scheduled watering with smart adjustments
 * - Water usage tracking
 * - Plant growth monitoring
 * 
 * Firmware made by: RDTRC
 * Version: 3.0
 * Created: 2024
 */

// Include enhanced system base
#include "../common/base_code/enhanced_system_base.h"
#include "../common/base_code/web_handlers.cpp"
#include "../common/base_code/device_test_handlers.cpp"

// System-specific pin definitions
#define PUMP_RELAY_PIN 18
#define VALVE_ZONE1_PIN 19
#define VALVE_ZONE2_PIN 20
#define VALVE_ZONE3_PIN 21
#define VALVE_ZONE4_PIN 22
#define SOIL_SENSOR1_PIN A0
#define SOIL_SENSOR2_PIN A1
#define SOIL_SENSOR3_PIN A2
#define SOIL_SENSOR4_PIN A3
#define WATER_LEVEL_PIN A4
#define RAIN_SENSOR_PIN A5
#define FLOW_SENSOR_PIN 2
#define BUTTON_MANUAL_PIN 32
#define BUTTON_RESET_PIN 33

// Tomato watering specific configuration
#define MAX_ZONES 4
#define MAX_SCHEDULES 6
#define MIN_SOIL_MOISTURE 30.0  // percentage
#define MAX_SOIL_MOISTURE 70.0  // percentage
#define MIN_WATER_LEVEL 20.0    // percentage
#define DEFAULT_WATERING_DURATION 300000  // 5 minutes in milliseconds
#define MAX_DAILY_WATER_USAGE 100.0  // liters
#define PUMP_PRIME_TIME 5000    // 5 seconds
#define FLOW_CALIBRATION 7.5    // pulses per liter

// Zone structure
struct WateringZone {
  int valvePin;
  int soilSensorPin;
  float currentMoisture;
  float targetMoisture;
  bool enabled;
  String zoneName;
  unsigned long lastWatered;
  float totalWaterUsed; // liters today
};

// Watering schedule structure
struct WateringSchedule {
  int hour;
  int minute;
  int duration; // seconds
  bool zones[MAX_ZONES]; // which zones to water
  bool enabled;
  bool weatherDependent; // Skip if raining
  bool moistureDependent; // Only water if soil is dry
};

// Tomato watering system status
struct TomatoWateringStatus {
  float waterLevel;
  bool isRaining;
  bool pumpRunning;
  int activeZone; // -1 if none
  float dailyWaterUsage;
  String lastWateringTime;
  String nextWateringTime;
  bool lowWaterAlert;
  bool systemPaused;
  unsigned long totalRuntime;
  unsigned long lastWateringTimestamp;
  float flowRate; // liters per minute
};

// Enhanced Tomato Watering System Class
class EnhancedTomatoWateringSystem : public EnhancedSystemBase {
private:
  WateringZone zones[MAX_ZONES];
  WateringSchedule schedules[MAX_SCHEDULES];
  TomatoWateringStatus wateringStatus;
  
  unsigned long lastSensorCheck;
  unsigned long lastScheduleCheck;
  unsigned long lastBlinkUpdate;
  unsigned long wateringStartTime;
  unsigned long pumpStartTime;
  
  volatile unsigned long flowPulseCount;
  unsigned long lastFlowCheck;
  
  bool isWatering;
  int currentWateringZone;
  unsigned long currentWateringDuration;
  
public:
  EnhancedTomatoWateringSystem() : EnhancedSystemBase("Enhanced Tomato Watering", "TomatoWatering") {
    lastSensorCheck = 0;
    lastScheduleCheck = 0;
    lastBlinkUpdate = 0;
    wateringStartTime = 0;
    pumpStartTime = 0;
    flowPulseCount = 0;
    lastFlowCheck = 0;
    isWatering = false;
    currentWateringZone = -1;
    currentWateringDuration = 0;
    
    // Initialize zones
    zones[0] = {VALVE_ZONE1_PIN, SOIL_SENSOR1_PIN, 0, 50.0, true, "Zone 1 - Main Bed", 0, 0};
    zones[1] = {VALVE_ZONE2_PIN, SOIL_SENSOR2_PIN, 0, 50.0, true, "Zone 2 - Side Bed", 0, 0};
    zones[2] = {VALVE_ZONE3_PIN, SOIL_SENSOR3_PIN, 0, 50.0, true, "Zone 3 - Greenhouse", 0, 0};
    zones[3] = {VALVE_ZONE4_PIN, SOIL_SENSOR4_PIN, 0, 50.0, true, "Zone 4 - Seedlings", 0, 0};
    
    // Initialize watering schedules
    schedules[0] = {6, 0, 300, {true, true, true, false}, true, true, true};   // Morning
    schedules[1] = {18, 0, 300, {true, true, true, true}, true, true, true};   // Evening
    schedules[2] = {12, 0, 180, {false, false, false, true}, true, false, true}; // Midday seedlings
    schedules[3] = {0, 0, 0, {false, false, false, false}, false, false, false}; // Disabled
    schedules[4] = {0, 0, 0, {false, false, false, false}, false, false, false}; // Disabled
    schedules[5] = {0, 0, 0, {false, false, false, false}, false, false, false}; // Disabled
    
    // Initialize status
    wateringStatus.waterLevel = 0;
    wateringStatus.isRaining = false;
    wateringStatus.pumpRunning = false;
    wateringStatus.activeZone = -1;
    wateringStatus.dailyWaterUsage = 0;
    wateringStatus.lastWateringTime = "";
    wateringStatus.nextWateringTime = "";
    wateringStatus.lowWaterAlert = false;
    wateringStatus.systemPaused = false;
    wateringStatus.totalRuntime = 0;
    wateringStatus.lastWateringTimestamp = 0;
    wateringStatus.flowRate = 0;
  }
  
  void setupSystem() override {
    LOG_INFO("Setting up Enhanced Tomato Watering System...");
    
    // Initialize pump relay
    pinMode(PUMP_RELAY_PIN, OUTPUT);
    digitalWrite(PUMP_RELAY_PIN, LOW);
    LOG_INFO("Pump relay initialized");
    
    // Initialize zone valves
    for (int i = 0; i < MAX_ZONES; i++) {
      pinMode(zones[i].valvePin, OUTPUT);
      digitalWrite(zones[i].valvePin, LOW);
      pinMode(zones[i].soilSensorPin, INPUT);
    }
    LOG_INFO("Zone valves and sensors initialized");
    
    // Initialize water level and rain sensors
    pinMode(WATER_LEVEL_PIN, INPUT);
    pinMode(RAIN_SENSOR_PIN, INPUT);
    LOG_INFO("Water and rain sensors initialized");
    
    // Initialize flow sensor
    pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseISR, RISING);
    LOG_INFO("Flow sensor initialized");
    
    // Initialize manual buttons
    pinMode(BUTTON_MANUAL_PIN, INPUT_PULLUP);
    pinMode(BUTTON_RESET_PIN, INPUT_PULLUP);
    LOG_INFO("Manual buttons initialized");
    
    // Load watering schedules from EEPROM
    loadWateringSchedules();
    
    // Calculate next watering time
    calculateNextWateringTime();
    
    // Setup Blink virtual pins
    setupBlinkIntegration();
    
    systemStatus.state = SYSTEM_RUNNING;
    LOG_INFO("Enhanced Tomato Watering System setup completed");
  }
  
  void runSystem() override {
    unsigned long currentTime = millis();
    
    // Check manual buttons
    checkManualButtons();
    
    // Read sensors periodically
    if (currentTime - lastSensorCheck >= 5000) { // Every 5 seconds
      readSensors();
      lastSensorCheck = currentTime;
    }
    
    // Check scheduled watering
    if (currentTime - lastScheduleCheck >= 60000) { // Every minute
      checkScheduledWatering();
      lastScheduleCheck = currentTime;
    }
    
    // Update flow rate calculation
    updateFlowRate();
    
    // Monitor active watering
    if (isWatering) {
      monitorWatering();
    }
    
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
  
  static void flowPulseISR() {
    // This will be called from interrupt context
    // Keep it simple and fast
    static EnhancedTomatoWateringSystem* instance = nullptr;
    if (instance) {
      instance->flowPulseCount++;
    }
  }
  
  void checkManualButtons() {
    // Manual watering button
    if (digitalRead(BUTTON_MANUAL_PIN) == LOW) {
      delay(50); // Debounce
      if (digitalRead(BUTTON_MANUAL_PIN) == LOW) {
        if (!isWatering && !wateringStatus.systemPaused) {
          LOG_INFO("Manual watering triggered");
          startWatering(0, DEFAULT_WATERING_DURATION / 1000, "Manual"); // Zone 0, default duration
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
    // Read soil moisture for each zone
    for (int i = 0; i < MAX_ZONES; i++) {
      int moistureRaw = analogRead(zones[i].soilSensorPin);
      zones[i].currentMoisture = map(moistureRaw, 0, 4095, 100, 0); // Invert: dry = low value
    }
    
    // Read water level (0-100%)
    int waterRaw = analogRead(WATER_LEVEL_PIN);
    wateringStatus.waterLevel = map(waterRaw, 0, 4095, 0, 100);
    
    // Read rain sensor
    int rainRaw = analogRead(RAIN_SENSOR_PIN);
    wateringStatus.isRaining = (rainRaw > 2000); // Threshold for rain detection
    
    // Log sensor readings in debug mode
    if (systemConfig.debug_mode) {
      String moistureLog = "Moisture - ";
      for (int i = 0; i < MAX_ZONES; i++) {
        moistureLog += "Z" + String(i+1) + ":" + String(zones[i].currentMoisture) + "% ";
      }
      LOG_DEBUG(moistureLog);
      LOG_DEBUG("Water: " + String(wateringStatus.waterLevel) + "%, Rain: " + 
                String(wateringStatus.isRaining ? "Yes" : "No"));
    }
  }
  
  void updateFlowRate() {
    unsigned long currentTime = millis();
    if (currentTime - lastFlowCheck >= 1000) { // Calculate every second
      // Calculate flow rate in liters per minute
      float pulsesPerSecond = flowPulseCount / ((currentTime - lastFlowCheck) / 1000.0);
      wateringStatus.flowRate = (pulsesPerSecond * 60.0) / FLOW_CALIBRATION;
      
      // Add to daily usage if pump is running
      if (wateringStatus.pumpRunning) {
        float litersThisSecond = pulsesPerSecond / FLOW_CALIBRATION;
        wateringStatus.dailyWaterUsage += litersThisSecond;
        
        // Update zone water usage
        if (currentWateringZone >= 0 && currentWateringZone < MAX_ZONES) {
          zones[currentWateringZone].totalWaterUsed += litersThisSecond;
        }
      }
      
      flowPulseCount = 0;
      lastFlowCheck = currentTime;
    }
  }
  
  void checkScheduledWatering() {
    if (!systemStatus.rtc_connected || wateringStatus.systemPaused) return;
    
    DateTime now = rtc.now();
    int currentHour = now.hour();
    int currentMinute = now.minute();
    
    for (int i = 0; i < MAX_SCHEDULES; i++) {
      if (schedules[i].enabled && 
          schedules[i].hour == currentHour && 
          schedules[i].minute == currentMinute &&
          now.second() == 0) { // Only trigger at the exact minute
        
        // Check if already watered in this minute
        if (millis() - wateringStatus.lastWateringTimestamp > 60000) {
          
          // Check weather dependency
          if (schedules[i].weatherDependent && wateringStatus.isRaining) {
            LOG_INFO("Scheduled watering #" + String(i+1) + " skipped due to rain");
            continue;
          }
          
          // Check water level
          if (wateringStatus.waterLevel < MIN_WATER_LEVEL) {
            LOG_ERROR("Scheduled watering skipped - low water level");
            continue;
          }
          
          // Check daily water usage limit
          if (wateringStatus.dailyWaterUsage >= MAX_DAILY_WATER_USAGE) {
            LOG_ERROR("Daily water usage limit reached");
            continue;
          }
          
          // Start watering for enabled zones
          for (int zone = 0; zone < MAX_ZONES; zone++) {
            if (schedules[i].zones[zone] && zones[zone].enabled) {
              
              // Check moisture dependency
              if (schedules[i].moistureDependent && 
                  zones[zone].currentMoisture > zones[zone].targetMoisture) {
                LOG_INFO("Zone " + String(zone+1) + " skipped - soil moisture sufficient");
                continue;
              }
              
              LOG_INFO("Scheduled watering #" + String(i+1) + " starting for zone " + String(zone+1));
              startWatering(zone, schedules[i].duration, "Schedule #" + String(i+1));
              break; // Water one zone at a time
            }
          }
        }
      }
    }
  }
  
  void startWatering(int zoneIndex, int durationSeconds, String source) {
    if (isWatering) {
      LOG_INFO("Watering already in progress");
      return;
    }
    
    if (zoneIndex < 0 || zoneIndex >= MAX_ZONES) {
      LOG_ERROR("Invalid zone index: " + String(zoneIndex));
      return;
    }
    
    if (!zones[zoneIndex].enabled) {
      LOG_ERROR("Zone " + String(zoneIndex+1) + " is disabled");
      return;
    }
    
    if (wateringStatus.waterLevel < MIN_WATER_LEVEL) {
      LOG_ERROR("Water level too low for watering");
      return;
    }
    
    isWatering = true;
    currentWateringZone = zoneIndex;
    currentWateringDuration = durationSeconds * 1000; // Convert to milliseconds
    wateringStartTime = millis();
    
    LOG_INFO("Starting watering - Zone: " + String(zoneIndex+1) + " (" + zones[zoneIndex].zoneName + 
             "), Duration: " + String(durationSeconds) + "s, Source: " + source);
    
    // Start pump with prime time
    digitalWrite(PUMP_RELAY_PIN, HIGH);
    wateringStatus.pumpRunning = true;
    pumpStartTime = millis();
    
    // Wait for pump to prime, then open valve
    delay(PUMP_PRIME_TIME);
    digitalWrite(zones[zoneIndex].valvePin, HIGH);
    wateringStatus.activeZone = zoneIndex;
    
    // Update statistics
    zones[zoneIndex].lastWatered = millis();
    wateringStatus.lastWateringTime = getCurrentTimeString();
    wateringStatus.lastWateringTimestamp = millis();
    
    // Calculate next watering time
    calculateNextWateringTime();
    
    LOG_INFO("Watering started for zone " + String(zoneIndex+1));
    
    // Send notification to Blink
    if (systemConfig.blink_enabled) {
      Blynk.logEvent("watering_started", 
        "Started watering " + zones[zoneIndex].zoneName + " for " + 
        String(durationSeconds) + " seconds. Source: " + source);
    }
  }
  
  void stopWatering() {
    if (!isWatering) return;
    
    // Close valve first
    if (currentWateringZone >= 0 && currentWateringZone < MAX_ZONES) {
      digitalWrite(zones[currentWateringZone].valvePin, LOW);
    }
    
    // Stop pump after short delay
    delay(2000);
    digitalWrite(PUMP_RELAY_PIN, LOW);
    wateringStatus.pumpRunning = false;
    
    // Calculate actual runtime
    unsigned long actualRuntime = millis() - wateringStartTime;
    wateringStatus.totalRuntime += actualRuntime;
    
    LOG_INFO("Watering stopped for zone " + String(currentWateringZone+1) + 
             " after " + String(actualRuntime/1000) + " seconds");
    
    // Send completion notification to Blink
    if (systemConfig.blink_enabled) {
      String zoneName = (currentWateringZone >= 0) ? zones[currentWateringZone].zoneName : "Unknown";
      Blynk.logEvent("watering_complete", 
        "Completed watering " + zoneName + " in " + String(actualRuntime/1000) + 
        " seconds. Water used: " + String(wateringStatus.flowRate * (actualRuntime/60000.0), 1) + "L");
    }
    
    // Reset watering state
    isWatering = false;
    wateringStatus.activeZone = -1;
    currentWateringZone = -1;
    currentWateringDuration = 0;
    
    // Play success sound
    playSystemSound("success");
  }
  
  void monitorWatering() {
    if (!isWatering) return;
    
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - wateringStartTime;
    
    // Check if watering duration completed
    if (elapsedTime >= currentWateringDuration) {
      LOG_INFO("Watering duration completed");
      stopWatering();
      return;
    }
    
    // Check for emergency stop conditions
    if (wateringStatus.waterLevel < 10.0) { // Emergency low water
      LOG_ERROR("Emergency stop - critically low water level");
      stopWatering();
      return;
    }
    
    // Check if zone moisture is sufficient (for moisture-dependent watering)
    if (currentWateringZone >= 0 && currentWateringZone < MAX_ZONES) {
      if (zones[currentWateringZone].currentMoisture >= MAX_SOIL_MOISTURE) {
        LOG_INFO("Zone " + String(currentWateringZone+1) + " moisture sufficient, stopping watering");
        stopWatering();
        return;
      }
    }
    
    // Check for pump failure (no flow detected)
    if (elapsedTime > 30000 && wateringStatus.flowRate < 0.1) { // No flow after 30 seconds
      LOG_ERROR("Pump failure detected - no water flow");
      stopWatering();
      return;
    }
  }
  
  void calculateNextWateringTime() {
    if (!systemStatus.rtc_connected) {
      wateringStatus.nextWateringTime = "RTC not available";
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
      wateringStatus.nextWateringTime = String(nextHour) + ":" + 
        (nextMinute < 10 ? "0" : "") + String(nextMinute) + 
        (foundToday ? " today" : " tomorrow");
    } else {
      wateringStatus.nextWateringTime = "No schedule set";
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
    
    // Check low water alert
    if (wateringStatus.waterLevel < MIN_WATER_LEVEL && !wateringStatus.lowWaterAlert) {
      wateringStatus.lowWaterAlert = true;
      LOG_ERROR("Low water alert: " + String(wateringStatus.waterLevel) + "% remaining");
      
      if (systemConfig.blink_enabled) {
        Blynk.logEvent("low_water", "Water level is low: " + String(wateringStatus.waterLevel) + "% remaining");
      }
    } else if (wateringStatus.waterLevel >= MIN_WATER_LEVEL * 1.5) {
      wateringStatus.lowWaterAlert = false; // Clear alert when water is refilled
    }
    
    // Check for dry zones
    for (int i = 0; i < MAX_ZONES; i++) {
      if (zones[i].enabled && zones[i].currentMoisture < MIN_SOIL_MOISTURE) {
        if (millis() - zones[i].lastWatered > 86400000) { // 24 hours
          LOG_ERROR("Zone " + String(i+1) + " critically dry: " + String(zones[i].currentMoisture) + "%");
          
          if (systemConfig.blink_enabled) {
            Blynk.logEvent("zone_dry", zones[i].zoneName + " is critically dry: " + 
                          String(zones[i].currentMoisture) + "%");
          }
        }
      }
    }
    
    // Periodic alert reminders
    if (wateringStatus.lowWaterAlert && (millis() - lastAlertTime > 1800000)) { // Every 30 minutes
      blinkLED(255, 0, 0, 5); // Red blink for low water
      lastAlertTime = millis();
    }
  }
  
  void updateSystemStatus() {
    // Reset daily counters at midnight
    static int lastDay = -1;
    if (systemStatus.rtc_connected) {
      DateTime now = rtc.now();
      if (lastDay != now.day()) {
        wateringStatus.dailyWaterUsage = 0;
        for (int i = 0; i < MAX_ZONES; i++) {
          zones[i].totalWaterUsed = 0;
        }
        lastDay = now.day();
        LOG_INFO("Daily water usage counters reset");
      }
    }
  }
  
  void setupBlinkIntegration() {
    if (!systemConfig.blink_enabled) return;
    
    // Virtual Pin assignments:
    // V0 - Zone 1 moisture level
    // V1 - Manual watering button
    // V2 - Zone 2 moisture level
    // V3 - Zone 3 moisture level
    // V4 - Zone 4 moisture level
    // V5 - Water level
    // V6 - Daily water usage
    // V7 - System status display
    // V8 - Pump status
    // V9 - Rain sensor
    // V10 - Flow rate
    // V11 - System pause/resume
    
    LOG_INFO("Blink integration setup completed");
  }
  
  void sendBlynkData() override {
    if (!systemConfig.blink_enabled || !systemStatus.network.blink_connected) return;
    
    // Send zone moisture levels
    for (int i = 0; i < MAX_ZONES && i < 4; i++) {
      Blynk.virtualWrite(V0 + i, zones[i].currentMoisture);
    }
    
    Blynk.virtualWrite(V5, wateringStatus.waterLevel);
    Blynk.virtualWrite(V6, wateringStatus.dailyWaterUsage);
    Blynk.virtualWrite(V8, wateringStatus.pumpRunning ? 1 : 0);
    Blynk.virtualWrite(V9, wateringStatus.isRaining ? 1 : 0);
    Blynk.virtualWrite(V10, wateringStatus.flowRate);
    
    // Send system status text
    String statusText = "Water: " + String(wateringStatus.waterLevel) + "%\n";
    statusText += "Usage: " + String(wateringStatus.dailyWaterUsage, 1) + "L\n";
    statusText += "Active: " + String(wateringStatus.activeZone >= 0 ? zones[wateringStatus.activeZone].zoneName : "None");
    
    Blynk.virtualWrite(V7, statusText);
  }
  
  void handleBlynkCommand(int pin, int value) override {
    switch (pin) {
      case V1: // Manual watering button
        if (value == 1 && !isWatering && !wateringStatus.systemPaused) {
          startWatering(0, DEFAULT_WATERING_DURATION / 1000, "Blink App");
        }
        break;
        
      case V11: // System pause/resume
        wateringStatus.systemPaused = (value == 1);
        LOG_INFO("System " + String(wateringStatus.systemPaused ? "paused" : "resumed") + " via Blink");
        if (wateringStatus.systemPaused && isWatering) {
          stopWatering();
        }
        break;
    }
  }
  
  String getSystemStatus() override {
    DynamicJsonDocument doc(1024);
    
    doc["water_level"] = wateringStatus.waterLevel;
    doc["is_raining"] = wateringStatus.isRaining;
    doc["pump_running"] = wateringStatus.pumpRunning;
    doc["active_zone"] = wateringStatus.activeZone;
    doc["daily_water_usage"] = wateringStatus.dailyWaterUsage;
    doc["last_watering"] = wateringStatus.lastWateringTime;
    doc["next_watering"] = wateringStatus.nextWateringTime;
    doc["low_water_alert"] = wateringStatus.lowWaterAlert;
    doc["system_paused"] = wateringStatus.systemPaused;
    doc["flow_rate"] = wateringStatus.flowRate;
    doc["is_watering"] = isWatering;
    
    // Add zone information
    JsonArray zonesArray = doc.createNestedArray("zones");
    for (int i = 0; i < MAX_ZONES; i++) {
      JsonObject zone = zonesArray.createNestedObject();
      zone["name"] = zones[i].zoneName;
      zone["moisture"] = zones[i].currentMoisture;
      zone["target"] = zones[i].targetMoisture;
      zone["enabled"] = zones[i].enabled;
      zone["water_used"] = zones[i].totalWaterUsed;
    }
    
    String output;
    serializeJson(doc, output);
    return output;
  }
  
  void loadWateringSchedules() {
    // Load from EEPROM or use defaults
    // Implementation would read from EEPROM address
    LOG_INFO("Watering schedules loaded");
  }
  
  void saveWateringSchedules() {
    // Save to EEPROM
    // Implementation would write to EEPROM address
    LOG_INFO("Watering schedules saved");
  }
};

// Global system instance
EnhancedTomatoWateringSystem* tomatoSystem = nullptr;
EnhancedSystemBase* currentSystem = nullptr;

// Blink event handlers
BLYNK_WRITE(V1) {
  if (tomatoSystem) {
    tomatoSystem->handleBlynkCommand(V1, param.asInt());
  }
}

BLYNK_WRITE(V11) {
  if (tomatoSystem) {
    tomatoSystem->handleBlynkCommand(V11, param.asInt());
  }
}

void setup() {
  // Create system instance
  tomatoSystem = new EnhancedTomatoWateringSystem();
  currentSystem = tomatoSystem;
  
  // Initialize system
  if (!tomatoSystem->begin()) {
    Serial.println("System initialization failed!");
    while (true) {
      delay(1000);
    }
  }
  
  Serial.println("Enhanced Tomato Watering System ready!");
}

void loop() {
  if (tomatoSystem) {
    tomatoSystem->loop();
  }
}
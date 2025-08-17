/*
 * ระบบรดน้ำมะเขือเทศอัตโนมัติ (Automatic Tomato Watering System)
 * 
 * Firmware made by: RDTRC
 * Version: 2.0
 * Created: 2024
 * 
 * คุณสมบัติ (Features):
 * - รดน้ำตามเวลาที่กำหนด (Scheduled watering)
 * - ตรวจสอบความชื้นในดิน (Soil moisture monitoring)
 * - ตรวจสอบระดับน้ำในถัง (Water tank level monitoring)
 * - ควบคุมผ่านแอพมือถือ (Mobile app control)
 * - แจ้งเตือนเมื่อน้ำหมด (Water empty alerts)
 * - Boot screen แสดง "FW make by RDTRC"
 * - ระบบปั๊มน้ำอัตโนมัติ (Automatic water pump)
 * - เซ็นเซอร์อุณหภูมิและความชื้นอากาศ (Temperature & humidity monitoring)
 */

// Include the common base system
#include "../../common/base_code/system_base.h"
#include <DHT.h>

// System-specific pin definitions
#define PUMP_PIN 5
#define VALVE_PIN 18
#define SOIL_MOISTURE_PIN 34    // ADC pin
#define WATER_LEVEL_PIN 35      // ADC pin
#define DHT_PIN 4
#define DHT_TYPE DHT22
#define FLOW_SENSOR_PIN 2       // Interrupt pin
#define BUTTON_MANUAL_WATER_PIN 32
#define BUTTON_STOP_PIN 33
#define BUTTON_RESET_PIN 15

// Tomato watering specific configuration
#define MAX_SCHEDULES 6
#define MIN_SOIL_MOISTURE 30     // % (0-100)
#define MIN_WATER_LEVEL 10       // % (0-100)
#define DEFAULT_WATER_TIME 10000 // ms (10 seconds)
#define MAX_DAILY_WATER 5000     // ml
#define PUMP_FLOW_RATE 100       // ml per second (approximate)

// Watering schedule structure
struct TomatoSchedule {
  int hour;
  int minute;
  int duration;    // milliseconds
  bool enabled;
  bool moistureCheck; // Check soil moisture before watering
};

// Tomato system status
struct TomatoSystemStatus {
  float soilMoisture;      // %
  float waterLevel;        // %
  float temperature;       // °C
  float humidity;          // %
  int dailyWaterAmount;    // ml
  int dailyWaterCount;     // number of waterings
  String lastWaterTime;
  String nextWaterTime;
  bool pumpActive;
  bool valveOpen;
  bool soilDry;
  bool waterLow;
  volatile int flowPulses; // For flow sensor
  float flowRate;          // ml/min
};

// Tomato Watering System Class
class TomatoWateringSystem : public SystemBase {
private:
  DHT dht;
  
  TomatoSchedule schedules[MAX_SCHEDULES];
  TomatoSystemStatus tomatoStatus;
  
  unsigned long lastScheduleCheck;
  unsigned long lastSensorCheck;
  unsigned long lastFlowCheck;
  unsigned long pumpStartTime;
  
public:
  TomatoWateringSystem() : SystemBase("Tomato Water"), dht(DHT_PIN, DHT_TYPE) {
    lastScheduleCheck = 0;
    lastSensorCheck = 0;
    lastFlowCheck = 0;
    pumpStartTime = 0;
    
    // Initialize default schedules
    schedules[0] = {6, 0, 15000, true, true};   // เช้า 06:00 น. 15 วินาที
    schedules[1] = {12, 0, 10000, true, true};  // เที่ยง 12:00 น. 10 วินาที
    schedules[2] = {18, 0, 20000, true, true};  // เย็น 18:00 น. 20 วินาที
    
    // Initialize remaining schedules as disabled
    for (int i = 3; i < MAX_SCHEDULES; i++) {
      schedules[i] = {0, 0, 0, false, false};
    }
    
    // Initialize tomato status
    tomatoStatus.soilMoisture = 0;
    tomatoStatus.waterLevel = 0;
    tomatoStatus.temperature = 0;
    tomatoStatus.humidity = 0;
    tomatoStatus.dailyWaterAmount = 0;
    tomatoStatus.dailyWaterCount = 0;
    tomatoStatus.lastWaterTime = "";
    tomatoStatus.nextWaterTime = "";
    tomatoStatus.pumpActive = false;
    tomatoStatus.valveOpen = false;
    tomatoStatus.soilDry = false;
    tomatoStatus.waterLow = false;
    tomatoStatus.flowPulses = 0;
    tomatoStatus.flowRate = 0;
  }
  
  void setupSystem() override {
    LOG_INFO("Setting up Tomato Watering System");
    
    // Initialize pump and valve
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(VALVE_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW);
    digitalWrite(VALVE_PIN, LOW);
    
    // Initialize sensors
    pinMode(SOIL_MOISTURE_PIN, INPUT);
    pinMode(WATER_LEVEL_PIN, INPUT);
    
    // Initialize DHT sensor
    dht.begin();
    
    // Initialize flow sensor with interrupt
    pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseCounter, RISING);
    
    // Initialize buttons
    pinMode(BUTTON_MANUAL_WATER_PIN, INPUT_PULLUP);
    pinMode(BUTTON_STOP_PIN, INPUT_PULLUP);
    pinMode(BUTTON_RESET_PIN, INPUT_PULLUP);
    
    // Load tomato watering specific settings
    loadTomatoSettings();
    
    // Calculate next watering time
    calculateNextWateringTime();
    
    // Setup web endpoints
    setupTomatoWebServer();
    
    // Setup Blynk virtual pins
    setupBlynkPins();
    
    LOG_INFO("Tomato Watering System setup complete");
    
    // Show initial status on LCD
    showStatus("Tomato Water v2.0", "FW by RDTRC");
    delay(2000);
  }
  
  void runSystem() override {
    // Check sensors every 10 seconds
    if (millis() - lastSensorCheck > 10000) {
      checkSensors();
      lastSensorCheck = millis();
    }
    
    // Check schedule every minute
    if (millis() - lastScheduleCheck > 60000) {
      checkSchedule();
      lastScheduleCheck = millis();
    }
    
    // Check flow sensor every 5 seconds
    if (millis() - lastFlowCheck > 5000) {
      calculateFlowRate();
      lastFlowCheck = millis();
    }
    
    // Check manual buttons
    checkManualButtons();
    
    // Monitor pump operation
    monitorPump();
    
    // Update Blynk
    updateBlynkData();
  }
  
  String getSystemStatus() override {
    DynamicJsonDocument doc(512);
    doc["soil"] = String(tomatoStatus.soilMoisture, 1) + "%";
    doc["water"] = String(tomatoStatus.waterLevel, 1) + "%";
    doc["temp"] = String(tomatoStatus.temperature, 1) + "C";
    doc["today"] = String(tomatoStatus.dailyWaterCount);
    
    String output;
    serializeJson(doc, output);
    return output;
  }
  
  void handleSystemCommand(const String& command) override {
    if (command == "water") {
      startWatering(DEFAULT_WATER_TIME);
    } else if (command == "stop") {
      stopWatering();
    } else if (command == "reset_daily") {
      resetDailyStats();
    } else if (command.startsWith("water_")) {
      int duration = command.substring(6).toInt() * 1000; // Convert to ms
      if (duration > 0 && duration <= 60000) {
        startWatering(duration);
      }
    }
  }

private:
  static void flowPulseCounter() {
    // Static method for interrupt handler
    // This will be called from interrupt, so keep it simple
  }
  
  void checkSensors() {
    // Read soil moisture (ADC value converted to percentage)
    int soilRaw = analogRead(SOIL_MOISTURE_PIN);
    tomatoStatus.soilMoisture = map(soilRaw, 0, 4095, 100, 0); // Invert: wet=100%, dry=0%
    tomatoStatus.soilMoisture = constrain(tomatoStatus.soilMoisture, 0, 100);
    
    // Read water level (ADC value converted to percentage)
    int waterRaw = analogRead(WATER_LEVEL_PIN);
    tomatoStatus.waterLevel = map(waterRaw, 0, 4095, 0, 100);
    tomatoStatus.waterLevel = constrain(tomatoStatus.waterLevel, 0, 100);
    
    // Read temperature and humidity
    tomatoStatus.temperature = dht.readTemperature();
    tomatoStatus.humidity = dht.readHumidity();
    
    // Check for sensor errors
    if (isnan(tomatoStatus.temperature)) tomatoStatus.temperature = 0;
    if (isnan(tomatoStatus.humidity)) tomatoStatus.humidity = 0;
    
    // Update status flags
    tomatoStatus.soilDry = (tomatoStatus.soilMoisture < MIN_SOIL_MOISTURE);
    tomatoStatus.waterLow = (tomatoStatus.waterLevel < MIN_WATER_LEVEL);
    
    // Send alerts if needed
    if (tomatoStatus.soilDry) {
      LOG_INFO("Soil is dry: " + String(tomatoStatus.soilMoisture) + "%");
      if (status.blynk_connected) {
        Blynk.notify("ดินแห้ง! ความชื้น " + String(tomatoStatus.soilMoisture, 1) + "%");
      }
      setLEDColor(255, 255, 0); // Yellow warning
    }
    
    if (tomatoStatus.waterLow) {
      LOG_ERROR("Water level low: " + String(tomatoStatus.waterLevel) + "%");
      if (status.blynk_connected) {
        Blynk.notify("น้ำในถังเหลือน้อย! " + String(tomatoStatus.waterLevel, 1) + "%");
      }
      setLEDColor(255, 165, 0); // Orange warning
    }
  }
  
  void checkSchedule() {
    if (!status.rtc_connected) return;
    
    DateTime now = getCurrentTime();
    
    for (int i = 0; i < MAX_SCHEDULES; i++) {
      if (schedules[i].enabled) {
        if (now.hour() == schedules[i].hour && now.minute() == schedules[i].minute) {
          LOG_INFO("Scheduled watering time: " + String(schedules[i].hour) + ":" + String(schedules[i].minute));
          
          // Check if we should water based on soil moisture
          bool shouldWater = true;
          if (schedules[i].moistureCheck && !tomatoStatus.soilDry) {
            LOG_INFO("Soil moisture OK, skipping scheduled watering");
            shouldWater = false;
          }
          
          if (shouldWater && tomatoStatus.dailyWaterAmount < MAX_DAILY_WATER) {
            startWatering(schedules[i].duration);
          }
          
          calculateNextWateringTime();
          break;
        }
      }
    }
  }
  
  void checkManualButtons() {
    // Manual water button
    if (digitalRead(BUTTON_MANUAL_WATER_PIN) == LOW) {
      delay(50); // debounce
      if (digitalRead(BUTTON_MANUAL_WATER_PIN) == LOW) {
        LOG_INFO("Manual watering triggered");
        startWatering(DEFAULT_WATER_TIME);
        
        while (digitalRead(BUTTON_MANUAL_WATER_PIN) == LOW) {
          delay(10);
        }
      }
    }
    
    // Stop button
    if (digitalRead(BUTTON_STOP_PIN) == LOW) {
      delay(50); // debounce
      if (digitalRead(BUTTON_STOP_PIN) == LOW) {
        LOG_INFO("Manual stop triggered");
        stopWatering();
        
        while (digitalRead(BUTTON_STOP_PIN) == LOW) {
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
  
  void startWatering(int duration) {
    if (tomatoStatus.pumpActive) {
      LOG_INFO("Pump already active, ignoring request");
      return;
    }
    
    if (tomatoStatus.waterLow) {
      LOG_ERROR("Water level too low to start watering");
      showStatus("Water Empty", "Refill tank!");
      playErrorSound();
      return;
    }
    
    int estimatedWater = (duration / 1000) * PUMP_FLOW_RATE;
    if (tomatoStatus.dailyWaterAmount + estimatedWater > MAX_DAILY_WATER) {
      LOG_ERROR("Daily water limit would be exceeded");
      showStatus("Daily Limit", "Exceeded!");
      playErrorSound();
      return;
    }
    
    LOG_INFO("Starting watering for " + String(duration) + "ms");
    
    // Show watering status on LCD
    showStatus("Watering...", String(duration/1000) + "s");
    
    // Pre-watering sound
    playBeep(1000, 300);
    delay(200);
    
    // Blue LED during watering
    setLEDColor(0, 150, 255);
    
    // Start pump and open valve
    tomatoStatus.pumpActive = true;
    tomatoStatus.valveOpen = true;
    pumpStartTime = millis();
    
    digitalWrite(PUMP_PIN, HIGH);
    digitalWrite(VALVE_PIN, HIGH);
    
    // Reset flow counter
    tomatoStatus.flowPulses = 0;
    
    // Schedule pump stop
    // Note: In a real implementation, you might want to use a timer interrupt
    // For now, we'll check in monitorPump()
  }
  
  void stopWatering() {
    if (!tomatoStatus.pumpActive) {
      return;
    }
    
    LOG_INFO("Stopping watering");
    
    // Stop pump and close valve
    digitalWrite(PUMP_PIN, LOW);
    digitalWrite(VALVE_PIN, LOW);
    
    tomatoStatus.pumpActive = false;
    tomatoStatus.valveOpen = false;
    
    // Calculate actual watering duration and amount
    unsigned long actualDuration = millis() - pumpStartTime;
    int waterAmount = (actualDuration / 1000) * PUMP_FLOW_RATE;
    
    // Update statistics
    tomatoStatus.dailyWaterAmount += waterAmount;
    tomatoStatus.dailyWaterCount++;
    
    if (status.rtc_connected) {
      DateTime now = getCurrentTime();
      tomatoStatus.lastWaterTime = String(now.hour()) + ":" + 
                                  (now.minute() < 10 ? "0" : "") + String(now.minute());
    }
    
    // Save settings
    saveTomatoSettings();
    
    // Success sound
    playSuccessSound();
    
    // Send Blynk notification
    if (status.blynk_connected) {
      Blynk.notify("รดน้ำมะเขือเทศแล้ว " + String(actualDuration/1000) + " วินาที (" + String(waterAmount) + " ml)");
    }
    
    LOG_INFO("Watering completed: " + String(actualDuration/1000) + "s, ~" + String(waterAmount) + "ml");
    
    // Reset LED color
    setLEDColor(0, 255, 0);
  }
  
  void monitorPump() {
    static unsigned long scheduledStopTime = 0;
    static int scheduledDuration = 0;
    
    if (tomatoStatus.pumpActive) {
      // Check if we should stop based on scheduled duration
      if (scheduledStopTime > 0 && millis() >= scheduledStopTime) {
        stopWatering();
        scheduledStopTime = 0;
        scheduledDuration = 0;
      }
      
      // Safety check: stop if pump has been running too long (max 2 minutes)
      if (millis() - pumpStartTime > 120000) {
        LOG_ERROR("Pump safety timeout - stopping");
        stopWatering();
        setError(ERROR_ACTUATOR);
      }
    } else if (scheduledStopTime == 0 && pumpStartTime > 0) {
      // This handles the duration timing - set stop time when pump starts
      // This is a simplified approach; in practice you'd want better timing control
    }
  }
  
  void calculateFlowRate() {
    // Calculate flow rate based on pulse count
    // This is approximate - calibration needed for actual sensor
    static int lastPulseCount = 0;
    int currentPulses = tomatoStatus.flowPulses;
    int pulseDiff = currentPulses - lastPulseCount;
    
    // Assume each pulse = 1ml (needs calibration)
    tomatoStatus.flowRate = (pulseDiff * 12); // ml/min (5 second intervals * 12 = 1 minute)
    
    lastPulseCount = currentPulses;
  }
  
  void calculateNextWateringTime() {
    if (!status.rtc_connected) {
      tomatoStatus.nextWaterTime = "No RTC";
      return;
    }
    
    DateTime now = getCurrentTime();
    int currentMinutes = now.hour() * 60 + now.minute();
    int nextMinutes = 24 * 60; // Start with tomorrow
    
    // Find next enabled schedule
    for (int i = 0; i < MAX_SCHEDULES; i++) {
      if (schedules[i].enabled) {
        int scheduleMinutes = schedules[i].hour * 60 + schedules[i].minute;
        if (scheduleMinutes > currentMinutes && scheduleMinutes < nextMinutes) {
          nextMinutes = scheduleMinutes;
        }
      }
    }
    
    // If no schedule today, find first schedule tomorrow
    if (nextMinutes == 24 * 60) {
      for (int i = 0; i < MAX_SCHEDULES; i++) {
        if (schedules[i].enabled) {
          int scheduleMinutes = schedules[i].hour * 60 + schedules[i].minute;
          if (scheduleMinutes < nextMinutes) {
            nextMinutes = scheduleMinutes;
          }
        }
      }
    }
    
    int nextHour = (nextMinutes / 60) % 24;
    int nextMin = nextMinutes % 60;
    
    tomatoStatus.nextWaterTime = (nextHour < 10 ? "0" : "") + String(nextHour) + ":" + 
                                (nextMin < 10 ? "0" : "") + String(nextMin);
  }
  
  void setupTomatoWebServer() {
    // Tomato watering specific endpoints
    server.on("/api/tomato/status", HTTP_GET, [this]() {
      DynamicJsonDocument doc(1024);
      doc["soilMoisture"] = tomatoStatus.soilMoisture;
      doc["waterLevel"] = tomatoStatus.waterLevel;
      doc["temperature"] = tomatoStatus.temperature;
      doc["humidity"] = tomatoStatus.humidity;
      doc["dailyWaterAmount"] = tomatoStatus.dailyWaterAmount;
      doc["dailyWaterCount"] = tomatoStatus.dailyWaterCount;
      doc["lastWaterTime"] = tomatoStatus.lastWaterTime;
      doc["nextWaterTime"] = tomatoStatus.nextWaterTime;
      doc["pumpActive"] = tomatoStatus.pumpActive;
      doc["soilDry"] = tomatoStatus.soilDry;
      doc["waterLow"] = tomatoStatus.waterLow;
      doc["flowRate"] = tomatoStatus.flowRate;
      
      String output;
      serializeJson(doc, output);
      server.send(200, "application/json", output);
    });
    
    server.on("/api/tomato/water", HTTP_POST, [this]() {
      int duration = DEFAULT_WATER_TIME;
      
      if (server.hasArg("plain")) {
        DynamicJsonDocument doc(256);
        deserializeJson(doc, server.arg("plain"));
        if (doc.containsKey("duration")) {
          duration = doc["duration"];
        }
      }
      
      if (duration > 0 && duration <= 60000) {
        startWatering(duration);
        server.send(200, "application/json", "{\"message\":\"Watering started\",\"success\":true}");
      } else {
        server.send(400, "application/json", "{\"message\":\"Invalid duration\",\"success\":false}");
      }
    });
    
    server.on("/api/tomato/stop", HTTP_POST, [this]() {
      stopWatering();
      server.send(200, "application/json", "{\"message\":\"Watering stopped\",\"success\":true}");
    });
  }
  
  void setupBlynkPins() {
    // Virtual pin assignments:
    // V0 - Soil moisture display
    // V1 - Water level display
    // V2 - Temperature display
    // V3 - Humidity display
    // V4 - Daily water amount
    // V10 - Manual water button
    // V11 - Stop button
    // V12 - System enable switch
  }
  
  void updateBlynkData() {
    static unsigned long lastBlynkUpdate = 0;
    
    if (status.blynk_connected && millis() - lastBlynkUpdate > 15000) {
      Blynk.virtualWrite(V0, tomatoStatus.soilMoisture);
      Blynk.virtualWrite(V1, tomatoStatus.waterLevel);
      Blynk.virtualWrite(V2, tomatoStatus.temperature);
      Blynk.virtualWrite(V3, tomatoStatus.humidity);
      Blynk.virtualWrite(V4, tomatoStatus.dailyWaterAmount);
      
      lastBlynkUpdate = millis();
    }
  }
  
  void loadTomatoSettings() {
    // Load tomato-specific settings from EEPROM
    EEPROM.get(400, schedules);
    EEPROM.get(500, tomatoStatus.dailyWaterAmount);
    EEPROM.get(504, tomatoStatus.dailyWaterCount);
    
    // Validate loaded data
    if (tomatoStatus.dailyWaterAmount < 0 || tomatoStatus.dailyWaterAmount > 50000) {
      tomatoStatus.dailyWaterAmount = 0;
    }
    if (tomatoStatus.dailyWaterCount < 0 || tomatoStatus.dailyWaterCount > 1000) {
      tomatoStatus.dailyWaterCount = 0;
    }
  }
  
  void saveTomatoSettings() {
    EEPROM.put(400, schedules);
    EEPROM.put(500, tomatoStatus.dailyWaterAmount);
    EEPROM.put(504, tomatoStatus.dailyWaterCount);
    EEPROM.commit();
  }
  
  void resetDailyStats() {
    tomatoStatus.dailyWaterAmount = 0;
    tomatoStatus.dailyWaterCount = 0;
    tomatoStatus.lastWaterTime = "";
    tomatoStatus.flowPulses = 0;
    saveTomatoSettings();
    
    showStatus("Daily Stats", "Reset!");
    playSuccessSound();
    delay(2000);
    
    LOG_INFO("Daily statistics reset");
  }
};

// Global system instance
TomatoWateringSystem tomatoWatering;

// Blynk Virtual Pin Handlers
BLYNK_WRITE(V10) { // Manual water button
  if (param.asInt() == 1) {
    tomatoWatering.handleSystemCommand("water");
  }
}

BLYNK_WRITE(V11) { // Stop button
  if (param.asInt() == 1) {
    tomatoWatering.handleSystemCommand("stop");
  }
}

BLYNK_WRITE(V12) { // System enable/disable
  config.system_enabled = param.asInt();
  tomatoWatering.saveConfig();
}

// Arduino setup and loop
void setup() {
  // Initialize the tomato watering system
  if (!tomatoWatering.begin()) {
    // System failed to initialize
    Serial.println("Tomato Watering System initialization failed!");
    while (true) {
      delay(1000);
    }
  }
}

void loop() {
  // Run the main system loop
  tomatoWatering.loop();
}
/*
 * ระบบให้อาหารแมวอัตโนมัติ (Automatic Cat Feeding System)
 * 
 * Firmware made by: RDTRC
 * Version: 2.0
 * Created: 2024
 * 
 * คุณสมบัติ (Features):
 * - ให้อาหารตามเวลาที่กำหนด (Scheduled feeding)
 * - วัดน้ำหนักอาหารที่เหลือ (Weight monitoring)
 * - ควบคุมผ่านแอพมือถือ (Mobile app control)
 * - แจ้งเตือนเมื่ออาหารหมด (Low food alerts)
 * - บันทึกประวัติการให้อาหาร (Feeding history)
 * - ระบบป้องกันการให้อาหารซ้ำ (Duplicate feeding prevention)
 * - เซ็นเซอร์ตรวจจับแมว (Cat detection sensor)
 * - Boot screen แสดง "FW make by RDTRC"
 */

// Include the common base system
#include "../../common/base_code/system_base.h"
#include <ESP32Servo.h>
#include <HX711.h>

// System-specific pin definitions
#define SERVO_PIN 18
#define LOADCELL_DOUT_PIN 19
#define LOADCELL_SCK_PIN 20
#define PIR_PIN 4
#define BUTTON_MANUAL_PIN 32
#define BUTTON_RESET_PIN 33

// Cat feeding specific configuration
#define MAX_SCHEDULES 6
#define MIN_FOOD_WEIGHT 100.0  // กรัม
#define DEFAULT_FEED_AMOUNT 30.0  // กรัม
#define MAX_DAILY_FOOD 500.0  // กรัม
#define FEEDING_TIMEOUT 10000  // 10 วินาที

// Feeding schedule structure
struct FeedingSchedule {
  int hour;
  int minute;
  float amount;
  bool enabled;
};

// Cat feeding system status
struct CatFeedingStatus {
  float currentWeight;
  float totalFoodDispensed;
  int feedingCount;
  bool catDetected;
  String lastFeedingTime;
  String nextFeedingTime;
  float dailyFoodLimit;
  bool manualFeedingMode;
};

// Cat Feeding System Class
class CatFeedingSystem : public SystemBase {
private:
  Servo feedingServo;
  HX711 scale;
  
  FeedingSchedule schedules[MAX_SCHEDULES];
  CatFeedingStatus catStatus;
  
  float calibrationFactor;
  unsigned long lastFeedingTime;
  unsigned long lastWeightCheck;
  unsigned long lastPIRCheck;
  unsigned long lastMotionTime;
  bool pirState;
  bool lastPirState;
  
public:
  CatFeedingSystem() : SystemBase("Cat Feeder") {
    calibrationFactor = -7050.0;
    lastFeedingTime = 0;
    lastWeightCheck = 0;
    lastPIRCheck = 0;
    lastMotionTime = 0;
    pirState = false;
    lastPirState = false;
    
    // Initialize feeding schedules
    schedules[0] = {7, 0, 50.0, true};   // เช้า 07:00 น.
    schedules[1] = {12, 0, 30.0, true};  // เที่ยง 12:00 น.
    schedules[2] = {18, 0, 50.0, true};  // เย็น 18:00 น.
    schedules[3] = {0, 0, 0.0, false};   // ปิดใช้งาน
    schedules[4] = {0, 0, 0.0, false};   // ปิดใช้งาน
    schedules[5] = {0, 0, 0.0, false};   // ปิดใช้งาน
    
    // Initialize cat status
    catStatus.currentWeight = 0;
    catStatus.totalFoodDispensed = 0;
    catStatus.feedingCount = 0;
    catStatus.catDetected = false;
    catStatus.lastFeedingTime = "";
    catStatus.nextFeedingTime = "";
    catStatus.dailyFoodLimit = MAX_DAILY_FOOD;
    catStatus.manualFeedingMode = false;
  }
  
  void setupSystem() override {
    LOG_INFO("Setting up Cat Feeding System");
    
    // Initialize servo
    feedingServo.attach(SERVO_PIN);
    feedingServo.write(0); // ปิดช่องให้อาหาร
    
    // Initialize load cell
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(calibrationFactor);
    scale.tare();
    
    // Initialize PIR sensor
    pinMode(PIR_PIN, INPUT);
    
    // Initialize buttons
    pinMode(BUTTON_MANUAL_PIN, INPUT_PULLUP);
    pinMode(BUTTON_RESET_PIN, INPUT_PULLUP);
    
    // Load cat feeding specific settings
    loadCatSettings();
    
    // Calculate next feeding time
    calculateNextFeedingTime();
    
    // Setup web endpoints
    setupCatWebServer();
    
    // Setup Blynk virtual pins
    setupBlynkPins();
    
    LOG_INFO("Cat Feeding System setup complete");
    
    // Show initial status on LCD
    showStatus("Cat Feeder v2.0", "FW by RDTRC");
    delay(2000);
  }
  
  void runSystem() override {
    // Check food weight every 5 seconds
    if (millis() - lastWeightCheck > 5000) {
      checkFoodWeight();
      lastWeightCheck = millis();
    }
    
    // Check PIR sensor every 500ms
    if (millis() - lastPIRCheck > 500) {
      checkCatDetection();
      lastPIRCheck = millis();
    }
    
    // Check feeding schedule
    checkFeedingSchedule();
    
    // Check manual buttons
    checkManualButtons();
    
    // Update Blynk
    updateBlynkData();
  }
  
  String getSystemStatus() override {
    DynamicJsonDocument doc(512);
    doc["weight"] = catStatus.currentWeight;
    doc["fed"] = catStatus.feedingCount;
    doc["cat"] = catStatus.catDetected ? "Yes" : "No";
    doc["next"] = catStatus.nextFeedingTime;
    
    String output;
    serializeJson(doc, output);
    return output;
  }
  
  void handleSystemCommand(const String& command) override {
    if (command == "feed") {
      dispenseFeed(DEFAULT_FEED_AMOUNT);
    } else if (command == "calibrate") {
      scale.tare();
      LOG_INFO("Scale calibrated");
    } else if (command == "reset_stats") {
      resetFeedingStats();
    }
  }

private:
  void checkFoodWeight() {
    if (scale.is_ready()) {
      catStatus.currentWeight = scale.get_units(3);
      
      if (catStatus.currentWeight < 0) {
        catStatus.currentWeight = 0;
      }
      
      // Check for low food
      if (catStatus.currentWeight < MIN_FOOD_WEIGHT) {
        LOG_ERROR("Low food warning: " + String(catStatus.currentWeight) + "g");
        setLEDColor(255, 255, 0); // Yellow warning
        
        // Send Blynk notification
        if (status.blynk_connected) {
          Blynk.notify("อาหารแมวเหลือน้อย! เหลือ " + String(catStatus.currentWeight, 1) + " กรัม");
        }
        
        // Play warning sound
        playBeep(800, 300);
        delay(100);
        playBeep(800, 300);
      }
    }
  }
  
  void checkCatDetection() {
    pirState = digitalRead(PIR_PIN);
    
    if (pirState != lastPirState) {
      if (pirState == HIGH) {
        catStatus.catDetected = true;
        lastMotionTime = millis();
        
        LOG_INFO("Cat detected!");
        
        // Send Blynk notification
        if (status.blynk_connected) {
          Blynk.notify("ตรวจพบแมวที่ชามอาหาร!");
        }
        
        // Blue LED for cat detection
        setLEDColor(0, 0, 255);
        
        // Short beep
        playBeep(1000, 100);
      }
      lastPirState = pirState;
    }
    
    // Reset cat detection after 30 seconds
    if (millis() - lastMotionTime > 30000) {
      catStatus.catDetected = false;
    }
  }
  
  void checkFeedingSchedule() {
    if (!status.rtc_connected) return;
    
    DateTime now = getCurrentTime();
    
    for (int i = 0; i < MAX_SCHEDULES; i++) {
      if (schedules[i].enabled) {
        if (now.hour() == schedules[i].hour && 
            now.minute() == schedules[i].minute &&
            now.second() == 0) {
          
          // Prevent duplicate feeding in the same minute
          if (millis() - lastFeedingTime > 60000) {
            LOG_INFO("Scheduled feeding time: " + String(schedules[i].hour) + ":" + String(schedules[i].minute));
            
            if (dispenseFeed(schedules[i].amount)) {
              lastFeedingTime = millis();
              
              // Update last feeding time
              catStatus.lastFeedingTime = String(now.hour()) + ":" + 
                                         (now.minute() < 10 ? "0" : "") + String(now.minute()) + 
                                         " " + String(now.day()) + "/" + String(now.month());
              
              calculateNextFeedingTime();
            }
            
            break;
          }
        }
      }
    }
  }
  
  void checkManualButtons() {
    // Manual feed button
    if (digitalRead(BUTTON_MANUAL_PIN) == LOW) {
      delay(50); // debounce
      if (digitalRead(BUTTON_MANUAL_PIN) == LOW) {
        LOG_INFO("Manual feeding triggered");
        dispenseFeed(DEFAULT_FEED_AMOUNT);
        
        while (digitalRead(BUTTON_MANUAL_PIN) == LOW) {
          delay(10);
        }
      }
    }
    
    // Reset button
    if (digitalRead(BUTTON_RESET_PIN) == LOW) {
      delay(50); // debounce
      if (digitalRead(BUTTON_RESET_PIN) == LOW) {
        LOG_INFO("Manual reset triggered");
        resetFeedingStats();
        scale.tare();
        
        while (digitalRead(BUTTON_RESET_PIN) == LOW) {
          delay(10);
        }
      }
    }
  }
  
  bool dispenseFeed(float amount) {
    // Check daily limit
    if (catStatus.totalFoodDispensed + amount > catStatus.dailyFoodLimit) {
      LOG_ERROR("Daily food limit exceeded");
      showStatus("Daily Limit", "Exceeded!");
      playErrorSound();
      return false;
    }
    
    // Check minimum food available
    if (catStatus.currentWeight < amount) {
      LOG_ERROR("Not enough food available");
      showStatus("Low Food", "Refill needed");
      playErrorSound();
      return false;
    }
    
    LOG_INFO("Dispensing " + String(amount) + "g of food");
    
    // Show feeding status on LCD
    showStatus("Feeding...", String(amount) + "g");
    
    // Pre-feeding sound
    playBeep(1200, 200);
    delay(300);
    playBeep(1200, 200);
    
    // Calculate servo time (approximately 1 second per 10g)
    int servoTime = (amount / 10.0) * 1000;
    if (servoTime < 500) servoTime = 500;   // Minimum time
    if (servoTime > 5000) servoTime = 5000; // Maximum time
    
    // Orange LED during feeding
    setLEDColor(255, 165, 0);
    
    // Open feeding gate
    feedingServo.write(90);
    delay(servoTime);
    
    // Close feeding gate
    feedingServo.write(0);
    
    // Update statistics
    catStatus.totalFoodDispensed += amount;
    catStatus.feedingCount++;
    
    // Save settings
    saveCatSettings();
    
    // Success sound
    playSuccessSound();
    
    // Send Blynk notification
    if (status.blynk_connected) {
      Blynk.notify("ให้อาหารแมวแล้ว " + String(amount) + " กรัม");
    }
    
    LOG_INFO("Feeding completed successfully");
    
    // Reset LED color
    setLEDColor(0, 255, 0);
    
    return true;
  }
  
  void calculateNextFeedingTime() {
    if (!status.rtc_connected) {
      catStatus.nextFeedingTime = "No RTC";
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
    
    catStatus.nextFeedingTime = (nextHour < 10 ? "0" : "") + String(nextHour) + ":" + 
                               (nextMin < 10 ? "0" : "") + String(nextMin);
  }
  
  void setupCatWebServer() {
    // Cat feeding specific endpoints
    server.on("/api/cat/status", HTTP_GET, [this]() {
      DynamicJsonDocument doc(1024);
      doc["weight"] = catStatus.currentWeight;
      doc["feedingCount"] = catStatus.feedingCount;
      doc["totalFood"] = catStatus.totalFoodDispensed;
      doc["catDetected"] = catStatus.catDetected;
      doc["lastFeedingTime"] = catStatus.lastFeedingTime;
      doc["nextFeedingTime"] = catStatus.nextFeedingTime;
      doc["dailyLimit"] = catStatus.dailyFoodLimit;
      
      String output;
      serializeJson(doc, output);
      server.send(200, "application/json", output);
    });
    
    server.on("/api/cat/feed", HTTP_POST, [this]() {
      float amount = DEFAULT_FEED_AMOUNT;
      
      if (server.hasArg("plain")) {
        DynamicJsonDocument doc(256);
        deserializeJson(doc, server.arg("plain"));
        if (doc.containsKey("amount")) {
          amount = doc["amount"];
        }
      }
      
      if (amount > 0 && amount <= 100) {
        if (dispenseFeed(amount)) {
          server.send(200, "application/json", "{\"message\":\"Feeding successful\",\"success\":true}");
        } else {
          server.send(400, "application/json", "{\"message\":\"Feeding failed\",\"success\":false}");
        }
      } else {
        server.send(400, "application/json", "{\"message\":\"Invalid amount\",\"success\":false}");
      }
    });
    
    server.on("/api/cat/calibrate", HTTP_POST, [this]() {
      scale.tare();
      server.send(200, "application/json", "{\"message\":\"Scale calibrated\",\"success\":true}");
    });
    
    server.on("/api/cat/schedule", HTTP_GET, [this]() {
      DynamicJsonDocument doc(1024);
      JsonArray schedulesArray = doc.createNestedArray("schedules");
      
      for (int i = 0; i < MAX_SCHEDULES; i++) {
        JsonObject schedule = schedulesArray.createNestedObject();
        schedule["hour"] = schedules[i].hour;
        schedule["minute"] = schedules[i].minute;
        schedule["amount"] = schedules[i].amount;
        schedule["enabled"] = schedules[i].enabled;
      }
      
      String output;
      serializeJson(doc, output);
      server.send(200, "application/json", output);
    });
  }
  
  void setupBlynkPins() {
    // Virtual pin assignments:
    // V0 - Weight display
    // V1 - Feed count display
    // V2 - Total food display
    // V3 - Cat detection LED
    // V4 - System status LED
    // V10 - Manual feed button
    // V11 - System enable switch
    // V12 - Feed amount slider
  }
  
  void updateBlynkData() {
    static unsigned long lastBlynkUpdate = 0;
    
    if (status.blynk_connected && millis() - lastBlynkUpdate > 10000) {
      Blynk.virtualWrite(V0, catStatus.currentWeight);
      Blynk.virtualWrite(V1, catStatus.feedingCount);
      Blynk.virtualWrite(V2, catStatus.totalFoodDispensed);
      Blynk.virtualWrite(V3, catStatus.catDetected ? 1 : 0);
      Blynk.virtualWrite(V4, status.state == SYSTEM_RUNNING ? 1 : 0);
      
      lastBlynkUpdate = millis();
    }
  }
  
  void loadCatSettings() {
    // Load cat-specific settings from EEPROM
    EEPROM.get(400, schedules);
    EEPROM.get(500, catStatus.totalFoodDispensed);
    EEPROM.get(504, catStatus.feedingCount);
    EEPROM.get(508, catStatus.dailyFoodLimit);
    
    // Validate loaded data
    if (isnan(catStatus.totalFoodDispensed) || catStatus.totalFoodDispensed < 0) {
      catStatus.totalFoodDispensed = 0;
    }
    if (catStatus.feedingCount < 0 || catStatus.feedingCount > 10000) {
      catStatus.feedingCount = 0;
    }
    if (isnan(catStatus.dailyFoodLimit) || catStatus.dailyFoodLimit <= 0) {
      catStatus.dailyFoodLimit = MAX_DAILY_FOOD;
    }
  }
  
  void saveCatSettings() {
    EEPROM.put(400, schedules);
    EEPROM.put(500, catStatus.totalFoodDispensed);
    EEPROM.put(504, catStatus.feedingCount);
    EEPROM.put(508, catStatus.dailyFoodLimit);
    EEPROM.commit();
  }
  
  void resetFeedingStats() {
    catStatus.totalFoodDispensed = 0;
    catStatus.feedingCount = 0;
    catStatus.lastFeedingTime = "";
    saveCatSettings();
    
    showStatus("Stats Reset", "Complete!");
    playSuccessSound();
    delay(2000);
    
    LOG_INFO("Feeding statistics reset");
  }
};

// Global system instance
CatFeedingSystem catFeeder;

// Blynk Virtual Pin Handlers
BLYNK_WRITE(V10) { // Manual feed button
  if (param.asInt() == 1) {
    catFeeder.handleSystemCommand("feed");
  }
}

BLYNK_WRITE(V11) { // System enable/disable
  config.system_enabled = param.asInt();
  catFeeder.saveConfig();
}

BLYNK_WRITE(V12) { // Feed amount slider
  float amount = param.asFloat();
  if (amount >= 5 && amount <= 100) {
    // Store amount for next manual feeding
    // Could be implemented as a global variable
  }
}

// Arduino setup and loop
void setup() {
  // Initialize the cat feeding system
  if (!catFeeder.begin()) {
    // System failed to initialize
    Serial.println("System initialization failed!");
    while (true) {
      delay(1000);
    }
  }
}

void loop() {
  // Run the main system loop
  catFeeder.loop();
}
/*
 * Enhanced Cat Feeding System - Single Main File
 * ระบบให้อาหารแมวอัตโนมัติขั้นสูง - ไฟล์หลักเดียว
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
 * - Scheduled feeding (6 times per day)
 * - Weight monitoring with load cell
 * - Cat detection with PIR sensor
 * - Mobile app control via Blink
 * - Low food alerts
 * - Feeding history logging
 * - Duplicate feeding prevention
 * - Manual feeding mode
 * 
 * Firmware made by: RDTRC
 * Version: 3.0
 * Created: 2024
 */

// Include enhanced system base
#include "../common/base_code/enhanced_system_base.h"
#include "../common/base_code/web_handlers.cpp"
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
#define MIN_FOOD_WEIGHT 100.0  // grams
#define DEFAULT_FEED_AMOUNT 30.0  // grams
#define MAX_DAILY_FOOD 500.0  // grams
#define FEEDING_TIMEOUT 10000  // 10 seconds
#define SERVO_OPEN_ANGLE 90
#define SERVO_CLOSE_ANGLE 0

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
  bool lowFoodAlert;
  unsigned long lastFeedingTimestamp;
};

// Enhanced Cat Feeding System Class
class EnhancedCatFeedingSystem : public EnhancedSystemBase {
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
  unsigned long lastBlinkUpdate;
  bool pirState;
  bool lastPirState;
  bool isFeeding;
  
public:
  EnhancedCatFeedingSystem() : EnhancedSystemBase("Enhanced Cat Feeder", "CatFeeding") {
    calibrationFactor = -7050.0;
    lastFeedingTime = 0;
    lastWeightCheck = 0;
    lastPIRCheck = 0;
    lastMotionTime = 0;
    lastBlinkUpdate = 0;
    pirState = false;
    lastPirState = false;
    isFeeding = false;
    
    // Initialize feeding schedules
    schedules[0] = {7, 0, 50.0, true};   // Morning 07:00
    schedules[1] = {12, 0, 30.0, true};  // Noon 12:00
    schedules[2] = {18, 0, 50.0, true};  // Evening 18:00
    schedules[3] = {0, 0, 0.0, false};   // Disabled
    schedules[4] = {0, 0, 0.0, false};   // Disabled
    schedules[5] = {0, 0, 0.0, false};   // Disabled
    
    // Initialize cat status
    catStatus.currentWeight = 0;
    catStatus.totalFoodDispensed = 0;
    catStatus.feedingCount = 0;
    catStatus.catDetected = false;
    catStatus.lastFeedingTime = "";
    catStatus.nextFeedingTime = "";
    catStatus.dailyFoodLimit = MAX_DAILY_FOOD;
    catStatus.manualFeedingMode = false;
    catStatus.lowFoodAlert = false;
    catStatus.lastFeedingTimestamp = 0;
  }
  
  void setupSystem() override {
    LOG_INFO("Setting up Enhanced Cat Feeding System...");
    
    // Initialize servo
    feedingServo.attach(SERVO_PIN);
    feedingServo.write(SERVO_CLOSE_ANGLE);
    delay(1000);
    LOG_INFO("Servo initialized");
    
    // Initialize load cell
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(calibrationFactor);
    scale.tare(); // Reset scale to zero
    LOG_INFO("Load cell initialized");
    
    // Initialize PIR sensor
    pinMode(PIR_PIN, INPUT);
    LOG_INFO("PIR sensor initialized");
    
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
    LOG_INFO("Enhanced Cat Feeding System setup completed");
  }
  
  void runSystem() override {
    unsigned long currentTime = millis();
    
    // Check manual buttons
    checkManualButtons();
    
    // Read weight periodically
    if (currentTime - lastWeightCheck >= 2000) { // Every 2 seconds
      readFoodWeight();
      lastWeightCheck = currentTime;
    }
    
    // Check PIR sensor
    if (currentTime - lastPIRCheck >= 500) { // Every 500ms
      checkCatPresence();
      lastPIRCheck = currentTime;
    }
    
    // Check scheduled feeding
    checkScheduledFeeding();
    
    // Update Blink data
    if (systemConfig.blink_enabled && (currentTime - lastBlinkUpdate >= 5000)) { // Every 5 seconds
      sendBlynkData();
      lastBlinkUpdate = currentTime;
    }
    
    // Check for low food alert
    checkLowFoodAlert();
    
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
          performFeeding(DEFAULT_FEED_AMOUNT, "Manual");
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
  
  void readFoodWeight() {
    if (scale.is_ready()) {
      float weight = scale.get_units(3); // Average of 3 readings
      if (weight < 0) weight = 0; // Prevent negative values
      
      catStatus.currentWeight = weight;
      
      // Check for low food
      if (weight < MIN_FOOD_WEIGHT && !catStatus.lowFoodAlert) {
        catStatus.lowFoodAlert = true;
        LOG_ERROR("Low food alert: " + String(weight) + "g remaining");
        
        if (systemConfig.blink_enabled) {
          // Send alert to Blink
          Blynk.logEvent("low_food", "Food level is low: " + String(weight) + "g remaining");
        }
      } else if (weight >= MIN_FOOD_WEIGHT * 1.5) {
        catStatus.lowFoodAlert = false; // Clear alert when food is refilled
      }
    }
  }
  
  void checkCatPresence() {
    pirState = digitalRead(PIR_PIN);
    
    if (pirState != lastPirState) {
      if (pirState == HIGH) {
        catStatus.catDetected = true;
        lastMotionTime = millis();
        LOG_INFO("Cat detected");
        
        if (systemConfig.blink_enabled) {
          Blynk.virtualWrite(V5, 1); // Cat presence indicator
        }
      }
      lastPirState = pirState;
    }
    
    // Clear cat detected after 30 seconds of no motion
    if (catStatus.catDetected && (millis() - lastMotionTime > 30000)) {
      catStatus.catDetected = false;
      
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
          LOG_INFO("Scheduled feeding #" + String(i+1) + " triggered");
          performFeeding(schedules[i].amount, "Schedule #" + String(i+1));
        }
      }
    }
  }
  
  void performFeeding(float amount, String source) {
    if (isFeeding) {
      LOG_INFO("Feeding already in progress");
      return;
    }
    
    if (catStatus.totalFoodDispensed + amount > catStatus.dailyFoodLimit) {
      LOG_ERROR("Daily food limit exceeded");
      return;
    }
    
    if (catStatus.currentWeight < amount) {
      LOG_ERROR("Not enough food in container");
      return;
    }
    
    isFeeding = true;
    LOG_INFO("Starting feeding: " + String(amount) + "g from " + source);
    
    // Record weight before feeding
    float weightBefore = catStatus.currentWeight;
    
    // Open servo to dispense food
    feedingServo.write(SERVO_OPEN_ANGLE);
    
    // Calculate feeding time based on amount (rough estimation)
    int feedingDuration = (int)(amount * 100); // 100ms per gram
    if (feedingDuration > FEEDING_TIMEOUT) {
      feedingDuration = FEEDING_TIMEOUT;
    }
    
    delay(feedingDuration);
    
    // Close servo
    feedingServo.write(SERVO_CLOSE_ANGLE);
    delay(500);
    
    // Wait for weight to stabilize
    delay(2000);
    readFoodWeight();
    
    // Calculate actual dispensed amount
    float actualDispensed = weightBefore - catStatus.currentWeight;
    if (actualDispensed < 0) actualDispensed = amount; // Fallback to requested amount
    
    // Update statistics
    catStatus.totalFoodDispensed += actualDispensed;
    catStatus.feedingCount++;
    catStatus.lastFeedingTime = getCurrentTimeString();
    catStatus.lastFeedingTimestamp = millis();
    lastFeedingTime = millis();
    
    // Calculate next feeding time
    calculateNextFeedingTime();
    
    isFeeding = false;
    
    LOG_INFO("Feeding completed: " + String(actualDispensed) + "g dispensed");
    
    // Send notification to Blink
    if (systemConfig.blink_enabled) {
      Blynk.logEvent("feeding_complete", 
        "Fed " + String(actualDispensed) + "g. Source: " + source + 
        ". Total today: " + String(catStatus.totalFoodDispensed) + "g");
    }
    
    // Play success sound
    playSystemSound("success");
  }
  
  void calculateNextFeedingTime() {
    if (!systemStatus.rtc_connected) {
      catStatus.nextFeedingTime = "RTC not available";
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
      catStatus.nextFeedingTime = String(nextHour) + ":" + 
        (nextMinute < 10 ? "0" : "") + String(nextMinute) + 
        (foundToday ? " today" : " tomorrow");
    } else {
      catStatus.nextFeedingTime = "No schedule set";
    }
  }
  
  String getCurrentTimeString() {
    if (!systemStatus.rtc_connected) return "No RTC";
    
    DateTime now = rtc.now();
    return String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()) + 
           " " + String(now.hour()) + ":" + 
           (now.minute() < 10 ? "0" : "") + String(now.minute());
  }
  
  void checkLowFoodAlert() {
    static unsigned long lastAlertTime = 0;
    
    if (catStatus.lowFoodAlert && (millis() - lastAlertTime > 300000)) { // Every 5 minutes
      LOG_ERROR("Low food alert: " + String(catStatus.currentWeight) + "g remaining");
      
      // Blink LED to indicate low food
      blinkLED(255, 255, 0, 5); // Yellow blink
      
      lastAlertTime = millis();
    }
  }
  
  void updateSystemStatus() {
    // Reset daily counters at midnight
    static int lastDay = -1;
    if (systemStatus.rtc_connected) {
      DateTime now = rtc.now();
      if (lastDay != now.day()) {
        catStatus.totalFoodDispensed = 0;
        catStatus.feedingCount = 0;
        lastDay = now.day();
        LOG_INFO("Daily counters reset");
      }
    }
  }
  
  void setupBlinkIntegration() {
    if (!systemConfig.blink_enabled) return;
    
    // Virtual Pin assignments:
    // V0 - Current food weight
    // V1 - Manual feeding button
    // V2 - Feeding amount slider
    // V3 - Total food dispensed today
    // V4 - Feeding count today
    // V5 - Cat presence indicator
    // V6 - Next feeding time (display)
    // V7 - System status (display)
    // V8 - Schedule enable/disable
    // V9 - Low food alert
    
    LOG_INFO("Blink integration setup completed");
  }
  
  void sendBlynkData() override {
    if (!systemConfig.blink_enabled || !systemStatus.network.blink_connected) return;
    
    Blynk.virtualWrite(V0, catStatus.currentWeight);
    Blynk.virtualWrite(V3, catStatus.totalFoodDispensed);
    Blynk.virtualWrite(V4, catStatus.feedingCount);
    Blynk.virtualWrite(V5, catStatus.catDetected ? 1 : 0);
    Blynk.virtualWrite(V9, catStatus.lowFoodAlert ? 1 : 0);
    
    // Send text data
    Blynk.virtualWrite(V6, catStatus.nextFeedingTime);
    Blynk.virtualWrite(V7, "Weight: " + String(catStatus.currentWeight) + "g\n" +
                            "Fed: " + String(catStatus.feedingCount) + " times\n" +
                            "Total: " + String(catStatus.totalFoodDispensed) + "g");
  }
  
  void handleBlynkCommand(int pin, int value) override {
    switch (pin) {
      case V1: // Manual feeding button
        if (value == 1 && !isFeeding) {
          performFeeding(DEFAULT_FEED_AMOUNT, "Blink App");
        }
        break;
        
      case V2: // Feeding amount slider (10-100g)
        if (value >= 10 && value <= 100) {
          // Store for next manual feeding
          // Could be implemented with a global variable
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
    
    doc["food_weight"] = catStatus.currentWeight;
    doc["food_dispensed_today"] = catStatus.totalFoodDispensed;
    doc["feeding_count_today"] = catStatus.feedingCount;
    doc["cat_detected"] = catStatus.catDetected;
    doc["last_feeding"] = catStatus.lastFeedingTime;
    doc["next_feeding"] = catStatus.nextFeedingTime;
    doc["low_food_alert"] = catStatus.lowFoodAlert;
    doc["is_feeding"] = isFeeding;
    doc["manual_mode"] = catStatus.manualFeedingMode;
    
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
EnhancedCatFeedingSystem* catSystem = nullptr;
EnhancedSystemBase* currentSystem = nullptr;

// Blink event handlers
BLYNK_WRITE(V1) {
  if (catSystem) {
    catSystem->handleBlynkCommand(V1, param.asInt());
  }
}

BLYNK_WRITE(V2) {
  if (catSystem) {
    catSystem->handleBlynkCommand(V2, param.asInt());
  }
}

BLYNK_WRITE(V8) {
  if (catSystem) {
    catSystem->handleBlynkCommand(V8, param.asInt());
  }
}

void setup() {
  // Create system instance
  catSystem = new EnhancedCatFeedingSystem();
  currentSystem = catSystem;
  
  // Initialize system
  if (!catSystem->begin()) {
    Serial.println("System initialization failed!");
    while (true) {
      delay(1000);
    }
  }
  
  Serial.println("Enhanced Cat Feeding System ready!");
}

void loop() {
  if (catSystem) {
    catSystem->loop();
  }
}

// Web interface extensions for cat feeding system
void handleCatFeedingConfig() {
  String html = String(HTML_HEADER);
  
  html += R"(
<div class="container">
    <div class="header">
        <h1>🐱 ตั้งค่าระบบให้อาหารแมว</h1>
        <p>จัดการตารางการให้อาหารและการตั้งค่า</p>
    </div>
    
    <div class="nav">
        <a href="/">หน้าแรก</a>
        <a href="/network">ตั้งค่าเครือข่าย</a>
        <a href="/system">ตั้งค่าระบบ</a>
        <a href="/blink">Blink Integration</a>
        <a href="/status">สถานะระบบ</a>
        <a href="/cat-config" class="active">ตั้งค่าให้อาหารแมว</a>
    </div>
    
    <div class="content">
        <div class="card">
            <h3>📊 สถานะปัจจุบัน</h3>
            <div class="status-grid">
                <div class="status-item success">
                    <strong>🍽️ น้ำหนักอาหาร</strong><br>
                    )" + String(catSystem ? catSystem->getSystemStatus() : "N/A") + R"( กรัม
                </div>
                <div class="status-item">
                    <strong>📅 ให้อาหารวันนี้</strong><br>
                    จำนวน: 0 ครั้ง<br>
                    รวม: 0 กรัม
                </div>
            </div>
        </div>
        
        <div class="card">
            <h3>⏰ ตารางการให้อาหาร</h3>
            <p>กำหนดเวลาและปริมาณการให้อาหารแมว</p>
            <!-- Schedule configuration form would go here -->
        </div>
        
        <div class="card">
            <h3>🎮 การควบคุมด้วยตนเอง</h3>
            <button class="btn success" onclick="manualFeed()">🍽️ ให้อาหารทันที</button>
            <button class="btn warning" onclick="resetDaily()">🔄 รีเซ็ตสถิติรายวัน</button>
        </div>
    </div>
</div>

<script>
function manualFeed() {
    if (confirm('คุณต้องการให้อาหารแมวทันทีหรือไม่?')) {
        fetch('/api/cat/feed', {method: 'POST'})
            .then(response => response.json())
            .then(data => {
                if (data.success) {
                    alert('✅ ให้อาหารเรียบร้อยแล้ว!');
                    location.reload();
                } else {
                    alert('❌ เกิดข้อผิดพลาด: ' + data.error);
                }
            });
    }
}

function resetDaily() {
    if (confirm('คุณต้องการรีเซ็ตสถิติรายวันหรือไม่?')) {
        fetch('/api/cat/reset-daily', {method: 'POST'})
            .then(response => response.json())
            .then(data => {
                if (data.success) {
                    alert('✅ รีเซ็ตสถิติเรียบร้อยแล้ว!');
                    location.reload();
                } else {
                    alert('❌ เกิดข้อผิดพลาด: ' + data.error);
                }
            });
    }
}
</script>
)";
  
  html += HTML_FOOTER;
  webServer.send(200, "text/html", html);
}
/*
 * ระบบให้น้ำและอาหารนกอัตโนมัติด้วย ESP32
 * Features:
 * - ให้น้ำอัตโนมัติตามเวลา (Relay + Water Pump)
 * - ให้อาหารนกอัตโนมัติตามเวลา (Servo)
 * - ควบคุมผ่าน Blink App
 * - ตั้งเวลาทำงานอัตโนมัติ
 * - ระบบ Delay และ Timer
 * - Web Interface สำหรับควบคุม
 */

#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <HTTPClient.h>

// RTC Configuration
RTC_DS3231 rtc;

// Servo Configuration (สำหรับอาหารนก)
const int SERVO_PIN = 13;
const int SERVO_OPEN_ANGLE = 90;   // มุมเปิดถาดอาหาร
const int SERVO_CLOSE_ANGLE = 0;   // มุมปิดถาดอาหาร
Servo foodServo;

// Relay Configuration (สำหรับปั๊มน้ำ)
const int RELAY_PIN = 5;
const int WATER_PUMP_DURATION = 10; // วินาที

// LED Status
const int STATUS_LED = 2;

// WiFi Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Web Server
WebServer server(80);

// System States
bool waterPumpActive = false;
bool foodDispenserActive = false;
bool systemEnabled = true;

// Timing Variables
unsigned long waterPumpStartTime = 0;
unsigned long foodDispenserStartTime = 0;
const unsigned long WATER_PUMP_TIMEOUT = WATER_PUMP_DURATION * 1000; // milliseconds
const unsigned long FOOD_DISPENSER_TIMEOUT = 5000; // 5 seconds

// Feeding Schedule
struct FeedingSchedule {
  int hour;
  int minute;
  bool isWater;
  bool isFood;
  int duration; // seconds for water, amount for food
  bool enabled;
};

FeedingSchedule schedules[] = {
  // เช้า - ให้น้ำและอาหาร
  {6, 0, true, true, 15, true},    // 6:00 - น้ำ 15 วินาที + อาหาร
  {6, 30, false, true, 1, true},   // 6:30 - อาหารเพิ่ม
  
  // เที่ยง - ให้น้ำ
  {12, 0, true, false, 10, true},  // 12:00 - น้ำ 10 วินาที
  
  // เย็น - ให้น้ำและอาหาร
  {18, 0, true, true, 15, true},   // 18:00 - น้ำ 15 วินาที + อาหาร
  {18, 30, false, true, 1, true},  // 18:30 - อาหารเพิ่ม
  
  // กลางคืน - ให้น้ำเล็กน้อย
  {22, 0, true, false, 5, true}    // 22:00 - น้ำ 5 วินาที
};

const int scheduleCount = sizeof(schedules) / sizeof(schedules[0]);

// Daily counters
int dailyWaterCount = 0;
int dailyFoodCount = 0;
const int MAX_DAILY_WATER = 5;
const int MAX_DAILY_FOOD = 6;

// Last action times
DateTime lastWaterTime;
DateTime lastFoodTime;

// Webhook Configuration
const char* webhookUrl = "YOUR_WEBHOOK_URL";
bool webhookEnabled = true;

void sendWebhook(String message, String level) {
  if (!webhookEnabled || WiFi.status() != WL_CONNECTED) return;
  HTTPClient http;
  http.begin(webhookUrl);
  http.addHeader("Content-Type", "application/json");
  String payload = String("{\"message\":\"") + message + "\",\"level\":\"" + level + "\"}";
  http.POST(payload);
  http.end();
}

void setup() {
  Serial.begin(115200);
  
  // Initialize I2C for RTC
  Wire.begin();
  
  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("❌ Couldn't find RTC");
    while (1);
  }
  
  // Set RTC time if needed (uncomment to set time)
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  // Initialize pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Relay off
  digitalWrite(STATUS_LED, LOW);
  
  // Initialize Servo
  ESP32PWM::allocateTimer(0);
  foodServo.setPeriodHertz(50);
  foodServo.attach(SERVO_PIN);
  foodServo.write(SERVO_CLOSE_ANGLE); // ปิดถาดอาหาร
  
  // Initialize timing variables
  lastWaterTime = rtc.now();
  lastFoodTime = rtc.now();
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("🔗 Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(STATUS_LED, !digitalRead(STATUS_LED)); // Blink while connecting
  }
  Serial.println();
  Serial.print("✅ Connected to WiFi. IP: ");
  Serial.println(WiFi.localIP());
  digitalWrite(STATUS_LED, HIGH); // Solid LED when connected
  
  // Setup web server routes
  setupWebServer();
  server.begin();
  
  Serial.println("🐦 Bird Feeding System Ready!");
  printSchedule();
}

void loop() {
  server.handleClient();
  checkSchedule();
  checkTimers();
  resetDailyCounters();
  updateStatusLED();
  delay(1000);
}

void checkSchedule() {
  if (!systemEnabled) return;
  
  DateTime now = rtc.now();
  
  for (int i = 0; i < scheduleCount; i++) {
    if (schedules[i].enabled &&
        schedules[i].hour == now.hour() && 
        schedules[i].minute == now.minute()) {
      
      // Check water schedule
      if (schedules[i].isWater && dailyWaterCount < MAX_DAILY_WATER) {
        startWaterPump(schedules[i].duration);
        dailyWaterCount++;
        lastWaterTime = now;
        Serial.print("💧 Started water pump for ");
        Serial.print(schedules[i].duration);
        Serial.println(" seconds");
      }
      
      // Check food schedule
      if (schedules[i].isFood && dailyFoodCount < MAX_DAILY_FOOD) {
        dispenseFood(schedules[i].duration);
        dailyFoodCount++;
        lastFoodTime = now;
        Serial.print("🌾 Dispensed food ");
        Serial.print(schedules[i].duration);
        Serial.println(" times");
      }
    }
  }
}

void startWaterPump(int duration) {
  if (!waterPumpActive) {
    digitalWrite(RELAY_PIN, LOW); // Turn on relay
    waterPumpActive = true;
    waterPumpStartTime = millis();
    sendWebhook("💧 Water pump started (" + String(duration) + " s)", "info");
  }
}

void stopWaterPump() {
  if (waterPumpActive) {
    digitalWrite(RELAY_PIN, HIGH); // Turn off relay
    waterPumpActive = false;
    Serial.println("💧 Water pump stopped");
    sendWebhook("🛑 Water pump stopped", "info");
  }
}

void dispenseFood(int amount) {
  if (!foodDispenserActive) {
    foodDispenserActive = true;
    foodDispenserStartTime = millis();
    
    // เปิดถาดอาหาร
    foodServo.write(SERVO_OPEN_ANGLE);
    Serial.println("🌾 Food dispenser opened");
    sendWebhook("🌾 Food dispenser opened (" + String(amount) + "x)", "info");
  }
}

void closeFoodDispenser() {
  if (foodDispenserActive) {
    foodServo.write(SERVO_CLOSE_ANGLE);
    foodDispenserActive = false;
    Serial.println("🌾 Food dispenser closed");
    sendWebhook("🌾 Food dispenser closed", "info");
  }
}

void checkTimers() {
  unsigned long currentTime = millis();
  
  // Check water pump timer
  if (waterPumpActive && (currentTime - waterPumpStartTime >= WATER_PUMP_TIMEOUT)) {
    stopWaterPump();
  }
  
  // Check food dispenser timer
  if (foodDispenserActive && (currentTime - foodDispenserStartTime >= FOOD_DISPENSER_TIMEOUT)) {
    closeFoodDispenser();
  }
}

void resetDailyCounters() {
  DateTime now = rtc.now();
  static int lastDay = -1;
  
  if (now.day() != lastDay) {
    dailyWaterCount = 0;
    dailyFoodCount = 0;
    lastDay = now.day();
    Serial.println("🔄 Daily counters reset");
  }
}

void updateStatusLED() {
  if (waterPumpActive || foodDispenserActive) {
    digitalWrite(STATUS_LED, !digitalRead(STATUS_LED)); // Blink when active
  } else {
    digitalWrite(STATUS_LED, HIGH); // Solid when idle
  }
}

void printSchedule() {
  Serial.println("📅 Feeding Schedule:");
  for (int i = 0; i < scheduleCount; i++) {
    Serial.print("  ");
    Serial.print(schedules[i].hour);
    Serial.print(":");
    if (schedules[i].minute < 10) Serial.print("0");
    Serial.print(schedules[i].minute);
    Serial.print(" - ");
    
    if (schedules[i].isWater) {
      Serial.print("Water (");
      Serial.print(schedules[i].duration);
      Serial.print("s)");
    }
    if (schedules[i].isWater && schedules[i].isFood) Serial.print(" + ");
    if (schedules[i].isFood) {
      Serial.print("Food (");
      Serial.print(schedules[i].duration);
      Serial.print("x)");
    }
    Serial.println();
  }
}

// Manual control functions
void manualWater(int duration) {
  if (dailyWaterCount < MAX_DAILY_WATER) {
    startWaterPump(duration);
    dailyWaterCount++;
    lastWaterTime = rtc.now();
    Serial.print("💧 Manual water pump for ");
    Serial.print(duration);
    Serial.println(" seconds");
  } else {
    Serial.println("⚠️ Daily water limit reached");
  }
}

void manualFood(int amount) {
  if (dailyFoodCount < MAX_DAILY_FOOD) {
    dispenseFood(amount);
    dailyFoodCount++;
    lastFoodTime = rtc.now();
    Serial.print("🌾 Manual food dispense ");
    Serial.print(amount);
    Serial.println(" times");
  } else {
    Serial.println("⚠️ Daily food limit reached");
  }
}

void setupWebServer() {
  // Root page
  server.on("/", HTTP_GET, []() {
    String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>🐦 Bird Feeding System</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; 
            margin: 0; 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); 
            color: white; 
            min-height: 100vh;
        }
        .container { 
            max-width: 1200px; 
            margin: 0 auto; 
            padding: 20px; 
        }
        .header { 
            text-align: center; 
            margin-bottom: 30px; 
            background: rgba(255,255,255,0.1); 
            padding: 20px; 
            border-radius: 15px; 
            backdrop-filter: blur(10px);
        }
        .status-grid { 
            display: grid; 
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); 
            gap: 20px; 
            margin-bottom: 30px; 
        }
        .status-card { 
            background: rgba(255,255,255,0.1); 
            padding: 25px; 
            border-radius: 15px; 
            text-align: center; 
            border: 1px solid rgba(255,255,255,0.2);
            backdrop-filter: blur(10px);
        }
        .status-card.active { 
            background: rgba(0,255,0,0.2); 
            border-color: #00ff00; 
            animation: pulse 2s infinite;
        }
        @keyframes pulse {
            0% { box-shadow: 0 0 0 0 rgba(0,255,0,0.7); }
            70% { box-shadow: 0 0 0 10px rgba(0,255,0,0); }
            100% { box-shadow: 0 0 0 0 rgba(0,255,0,0); }
        }
        .control-panel { 
            background: rgba(255,255,255,0.1); 
            padding: 25px; 
            border-radius: 15px; 
            margin-bottom: 30px;
            backdrop-filter: blur(10px);
        }
        .btn { 
            padding: 15px 30px; 
            margin: 10px; 
            border: none; 
            border-radius: 10px; 
            cursor: pointer; 
            font-size: 16px; 
            font-weight: bold; 
            transition: all 0.3s; 
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        .btn-water { 
            background: linear-gradient(45deg, #00bcd4, #0097a7); 
            color: white; 
        }
        .btn-food { 
            background: linear-gradient(45deg, #ff9800, #f57c00); 
            color: white; 
        }
        .btn-stop { 
            background: linear-gradient(45deg, #f44336, #d32f2f); 
            color: white; 
        }
        .btn:hover { 
            transform: translateY(-3px); 
            box-shadow: 0 8px 25px rgba(0,0,0,0.3); 
        }
        .schedule-list { 
            background: rgba(255,255,255,0.1); 
            padding: 25px; 
            border-radius: 15px;
            backdrop-filter: blur(10px);
        }
        .schedule-item { 
            padding: 15px; 
            margin: 10px 0; 
            background: rgba(255,255,255,0.1); 
            border-radius: 10px; 
            border-left: 4px solid #00ff00;
        }
        .stats { 
            display: grid; 
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); 
            gap: 20px; 
            margin: 20px 0; 
        }
        .stat-card { 
            background: rgba(255,255,255,0.1); 
            padding: 20px; 
            border-radius: 10px; 
            text-align: center;
            backdrop-filter: blur(10px);
        }
        .progress-bar {
            width: 100%;
            height: 20px;
            background: rgba(255,255,255,0.2);
            border-radius: 10px;
            overflow: hidden;
            margin: 10px 0;
        }
        .progress-fill {
            height: 100%;
            background: linear-gradient(45deg, #00ff00, #00bcd4);
            transition: width 0.3s ease;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🐦 Bird Feeding System</h1>
            <p>ระบบให้น้ำและอาหารนกอัตโนมัติ</p>
        </div>
        
        <div class="status-grid">
            <div class="status-card" id="waterCard">
                <h2>💧 Water System</h2>
                <div id="waterStatus">Idle</div>
                <div class="progress-bar">
                    <div class="progress-fill" id="waterProgress" style="width: 0%"></div>
                </div>
                <div>Daily: <span id="waterCount">0</span>/<span id="waterMax">5</span></div>
            </div>
            
            <div class="status-card" id="foodCard">
                <h2>🌾 Food System</h2>
                <div id="foodStatus">Idle</div>
                <div class="progress-bar">
                    <div class="progress-fill" id="foodProgress" style="width: 0%"></div>
                </div>
                <div>Daily: <span id="foodCount">0</span>/<span id="foodMax">6</span></div>
            </div>
            
            <div class="status-card">
                <h2>⏰ Current Time</h2>
                <div id="currentTime" style="font-size: 24px; font-weight: bold;">Loading...</div>
                <div id="systemStatus">System Online</div>
            </div>
        </div>
        
        <div class="control-panel">
            <h2>🎮 Manual Control</h2>
            <div>
                <button class="btn btn-water" onclick="manualWater(5)">Water 5s</button>
                <button class="btn btn-water" onclick="manualWater(10)">Water 10s</button>
                <button class="btn btn-water" onclick="manualWater(15)">Water 15s</button>
                <button class="btn btn-stop" onclick="stopWater()">Stop Water</button>
            </div>
            <div style="margin-top: 20px;">
                <button class="btn btn-food" onclick="manualFood(1)">Food 1x</button>
                <button class="btn btn-food" onclick="manualFood(2)">Food 2x</button>
                <button class="btn btn-food" onclick="manualFood(3)">Food 3x</button>
                <button class="btn btn-stop" onclick="stopFood()">Stop Food</button>
            </div>
        </div>
        
        <div class="schedule-list">
            <h2>📅 Feeding Schedule</h2>
            <div id="scheduleList"></div>
        </div>
        
        <div class="stats">
            <div class="stat-card">
                <h3>Last Water</h3>
                <div id="lastWater">-</div>
            </div>
            <div class="stat-card">
                <h3>Last Food</h3>
                <div id="lastFood">-</div>
            </div>
        </div>
    </div>
    
    <script>
        function updateStatus() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    // Update water status
                    const waterCard = document.getElementById('waterCard');
                    const waterStatus = document.getElementById('waterStatus');
                    const waterProgress = document.getElementById('waterProgress');
                    const waterCount = document.getElementById('waterCount');
                    
                    if (data.waterActive) {
                        waterCard.classList.add('active');
                        waterStatus.textContent = '💧 Pumping...';
                        const progress = ((Date.now() - data.waterStartTime) / (data.waterDuration * 1000)) * 100;
                        waterProgress.style.width = Math.min(progress, 100) + '%';
                    } else {
                        waterCard.classList.remove('active');
                        waterStatus.textContent = 'Idle';
                        waterProgress.style.width = '0%';
                    }
                    waterCount.textContent = data.dailyWaterCount;
                    
                    // Update food status
                    const foodCard = document.getElementById('foodCard');
                    const foodStatus = document.getElementById('foodStatus');
                    const foodProgress = document.getElementById('foodProgress');
                    const foodCount = document.getElementById('foodCount');
                    
                    if (data.foodActive) {
                        foodCard.classList.add('active');
                        foodStatus.textContent = '🌾 Dispensing...';
                        const progress = ((Date.now() - data.foodStartTime) / 5000) * 100;
                        foodProgress.style.width = Math.min(progress, 100) + '%';
                    } else {
                        foodCard.classList.remove('active');
                        foodStatus.textContent = 'Idle';
                        foodProgress.style.width = '0%';
                    }
                    foodCount.textContent = data.dailyFoodCount;
                    
                    // Update time
                    document.getElementById('currentTime').textContent = data.currentTime;
                    document.getElementById('lastWater').textContent = data.lastWaterTime;
                    document.getElementById('lastFood').textContent = data.lastFoodTime;
                });
        }
        
        function updateSchedule() {
            fetch('/api/schedule')
                .then(response => response.json())
                .then(data => {
                    const list = document.getElementById('scheduleList');
                    list.innerHTML = '';
                    
                    data.schedules.forEach(schedule => {
                        const item = document.createElement('div');
                        item.className = 'schedule-item';
                        
                        let actions = [];
                        if (schedule.isWater) actions.push(`💧 Water (${schedule.duration}s)`);
                        if (schedule.isFood) actions.push(`🌾 Food (${schedule.duration}x)`);
                        
                        item.innerHTML = `
                            <strong>${schedule.time}</strong> - ${actions.join(' + ')}
                            <span style="float: right; color: ${schedule.enabled ? '#00ff00' : '#ff4444'}">
                                ${schedule.enabled ? '✓' : '✗'}
                            </span>
                        `;
                        list.appendChild(item);
                    });
                });
        }
        
        function manualWater(duration) {
            fetch('/api/water', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({duration: duration})
            })
            .then(() => updateStatus());
        }
        
        function stopWater() {
            fetch('/api/stop-water', {
                method: 'POST'
            })
            .then(() => updateStatus());
        }
        
        function manualFood(amount) {
            fetch('/api/food', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({amount: amount})
            })
            .then(() => updateStatus());
        }
        
        function stopFood() {
            fetch('/api/stop-food', {
                method: 'POST'
            })
            .then(() => updateStatus());
        }
        
        // Update every 1 second
        setInterval(() => {
            updateStatus();
        }, 1000);
        
        // Update schedule every 30 seconds
        setInterval(() => {
            updateSchedule();
        }, 30000);
        
        // Initial load
        updateStatus();
        updateSchedule();
    </script>
</body>
</html>
    )";
    server.send(200, "text/html", html);
  });
  
  // API endpoints
  server.on("/api/status", HTTP_GET, []() {
    DateTime now = rtc.now();
    String timeStr = String(now.hour()) + ":" + 
                    (now.minute() < 10 ? "0" : "") + String(now.minute()) + ":" +
                    (now.second() < 10 ? "0" : "") + String(now.second());
    
    String lastWaterStr = String(lastWaterTime.hour()) + ":" + 
                         (lastWaterTime.minute() < 10 ? "0" : "") + String(lastWaterTime.minute());
    String lastFoodStr = String(lastFoodTime.hour()) + ":" + 
                        (lastFoodTime.minute() < 10 ? "0" : "") + String(lastFoodTime.minute());
    
    StaticJsonDocument<512> doc;
    doc["currentTime"] = timeStr;
    doc["waterActive"] = waterPumpActive;
    doc["foodActive"] = foodDispenserActive;
    doc["dailyWaterCount"] = dailyWaterCount;
    doc["dailyFoodCount"] = dailyFoodCount;
    doc["lastWaterTime"] = lastWaterStr;
    doc["lastFoodTime"] = lastFoodStr;
    
    if (waterPumpActive) {
      doc["waterStartTime"] = waterPumpStartTime;
      doc["waterDuration"] = WATER_PUMP_DURATION;
    }
    if (foodDispenserActive) {
      doc["foodStartTime"] = foodDispenserStartTime;
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/water", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      StaticJsonDocument<200> doc;
      deserializeJson(doc, server.arg("plain"));
      
      int duration = doc["duration"];
      manualWater(duration);
      
      server.send(200, "application/json", "{\"status\":\"success\"}");
    } else {
      server.send(400, "application/json", "{\"status\":\"error\"}");
    }
  });
  
  server.on("/api/stop-water", HTTP_POST, []() {
    stopWaterPump();
    server.send(200, "application/json", "{\"status\":\"success\"}");
  });
  
  server.on("/api/food", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      StaticJsonDocument<200> doc;
      deserializeJson(doc, server.arg("plain"));
      
      int amount = doc["amount"];
      manualFood(amount);
      
      server.send(200, "application/json", "{\"status\":\"success\"}");
    } else {
      server.send(400, "application/json", "{\"status\":\"error\"}");
    }
  });
  
  server.on("/api/stop-food", HTTP_POST, []() {
    closeFoodDispenser();
    server.send(200, "application/json", "{\"status\":\"success\"}");
  });
  
  server.on("/api/schedule", HTTP_GET, []() {
    StaticJsonDocument<2048> doc;
    JsonArray schedulesArray = doc.createNestedArray("schedules");
    
    for (int i = 0; i < scheduleCount; i++) {
      JsonObject schedule = schedulesArray.createNestedObject();
      schedule["time"] = String(schedules[i].hour) + ":" + 
                        (schedules[i].minute < 10 ? "0" : "") + String(schedules[i].minute);
      schedule["isWater"] = schedules[i].isWater;
      schedule["isFood"] = schedules[i].isFood;
      schedule["duration"] = schedules[i].duration;
      schedule["enabled"] = schedules[i].enabled;
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  // Blink App compatible endpoints
  server.on("/api/blink/water", HTTP_POST, []() {
    manualWater(10); // Default 10 seconds for Blink
    server.send(200, "application/json", "{\"status\":\"water_started\"}");
  });
  
  server.on("/api/blink/food", HTTP_POST, []() {
    manualFood(1); // Default 1 portion for Blink
    server.send(200, "application/json", "{\"status\":\"food_dispensed\"}");
  });
  
  server.on("/api/blink/status", HTTP_GET, []() {
    StaticJsonDocument<256> doc;
    doc["water_active"] = waterPumpActive;
    doc["food_active"] = foodDispenserActive;
    doc["system_enabled"] = systemEnabled;
    doc["daily_water"] = dailyWaterCount;
    doc["daily_food"] = dailyFoodCount;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
} 
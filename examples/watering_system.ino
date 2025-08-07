/*
 * ระบบรดน้ำอัตโนมัติด้วย ESP32 + RTC + Relay + Blink Camera
 * สำหรับการปลูกพืชในบ้าน
 * 
 * Features:
 * - รดน้ำอัตโนมัติตามเวลา
 * - ควบคุมผ่านเว็บไซต์
 * - เชื่อมต่อ Blink Camera สำหรับการติดตาม
 * - ตรวจสอบความชื้นดิน (optional)
 * - ป้องกันการรดน้ำเกิน
 * - ระบบแจ้งเตือนผ่าน Webhook
 */

#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

// RTC Configuration
RTC_DS3231 rtc;

// Relay Configuration
const int RELAY_COUNT = 4;
const int relayPins[RELAY_COUNT] = {5, 18, 19, 21};

// Watering System Configuration
const int WATERING_DURATION = 30; // นาที
const int MAX_WATERING_PER_DAY = 3; // ครั้งต่อวัน
int wateringCount[RELAY_COUNT] = {0, 0, 0, 0};
DateTime lastWatering[RELAY_COUNT];

// WiFi Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Blink Camera Configuration
const char* blinkEmail = "YOUR_BLINK_EMAIL";
const char* blinkPassword = "YOUR_BLINK_PASSWORD";
const char* blinkAccountId = "YOUR_BLINK_ACCOUNT_ID";
const char* blinkNetworkId = "YOUR_BLINK_NETWORK_ID";
const char* blinkCameraId = "YOUR_BLINK_CAMERA_ID";

// Blink API endpoints
const char* blinkAuthUrl = "https://rest-prod.immedia-semi.com/api/v5/account/login";
const char* blinkArmUrl = "https://rest-prod.immedia-semi.com/api/v1/networks/";
const char* blinkDisarmUrl = "https://rest-prod.immedia-semi.com/api/v1/networks/";

// Webhook Configuration
const char* webhookUrl = "YOUR_WEBHOOK_URL";
bool webhookEnabled = true;

// Web Server
WebServer server(80);

// Blink authentication
String blinkAuthToken = "";
unsigned long lastBlinkAuth = 0;
const unsigned long BLINK_AUTH_INTERVAL = 3600000; // 1 hour

// Watering Schedule for different plants
struct WateringSchedule {
  int hour;
  int minute;
  int relayIndex;
  int duration; // minutes
  bool enabled;
};

WateringSchedule wateringSchedules[] = {
  // Morning watering - 6:00 AM
  {6, 0, 0, 15, true},   // Plant 1 - 15 minutes
  {6, 5, 1, 20, true},   // Plant 2 - 20 minutes
  {6, 10, 2, 10, true},  // Plant 3 - 10 minutes
  {6, 15, 3, 25, true},  // Plant 4 - 25 minutes
  
  // Evening watering - 6:00 PM
  {18, 0, 0, 10, true},  // Plant 1 - 10 minutes
  {18, 5, 1, 15, true},  // Plant 2 - 15 minutes
  {18, 10, 2, 8, true},  // Plant 3 - 8 minutes
  {18, 15, 3, 20, true}  // Plant 4 - 20 minutes
};

const int scheduleCount = sizeof(wateringSchedules) / sizeof(wateringSchedules[0]);

// Relay states and timing
bool relayStates[RELAY_COUNT] = {false, false, false, false};
DateTime relayStartTime[RELAY_COUNT];
bool relayTiming[RELAY_COUNT] = {false, false, false, false};

void setup() {
  Serial.begin(115200);
  
  // Initialize I2C for RTC
  Wire.begin();
  
  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  // Set RTC time if needed (uncomment to set time)
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  // Initialize relay pins
  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // Relay off
    lastWatering[i] = rtc.now();
  }
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to WiFi. IP: ");
  Serial.println(WiFi.localIP());
  
  // Setup web server routes
  setupWebServer();
  server.begin();
  
  // Initialize Blink camera
  authenticateBlink();
  
  Serial.println("🌱 Smart Watering System Ready with Blink Camera!");
  sendWebhook("🌱 Basic Watering System started", "info");
  printSchedule();
}

void loop() {
  server.handleClient();
  checkWateringSchedule();
  checkRelayTiming();
  resetDailyCounters();
  checkBlinkConnection();
  delay(1000);
}

void checkWateringSchedule() {
  DateTime now = rtc.now();
  
  for (int i = 0; i < scheduleCount; i++) {
    if (wateringSchedules[i].enabled &&
        wateringSchedules[i].hour == now.hour() && 
        wateringSchedules[i].minute == now.minute()) {
      
      int relayIndex = wateringSchedules[i].relayIndex;
      int duration = wateringSchedules[i].duration;
      
      // Check if we haven't exceeded daily limit
      if (wateringCount[relayIndex] < MAX_WATERING_PER_DAY) {
        startWatering(relayIndex, duration);
        wateringCount[relayIndex]++;
        lastWatering[relayIndex] = now;
        
        Serial.print("💧 Started watering Plant ");
        Serial.print(relayIndex + 1);
        Serial.print(" for ");
        Serial.print(duration);
        Serial.println(" minutes");
      } else {
        Serial.print("⚠️ Daily watering limit reached for Plant ");
        Serial.println(relayIndex + 1);
      }
    }
  }
}

void startWatering(int relayIndex, int duration) {
  if (relayIndex >= 0 && relayIndex < RELAY_COUNT) {
    // Arm Blink camera for monitoring
    armBlinkCamera();
    
    digitalWrite(relayPins[relayIndex], LOW); // Turn on relay
    relayStates[relayIndex] = true;
    relayStartTime[relayIndex] = rtc.now();
    relayTiming[relayIndex] = true;
    
    Serial.println("💧 Started watering zone " + String(relayIndex + 1) + " for " + String(duration) + " minutes");
    sendWebhook("💧 Zone " + String(relayIndex + 1) + " watering started (" + String(duration) + " min)", "info");
  }
}

void checkRelayTiming() {
  DateTime now = rtc.now();
  
  for (int i = 0; i < RELAY_COUNT; i++) {
    if (relayTiming[i]) {
      // Calculate elapsed minutes
      int elapsedMinutes = (now.unixtime() - relayStartTime[i].unixtime()) / 60;
      
      // Find the scheduled duration for this relay
      int scheduledDuration = 0;
      for (int j = 0; j < scheduleCount; j++) {
        if (wateringSchedules[j].relayIndex == i) {
          scheduledDuration = wateringSchedules[j].duration;
          break;
        }
      }
      
      if (elapsedMinutes >= scheduledDuration) {
        stopWatering(i);
        Serial.print("💧 Stopped watering Plant ");
        Serial.println(i + 1);
      }
    }
  }
}

void stopWatering(int relayIndex) {
  if (relayIndex >= 0 && relayIndex < RELAY_COUNT) {
    digitalWrite(relayPins[relayIndex], HIGH); // Turn off relay
    relayStates[relayIndex] = false;
    relayTiming[relayIndex] = false;
    
    // Disarm Blink camera
    disarmBlinkCamera();
    
    Serial.println("🛑 Stopped watering zone " + String(relayIndex + 1));
    sendWebhook("🛑 Zone " + String(relayIndex + 1) + " watering completed", "info");
  }
}

void resetDailyCounters() {
  DateTime now = rtc.now();
  static int lastDay = -1;
  
  if (now.day() != lastDay) {
    for (int i = 0; i < RELAY_COUNT; i++) {
      wateringCount[i] = 0;
    }
    lastDay = now.day();
    Serial.println("🔄 Daily watering counters reset");
  }
}

void printSchedule() {
  Serial.println("📅 Watering Schedule:");
  for (int i = 0; i < scheduleCount; i++) {
    Serial.print("  ");
    Serial.print(wateringSchedules[i].hour);
    Serial.print(":");
    if (wateringSchedules[i].minute < 10) Serial.print("0");
    Serial.print(wateringSchedules[i].minute);
    Serial.print(" - Plant ");
    Serial.print(wateringSchedules[i].relayIndex + 1);
    Serial.print(" (");
    Serial.print(wateringSchedules[i].duration);
    Serial.println(" min)");
  }
}

void setupWebServer() {
  // Root page
  server.on("/", HTTP_GET, []() {
    String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>🌱 Smart Watering System</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; }
        .container { max-width: 1000px; margin: 0 auto; background: rgba(255,255,255,0.1); padding: 30px; border-radius: 15px; backdrop-filter: blur(10px); }
        .plant-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; margin: 30px 0; }
        .plant-card { background: rgba(255,255,255,0.2); padding: 20px; border-radius: 10px; text-align: center; border: 1px solid rgba(255,255,255,0.3); }
        .plant-card.watering { background: rgba(0,255,0,0.3); border-color: #00ff00; }
        .plant-card.idle { background: rgba(255,255,255,0.2); }
        .btn { padding: 12px 24px; margin: 8px; border: none; border-radius: 8px; cursor: pointer; font-size: 16px; font-weight: bold; transition: all 0.3s; }
        .btn-water { background: #00ff00; color: #000; }
        .btn-stop { background: #ff4444; color: white; }
        .btn:hover { transform: translateY(-2px); box-shadow: 0 5px 15px rgba(0,0,0,0.3); }
        .status { font-size: 18px; margin: 10px 0; }
        .schedule-list { background: rgba(255,255,255,0.1); padding: 20px; border-radius: 10px; margin: 20px 0; }
        .schedule-item { padding: 10px; margin: 5px 0; background: rgba(255,255,255,0.1); border-radius: 5px; }
        .stats { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; margin: 20px 0; }
        .stat-card { background: rgba(255,255,255,0.2); padding: 15px; border-radius: 8px; text-align: center; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🌱 Smart Watering System</h1>
        
        <div class="stats">
            <div class="stat-card">
                <h3>Current Time</h3>
                <div id="currentTime" style="font-size: 24px; font-weight: bold;">Loading...</div>
            </div>
            <div class="stat-card">
                <h3>System Status</h3>
                <div id="systemStatus">Online</div>
            </div>
        </div>
        
        <h2>🌿 Plant Control</h2>
        <div class="plant-grid" id="plantGrid">
        </div>
        
        <h2>📅 Watering Schedule</h2>
        <div class="schedule-list" id="scheduleList">
        </div>
        
        <h2>📊 Daily Statistics</h2>
        <div class="stats" id="statsGrid">
        </div>
    </div>
    
    <script>
        function updateTime() {
            fetch('/api/time')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('currentTime').textContent = data.time;
                });
        }
        
        function updatePlants() {
            fetch('/api/plants')
                .then(response => response.json())
                .then(data => {
                    const grid = document.getElementById('plantGrid');
                    grid.innerHTML = '';
                    
                    data.plants.forEach((plant, index) => {
                        const card = document.createElement('div');
                        card.className = `plant-card ${plant.watering ? 'watering' : 'idle'}`;
                        card.innerHTML = `
                            <h3>🌿 Plant ${index + 1}</h3>
                            <div class="status">
                                Status: <strong>${plant.watering ? '💧 Watering' : '🌱 Idle'}</strong>
                            </div>
                            <div>Daily Count: ${plant.dailyCount}/3</div>
                            <div>Last Watered: ${plant.lastWatered}</div>
                            <button class="btn btn-water" onclick="waterPlant(${index}, 5)">Water 5min</button>
                            <button class="btn btn-water" onclick="waterPlant(${index}, 10)">Water 10min</button>
                            <button class="btn btn-stop" onclick="stopWatering(${index})">Stop</button>
                        `;
                        grid.appendChild(card);
                    });
                });
        }
        
        function waterPlant(index, duration) {
            fetch('/api/water', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({index: index, duration: duration})
            })
            .then(() => updatePlants());
        }
        
        function stopWatering(index) {
            fetch('/api/stop', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({index: index})
            })
            .then(() => updatePlants());
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
                        item.innerHTML = `
                            <strong>${schedule.time}</strong> - Plant ${schedule.plantIndex + 1} 
                            <span style="color: #00ff00">(${schedule.duration} min)</span>
                        `;
                        list.appendChild(item);
                    });
                });
        }
        
        function updateStats() {
            fetch('/api/stats')
                .then(response => response.json())
                .then(data => {
                    const grid = document.getElementById('statsGrid');
                    grid.innerHTML = '';
                    
                    data.stats.forEach((stat, index) => {
                        const card = document.createElement('div');
                        card.className = 'stat-card';
                        card.innerHTML = `
                            <h3>Plant ${index + 1}</h3>
                            <div>Today: ${stat.today} times</div>
                            <div>Last: ${stat.lastWatered}</div>
                        `;
                        grid.appendChild(card);
                    });
                });
        }
        
        // Update every 2 seconds
        setInterval(() => {
            updateTime();
            updatePlants();
            updateSchedule();
            updateStats();
        }, 2000);
        
        // Initial load
        updateTime();
        updatePlants();
        updateSchedule();
        updateStats();
    </script>
</body>
</html>
    )";
    server.send(200, "text/html", html);
  });
  
  // API endpoints
  server.on("/api/time", HTTP_GET, []() {
    DateTime now = rtc.now();
    String timeStr = String(now.hour()) + ":" + 
                    (now.minute() < 10 ? "0" : "") + String(now.minute()) + ":" +
                    (now.second() < 10 ? "0" : "") + String(now.second());
    
    StaticJsonDocument<200> doc;
    doc["time"] = timeStr;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/plants", HTTP_GET, []() {
    StaticJsonDocument<2048> doc;
    JsonArray plants = doc.createNestedArray("plants");
    
    for (int i = 0; i < RELAY_COUNT; i++) {
      JsonObject plant = plants.createNestedObject();
      plant["index"] = i;
      plant["watering"] = relayStates[i];
      plant["dailyCount"] = wateringCount[i];
      
      // Format last watered time
      String lastWatered = String(lastWatering[i].hour()) + ":" + 
                          (lastWatering[i].minute() < 10 ? "0" : "") + String(lastWatering[i].minute());
      plant["lastWatered"] = lastWatered;
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/water", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      StaticJsonDocument<200> doc;
      deserializeJson(doc, server.arg("plain"));
      
      int index = doc["index"];
      int duration = doc["duration"];
      
      if (wateringCount[index] < MAX_WATERING_PER_DAY) {
        startWatering(index, duration);
        wateringCount[index]++;
        lastWatering[index] = rtc.now();
        server.send(200, "application/json", "{\"status\":\"success\"}");
      } else {
        server.send(400, "application/json", "{\"status\":\"daily_limit_reached\"}");
      }
    } else {
      server.send(400, "application/json", "{\"status\":\"error\"}");
    }
  });
  
  server.on("/api/stop", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      StaticJsonDocument<200> doc;
      deserializeJson(doc, server.arg("plain"));
      
      int index = doc["index"];
      stopWatering(index);
      
      server.send(200, "application/json", "{\"status\":\"success\"}");
    } else {
      server.send(400, "application/json", "{\"status\":\"error\"}");
    }
  });
  
  server.on("/api/schedule", HTTP_GET, []() {
    StaticJsonDocument<2048> doc;
    JsonArray schedulesArray = doc.createNestedArray("schedules");
    
    for (int i = 0; i < scheduleCount; i++) {
      JsonObject schedule = schedulesArray.createNestedObject();
      schedule["time"] = String(wateringSchedules[i].hour) + ":" + 
                        (wateringSchedules[i].minute < 10 ? "0" : "") + String(wateringSchedules[i].minute);
      schedule["plantIndex"] = wateringSchedules[i].relayIndex;
      schedule["duration"] = wateringSchedules[i].duration;
      schedule["enabled"] = wateringSchedules[i].enabled;
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/stats", HTTP_GET, []() {
    StaticJsonDocument<1024> doc;
    JsonArray stats = doc.createNestedArray("stats");
    
    for (int i = 0; i < RELAY_COUNT; i++) {
      JsonObject stat = stats.createNestedObject();
      stat["today"] = wateringCount[i];
      stat["lastWatered"] = String(lastWatering[i].hour()) + ":" + 
                           (lastWatering[i].minute() < 10 ? "0" : "") + String(lastWatering[i].minute());
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  // Blink integration endpoints
  server.on("/api/blink/test", HTTP_POST, []() {
    armBlinkCamera();
    delay(2000);
    disarmBlinkCamera();
    server.send(200, "application/json", "{\"status\":\"blink_test_completed\"}");
  });
  
  server.on("/api/blink/water", HTTP_POST, []() {
    int zone = 0;
    if (server.hasArg("zone")) {
      zone = server.arg("zone").toInt();
    }
    
    if (zone >= 0 && zone < RELAY_COUNT) {
      startWatering(zone, 5); // 5 minutes default for Blink
      server.send(200, "application/json", "{\"status\":\"watering_started\",\"zone\":" + String(zone) + "}");
    } else {
      server.send(400, "application/json", "{\"error\":\"invalid_zone\"}");
    }
  });
  
  server.on("/api/blink/status", HTTP_GET, []() {
    StaticJsonDocument<512> doc;
    doc["blink_connected"] = !blinkAuthToken.isEmpty();
    doc["system_status"] = "running";
    doc["active_zones"] = 0;
    
    for (int i = 0; i < RELAY_COUNT; i++) {
      if (isWatering[i]) doc["active_zones"] = doc["active_zones"].as<int>() + 1;
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
}

// Blink Camera Integration Functions
void authenticateBlink() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  http.begin(blinkAuthUrl);
  http.addHeader("Content-Type", "application/json");
  
  StaticJsonDocument<256> doc;
  doc["email"] = blinkEmail;
  doc["password"] = blinkPassword;
  
  String requestBody;
  serializeJson(doc, requestBody);
  
  int httpCode = http.POST(requestBody);
  
  if (httpCode == 200) {
    String response = http.getString();
    StaticJsonDocument<1024> responseDoc;
    deserializeJson(responseDoc, response);
    
    blinkAuthToken = responseDoc["auth"]["token"].as<String>();
    lastBlinkAuth = millis();
    Serial.println("✅ Blink authentication successful");
    sendWebhook("📷 Blink camera connected successfully", "info");
  } else {
    Serial.print("❌ Blink authentication failed: ");
    Serial.println(httpCode);
  }
  
  http.end();
}

void checkBlinkConnection() {
  if (millis() - lastBlinkAuth > BLINK_AUTH_INTERVAL) {
    authenticateBlink();
  }
}

void armBlinkCamera() {
  if (blinkAuthToken.isEmpty() || WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  String url = blinkArmUrl + String(blinkNetworkId) + "/camera/" + String(blinkCameraId) + "/enable";
  http.begin(url);
  http.addHeader("TOKEN_AUTH", blinkAuthToken);
  
  int httpCode = http.POST("");
  if (httpCode == 200) {
    Serial.println("🔒 Blink camera armed for monitoring");
  } else {
    Serial.print("❌ Failed to arm Blink camera: ");
    Serial.println(httpCode);
  }
  
  http.end();
}

void disarmBlinkCamera() {
  if (blinkAuthToken.isEmpty() || WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  String url = blinkDisarmUrl + String(blinkNetworkId) + "/camera/" + String(blinkCameraId) + "/disable";
  http.begin(url);
  http.addHeader("TOKEN_AUTH", blinkAuthToken);
  
  int httpCode = http.POST("");
  if (httpCode == 200) {
    Serial.println("🔓 Blink camera disarmed");
  } else {
    Serial.print("❌ Failed to disarm Blink camera: ");
    Serial.println(httpCode);
  }
  
  http.end();
}

void sendWebhook(String message, String level) {
  if (!webhookEnabled || WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  http.begin(webhookUrl);
  http.addHeader("Content-Type", "application/json");
  
  StaticJsonDocument<512> doc;
  doc["timestamp"] = getCurrentTime();
  doc["system"] = "Basic Watering System";
  doc["message"] = message;
  doc["level"] = level;
  doc["ip"] = WiFi.localIP().toString();
  
  String payload;
  serializeJson(doc, payload);
  
  int httpCode = http.POST(payload);
  if (httpCode > 0) {
    Serial.println("📤 Webhook sent: " + message);
  }
  
  http.end();
}

String getCurrentTime() {
  DateTime now = rtc.now();
  return String(now.year()) + "-" + 
         String(now.month()) + "-" + 
         String(now.day()) + " " + 
         String(now.hour()) + ":" + 
         String(now.minute()) + ":" + 
         String(now.second());
} 
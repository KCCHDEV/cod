#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// Mushroom Configuration
RTC_DS3231 rtc;
const int RELAY_COUNT = 4;
const int relayPins[RELAY_COUNT] = {5, 18, 19, 21};

// Mushroom-specific settings
const int MUSHROOM_MOISTURE_MIN = 70; // เห็ดนางฟ้าต้องการความชื้นสูง
const int MUSHROOM_MOISTURE_MAX = 95;
const int MUSHROOM_TEMP_MIN = 20;
const int MUSHROOM_TEMP_MAX = 30;

// WiFi & Webhook
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* webhookUrl = "YOUR_WEBHOOK_URL";
bool webhookEnabled = true;

// System variables
WebServer server(80);
bool relayStates[RELAY_COUNT] = {false, false, false, false};
int wateringCount[RELAY_COUNT] = {0, 0, 0, 0};
int moisturePercent[4] = {75, 82, 68, 88}; // เห็ดนางฟ้าต้องการความชื้นสูง
String moistureStatus[4] = {"Good", "Excellent", "Low", "Excellent"};
unsigned long lastMoistureUpdate = 0;
unsigned long lastWateringTime[RELAY_COUNT] = {0, 0, 0, 0};

// Mushroom growth tracking
struct MushroomBed {
  int growthStage; // 0=Inoculation, 1=Colonization, 2=Fruiting, 3=Harvest
  int daysInStage;
  float temperature;
  int humidity;
  bool needsAttention;
  String status;
};

MushroomBed mushroomBeds[4] = {
  {2, 5, 25.5, 78, false, "Fruiting"},
  {1, 12, 24.8, 82, false, "Colonizing"},
  {2, 3, 26.2, 68, true, "Needs Water"},
  {3, 0, 25.0, 88, false, "Ready to Harvest"}
};

void setup() {
  Serial.begin(115200);
  
  Wire.begin();
  if (!rtc.begin()) {
    Serial.println("RTC not found - using demo time");
  } else {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH);
  }
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nWiFi connection failed - demo mode only");
  }
  
  setupWebServer();
  server.begin();
  
  Serial.println("🍄 Mushroom Watering System Ready!");
  Serial.println("Optimized for Pink Oyster Mushrooms (เห็ดนางฟ้า)");
}

void loop() {
  server.handleClient();
  updateMushroomData();
  checkMushroomWatering();
  updateGrowthStages();
  delay(1000);
}

void updateMushroomData() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastMoistureUpdate >= 15000) {
    lastMoistureUpdate = currentTime;
    
    for (int i = 0; i < 4; i++) {
      int change = random(-3, 4);
      moisturePercent[i] += change;
      
      if (moisturePercent[i] > MUSHROOM_MOISTURE_MAX) moisturePercent[i] = MUSHROOM_MOISTURE_MAX;
      if (moisturePercent[i] < MUSHROOM_MOISTURE_MIN) moisturePercent[i] = MUSHROOM_MOISTURE_MIN;
      
      if (moisturePercent[i] >= 85) {
        moistureStatus[i] = "Excellent";
      } else if (moisturePercent[i] >= 75) {
        moistureStatus[i] = "Good";
      } else if (moisturePercent[i] >= 70) {
        moistureStatus[i] = "Low";
      } else {
        moistureStatus[i] = "Critical";
      }
      
      mushroomBeds[i].humidity = moisturePercent[i];
      mushroomBeds[i].temperature = 24.0 + random(-2, 3);
      
      if (relayStates[i]) {
        moisturePercent[i] += 3;
        mushroomBeds[i].humidity = moisturePercent[i];
        if (moisturePercent[i] > MUSHROOM_MOISTURE_MAX) moisturePercent[i] = MUSHROOM_MOISTURE_MAX;
      }
      
      mushroomBeds[i].needsAttention = (moisturePercent[i] < 75 || mushroomBeds[i].temperature > 28);
    }
    
    Serial.println("🍄 Updated mushroom bed data:");
    for (int i = 0; i < 4; i++) {
      Serial.print("Bed ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(moisturePercent[i]);
      Serial.print("% humidity, ");
      Serial.print(mushroomBeds[i].temperature, 1);
      Serial.print("°C, ");
      Serial.print(mushroomBeds[i].status);
      Serial.println();
    }
  }
}

void updateGrowthStages() {
  static unsigned long lastGrowthUpdate = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastGrowthUpdate >= 3600000) {
    lastGrowthUpdate = currentTime;
    
    for (int i = 0; i < 4; i++) {
      mushroomBeds[i].daysInStage++;
      
      if (mushroomBeds[i].growthStage == 1 && mushroomBeds[i].daysInStage >= 14) {
        mushroomBeds[i].growthStage = 2;
        mushroomBeds[i].daysInStage = 0;
        mushroomBeds[i].status = "Fruiting";
        Serial.print("🍄 Bed ");
        Serial.print(i + 1);
        Serial.println(" started fruiting!");
      } else if (mushroomBeds[i].growthStage == 2 && mushroomBeds[i].daysInStage >= 7) {
        mushroomBeds[i].growthStage = 3;
        mushroomBeds[i].daysInStage = 0;
        mushroomBeds[i].status = "Ready to Harvest";
        Serial.print("🍄 Bed ");
        Serial.print(i + 1);
        Serial.println(" ready for harvest!");
      }
    }
  }
}

void checkMushroomWatering() {
  unsigned long currentTime = millis();
  
  for (int i = 0; i < RELAY_COUNT; i++) {
    if (relayStates[i] && (currentTime - lastWateringTime[i] > 20000)) {
      stopWatering(i);
      Serial.print("🍄 Auto-stopped watering Bed ");
      Serial.println(i + 1);
    }
  }
}

void startWatering(int relayIndex, int duration) {
  if (relayIndex >= 0 && relayIndex < RELAY_COUNT) {
    digitalWrite(relayPins[relayIndex], LOW);
    relayStates[relayIndex] = true;
    lastWateringTime[relayIndex] = millis();
    
    if (wateringCount[relayIndex] < 8) {
      wateringCount[relayIndex]++;
    }
    
    Serial.print("🍄 Started watering Bed ");
    Serial.print(relayIndex + 1);
    Serial.print(" for ");
    Serial.print(duration);
    Serial.println(" minutes");
  }
}

void stopWatering(int relayIndex) {
  if (relayIndex >= 0 && relayIndex < RELAY_COUNT) {
    digitalWrite(relayPins[relayIndex], HIGH);
    relayStates[relayIndex] = false;
    
    Serial.print("🍄 Stopped watering Bed ");
    Serial.println(relayIndex + 1);
  }
}

void setupWebServer() {
  server.on("/", HTTP_GET, []() {
    String html = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Mushroom Watering System</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; min-height: 100vh; }
        .container { max-width: 1200px; margin: 0 auto; }
        .header { text-align: center; margin-bottom: 30px; }
        .stats { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 20px; margin-bottom: 30px; }
        .stat-card { background: rgba(255,255,255,0.1); padding: 20px; border-radius: 10px; text-align: center; }
        .beds { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; margin-bottom: 30px; }
        .bed-card { background: rgba(255,255,255,0.1); padding: 20px; border-radius: 10px; }
        .controls { background: rgba(255,255,255,0.1); padding: 20px; border-radius: 10px; margin-bottom: 20px; }
        .button { background: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; margin: 5px; }
        .button:hover { background: #45a049; }
        .button-red { background: #f44336; }
        .button-red:hover { background: #da190b; }
        .button-blue { background: #2196F3; }
        .button-blue:hover { background: #0b7dda; }
        .moisture-bar { width: 100%; height: 20px; background: #ddd; border-radius: 10px; overflow: hidden; margin: 10px 0; }
        .moisture-fill { height: 100%; background: linear-gradient(90deg, #ff0000, #ffff00, #00ff00); transition: width 0.3s; }
        .status { padding: 5px 10px; border-radius: 5px; font-weight: bold; }
        .status-watering { background: #2196F3; }
        .status-idle { background: #4CAF50; }
        .status-attention { background: #ff9800; }
        .status-harvest { background: #f44336; }
        .refresh { text-align: center; margin: 20px 0; }
        .refresh a { color: white; text-decoration: none; padding: 10px 20px; background: rgba(255,255,255,0.2); border-radius: 5px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>Mushroom Watering System</h1>
            <p>Pink Oyster Mushroom Cultivation</p>
        </div>

        <div class="stats">
            <div class="stat-card">
                <h3>Current Time</h3>
                <p id="time">Loading...</p>
            </div>
            <div class="stat-card">
                <h3>Average Temperature</h3>
                <p id="avgTemp">25.2C</p>
            </div>
            <div class="stat-card">
                <h3>Average Humidity</h3>
                <p id="avgHumidity">78%</p>
            </div>
            <div class="stat-card">
                <h3>Active Beds</h3>
                <p id="activeBeds">0</p>
            </div>
        </div>

        <div class="beds" id="bedGrid">
            <!-- Bed cards will be generated here -->
        </div>

        <div class="controls">
            <h3>System Controls</h3>
            <form method="POST" action="/control">
                <button type="submit" name="action" value="simulate" class="button">Simulate Growth</button>
                <button type="submit" name="action" value="reset" class="button button-red">Reset All Beds</button>
                <button type="submit" name="action" value="harvest" class="button button-blue">Harvest Ready Beds</button>
            </form>
        </div>

        <div class="refresh">
            <a href="/">Refresh Page</a>
        </div>
    </div>
</body>
</html>
    )";
    server.send(200, "text/html", html);
  });

  server.on("/control", HTTP_POST, []() {
    String action = server.hasArg("action") ? server.arg("action") : "";
    
    if (action == "simulate") {
      for (int i = 0; i < RELAY_COUNT; i++) {
        if (mushroomBeds[i].growthStage < 3) {
          mushroomBeds[i].daysInStage += 2;
          
          if (mushroomBeds[i].growthStage == 1 && mushroomBeds[i].daysInStage >= 14) {
            mushroomBeds[i].growthStage = 2;
            mushroomBeds[i].daysInStage = 0;
            mushroomBeds[i].status = "Fruiting";
          } else if (mushroomBeds[i].growthStage == 2 && mushroomBeds[i].daysInStage >= 7) {
            mushroomBeds[i].growthStage = 3;
            mushroomBeds[i].daysInStage = 0;
            mushroomBeds[i].status = "Ready to Harvest";
          }
        }
      }
    } else if (action == "reset") {
      for (int i = 0; i < RELAY_COUNT; i++) {
        wateringCount[i] = 0;
        relayStates[i] = false;
        digitalWrite(relayPins[i], HIGH);
        
        mushroomBeds[i].growthStage = 1;
        mushroomBeds[i].daysInStage = 0;
        mushroomBeds[i].status = "Colonizing";
        mushroomBeds[i].needsAttention = false;
      }
      
      moisturePercent[0] = 75;
      moisturePercent[1] = 82;
      moisturePercent[2] = 68;
      moisturePercent[3] = 88;
    } else if (action == "harvest") {
      for (int i = 0; i < RELAY_COUNT; i++) {
        if (mushroomBeds[i].growthStage == 3) {
          mushroomBeds[i].growthStage = 2;
          mushroomBeds[i].daysInStage = 0;
          mushroomBeds[i].status = "Fruiting";
          Serial.print("Harvested Bed ");
          Serial.println(i + 1);
        }
      }
    }
    
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  });

  server.on("/water", HTTP_POST, []() {
    if (server.hasArg("bed") && server.hasArg("duration")) {
      int bed = server.arg("bed").toInt();
      int duration = server.arg("duration").toInt();
      
      if (bed >= 0 && bed < RELAY_COUNT && wateringCount[bed] < 8) {
        startWatering(bed, duration);
      }
    }
    
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  });

  server.on("/stop", HTTP_POST, []() {
    if (server.hasArg("bed")) {
      int bed = server.arg("bed").toInt();
      
      if (bed >= 0 && bed < RELAY_COUNT) {
        stopWatering(bed);
      }
    }
    
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  });
  
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
  
  server.on("/api/beds", HTTP_GET, []() {
    StaticJsonDocument<4096> doc;
    JsonArray beds = doc.createNestedArray("beds");
    
    for (int i = 0; i < RELAY_COUNT; i++) {
      JsonObject bed = beds.createNestedObject();
      bed["index"] = i;
      bed["watering"] = relayStates[i];
      bed["dailyCount"] = wateringCount[i];
      bed["humidity"] = moisturePercent[i];
      bed["temperature"] = mushroomBeds[i].temperature;
      bed["growthStage"] = mushroomBeds[i].growthStage;
      bed["status"] = mushroomBeds[i].status;
      bed["needsAttention"] = mushroomBeds[i].needsAttention;
      bed["daysInStage"] = mushroomBeds[i].daysInStage;
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
      
      if (wateringCount[index] < 8) {
        startWatering(index, duration);
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
  
  server.on("/api/stats", HTTP_GET, []() {
    StaticJsonDocument<1024> doc;
    JsonArray stats = doc.createNestedArray("stats");
    
    for (int i = 0; i < RELAY_COUNT; i++) {
      JsonObject stat = stats.createNestedObject();
      stat["today"] = wateringCount[i];
      stat["humidity"] = moisturePercent[i];
      stat["temperature"] = mushroomBeds[i].temperature;
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/demo/growth", HTTP_POST, []() {
    for (int i = 0; i < RELAY_COUNT; i++) {
      if (mushroomBeds[i].growthStage < 3) {
        mushroomBeds[i].daysInStage += 2;
        
        if (mushroomBeds[i].growthStage == 1 && mushroomBeds[i].daysInStage >= 14) {
          mushroomBeds[i].growthStage = 2;
          mushroomBeds[i].daysInStage = 0;
          mushroomBeds[i].status = "Fruiting";
        } else if (mushroomBeds[i].growthStage == 2 && mushroomBeds[i].daysInStage >= 7) {
          mushroomBeds[i].growthStage = 3;
          mushroomBeds[i].daysInStage = 0;
          mushroomBeds[i].status = "Ready to Harvest";
        }
      }
    }
    
    server.send(200, "application/json", "{\"status\":\"growth_simulated\"}");
  });
  
  server.on("/api/demo/reset", HTTP_POST, []() {
    for (int i = 0; i < RELAY_COUNT; i++) {
      wateringCount[i] = 0;
      relayStates[i] = false;
      digitalWrite(relayPins[i], HIGH);
      
      mushroomBeds[i].growthStage = 1;
      mushroomBeds[i].daysInStage = 0;
      mushroomBeds[i].status = "Colonizing";
      mushroomBeds[i].needsAttention = false;
    }
    
    moisturePercent[0] = 75;
    moisturePercent[1] = 82;
    moisturePercent[2] = 68;
    moisturePercent[3] = 88;
    
    server.send(200, "application/json", "{\"status\":\"beds_reset\"}");
  });
  
  server.on("/api/demo/harvest", HTTP_POST, []() {
    for (int i = 0; i < RELAY_COUNT; i++) {
      if (mushroomBeds[i].growthStage == 3) {
        mushroomBeds[i].growthStage = 2;
        mushroomBeds[i].daysInStage = 0;
        mushroomBeds[i].status = "Fruiting";
        Serial.print("Harvested Bed ");
        Serial.println(i + 1);
      }
    }
    
    server.send(200, "application/json", "{\"status\":\"harvested\"}");
  });
} 
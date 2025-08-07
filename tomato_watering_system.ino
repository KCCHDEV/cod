/*
 * ระบบให้น้ำต้นมะเขือเทศอัตโนมัติด้วย ESP32
 * Features:
 * - วัดอุณหภูมิและความชื้นอากาศ (DHT22)
 * - วัดความชื้นในดิน
 * - ให้น้ำอัตโนมัติตามเงื่อนไข
 * - ควบคุมผ่าน Blink App
 * - Web Interface สำหรับควบคุม
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <RTClib.h>

// DHT22 Configuration
#define DHT_PIN 4
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

// Soil Moisture Sensor Configuration
#define SOIL_MOISTURE_PIN 36
#define SOIL_MOISTURE_POWER_PIN 25

// Relay Configuration
#define RELAY_PIN 5
#define WATER_PUMP_DURATION 30

// LED Status
#define STATUS_LED 2

// RTC Configuration
RTC_DS3231 rtc;

// WiFi Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Web Server
WebServer server(80);

// System States
bool waterPumpActive = false;
bool systemEnabled = true;
bool autoMode = true;

// Timing Variables
unsigned long waterPumpStartTime = 0;
const unsigned long WATER_PUMP_TIMEOUT = WATER_PUMP_DURATION * 1000;

// Sensor Thresholds
const int SOIL_MOISTURE_DRY = 2000;
const int SOIL_MOISTURE_WET = 1500;
const float TEMP_MAX = 35.0;
const float HUMIDITY_MIN = 30.0;

// Daily counters
int dailyWaterCount = 0;
const int MAX_DAILY_WATER = 8;
const int MIN_WATER_INTERVAL = 2;

// Last action times
DateTime lastWaterTime;
DateTime lastSensorRead;

// Sensor data
float temperature = 0;
float humidity = 0;
int soilMoisture = 0;
bool soilIsDry = false;

void setup() {
  Serial.begin(115200);
  
  // Initialize I2C for RTC
  Wire.begin();
  
  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("❌ Couldn't find RTC");
    while (1);
  }
  
  // Initialize pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  pinMode(SOIL_MOISTURE_POWER_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(STATUS_LED, LOW);
  digitalWrite(SOIL_MOISTURE_POWER_PIN, LOW);
  
  // Initialize DHT22
  dht.begin();
  
  // Initialize timing variables
  lastWaterTime = rtc.now();
  lastSensorRead = rtc.now();
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("🔗 Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
  }
  Serial.println();
  Serial.print("✅ Connected to WiFi. IP: ");
  Serial.println(WiFi.localIP());
  digitalWrite(STATUS_LED, HIGH);
  
  // Setup web server routes
  setupWebServer();
  server.begin();
  
  Serial.println("🍅 Tomato Watering System Ready!");
}

void loop() {
  server.handleClient();
  readSensors();
  checkWateringConditions();
  checkTimers();
  resetDailyCounters();
  updateStatusLED();
  delay(5000);
}

void readSensors() {
  DateTime now = rtc.now();
  
  if ((now.unixtime() - lastSensorRead.unixtime()) >= 30) {
    lastSensorRead = now;
    
    // Read DHT22
    float newTemp = dht.readTemperature();
    float newHumidity = dht.readHumidity();
    
    if (!isnan(newTemp) && !isnan(newHumidity)) {
      temperature = newTemp;
      humidity = newHumidity;
    }
    
    // Read soil moisture
    digitalWrite(SOIL_MOISTURE_POWER_PIN, HIGH);
    delay(100);
    soilMoisture = analogRead(SOIL_MOISTURE_PIN);
    digitalWrite(SOIL_MOISTURE_POWER_PIN, LOW);
    
    soilIsDry = (soilMoisture > SOIL_MOISTURE_DRY);
    
    Serial.print("🌡️ Temp: ");
    Serial.print(temperature);
    Serial.print("°C, 💧 Humidity: ");
    Serial.print(humidity);
    Serial.print("%, 🌱 Soil: ");
    Serial.print(soilMoisture);
    Serial.print(" (");
    Serial.print(soilIsDry ? "Dry" : "Wet");
    Serial.println(")");
  }
}

void checkWateringConditions() {
  if (!systemEnabled || !autoMode) return;
  
  DateTime now = rtc.now();
  int hoursSinceLastWater = (now.unixtime() - lastWaterTime.unixtime()) / 3600;
  
  if (hoursSinceLastWater < MIN_WATER_INTERVAL) return;
  if (dailyWaterCount >= MAX_DAILY_WATER) {
    Serial.println("⚠️ Daily watering limit reached");
    return;
  }
  
  bool shouldWater = false;
  String reason = "";
  
  if (soilIsDry) {
    shouldWater = true;
    reason = "Soil is dry";
  } else if (temperature > TEMP_MAX && humidity < HUMIDITY_MIN) {
    shouldWater = true;
    reason = "High temperature and low humidity";
  } else if (soilMoisture > (SOIL_MOISTURE_DRY + 500)) {
    shouldWater = true;
    reason = "Very dry soil";
  }
  
  if (shouldWater) {
    startWaterPump(WATER_PUMP_DURATION);
    dailyWaterCount++;
    lastWaterTime = now;
    Serial.print("💧 Started watering: ");
    Serial.println(reason);
  }
}

void startWaterPump(int duration) {
  if (!waterPumpActive) {
    digitalWrite(RELAY_PIN, LOW);
    waterPumpActive = true;
    waterPumpStartTime = millis();
  }
}

void stopWaterPump() {
  if (waterPumpActive) {
    digitalWrite(RELAY_PIN, HIGH);
    waterPumpActive = false;
    Serial.println("💧 Water pump stopped");
  }
}

void checkTimers() {
  unsigned long currentTime = millis();
  
  if (waterPumpActive && (currentTime - waterPumpStartTime >= WATER_PUMP_TIMEOUT)) {
    stopWaterPump();
  }
}

void resetDailyCounters() {
  DateTime now = rtc.now();
  static int lastDay = -1;
  
  if (now.day() != lastDay) {
    dailyWaterCount = 0;
    lastDay = now.day();
    Serial.println("🔄 Daily counters reset");
  }
}

void updateStatusLED() {
  if (waterPumpActive) {
    digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
  } else if (soilIsDry) {
    digitalWrite(STATUS_LED, HIGH);
  } else {
    digitalWrite(STATUS_LED, LOW);
  }
}

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

void setupWebServer() {
  server.on("/", HTTP_GET, []() {
    String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>🍅 Tomato Watering System</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; 
            margin: 0; 
            background: linear-gradient(135deg, #ff6b6b 0%, #ee5a24 100%); 
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
        .sensor-grid { 
            display: grid; 
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); 
            gap: 20px; 
            margin-bottom: 30px; 
        }
        .sensor-card { 
            background: rgba(255,255,255,0.1); 
            padding: 25px; 
            border-radius: 15px; 
            text-align: center; 
            border: 1px solid rgba(255,255,255,0.2);
            backdrop-filter: blur(10px);
        }
        .sensor-card.warning { 
            background: rgba(255,193,7,0.2); 
            border-color: #ffc107; 
        }
        .sensor-card.danger { 
            background: rgba(220,53,69,0.2); 
            border-color: #dc3545; 
        }
        .sensor-card.good { 
            background: rgba(40,167,69,0.2); 
            border-color: #28a745; 
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
        .btn-stop { 
            background: linear-gradient(45deg, #f44336, #d32f2f); 
            color: white; 
        }
        .btn-toggle { 
            background: linear-gradient(45deg, #ff9800, #f57c00); 
            color: white; 
        }
        .btn:hover { 
            transform: translateY(-3px); 
            box-shadow: 0 8px 25px rgba(0,0,0,0.3); 
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
            <h1>🍅 Tomato Watering System</h1>
            <p>ระบบให้น้ำต้นมะเขือเทศอัตโนมัติ</p>
        </div>
        
        <div class="sensor-grid">
            <div class="sensor-card" id="tempCard">
                <h2>🌡️ Temperature</h2>
                <div id="temperature" style="font-size: 32px; font-weight: bold;">--°C</div>
                <div id="tempStatus">Loading...</div>
            </div>
            
            <div class="sensor-card" id="humidityCard">
                <h2>💧 Humidity</h2>
                <div id="humidity" style="font-size: 32px; font-weight: bold;">--%</div>
                <div id="humidityStatus">Loading...</div>
            </div>
            
            <div class="sensor-card" id="soilCard">
                <h2>🌱 Soil Moisture</h2>
                <div id="soilMoisture" style="font-size: 32px; font-weight: bold;">--</div>
                <div id="soilStatus">Loading...</div>
                <div class="progress-bar">
                    <div class="progress-fill" id="soilProgress" style="width: 0%"></div>
                </div>
            </div>
            
            <div class="sensor-card" id="waterCard">
                <h2>💧 Water System</h2>
                <div id="waterStatus">Idle</div>
                <div class="progress-bar">
                    <div class="progress-fill" id="waterProgress" style="width: 0%"></div>
                </div>
                <div>Daily: <span id="waterCount">0</span>/<span id="waterMax">8</span></div>
            </div>
        </div>
        
        <div class="control-panel">
            <h2>🎮 Manual Control</h2>
            <div>
                <button class="btn btn-water" onclick="manualWater(15)">Water 15s</button>
                <button class="btn btn-water" onclick="manualWater(30)">Water 30s</button>
                <button class="btn btn-water" onclick="manualWater(60)">Water 1min</button>
                <button class="btn btn-stop" onclick="stopWater()">Stop Water</button>
            </div>
            <div style="margin-top: 20px;">
                <button class="btn btn-toggle" onclick="toggleAutoMode()" id="autoBtn">Auto Mode: ON</button>
                <button class="btn btn-toggle" onclick="toggleSystem()" id="systemBtn">System: ON</button>
            </div>
        </div>
    </div>
    
    <script>
        function updateStatus() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    // Update temperature
                    const tempCard = document.getElementById('tempCard');
                    const tempStatus = document.getElementById('tempStatus');
                    document.getElementById('temperature').textContent = data.temperature.toFixed(1) + '°C';
                    
                    if (data.temperature > 35) {
                        tempCard.className = 'sensor-card danger';
                        tempStatus.textContent = '⚠️ Too Hot';
                    } else if (data.temperature > 30) {
                        tempCard.className = 'sensor-card warning';
                        tempStatus.textContent = '🌡️ Warm';
                    } else {
                        tempCard.className = 'sensor-card good';
                        tempStatus.textContent = '✅ Good';
                    }
                    
                    // Update humidity
                    const humidityCard = document.getElementById('humidityCard');
                    const humidityStatus = document.getElementById('humidityStatus');
                    document.getElementById('humidity').textContent = data.humidity.toFixed(1) + '%';
                    
                    if (data.humidity < 30) {
                        humidityCard.className = 'sensor-card danger';
                        humidityStatus.textContent = '⚠️ Too Dry';
                    } else if (data.humidity < 50) {
                        humidityCard.className = 'sensor-card warning';
                        humidityStatus.textContent = '💧 Low';
                    } else {
                        humidityCard.className = 'sensor-card good';
                        humidityStatus.textContent = '✅ Good';
                    }
                    
                    // Update soil moisture
                    const soilCard = document.getElementById('soilCard');
                    const soilStatus = document.getElementById('soilStatus');
                    const soilProgress = document.getElementById('soilProgress');
                    document.getElementById('soilMoisture').textContent = data.soilMoisture;
                    
                    const soilPercent = ((data.soilMoisture - 1500) / (2000 - 1500)) * 100;
                    soilProgress.style.width = Math.min(Math.max(soilPercent, 0), 100) + '%';
                    
                    if (data.soilIsDry) {
                        soilCard.className = 'sensor-card danger';
                        soilStatus.textContent = '⚠️ Needs Water';
                    } else {
                        soilCard.className = 'sensor-card good';
                        soilStatus.textContent = '✅ Moist';
                    }
                    
                    // Update water system
                    const waterCard = document.getElementById('waterCard');
                    const waterStatus = document.getElementById('waterStatus');
                    const waterProgress = document.getElementById('waterProgress');
                    const waterCount = document.getElementById('waterCount');
                    
                    if (data.waterActive) {
                        waterCard.className = 'sensor-card warning';
                        waterStatus.textContent = '💧 Pumping...';
                        const progress = ((Date.now() - data.waterStartTime) / (data.waterDuration * 1000)) * 100;
                        waterProgress.style.width = Math.min(progress, 100) + '%';
                    } else {
                        waterCard.className = 'sensor-card';
                        waterStatus.textContent = 'Idle';
                        waterProgress.style.width = '0%';
                    }
                    waterCount.textContent = data.dailyWaterCount;
                    
                    // Update buttons
                    document.getElementById('autoBtn').textContent = 'Auto Mode: ' + (data.autoMode ? 'ON' : 'OFF');
                    document.getElementById('systemBtn').textContent = 'System: ' + (data.systemEnabled ? 'ON' : 'OFF');
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
        
        function toggleAutoMode() {
            fetch('/api/toggle-auto', {
                method: 'POST'
            })
            .then(() => updateStatus());
        }
        
        function toggleSystem() {
            fetch('/api/toggle-system', {
                method: 'POST'
            })
            .then(() => updateStatus());
        }
        
        // Update every 2 seconds
        setInterval(() => {
            updateStatus();
        }, 2000);
        
        // Initial load
        updateStatus();
    </script>
</body>
</html>
    )";
    server.send(200, "text/html", html);
  });
  
  // API endpoints
  server.on("/api/status", HTTP_GET, []() {
    DateTime now = rtc.now();
    String lastWaterStr = String(lastWaterTime.hour()) + ":" + 
                         (lastWaterTime.minute() < 10 ? "0" : "") + String(lastWaterTime.minute());
    
    StaticJsonDocument<512> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["soilMoisture"] = soilMoisture;
    doc["soilIsDry"] = soilIsDry;
    doc["waterActive"] = waterPumpActive;
    doc["systemEnabled"] = systemEnabled;
    doc["autoMode"] = autoMode;
    doc["dailyWaterCount"] = dailyWaterCount;
    doc["lastWaterTime"] = lastWaterStr;
    
    if (waterPumpActive) {
      doc["waterStartTime"] = waterPumpStartTime;
      doc["waterDuration"] = WATER_PUMP_DURATION;
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
  
  server.on("/api/toggle-auto", HTTP_POST, []() {
    autoMode = !autoMode;
    server.send(200, "application/json", "{\"status\":\"success\",\"autoMode\":" + String(autoMode ? "true" : "false") + "}");
  });
  
  server.on("/api/toggle-system", HTTP_POST, []() {
    systemEnabled = !systemEnabled;
    server.send(200, "application/json", "{\"status\":\"success\",\"systemEnabled\":" + String(systemEnabled ? "true" : "false") + "}");
  });
  
  // Blink App compatible endpoints
  server.on("/api/blink/water", HTTP_POST, []() {
    manualWater(30);
    server.send(200, "application/json", "{\"status\":\"water_started\"}");
  });
  
  server.on("/api/blink/status", HTTP_GET, []() {
    StaticJsonDocument<512> doc;
    doc["water_active"] = waterPumpActive;
    doc["system_enabled"] = systemEnabled;
    doc["auto_mode"] = autoMode;
    doc["daily_water"] = dailyWaterCount;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["soil_moisture"] = soilMoisture;
    doc["soil_is_dry"] = soilIsDry;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/blink/emergency", HTTP_POST, []() {
    if (soilIsDry) {
      manualWater(60);
      server.send(200, "application/json", "{\"status\":\"emergency_watering_started\"}");
    } else {
      server.send(200, "application/json", "{\"status\":\"soil_not_dry\"}");
    }
  });
} 
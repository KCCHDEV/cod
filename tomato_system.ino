/*
 * ระบบให้น้ำต้นมะเขือเทศอัตโนมัติ
 * ESP32 + DHT22 + Soil Moisture + Relay
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define DHT_PIN 4
#define DHT_TYPE DHT22
#define SOIL_PIN 36
#define RELAY_PIN 5
#define LED_PIN 2

DHT dht(DHT_PIN, DHT_TYPE);
WebServer server(80);

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

bool waterPumpActive = false;
bool autoMode = true;
int dailyWaterCount = 0;
const int MAX_DAILY_WATER = 8;

float temperature = 0;
float humidity = 0;
int soilMoisture = 0;
bool soilIsDry = false;

void setup() {
  Serial.begin(115200);
  
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  
  dht.begin();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  setupWebServer();
  server.begin();
}

void loop() {
  server.handleClient();
  readSensors();
  checkWateringConditions();
  updateStatusLED();
  delay(5000);
}

void readSensors() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  soilMoisture = analogRead(SOIL_PIN);
  soilIsDry = (soilMoisture > 2000);
  
  Serial.printf("Temp: %.1f°C, Humidity: %.1f%%, Soil: %d (%s)\n", 
                temperature, humidity, soilMoisture, soilIsDry ? "Dry" : "Wet");
}

void checkWateringConditions() {
  if (!autoMode || dailyWaterCount >= MAX_DAILY_WATER) return;
  
  if (soilIsDry || (temperature > 35 && humidity < 30)) {
    startWaterPump(30);
    dailyWaterCount++;
  }
}

void startWaterPump(int duration) {
  if (!waterPumpActive) {
    digitalWrite(RELAY_PIN, LOW);
    waterPumpActive = true;
    Serial.println("💧 Water pump started");
    
    // Simple timer
    delay(duration * 1000);
    stopWaterPump();
  }
}

void stopWaterPump() {
  digitalWrite(RELAY_PIN, HIGH);
  waterPumpActive = false;
  Serial.println("💧 Water pump stopped");
}

void updateStatusLED() {
  if (waterPumpActive) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  } else if (soilIsDry) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

void manualWater(int duration) {
  if (dailyWaterCount < MAX_DAILY_WATER) {
    startWaterPump(duration);
    dailyWaterCount++;
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
    <style>
        body { font-family: Arial; background: linear-gradient(135deg, #ff6b6b, #ee5a24); color: white; margin: 20px; }
        .container { max-width: 800px; margin: 0 auto; }
        .card { background: rgba(255,255,255,0.1); padding: 20px; margin: 10px 0; border-radius: 10px; }
        .btn { padding: 10px 20px; margin: 5px; border: none; border-radius: 5px; cursor: pointer; }
        .btn-water { background: #00bcd4; color: white; }
        .btn-stop { background: #f44336; color: white; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🍅 Tomato Watering System</h1>
        
        <div class="card">
            <h2>📊 Sensor Data</h2>
            <div>Temperature: <span id="temp">--</span>°C</div>
            <div>Humidity: <span id="humidity">--</span>%</div>
            <div>Soil Moisture: <span id="soil">--</span></div>
            <div>Status: <span id="status">Loading...</span></div>
        </div>
        
        <div class="card">
            <h2>🎮 Control</h2>
            <button class="btn btn-water" onclick="water(15)">Water 15s</button>
            <button class="btn btn-water" onclick="water(30)">Water 30s</button>
            <button class="btn btn-stop" onclick="stopWater()">Stop</button>
        </div>
        
        <div class="card">
            <h2>📈 Statistics</h2>
            <div>Daily Waterings: <span id="dailyCount">0</span>/8</div>
        </div>
    </div>
    
    <script>
        function updateStatus() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('temp').textContent = data.temperature.toFixed(1);
                    document.getElementById('humidity').textContent = data.humidity.toFixed(1);
                    document.getElementById('soil').textContent = data.soilMoisture;
                    document.getElementById('dailyCount').textContent = data.dailyWaterCount;
                    document.getElementById('status').textContent = data.waterActive ? '💧 Watering...' : 'Idle';
                });
        }
        
        function water(duration) {
            fetch('/api/water', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({duration: duration})
            })
            .then(() => updateStatus());
        }
        
        function stopWater() {
            fetch('/api/stop-water', {method: 'POST'})
                .then(() => updateStatus());
        }
        
        setInterval(updateStatus, 2000);
        updateStatus();
    </script>
</body>
</html>
    )";
    server.send(200, "text/html", html);
  });
  
  server.on("/api/status", HTTP_GET, []() {
    StaticJsonDocument<256> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["soilMoisture"] = soilMoisture;
    doc["soilIsDry"] = soilIsDry;
    doc["waterActive"] = waterPumpActive;
    doc["dailyWaterCount"] = dailyWaterCount;
    doc["autoMode"] = autoMode;
    
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
    }
    server.send(200, "application/json", "{\"status\":\"success\"}");
  });
  
  server.on("/api/stop-water", HTTP_POST, []() {
    stopWaterPump();
    server.send(200, "application/json", "{\"status\":\"success\"}");
  });
  
  // Blink App endpoints
  server.on("/api/blink/water", HTTP_POST, []() {
    manualWater(30);
    server.send(200, "application/json", "{\"status\":\"water_started\"}");
  });
  
  server.on("/api/blink/status", HTTP_GET, []() {
    StaticJsonDocument<256> doc;
    doc["water_active"] = waterPumpActive;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["soil_moisture"] = soilMoisture;
    doc["soil_is_dry"] = soilIsDry;
    doc["daily_water"] = dailyWaterCount;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
} 
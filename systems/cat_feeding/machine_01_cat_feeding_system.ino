/*
 * RDTRC Multi-Machine Cat Feeding System - Machine #01
 * Version: 4.0 - Multi-Machine Architecture
 * Firmware made by: RDTRC
 * Updated: 2024
 * 
 * Machine ID: CAT_FEED_001
 * Location: Kitchen
 * Features:
 * - Automated feeding schedule (6 times/day)
 * - Load cell weight monitoring (HX711)
 * - PIR motion detection
 * - Ultrasonic food level sensor
 * - MQTT communication with coordinator
 * - Blink integration for mobile control
 * - Hotspot fallback mode
 * - Web interface for local control
 */

#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <BlynkSimpleEsp32.h>
#include <HX711.h>
#include <ESP32Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Machine Configuration
#define MACHINE_ID "CAT_FEED_001"
#define MACHINE_NAME "Cat Feeding Station #1"
#define MACHINE_TYPE "cat_feeding"
#define MACHINE_LOCATION "Kitchen"
#define FIRMWARE_VERSION "4.0"
#define FIRMWARE_MAKER "RDTRC"

// Network Configuration
const char* ssid = "RDTRC_IoT_Network";
const char* password = "RDTRC2024!";
const char* hotspot_ssid = "RDTRC_CAT_001";
const char* hotspot_password = "rdtrc123";

// MQTT Configuration
const char* mqtt_server = "192.168.1.10";
const int mqtt_port = 1883;
const char* mqtt_client_id = "cat_feeding_001";
const char* status_topic = "rdtrc/cat_feeding/machine01/status";
const char* data_topic = "rdtrc/cat_feeding/machine01/data";
const char* command_topic = "rdtrc/command/CAT_FEED_001";

// Blynk Configuration
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_DEVICE_NAME "RDTRC Cat Feeder #1"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

// Pin Definitions
#define SERVO_PIN 18
#define LOAD_CELL_DOUT_PIN 19
#define LOAD_CELL_SCK_PIN 21
#define PIR_SENSOR_PIN 22
#define ULTRASONIC_TRIG_PIN 23
#define ULTRASONIC_ECHO_PIN 25
#define BUZZER_PIN 26
#define STATUS_LED_PIN 2
#define RESET_BUTTON_PIN 0

// Feeding Configuration
#define DEFAULT_PORTION_SIZE 30  // grams
#define MIN_PORTION_SIZE 5
#define MAX_PORTION_SIZE 100
#define FOOD_CONTAINER_HEIGHT 20  // cm
#define LOW_FOOD_THRESHOLD 3      // cm

// System Objects
WebServer server(80);
WiFiClient espClient;
PubSubClient mqttClient(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000);
HX711 scale;
Servo feedingServo;

// System Variables
bool isWiFiConnected = false;
bool isMQTTConnected = false;
bool isHotspotMode = false;
unsigned long lastHeartbeat = 0;
unsigned long lastDataSend = 0;
unsigned long bootTime = 0;

// Feeding Variables
int feedingTimes[6] = {7, 11, 15, 18, 21, 23}; // Hours (24h format)
int portionSize = DEFAULT_PORTION_SIZE;
bool feedingInProgress = false;
unsigned long lastFeedingTime = 0;
float currentWeight = 0;
bool catPresent = false;
float foodLevel = 0;
int totalFeedingsToday = 0;

// Calibration values (adjust based on your setup)
float calibrationFactor = -7050.0; // HX711 calibration factor
float tareWeight = 0;

// Function Declarations
void setupSystem();
void setupWiFi();
void setupMQTT();
void setupWebServer();
void setupBlynk();
void displayBootScreen();
void handleSystemLoop();
void handleMQTTMessage(char* topic, byte* payload, unsigned int length);
void checkFeedingSchedule();
void performFeeding(int portion);
void readSensors();
void sendDataToCoordinator();
void handleWebInterface();
void calibrateScale();

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  bootTime = millis();
  displayBootScreen();
  setupSystem();
  
  Serial.println("✅ RDTRC Cat Feeding System Ready!");
  Serial.println("🌐 Web Interface: http://cat-feeder-01.local");
  Serial.println("📡 MQTT Topic: " + String(status_topic));
}

void loop() {
  handleSystemLoop();
  
  // Handle web server
  server.handleClient();
  
  // Handle MQTT
  if (isMQTTConnected) {
    mqttClient.loop();
  }
  
  // Handle Blynk
  if (isWiFiConnected) {
    Blynk.run();
  }
  
  // Check feeding schedule
  checkFeedingSchedule();
  
  // Read sensors every 10 seconds
  static unsigned long lastSensorRead = 0;
  if (millis() - lastSensorRead > 10000) {
    readSensors();
    lastSensorRead = millis();
  }
  
  // Send data to coordinator every 60 seconds
  if (millis() - lastDataSend > 60000) {
    sendDataToCoordinator();
    lastDataSend = millis();
  }
  
  delay(100);
}

void displayBootScreen() {
  Serial.println("\n" + String("=").repeat(60));
  Serial.println("🐱 RDTRC Cat Feeding System");
  Serial.println("Machine ID: " + String(MACHINE_ID));
  Serial.println("Location: " + String(MACHINE_LOCATION));
  Serial.println("");
  Serial.println("Firmware made by: " + String(FIRMWARE_MAKER));
  Serial.println("Version: " + String(FIRMWARE_VERSION));
  Serial.println("");
  Serial.println("Initializing Feeding System...");
  Serial.println("=".repeat(60));
  delay(3000);
}

void setupSystem() {
  // Initialize pins
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize servo
  feedingServo.attach(SERVO_PIN);
  feedingServo.write(0); // Closed position
  
  // Initialize load cell
  scale.begin(LOAD_CELL_DOUT_PIN, LOAD_CELL_SCK_PIN);
  scale.set_scale(calibrationFactor);
  scale.tare(); // Reset to 0
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ SPIFFS initialization failed");
  } else {
    Serial.println("✅ SPIFFS initialized");
  }
  
  // Setup network and services
  setupWiFi();
  setupMQTT();
  setupWebServer();
  setupBlynk();
  
  // Initialize NTP
  timeClient.begin();
  timeClient.update();
  
  // Boot sequence
  for (int i = 0; i < 3; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    tone(BUZZER_PIN, 800, 200);
    delay(300);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(200);
  }
  
  // Initial sensor reading
  readSensors();
  sendDataToCoordinator();
}

void setupWiFi() {
  Serial.println("🔗 Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    isWiFiConnected = true;
    Serial.println("\n✅ WiFi Connected!");
    Serial.println("📍 IP Address: " + WiFi.localIP().toString());
    
    // Setup mDNS
    if (MDNS.begin("cat-feeder-01")) {
      Serial.println("✅ mDNS responder started: cat-feeder-01.local");
    }
  } else {
    Serial.println("\n❌ WiFi connection failed. Starting hotspot...");
    setupHotspot();
  }
}

void setupHotspot() {
  WiFi.softAP(hotspot_ssid, hotspot_password);
  isHotspotMode = true;
  Serial.println("🔥 Hotspot started: " + String(hotspot_ssid));
  Serial.println("📍 Hotspot IP: " + WiFi.softAPIP().toString());
}

void setupMQTT() {
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(handleMQTTMessage);
  
  if (isWiFiConnected) {
    connectMQTT();
  }
}

void connectMQTT() {
  while (!mqttClient.connected() && isWiFiConnected) {
    Serial.println("🔗 Connecting to MQTT...");
    
    if (mqttClient.connect(mqtt_client_id)) {
      isMQTTConnected = true;
      Serial.println("✅ MQTT Connected!");
      
      // Subscribe to command topic
      mqttClient.subscribe(command_topic);
      mqttClient.subscribe("rdtrc/broadcast/discovery");
      
      // Publish initial status
      publishStatus("online");
      
    } else {
      Serial.println("❌ MQTT connection failed. Retry in 5 seconds...");
      delay(5000);
    }
  }
}

void setupWebServer() {
  // Main interface
  server.on("/", handleWebInterface);
  
  // API endpoints
  server.on("/api/status", HTTP_GET, []() {
    JsonDocument doc;
    doc["machine_id"] = MACHINE_ID;
    doc["machine_name"] = MACHINE_NAME;
    doc["type"] = MACHINE_TYPE;
    doc["location"] = MACHINE_LOCATION;
    doc["version"] = FIRMWARE_VERSION;
    doc["uptime"] = millis() - bootTime;
    doc["wifi_connected"] = isWiFiConnected;
    doc["mqtt_connected"] = isMQTTConnected;
    doc["current_weight"] = currentWeight;
    doc["food_level"] = foodLevel;
    doc["cat_present"] = catPresent;
    doc["portion_size"] = portionSize;
    doc["feedings_today"] = totalFeedingsToday;
    doc["timestamp"] = timeClient.getEpochTime();
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/feed", HTTP_POST, []() {
    if (server.hasArg("portion")) {
      int portion = server.arg("portion").toInt();
      if (portion >= MIN_PORTION_SIZE && portion <= MAX_PORTION_SIZE) {
        performFeeding(portion);
        server.send(200, "application/json", "{\"status\":\"feeding_started\"}");
      } else {
        server.send(400, "application/json", "{\"error\":\"invalid_portion_size\"}");
      }
    } else {
      performFeeding(portionSize);
      server.send(200, "application/json", "{\"status\":\"feeding_started\"}");
    }
  });
  
  server.on("/api/settings", HTTP_POST, []() {
    if (server.hasArg("portion_size")) {
      int newPortion = server.arg("portion_size").toInt();
      if (newPortion >= MIN_PORTION_SIZE && newPortion <= MAX_PORTION_SIZE) {
        portionSize = newPortion;
        server.send(200, "application/json", "{\"status\":\"settings_updated\"}");
      } else {
        server.send(400, "application/json", "{\"error\":\"invalid_portion_size\"}");
      }
    }
  });
  
  server.begin();
  Serial.println("✅ Web server started on port 80");
}

void setupBlynk() {
  if (isWiFiConnected) {
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
    Serial.println("✅ Blynk connected");
  }
}

void handleSystemLoop() {
  // Status LED heartbeat
  if (millis() - lastHeartbeat > 2000) {
    digitalWrite(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
    lastHeartbeat = millis();
  }
  
  // Check WiFi connection
  if (!isWiFiConnected && WiFi.status() == WL_CONNECTED) {
    isWiFiConnected = true;
    Serial.println("✅ WiFi reconnected");
    connectMQTT();
    setupBlynk();
  }
  
  // Check MQTT connection
  if (isWiFiConnected && !mqttClient.connected()) {
    isMQTTConnected = false;
    connectMQTT();
  }
  
  // Update time
  timeClient.update();
}

void handleMQTTMessage(char* topic, byte* payload, unsigned int length) {
  String topicStr = String(topic);
  String message = "";
  
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.println("📨 MQTT: " + topicStr + " -> " + message);
  
  if (topicStr == command_topic) {
    JsonDocument doc;
    if (deserializeJson(doc, message) == DeserializationError::Ok) {
      String command = doc["command"];
      
      if (command == "feed") {
        int portion = doc.containsKey("portion") ? doc["portion"].as<int>() : portionSize;
        performFeeding(portion);
      } else if (command == "status") {
        publishStatus("online");
        sendDataToCoordinator();
      } else if (command == "restart") {
        ESP.restart();
      } else if (command == "set_portion") {
        if (doc.containsKey("size")) {
          int newSize = doc["size"];
          if (newSize >= MIN_PORTION_SIZE && newSize <= MAX_PORTION_SIZE) {
            portionSize = newSize;
            publishStatus("portion_updated");
          }
        }
      }
    }
  } else if (topicStr == "rdtrc/broadcast/discovery") {
    // Respond to coordinator discovery
    publishStatus("discovery_response");
  }
}

void checkFeedingSchedule() {
  if (feedingInProgress) return;
  
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  // Check if it's time to feed
  for (int i = 0; i < 6; i++) {
    if (currentHour == feedingTimes[i] && currentMinute == 0) {
      // Check if we already fed at this time today
      unsigned long currentTime = timeClient.getEpochTime();
      if (currentTime - lastFeedingTime > 3600) { // At least 1 hour since last feeding
        Serial.println("⏰ Scheduled feeding time: " + String(feedingTimes[i]) + ":00");
        performFeeding(portionSize);
        break;
      }
    }
  }
}

void performFeeding(int portion) {
  if (feedingInProgress) {
    Serial.println("⚠️ Feeding already in progress");
    return;
  }
  
  feedingInProgress = true;
  lastFeedingTime = timeClient.getEpochTime();
  
  Serial.println("🍽️ Starting feeding: " + String(portion) + "g");
  
  // Pre-feeding alert
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 1000, 200);
    delay(300);
  }
  
  // Record initial weight
  float initialWeight = scale.get_units();
  
  // Open feeding mechanism
  feedingServo.write(90); // Open position
  
  // Calculate feeding duration based on portion size
  int feedingDuration = map(portion, MIN_PORTION_SIZE, MAX_PORTION_SIZE, 1000, 5000);
  delay(feedingDuration);
  
  // Close feeding mechanism
  feedingServo.write(0); // Closed position
  
  delay(2000); // Wait for food to settle
  
  // Record final weight
  float finalWeight = scale.get_units();
  float actualPortion = finalWeight - initialWeight;
  
  Serial.println("✅ Feeding completed. Actual portion: " + String(actualPortion) + "g");
  
  // Success sound
  tone(BUZZER_PIN, 1500, 500);
  delay(600);
  tone(BUZZER_PIN, 2000, 500);
  
  totalFeedingsToday++;
  feedingInProgress = false;
  
  // Publish feeding data
  JsonDocument feedingDoc;
  feedingDoc["machine_id"] = MACHINE_ID;
  feedingDoc["event"] = "feeding_completed";
  feedingDoc["requested_portion"] = portion;
  feedingDoc["actual_portion"] = actualPortion;
  feedingDoc["timestamp"] = timeClient.getEpochTime();
  
  String feedingMsg;
  serializeJson(feedingDoc, feedingMsg);
  mqttClient.publish(data_topic, feedingMsg.c_str());
  
  // Update Blynk
  if (isWiFiConnected) {
    Blynk.virtualWrite(V1, totalFeedingsToday);
    Blynk.virtualWrite(V2, actualPortion);
  }
}

void readSensors() {
  // Read load cell
  currentWeight = scale.get_units();
  
  // Read PIR sensor
  catPresent = digitalRead(PIR_SENSOR_PIN);
  
  // Read ultrasonic sensor for food level
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  
  long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
  foodLevel = FOOD_CONTAINER_HEIGHT - (duration * 0.034 / 2);
  
  // Check for low food alert
  if (foodLevel < LOW_FOOD_THRESHOLD) {
    JsonDocument alertDoc;
    alertDoc["machine_id"] = MACHINE_ID;
    alertDoc["alert_type"] = "low_food";
    alertDoc["food_level"] = foodLevel;
    alertDoc["timestamp"] = timeClient.getEpochTime();
    
    String alertMsg;
    serializeJson(alertDoc, alertMsg);
    mqttClient.publish("rdtrc/alerts/cat_feeding", alertMsg.c_str());
    
    // Alert sound
    for (int i = 0; i < 5; i++) {
      tone(BUZZER_PIN, 500, 200);
      delay(300);
    }
  }
}

void sendDataToCoordinator() {
  if (!isMQTTConnected) return;
  
  JsonDocument dataDoc;
  dataDoc["machine_id"] = MACHINE_ID;
  dataDoc["machine_name"] = MACHINE_NAME;
  dataDoc["type"] = MACHINE_TYPE;
  dataDoc["location"] = MACHINE_LOCATION;
  dataDoc["ip"] = WiFi.localIP().toString();
  dataDoc["current_weight"] = currentWeight;
  dataDoc["food_level"] = foodLevel;
  dataDoc["cat_present"] = catPresent;
  dataDoc["portion_size"] = portionSize;
  dataDoc["feedings_today"] = totalFeedingsToday;
  dataDoc["uptime"] = millis() - bootTime;
  dataDoc["timestamp"] = timeClient.getEpochTime();
  
  String dataMsg;
  serializeJson(dataDoc, dataMsg);
  mqttClient.publish(data_topic, dataMsg.c_str());
}

void publishStatus(String status) {
  if (!isMQTTConnected) return;
  
  JsonDocument statusDoc;
  statusDoc["machine_id"] = MACHINE_ID;
  statusDoc["status"] = status;
  statusDoc["ip"] = WiFi.localIP().toString();
  statusDoc["timestamp"] = timeClient.getEpochTime();
  
  String statusMsg;
  serializeJson(statusDoc, statusMsg);
  mqttClient.publish(status_topic, statusMsg.c_str());
}

void handleWebInterface() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>RDTRC Cat Feeder #1</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #0f1419; color: #e6e6e6; }
        .header { background: linear-gradient(135deg, #ff6b6b 0%, #ffa500 100%); padding: 20px; text-align: center; }
        .header h1 { color: white; margin-bottom: 10px; }
        .container { max-width: 800px; margin: 20px auto; padding: 0 20px; }
        .status-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; margin-bottom: 30px; }
        .status-card { background: #1a1f2e; border-radius: 10px; padding: 20px; text-align: center; }
        .status-card h3 { color: #ff6b6b; margin-bottom: 10px; font-size: 14px; }
        .status-card .value { font-size: 24px; font-weight: bold; }
        .controls { background: #1a1f2e; border-radius: 10px; padding: 20px; margin-bottom: 20px; }
        .control-group { margin-bottom: 20px; }
        .control-group label { display: block; margin-bottom: 5px; color: #a0aec0; }
        .control-group input { width: 100%; padding: 10px; border: 1px solid #4a5568; border-radius: 5px; background: #2d3748; color: #e6e6e6; }
        .btn { padding: 12px 24px; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; margin: 5px; }
        .btn-primary { background: #ff6b6b; color: white; }
        .btn-secondary { background: #4a5568; color: white; }
        .btn:hover { opacity: 0.8; }
        .feeding-log { background: #1a1f2e; border-radius: 10px; padding: 20px; }
    </style>
    <script>
        function feedNow() {
            const portion = document.getElementById('portion').value || 30;
            fetch('/api/feed', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `portion=${portion}`
            })
            .then(response => response.json())
            .then(data => {
                alert('Feeding started!');
                setTimeout(refreshStatus, 2000);
            });
        }
        
        function updateSettings() {
            const portionSize = document.getElementById('default-portion').value;
            fetch('/api/settings', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `portion_size=${portionSize}`
            })
            .then(response => response.json())
            .then(data => {
                alert('Settings updated!');
                refreshStatus();
            });
        }
        
        function refreshStatus() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('weight').textContent = data.current_weight.toFixed(1) + 'g';
                    document.getElementById('food-level').textContent = data.food_level.toFixed(1) + 'cm';
                    document.getElementById('cat-present').textContent = data.cat_present ? 'Yes' : 'No';
                    document.getElementById('feedings-today').textContent = data.feedings_today;
                    document.getElementById('uptime').textContent = Math.floor(data.uptime / 1000 / 60) + ' min';
                });
        }
        
        setInterval(refreshStatus, 10000);
        window.onload = refreshStatus;
    </script>
</head>
<body>
    <div class="header">
        <h1>🐱 RDTRC Cat Feeder #1</h1>
        <p>Kitchen Station - Firmware by RDTRC v4.0</p>
    </div>
    
    <div class="container">
        <div class="status-grid">
            <div class="status-card">
                <h3>Bowl Weight</h3>
                <div class="value" id="weight">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Food Level</h3>
                <div class="value" id="food-level">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Cat Present</h3>
                <div class="value" id="cat-present">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Feedings Today</h3>
                <div class="value" id="feedings-today">Loading...</div>
            </div>
            <div class="status-card">
                <h3>Uptime</h3>
                <div class="value" id="uptime">Loading...</div>
            </div>
        </div>
        
        <div class="controls">
            <h2 style="color: #ff6b6b; margin-bottom: 20px;">🎛️ Manual Controls</h2>
            
            <div class="control-group">
                <label for="portion">Feed Amount (grams):</label>
                <input type="number" id="portion" min="5" max="100" value="30">
                <button class="btn btn-primary" onclick="feedNow()">🍽️ Feed Now</button>
            </div>
            
            <div class="control-group">
                <label for="default-portion">Default Portion Size:</label>
                <input type="number" id="default-portion" min="5" max="100" value="30">
                <button class="btn btn-secondary" onclick="updateSettings()">💾 Save Settings</button>
            </div>
        </div>
        
        <div class="feeding-log">
            <h2 style="color: #ff6b6b; margin-bottom: 15px;">📊 System Information</h2>
            <p><strong>Machine ID:</strong> CAT_FEED_001</p>
            <p><strong>Location:</strong> Kitchen</p>
            <p><strong>Feeding Schedule:</strong> 07:00, 11:00, 15:00, 18:00, 21:00, 23:00</p>
            <p><strong>Portion Range:</strong> 5-100g</p>
        </div>
    </div>
</body>
</html>
  )";
  
  server.send(200, "text/html", html);
}

// Blynk Virtual Pin Handlers
BLYNK_WRITE(V0) {
  if (param.asInt() == 1) {
    performFeeding(portionSize);
  }
}

BLYNK_WRITE(V3) {
  int newPortion = param.asInt();
  if (newPortion >= MIN_PORTION_SIZE && newPortion <= MAX_PORTION_SIZE) {
    portionSize = newPortion;
  }
}

BLYNK_READ(V1) {
  Blynk.virtualWrite(V1, totalFeedingsToday);
}

BLYNK_READ(V2) {
  Blynk.virtualWrite(V2, currentWeight);
}

BLYNK_READ(V4) {
  Blynk.virtualWrite(V4, foodLevel);
}

BLYNK_READ(V5) {
  Blynk.virtualWrite(V5, catPresent ? 1 : 0);
}
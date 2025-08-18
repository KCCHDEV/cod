/*
 * RDTRC Multi-Machine Central Coordinator System
 * Version: 4.0 - Multi-Machine Architecture
 * Firmware made by: RDTRC
 * Updated: 2024
 * 
 * Features:
 * - Central coordination of all IoT machines
 * - MQTT broker and message routing
 * - Web dashboard for monitoring all systems
 * - Data logging and analytics
 * - Alert management and notifications
 * - Automatic system discovery
 * - Remote control capabilities
 */

#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <BlynkSimpleEsp32.h>

// System Configuration
#define FIRMWARE_VERSION "4.0"
#define FIRMWARE_MAKER "RDTRC"
#define SYSTEM_TYPE "COORDINATOR"
#define DEVICE_ID "COORDINATOR_001"

// Network Configuration
const char* ssid = "RDTRC_IoT_Network";
const char* password = "RDTRC2024!";
const char* hotspot_ssid = "RDTRC_COORDINATOR";
const char* hotspot_password = "rdtrc123";

// MQTT Configuration
const char* mqtt_server = "192.168.1.10";
const int mqtt_port = 1883;
const char* mqtt_client_id = "rdtrc_coordinator";

// Blynk Configuration
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_DEVICE_NAME "RDTRC Coordinator"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

// Pin Definitions
#define STATUS_LED_PIN 2
#define BUZZER_PIN 4
#define RESET_BUTTON_PIN 0

// System Objects
WebServer server(80);
WiFiClient espClient;
PubSubClient mqttClient(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); // UTC+7 for Thailand

// System Variables
bool isWiFiConnected = false;
bool isMQTTConnected = false;
bool isHotspotMode = false;
unsigned long lastHeartbeat = 0;
unsigned long lastSystemCheck = 0;
unsigned long bootTime = 0;

// Machine Status Tracking
struct MachineStatus {
  String id;
  String name;
  String type;
  String location;
  String ip;
  bool online;
  unsigned long lastSeen;
  JsonObject lastData;
};

std::vector<MachineStatus> machines;
JsonDocument systemData;
JsonDocument alertsData;

// Function Declarations
void setupSystem();
void setupWiFi();
void setupMQTT();
void setupWebServer();
void setupBlynk();
void displayBootScreen();
void handleSystemLoop();
void handleMQTTMessage(char* topic, byte* payload, unsigned int length);
void discoverMachines();
void updateMachineStatus();
void handleWebDashboard();
void handleAPIEndpoints();
void sendSystemAlerts();
void logSystemData();

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  bootTime = millis();
  displayBootScreen();
  setupSystem();
  
  Serial.println("✅ RDTRC Central Coordinator System Ready!");
  Serial.println("🌐 Web Dashboard: http://coordinator.local or http://192.168.1.10");
  Serial.println("📡 MQTT Broker: 192.168.1.10:1883");
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
  
  // System maintenance tasks
  if (millis() - lastSystemCheck > 30000) { // Every 30 seconds
    updateMachineStatus();
    sendSystemAlerts();
    logSystemData();
    lastSystemCheck = millis();
  }
  
  delay(100);
}

void displayBootScreen() {
  Serial.println("\n" + String("=").repeat(60));
  Serial.println("🤖 RDTRC IoT Multi-Machine System");
  Serial.println("📡 Central Coordinator Hub");
  Serial.println("");
  Serial.println("Firmware made by: " + String(FIRMWARE_MAKER));
  Serial.println("Version: " + String(FIRMWARE_VERSION));
  Serial.println("Device ID: " + String(DEVICE_ID));
  Serial.println("System Type: " + String(SYSTEM_TYPE));
  Serial.println("");
  Serial.println("Initializing Coordinator Systems...");
  Serial.println("=".repeat(60));
  delay(3000);
}

void setupSystem() {
  // Initialize pins
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  
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
  
  // Boot sound
  for (int i = 0; i < 3; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    tone(BUZZER_PIN, 1000, 200);
    delay(300);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(200);
  }
  
  // Start machine discovery
  discoverMachines();
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
    if (MDNS.begin("coordinator")) {
      Serial.println("✅ mDNS responder started: coordinator.local");
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
  while (!mqttClient.connected()) {
    Serial.println("🔗 Connecting to MQTT...");
    
    if (mqttClient.connect(mqtt_client_id)) {
      isMQTTConnected = true;
      Serial.println("✅ MQTT Connected!");
      
      // Subscribe to all machine topics
      mqttClient.subscribe("rdtrc/+/+/status");
      mqttClient.subscribe("rdtrc/+/+/data");
      mqttClient.subscribe("rdtrc/+/+/alert");
      mqttClient.subscribe("rdtrc/coordinator/command");
      
      // Publish coordinator status
      JsonDocument statusDoc;
      statusDoc["device_id"] = DEVICE_ID;
      statusDoc["type"] = SYSTEM_TYPE;
      statusDoc["status"] = "online";
      statusDoc["timestamp"] = timeClient.getEpochTime();
      
      String statusMsg;
      serializeJson(statusDoc, statusMsg);
      mqttClient.publish("rdtrc/coordinator/hub/status", statusMsg.c_str());
      
    } else {
      Serial.println("❌ MQTT connection failed. Retry in 5 seconds...");
      delay(5000);
    }
  }
}

void setupWebServer() {
  // Main dashboard
  server.on("/", handleWebDashboard);
  
  // API endpoints
  server.on("/api/status", HTTP_GET, []() {
    JsonDocument doc;
    doc["system"] = SYSTEM_TYPE;
    doc["version"] = FIRMWARE_VERSION;
    doc["uptime"] = millis() - bootTime;
    doc["wifi_connected"] = isWiFiConnected;
    doc["mqtt_connected"] = isMQTTConnected;
    doc["machines_count"] = machines.size();
    doc["timestamp"] = timeClient.getEpochTime();
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/machines", HTTP_GET, []() {
    JsonDocument doc;
    JsonArray machineArray = doc.createNestedArray("machines");
    
    for (auto& machine : machines) {
      JsonObject machineObj = machineArray.createNestedObject();
      machineObj["id"] = machine.id;
      machineObj["name"] = machine.name;
      machineObj["type"] = machine.type;
      machineObj["location"] = machine.location;
      machineObj["ip"] = machine.ip;
      machineObj["online"] = machine.online;
      machineObj["last_seen"] = machine.lastSeen;
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/control", HTTP_POST, []() {
    if (server.hasArg("machine_id") && server.hasArg("command")) {
      String machineId = server.arg("machine_id");
      String command = server.arg("command");
      
      // Send command via MQTT
      String topic = "rdtrc/command/" + machineId;
      mqttClient.publish(topic.c_str(), command.c_str());
      
      server.send(200, "application/json", "{\"status\":\"command_sent\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"missing_parameters\"}");
    }
  });
  
  // Serve static files
  server.serveStatic("/", SPIFFS, "/");
  
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
  if (millis() - lastHeartbeat > 1000) {
    digitalWrite(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
    lastHeartbeat = millis();
  }
  
  // Check WiFi connection
  if (!isWiFiConnected && WiFi.status() == WL_CONNECTED) {
    isWiFiConnected = true;
    Serial.println("✅ WiFi reconnected");
    connectMQTT();
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
  
  // Parse topic to extract machine info
  // Format: rdtrc/{system_type}/{machine_id}/{message_type}
  int firstSlash = topicStr.indexOf('/', 6); // Skip "rdtrc/"
  int secondSlash = topicStr.indexOf('/', firstSlash + 1);
  int thirdSlash = topicStr.indexOf('/', secondSlash + 1);
  
  if (firstSlash > 0 && secondSlash > 0 && thirdSlash > 0) {
    String systemType = topicStr.substring(6, firstSlash);
    String machineId = topicStr.substring(firstSlash + 1, secondSlash);
    String messageType = topicStr.substring(thirdSlash + 1);
    
    // Update machine status
    updateMachineFromMQTT(machineId, systemType, messageType, message);
  }
  
  // Forward to Blynk if needed
  if (isWiFiConnected) {
    // Send data to Blynk virtual pins based on message type
    if (topicStr.contains("temperature")) {
      JsonDocument doc;
      deserializeJson(doc, message);
      if (doc.containsKey("temperature")) {
        Blynk.virtualWrite(V1, doc["temperature"].as<float>());
      }
    }
  }
}

void updateMachineFromMQTT(String machineId, String systemType, String messageType, String message) {
  // Find or create machine entry
  MachineStatus* machine = nullptr;
  for (auto& m : machines) {
    if (m.id == machineId) {
      machine = &m;
      break;
    }
  }
  
  if (!machine) {
    machines.push_back({machineId, machineId, systemType, "Unknown", "", true, millis(), JsonObject()});
    machine = &machines.back();
  }
  
  // Update machine status
  machine->online = true;
  machine->lastSeen = millis();
  
  // Parse and store data
  JsonDocument doc;
  if (deserializeJson(doc, message) == DeserializationError::Ok) {
    // Store relevant data based on message type
    if (messageType == "status") {
      if (doc.containsKey("ip")) machine->ip = doc["ip"].as<String>();
      if (doc.containsKey("location")) machine->location = doc["location"].as<String>();
    }
  }
}

void discoverMachines() {
  Serial.println("🔍 Discovering machines on network...");
  
  // Send discovery broadcast
  JsonDocument discoveryDoc;
  discoveryDoc["command"] = "discover";
  discoveryDoc["coordinator_id"] = DEVICE_ID;
  discoveryDoc["timestamp"] = timeClient.getEpochTime();
  
  String discoveryMsg;
  serializeJson(discoveryDoc, discoveryMsg);
  
  if (isMQTTConnected) {
    mqttClient.publish("rdtrc/broadcast/discovery", discoveryMsg.c_str());
  }
}

void updateMachineStatus() {
  unsigned long currentTime = millis();
  
  for (auto& machine : machines) {
    // Mark machine offline if not seen for 2 minutes
    if (currentTime - machine.lastSeen > 120000) {
      if (machine.online) {
        machine.online = false;
        Serial.println("⚠️ Machine " + machine.id + " went offline");
        
        // Send alert
        JsonDocument alertDoc;
        alertDoc["type"] = "machine_offline";
        alertDoc["machine_id"] = machine.id;
        alertDoc["timestamp"] = timeClient.getEpochTime();
        
        String alertMsg;
        serializeJson(alertDoc, alertMsg);
        mqttClient.publish("rdtrc/alerts/system", alertMsg.c_str());
      }
    }
  }
}

void handleWebDashboard() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>RDTRC Multi-Machine Dashboard</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #0f1419; color: #e6e6e6; }
        .header { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); padding: 20px; text-align: center; }
        .header h1 { color: white; margin-bottom: 10px; }
        .header p { color: #f0f0f0; opacity: 0.9; }
        .container { max-width: 1200px; margin: 20px auto; padding: 0 20px; }
        .stats-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; margin-bottom: 30px; }
        .stat-card { background: #1a1f2e; border-radius: 10px; padding: 20px; border-left: 4px solid #667eea; }
        .stat-card h3 { color: #667eea; margin-bottom: 10px; }
        .stat-card .value { font-size: 24px; font-weight: bold; color: #e6e6e6; }
        .machines-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; }
        .machine-card { background: #1a1f2e; border-radius: 10px; padding: 20px; border: 1px solid #2d3748; }
        .machine-card.online { border-left: 4px solid #48bb78; }
        .machine-card.offline { border-left: 4px solid #f56565; }
        .machine-header { display: flex; justify-content: between; align-items: center; margin-bottom: 15px; }
        .machine-status { display: inline-block; padding: 4px 8px; border-radius: 4px; font-size: 12px; font-weight: bold; }
        .status-online { background: #48bb78; color: white; }
        .status-offline { background: #f56565; color: white; }
        .machine-info { margin-bottom: 15px; }
        .machine-info div { margin: 5px 0; color: #a0aec0; }
        .controls { display: flex; gap: 10px; }
        .btn { padding: 8px 16px; border: none; border-radius: 4px; cursor: pointer; font-size: 12px; }
        .btn-primary { background: #667eea; color: white; }
        .btn-secondary { background: #4a5568; color: white; }
        .btn:hover { opacity: 0.8; }
        .refresh-btn { position: fixed; bottom: 20px; right: 20px; background: #667eea; color: white; border: none; padding: 15px; border-radius: 50%; cursor: pointer; font-size: 18px; }
    </style>
    <script>
        function refreshData() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => updateSystemStats(data));
            
            fetch('/api/machines')
                .then(response => response.json())
                .then(data => updateMachines(data.machines));
        }
        
        function updateSystemStats(data) {
            document.getElementById('uptime').textContent = Math.floor(data.uptime / 1000 / 60) + ' minutes';
            document.getElementById('wifi-status').textContent = data.wifi_connected ? 'Connected' : 'Disconnected';
            document.getElementById('mqtt-status').textContent = data.mqtt_connected ? 'Connected' : 'Disconnected';
            document.getElementById('machines-count').textContent = data.machines_count;
        }
        
        function updateMachines(machines) {
            const container = document.getElementById('machines-container');
            container.innerHTML = '';
            
            machines.forEach(machine => {
                const card = createMachineCard(machine);
                container.appendChild(card);
            });
        }
        
        function createMachineCard(machine) {
            const div = document.createElement('div');
            div.className = `machine-card ${machine.online ? 'online' : 'offline'}`;
            
            div.innerHTML = `
                <div class="machine-header">
                    <h3>${machine.name}</h3>
                    <span class="machine-status ${machine.online ? 'status-online' : 'status-offline'}">
                        ${machine.online ? 'ONLINE' : 'OFFLINE'}
                    </span>
                </div>
                <div class="machine-info">
                    <div><strong>ID:</strong> ${machine.id}</div>
                    <div><strong>Type:</strong> ${machine.type}</div>
                    <div><strong>Location:</strong> ${machine.location}</div>
                    <div><strong>IP:</strong> ${machine.ip || 'Unknown'}</div>
                    <div><strong>Last Seen:</strong> ${new Date(machine.last_seen).toLocaleString()}</div>
                </div>
                <div class="controls">
                    <button class="btn btn-primary" onclick="controlMachine('${machine.id}', 'status')">Status</button>
                    <button class="btn btn-secondary" onclick="controlMachine('${machine.id}', 'restart')">Restart</button>
                </div>
            `;
            
            return div;
        }
        
        function controlMachine(machineId, command) {
            fetch('/api/control', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: `machine_id=${machineId}&command=${command}`
            })
            .then(response => response.json())
            .then(data => {
                alert(`Command sent to ${machineId}: ${command}`);
            });
        }
        
        // Auto refresh every 30 seconds
        setInterval(refreshData, 30000);
        
        // Initial load
        window.onload = refreshData;
    </script>
</head>
<body>
    <div class="header">
        <h1>🤖 RDTRC Multi-Machine Dashboard</h1>
        <p>Central Coordinator Hub - Firmware by RDTRC v4.0</p>
    </div>
    
    <div class="container">
        <div class="stats-grid">
            <div class="stat-card">
                <h3>System Uptime</h3>
                <div class="value" id="uptime">Loading...</div>
            </div>
            <div class="stat-card">
                <h3>WiFi Status</h3>
                <div class="value" id="wifi-status">Loading...</div>
            </div>
            <div class="stat-card">
                <h3>MQTT Status</h3>
                <div class="value" id="mqtt-status">Loading...</div>
            </div>
            <div class="stat-card">
                <h3>Connected Machines</h3>
                <div class="value" id="machines-count">Loading...</div>
            </div>
        </div>
        
        <h2 style="margin-bottom: 20px; color: #667eea;">🏭 Machine Status</h2>
        <div class="machines-grid" id="machines-container">
            <!-- Machines will be loaded here -->
        </div>
    </div>
    
    <button class="refresh-btn" onclick="refreshData()" title="Refresh Data">🔄</button>
</body>
</html>
  )";
  
  server.send(200, "text/html", html);
}

void sendSystemAlerts() {
  // Implementation for sending system alerts
  // This could include email, LINE notifications, etc.
}

void logSystemData() {
  // Log system data to SPIFFS or external storage
  JsonDocument logDoc;
  logDoc["timestamp"] = timeClient.getEpochTime();
  logDoc["uptime"] = millis() - bootTime;
  logDoc["machines_online"] = 0;
  
  for (auto& machine : machines) {
    if (machine.online) logDoc["machines_online"] = logDoc["machines_online"].as<int>() + 1;
  }
  
  // Save to file (implement file rotation as needed)
  String logData;
  serializeJson(logDoc, logData);
  
  File logFile = SPIFFS.open("/system.log", "a");
  if (logFile) {
    logFile.println(logData);
    logFile.close();
  }
}

// Blynk Virtual Pin Handlers
BLYNK_WRITE(V0) {
  // Remote control commands from Blynk app
  int command = param.asInt();
  if (command == 1) {
    discoverMachines();
  }
}

BLYNK_READ(V1) {
  // Send system status to Blynk
  Blynk.virtualWrite(V1, machines.size());
}
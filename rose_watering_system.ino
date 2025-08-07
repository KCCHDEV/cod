#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

// Rose Garden Configuration
RTC_DS3231 rtc;
const int RELAY_COUNT = 4;
const int relayPins[RELAY_COUNT] = {5, 18, 19, 21};

// Rose-specific settings
const int ROSE_MOISTURE_MIN = 60; // กุหลาบต้องการความชื้นปานกลาง
const int ROSE_MOISTURE_MAX = 85;
const int ROSE_TEMP_MIN = 18;
const int ROSE_TEMP_MAX = 28;
const int ROSE_LIGHT_HOURS = 6; // กุหลาบต้องการแสง 6-8 ชั่วโมง

// WiFi & Blink Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* blinkEmail = "YOUR_BLINK_EMAIL";
const char* blinkPassword = "YOUR_BLINK_PASSWORD";
const char* blinkAccountId = "YOUR_BLINK_ACCOUNT_ID";
const char* blinkNetworkId = "YOUR_BLINK_NETWORK_ID";
const char* blinkCameraId = "YOUR_BLINK_CAMERA_ID";

// Blink API endpoints
const char* blinkAuthUrl = "https://rest-prod.immedia-semi.com/api/v5/account/login";
const char* blinkCameraUrl = "https://rest-prod.immedia-semi.com/api/v1/networks/";
const char* blinkArmUrl = "https://rest-prod.immedia-semi.com/api/v1/networks/";
const char* blinkDisarmUrl = "https://rest-prod.immedia-semi.com/api/v1/networks/";

// System variables
WebServer server(80);
bool relayStates[RELAY_COUNT] = {false, false, false, false};
int wateringCount[RELAY_COUNT] = {0, 0, 0, 0};
int moisturePercent[4] = {65, 72, 58, 78}; // กุหลาบต้องการความชื้นปานกลาง
String moistureStatus[4] = {"Good", "Excellent", "Low", "Excellent"};
unsigned long lastMoistureUpdate = 0;
unsigned long lastWateringTime[RELAY_COUNT] = {0, 0, 0, 0};

// Rose garden tracking
struct RoseBed {
  int bedId;
  String roseType; // "Red Rose", "Pink Rose", "White Rose", "Yellow Rose"
  int growthStage; // 0=Planting, 1=Growing, 2=Budding, 3=Flowering, 4=Pruning
  int daysInStage;
  float temperature;
  int humidity;
  int lightHours;
  bool needsAttention;
  String status;
  int bloomCount;
  String lastPruned;
};

RoseBed roseBeds[4] = {
  {1, "Red Rose", 3, 5, 24.5, 72, 6, false, "Flowering", 3, "2024-01-15"},
  {2, "Pink Rose", 2, 8, 23.8, 78, 5, false, "Budding", 0, "2024-01-10"},
  {3, "White Rose", 1, 15, 25.2, 58, 7, true, "Needs Water", 0, "2024-01-05"},
  {4, "Yellow Rose", 4, 2, 24.0, 82, 6, false, "Ready to Prune", 2, "2024-01-12"}
};

// Blink authentication
String blinkAuthToken = "";
unsigned long lastBlinkAuth = 0;
const unsigned long BLINK_AUTH_INTERVAL = 3600000; // 1 hour

// Demo mode variables
bool demoMode = true;
unsigned long demoStartTime = 0;
int demoCycle = 0;

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
    authenticateBlink();
  } else {
    Serial.println("\nWiFi connection failed - demo mode only");
  }
  
  setupWebServer();
  server.begin();
  
  demoStartTime = millis();
  
  Serial.println("🌹 Rose Garden Watering System Ready!");
  Serial.println("Connected to Blink Security Camera");
}

void loop() {
  server.handleClient();
  updateRoseData();
  checkRoseWatering();
  updateGrowthStages();
  checkBlinkConnection();
  delay(1000);
}

void authenticateBlink() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  http.begin(blinkAuthUrl);
  http.addHeader("Content-Type", "application/json");
  
  StaticJsonDocument<200> doc;
  doc["email"] = blinkEmail;
  doc["password"] = blinkPassword;
  doc["unique_id"] = "ESP32_Rose_Garden";
  doc["client_specifier"] = "ESP32_Rose_Garden";
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  int httpResponseCode = http.POST(jsonString);
  
  if (httpResponseCode == 200) {
    String response = http.getString();
    StaticJsonDocument<1024> responseDoc;
    deserializeJson(responseDoc, response);
    
    if (responseDoc.containsKey("auth")) {
      blinkAuthToken = responseDoc["auth"]["token"].as<String>();
      lastBlinkAuth = millis();
      Serial.println("✅ Blink authentication successful");
    }
  } else {
    Serial.print("❌ Blink authentication failed: ");
    Serial.println(httpResponseCode);
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
  
  int httpResponseCode = http.POST("");
  
  if (httpResponseCode == 200) {
    Serial.println("🔒 Blink camera armed");
  } else {
    Serial.print("❌ Failed to arm Blink camera: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}

void disarmBlinkCamera() {
  if (blinkAuthToken.isEmpty() || WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  String url = blinkDisarmUrl + String(blinkNetworkId) + "/camera/" + String(blinkCameraId) + "/disable";
  http.begin(url);
  http.addHeader("TOKEN_AUTH", blinkAuthToken);
  
  int httpResponseCode = http.POST("");
  
  if (httpResponseCode == 200) {
    Serial.println("🔓 Blink camera disarmed");
  } else {
    Serial.print("❌ Failed to disarm Blink camera: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}

void updateRoseData() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastMoistureUpdate >= 20000) { // 20 seconds for roses
    lastMoistureUpdate = currentTime;
    demoCycle++;
    
    for (int i = 0; i < 4; i++) {
      int change = random(-2, 3); // Smaller changes for roses
      moisturePercent[i] += change;
      
      if (moisturePercent[i] > ROSE_MOISTURE_MAX) moisturePercent[i] = ROSE_MOISTURE_MAX;
      if (moisturePercent[i] < ROSE_MOISTURE_MIN) moisturePercent[i] = ROSE_MOISTURE_MIN;
      
      if (moisturePercent[i] >= 75) {
        moistureStatus[i] = "Excellent";
      } else if (moisturePercent[i] >= 65) {
        moistureStatus[i] = "Good";
      } else if (moisturePercent[i] >= 60) {
        moistureStatus[i] = "Low";
      } else {
        moistureStatus[i] = "Critical";
      }
      
      roseBeds[i].humidity = moisturePercent[i];
      roseBeds[i].temperature = 23.0 + random(-3, 4); // 20-27°C
      roseBeds[i].lightHours = 5 + random(0, 4); // 5-8 hours
      
      if (relayStates[i]) {
        moisturePercent[i] += 2; // Roses respond moderately to watering
        roseBeds[i].humidity = moisturePercent[i];
        if (moisturePercent[i] > ROSE_MOISTURE_MAX) moisturePercent[i] = ROSE_MOISTURE_MAX;
      }
      
      roseBeds[i].needsAttention = (moisturePercent[i] < 65 || roseBeds[i].temperature > 26 || roseBeds[i].lightHours < 5);
    }
    
    Serial.println("🌹 Updated rose garden data:");
    for (int i = 0; i < 4; i++) {
      Serial.print("Rose Bed ");
      Serial.print(i + 1);
      Serial.print(" (");
      Serial.print(roseBeds[i].roseType);
      Serial.print("): ");
      Serial.print(moisturePercent[i]);
      Serial.print("% humidity, ");
      Serial.print(roseBeds[i].temperature, 1);
      Serial.print("°C, ");
      Serial.print(roseBeds[i].lightHours);
      Serial.print("h light, ");
      Serial.print(roseBeds[i].status);
      Serial.println();
    }
  }
}

void updateGrowthStages() {
  static unsigned long lastGrowthUpdate = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastGrowthUpdate >= 7200000) { // 2 hours for roses
    lastGrowthUpdate = currentTime;
    
    for (int i = 0; i < 4; i++) {
      roseBeds[i].daysInStage++;
      
      if (roseBeds[i].growthStage == 1 && roseBeds[i].daysInStage >= 21) {
        roseBeds[i].growthStage = 2;
        roseBeds[i].daysInStage = 0;
        roseBeds[i].status = "Budding";
        Serial.print("🌹 Rose Bed ");
        Serial.print(i + 1);
        Serial.println(" started budding!");
      } else if (roseBeds[i].growthStage == 2 && roseBeds[i].daysInStage >= 14) {
        roseBeds[i].growthStage = 3;
        roseBeds[i].daysInStage = 0;
        roseBeds[i].status = "Flowering";
        roseBeds[i].bloomCount++;
        Serial.print("🌹 Rose Bed ");
        Serial.print(i + 1);
        Serial.println(" started flowering!");
      } else if (roseBeds[i].growthStage == 3 && roseBeds[i].daysInStage >= 21) {
        roseBeds[i].growthStage = 4;
        roseBeds[i].daysInStage = 0;
        roseBeds[i].status = "Ready to Prune";
        Serial.print("🌹 Rose Bed ");
        Serial.print(i + 1);
        Serial.println(" ready for pruning!");
      }
    }
  }
}

void checkRoseWatering() {
  unsigned long currentTime = millis();
  
  for (int i = 0; i < RELAY_COUNT; i++) {
    if (relayStates[i] && (currentTime - lastWateringTime[i] > 30000)) { // 30 seconds for roses
      stopWatering(i);
      Serial.print("🌹 Auto-stopped watering Rose Bed ");
      Serial.println(i + 1);
    }
  }
}

void startWatering(int relayIndex, int duration) {
  if (relayIndex >= 0 && relayIndex < RELAY_COUNT) {
    digitalWrite(relayPins[relayIndex], LOW);
    relayStates[relayIndex] = true;
    lastWateringTime[relayIndex] = millis();
    
    if (wateringCount[relayIndex] < 6) { // Roses need moderate watering
      wateringCount[relayIndex]++;
    }
    
    // Arm Blink camera when watering starts
    armBlinkCamera();
    
    Serial.print("🌹 Started watering Rose Bed ");
    Serial.print(relayIndex + 1);
    Serial.print(" (");
    Serial.print(roseBeds[relayIndex].roseType);
    Serial.print(") for ");
    Serial.print(duration);
    Serial.println(" minutes");
  }
}

void stopWatering(int relayIndex) {
  if (relayIndex >= 0 && relayIndex < RELAY_COUNT) {
    digitalWrite(relayPins[relayIndex], HIGH);
    relayStates[relayIndex] = false;
    
    // Disarm Blink camera when watering stops
    disarmBlinkCamera();
    
    Serial.print("🌹 Stopped watering Rose Bed ");
    Serial.print(relayIndex + 1);
    Serial.print(" (");
    Serial.print(roseBeds[relayIndex].roseType);
    Serial.println(")");
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
    <title>🌹 Rose Garden Watering System</title>
    <script src="https://cdn.tailwindcss.com"></script>
    <link href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css" rel="stylesheet">
    <style>
        .gradient-bg { background: linear-gradient(135deg, #ff6b6b 0%, #feca57 50%, #48dbfb 100%); }
        .glass-effect { background: rgba(255, 255, 255, 0.1); backdrop-filter: blur(10px); border: 1px solid rgba(255, 255, 255, 0.2); }
        .moisture-bar { background: linear-gradient(90deg, #ef4444 0%, #f59e0b 25%, #10b981 50%, #3b82f6 100%); }
        .pulse-animation { animation: pulse 2s infinite; }
        .rose-badge { background: linear-gradient(45deg, #ff6b6b, #feca57); }
        @keyframes pulse { 0%, 100% { opacity: 1; } 50% { opacity: 0.5; } }
        .floating { animation: float 3s ease-in-out infinite; }
        @keyframes float { 0%, 100% { transform: translateY(0px); } 50% { transform: translateY(-10px); } }
        .rose-red { background: linear-gradient(45deg, #ff6b6b, #ee5a52); }
        .rose-pink { background: linear-gradient(45deg, #ff9ff3, #f368e0); }
        .rose-white { background: linear-gradient(45deg, #f8f9fa, #e9ecef); }
        .rose-yellow { background: linear-gradient(45deg, #feca57, #ff9ff3); }
    </style>
</head>
<body class="gradient-bg min-h-screen text-white">
    <div class="container mx-auto px-4 py-8">
        <div class="text-center mb-4">
            <span class="rose-badge px-4 py-2 rounded-full text-white font-bold text-sm floating">
                <i class="fas fa-camera mr-2"></i>Connected to Blink Security
            </span>
        </div>

        <div class="text-center mb-8">
            <h1 class="text-4xl font-bold mb-2">
                <i class="fas fa-rose text-red-400"></i>
                Rose Garden Watering System
            </h1>
            <p class="text-lg opacity-90">Smart Rose Care with Blink Security Integration</p>
            <p class="text-sm opacity-75 mt-2">ระบบรดน้ำกุหลาบไม้ - เชื่อมต่อ Blink Camera</p>
        </div>

        <div class="grid grid-cols-1 md:grid-cols-4 gap-6 mb-8">
            <div class="glass-effect rounded-xl p-6 text-center">
                <i class="fas fa-clock text-3xl text-blue-400 mb-3"></i>
                <h3 class="text-lg font-semibold mb-2">Current Time</h3>
                <div id="currentTime" class="text-2xl font-bold">Loading...</div>
            </div>
            
            <div class="glass-effect rounded-xl p-6 text-center">
                <i class="fas fa-thermometer-half text-3xl text-red-400 mb-3"></i>
                <h3 class="text-lg font-semibold mb-2">Avg Temperature</h3>
                <div id="avgTemp" class="text-2xl font-bold">24.2°C</div>
            </div>
            
            <div class="glass-effect rounded-xl p-6 text-center">
                <i class="fas fa-tint text-3xl text-blue-400 mb-3"></i>
                <h3 class="text-lg font-semibold mb-2">Avg Humidity</h3>
                <div id="avgHumidity" class="text-2xl font-bold">68%</div>
            </div>
            
            <div class="glass-effect rounded-xl p-6 text-center">
                <i class="fas fa-sun text-3xl text-yellow-400 mb-3"></i>
                <h3 class="text-lg font-semibold mb-2">Avg Light</h3>
                <div id="avgLight" class="text-2xl font-bold">6h</div>
            </div>
        </div>

        <div class="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6 mb-8" id="roseGrid">
        </div>

        <div class="glass-effect rounded-xl p-6 mb-8">
            <h3 class="text-xl font-semibold mb-4">
                <i class="fas fa-gamepad text-purple-400"></i>
                Rose Garden Controls
            </h3>
            <div class="grid grid-cols-1 md:grid-cols-4 gap-4">
                <button onclick="simulateRoseGrowth()" class="bg-purple-500 hover:bg-purple-600 px-4 py-3 rounded-lg transition-colors">
                    <i class="fas fa-seedling mr-2"></i>Simulate Growth
                </button>
                <button onclick="resetRoseData()" class="bg-orange-500 hover:bg-orange-600 px-4 py-3 rounded-lg transition-colors">
                    <i class="fas fa-redo mr-2"></i>Reset All Beds
                </button>
                <button onclick="pruneRoses()" class="bg-green-500 hover:bg-green-600 px-4 py-3 rounded-lg transition-colors">
                    <i class="fas fa-cut mr-2"></i>Prune Ready Roses
                </button>
                <button onclick="testBlinkCamera()" class="bg-blue-500 hover:bg-blue-600 px-4 py-3 rounded-lg transition-colors">
                    <i class="fas fa-camera mr-2"></i>Test Blink Camera
                </button>
            </div>
        </div>

        <div class="glass-effect rounded-xl p-6">
            <h3 class="text-xl font-semibold mb-4">
                <i class="fas fa-chart-line text-green-400"></i>
                Rose Growth Statistics
            </h3>
            <div id="statsGrid" class="grid grid-cols-2 md:grid-cols-4 gap-4"></div>
        </div>

        <div class="glass-effect rounded-xl p-6 mt-8">
            <h3 class="text-xl font-semibold mb-4">
                <i class="fas fa-lightbulb text-yellow-400"></i>
                Rose Growing Tips
            </h3>
            <div class="grid grid-cols-1 md:grid-cols-2 gap-6">
                <div>
                    <h4 class="font-semibold mb-2">Optimal Conditions:</h4>
                    <ul class="text-sm space-y-1 opacity-90">
                        <li>• Temperature: 18-28°C</li>
                        <li>• Humidity: 60-85%</li>
                        <li>• Light: 6-8 hours daily</li>
                        <li>• Air circulation: Good</li>
                        <li>• Watering: 2-3 times daily</li>
                    </ul>
                </div>
                <div>
                    <h4 class="font-semibold mb-2">Growth Stages:</h4>
                    <ul class="text-sm space-y-1 opacity-90">
                        <li>• Planting: 0-7 days</li>
                        <li>• Growing: 14-21 days</li>
                        <li>• Budding: 10-14 days</li>
                        <li>• Flowering: 14-21 days</li>
                        <li>• Pruning: Every 3-4 weeks</li>
                    </ul>
                </div>
            </div>
        </div>

        <div class="glass-effect rounded-xl p-6 mt-8">
            <h3 class="text-xl font-semibold mb-4">
                <i class="fas fa-shield-alt text-blue-400"></i>
                Blink Security Status
            </h3>
            <div class="grid grid-cols-1 md:grid-cols-3 gap-4">
                <div class="text-center p-4 bg-white bg-opacity-10 rounded-lg">
                    <i class="fas fa-camera text-3xl text-blue-400 mb-2"></i>
                    <div class="font-semibold">Camera Status</div>
                    <div id="blinkStatus" class="text-green-400">Connected</div>
                </div>
                <div class="text-center p-4 bg-white bg-opacity-10 rounded-lg">
                    <i class="fas fa-eye text-3xl text-green-400 mb-2"></i>
                    <div class="font-semibold">Monitoring</div>
                    <div id="monitoringStatus" class="text-green-400">Active</div>
                </div>
                <div class="text-center p-4 bg-white bg-opacity-10 rounded-lg">
                    <i class="fas fa-bell text-3xl text-orange-400 mb-2"></i>
                    <div class="font-semibold">Alerts</div>
                    <div id="alertStatus" class="text-green-400">Enabled</div>
                </div>
            </div>
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
        
        function updateRoses() {
            fetch('/api/roses')
                .then(response => response.json())
                .then(data => {
                    const grid = document.getElementById('roseGrid');
                    grid.innerHTML = '';
                    
                    let activeCount = 0;
                    let totalTemp = 0;
                    let totalHumidity = 0;
                    let totalLight = 0;
                    
                    data.roses.forEach((rose, index) => {
                        if (rose.watering) activeCount++;
                        totalTemp += rose.temperature;
                        totalHumidity += rose.humidity;
                        totalLight += rose.lightHours;
                        
                        const card = document.createElement('div');
                        let cardClass = 'glass-effect rounded-xl p-6 text-center transition-all duration-300 hover:scale-105';
                        let statusColor = 'text-green-400';
                        let statusIcon = 'fas fa-rose';
                        let roseClass = 'rose-red';
                        
                        if (rose.roseType.includes('Pink')) roseClass = 'rose-pink';
                        else if (rose.roseType.includes('White')) roseClass = 'rose-white';
                        else if (rose.roseType.includes('Yellow')) roseClass = 'rose-yellow';
                        
                        if (rose.watering) {
                            cardClass += ' pulse-animation';
                            statusColor = 'text-blue-400';
                            statusIcon = 'fas fa-tint';
                        } else if (rose.needsAttention) {
                            statusColor = 'text-red-400';
                            statusIcon = 'fas fa-exclamation-triangle';
                        } else if (rose.growthStage === 4) {
                            statusColor = 'text-yellow-400';
                            statusIcon = 'fas fa-cut';
                        }
                        
                        card.className = cardClass;
                        card.innerHTML = `
                            <div class="mb-4">
                                <div class="${roseClass} w-16 h-16 rounded-full mx-auto mb-2 flex items-center justify-center">
                                    <i class="${statusIcon} text-2xl text-white"></i>
                                </div>
                            </div>
                            <h3 class="text-xl font-bold mb-2">${rose.roseType}</h3>
                            <div class="space-y-2 mb-4">
                                <div class="flex justify-between">
                                    <span>Status:</span>
                                    <span class="font-semibold ${statusColor}">${rose.watering ? 'Watering' : 'Idle'}</span>
                                </div>
                                <div class="flex justify-between">
                                    <span>Humidity:</span>
                                    <span class="font-semibold">${rose.humidity}%</span>
                                </div>
                                <div class="flex justify-between">
                                    <span>Temperature:</span>
                                    <span class="font-semibold">${rose.temperature}°C</span>
                                </div>
                                <div class="flex justify-between">
                                    <span>Light:</span>
                                    <span class="font-semibold">${rose.lightHours}h</span>
                                </div>
                                <div class="flex justify-between">
                                    <span>Growth:</span>
                                    <span class="font-semibold text-purple-400">${rose.status}</span>
                                </div>
                                <div class="flex justify-between">
                                    <span>Blooms:</span>
                                    <span class="font-semibold">${rose.bloomCount}</span>
                                </div>
                                <div class="flex justify-between">
                                    <span>Today:</span>
                                    <span class="font-semibold">${rose.dailyCount}/6</span>
                                </div>
                            </div>
                            <div class="w-full bg-gray-700 rounded-full h-3 mb-4">
                                <div class="moisture-bar h-3 rounded-full transition-all duration-500" 
                                     style="width: ${rose.humidity}%"></div>
                            </div>
                            <div class="grid grid-cols-2 gap-2">
                                <button onclick="waterRose(${index}, 3)" 
                                        class="bg-green-500 hover:bg-green-600 px-3 py-2 rounded-lg text-sm transition-colors">
                                    <i class="fas fa-tint mr-1"></i>3min
                                </button>
                                <button onclick="waterRose(${index}, 5)" 
                                        class="bg-blue-500 hover:bg-blue-600 px-3 py-2 rounded-lg text-sm transition-colors">
                                    <i class="fas fa-tint mr-1"></i>5min
                                </button>
                                <button onclick="stopWatering(${index})" 
                                        class="bg-red-500 hover:bg-red-600 px-3 py-2 rounded-lg text-sm transition-colors col-span-2">
                                    <i class="fas fa-stop mr-1"></i>Stop
                                </button>
                            </div>
                        `;
                        grid.appendChild(card);
                    });
                    
                    document.getElementById('avgTemp').textContent = (totalTemp / data.roses.length).toFixed(1) + '°C';
                    document.getElementById('avgHumidity').textContent = Math.round(totalHumidity / data.roses.length) + '%';
                    document.getElementById('avgLight').textContent = Math.round(totalLight / data.roses.length) + 'h';
                });
        }
        
        function waterRose(index, duration) {
            fetch('/api/water', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({index: index, duration: duration})
            })
            .then(() => updateRoses());
        }
        
        function stopWatering(index) {
            fetch('/api/stop', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({index: index})
            })
            .then(() => updateRoses());
        }
        
        function updateStats() {
            fetch('/api/stats')
                .then(response => response.json())
                .then(data => {
                    const grid = document.getElementById('statsGrid');
                    grid.innerHTML = '';
                    
                    data.stats.forEach((stat, index) => {
                        const card = document.createElement('div');
                        card.className = 'text-center p-4 bg-white bg-opacity-10 rounded-lg';
                        card.innerHTML = `
                            <div class="text-2xl font-bold text-green-400">${stat.today}</div>
                            <div class="text-sm opacity-90">${stat.roseType} Today</div>
                        `;
                        grid.appendChild(card);
                    });
                });
        }
        
        function simulateRoseGrowth() {
            fetch('/api/demo/growth', {method: 'POST'})
                .then(() => {
                    alert('Rose growth simulated!');
                    updateRoses();
                });
        }
        
        function resetRoseData() {
            fetch('/api/demo/reset', {method: 'POST'})
                .then(() => {
                    alert('All rose beds reset!');
                    updateRoses();
                    updateStats();
                });
        }
        
        function pruneRoses() {
            fetch('/api/demo/prune', {method: 'POST'})
                .then(() => {
                    alert('Ready roses pruned!');
                    updateRoses();
                });
        }
        
        function testBlinkCamera() {
            fetch('/api/blink/test', {method: 'POST'})
                .then(() => {
                    alert('Blink camera test completed!');
                });
        }
        
        setInterval(() => {
            updateTime();
            updateRoses();
            updateStats();
        }, 3000);
        
        updateTime();
        updateRoses();
        updateStats();
    </script>
</body>
</html>
    )";
    server.send(200, "text/html", html);
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
  
  server.on("/api/roses", HTTP_GET, []() {
    StaticJsonDocument<4096> doc;
    JsonArray roses = doc.createNestedArray("roses");
    
    for (int i = 0; i < RELAY_COUNT; i++) {
      JsonObject rose = roses.createNestedObject();
      rose["index"] = i;
      rose["watering"] = relayStates[i];
      rose["dailyCount"] = wateringCount[i];
      rose["humidity"] = moisturePercent[i];
      rose["temperature"] = roseBeds[i].temperature;
      rose["lightHours"] = roseBeds[i].lightHours;
      rose["growthStage"] = roseBeds[i].growthStage;
      rose["status"] = roseBeds[i].status;
      rose["needsAttention"] = roseBeds[i].needsAttention;
      rose["daysInStage"] = roseBeds[i].daysInStage;
      rose["roseType"] = roseBeds[i].roseType;
      rose["bloomCount"] = roseBeds[i].bloomCount;
      rose["lastPruned"] = roseBeds[i].lastPruned;
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
      
      if (wateringCount[index] < 6) {
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
      stat["temperature"] = roseBeds[i].temperature;
      stat["roseType"] = roseBeds[i].roseType;
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  server.on("/api/demo/growth", HTTP_POST, []() {
    for (int i = 0; i < RELAY_COUNT; i++) {
      if (roseBeds[i].growthStage < 4) {
        roseBeds[i].daysInStage += 3;
        
        if (roseBeds[i].growthStage == 1 && roseBeds[i].daysInStage >= 21) {
          roseBeds[i].growthStage = 2;
          roseBeds[i].daysInStage = 0;
          roseBeds[i].status = "Budding";
        } else if (roseBeds[i].growthStage == 2 && roseBeds[i].daysInStage >= 14) {
          roseBeds[i].growthStage = 3;
          roseBeds[i].daysInStage = 0;
          roseBeds[i].status = "Flowering";
          roseBeds[i].bloomCount++;
        } else if (roseBeds[i].growthStage == 3 && roseBeds[i].daysInStage >= 21) {
          roseBeds[i].growthStage = 4;
          roseBeds[i].daysInStage = 0;
          roseBeds[i].status = "Ready to Prune";
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
      
      roseBeds[i].growthStage = 1;
      roseBeds[i].daysInStage = 0;
      roseBeds[i].status = "Growing";
      roseBeds[i].needsAttention = false;
      roseBeds[i].bloomCount = 0;
    }
    
    moisturePercent[0] = 65;
    moisturePercent[1] = 72;
    moisturePercent[2] = 58;
    moisturePercent[3] = 78;
    
    server.send(200, "application/json", "{\"status\":\"beds_reset\"}");
  });
  
  server.on("/api/demo/prune", HTTP_POST, []() {
    for (int i = 0; i < RELAY_COUNT; i++) {
      if (roseBeds[i].growthStage == 4) {
        roseBeds[i].growthStage = 1;
        roseBeds[i].daysInStage = 0;
        roseBeds[i].status = "Growing";
        roseBeds[i].lastPruned = "2024-01-20";
        Serial.print("🌹 Pruned Rose Bed ");
        Serial.println(i + 1);
      }
    }
    
    server.send(200, "application/json", "{\"status\":\"pruned\"}");
  });
  
  server.on("/api/blink/test", HTTP_POST, []() {
    armBlinkCamera();
    delay(2000);
    disarmBlinkCamera();
    
    server.send(200, "application/json", "{\"status\":\"blink_test_completed\"}");
  });
}

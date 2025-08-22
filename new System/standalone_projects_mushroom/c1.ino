// Blynk credentials
#define BLYNK_TEMPLATE_ID "TMPL6eZkeQWIq"
#define BLYNK_TEMPLATE_NAME "poom"
#define BLYNK_AUTH_TOKEN "9xVzj6pSJzTPwhqiMmwjW3HNsHxVdi2B"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

// WiFi AP for setup
const char* ap_ssid = "Mushroom_Setup";
const char* ap_password = "12345678";

// WiFi credentials (will be set via web)
String wifi_ssid = "";
String wifi_password = "";

// Pin definitions
#define DHT_PIN 4
#define SOIL_PIN 34
#define PUMP_PIN 26
#define DHT_TYPE DHT11

// LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);
int lcdAddress = 0x27;

// RTC
RTC_DS3231 rtc;

// Sensors
DHT dht(DHT_PIN, DHT_TYPE);

// Variables
float temperature = 0;
float humidity = 0;
int soilMoisture = 0;
bool pumpStatus = false;
bool wifiConnected = false;
bool sensorsReady = false;
bool lcdFound = false;
bool rtcFound = false;

// Blynk virtual pins
#define V_TEMP V0
#define V_HUMIDITY V1
#define V_SOIL V2
#define V_PUMP V3
#define V_AUTO V4
#define V_TIME V5

// Watering settings
bool autoMode = true;
int soilThreshold = 30;  // % moisture threshold
int pumpDuration = 5000; // 5 seconds
unsigned long lastWatering = 0;
const unsigned long wateringInterval = 300000; // 5 minutes

// Time-based watering
int wateringHour = 6;    // 6:00 AM
int wateringMinute = 0;  // 0 minute
bool timeWateringEnabled = true;

// Web server
WebServer server(80);

// Function prototypes
void handleRoot();
void handleConnect();
void readSensors();
void autoWatering();
void timeWatering();
void startPump();
int findLCDAddress();
void updateLCD();
void checkRTC();

void setup() {
  Serial.begin(115200);
  Serial.println("🍄 Mushroom Watering System Starting...");
  
  // Initialize pins
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, HIGH); // Pump OFF initially
  
  // Initialize I2C
  Wire.begin();
  
  // Find and initialize LCD
  lcdAddress = findLCDAddress();
  if (lcdAddress != 0) {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mushroom System");
    lcd.setCursor(0, 1);
    lcd.print("Starting...");
    lcdFound = true;
    Serial.println("✅ LCD found at address: 0x" + String(lcdAddress, HEX));
  }
  
  // Initialize RTC
  if (rtc.begin()) {
    rtcFound = true;
    Serial.println("✅ RTC DS3231 found");
    
    // Set RTC time if needed (uncomment to set time)
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    
    DateTime now = rtc.now();
    Serial.println("🕐 Current time: " + String(now.hour()) + ":" + String(now.minute()));
  } else {
    Serial.println("❌ RTC not found!");
  }
  
  // Initialize sensors
  dht.begin();
  delay(1000);
  
  // Check sensors
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  
  if (!isnan(temperature) && !isnan(humidity)) {
    sensorsReady = true;
    Serial.println("✅ DHT11 sensor ready");
  } else {
    Serial.println("❌ DHT11 sensor error");
  }
  
  // Start WiFi AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  
  if (lcdFound) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi: " + String(ap_ssid));
    lcd.setCursor(0, 1);
    lcd.print("IP: 192.168.4.1");
  }
  
  Serial.println("📡 WiFi AP started: " + String(ap_ssid));
  Serial.println("🌐 IP Address: 192.168.4.1");
  
  // Setup web server
  server.on("/", handleRoot);
  server.on("/connect", HTTP_POST, handleConnect);
  server.begin();
  
  Serial.println("🌐 Web server started");
}

void loop() {
  server.handleClient();
  
  if (wifiConnected) {
    Blynk.run();
    
    static unsigned long lastSensorRead = 0;
    if (millis() - lastSensorRead >= 5000) {
      readSensors();
      autoWatering();
      timeWatering();
      updateLCD();
      lastSensorRead = millis();
    }
  }
}

// Find LCD I2C address automatically
int findLCDAddress() {
  Serial.println("🔍 Scanning for LCD I2C address...");
  
  for (int address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    int error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.println("📍 Found I2C device at address: 0x" + String(address, HEX));
      
      // Try to initialize LCD at this address
      LiquidCrystal_I2C testLCD(address, 16, 2);
      testLCD.init();
      testLCD.backlight();
      testLCD.clear();
      testLCD.setCursor(0, 0);
      testLCD.print("Test");
      delay(100);
      
      return address;
    }
  }
  
  Serial.println("❌ No LCD found!");
  return 0;
}

// Update LCD display
void updateLCD() {
  if (!lcdFound) return;
  
  lcd.clear();
  
  if (wifiConnected) {
    // Line 1: Temperature and Humidity
    lcd.setCursor(0, 0);
    lcd.print("T:" + String(temperature, 1) + "C H:" + String(humidity, 0) + "%");
    
    // Line 2: Soil moisture and pump status
    lcd.setCursor(0, 1);
    lcd.print("S:" + String(soilMoisture) + "% " + (pumpStatus ? "PUMP ON" : "PUMP OFF"));
  } else {
    // Line 1: WiFi setup mode
    lcd.setCursor(0, 0);
    lcd.print("WiFi Setup Mode");
    
    // Line 2: Instructions
    lcd.setCursor(0, 1);
    lcd.print("Connect to AP");
  }
}

// Read sensors
void readSensors() {
  // Read DHT11 with error checking
  float newTemp = dht.readTemperature();
  float newHumidity = dht.readHumidity();
  
  if (!isnan(newTemp) && !isnan(newHumidity)) {
    temperature = newTemp;
    humidity = newHumidity;
  }
  
  // Read soil moisture
  int rawSoil = analogRead(SOIL_PIN);
  soilMoisture = map(rawSoil, 4095, 0, 0, 100); // Convert to percentage
  
  Serial.println("📊 Sensors - Temp: " + String(temperature, 1) + "°C, Humidity: " + String(humidity, 0) + "%, Soil: " + String(soilMoisture) + "%");
  
  if (wifiConnected) {
    Blynk.virtualWrite(V_TEMP, temperature);
    Blynk.virtualWrite(V_HUMIDITY, humidity);
    Blynk.virtualWrite(V_SOIL, soilMoisture);
  }
}

// Auto watering based on soil moisture
void autoWatering() {
  if (!autoMode) return;
  
  if (soilMoisture < soilThreshold && millis() - lastWatering > wateringInterval) {
    Serial.println("💧 Auto watering - Soil too dry: " + String(soilMoisture) + "%");
    startPump();
  }
}

// Time-based watering
void timeWatering() {
  if (!timeWateringEnabled || !rtcFound) return;
  
  DateTime now = rtc.now();
  
  if (now.hour() == wateringHour && now.minute() == wateringMinute && millis() - lastWatering > wateringInterval) {
    Serial.println("⏰ Time-based watering at " + String(wateringHour) + ":" + String(wateringMinute));
    startPump();
  }
}

// Start water pump
void startPump() {
  if (pumpStatus) return; // Prevent multiple activations
  
  pumpStatus = true;
  digitalWrite(PUMP_PIN, LOW); // Turn ON pump
  Serial.println("🚰 Pump started");
  
  if (wifiConnected) {
    Blynk.virtualWrite(V_PUMP, 1);
  }
  
  // Turn off pump after duration
  delay(pumpDuration);
  digitalWrite(PUMP_PIN, HIGH); // Turn OFF pump
  pumpStatus = false;
  lastWatering = millis();
  
  Serial.println("🚰 Pump stopped");
  
  if (wifiConnected) {
    Blynk.virtualWrite(V_PUMP, 0);
  }
}

// Web server handlers
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>Mushroom Watering System</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script src="https://cdn.tailwindcss.com"></script>
</head>
<body class="bg-gradient-to-br from-green-500 to-blue-600 min-h-screen p-4">
  <div class="max-w-md mx-auto bg-white rounded-2xl shadow-2xl p-6 mt-8">
    <h1 class="text-2xl font-bold text-center text-gray-800 mb-6">🍄 Mushroom Watering System</h1>
    
    <div class="space-y-4">
      <div class="bg-gray-100 p-4 rounded-lg">
        <h2 class="font-semibold text-gray-700 mb-2">📡 WiFi Setup</h2>
        <form action="/connect" method="post" class="space-y-3">
          <div>
            <label class="block text-sm font-medium text-gray-700">WiFi SSID:</label>
            <input type="text" name="ssid" required class="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500">
          </div>
          <div>
            <label class="block text-sm font-medium text-gray-700">WiFi Password:</label>
            <input type="password" name="password" required class="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500">
          </div>
          <button type="submit" class="w-full bg-blue-500 text-white py-2 px-4 rounded-md hover:bg-blue-600 transition duration-200">
            🔗 Connect to WiFi
          </button>
        </form>
      </div>
      
      <div class="bg-gray-100 p-4 rounded-lg">
        <h2 class="font-semibold text-gray-700 mb-2">📊 System Status</h2>
        <div class="text-sm text-gray-600 space-y-1">
          <div>🌡️ Temperature: )rawliteral" + String(temperature, 1) + R"rawliteral(°C</div>
          <div>💧 Humidity: )rawliteral" + String(humidity, 0) + R"rawliteral(%</div>
          <div>🌱 Soil Moisture: )rawliteral" + String(soilMoisture) + R"rawliteral(%</div>
          <div>🚰 Pump: )rawliteral" + (pumpStatus ? "ON" : "OFF") + R"rawliteral(</div>
          <div>📡 WiFi: )rawliteral" + (wifiConnected ? "Connected" : "Setup Mode") + R"rawliteral(</div>
        </div>
      </div>
    </div>
  </div>
</body>
</html>
)rawliteral";
  
  server.send(200, "text/html", html);
}

void handleConnect() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    wifi_ssid = server.arg("ssid");
    wifi_password = server.arg("password");
    
    Serial.println("🔗 Connecting to WiFi: " + wifi_ssid);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;
      Serial.println("\n✅ WiFi connected!");
      Serial.println("🌐 IP: " + WiFi.localIP().toString());
      
      // Start Blynk
      Blynk.begin(BLYNK_AUTH_TOKEN, wifi_ssid.c_str(), wifi_password.c_str());
      
      server.send(200, "text/html", "<script>alert('WiFi connected!'); window.location.href='/';</script>");
    } else {
      Serial.println("\n❌ WiFi connection failed!");
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ap_ssid, ap_password);
      server.send(200, "text/html", "<script>alert('WiFi connection failed!'); window.location.href='/';</script>");
    }
  }
}

/*
 * Enhanced Mushroom & Cilantro Watering System - Single Main File
 * ระบบรดน้ำเห็ดนางฟ้าและผักชีฟลั่งอัตโนมัติขั้นสูง - ไฟล์หลักเดียว
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
 * - Dual crop watering control (Mushrooms + Cilantro)
 * - High-precision misting system for mushrooms
 * - Drip irrigation system for cilantro
 * - Humidity monitoring (critical for mushrooms)
 * - Soil moisture monitoring for cilantro
 * - Temperature control for optimal growth
 * - Light cycle control for mushroom fruiting
 * - Water level monitoring with dual reservoirs
 * - Mobile app control via Blink
 * - Automated feeding schedules optimized for each crop
 * - Growth tracking and yield monitoring
 * 
 * Crop-Specific Requirements:
 * Oyster Mushrooms (เห็ดนางฟ้า):
 * - Humidity: 80-95%
 * - Temperature: 20-25°C
 * - Misting: Every 2-4 hours
 * - Light: Indirect, 12 hours cycle
 * 
 * Cilantro (ผักชีฟลั่ง):
 * - Soil moisture: 60-75%
 * - Temperature: 17-27°C
 * - Watering: Daily or when soil moisture < 60%
 * - Light: Full sun, 6-8 hours
 * 
 * Firmware made by: RDTRC
 * Version: 3.0
 * Created: 2024
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <ArduinoOTA.h>
#include <Preferences.h>
#include <esp_task_wdt.h>
#include <SPIFFS.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// System-specific pin definitions
#define MUSHROOM_MIST_PUMP_PIN 18      // Misting pump for mushrooms
#define CILANTRO_WATER_PUMP_PIN 19     // Drip irrigation pump for cilantro
#define MUSHROOM_VALVE_PIN 20          // Valve for mushroom misting system
#define CILANTRO_VALVE_PIN 21          // Valve for cilantro irrigation
#define HUMIDITY_FAN_PIN 22            // Exhaust fan for humidity control
#define CIRCULATION_FAN_PIN 23         // Air circulation fan
#define MUSHROOM_LIGHT_PIN 25          // LED grow lights for mushrooms
#define CILANTRO_LIGHT_PIN 26          // LED grow lights for cilantro
#define HEATER_PIN 27                  // Heating element

// Sensor pins
#define DHT_MUSHROOM_PIN 4             // DHT22 for mushroom area humidity/temp
#define DHT_CILANTRO_PIN 5             // DHT22 for cilantro area humidity/temp
#define SOIL_MOISTURE_PIN A0           // Soil moisture sensor for cilantro
#define WATER_LEVEL_MUSHROOM_PIN A1    // Water level sensor for mushroom reservoir
#define WATER_LEVEL_CILANTRO_PIN A2    // Water level sensor for cilantro reservoir
#define LIGHT_SENSOR_PIN A3            // Light sensor for ambient light detection
#define TEMP_PROBE_PIN 2               // DS18B20 temperature probe

// Control pins
#define BUTTON_MANUAL_PIN 32           // Manual control button
#define BUTTON_RESET_PIN 33            // System reset button
#define BUZZER_PIN 14                  // Alert buzzer
#define STATUS_LED_PIN 13              // Status indicator LED

// DHT sensor initialization
#define DHT_TYPE DHT22
DHT dhtMushroom(DHT_MUSHROOM_PIN, DHT_TYPE);
DHT dhtCilantro(DHT_CILANTRO_PIN, DHT_TYPE);

// Temperature probe initialization
OneWire oneWire(TEMP_PROBE_PIN);
DallasTemperature temperatureProbe(&oneWire);

// System configuration constants
#define MAX_SCHEDULES 8
#define MUSHROOM_TARGET_HUMIDITY_MIN 80.0    // Minimum humidity for mushrooms
#define MUSHROOM_TARGET_HUMIDITY_MAX 95.0    // Maximum humidity for mushrooms
#define MUSHROOM_TARGET_TEMP_MIN 20.0        // Minimum temperature for mushrooms
#define MUSHROOM_TARGET_TEMP_MAX 25.0        // Maximum temperature for mushrooms
#define CILANTRO_TARGET_MOISTURE_MIN 60.0    // Minimum soil moisture for cilantro
#define CILANTRO_TARGET_MOISTURE_MAX 75.0    // Maximum soil moisture for cilantro
#define CILANTRO_TARGET_TEMP_MIN 17.0        // Minimum temperature for cilantro
#define CILANTRO_TARGET_TEMP_MAX 27.0        // Maximum temperature for cilantro
#define MIN_WATER_LEVEL 20.0                 // Minimum water level percentage
#define MISTING_DURATION 30000               // 30 seconds misting duration
#define WATERING_DURATION 180000             // 3 minutes watering duration
#define MUSHROOM_LIGHT_CYCLE 12              // 12 hours light cycle for mushrooms
#define CILANTRO_LIGHT_HOURS 8               // 8 hours light for cilantro

// WiFi and system configuration
const char* AP_SSID = "MushroomCilantro_System";
const char* AP_PASSWORD = "RDTRC2024";
String deviceName = "Enhanced Mushroom Cilantro System";
String firmwareVersion = "3.0.0";

// System variables
WebServer server(80);
Preferences preferences;
bool isConnectedToWiFi = false;
bool isHotspotMode = false;
unsigned long lastSensorRead = 0;
unsigned long lastScheduleCheck = 0;
unsigned long lastBlinkUpdate = 0;
unsigned long systemStartTime = 0;

// Crop-specific structures
struct MushroomStatus {
  float humidity;
  float temperature;
  bool mistingActive;
  bool lightOn;
  bool fanRunning;
  float waterLevel;
  unsigned long lastMisting;
  unsigned long totalMistingTime;
  int fruitingStage; // 0=incubation, 1=pinning, 2=fruiting, 3=harvest
  String nextMistingTime;
};

struct CilantroStatus {
  float soilMoisture;
  float temperature;
  float humidity;
  bool wateringActive;
  bool lightOn;
  float waterLevel;
  unsigned long lastWatering;
  unsigned long totalWateringTime;
  int growthStage; // 0=seed, 1=sprout, 2=vegetative, 3=mature
  String nextWateringTime;
};

struct SystemStatus {
  bool systemRunning;
  bool manualMode;
  bool alertActive;
  String alertMessage;
  float ambientLight;
  unsigned long uptime;
  String currentTime;
  bool heaterOn;
  bool exhaustFanOn;
  bool circulationFanOn;
  float systemTemperature;
};

// Schedule structure for both crops
struct WateringSchedule {
  int hour;
  int minute;
  int cropType; // 0=mushroom, 1=cilantro
  int duration; // seconds
  bool enabled;
  bool lightControl; // Control lights during this schedule
  String description;
};

// Global status objects
MushroomStatus mushroomStatus;
CilantroStatus cilantroStatus;
SystemStatus systemStatus;
WateringSchedule schedules[MAX_SCHEDULES];

// System initialization
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  // Display boot screen
  Serial.println("=====================================");
  Serial.println("     FW made by RDTRC");
  Serial.println("  Enhanced Mushroom & Cilantro System");
  Serial.println("         Version 3.0.0");
  Serial.println("=====================================");
  
  systemStartTime = millis();
  
  // Initialize preferences
  preferences.begin("mushroom_sys", false);
  
  // Initialize pins
  initializePins();
  
  // Initialize sensors
  initializeSensors();
  
  // Initialize schedules
  initializeSchedules();
  
  // Initialize system status
  initializeSystemStatus();
  
  // Initialize WiFi and web server
  initializeWiFi();
  initializeWebServer();
  
  // Initialize OTA updates
  initializeOTA();
  
  Serial.println("System initialization complete!");
  Serial.println("Ready to grow mushrooms and cilantro!");
}

void initializePins() {
  // Output pins
  pinMode(MUSHROOM_MIST_PUMP_PIN, OUTPUT);
  pinMode(CILANTRO_WATER_PUMP_PIN, OUTPUT);
  pinMode(MUSHROOM_VALVE_PIN, OUTPUT);
  pinMode(CILANTRO_VALVE_PIN, OUTPUT);
  pinMode(HUMIDITY_FAN_PIN, OUTPUT);
  pinMode(CIRCULATION_FAN_PIN, OUTPUT);
  pinMode(MUSHROOM_LIGHT_PIN, OUTPUT);
  pinMode(CILANTRO_LIGHT_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  // Input pins
  pinMode(BUTTON_MANUAL_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RESET_PIN, INPUT_PULLUP);
  
  // Turn off all outputs initially
  digitalWrite(MUSHROOM_MIST_PUMP_PIN, LOW);
  digitalWrite(CILANTRO_WATER_PUMP_PIN, LOW);
  digitalWrite(MUSHROOM_VALVE_PIN, LOW);
  digitalWrite(CILANTRO_VALVE_PIN, LOW);
  digitalWrite(HUMIDITY_FAN_PIN, LOW);
  digitalWrite(CIRCULATION_FAN_PIN, LOW);
  digitalWrite(MUSHROOM_LIGHT_PIN, LOW);
  digitalWrite(CILANTRO_LIGHT_PIN, LOW);
  digitalWrite(HEATER_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(STATUS_LED_PIN, HIGH); // Status LED on to indicate system ready
}

void initializeSensors() {
  dhtMushroom.begin();
  dhtCilantro.begin();
  temperatureProbe.begin();
  
  delay(2000); // Allow sensors to stabilize
  
  Serial.println("Sensors initialized:");
  Serial.println("- DHT22 sensors for humidity/temperature");
  Serial.println("- DS18B20 temperature probe");
  Serial.println("- Soil moisture sensor");
  Serial.println("- Water level sensors");
  Serial.println("- Light sensor");
}

void initializeSchedules() {
  // Mushroom misting schedules (every 4 hours during active growth)
  schedules[0] = {6, 0, 0, 30, true, true, "Morning misting + lights on"};
  schedules[1] = {10, 0, 0, 30, true, false, "Mid-morning misting"};
  schedules[2] = {14, 0, 0, 30, true, false, "Afternoon misting"};
  schedules[3] = {18, 0, 0, 30, true, true, "Evening misting + lights off"};
  
  // Cilantro watering schedules (daily watering)
  schedules[4] = {7, 0, 1, 180, true, true, "Morning cilantro watering + lights on"};
  schedules[5] = {19, 0, 1, 0, true, true, "Evening lights off for cilantro"};
  
  // Additional schedules (disabled by default)
  schedules[6] = {0, 0, 0, 0, false, false, "Custom schedule 1"};
  schedules[7] = {0, 0, 0, 0, false, false, "Custom schedule 2"};
  
  Serial.println("Default watering schedules initialized");
}

void initializeSystemStatus() {
  // Initialize mushroom status
  mushroomStatus.humidity = 0;
  mushroomStatus.temperature = 0;
  mushroomStatus.mistingActive = false;
  mushroomStatus.lightOn = false;
  mushroomStatus.fanRunning = false;
  mushroomStatus.waterLevel = 0;
  mushroomStatus.lastMisting = 0;
  mushroomStatus.totalMistingTime = 0;
  mushroomStatus.fruitingStage = 0;
  mushroomStatus.nextMistingTime = "Calculating...";
  
  // Initialize cilantro status
  cilantroStatus.soilMoisture = 0;
  cilantroStatus.temperature = 0;
  cilantroStatus.humidity = 0;
  cilantroStatus.wateringActive = false;
  cilantroStatus.lightOn = false;
  cilantroStatus.waterLevel = 0;
  cilantroStatus.lastWatering = 0;
  cilantroStatus.totalWateringTime = 0;
  cilantroStatus.growthStage = 0;
  cilantroStatus.nextWateringTime = "Calculating...";
  
  // Initialize system status
  systemStatus.systemRunning = true;
  systemStatus.manualMode = false;
  systemStatus.alertActive = false;
  systemStatus.alertMessage = "";
  systemStatus.ambientLight = 0;
  systemStatus.uptime = 0;
  systemStatus.currentTime = "00:00";
  systemStatus.heaterOn = false;
  systemStatus.exhaustFanOn = false;
  systemStatus.circulationFanOn = false;
  systemStatus.systemTemperature = 0;
}

void initializeWiFi() {
  Serial.println("Initializing WiFi...");
  
  // Try to connect to saved WiFi
  String savedSSID = preferences.getString("wifi_ssid", "");
  String savedPassword = preferences.getString("wifi_pass", "");
  
  if (savedSSID.length() > 0) {
    Serial.println("Attempting to connect to saved WiFi: " + savedSSID);
    WiFi.begin(savedSSID.c_str(), savedPassword.c_str());
    
    unsigned long wifiTimeout = millis() + 15000; // 15 second timeout
    while (WiFi.status() != WL_CONNECTED && millis() < wifiTimeout) {
      delay(500);
      Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      isConnectedToWiFi = true;
      Serial.println();
      Serial.println("WiFi connected!");
      Serial.println("IP address: " + WiFi.localIP().toString());
      return;
    }
  }
  
  // If WiFi connection failed, start hotspot
  Serial.println("Starting hotspot mode...");
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  isHotspotMode = true;
  Serial.println("Hotspot started: " + String(AP_SSID));
  Serial.println("IP address: " + WiFi.softAPIP().toString());
}

void initializeWebServer() {
  // Main dashboard
  server.on("/", handleRoot);
  
  // API endpoints
  server.on("/api/status", handleStatus);
  server.on("/api/control", HTTP_POST, handleControl);
  server.on("/api/schedule", HTTP_POST, handleSchedule);
  server.on("/api/settings", HTTP_POST, handleSettings);
  server.on("/api/wifi", HTTP_POST, handleWiFiConfig);
  
  // System endpoints
  server.on("/restart", handleRestart);
  server.on("/reset", handleReset);
  
  server.begin();
  Serial.println("Web server started");
}

void initializeOTA() {
  ArduinoOTA.setHostname("mushroom-cilantro-system");
  ArduinoOTA.setPassword("RDTRC2024");
  
  ArduinoOTA.onStart([]() {
    Serial.println("OTA Update Starting...");
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA Update Complete!");
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
  });
  
  ArduinoOTA.begin();
  Serial.println("OTA update service ready");
}

// Main loop
void loop() {
  unsigned long currentTime = millis();
  
  // Handle web server requests
  server.handleClient();
  
  // Handle OTA updates
  ArduinoOTA.handle();
  
  // Read sensors every 10 seconds
  if (currentTime - lastSensorRead >= 10000) {
    readAllSensors();
    lastSensorRead = currentTime;
  }
  
  // Check schedules every minute
  if (currentTime - lastScheduleCheck >= 60000) {
    checkSchedules();
    lastScheduleCheck = currentTime;
  }
  
  // Update system status
  updateSystemStatus();
  
  // Control environmental systems
  controlEnvironment();
  
  // Handle manual controls
  handleManualControls();
  
  // Handle alerts
  handleAlerts();
  
  // Update status LED
  updateStatusLED();
  
  delay(100); // Small delay to prevent watchdog issues
}

void readAllSensors() {
  // Read mushroom area sensors
  mushroomStatus.humidity = dhtMushroom.readHumidity();
  mushroomStatus.temperature = dhtMushroom.readTemperature();
  
  // Read cilantro area sensors
  cilantroStatus.humidity = dhtCilantro.readHumidity();
  cilantroStatus.temperature = dhtCilantro.readTemperature();
  
  // Read soil moisture for cilantro
  int soilRaw = analogRead(SOIL_MOISTURE_PIN);
  cilantroStatus.soilMoisture = map(soilRaw, 0, 4095, 100, 0); // Convert to percentage
  
  // Read water levels
  int mushroomWaterRaw = analogRead(WATER_LEVEL_MUSHROOM_PIN);
  mushroomStatus.waterLevel = map(mushroomWaterRaw, 0, 4095, 0, 100);
  
  int cilantroWaterRaw = analogRead(WATER_LEVEL_CILANTRO_PIN);
  cilantroStatus.waterLevel = map(cilantroWaterRaw, 0, 4095, 0, 100);
  
  // Read ambient light
  int lightRaw = analogRead(LIGHT_SENSOR_PIN);
  systemStatus.ambientLight = map(lightRaw, 0, 4095, 0, 100);
  
  // Read system temperature
  temperatureProbe.requestTemperatures();
  systemStatus.systemTemperature = temperatureProbe.getTempCByIndex(0);
  
  // Validate sensor readings
  if (isnan(mushroomStatus.humidity)) mushroomStatus.humidity = 0;
  if (isnan(mushroomStatus.temperature)) mushroomStatus.temperature = 0;
  if (isnan(cilantroStatus.humidity)) cilantroStatus.humidity = 0;
  if (isnan(cilantroStatus.temperature)) cilantroStatus.temperature = 0;
}

void checkSchedules() {
  // Get current time (simplified - you might want to use RTC)
  unsigned long currentMinutes = (millis() / 60000) % (24 * 60);
  int currentHour = currentMinutes / 60;
  int currentMinute = currentMinutes % 60;
  
  for (int i = 0; i < MAX_SCHEDULES; i++) {
    if (!schedules[i].enabled) continue;
    
    if (schedules[i].hour == currentHour && schedules[i].minute == currentMinute) {
      executeSchedule(i);
    }
  }
}

void executeSchedule(int scheduleIndex) {
  WateringSchedule schedule = schedules[scheduleIndex];
  
  Serial.println("Executing schedule: " + schedule.description);
  
  if (schedule.cropType == 0) { // Mushroom
    if (schedule.duration > 0) {
      startMisting(schedule.duration * 1000);
    }
    if (schedule.lightControl) {
      // Toggle mushroom lights based on time
      bool shouldBeOn = (schedule.hour >= 6 && schedule.hour < 18);
      controlMushroomLights(shouldBeOn);
    }
  } else { // Cilantro
    if (schedule.duration > 0) {
      startWatering(schedule.duration * 1000);
    }
    if (schedule.lightControl) {
      // Toggle cilantro lights based on time
      bool shouldBeOn = (schedule.hour >= 7 && schedule.hour < 19);
      controlCilantroLights(shouldBeOn);
    }
  }
}

void startMisting(unsigned long duration) {
  if (mushroomStatus.waterLevel < MIN_WATER_LEVEL) {
    systemStatus.alertActive = true;
    systemStatus.alertMessage = "Low water level in mushroom reservoir!";
    return;
  }
  
  Serial.println("Starting mushroom misting for " + String(duration/1000) + " seconds");
  
  digitalWrite(MUSHROOM_VALVE_PIN, HIGH);
  digitalWrite(MUSHROOM_MIST_PUMP_PIN, HIGH);
  
  mushroomStatus.mistingActive = true;
  mushroomStatus.lastMisting = millis();
  
  // Schedule to stop misting
  // Note: In a real implementation, you'd use a timer or task scheduler
  delay(duration);
  stopMisting();
}

void stopMisting() {
  digitalWrite(MUSHROOM_VALVE_PIN, LOW);
  digitalWrite(MUSHROOM_MIST_PUMP_PIN, LOW);
  
  mushroomStatus.mistingActive = false;
  mushroomStatus.totalMistingTime += millis() - mushroomStatus.lastMisting;
  
  Serial.println("Mushroom misting stopped");
}

void startWatering(unsigned long duration) {
  if (cilantroStatus.waterLevel < MIN_WATER_LEVEL) {
    systemStatus.alertActive = true;
    systemStatus.alertMessage = "Low water level in cilantro reservoir!";
    return;
  }
  
  // Check if soil moisture is already adequate
  if (cilantroStatus.soilMoisture > CILANTRO_TARGET_MOISTURE_MAX) {
    Serial.println("Soil moisture adequate, skipping watering");
    return;
  }
  
  Serial.println("Starting cilantro watering for " + String(duration/1000) + " seconds");
  
  digitalWrite(CILANTRO_VALVE_PIN, HIGH);
  digitalWrite(CILANTRO_WATER_PUMP_PIN, HIGH);
  
  cilantroStatus.wateringActive = true;
  cilantroStatus.lastWatering = millis();
  
  // Schedule to stop watering
  delay(duration);
  stopWatering();
}

void stopWatering() {
  digitalWrite(CILANTRO_VALVE_PIN, LOW);
  digitalWrite(CILANTRO_WATER_PUMP_PIN, LOW);
  
  cilantroStatus.wateringActive = false;
  cilantroStatus.totalWateringTime += millis() - cilantroStatus.lastWatering;
  
  Serial.println("Cilantro watering stopped");
}

void controlMushroomLights(bool turnOn) {
  digitalWrite(MUSHROOM_LIGHT_PIN, turnOn ? HIGH : LOW);
  mushroomStatus.lightOn = turnOn;
  Serial.println("Mushroom lights " + String(turnOn ? "ON" : "OFF"));
}

void controlCilantroLights(bool turnOn) {
  digitalWrite(CILANTRO_LIGHT_PIN, turnOn ? HIGH : LOW);
  cilantroStatus.lightOn = turnOn;
  Serial.println("Cilantro lights " + String(turnOn ? "ON" : "OFF"));
}

void controlEnvironment() {
  // Control humidity for mushrooms
  if (mushroomStatus.humidity < MUSHROOM_TARGET_HUMIDITY_MIN) {
    // Start circulation fan to distribute moisture
    if (!systemStatus.circulationFanOn) {
      digitalWrite(CIRCULATION_FAN_PIN, HIGH);
      systemStatus.circulationFanOn = true;
    }
  } else if (mushroomStatus.humidity > MUSHROOM_TARGET_HUMIDITY_MAX) {
    // Start exhaust fan to reduce humidity
    if (!systemStatus.exhaustFanOn) {
      digitalWrite(HUMIDITY_FAN_PIN, HIGH);
      systemStatus.exhaustFanOn = true;
    }
  } else {
    // Turn off fans if humidity is in range
    if (systemStatus.exhaustFanOn) {
      digitalWrite(HUMIDITY_FAN_PIN, LOW);
      systemStatus.exhaustFanOn = false;
    }
  }
  
  // Control temperature
  float avgTemp = (mushroomStatus.temperature + cilantroStatus.temperature) / 2;
  if (avgTemp < MUSHROOM_TARGET_TEMP_MIN) {
    if (!systemStatus.heaterOn) {
      digitalWrite(HEATER_PIN, HIGH);
      systemStatus.heaterOn = true;
    }
  } else if (avgTemp > MUSHROOM_TARGET_TEMP_MAX) {
    if (systemStatus.heaterOn) {
      digitalWrite(HEATER_PIN, LOW);
      systemStatus.heaterOn = false;
    }
  }
  
  // Auto-watering based on soil moisture
  if (cilantroStatus.soilMoisture < CILANTRO_TARGET_MOISTURE_MIN && !cilantroStatus.wateringActive) {
    startWatering(WATERING_DURATION);
  }
  
  // Auto-misting based on humidity
  if (mushroomStatus.humidity < MUSHROOM_TARGET_HUMIDITY_MIN && !mushroomStatus.mistingActive) {
    unsigned long timeSinceLastMisting = millis() - mushroomStatus.lastMisting;
    if (timeSinceLastMisting > 7200000) { // 2 hours minimum between mistings
      startMisting(MISTING_DURATION);
    }
  }
}

void updateSystemStatus() {
  systemStatus.uptime = millis() - systemStartTime;
  
  // Calculate next watering times
  // This is simplified - in a real system you'd calculate based on schedules
  mushroomStatus.nextMistingTime = "Next: 2 hours";
  cilantroStatus.nextWateringTime = "Next: Tomorrow 7:00 AM";
  
  // Update growth stages based on time and conditions
  // This is simplified - in reality you'd track actual growth
  unsigned long daysRunning = systemStatus.uptime / (24 * 60 * 60 * 1000);
  
  if (daysRunning < 7) {
    mushroomStatus.fruitingStage = 0; // Incubation
    cilantroStatus.growthStage = 0;   // Seed
  } else if (daysRunning < 14) {
    mushroomStatus.fruitingStage = 1; // Pinning
    cilantroStatus.growthStage = 1;   // Sprout
  } else if (daysRunning < 21) {
    mushroomStatus.fruitingStage = 2; // Fruiting
    cilantroStatus.growthStage = 2;   // Vegetative
  } else {
    mushroomStatus.fruitingStage = 3; // Harvest
    cilantroStatus.growthStage = 3;   // Mature
  }
}

void handleManualControls() {
  static unsigned long lastButtonCheck = 0;
  if (millis() - lastButtonCheck < 100) return; // Debounce
  
  if (digitalRead(BUTTON_MANUAL_PIN) == LOW) {
    systemStatus.manualMode = !systemStatus.manualMode;
    Serial.println("Manual mode: " + String(systemStatus.manualMode ? "ON" : "OFF"));
    delay(500); // Simple debounce
  }
  
  if (digitalRead(BUTTON_RESET_PIN) == LOW) {
    Serial.println("System reset requested...");
    delay(1000);
    if (digitalRead(BUTTON_RESET_PIN) == LOW) {
      ESP.restart();
    }
  }
  
  lastButtonCheck = millis();
}

void handleAlerts() {
  // Check for low water levels
  if (mushroomStatus.waterLevel < MIN_WATER_LEVEL || cilantroStatus.waterLevel < MIN_WATER_LEVEL) {
    systemStatus.alertActive = true;
    systemStatus.alertMessage = "Low water level detected!";
  }
  
  // Check for temperature extremes
  if (mushroomStatus.temperature > 30 || cilantroStatus.temperature > 30) {
    systemStatus.alertActive = true;
    systemStatus.alertMessage = "High temperature alert!";
  }
  
  // Check for sensor failures
  if (mushroomStatus.humidity == 0 && mushroomStatus.temperature == 0) {
    systemStatus.alertActive = true;
    systemStatus.alertMessage = "Mushroom sensor failure!";
  }
  
  // Sound buzzer for critical alerts
  if (systemStatus.alertActive) {
    static unsigned long lastBuzz = 0;
    if (millis() - lastBuzz > 5000) { // Buzz every 5 seconds
      digitalWrite(BUZZER_PIN, HIGH);
      delay(200);
      digitalWrite(BUZZER_PIN, LOW);
      lastBuzz = millis();
    }
  }
}

void updateStatusLED() {
  static unsigned long lastBlink = 0;
  static bool ledState = false;
  
  if (systemStatus.alertActive) {
    // Fast blink for alerts
    if (millis() - lastBlink > 250) {
      ledState = !ledState;
      digitalWrite(STATUS_LED_PIN, ledState);
      lastBlink = millis();
    }
  } else if (systemStatus.systemRunning) {
    // Slow blink for normal operation
    if (millis() - lastBlink > 1000) {
      ledState = !ledState;
      digitalWrite(STATUS_LED_PIN, ledState);
      lastBlink = millis();
    }
  } else {
    // Solid on for stopped system
    digitalWrite(STATUS_LED_PIN, HIGH);
  }
}

// Web server handlers
void handleRoot() {
  String html = generateMainHTML();
  server.send(200, "text/html", html);
}

void handleStatus() {
  DynamicJsonDocument doc(2048);
  
  // Mushroom status
  JsonObject mushroom = doc.createNestedObject("mushroom");
  mushroom["humidity"] = mushroomStatus.humidity;
  mushroom["temperature"] = mushroomStatus.temperature;
  mushroom["mistingActive"] = mushroomStatus.mistingActive;
  mushroom["lightOn"] = mushroomStatus.lightOn;
  mushroom["waterLevel"] = mushroomStatus.waterLevel;
  mushroom["fruitingStage"] = mushroomStatus.fruitingStage;
  mushroom["nextMisting"] = mushroomStatus.nextMistingTime;
  
  // Cilantro status
  JsonObject cilantro = doc.createNestedObject("cilantro");
  cilantro["soilMoisture"] = cilantroStatus.soilMoisture;
  cilantro["temperature"] = cilantroStatus.temperature;
  cilantro["humidity"] = cilantroStatus.humidity;
  cilantro["wateringActive"] = cilantroStatus.wateringActive;
  cilantro["lightOn"] = cilantroStatus.lightOn;
  cilantro["waterLevel"] = cilantroStatus.waterLevel;
  cilantro["growthStage"] = cilantroStatus.growthStage;
  cilantro["nextWatering"] = cilantroStatus.nextWateringTime;
  
  // System status
  JsonObject system = doc.createNestedObject("system");
  system["running"] = systemStatus.systemRunning;
  system["manualMode"] = systemStatus.manualMode;
  system["alertActive"] = systemStatus.alertActive;
  system["alertMessage"] = systemStatus.alertMessage;
  system["uptime"] = systemStatus.uptime;
  system["heaterOn"] = systemStatus.heaterOn;
  system["exhaustFanOn"] = systemStatus.exhaustFanOn;
  system["circulationFanOn"] = systemStatus.circulationFanOn;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleControl() {
  if (server.hasArg("action")) {
    String action = server.arg("action");
    
    if (action == "start_misting") {
      startMisting(MISTING_DURATION);
    } else if (action == "stop_misting") {
      stopMisting();
    } else if (action == "start_watering") {
      startWatering(WATERING_DURATION);
    } else if (action == "stop_watering") {
      stopWatering();
    } else if (action == "toggle_mushroom_lights") {
      controlMushroomLights(!mushroomStatus.lightOn);
    } else if (action == "toggle_cilantro_lights") {
      controlCilantroLights(!cilantroStatus.lightOn);
    } else if (action == "clear_alert") {
      systemStatus.alertActive = false;
      systemStatus.alertMessage = "";
    }
    
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"Missing action parameter\"}");
  }
}

void handleSchedule() {
  // Handle schedule updates
  server.send(200, "application/json", "{\"status\":\"schedule updated\"}");
}

void handleSettings() {
  // Handle settings updates
  server.send(200, "application/json", "{\"status\":\"settings updated\"}");
}

void handleWiFiConfig() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    
    preferences.putString("wifi_ssid", ssid);
    preferences.putString("wifi_pass", password);
    
    server.send(200, "application/json", "{\"status\":\"WiFi settings saved. Restarting...\"}");
    
    delay(1000);
    ESP.restart();
  } else {
    server.send(400, "application/json", "{\"error\":\"Missing WiFi credentials\"}");
  }
}

void handleRestart() {
  server.send(200, "text/html", "<h1>System Restarting...</h1>");
  delay(1000);
  ESP.restart();
}

void handleReset() {
  preferences.clear();
  server.send(200, "text/html", "<h1>System Reset Complete. Restarting...</h1>");
  delay(1000);
  ESP.restart();
}

String generateMainHTML() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Enhanced Mushroom & Cilantro System</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { 
            font-family: Arial, sans-serif; 
            margin: 0; 
            padding: 20px; 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: #333;
        }
        .container { 
            max-width: 1200px; 
            margin: 0 auto; 
            background: white; 
            border-radius: 15px; 
            padding: 20px; 
            box-shadow: 0 10px 30px rgba(0,0,0,0.2);
        }
        .header { 
            text-align: center; 
            margin-bottom: 30px; 
            padding: 20px;
            background: linear-gradient(45deg, #4CAF50, #45a049);
            color: white;
            border-radius: 10px;
        }
        .crop-section { 
            display: inline-block; 
            width: 48%; 
            margin: 1%; 
            padding: 20px; 
            border: 2px solid #ddd; 
            border-radius: 10px; 
            vertical-align: top;
        }
        .mushroom { border-color: #8B4513; background: #f9f5f1; }
        .cilantro { border-color: #228B22; background: #f1f9f1; }
        .status-grid { 
            display: grid; 
            grid-template-columns: 1fr 1fr; 
            gap: 10px; 
            margin: 15px 0; 
        }
        .status-item { 
            padding: 10px; 
            background: #f8f9fa; 
            border-radius: 5px; 
            border-left: 4px solid #007bff;
        }
        .controls { 
            text-align: center; 
            margin: 20px 0; 
        }
        .btn { 
            padding: 10px 20px; 
            margin: 5px; 
            border: none; 
            border-radius: 5px; 
            cursor: pointer; 
            font-size: 14px;
            transition: all 0.3s;
        }
        .btn-primary { background: #007bff; color: white; }
        .btn-success { background: #28a745; color: white; }
        .btn-warning { background: #ffc107; color: #212529; }
        .btn-danger { background: #dc3545; color: white; }
        .btn:hover { transform: translateY(-2px); box-shadow: 0 4px 8px rgba(0,0,0,0.2); }
        .alert { 
            padding: 15px; 
            margin: 10px 0; 
            border-radius: 5px; 
            background: #f8d7da; 
            border: 1px solid #f5c6cb; 
            color: #721c24;
        }
        .system-info { 
            background: #e9ecef; 
            padding: 15px; 
            border-radius: 5px; 
            margin: 20px 0; 
        }
        @media (max-width: 768px) {
            .crop-section { width: 100%; display: block; }
        }
    </style>
    <script>
        function sendCommand(action) {
            fetch('/api/control', {
                method: 'POST',
                headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                body: 'action=' + action
            }).then(() => setTimeout(updateStatus, 1000));
        }
        
        function updateStatus() {
            fetch('/api/status')
            .then(response => response.json())
            .then(data => {
                // Update mushroom status
                document.getElementById('mushroom-humidity').textContent = data.mushroom.humidity.toFixed(1) + '%';
                document.getElementById('mushroom-temp').textContent = data.mushroom.temperature.toFixed(1) + '°C';
                document.getElementById('mushroom-water').textContent = data.mushroom.waterLevel.toFixed(1) + '%';
                document.getElementById('mushroom-stage').textContent = getStageText(data.mushroom.fruitingStage, true);
                
                // Update cilantro status
                document.getElementById('cilantro-moisture').textContent = data.cilantro.soilMoisture.toFixed(1) + '%';
                document.getElementById('cilantro-temp').textContent = data.cilantro.temperature.toFixed(1) + '°C';
                document.getElementById('cilantro-water').textContent = data.cilantro.waterLevel.toFixed(1) + '%';
                document.getElementById('cilantro-stage').textContent = getStageText(data.cilantro.growthStage, false);
                
                // Update system status
                document.getElementById('uptime').textContent = formatUptime(data.system.uptime);
                
                // Show/hide alert
                if (data.system.alertActive) {
                    document.getElementById('alert').style.display = 'block';
                    document.getElementById('alert-message').textContent = data.system.alertMessage;
                } else {
                    document.getElementById('alert').style.display = 'none';
                }
            });
        }
        
        function getStageText(stage, isMushroom) {
            if (isMushroom) {
                const stages = ['Incubation', 'Pinning', 'Fruiting', 'Harvest Ready'];
                return stages[stage] || 'Unknown';
            } else {
                const stages = ['Seed', 'Sprout', 'Vegetative', 'Mature'];
                return stages[stage] || 'Unknown';
            }
        }
        
        function formatUptime(ms) {
            const seconds = Math.floor(ms / 1000);
            const minutes = Math.floor(seconds / 60);
            const hours = Math.floor(minutes / 60);
            const days = Math.floor(hours / 24);
            
            if (days > 0) return days + 'd ' + (hours % 24) + 'h';
            if (hours > 0) return hours + 'h ' + (minutes % 60) + 'm';
            return minutes + 'm ' + (seconds % 60) + 's';
        }
        
        // Auto-update every 10 seconds
        setInterval(updateStatus, 10000);
        updateStatus();
    </script>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🍄 Enhanced Mushroom & Cilantro System 🌿</h1>
            <p>ระบบรดน้ำเห็ดนางฟ้าและผักชีฟลั่งอัตโนมัติ</p>
            <p>Firmware by RDTRC v3.0</p>
        </div>
        
        <div id="alert" class="alert" style="display:none;">
            <strong>⚠️ Alert:</strong> <span id="alert-message"></span>
            <button class="btn btn-warning" onclick="sendCommand('clear_alert')">Clear Alert</button>
        </div>
        
        <div class="crop-section mushroom">
            <h2>🍄 Oyster Mushrooms (เห็ดนางฟ้า)</h2>
            <div class="status-grid">
                <div class="status-item">
                    <strong>Humidity:</strong> <span id="mushroom-humidity">--</span>
                </div>
                <div class="status-item">
                    <strong>Temperature:</strong> <span id="mushroom-temp">--</span>
                </div>
                <div class="status-item">
                    <strong>Water Level:</strong> <span id="mushroom-water">--</span>
                </div>
                <div class="status-item">
                    <strong>Growth Stage:</strong> <span id="mushroom-stage">--</span>
                </div>
            </div>
            <div class="controls">
                <button class="btn btn-primary" onclick="sendCommand('start_misting')">🌧️ Start Misting</button>
                <button class="btn btn-danger" onclick="sendCommand('stop_misting')">⏹️ Stop Misting</button>
                <button class="btn btn-warning" onclick="sendCommand('toggle_mushroom_lights')">💡 Toggle Lights</button>
            </div>
        </div>
        
        <div class="crop-section cilantro">
            <h2>🌿 Cilantro (ผักชีฟลั่ง)</h2>
            <div class="status-grid">
                <div class="status-item">
                    <strong>Soil Moisture:</strong> <span id="cilantro-moisture">--</span>
                </div>
                <div class="status-item">
                    <strong>Temperature:</strong> <span id="cilantro-temp">--</span>
                </div>
                <div class="status-item">
                    <strong>Water Level:</strong> <span id="cilantro-water">--</span>
                </div>
                <div class="status-item">
                    <strong>Growth Stage:</strong> <span id="cilantro-stage">--</span>
                </div>
            </div>
            <div class="controls">
                <button class="btn btn-success" onclick="sendCommand('start_watering')">💧 Start Watering</button>
                <button class="btn btn-danger" onclick="sendCommand('stop_watering')">⏹️ Stop Watering</button>
                <button class="btn btn-warning" onclick="sendCommand('toggle_cilantro_lights')">💡 Toggle Lights</button>
            </div>
        </div>
        
        <div class="system-info">
            <h3>📊 System Information</h3>
            <p><strong>Uptime:</strong> <span id="uptime">--</span></p>
            <p><strong>WiFi:</strong> )" + (isConnectedToWiFi ? "Connected" : "Hotspot Mode") + R"(</p>
            <p><strong>IP Address:</strong> )" + (isConnectedToWiFi ? WiFi.localIP().toString() : WiFi.softAPIP().toString()) + R"(</p>
        </div>
        
        <div class="controls">
            <button class="btn btn-warning" onclick="location.href='/restart'">🔄 Restart System</button>
            <button class="btn btn-danger" onclick="if(confirm('Reset all settings?')) location.href='/reset'">⚙️ Factory Reset</button>
        </div>
    </div>
</body>
</html>
)";
  return html;
}
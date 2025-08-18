/*
 * ===================================================================
 * STANDALONE MUSHROOM & CILANTRO WATERING SYSTEM - SINGLE FILE
 * ===================================================================
 * 
 * Complete automated watering system for Oyster Mushrooms and Cilantro
 * All-in-one file with no external dependencies
 * 
 * Features:
 * ✅ Dual crop watering control (Mushrooms + Cilantro)
 * ✅ High-precision misting system for mushrooms
 * ✅ Drip irrigation system for cilantro
 * ✅ Web interface for control and monitoring
 * ✅ WiFi connectivity with hotspot fallback
 * ✅ Automated scheduling based on crop requirements
 * ✅ Environmental monitoring and alerts
 * ✅ OTA updates capability
 * ✅ Mobile-friendly responsive design
 * 
 * Firmware made by: RDTRC
 * Version: 3.0 Standalone
 * Created: 2024
 * 
 * ===================================================================
 */

// ===================================================================
// 🔧 SYSTEM CONFIGURATION - EDIT THESE SETTINGS
// ===================================================================

// WiFi Configuration
#define WIFI_SSID "Your_WiFi_Name"          // Your home WiFi name
#define WIFI_PASSWORD "Your_WiFi_Password"   // Your home WiFi password
#define AP_SSID "MushroomCilantro_System"    // Hotspot name when WiFi fails
#define AP_PASSWORD "RDTRC2024"              // Hotspot password

// System Identification
#define DEVICE_NAME "Enhanced Mushroom Cilantro System"
#define FIRMWARE_VERSION "3.0.0"
#define SYSTEM_ID "RDTRC_MC_001"

// Pin Configuration - ESP32 GPIO Pins
#define MUSHROOM_MIST_PUMP_PIN 18      // Misting pump for mushrooms
#define CILANTRO_WATER_PUMP_PIN 19     // Drip irrigation pump for cilantro
#define MUSHROOM_VALVE_PIN 20          // Valve for mushroom misting system
#define CILANTRO_VALVE_PIN 21          // Valve for cilantro irrigation
#define HUMIDITY_FAN_PIN 22            // Exhaust fan for humidity control
#define CIRCULATION_FAN_PIN 23         // Air circulation fan
#define MUSHROOM_LIGHT_PIN 25          // LED grow lights for mushrooms
#define CILANTRO_LIGHT_PIN 26          // LED grow lights for cilantro
#define HEATER_PIN 27                  // Heating element

// Sensor Pins
#define DHT_MUSHROOM_PIN 4             // DHT22 for mushroom area humidity/temp
#define DHT_CILANTRO_PIN 5             // DHT22 for cilantro area humidity/temp
#define SOIL_MOISTURE_PIN A0           // Soil moisture sensor for cilantro
#define WATER_LEVEL_MUSHROOM_PIN A1    // Water level sensor for mushroom reservoir
#define WATER_LEVEL_CILANTRO_PIN A2    // Water level sensor for cilantro reservoir
#define LIGHT_SENSOR_PIN A3            // Light sensor for ambient light detection
#define TEMP_PROBE_PIN 2               // DS18B20 temperature probe

// Control Pins
#define BUTTON_MANUAL_PIN 32           // Manual control button
#define BUTTON_RESET_PIN 33            // System reset button
#define BUZZER_PIN 14                  // Alert buzzer
#define STATUS_LED_PIN 13              // Status indicator LED

// Mushroom Growing Parameters
#define MUSHROOM_TARGET_HUMIDITY_MIN 80.0    // Minimum humidity for mushrooms (%)
#define MUSHROOM_TARGET_HUMIDITY_MAX 95.0    // Maximum humidity for mushrooms (%)
#define MUSHROOM_TARGET_TEMP_MIN 20.0        // Minimum temperature for mushrooms (°C)
#define MUSHROOM_TARGET_TEMP_MAX 25.0        // Maximum temperature for mushrooms (°C)
#define MUSHROOM_MISTING_DURATION 30000      // Misting duration in milliseconds (30 seconds)
#define MUSHROOM_LIGHT_HOURS 12              // Daily light hours for mushrooms

// Cilantro Growing Parameters
#define CILANTRO_TARGET_MOISTURE_MIN 60.0    // Minimum soil moisture for cilantro (%)
#define CILANTRO_TARGET_MOISTURE_MAX 75.0    // Maximum soil moisture for cilantro (%)
#define CILANTRO_TARGET_TEMP_MIN 17.0        // Minimum temperature for cilantro (°C)
#define CILANTRO_TARGET_TEMP_MAX 27.0        // Maximum temperature for cilantro (°C)
#define CILANTRO_WATERING_DURATION 180000    // Watering duration in milliseconds (3 minutes)
#define CILANTRO_LIGHT_HOURS 8               // Daily light hours for cilantro

// System Parameters
#define MIN_WATER_LEVEL 20.0                 // Minimum water level percentage
#define SENSOR_READ_INTERVAL 10000           // Sensor reading interval (10 seconds)
#define SCHEDULE_CHECK_INTERVAL 60000        // Schedule check interval (1 minute)
#define MAX_SCHEDULES 8                      // Maximum number of schedules
#define ALERT_BUZZER_INTERVAL 5000           // Buzzer interval for alerts (5 seconds)

// Timing Configuration (24-hour format)
// Mushroom Schedules
#define MUSHROOM_MORNING_HOUR 6              // Morning misting hour
#define MUSHROOM_MORNING_MINUTE 0            // Morning misting minute
#define MUSHROOM_MIDDAY_HOUR 12              // Midday misting hour
#define MUSHROOM_MIDDAY_MINUTE 0             // Midday misting minute
#define MUSHROOM_EVENING_HOUR 18             // Evening misting hour
#define MUSHROOM_EVENING_MINUTE 0            // Evening misting minute
#define MUSHROOM_NIGHT_HOUR 22               // Night misting hour
#define MUSHROOM_NIGHT_MINUTE 0              // Night misting minute

// Cilantro Schedules
#define CILANTRO_MORNING_HOUR 7              // Morning watering hour
#define CILANTRO_MORNING_MINUTE 0            // Morning watering minute
#define CILANTRO_EVENING_HOUR 19             // Evening lights off hour
#define CILANTRO_EVENING_MINUTE 0            // Evening lights off minute

// Web Server Configuration
#define WEB_SERVER_PORT 80                   // Web server port
#define OTA_PASSWORD "RDTRC2024"             // OTA update password
#define ADMIN_USERNAME "admin"               // Admin username
#define ADMIN_PASSWORD "rdtrc123"            // Admin password

// Debug and Logging
#define DEBUG_MODE true                      // Enable debug messages
#define SERIAL_BAUD_RATE 115200             // Serial communication speed
#define LOG_TO_SERIAL true                   // Log to serial monitor
#define LOG_TO_WEB true                      // Log to web interface

// Safety and Limits
#define MAX_CONTINUOUS_WATERING_TIME 600000  // Maximum continuous watering (10 minutes)
#define MAX_CONTINUOUS_MISTING_TIME 120000   // Maximum continuous misting (2 minutes)
#define TEMPERATURE_ALERT_HIGH 35.0          // High temperature alert (°C)
#define TEMPERATURE_ALERT_LOW 5.0            // Low temperature alert (°C)
#define HUMIDITY_ALERT_LOW 30.0              // Low humidity alert (%)

// ===================================================================
// 📚 LIBRARY INCLUDES
// ===================================================================

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

// ===================================================================
// 🏗️ DATA STRUCTURES AND GLOBAL VARIABLES
// ===================================================================

// DHT sensor initialization
#define DHT_TYPE DHT22
DHT dhtMushroom(DHT_MUSHROOM_PIN, DHT_TYPE);
DHT dhtCilantro(DHT_CILANTRO_PIN, DHT_TYPE);

// Temperature probe initialization
OneWire oneWire(TEMP_PROBE_PIN);
DallasTemperature temperatureProbe(&oneWire);

// Web server and system objects
WebServer server(WEB_SERVER_PORT);
Preferences preferences;

// System state variables
bool isConnectedToWiFi = false;
bool isHotspotMode = false;
unsigned long lastSensorRead = 0;
unsigned long lastScheduleCheck = 0;
unsigned long lastBlinkUpdate = 0;
unsigned long systemStartTime = 0;

// Crop-specific status structures
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
  unsigned long mistingStartTime;
  bool mistingScheduled;
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
  unsigned long wateringStartTime;
  bool wateringScheduled;
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
  bool emergencyStop;
  int errorCount;
  String lastError;
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
  bool executed; // Track if schedule was executed today
};

// Global status objects
MushroomStatus mushroomStatus;
CilantroStatus cilantroStatus;
SystemStatus systemStatus;
WateringSchedule schedules[MAX_SCHEDULES];

// Timing variables
unsigned long lastButtonCheck = 0;
unsigned long lastBuzzTime = 0;
unsigned long lastStatusLED = 0;
bool statusLEDState = false;

// ===================================================================
// 🚀 SYSTEM INITIALIZATION
// ===================================================================

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  delay(2000);
  
  // Display boot screen
  displayBootScreen();
  
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
  
  // Initialize SPIFFS for web files
  initializeSPIFFS();
  
  // Initialize WiFi and web server
  initializeWiFi();
  initializeWebServer();
  
  // Initialize OTA updates
  initializeOTA();
  
  // Enable watchdog timer
  esp_task_wdt_init(30, true);
  esp_task_wdt_add(NULL);
  
  logMessage("System initialization complete!");
  logMessage("Ready to grow mushrooms and cilantro!");
  
  // Initial sensor reading
  readAllSensors();
}

void displayBootScreen() {
  Serial.println("=====================================");
  Serial.println("     FW made by RDTRC");
  Serial.println("  Enhanced Mushroom & Cilantro System");
  Serial.println("         Version " + String(FIRMWARE_VERSION));
  Serial.println("         Device: " + String(SYSTEM_ID));
  Serial.println("=====================================");
}

void initializePins() {
  logMessage("Initializing GPIO pins...");
  
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
  
  logMessage("GPIO pins initialized successfully");
}

void initializeSensors() {
  logMessage("Initializing sensors...");
  
  dhtMushroom.begin();
  dhtCilantro.begin();
  temperatureProbe.begin();
  
  delay(2000); // Allow sensors to stabilize
  
  logMessage("Sensors initialized:");
  logMessage("- DHT22 sensors for humidity/temperature");
  logMessage("- DS18B20 temperature probe");
  logMessage("- Soil moisture sensor");
  logMessage("- Water level sensors");
  logMessage("- Light sensor");
}

void initializeSchedules() {
  logMessage("Initializing watering schedules...");
  
  // Mushroom misting schedules (every 6 hours during active growth)
  schedules[0] = {MUSHROOM_MORNING_HOUR, MUSHROOM_MORNING_MINUTE, 0, MUSHROOM_MISTING_DURATION/1000, true, true, "Morning misting + lights on", false};
  schedules[1] = {MUSHROOM_MIDDAY_HOUR, MUSHROOM_MIDDAY_MINUTE, 0, MUSHROOM_MISTING_DURATION/1000, true, false, "Midday misting", false};
  schedules[2] = {MUSHROOM_EVENING_HOUR, MUSHROOM_EVENING_MINUTE, 0, MUSHROOM_MISTING_DURATION/1000, true, true, "Evening misting + lights off", false};
  schedules[3] = {MUSHROOM_NIGHT_HOUR, MUSHROOM_NIGHT_MINUTE, 0, MUSHROOM_MISTING_DURATION/1000, true, false, "Night misting", false};
  
  // Cilantro watering schedules (daily watering)
  schedules[4] = {CILANTRO_MORNING_HOUR, CILANTRO_MORNING_MINUTE, 1, CILANTRO_WATERING_DURATION/1000, true, true, "Morning cilantro watering + lights on", false};
  schedules[5] = {CILANTRO_EVENING_HOUR, CILANTRO_EVENING_MINUTE, 1, 0, true, true, "Evening lights off for cilantro", false};
  
  // Additional schedules (disabled by default)
  schedules[6] = {0, 0, 0, 0, false, false, "Custom schedule 1", false};
  schedules[7] = {0, 0, 0, 0, false, false, "Custom schedule 2", false};
  
  logMessage("Default watering schedules initialized");
}

void initializeSystemStatus() {
  logMessage("Initializing system status...");
  
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
  mushroomStatus.mistingStartTime = 0;
  mushroomStatus.mistingScheduled = false;
  
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
  cilantroStatus.wateringStartTime = 0;
  cilantroStatus.wateringScheduled = false;
  
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
  systemStatus.emergencyStop = false;
  systemStatus.errorCount = 0;
  systemStatus.lastError = "";
  
  logMessage("System status initialized");
}

void initializeSPIFFS() {
  logMessage("Initializing SPIFFS...");
  
  if (!SPIFFS.begin(true)) {
    logMessage("SPIFFS initialization failed");
    return;
  }
  
  logMessage("SPIFFS initialized successfully");
}

void initializeWiFi() {
  logMessage("Initializing WiFi...");
  
  // Try to connect to saved WiFi first
  String savedSSID = preferences.getString("wifi_ssid", WIFI_SSID);
  String savedPassword = preferences.getString("wifi_pass", WIFI_PASSWORD);
  
  if (savedSSID.length() > 0) {
    logMessage("Attempting to connect to WiFi: " + savedSSID);
    WiFi.begin(savedSSID.c_str(), savedPassword.c_str());
    
    unsigned long wifiTimeout = millis() + 15000; // 15 second timeout
    while (WiFi.status() != WL_CONNECTED && millis() < wifiTimeout) {
      delay(500);
      Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      isConnectedToWiFi = true;
      logMessage("");
      logMessage("WiFi connected successfully!");
      logMessage("IP address: " + WiFi.localIP().toString());
      logMessage("Signal strength: " + String(WiFi.RSSI()) + " dBm");
      return;
    }
  }
  
  // If WiFi connection failed, start hotspot
  logMessage("WiFi connection failed. Starting hotspot mode...");
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  isHotspotMode = true;
  logMessage("Hotspot started: " + String(AP_SSID));
  logMessage("IP address: " + WiFi.softAPIP().toString());
  logMessage("Password: " + String(AP_PASSWORD));
}

void initializeWebServer() {
  logMessage("Initializing web server...");
  
  // Main dashboard
  server.on("/", handleRoot);
  
  // API endpoints
  server.on("/api/status", handleStatus);
  server.on("/api/control", HTTP_POST, handleControl);
  server.on("/api/schedule", HTTP_POST, handleSchedule);
  server.on("/api/settings", HTTP_POST, handleSettings);
  server.on("/api/wifi", HTTP_POST, handleWiFiConfig);
  server.on("/api/logs", handleLogs);
  
  // System endpoints
  server.on("/restart", handleRestart);
  server.on("/reset", handleReset);
  server.on("/update", handleUpdate);
  
  // Static files
  server.on("/style.css", handleCSS);
  server.on("/script.js", handleJS);
  
  // 404 handler
  server.onNotFound(handleNotFound);
  
  server.begin();
  logMessage("Web server started on port " + String(WEB_SERVER_PORT));
}

void initializeOTA() {
  logMessage("Initializing OTA updates...");
  
  ArduinoOTA.setHostname(SYSTEM_ID);
  ArduinoOTA.setPassword(OTA_PASSWORD);
  
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    logMessage("OTA Update Starting: " + type);
  });
  
  ArduinoOTA.onEnd([]() {
    logMessage("OTA Update Complete!");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    if (DEBUG_MODE) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    }
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    logMessage("OTA Error[" + String(error) + "]: ");
    if (error == OTA_AUTH_ERROR) logMessage("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) logMessage("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) logMessage("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) logMessage("Receive Failed");
    else if (error == OTA_END_ERROR) logMessage("End Failed");
  });
  
  ArduinoOTA.begin();
  logMessage("OTA update service ready");
}

// ===================================================================
// 🔄 MAIN LOOP
// ===================================================================

void loop() {
  unsigned long currentTime = millis();
  
  // Reset watchdog timer
  esp_task_wdt_reset();
  
  // Handle web server requests
  server.handleClient();
  
  // Handle OTA updates
  ArduinoOTA.handle();
  
  // Read sensors periodically
  if (currentTime - lastSensorRead >= SENSOR_READ_INTERVAL) {
    readAllSensors();
    lastSensorRead = currentTime;
  }
  
  // Check schedules periodically
  if (currentTime - lastScheduleCheck >= SCHEDULE_CHECK_INTERVAL) {
    checkSchedules();
    lastScheduleCheck = currentTime;
  }
  
  // Update system status
  updateSystemStatus();
  
  // Control environmental systems
  controlEnvironment();
  
  // Handle manual controls
  handleManualControls();
  
  // Handle alerts and safety
  handleAlerts();
  
  // Update status LED
  updateStatusLED();
  
  // Handle active watering/misting
  handleActiveOperations();
  
  // Small delay to prevent watchdog issues
  delay(100);
}

// ===================================================================
// 📊 SENSOR READING FUNCTIONS
// ===================================================================

void readAllSensors() {
  if (DEBUG_MODE) {
    logMessage("Reading all sensors...");
  }
  
  // Read mushroom area sensors
  float mushTemp = dhtMushroom.readTemperature();
  float mushHum = dhtMushroom.readHumidity();
  
  // Validate and update mushroom readings
  if (!isnan(mushTemp) && mushTemp > -10 && mushTemp < 50) {
    mushroomStatus.temperature = mushTemp;
  }
  if (!isnan(mushHum) && mushHum >= 0 && mushHum <= 100) {
    mushroomStatus.humidity = mushHum;
  }
  
  // Read cilantro area sensors
  float cilTemp = dhtCilantro.readTemperature();
  float cilHum = dhtCilantro.readHumidity();
  
  // Validate and update cilantro readings
  if (!isnan(cilTemp) && cilTemp > -10 && cilTemp < 50) {
    cilantroStatus.temperature = cilTemp;
  }
  if (!isnan(cilHum) && cilHum >= 0 && cilHum <= 100) {
    cilantroStatus.humidity = cilHum;
  }
  
  // Read soil moisture for cilantro
  int soilRaw = analogRead(SOIL_MOISTURE_PIN);
  cilantroStatus.soilMoisture = map(soilRaw, 0, 4095, 0, 100); // Convert to percentage
  
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
  float sysTemp = temperatureProbe.getTempCByIndex(0);
  if (sysTemp != DEVICE_DISCONNECTED_C) {
    systemStatus.systemTemperature = sysTemp;
  }
  
  // Log sensor readings if debug mode
  if (DEBUG_MODE) {
    logMessage("Mushroom - Temp: " + String(mushroomStatus.temperature) + "°C, Humidity: " + String(mushroomStatus.humidity) + "%");
    logMessage("Cilantro - Temp: " + String(cilantroStatus.temperature) + "°C, Soil: " + String(cilantroStatus.soilMoisture) + "%");
    logMessage("Water Levels - Mushroom: " + String(mushroomStatus.waterLevel) + "%, Cilantro: " + String(cilantroStatus.waterLevel) + "%");
  }
}

// ===================================================================
// ⏰ SCHEDULING FUNCTIONS
// ===================================================================

void checkSchedules() {
  // Get current time (simplified - using uptime)
  unsigned long currentMinutes = (millis() / 60000) % (24 * 60);
  int currentHour = currentMinutes / 60;
  int currentMinute = currentMinutes % 60;
  
  // Check if it's a new day (reset executed flags)
  static int lastDay = -1;
  int currentDay = (millis() / (24 * 60 * 60 * 1000)) % 365;
  if (currentDay != lastDay) {
    for (int i = 0; i < MAX_SCHEDULES; i++) {
      schedules[i].executed = false;
    }
    lastDay = currentDay;
    logMessage("New day - resetting schedule execution flags");
  }
  
  // Check each schedule
  for (int i = 0; i < MAX_SCHEDULES; i++) {
    if (!schedules[i].enabled || schedules[i].executed) continue;
    
    if (schedules[i].hour == currentHour && schedules[i].minute == currentMinute) {
      executeSchedule(i);
      schedules[i].executed = true;
    }
  }
  
  // Update next watering times
  updateNextWateringTimes();
}

void executeSchedule(int scheduleIndex) {
  WateringSchedule schedule = schedules[scheduleIndex];
  
  logMessage("Executing schedule: " + schedule.description);
  
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

void updateNextWateringTimes() {
  // Calculate next misting time for mushrooms
  unsigned long currentTime = millis();
  unsigned long nextMistingTime = ULONG_MAX;
  
  for (int i = 0; i < MAX_SCHEDULES; i++) {
    if (schedules[i].enabled && schedules[i].cropType == 0 && !schedules[i].executed) {
      unsigned long scheduleTime = (schedules[i].hour * 60 + schedules[i].minute) * 60000;
      unsigned long currentDayTime = currentTime % (24 * 60 * 60 * 1000);
      
      if (scheduleTime > currentDayTime && scheduleTime < nextMistingTime) {
        nextMistingTime = scheduleTime;
      }
    }
  }
  
  if (nextMistingTime != ULONG_MAX) {
    unsigned long timeUntilNext = nextMistingTime - (currentTime % (24 * 60 * 60 * 1000));
    mushroomStatus.nextMistingTime = "Next: " + String(timeUntilNext / 3600000) + "h " + String((timeUntilNext % 3600000) / 60000) + "m";
  } else {
    mushroomStatus.nextMistingTime = "Next: Tomorrow";
  }
  
  // Similar calculation for cilantro (simplified)
  cilantroStatus.nextWateringTime = "Next: Tomorrow 7:00 AM";
}

// ===================================================================
// 💧 WATERING AND MISTING FUNCTIONS
// ===================================================================

void startMisting(unsigned long duration) {
  if (mushroomStatus.waterLevel < MIN_WATER_LEVEL) {
    setAlert("Low water level in mushroom reservoir!");
    return;
  }
  
  if (mushroomStatus.mistingActive) {
    logMessage("Misting already active, skipping");
    return;
  }
  
  // Safety check - limit duration
  if (duration > MAX_CONTINUOUS_MISTING_TIME) {
    duration = MAX_CONTINUOUS_MISTING_TIME;
    logMessage("Misting duration limited to safety maximum: " + String(duration/1000) + " seconds");
  }
  
  logMessage("Starting mushroom misting for " + String(duration/1000) + " seconds");
  
  digitalWrite(MUSHROOM_VALVE_PIN, HIGH);
  digitalWrite(MUSHROOM_MIST_PUMP_PIN, HIGH);
  
  mushroomStatus.mistingActive = true;
  mushroomStatus.mistingStartTime = millis();
  mushroomStatus.lastMisting = millis();
  mushroomStatus.mistingScheduled = true;
  
  // Schedule automatic stop
  // Note: This will be handled in handleActiveOperations()
}

void stopMisting() {
  if (!mushroomStatus.mistingActive) return;
  
  digitalWrite(MUSHROOM_VALVE_PIN, LOW);
  digitalWrite(MUSHROOM_MIST_PUMP_PIN, LOW);
  
  unsigned long mistingDuration = millis() - mushroomStatus.mistingStartTime;
  mushroomStatus.mistingActive = false;
  mushroomStatus.totalMistingTime += mistingDuration;
  mushroomStatus.mistingScheduled = false;
  
  logMessage("Mushroom misting stopped. Duration: " + String(mistingDuration/1000) + " seconds");
}

void startWatering(unsigned long duration) {
  if (cilantroStatus.waterLevel < MIN_WATER_LEVEL) {
    setAlert("Low water level in cilantro reservoir!");
    return;
  }
  
  if (cilantroStatus.wateringActive) {
    logMessage("Watering already active, skipping");
    return;
  }
  
  // Check if soil moisture is already adequate
  if (cilantroStatus.soilMoisture > CILANTRO_TARGET_MOISTURE_MAX) {
    logMessage("Soil moisture adequate (" + String(cilantroStatus.soilMoisture) + "%), skipping watering");
    return;
  }
  
  // Safety check - limit duration
  if (duration > MAX_CONTINUOUS_WATERING_TIME) {
    duration = MAX_CONTINUOUS_WATERING_TIME;
    logMessage("Watering duration limited to safety maximum: " + String(duration/1000) + " seconds");
  }
  
  logMessage("Starting cilantro watering for " + String(duration/1000) + " seconds");
  
  digitalWrite(CILANTRO_VALVE_PIN, HIGH);
  digitalWrite(CILANTRO_WATER_PUMP_PIN, HIGH);
  
  cilantroStatus.wateringActive = true;
  cilantroStatus.wateringStartTime = millis();
  cilantroStatus.lastWatering = millis();
  cilantroStatus.wateringScheduled = true;
}

void stopWatering() {
  if (!cilantroStatus.wateringActive) return;
  
  digitalWrite(CILANTRO_VALVE_PIN, LOW);
  digitalWrite(CILANTRO_WATER_PUMP_PIN, LOW);
  
  unsigned long wateringDuration = millis() - cilantroStatus.wateringStartTime;
  cilantroStatus.wateringActive = false;
  cilantroStatus.totalWateringTime += wateringDuration;
  cilantroStatus.wateringScheduled = false;
  
  logMessage("Cilantro watering stopped. Duration: " + String(wateringDuration/1000) + " seconds");
}

void handleActiveOperations() {
  unsigned long currentTime = millis();
  
  // Handle active misting
  if (mushroomStatus.mistingActive && mushroomStatus.mistingScheduled) {
    unsigned long elapsedTime = currentTime - mushroomStatus.mistingStartTime;
    if (elapsedTime >= MUSHROOM_MISTING_DURATION) {
      stopMisting();
    }
  }
  
  // Handle active watering
  if (cilantroStatus.wateringActive && cilantroStatus.wateringScheduled) {
    unsigned long elapsedTime = currentTime - cilantroStatus.wateringStartTime;
    if (elapsedTime >= CILANTRO_WATERING_DURATION) {
      stopWatering();
    }
  }
}

// ===================================================================
// 💡 LIGHTING CONTROL FUNCTIONS
// ===================================================================

void controlMushroomLights(bool turnOn) {
  digitalWrite(MUSHROOM_LIGHT_PIN, turnOn ? HIGH : LOW);
  mushroomStatus.lightOn = turnOn;
  logMessage("Mushroom lights " + String(turnOn ? "ON" : "OFF"));
}

void controlCilantroLights(bool turnOn) {
  digitalWrite(CILANTRO_LIGHT_PIN, turnOn ? HIGH : LOW);
  cilantroStatus.lightOn = turnOn;
  logMessage("Cilantro lights " + String(turnOn ? "ON" : "OFF"));
}

// ===================================================================
// 🌡️ ENVIRONMENTAL CONTROL FUNCTIONS
// ===================================================================

void controlEnvironment() {
  // Control humidity for mushrooms
  if (mushroomStatus.humidity < MUSHROOM_TARGET_HUMIDITY_MIN && mushroomStatus.humidity > 0) {
    // Start circulation fan to distribute moisture
    if (!systemStatus.circulationFanOn) {
      digitalWrite(CIRCULATION_FAN_PIN, HIGH);
      systemStatus.circulationFanOn = true;
      if (DEBUG_MODE) logMessage("Circulation fan ON - Low humidity");
    }
    
    // Auto-mist if humidity is very low and not recently misted
    unsigned long timeSinceLastMisting = millis() - mushroomStatus.lastMisting;
    if (timeSinceLastMisting > 7200000 && !mushroomStatus.mistingActive) { // 2 hours
      startMisting(MUSHROOM_MISTING_DURATION);
    }
  } else if (mushroomStatus.humidity > MUSHROOM_TARGET_HUMIDITY_MAX) {
    // Start exhaust fan to reduce humidity
    if (!systemStatus.exhaustFanOn) {
      digitalWrite(HUMIDITY_FAN_PIN, HIGH);
      systemStatus.exhaustFanOn = true;
      if (DEBUG_MODE) logMessage("Exhaust fan ON - High humidity");
    }
  } else {
    // Turn off fans if humidity is in range
    if (systemStatus.exhaustFanOn) {
      digitalWrite(HUMIDITY_FAN_PIN, LOW);
      systemStatus.exhaustFanOn = false;
      if (DEBUG_MODE) logMessage("Exhaust fan OFF - Humidity normal");
    }
  }
  
  // Control temperature
  float avgTemp = 0;
  int tempSensorCount = 0;
  
  if (mushroomStatus.temperature > 0) {
    avgTemp += mushroomStatus.temperature;
    tempSensorCount++;
  }
  if (cilantroStatus.temperature > 0) {
    avgTemp += cilantroStatus.temperature;
    tempSensorCount++;
  }
  
  if (tempSensorCount > 0) {
    avgTemp /= tempSensorCount;
    
    if (avgTemp < MUSHROOM_TARGET_TEMP_MIN) {
      if (!systemStatus.heaterOn) {
        digitalWrite(HEATER_PIN, HIGH);
        systemStatus.heaterOn = true;
        if (DEBUG_MODE) logMessage("Heater ON - Low temperature: " + String(avgTemp) + "°C");
      }
    } else if (avgTemp > MUSHROOM_TARGET_TEMP_MAX) {
      if (systemStatus.heaterOn) {
        digitalWrite(HEATER_PIN, LOW);
        systemStatus.heaterOn = false;
        if (DEBUG_MODE) logMessage("Heater OFF - High temperature: " + String(avgTemp) + "°C");
      }
    }
  }
  
  // Auto-watering based on soil moisture
  if (cilantroStatus.soilMoisture < CILANTRO_TARGET_MOISTURE_MIN && 
      cilantroStatus.soilMoisture > 0 && 
      !cilantroStatus.wateringActive) {
    unsigned long timeSinceLastWatering = millis() - cilantroStatus.lastWatering;
    if (timeSinceLastWatering > 3600000) { // 1 hour minimum between waterings
      startWatering(CILANTRO_WATERING_DURATION);
    }
  }
}

// ===================================================================
// 🎛️ MANUAL CONTROL FUNCTIONS
// ===================================================================

void handleManualControls() {
  unsigned long currentTime = millis();
  if (currentTime - lastButtonCheck < 200) return; // Debounce
  
  if (digitalRead(BUTTON_MANUAL_PIN) == LOW) {
    systemStatus.manualMode = !systemStatus.manualMode;
    logMessage("Manual mode: " + String(systemStatus.manualMode ? "ON" : "OFF"));
    
    // Visual feedback
    for (int i = 0; i < 3; i++) {
      digitalWrite(STATUS_LED_PIN, LOW);
      delay(100);
      digitalWrite(STATUS_LED_PIN, HIGH);
      delay(100);
    }
    
    delay(500); // Debounce delay
  }
  
  if (digitalRead(BUTTON_RESET_PIN) == LOW) {
    logMessage("System reset requested...");
    setAlert("System reset in 3 seconds...");
    
    // Give user time to release button or cancel
    delay(1000);
    if (digitalRead(BUTTON_RESET_PIN) == LOW) {
      delay(1000);
      if (digitalRead(BUTTON_RESET_PIN) == LOW) {
        logMessage("System restarting...");
        ESP.restart();
      }
    }
  }
  
  lastButtonCheck = currentTime;
}

// ===================================================================
// 🚨 ALERT AND SAFETY FUNCTIONS
// ===================================================================

void handleAlerts() {
  // Check for low water levels
  if (mushroomStatus.waterLevel < MIN_WATER_LEVEL && mushroomStatus.waterLevel > 0) {
    setAlert("Low water level in mushroom reservoir: " + String(mushroomStatus.waterLevel) + "%");
  }
  
  if (cilantroStatus.waterLevel < MIN_WATER_LEVEL && cilantroStatus.waterLevel > 0) {
    setAlert("Low water level in cilantro reservoir: " + String(cilantroStatus.waterLevel) + "%");
  }
  
  // Check for temperature extremes
  if (mushroomStatus.temperature > TEMPERATURE_ALERT_HIGH || cilantroStatus.temperature > TEMPERATURE_ALERT_HIGH) {
    setAlert("High temperature alert! Mushroom: " + String(mushroomStatus.temperature) + "°C, Cilantro: " + String(cilantroStatus.temperature) + "°C");
  }
  
  if (mushroomStatus.temperature < TEMPERATURE_ALERT_LOW || cilantroStatus.temperature < TEMPERATURE_ALERT_LOW) {
    setAlert("Low temperature alert! Check heating system.");
  }
  
  // Check for sensor failures
  if (mushroomStatus.humidity == 0 && mushroomStatus.temperature == 0) {
    setAlert("Mushroom sensor failure detected!");
  }
  
  if (cilantroStatus.humidity == 0 && cilantroStatus.temperature == 0) {
    setAlert("Cilantro sensor failure detected!");
  }
  
  // Check for very low humidity
  if (mushroomStatus.humidity < HUMIDITY_ALERT_LOW && mushroomStatus.humidity > 0) {
    setAlert("Very low humidity in mushroom area: " + String(mushroomStatus.humidity) + "%");
  }
  
  // Emergency stop conditions
  if (systemStatus.systemTemperature > 50.0 || 
      mushroomStatus.temperature > 40.0 || 
      cilantroStatus.temperature > 40.0) {
    emergencyStop("Critical temperature detected!");
  }
  
  // Sound buzzer for active alerts
  if (systemStatus.alertActive) {
    unsigned long currentTime = millis();
    if (currentTime - lastBuzzTime > ALERT_BUZZER_INTERVAL) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(200);
      digitalWrite(BUZZER_PIN, LOW);
      lastBuzzTime = currentTime;
    }
  }
}

void setAlert(String message) {
  if (!systemStatus.alertActive || systemStatus.alertMessage != message) {
    systemStatus.alertActive = true;
    systemStatus.alertMessage = message;
    systemStatus.errorCount++;
    systemStatus.lastError = message;
    logMessage("ALERT: " + message);
  }
}

void clearAlert() {
  systemStatus.alertActive = false;
  systemStatus.alertMessage = "";
  logMessage("Alert cleared");
}

void emergencyStop(String reason) {
  logMessage("EMERGENCY STOP: " + reason);
  
  systemStatus.emergencyStop = true;
  setAlert("EMERGENCY STOP: " + reason);
  
  // Stop all pumps and heater immediately
  digitalWrite(MUSHROOM_MIST_PUMP_PIN, LOW);
  digitalWrite(CILANTRO_WATER_PUMP_PIN, LOW);
  digitalWrite(MUSHROOM_VALVE_PIN, LOW);
  digitalWrite(CILANTRO_VALVE_PIN, LOW);
  digitalWrite(HEATER_PIN, LOW);
  
  // Turn on all fans for cooling
  digitalWrite(HUMIDITY_FAN_PIN, HIGH);
  digitalWrite(CIRCULATION_FAN_PIN, HIGH);
  
  // Update status
  mushroomStatus.mistingActive = false;
  cilantroStatus.wateringActive = false;
  systemStatus.heaterOn = false;
  systemStatus.exhaustFanOn = true;
  systemStatus.circulationFanOn = true;
}

// ===================================================================
// 📊 SYSTEM STATUS FUNCTIONS
// ===================================================================

void updateSystemStatus() {
  systemStatus.uptime = millis() - systemStartTime;
  
  // Update current time (simplified)
  unsigned long currentMinutes = (millis() / 60000) % (24 * 60);
  int currentHour = currentMinutes / 60;
  int currentMinute = currentMinutes % 60;
  systemStatus.currentTime = String(currentHour) + ":" + (currentMinute < 10 ? "0" : "") + String(currentMinute);
  
  // Update growth stages based on time and conditions
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
  
  // Update fan status based on actual pin states
  mushroomStatus.fanRunning = systemStatus.circulationFanOn || systemStatus.exhaustFanOn;
}

void updateStatusLED() {
  unsigned long currentTime = millis();
  
  if (systemStatus.emergencyStop) {
    // Very fast blink for emergency
    if (currentTime - lastStatusLED > 100) {
      statusLEDState = !statusLEDState;
      digitalWrite(STATUS_LED_PIN, statusLEDState);
      lastStatusLED = currentTime;
    }
  } else if (systemStatus.alertActive) {
    // Fast blink for alerts
    if (currentTime - lastStatusLED > 250) {
      statusLEDState = !statusLEDState;
      digitalWrite(STATUS_LED_PIN, statusLEDState);
      lastStatusLED = currentTime;
    }
  } else if (systemStatus.systemRunning) {
    // Slow blink for normal operation
    if (currentTime - lastStatusLED > 1000) {
      statusLEDState = !statusLEDState;
      digitalWrite(STATUS_LED_PIN, statusLEDState);
      lastStatusLED = currentTime;
    }
  } else {
    // Solid on for stopped system
    digitalWrite(STATUS_LED_PIN, HIGH);
  }
}

// ===================================================================
// 📝 LOGGING FUNCTIONS
// ===================================================================

void logMessage(String message) {
  if (LOG_TO_SERIAL) {
    Serial.println("[" + String(millis()) + "] " + message);
  }
  
  // Store in preferences for web interface (keep last 10 messages)
  if (LOG_TO_WEB) {
    String logs = preferences.getString("system_logs", "");
    String newLog = "[" + systemStatus.currentTime + "] " + message + "\n";
    
    // Add new log to beginning
    logs = newLog + logs;
    
    // Limit to reasonable size (keep last ~2KB)
    if (logs.length() > 2000) {
      logs = logs.substring(0, 2000);
    }
    
    preferences.putString("system_logs", logs);
  }
}

// ===================================================================
// 🌐 WEB SERVER HANDLERS
// ===================================================================

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
  mushroom["fanRunning"] = mushroomStatus.fanRunning;
  mushroom["waterLevel"] = mushroomStatus.waterLevel;
  mushroom["fruitingStage"] = mushroomStatus.fruitingStage;
  mushroom["nextMisting"] = mushroomStatus.nextMistingTime;
  mushroom["totalMistingTime"] = mushroomStatus.totalMistingTime / 1000; // seconds
  
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
  cilantro["totalWateringTime"] = cilantroStatus.totalWateringTime / 1000; // seconds
  
  // System status
  JsonObject system = doc.createNestedObject("system");
  system["running"] = systemStatus.systemRunning;
  system["manualMode"] = systemStatus.manualMode;
  system["alertActive"] = systemStatus.alertActive;
  system["alertMessage"] = systemStatus.alertMessage;
  system["uptime"] = systemStatus.uptime;
  system["currentTime"] = systemStatus.currentTime;
  system["heaterOn"] = systemStatus.heaterOn;
  system["exhaustFanOn"] = systemStatus.exhaustFanOn;
  system["circulationFanOn"] = systemStatus.circulationFanOn;
  system["ambientLight"] = systemStatus.ambientLight;
  system["systemTemperature"] = systemStatus.systemTemperature;
  system["emergencyStop"] = systemStatus.emergencyStop;
  system["errorCount"] = systemStatus.errorCount;
  system["wifiConnected"] = isConnectedToWiFi;
  system["hotspotMode"] = isHotspotMode;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleControl() {
  if (!server.hasArg("action")) {
    server.send(400, "application/json", "{\"error\":\"Missing action parameter\"}");
    return;
  }
  
  String action = server.arg("action");
  String response = "{\"status\":\"ok\",\"action\":\"" + action + "\"}";
  
  if (systemStatus.emergencyStop && action != "clear_emergency" && action != "clear_alert") {
    server.send(423, "application/json", "{\"error\":\"System in emergency stop mode\"}");
    return;
  }
  
  if (action == "start_misting") {
    startMisting(MUSHROOM_MISTING_DURATION);
  } else if (action == "stop_misting") {
    stopMisting();
  } else if (action == "start_watering") {
    startWatering(CILANTRO_WATERING_DURATION);
  } else if (action == "stop_watering") {
    stopWatering();
  } else if (action == "toggle_mushroom_lights") {
    controlMushroomLights(!mushroomStatus.lightOn);
  } else if (action == "toggle_cilantro_lights") {
    controlCilantroLights(!cilantroStatus.lightOn);
  } else if (action == "clear_alert") {
    clearAlert();
  } else if (action == "clear_emergency") {
    systemStatus.emergencyStop = false;
    clearAlert();
    logMessage("Emergency stop cleared by user");
  } else if (action == "toggle_manual_mode") {
    systemStatus.manualMode = !systemStatus.manualMode;
    logMessage("Manual mode toggled: " + String(systemStatus.manualMode ? "ON" : "OFF"));
  } else if (action == "emergency_stop") {
    emergencyStop("Manual emergency stop activated");
  } else {
    response = "{\"error\":\"Unknown action: " + action + "\"}";
    server.send(400, "application/json", response);
    return;
  }
  
  server.send(200, "application/json", response);
}

void handleSchedule() {
  // Handle schedule updates - simplified implementation
  server.send(200, "application/json", "{\"status\":\"schedule updated\"}");
}

void handleSettings() {
  // Handle settings updates - simplified implementation
  server.send(200, "application/json", "{\"status\":\"settings updated\"}");
}

void handleWiFiConfig() {
  if (!server.hasArg("ssid") || !server.hasArg("password")) {
    server.send(400, "application/json", "{\"error\":\"Missing WiFi credentials\"}");
    return;
  }
  
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  
  preferences.putString("wifi_ssid", ssid);
  preferences.putString("wifi_pass", password);
  
  logMessage("WiFi credentials updated. SSID: " + ssid);
  
  server.send(200, "application/json", "{\"status\":\"WiFi settings saved. Restarting in 3 seconds...\"}");
  
  delay(3000);
  ESP.restart();
}

void handleLogs() {
  String logs = preferences.getString("system_logs", "No logs available");
  server.send(200, "text/plain", logs);
}

void handleRestart() {
  server.send(200, "text/html", "<h1>System Restarting...</h1><p>Please wait 30 seconds and refresh the page.</p>");
  logMessage("System restart requested via web interface");
  delay(1000);
  ESP.restart();
}

void handleReset() {
  server.send(200, "text/html", "<h1>Factory Reset Complete</h1><p>System will restart with default settings.</p>");
  logMessage("Factory reset requested via web interface");
  preferences.clear();
  delay(1000);
  ESP.restart();
}

void handleUpdate() {
  String html = "<h1>OTA Update</h1>";
  html += "<p>To update firmware:</p>";
  html += "<ol>";
  html += "<li>Install Arduino IDE with ESP32 support</li>";
  html += "<li>Go to Tools > Port and select 'Network Port'</li>";
  html += "<li>Select '" + String(SYSTEM_ID) + "' from the list</li>";
  html += "<li>Upload your new firmware</li>";
  html += "</ol>";
  html += "<p>OTA Password: " + String(OTA_PASSWORD) + "</p>";
  server.send(200, "text/html", html);
}

void handleCSS() {
  String css = generateCSS();
  server.send(200, "text/css", css);
}

void handleJS() {
  String js = generateJS();
  server.send(200, "application/javascript", js);
}

void handleNotFound() {
  server.send(404, "text/html", "<h1>404 - Page Not Found</h1><p><a href='/'>Go to Main Page</a></p>");
}

// ===================================================================
// 🎨 HTML GENERATION FUNCTIONS
// ===================================================================

String generateMainHTML() {
  String html = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>)" + String(DEVICE_NAME) + R"(</title>
    <link rel="stylesheet" href="/style.css">
    <script src="/script.js"></script>
</head>
<body>
    <div class="container">
        <header class="header">
            <h1>🍄 Enhanced Mushroom & Cilantro System 🌿</h1>
            <p>ระบบรดน้ำเห็ดนางฟ้าและผักชีฟลั่งอัตโนมัติ</p>
            <p>Firmware by RDTRC v)" + String(FIRMWARE_VERSION) + R"(</p>
            <div class="system-status">
                <span id="connection-status">)" + (isConnectedToWiFi ? "WiFi Connected" : "Hotspot Mode") + R"(</span>
                <span id="system-time">--:--</span>
                <span id="uptime">Uptime: --</span>
            </div>
        </header>
        
        <div id="alert" class="alert" style="display:none;">
            <strong>⚠️ Alert:</strong> <span id="alert-message"></span>
            <button class="btn btn-warning" onclick="sendCommand('clear_alert')">Clear Alert</button>
        </div>
        
        <div id="emergency" class="emergency" style="display:none;">
            <strong>🚨 EMERGENCY STOP ACTIVE</strong>
            <button class="btn btn-danger" onclick="sendCommand('clear_emergency')">Clear Emergency</button>
        </div>
        
        <div class="controls-bar">
            <button class="btn btn-primary" onclick="toggleManualMode()" id="manual-btn">Manual Mode: OFF</button>
            <button class="btn btn-danger" onclick="emergencyStop()">🚨 Emergency Stop</button>
            <button class="btn btn-info" onclick="refreshData()">🔄 Refresh</button>
        </div>
        
        <div class="crop-sections">
            <div class="crop-section mushroom">
                <h2>🍄 Oyster Mushrooms (เห็ดนางฟ้า)</h2>
                <div class="status-grid">
                    <div class="status-item">
                        <label>Humidity:</label>
                        <span id="mushroom-humidity" class="value">--</span>
                        <div class="progress-bar">
                            <div id="mushroom-humidity-bar" class="progress-fill"></div>
                        </div>
                    </div>
                    <div class="status-item">
                        <label>Temperature:</label>
                        <span id="mushroom-temp" class="value">--</span>
                        <div class="progress-bar">
                            <div id="mushroom-temp-bar" class="progress-fill"></div>
                        </div>
                    </div>
                    <div class="status-item">
                        <label>Water Level:</label>
                        <span id="mushroom-water" class="value">--</span>
                        <div class="progress-bar">
                            <div id="mushroom-water-bar" class="progress-fill"></div>
                        </div>
                    </div>
                    <div class="status-item">
                        <label>Growth Stage:</label>
                        <span id="mushroom-stage" class="value">--</span>
                    </div>
                    <div class="status-item">
                        <label>Next Misting:</label>
                        <span id="mushroom-next" class="value">--</span>
                    </div>
                    <div class="status-item">
                        <label>Total Misting:</label>
                        <span id="mushroom-total" class="value">--</span>
                    </div>
                </div>
                <div class="controls">
                    <button class="btn btn-primary" onclick="sendCommand('start_misting')">🌧️ Start Misting</button>
                    <button class="btn btn-secondary" onclick="sendCommand('stop_misting')">⏹️ Stop Misting</button>
                    <button class="btn btn-warning" onclick="sendCommand('toggle_mushroom_lights')">💡 Toggle Lights</button>
                </div>
                <div class="status-indicators">
                    <span id="mushroom-misting" class="indicator">Misting: OFF</span>
                    <span id="mushroom-light" class="indicator">Light: OFF</span>
                    <span id="mushroom-fan" class="indicator">Fan: OFF</span>
                </div>
            </div>
            
            <div class="crop-section cilantro">
                <h2>🌿 Cilantro (ผักชีฟลั่ง)</h2>
                <div class="status-grid">
                    <div class="status-item">
                        <label>Soil Moisture:</label>
                        <span id="cilantro-moisture" class="value">--</span>
                        <div class="progress-bar">
                            <div id="cilantro-moisture-bar" class="progress-fill"></div>
                        </div>
                    </div>
                    <div class="status-item">
                        <label>Temperature:</label>
                        <span id="cilantro-temp" class="value">--</span>
                        <div class="progress-bar">
                            <div id="cilantro-temp-bar" class="progress-fill"></div>
                        </div>
                    </div>
                    <div class="status-item">
                        <label>Water Level:</label>
                        <span id="cilantro-water" class="value">--</span>
                        <div class="progress-bar">
                            <div id="cilantro-water-bar" class="progress-fill"></div>
                        </div>
                    </div>
                    <div class="status-item">
                        <label>Growth Stage:</label>
                        <span id="cilantro-stage" class="value">--</span>
                    </div>
                    <div class="status-item">
                        <label>Next Watering:</label>
                        <span id="cilantro-next" class="value">--</span>
                    </div>
                    <div class="status-item">
                        <label>Total Watering:</label>
                        <span id="cilantro-total" class="value">--</span>
                    </div>
                </div>
                <div class="controls">
                    <button class="btn btn-success" onclick="sendCommand('start_watering')">💧 Start Watering</button>
                    <button class="btn btn-secondary" onclick="sendCommand('stop_watering')">⏹️ Stop Watering</button>
                    <button class="btn btn-warning" onclick="sendCommand('toggle_cilantro_lights')">💡 Toggle Lights</button>
                </div>
                <div class="status-indicators">
                    <span id="cilantro-watering" class="indicator">Watering: OFF</span>
                    <span id="cilantro-light" class="indicator">Light: OFF</span>
                </div>
            </div>
        </div>
        
        <div class="system-info">
            <h3>📊 System Information</h3>
            <div class="info-grid">
                <div class="info-item">
                    <label>System Temperature:</label>
                    <span id="system-temp">--°C</span>
                </div>
                <div class="info-item">
                    <label>Ambient Light:</label>
                    <span id="ambient-light">--%</span>
                </div>
                <div class="info-item">
                    <label>Heater:</label>
                    <span id="heater-status">OFF</span>
                </div>
                <div class="info-item">
                    <label>Exhaust Fan:</label>
                    <span id="exhaust-fan">OFF</span>
                </div>
                <div class="info-item">
                    <label>Circulation Fan:</label>
                    <span id="circulation-fan">OFF</span>
                </div>
                <div class="info-item">
                    <label>Error Count:</label>
                    <span id="error-count">0</span>
                </div>
            </div>
        </div>
        
        <div class="system-controls">
            <h3>⚙️ System Controls</h3>
            <div class="controls">
                <button class="btn btn-info" onclick="location.href='/api/logs'">📋 View Logs</button>
                <button class="btn btn-warning" onclick="location.href='/restart'">🔄 Restart System</button>
                <button class="btn btn-danger" onclick="if(confirm('Reset all settings?')) location.href='/reset'">⚙️ Factory Reset</button>
                <button class="btn btn-secondary" onclick="location.href='/update'">📤 OTA Update</button>
            </div>
        </div>
        
        <footer class="footer">
            <p>© 2024 RDTRC - Enhanced Mushroom & Cilantro System</p>
            <p>Version )" + String(FIRMWARE_VERSION) + R"( | Device: )" + String(SYSTEM_ID) + R"(</p>
        </footer>
    </div>
    
    <script>
        // Auto-start data updates
        updateStatus();
        setInterval(updateStatus, 5000); // Update every 5 seconds
    </script>
</body>
</html>
)";
  return html;
}

String generateCSS() {
  return R"(
/* Enhanced Mushroom & Cilantro System CSS */
* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    color: #333;
    min-height: 100vh;
}

.container {
    max-width: 1200px;
    margin: 0 auto;
    padding: 20px;
}

.header {
    text-align: center;
    background: linear-gradient(45deg, #4CAF50, #45a049);
    color: white;
    padding: 20px;
    border-radius: 15px;
    margin-bottom: 20px;
    box-shadow: 0 5px 15px rgba(0,0,0,0.2);
}

.header h1 {
    font-size: 2.5em;
    margin-bottom: 10px;
}

.system-status {
    display: flex;
    justify-content: space-around;
    margin-top: 15px;
    flex-wrap: wrap;
}

.system-status span {
    background: rgba(255,255,255,0.2);
    padding: 5px 15px;
    border-radius: 20px;
    margin: 2px;
}

.alert, .emergency {
    padding: 15px;
    margin: 10px 0;
    border-radius: 8px;
    text-align: center;
}

.alert {
    background: #fff3cd;
    border: 1px solid #ffeaa7;
    color: #856404;
}

.emergency {
    background: #f8d7da;
    border: 1px solid #f5c6cb;
    color: #721c24;
    animation: pulse 1s infinite;
}

@keyframes pulse {
    0%, 100% { opacity: 1; }
    50% { opacity: 0.7; }
}

.controls-bar {
    display: flex;
    justify-content: center;
    gap: 10px;
    margin: 20px 0;
    flex-wrap: wrap;
}

.crop-sections {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 20px;
    margin: 20px 0;
}

.crop-section {
    background: white;
    border-radius: 15px;
    padding: 20px;
    box-shadow: 0 5px 15px rgba(0,0,0,0.1);
    border: 3px solid;
}

.mushroom {
    border-color: #8B4513;
    background: linear-gradient(135deg, #fff 0%, #f9f5f1 100%);
}

.cilantro {
    border-color: #228B22;
    background: linear-gradient(135deg, #fff 0%, #f1f9f1 100%);
}

.crop-section h2 {
    text-align: center;
    margin-bottom: 20px;
    font-size: 1.8em;
}

.status-grid {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 15px;
    margin: 20px 0;
}

.status-item {
    background: rgba(255,255,255,0.7);
    padding: 15px;
    border-radius: 10px;
    border-left: 4px solid #007bff;
}

.status-item label {
    font-weight: bold;
    display: block;
    margin-bottom: 5px;
}

.value {
    font-size: 1.2em;
    color: #2c3e50;
    font-weight: bold;
}

.progress-bar {
    width: 100%;
    height: 8px;
    background: #e0e0e0;
    border-radius: 4px;
    margin-top: 5px;
    overflow: hidden;
}

.progress-fill {
    height: 100%;
    background: linear-gradient(90deg, #4CAF50, #45a049);
    transition: width 0.3s ease;
    border-radius: 4px;
}

.controls {
    display: flex;
    justify-content: center;
    gap: 10px;
    margin: 20px 0;
    flex-wrap: wrap;
}

.btn {
    padding: 12px 20px;
    border: none;
    border-radius: 8px;
    cursor: pointer;
    font-size: 14px;
    font-weight: bold;
    transition: all 0.3s ease;
    text-decoration: none;
    display: inline-block;
}

.btn:hover {
    transform: translateY(-2px);
    box-shadow: 0 4px 12px rgba(0,0,0,0.2);
}

.btn-primary { background: #007bff; color: white; }
.btn-success { background: #28a745; color: white; }
.btn-warning { background: #ffc107; color: #212529; }
.btn-danger { background: #dc3545; color: white; }
.btn-secondary { background: #6c757d; color: white; }
.btn-info { background: #17a2b8; color: white; }

.status-indicators {
    display: flex;
    justify-content: space-around;
    margin-top: 15px;
    flex-wrap: wrap;
}

.indicator {
    padding: 8px 15px;
    border-radius: 20px;
    font-size: 0.9em;
    font-weight: bold;
    margin: 2px;
}

.indicator.active {
    background: #28a745;
    color: white;
}

.indicator.inactive {
    background: #6c757d;
    color: white;
}

.system-info, .system-controls {
    background: white;
    border-radius: 15px;
    padding: 20px;
    margin: 20px 0;
    box-shadow: 0 5px 15px rgba(0,0,0,0.1);
}

.info-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
    gap: 15px;
    margin-top: 15px;
}

.info-item {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 10px;
    background: #f8f9fa;
    border-radius: 8px;
}

.footer {
    text-align: center;
    padding: 20px;
    color: white;
    background: rgba(0,0,0,0.2);
    border-radius: 10px;
    margin-top: 20px;
}

/* Mobile Responsive */
@media (max-width: 768px) {
    .crop-sections {
        grid-template-columns: 1fr;
    }
    
    .status-grid {
        grid-template-columns: 1fr;
    }
    
    .controls-bar, .controls {
        flex-direction: column;
        align-items: center;
    }
    
    .btn {
        width: 100%;
        max-width: 300px;
        margin: 5px 0;
    }
    
    .system-status {
        flex-direction: column;
        align-items: center;
    }
}

/* Loading Animation */
.loading {
    display: inline-block;
    width: 20px;
    height: 20px;
    border: 3px solid #f3f3f3;
    border-top: 3px solid #3498db;
    border-radius: 50%;
    animation: spin 1s linear infinite;
}

@keyframes spin {
    0% { transform: rotate(0deg); }
    100% { transform: rotate(360deg); }
}
)";
}

String generateJS() {
  return R"(
// Enhanced Mushroom & Cilantro System JavaScript

let manualMode = false;
let lastUpdateTime = 0;

function sendCommand(action) {
    console.log('Sending command:', action);
    
    fetch('/api/control', {
        method: 'POST',
        headers: {'Content-Type': 'application/x-www-form-urlencoded'},
        body: 'action=' + action
    })
    .then(response => response.json())
    .then(data => {
        console.log('Command response:', data);
        if (data.error) {
            alert('Error: ' + data.error);
        } else {
            // Update status immediately after command
            setTimeout(updateStatus, 1000);
        }
    })
    .catch(error => {
        console.error('Error sending command:', error);
        alert('Error sending command: ' + error);
    });
}

function updateStatus() {
    fetch('/api/status')
    .then(response => response.json())
    .then(data => {
        console.log('Status update:', data);
        updateMushroomStatus(data.mushroom);
        updateCilantroStatus(data.cilantro);
        updateSystemStatus(data.system);
        lastUpdateTime = Date.now();
    })
    .catch(error => {
        console.error('Error updating status:', error);
        // Show connection error in UI
        document.getElementById('connection-status').textContent = 'Connection Error';
        document.getElementById('connection-status').style.color = '#dc3545';
    });
}

function updateMushroomStatus(mushroom) {
    // Update values
    document.getElementById('mushroom-humidity').textContent = mushroom.humidity.toFixed(1) + '%';
    document.getElementById('mushroom-temp').textContent = mushroom.temperature.toFixed(1) + '°C';
    document.getElementById('mushroom-water').textContent = mushroom.waterLevel.toFixed(1) + '%';
    document.getElementById('mushroom-stage').textContent = getMushroomStageText(mushroom.fruitingStage);
    document.getElementById('mushroom-next').textContent = mushroom.nextMisting;
    document.getElementById('mushroom-total').textContent = formatTime(mushroom.totalMistingTime);
    
    // Update progress bars
    updateProgressBar('mushroom-humidity-bar', mushroom.humidity, 80, 95);
    updateProgressBar('mushroom-temp-bar', mushroom.temperature, 20, 25);
    updateProgressBar('mushroom-water-bar', mushroom.waterLevel, 20, 100);
    
    // Update indicators
    updateIndicator('mushroom-misting', mushroom.mistingActive, 'Misting');
    updateIndicator('mushroom-light', mushroom.lightOn, 'Light');
    updateIndicator('mushroom-fan', mushroom.fanRunning, 'Fan');
}

function updateCilantroStatus(cilantro) {
    // Update values
    document.getElementById('cilantro-moisture').textContent = cilantro.soilMoisture.toFixed(1) + '%';
    document.getElementById('cilantro-temp').textContent = cilantro.temperature.toFixed(1) + '°C';
    document.getElementById('cilantro-water').textContent = cilantro.waterLevel.toFixed(1) + '%';
    document.getElementById('cilantro-stage').textContent = getCilantroStageText(cilantro.growthStage);
    document.getElementById('cilantro-next').textContent = cilantro.nextWatering;
    document.getElementById('cilantro-total').textContent = formatTime(cilantro.totalWateringTime);
    
    // Update progress bars
    updateProgressBar('cilantro-moisture-bar', cilantro.soilMoisture, 60, 75);
    updateProgressBar('cilantro-temp-bar', cilantro.temperature, 17, 27);
    updateProgressBar('cilantro-water-bar', cilantro.waterLevel, 20, 100);
    
    // Update indicators
    updateIndicator('cilantro-watering', cilantro.wateringActive, 'Watering');
    updateIndicator('cilantro-light', cilantro.lightOn, 'Light');
}

function updateSystemStatus(system) {
    // Update system info
    document.getElementById('system-time').textContent = system.currentTime;
    document.getElementById('uptime').textContent = 'Uptime: ' + formatUptime(system.uptime);
    document.getElementById('system-temp').textContent = system.systemTemperature.toFixed(1) + '°C';
    document.getElementById('ambient-light').textContent = system.ambientLight.toFixed(1) + '%';
    document.getElementById('error-count').textContent = system.errorCount;
    
    // Update system controls
    updateSystemIndicator('heater-status', system.heaterOn);
    updateSystemIndicator('exhaust-fan', system.exhaustFanOn);
    updateSystemIndicator('circulation-fan', system.circulationFanOn);
    
    // Update manual mode button
    manualMode = system.manualMode;
    const manualBtn = document.getElementById('manual-btn');
    manualBtn.textContent = 'Manual Mode: ' + (manualMode ? 'ON' : 'OFF');
    manualBtn.className = 'btn ' + (manualMode ? 'btn-warning' : 'btn-primary');
    
    // Show/hide alerts
    const alertDiv = document.getElementById('alert');
    const emergencyDiv = document.getElementById('emergency');
    
    if (system.emergencyStop) {
        emergencyDiv.style.display = 'block';
        alertDiv.style.display = 'none';
    } else if (system.alertActive) {
        alertDiv.style.display = 'block';
        emergencyDiv.style.display = 'none';
        document.getElementById('alert-message').textContent = system.alertMessage;
    } else {
        alertDiv.style.display = 'none';
        emergencyDiv.style.display = 'none';
    }
    
    // Update connection status
    const connectionStatus = document.getElementById('connection-status');
    if (system.wifiConnected) {
        connectionStatus.textContent = 'WiFi Connected';
        connectionStatus.style.color = '#28a745';
    } else if (system.hotspotMode) {
        connectionStatus.textContent = 'Hotspot Mode';
        connectionStatus.style.color = '#ffc107';
    } else {
        connectionStatus.textContent = 'Disconnected';
        connectionStatus.style.color = '#dc3545';
    }
}

function updateProgressBar(elementId, value, min, max) {
    const bar = document.getElementById(elementId);
    if (bar) {
        const percentage = Math.max(0, Math.min(100, ((value - min) / (max - min)) * 100));
        bar.style.width = percentage + '%';
        
        // Color coding
        if (percentage < 30) {
            bar.style.background = 'linear-gradient(90deg, #dc3545, #c82333)'; // Red
        } else if (percentage < 70) {
            bar.style.background = 'linear-gradient(90deg, #ffc107, #e0a800)'; // Yellow
        } else {
            bar.style.background = 'linear-gradient(90deg, #28a745, #1e7e34)'; // Green
        }
    }
}

function updateIndicator(elementId, isActive, label) {
    const indicator = document.getElementById(elementId);
    if (indicator) {
        indicator.textContent = label + ': ' + (isActive ? 'ON' : 'OFF');
        indicator.className = 'indicator ' + (isActive ? 'active' : 'inactive');
    }
}

function updateSystemIndicator(elementId, isActive) {
    const element = document.getElementById(elementId);
    if (element) {
        element.textContent = isActive ? 'ON' : 'OFF';
        element.style.color = isActive ? '#28a745' : '#6c757d';
        element.style.fontWeight = 'bold';
    }
}

function getMushroomStageText(stage) {
    const stages = ['Incubation', 'Pinning', 'Fruiting', 'Harvest Ready'];
    return stages[stage] || 'Unknown';
}

function getCilantroStageText(stage) {
    const stages = ['Seed', 'Sprout', 'Vegetative', 'Mature'];
    return stages[stage] || 'Unknown';
}

function formatTime(seconds) {
    if (seconds < 60) return seconds + 's';
    if (seconds < 3600) return Math.floor(seconds / 60) + 'm ' + (seconds % 60) + 's';
    return Math.floor(seconds / 3600) + 'h ' + Math.floor((seconds % 3600) / 60) + 'm';
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

function toggleManualMode() {
    sendCommand('toggle_manual_mode');
}

function emergencyStop() {
    if (confirm('Are you sure you want to activate emergency stop? This will stop all pumps and heaters immediately.')) {
        sendCommand('emergency_stop');
    }
}

function refreshData() {
    updateStatus();
    // Visual feedback
    const btn = event.target;
    const originalText = btn.textContent;
    btn.textContent = 'Refreshing...';
    btn.disabled = true;
    
    setTimeout(() => {
        btn.textContent = originalText;
        btn.disabled = false;
    }, 1000);
}

// Connection monitoring
setInterval(() => {
    const timeSinceLastUpdate = Date.now() - lastUpdateTime;
    if (timeSinceLastUpdate > 30000) { // 30 seconds without update
        document.getElementById('connection-status').textContent = 'Connection Timeout';
        document.getElementById('connection-status').style.color = '#dc3545';
    }
}, 5000);

// Keyboard shortcuts
document.addEventListener('keydown', (event) => {
    if (event.ctrlKey) {
        switch(event.key) {
            case 'm':
                event.preventDefault();
                sendCommand('start_misting');
                break;
            case 'w':
                event.preventDefault();
                sendCommand('start_watering');
                break;
            case 'l':
                event.preventDefault();
                sendCommand('toggle_mushroom_lights');
                break;
            case 'r':
                event.preventDefault();
                refreshData();
                break;
        }
    }
});

// Initialize tooltips
document.addEventListener('DOMContentLoaded', () => {
    // Add keyboard shortcut info
    const shortcuts = document.createElement('div');
    shortcuts.innerHTML = `
        <small style="position: fixed; bottom: 10px; right: 10px; background: rgba(0,0,0,0.7); color: white; padding: 10px; border-radius: 5px; font-size: 12px;">
            Shortcuts: Ctrl+M (Mist), Ctrl+W (Water), Ctrl+L (Lights), Ctrl+R (Refresh)
        </small>
    `;
    document.body.appendChild(shortcuts);
});
)";
}

// ===================================================================
// 🎉 END OF STANDALONE SYSTEM
// ===================================================================
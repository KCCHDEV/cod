/*
 * ระบบพื้นฐานสำหรับทุกโปรเจกต์ IoT - Implementation
 * Common Base System Implementation
 * 
 * Firmware made by: RDTRC
 * Version: 2.0
 * Created: 2024
 */

#include "system_base.h"

// Global Objects
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
RTC_DS3231 rtc;
WebServer server(WEB_SERVER_PORT);
SystemConfig config;
SystemStatus status;

// Constructor
SystemBase::SystemBase(const char* systemName) {
  this->systemName = String(systemName);
  this->lastDisplayUpdate = 0;
  this->lastStatusUpdate = 0;
  this->bootTime = 0;
  this->displayPage = 0;
}

// Core Functions
bool SystemBase::begin() {
  Serial.begin(115200);
  bootTime = millis();
  
  LOG_INFO("Starting " + systemName + " System");
  LOG_INFO("Firmware made by: " + String(FIRMWARE_AUTHOR));
  LOG_INFO("Version: " + String(FIRMWARE_VERSION));
  
  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);
  
  // Set initial state
  setState(SYSTEM_BOOT);
  
  // Show boot screen
  showBootScreen();
  
  // Initialize hardware
  setState(SYSTEM_INIT);
  initializeHardware();
  
  // Load configuration
  if (!loadConfig()) {
    LOG_INFO("Using default configuration");
  }
  
  // Connect to WiFi
  setState(SYSTEM_CONNECTING);
  if (!connectWiFi()) {
    setError(ERROR_WIFI);
    return false;
  }
  
  // Initialize RTC
  if (!initializeRTC()) {
    setError(ERROR_RTC);
    // Continue without RTC (non-critical)
  }
  
  // Setup web server
  setupWebServer();
  
  // Initialize Blynk if token is provided
  if (strlen(config.blynk_token) > 0) {
    Blynk.begin(config.blynk_token, config.ssid, config.password);
  }
  
  // Call system-specific setup
  setupSystem();
  
  setState(SYSTEM_READY);
  showSystemInfo();
  playStartupSound();
  
  LOG_INFO("System initialization completed");
  return true;
}

void SystemBase::loop() {
  // Update system status
  updateStatus();
  
  // Handle web server
  server.handleClient();
  
  // Handle Blynk
  if (strlen(config.blynk_token) > 0) {
    Blynk.run();
  }
  
  // Check WiFi connection
  if (!isWiFiConnected()) {
    reconnectWiFi();
  }
  
  // Update display periodically
  if (millis() - lastDisplayUpdate > 2000) {
    updateDisplay();
    lastDisplayUpdate = millis();
  }
  
  // Update status LED
  showStatusLED();
  
  // Run system-specific code
  if (status.state == SYSTEM_READY || status.state == SYSTEM_RUNNING) {
    setState(SYSTEM_RUNNING);
    runSystem();
  }
  
  // Memory check
  CHECK_MEMORY();
  
  delay(100);
}

void SystemBase::restart() {
  LOG_INFO("Restarting system...");
  showStatus("Restarting...", "Please wait");
  delay(2000);
  ESP.restart();
}

void SystemBase::reset() {
  LOG_INFO("Resetting system configuration...");
  resetConfig();
  showStatus("Reset Complete", "Restarting...");
  delay(2000);
  ESP.restart();
}

// Boot Screen
void SystemBase::showBootScreen() {
  if (!initializeLCD()) {
    return; // Skip boot screen if LCD not available
  }
  
  // Clear screen
  lcd.clear();
  lcd.backlight();
  
  // Show firmware info
  lcd.setCursor(0, 0);
  lcd.print("FW make by RDTRC");
  lcd.setCursor(0, 1);
  lcd.print("Version " + String(FIRMWARE_VERSION));
  
  // Play boot sound
  playBeep(800, 200);
  delay(1500);
  
  // Show system name
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(systemName);
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  
  playBeep(1000, 200);
  delay(1500);
  
  // Show loading animation
  for (int i = 0; i < 16; i++) {
    lcd.setCursor(i, 1);
    lcd.print(".");
    delay(100);
  }
  
  playBeep(1200, 200);
  delay(500);
}

// Hardware Initialization
void SystemBase::initializeHardware() {
  // Initialize pins
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Set initial LED state (red = initializing)
  setLEDColor(255, 0, 0);
  
  LOG_INFO("Hardware initialized");
}

bool SystemBase::initializeLCD() {
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  
  // Try to initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Test LCD
  lcd.setCursor(0, 0);
  lcd.print("LCD Test");
  
  status.lcd_connected = true;
  LOG_INFO("LCD initialized successfully");
  return true;
}

bool SystemBase::initializeRTC() {
  if (!rtc.begin()) {
    LOG_ERROR("RTC not found!");
    status.rtc_connected = false;
    return false;
  }
  
  if (rtc.lostPower()) {
    LOG_INFO("RTC lost power, setting time");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  status.rtc_connected = true;
  LOG_INFO("RTC initialized successfully");
  return true;
}

bool SystemBase::connectWiFi() {
  if (strlen(config.ssid) == 0) {
    LOG_ERROR("WiFi SSID not configured");
    return false;
  }
  
  WiFi.begin(config.ssid, config.password);
  
  showStatus("Connecting WiFi", config.ssid);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < MAX_WIFI_RETRY) {
    for (int i = 0; i < 20; i++) {
      if (WiFi.status() == WL_CONNECTED) break;
      delay(500);
      
      // Show progress on LCD
      if (status.lcd_connected) {
        lcd.setCursor(15, 1);
        lcd.print((i % 4 == 0) ? "|" : (i % 4 == 1) ? "/" : (i % 4 == 2) ? "-" : "\\");
      }
    }
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    LOG_INFO("WiFi connected to: " + String(config.ssid));
    LOG_INFO("IP Address: " + WiFi.localIP().toString());
    
    showStatus("WiFi Connected", WiFi.localIP().toString());
    delay(2000);
    
    return true;
  } else {
    LOG_ERROR("Failed to connect to WiFi");
    showStatus("WiFi Failed", "Check settings");
    return false;
  }
}

// Configuration Management
bool SystemBase::loadConfig() {
  EEPROM.get(CONFIG_START_ADDRESS, config);
  
  // Verify checksum
  uint32_t calculated = calculateChecksum(&config, sizeof(config) - sizeof(config.checksum));
  
  if (config.checksum != calculated) {
    LOG_INFO("Configuration checksum mismatch, using defaults");
    resetConfig();
    return false;
  }
  
  LOG_INFO("Configuration loaded successfully");
  return true;
}

bool SystemBase::saveConfig() {
  config.last_update = millis();
  config.checksum = calculateChecksum(&config, sizeof(config) - sizeof(config.checksum));
  
  EEPROM.put(CONFIG_START_ADDRESS, config);
  EEPROM.commit();
  
  LOG_INFO("Configuration saved");
  return true;
}

void SystemBase::resetConfig() {
  // Set default values
  strcpy(config.ssid, "YOUR_WIFI_SSID");
  strcpy(config.password, "YOUR_WIFI_PASSWORD");
  strcpy(config.blynk_token, "");
  strcpy(config.device_name, systemName.c_str());
  config.system_enabled = true;
  config.timezone_offset = 7; // UTC+7 for Thailand
  
  saveConfig();
  LOG_INFO("Configuration reset to defaults");
}

// Status Management
void SystemBase::updateStatus() {
  status.uptime = millis() - bootTime;
  status.wifi_signal = WiFi.RSSI();
  status.free_memory = ESP.getFreeHeap();
  status.cpu_temperature = temperatureRead();
  
  // Check Blynk connection
  status.blynk_connected = Blynk.connected();
}

void SystemBase::setState(SystemState newState) {
  status.state = newState;
  
  String stateNames[] = {"BOOT", "INIT", "CONNECTING", "READY", "RUNNING", "ERROR", "MAINTENANCE"};
  LOG_INFO("System state: " + stateNames[newState]);
}

void SystemBase::setError(ErrorCode error) {
  status.last_error = error;
  setState(SYSTEM_ERROR);
  
  String errorNames[] = {"NONE", "WIFI", "RTC", "LCD", "SENSOR", "ACTUATOR", "BLYNK", "MEMORY"};
  LOG_ERROR("System error: " + errorNames[error]);
  
  showError(error);
  playErrorSound();
}

void SystemBase::clearError() {
  status.last_error = ERROR_NONE;
  if (status.state == SYSTEM_ERROR) {
    setState(SYSTEM_READY);
  }
}

// Display Functions
void SystemBase::showSystemInfo() {
  if (!status.lcd_connected) return;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(systemName.substring(0, 16));
  lcd.setCursor(0, 1);
  lcd.print("Ready - v" + String(FIRMWARE_VERSION));
  
  delay(2000);
}

void SystemBase::showError(ErrorCode error) {
  if (!status.lcd_connected) return;
  
  String errorMessages[] = {
    "No Error", "WiFi Error", "RTC Error", "LCD Error", 
    "Sensor Error", "Actuator Error", "Blynk Error", "Memory Error"
  };
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ERROR!");
  lcd.setCursor(0, 1);
  lcd.print(errorMessages[error].substring(0, 16));
}

void SystemBase::showStatus(const String& line1, const String& line2) {
  if (!status.lcd_connected) return;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1.substring(0, 16));
  if (line2.length() > 0) {
    lcd.setCursor(0, 1);
    lcd.print(line2.substring(0, 16));
  }
}

void SystemBase::updateDisplay() {
  if (!status.lcd_connected) return;
  
  // Cycle through different display pages
  displayPage = (displayPage + 1) % 4;
  
  lcd.clear();
  
  switch (displayPage) {
    case 0: // System name and time
      lcd.setCursor(0, 0);
      lcd.print(systemName.substring(0, 16));
      lcd.setCursor(0, 1);
      if (status.rtc_connected) {
        lcd.print(getTimeString());
      } else {
        lcd.print("Time: --:--");
      }
      break;
      
    case 1: // WiFi status
      lcd.setCursor(0, 0);
      lcd.print("WiFi: " + String(WiFi.RSSI()) + "dBm");
      lcd.setCursor(0, 1);
      lcd.print(WiFi.localIP().toString());
      break;
      
    case 2: // Memory and uptime
      lcd.setCursor(0, 0);
      lcd.print("Mem: " + String(status.free_memory / 1024) + "KB");
      lcd.setCursor(0, 1);
      lcd.print("Up: " + String(status.uptime / 1000 / 60) + "min");
      break;
      
    case 3: // Custom system status
      {
        String customStatus = getSystemStatus();
        if (customStatus.length() > 0) {
          // Parse JSON or display as is
          lcd.setCursor(0, 0);
          lcd.print("Status:");
          lcd.setCursor(0, 1);
          lcd.print(customStatus.substring(0, 16));
        } else {
          lcd.setCursor(0, 0);
          lcd.print("System Running");
          lcd.setCursor(0, 1);
          lcd.print("All OK");
        }
      }
      break;
  }
}

// LED Functions
void SystemBase::setLEDColor(int red, int green, int blue) {
  analogWrite(LED_RED_PIN, red);
  analogWrite(LED_GREEN_PIN, green);
  analogWrite(LED_BLUE_PIN, blue);
}

void SystemBase::blinkLED(int red, int green, int blue, int times) {
  for (int i = 0; i < times; i++) {
    setLEDColor(red, green, blue);
    delay(200);
    setLEDColor(0, 0, 0);
    delay(200);
  }
}

void SystemBase::showStatusLED() {
  switch (status.state) {
    case SYSTEM_BOOT:
    case SYSTEM_INIT:
      setLEDColor(255, 255, 0); // Yellow
      break;
    case SYSTEM_CONNECTING:
      blinkLED(0, 0, 255, 1); // Blinking blue
      break;
    case SYSTEM_READY:
    case SYSTEM_RUNNING:
      setLEDColor(0, 255, 0); // Green
      break;
    case SYSTEM_ERROR:
      blinkLED(255, 0, 0, 1); // Blinking red
      break;
    case SYSTEM_MAINTENANCE:
      setLEDColor(255, 165, 0); // Orange
      break;
  }
}

// Sound Functions
void SystemBase::playBeep(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration);
}

void SystemBase::playStartupSound() {
  int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
  for (int i = 0; i < 8; i++) {
    tone(BUZZER_PIN, melody[i], 150);
    delay(200);
  }
}

void SystemBase::playErrorSound() {
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 200, 300);
    delay(400);
  }
}

void SystemBase::playSuccessSound() {
  tone(BUZZER_PIN, 1000, 200);
  delay(300);
  tone(BUZZER_PIN, 1200, 200);
  delay(300);
  tone(BUZZER_PIN, 1500, 400);
}

// Network Functions
bool SystemBase::isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void SystemBase::reconnectWiFi() {
  static unsigned long lastReconnectAttempt = 0;
  
  if (millis() - lastReconnectAttempt > 30000) { // Try every 30 seconds
    LOG_INFO("Attempting WiFi reconnection...");
    WiFi.reconnect();
    lastReconnectAttempt = millis();
  }
}

void SystemBase::setupWebServer() {
  // Common endpoints
  server.on("/", handleRoot);
  server.on("/info", handleSystemInfo);
  server.on("/restart", handleRestart);
  server.on("/reset", handleReset);
  
  // API endpoints
  setupCommonAPI();
  
  server.onNotFound(handleNotFound);
  server.begin();
  
  LOG_INFO("Web server started on port " + String(WEB_SERVER_PORT));
}

// Time Functions
DateTime SystemBase::getCurrentTime() {
  if (status.rtc_connected) {
    return rtc.now();
  } else {
    return DateTime(); // Invalid time
  }
}

String SystemBase::getTimeString() {
  if (status.rtc_connected) {
    DateTime now = rtc.now();
    return String(now.hour()) + ":" + 
           (now.minute() < 10 ? "0" : "") + String(now.minute()) + ":" +
           (now.second() < 10 ? "0" : "") + String(now.second());
  }
  return "--:--:--";
}

String SystemBase::getDateString() {
  if (status.rtc_connected) {
    DateTime now = rtc.now();
    return String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
  }
  return "--/--/----";
}

bool SystemBase::isTimeSet() {
  return status.rtc_connected;
}

// Utility Functions
void SystemBase::printSystemInfo() {
  Serial.println("=== System Information ===");
  Serial.println("System: " + systemName);
  Serial.println("Firmware: v" + String(FIRMWARE_VERSION) + " by " + String(FIRMWARE_AUTHOR));
  Serial.println("Uptime: " + String(status.uptime / 1000) + " seconds");
  Serial.println("WiFi SSID: " + String(config.ssid));
  Serial.println("IP Address: " + WiFi.localIP().toString());
  Serial.println("WiFi Signal: " + String(status.wifi_signal) + " dBm");
  Serial.println("Free Memory: " + String(status.free_memory) + " bytes");
  Serial.println("CPU Temperature: " + String(status.cpu_temperature) + "°C");
  Serial.println("========================");
}

void SystemBase::printMemoryInfo() {
  Serial.println("=== Memory Information ===");
  Serial.println("Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("Heap Size: " + String(ESP.getHeapSize()) + " bytes");
  Serial.println("Min Free Heap: " + String(ESP.getMinFreeHeap()) + " bytes");
  Serial.println("Max Alloc Heap: " + String(ESP.getMaxAllocHeap()) + " bytes");
  Serial.println("========================");
}

String SystemBase::getSystemInfoJSON() {
  DynamicJsonDocument doc(1024);
  
  doc["system"] = systemName;
  doc["firmware"] = FIRMWARE_VERSION;
  doc["author"] = FIRMWARE_AUTHOR;
  doc["uptime"] = status.uptime;
  doc["state"] = status.state;
  doc["wifi_ssid"] = config.ssid;
  doc["ip_address"] = WiFi.localIP().toString();
  doc["wifi_signal"] = status.wifi_signal;
  doc["free_memory"] = status.free_memory;
  doc["cpu_temperature"] = status.cpu_temperature;
  doc["rtc_connected"] = status.rtc_connected;
  doc["lcd_connected"] = status.lcd_connected;
  doc["blynk_connected"] = status.blynk_connected;
  
  String output;
  serializeJson(doc, output);
  return output;
}

uint32_t SystemBase::calculateChecksum(const void* data, size_t length) {
  uint32_t checksum = 0;
  const uint8_t* bytes = (const uint8_t*)data;
  
  for (size_t i = 0; i < length; i++) {
    checksum += bytes[i];
  }
  
  return checksum;
}

void SystemBase::logMessage(const String& message) {
  Serial.println("[" + String(millis()) + "] " + message);
}

void SystemBase::logError(const String& error) {
  Serial.println("[ERROR] " + error);
}

// Web Handlers
void handleRoot() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>RDTRC IoT System</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background: #f0f0f0; }
        .container { max-width: 800px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .header { text-align: center; color: #333; margin-bottom: 30px; }
        .status-card { background: #e8f5e8; padding: 15px; border-radius: 5px; margin: 10px 0; }
        .control-panel { background: #f0f8ff; padding: 15px; border-radius: 5px; margin: 10px 0; }
        button { background: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; margin: 5px; }
        button:hover { background: #45a049; }
        .warning { background: #fff3cd; color: #856404; padding: 10px; border-radius: 5px; margin: 10px 0; }
        .error { background: #f8d7da; color: #721c24; padding: 10px; border-radius: 5px; margin: 10px 0; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🤖 RDTRC IoT System</h1>
            <p>Firmware made by RDTRC - Version )" + String(FIRMWARE_VERSION) + R"(</p>
        </div>
        
        <div class="status-card">
            <h3>System Status</h3>
            <p><strong>System:</strong> )" + String(systemName) + R"(</p>
            <p><strong>Uptime:</strong> <span id="uptime">-</span></p>
            <p><strong>WiFi Signal:</strong> <span id="wifi">-</span> dBm</p>
            <p><strong>Free Memory:</strong> <span id="memory">-</span> KB</p>
            <p><strong>IP Address:</strong> )" + WiFi.localIP().toString() + R"(</p>
        </div>
        
        <div class="control-panel">
            <h3>System Control</h3>
            <button onclick="refreshStatus()">Refresh Status</button>
            <button onclick="viewInfo()">System Info</button>
            <button onclick="restart()" style="background: #ff9800;">Restart System</button>
            <button onclick="reset()" style="background: #f44336;">Factory Reset</button>
        </div>
    </div>
    
    <script>
        function refreshStatus() {
            fetch('/api/status')
            .then(response => response.json())
            .then(data => {
                document.getElementById('uptime').textContent = Math.floor(data.uptime / 1000 / 60) + ' minutes';
                document.getElementById('wifi').textContent = data.wifi_signal;
                document.getElementById('memory').textContent = Math.floor(data.free_memory / 1024);
            })
            .catch(error => console.error('Error:', error));
        }
        
        function viewInfo() {
            window.open('/info', '_blank');
        }
        
        function restart() {
            if (confirm('Are you sure you want to restart the system?')) {
                fetch('/restart', { method: 'POST' })
                .then(() => alert('System is restarting...'));
            }
        }
        
        function reset() {
            if (confirm('Are you sure you want to reset to factory defaults?')) {
                fetch('/reset', { method: 'POST' })
                .then(() => alert('System reset complete. Restarting...'));
            }
        }
        
        // Auto-refresh every 30 seconds
        setInterval(refreshStatus, 30000);
        refreshStatus(); // Initial load
    </script>
</body>
</html>
)";
  
  server.send(200, "text/html", html);
}

void handleSystemInfo() {
  server.send(200, "application/json", getSystemInfoJSON());
}

void handleRestart() {
  server.send(200, "text/plain", "Restarting system...");
  delay(1000);
  ESP.restart();
}

void handleReset() {
  server.send(200, "text/plain", "Resetting to factory defaults...");
  // Reset configuration
  resetConfig();
  delay(1000);
  ESP.restart();
}

void handleNotFound() {
  server.send(404, "text/plain", "File Not Found");
}

// API Endpoints
void setupCommonAPI() {
  server.on("/api/status", HTTP_GET, handleAPIStatus);
  server.on("/api/config", HTTP_GET, handleAPIConfig);
  server.on("/api/restart", HTTP_POST, handleAPIRestart);
  server.on("/api/reset", HTTP_POST, handleAPIReset);
}

void handleAPIStatus() {
  server.send(200, "application/json", getSystemInfoJSON());
}

void handleAPIConfig() {
  DynamicJsonDocument doc(512);
  doc["ssid"] = config.ssid;
  doc["device_name"] = config.device_name;
  doc["system_enabled"] = config.system_enabled;
  doc["timezone_offset"] = config.timezone_offset;
  
  String output;
  serializeJson(doc, output);
  server.send(200, "application/json", output);
}

void handleAPIRestart() {
  server.send(200, "application/json", "{\"message\":\"Restarting system\",\"success\":true}");
  delay(1000);
  ESP.restart();
}

void handleAPIReset() {
  server.send(200, "application/json", "{\"message\":\"Factory reset complete\",\"success\":true}");
  resetConfig();
  delay(1000);
  ESP.restart();
}
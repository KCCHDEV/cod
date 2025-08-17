/*
 * Enhanced System Base Implementation
 * ระบบพื้นฐานขั้นสูงสำหรับทุกโปรเจกต์ IoT
 * 
 * Firmware made by: RDTRC
 * Version: 3.0
 * Created: 2024
 */

#include "enhanced_system_base.h"

// Global Objects
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
RTC_DS3231 rtc;
WebServer webServer(WEB_SERVER_PORT);
DNSServer dnsServer;
EnhancedSystemConfig systemConfig;
EnhancedSystemStatus systemStatus;

// Constructor
EnhancedSystemBase::EnhancedSystemBase(const char* systemName, const char* systemType) {
  this->systemName = String(systemName);
  this->systemType = String(systemType);
  
  bootTime = millis();
  lastDisplayUpdate = 0;
  lastStatusUpdate = 0;
  lastNetworkCheck = 0;
  lastBlinkPing = 0;
  lastHealthCheck = 0;
  currentDisplayPage = 0;
  hotspotModeRequested = false;
  configurationMode = false;
  hotspotStartTime = 0;
}

// Core System Functions
bool EnhancedSystemBase::begin() {
  Serial.begin(115200);
  delay(1000);
  
  LOG_INFO("=== RDTRC Enhanced System Starting ===");
  LOG_INFO("System: " + systemName + " (" + systemType + ")");
  LOG_INFO("Firmware Version: " + String(FIRMWARE_VERSION));
  
  // Show boot screen
  showBootScreen();
  
  // Initialize hardware
  if (!initializeHardware()) {
    LOG_ERROR("Hardware initialization failed");
    return false;
  }
  
  // Initialize file system
  if (!initializeFileSystem()) {
    LOG_ERROR("File system initialization failed");
    return false;
  }
  
  // Load configuration
  if (!loadConfiguration()) {
    LOG_ERROR("Configuration loading failed, using defaults");
    resetConfiguration();
  }
  
  // Initialize network
  systemStatus.state = SYSTEM_WIFI_CONNECTING;
  updateDisplay();
  
  // Check for hotspot mode request
  if (digitalRead(BUTTON_HOTSPOT_PIN) == LOW) {
    delay(3000);
    if (digitalRead(BUTTON_HOTSPOT_PIN) == LOW) {
      hotspotModeRequested = true;
      LOG_INFO("Hotspot mode requested by user");
    }
  }
  
  // Setup network based on configuration or request
  bool networkInitialized = false;
  if (hotspotModeRequested || systemConfig.network_mode == MODE_HOTSPOT) {
    networkInitialized = startHotspot();
  } else if (systemConfig.network_mode == MODE_HYBRID) {
    networkInitialized = startHybridMode();
  } else {
    networkInitialized = connectToWiFi();
    if (!networkInitialized && systemConfig.auto_hotspot_fallback) {
      LOG_INFO("WiFi failed, starting hotspot fallback");
      networkInitialized = startHotspot();
    }
  }
  
  if (networkInitialized) {
    // Initialize web interface
    initializeWebInterface();
    
    // Setup OTA if enabled
    if (systemConfig.ota_enabled) {
      setupOTA();
    }
    
    // Connect to Blink if enabled
    if (systemConfig.blink_enabled) {
      systemStatus.state = SYSTEM_BLINK_CONNECTING;
      updateDisplay();
      connectToBlink();
    }
  }
  
  // Initialize system-specific components
  setupSystem();
  
  systemStatus.state = SYSTEM_READY;
  updateDisplay();
  
  LOG_INFO("System initialization completed successfully");
  playSystemSound("startup");
  
  return true;
}

void EnhancedSystemBase::loop() {
  unsigned long currentTime = millis();
  
  // Handle web requests
  webServer.handleClient();
  
  // Handle DNS for captive portal
  if (systemStatus.network.hotspot_active) {
    dnsServer.processNextRequest();
  }
  
  // Handle OTA updates
  if (systemConfig.ota_enabled) {
    ArduinoOTA.handle();
  }
  
  // Check network status periodically
  if (currentTime - lastNetworkCheck >= 30000) { // Every 30 seconds
    checkNetworkStatus();
    lastNetworkCheck = currentTime;
  }
  
  // Handle Blink connection
  if (systemConfig.blink_enabled) {
    handleBlinkConnection();
  }
  
  // Update system metrics
  if (currentTime - lastStatusUpdate >= 5000) { // Every 5 seconds
    updateSystemMetrics();
    lastStatusUpdate = currentTime;
  }
  
  // Update display
  if (currentTime - lastDisplayUpdate >= 2000) { // Every 2 seconds
    updateDisplay();
    lastDisplayUpdate = currentTime;
  }
  
  // Check system health
  if (currentTime - lastHealthCheck >= 60000) { // Every minute
    checkSystemHealth();
    lastHealthCheck = currentTime;
  }
  
  // Update status LED
  updateStatusLED();
  
  // Run system-specific code
  if (systemStatus.state == SYSTEM_RUNNING) {
    runSystem();
  }
  
  // Memory management
  CHECK_MEMORY();
  
  delay(100); // Small delay to prevent watchdog issues
}

void EnhancedSystemBase::showBootScreen() {
  // Initialize LCD if available
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  lcd.init();
  lcd.backlight();
  
  // Show boot screen on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FW make by RDTRC");
  lcd.setCursor(0, 1);
  lcd.print("Ver " + String(FIRMWARE_VERSION) + " - " + String(FIRMWARE_YEAR));
  
  // Boot LED sequence
  setLEDColor(255, 0, 0); // Red
  delay(500);
  setLEDColor(255, 255, 0); // Yellow
  delay(500);
  setLEDColor(0, 255, 0); // Green
  delay(500);
  setLEDColor(0, 0, 255); // Blue
  delay(500);
  setLEDColor(0, 0, 0); // Off
  
  delay(2000); // Show boot screen for 2 seconds
}

bool EnhancedSystemBase::initializeHardware() {
  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);
  
  // Initialize I2C
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  systemStatus.lcd_connected = true;
  
  // Initialize RTC
  if (rtc.begin()) {
    systemStatus.rtc_connected = true;
    LOG_INFO("RTC initialized successfully");
  } else {
    systemStatus.rtc_connected = false;
    LOG_ERROR("RTC initialization failed");
  }
  
  // Initialize LED pins
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize button pins
  pinMode(BUTTON_HOTSPOT_PIN, INPUT_PULLUP);
  
  // Initialize status
  systemStatus.sensors_ok = true;
  systemStatus.free_memory = ESP.getFreeHeap();
  systemStatus.total_memory = ESP.getHeapSize();
  
  return true;
}

bool EnhancedSystemBase::initializeFileSystem() {
  if (!SPIFFS.begin(true)) {
    LOG_ERROR("SPIFFS initialization failed");
    return false;
  }
  
  systemStatus.sd_card_available = true;
  LOG_INFO("File system initialized successfully");
  return true;
}

bool EnhancedSystemBase::connectToWiFi() {
  LOG_INFO("Attempting to connect to WiFi...");
  
  WiFi.mode(WIFI_STA);
  
  // Try primary WiFi first
  if (strlen(systemConfig.primary_ssid) > 0) {
    LOG_INFO("Connecting to primary WiFi: " + String(systemConfig.primary_ssid));
    WiFi.begin(systemConfig.primary_ssid, systemConfig.primary_password);
    
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < WIFI_CONNECT_TIMEOUT) {
      delay(500);
      Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      systemStatus.network.wifi_connected = true;
      systemStatus.network.local_ip = WiFi.localIP().toString();
      systemStatus.network.gateway_ip = WiFi.gatewayIP().toString();
      systemStatus.network.wifi_signal_strength = WiFi.RSSI();
      systemStatus.network.connection_time = millis();
      
      LOG_INFO("WiFi connected successfully");
      LOG_INFO("IP Address: " + systemStatus.network.local_ip);
      LOG_INFO("Signal Strength: " + String(systemStatus.network.wifi_signal_strength) + " dBm");
      
      // Test internet connectivity
      systemStatus.network.internet_available = testInternetConnection();
      
      return true;
    }
  }
  
  // Try secondary WiFi if primary failed
  if (strlen(systemConfig.secondary_ssid) > 0) {
    LOG_INFO("Trying secondary WiFi: " + String(systemConfig.secondary_ssid));
    WiFi.begin(systemConfig.secondary_ssid, systemConfig.secondary_password);
    
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < WIFI_CONNECT_TIMEOUT) {
      delay(500);
      Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      systemStatus.network.wifi_connected = true;
      systemStatus.network.local_ip = WiFi.localIP().toString();
      systemStatus.network.gateway_ip = WiFi.gatewayIP().toString();
      systemStatus.network.wifi_signal_strength = WiFi.RSSI();
      systemStatus.network.connection_time = millis();
      
      LOG_INFO("Secondary WiFi connected successfully");
      LOG_INFO("IP Address: " + systemStatus.network.local_ip);
      
      systemStatus.network.internet_available = testInternetConnection();
      
      return true;
    }
  }
  
  LOG_ERROR("WiFi connection failed");
  systemStatus.network.wifi_connected = false;
  return false;
}

bool EnhancedSystemBase::startHotspot() {
  LOG_INFO("Starting hotspot mode...");
  
  WiFi.mode(WIFI_AP);
  
  String hotspotSSID = String(systemConfig.hotspot_ssid);
  if (hotspotSSID.length() == 0) {
    hotspotSSID = generateUniqueSSID();
    strcpy(systemConfig.hotspot_ssid, hotspotSSID.c_str());
  }
  
  String hotspotPassword = String(systemConfig.hotspot_password);
  if (hotspotPassword.length() < 8) {
    hotspotPassword = HOTSPOT_PASSWORD;
    strcpy(systemConfig.hotspot_password, hotspotPassword.c_str());
  }
  
  bool success = WiFi.softAP(hotspotSSID.c_str(), hotspotPassword.c_str(), 1, 0, MAX_CLIENTS);
  
  if (success) {
    systemStatus.network.hotspot_active = true;
    systemStatus.network.hotspot_ip = WiFi.softAPIP().toString();
    systemStatus.state = SYSTEM_HOTSPOT_MODE;
    hotspotStartTime = millis();
    
    LOG_INFO("Hotspot started successfully");
    LOG_INFO("SSID: " + hotspotSSID);
    LOG_INFO("Password: " + hotspotPassword);
    LOG_INFO("IP Address: " + systemStatus.network.hotspot_ip);
    
    // Setup captive portal
    setupCaptivePortal();
    
    return true;
  } else {
    LOG_ERROR("Failed to start hotspot");
    systemStatus.network.hotspot_active = false;
    return false;
  }
}

bool EnhancedSystemBase::startHybridMode() {
  LOG_INFO("Starting hybrid mode (WiFi + Hotspot)...");
  
  // First try to connect to WiFi
  bool wifiConnected = connectToWiFi();
  
  // Then start hotspot regardless of WiFi status
  WiFi.mode(WIFI_AP_STA);
  
  String hotspotSSID = generateUniqueSSID();
  bool hotspotStarted = WiFi.softAP(hotspotSSID.c_str(), HOTSPOT_PASSWORD, 1, 0, MAX_CLIENTS);
  
  if (hotspotStarted) {
    systemStatus.network.hotspot_active = true;
    systemStatus.network.hotspot_ip = WiFi.softAPIP().toString();
    
    if (!wifiConnected) {
      // If WiFi failed, try again in hybrid mode
      if (strlen(systemConfig.primary_ssid) > 0) {
        WiFi.begin(systemConfig.primary_ssid, systemConfig.primary_password);
      }
    }
    
    setupCaptivePortal();
    
    LOG_INFO("Hybrid mode started");
    LOG_INFO("Hotspot SSID: " + hotspotSSID);
    LOG_INFO("Hotspot IP: " + systemStatus.network.hotspot_ip);
    
    return true;
  }
  
  return wifiConnected; // At least WiFi should work
}

void EnhancedSystemBase::setupCaptivePortal() {
  // Setup DNS server for captive portal
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  
  LOG_INFO("Captive portal DNS server started");
}

bool EnhancedSystemBase::testInternetConnection() {
  HTTPClient http;
  http.begin("http://www.google.com");
  http.setTimeout(5000);
  
  int httpCode = http.GET();
  http.end();
  
  bool connected = (httpCode > 0);
  LOG_INFO("Internet connectivity: " + String(connected ? "Available" : "Not available"));
  
  return connected;
}

bool EnhancedSystemBase::connectToBlink() {
  if (!systemConfig.blink_enabled || strlen(systemConfig.blink_token) == 0) {
    return false;
  }
  
  LOG_BLINK("Connecting to Blink...");
  
  // Initialize Blynk with token
  Blynk.config(systemConfig.blink_token, systemConfig.blink_server, systemConfig.blink_port);
  
  unsigned long startTime = millis();
  while (!Blynk.connected() && (millis() - startTime) < BLINK_CONNECT_TIMEOUT) {
    Blynk.run();
    delay(100);
  }
  
  if (Blynk.connected()) {
    systemStatus.network.blink_connected = true;
    systemStatus.network.last_blink_ping = millis();
    LOG_BLINK("Blink connected successfully");
    return true;
  } else {
    systemStatus.network.blink_connected = false;
    LOG_ERROR("Blink connection failed");
    return false;
  }
}

void EnhancedSystemBase::handleBlinkConnection() {
  if (!systemConfig.blink_enabled) return;
  
  unsigned long currentTime = millis();
  
  // Run Blynk
  if (systemStatus.network.blink_connected) {
    Blynk.run();
    
    // Check if still connected
    if (Blynk.connected()) {
      systemStatus.network.last_blink_ping = currentTime;
    } else {
      systemStatus.network.blink_connected = false;
      LOG_ERROR("Blink connection lost");
    }
  } else {
    // Try to reconnect every 30 seconds
    if (currentTime - systemStatus.network.last_blink_ping > 30000) {
      connectToBlink();
    }
  }
}

void EnhancedSystemBase::initializeWebInterface() {
  setupWebServer();
  webServer.begin();
  
  // Setup mDNS
  String mdnsName = systemName;
  mdnsName.toLowerCase();
  mdnsName.replace(" ", "-");
  
  if (MDNS.begin(mdnsName.c_str())) {
    MDNS.addService("http", "tcp", 80);
    LOG_INFO("mDNS responder started: " + mdnsName + ".local");
  }
  
  LOG_INFO("Web server started on port " + String(WEB_SERVER_PORT));
}

void EnhancedSystemBase::setupWebServer() {
  // Main pages
  webServer.on("/", handleRoot);
  webServer.on("/network", handleNetworkConfig);
  webServer.on("/system", handleSystemConfig);
  webServer.on("/blink", handleBlinkConfig);
  webServer.on("/status", handleSystemInfo);
  webServer.on("/scan", handleNetworkScan);
  webServer.on("/restart", handleRestart);
  webServer.on("/reset", handleFactoryReset);
  webServer.on("/update", handleOTAUpdate);
  
  // API endpoints
  webServer.on("/api/status", handleAPIStatus);
  webServer.on("/api/config", handleAPIConfig);
  webServer.on("/api/networks", handleAPINetworks);
  webServer.on("/api/restart", handleAPIRestart);
  webServer.on("/api/reset", handleAPIReset);
  webServer.on("/api/blink", handleAPIBlink);
  
  // Captive portal
  webServer.onNotFound(handleNotFound);
}

void EnhancedSystemBase::updateDisplay() {
  if (!systemStatus.lcd_connected) return;
  
  lcd.clear();
  
  switch (currentDisplayPage) {
    case 0: // System status
      showSystemStatus();
      break;
    case 1: // Network info
      showNetworkInfo();
      break;
    case 2: // System info
      lcd.setCursor(0, 0);
      lcd.print(systemName.substring(0, 16));
      lcd.setCursor(0, 1);
      lcd.print("Up:" + formatUptime(millis() - bootTime).substring(0, 12));
      break;
    default:
      currentDisplayPage = 0;
      break;
  }
  
  // Auto cycle pages every 10 seconds
  static unsigned long lastPageChange = 0;
  if (millis() - lastPageChange > 10000) {
    currentDisplayPage = (currentDisplayPage + 1) % 3;
    lastPageChange = millis();
  }
}

void EnhancedSystemBase::showSystemStatus() {
  lcd.setCursor(0, 0);
  
  switch (systemStatus.state) {
    case SYSTEM_BOOT:
      lcd.print("Booting...");
      break;
    case SYSTEM_INIT:
      lcd.print("Initializing...");
      break;
    case SYSTEM_WIFI_CONNECTING:
      lcd.print("Connecting WiFi");
      break;
    case SYSTEM_HOTSPOT_MODE:
      lcd.print("Hotspot Mode");
      break;
    case SYSTEM_BLINK_CONNECTING:
      lcd.print("Connecting Blink");
      break;
    case SYSTEM_READY:
      lcd.print("System Ready");
      break;
    case SYSTEM_RUNNING:
      lcd.print("Running");
      break;
    case SYSTEM_ERROR:
      lcd.print("System Error");
      break;
    default:
      lcd.print("Unknown State");
      break;
  }
  
  lcd.setCursor(0, 1);
  if (systemStatus.network.wifi_connected) {
    lcd.print("WiFi:" + systemStatus.network.local_ip.substring(0, 11));
  } else if (systemStatus.network.hotspot_active) {
    lcd.print("AP:" + systemStatus.network.hotspot_ip.substring(0, 13));
  } else {
    lcd.print("No Network");
  }
}

void EnhancedSystemBase::showNetworkInfo() {
  lcd.setCursor(0, 0);
  if (systemStatus.network.wifi_connected) {
    lcd.print("WiFi Connected");
    lcd.setCursor(0, 1);
    lcd.print("RSSI:" + String(systemStatus.network.wifi_signal_strength) + "dBm");
  } else if (systemStatus.network.hotspot_active) {
    lcd.print("Hotspot Active");
    lcd.setCursor(0, 1);
    lcd.print("Clients:" + String(systemStatus.network.connected_clients));
  } else {
    lcd.print("Network Down");
    lcd.setCursor(0, 1);
    lcd.print("Check Config");
  }
}

void EnhancedSystemBase::updateStatusLED() {
  static unsigned long lastLEDUpdate = 0;
  static bool ledState = false;
  
  if (millis() - lastLEDUpdate < 500) return; // Update every 500ms
  lastLEDUpdate = millis();
  ledState = !ledState;
  
  switch (systemStatus.state) {
    case SYSTEM_BOOT:
    case SYSTEM_INIT:
      setLEDColor(255, 255, 0); // Yellow - booting
      break;
      
    case SYSTEM_WIFI_CONNECTING:
    case SYSTEM_BLINK_CONNECTING:
      setLEDColor(ledState ? 0 : 255, ledState ? 0 : 255, 0); // Blinking yellow
      break;
      
    case SYSTEM_HOTSPOT_MODE:
      setLEDColor(ledState ? 0 : 255, 0, ledState ? 0 : 255); // Blinking purple
      break;
      
    case SYSTEM_READY:
    case SYSTEM_RUNNING:
      if (systemStatus.network.blink_connected) {
        setLEDColor(0, 255, 0); // Green - all good
      } else if (systemStatus.network.wifi_connected) {
        setLEDColor(0, 255, 255); // Cyan - WiFi only
      } else if (systemStatus.network.hotspot_active) {
        setLEDColor(0, 0, 255); // Blue - hotspot only
      } else {
        setLEDColor(255, 255, 255); // White - no network
      }
      break;
      
    case SYSTEM_ERROR:
      setLEDColor(ledState ? 0 : 255, 0, 0); // Blinking red
      break;
      
    default:
      setLEDColor(255, 0, 255); // Magenta - unknown state
      break;
  }
}

void EnhancedSystemBase::setLEDColor(int red, int green, int blue) {
  analogWrite(LED_RED_PIN, red);
  analogWrite(LED_GREEN_PIN, green);
  analogWrite(LED_BLUE_PIN, blue);
}

void EnhancedSystemBase::playSystemSound(const String& soundType) {
  if (soundType == "startup") {
    for (int i = 0; i < 3; i++) {
      tone(BUZZER_PIN, 1000 + (i * 200), 200);
      delay(300);
    }
  } else if (soundType == "error") {
    for (int i = 0; i < 5; i++) {
      tone(BUZZER_PIN, 500, 100);
      delay(150);
    }
  } else if (soundType == "success") {
    tone(BUZZER_PIN, 1500, 300);
    delay(400);
    tone(BUZZER_PIN, 2000, 300);
  }
}

void EnhancedSystemBase::updateSystemMetrics() {
  systemStatus.uptime = millis() - bootTime;
  systemStatus.free_memory = ESP.getFreeHeap();
  systemStatus.memory_usage_percent = ((float)(systemStatus.total_memory - systemStatus.free_memory) / systemStatus.total_memory) * 100.0;
  systemStatus.cpu_temperature = temperatureRead();
  
  if (systemStatus.network.hotspot_active) {
    systemStatus.network.connected_clients = WiFi.softAPgetStationNum();
  }
  
  if (systemStatus.network.wifi_connected) {
    systemStatus.network.wifi_signal_strength = WiFi.RSSI();
  }
}

void EnhancedSystemBase::checkSystemHealth() {
  // Check memory
  if (systemStatus.free_memory < 10000) {
    logError("Low memory: " + String(systemStatus.free_memory) + " bytes");
  }
  
  // Check temperature
  if (systemStatus.cpu_temperature > 80.0) {
    logError("High CPU temperature: " + String(systemStatus.cpu_temperature) + "°C");
  }
  
  // Check WiFi connection
  if (systemConfig.network_mode == MODE_WIFI_CLIENT && !systemStatus.network.wifi_connected) {
    LOG_ERROR("WiFi connection lost, attempting reconnection");
    connectToWiFi();
  }
  
  // Check Blink connection
  if (systemConfig.blink_enabled && !systemStatus.network.blink_connected) {
    if (millis() - systemStatus.network.last_blink_ping > 60000) { // 1 minute timeout
      LOG_ERROR("Blink connection timeout, attempting reconnection");
      connectToBlink();
    }
  }
}

// Configuration Management
bool EnhancedSystemBase::loadConfiguration() {
  LOG_INFO("Loading configuration from EEPROM...");
  
  EEPROM.get(CONFIG_START_ADDRESS, systemConfig);
  
  // Validate checksum
  uint32_t calculatedChecksum = calculateChecksum(&systemConfig, sizeof(systemConfig) - sizeof(uint32_t));
  
  if (systemConfig.checksum != calculatedChecksum) {
    LOG_ERROR("Configuration checksum mismatch, using defaults");
    return false;
  }
  
  LOG_INFO("Configuration loaded successfully");
  return true;
}

bool EnhancedSystemBase::saveConfiguration() {
  LOG_INFO("Saving configuration to EEPROM...");
  
  systemConfig.last_update = millis();
  systemConfig.checksum = calculateChecksum(&systemConfig, sizeof(systemConfig) - sizeof(uint32_t));
  
  EEPROM.put(CONFIG_START_ADDRESS, systemConfig);
  EEPROM.commit();
  
  LOG_INFO("Configuration saved successfully");
  return true;
}

void EnhancedSystemBase::resetConfiguration() {
  LOG_INFO("Resetting configuration to defaults...");
  
  // Clear the structure
  memset(&systemConfig, 0, sizeof(systemConfig));
  
  // Set default values
  strcpy(systemConfig.device_name, systemName.c_str());
  strcpy(systemConfig.system_type, systemType.c_str());
  strcpy(systemConfig.hotspot_ssid, generateUniqueSSID().c_str());
  strcpy(systemConfig.hotspot_password, HOTSPOT_PASSWORD);
  strcpy(systemConfig.blink_server, "blynk.cloud");
  
  systemConfig.hotspot_enabled = true;
  systemConfig.auto_hotspot_fallback = true;
  systemConfig.blink_port = 443;
  systemConfig.blink_enabled = false;
  systemConfig.blink_ssl = true;
  systemConfig.system_enabled = true;
  systemConfig.timezone_offset = 7; // GMT+7 for Thailand
  systemConfig.network_mode = MODE_WIFI_CLIENT;
  systemConfig.ota_enabled = true;
  systemConfig.debug_mode = false;
  
  saveConfiguration();
}

uint32_t EnhancedSystemBase::calculateChecksum(const void* data, size_t length) {
  uint32_t checksum = 0;
  const uint8_t* bytes = (const uint8_t*)data;
  
  for (size_t i = 0; i < length; i++) {
    checksum = ((checksum << 1) | (checksum >> 31)) ^ bytes[i];
  }
  
  return checksum;
}

String EnhancedSystemBase::formatUptime(unsigned long uptime) {
  unsigned long seconds = uptime / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;
  
  if (days > 0) {
    return String(days) + "d " + String(hours % 24) + "h";
  } else if (hours > 0) {
    return String(hours) + "h " + String(minutes % 60) + "m";
  } else {
    return String(minutes) + "m " + String(seconds % 60) + "s";
  }
}

String EnhancedSystemBase::formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < 1024 * 1024) {
    return String(bytes / 1024) + "KB";
  } else {
    return String(bytes / (1024 * 1024)) + "MB";
  }
}

String EnhancedSystemBase::generateUniqueSSID() {
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  return String(HOTSPOT_SSID_PREFIX) + systemType + "_" + mac.substring(6);
}

void EnhancedSystemBase::logMessage(const String& message) {
  LOG_INFO(message);
}

void EnhancedSystemBase::logError(const String& error) {
  LOG_ERROR(error);
  systemStatus.last_error = error;
  systemStatus.error_count++;
  systemStatus.last_error_time = millis();
}

// Restart and reset functions
void EnhancedSystemBase::restart() {
  LOG_INFO("System restart requested");
  playSystemSound("success");
  delay(1000);
  ESP.restart();
}

void EnhancedSystemBase::factoryReset() {
  LOG_INFO("Factory reset requested");
  
  // Clear EEPROM
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  
  // Clear SPIFFS
  SPIFFS.format();
  
  playSystemSound("success");
  delay(2000);
  ESP.restart();
}

// Setup OTA
void EnhancedSystemBase::setupOTA() {
  ArduinoOTA.setHostname(systemConfig.device_name);
  ArduinoOTA.setPassword("rdtrc2024");
  
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {
      type = "filesystem";
    }
    LOG_INFO("Start updating " + type);
  });
  
  ArduinoOTA.onEnd([]() {
    LOG_INFO("OTA Update completed");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    LOG_INFO("Progress: " + String(progress / (total / 100)) + "%");
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    LOG_ERROR("OTA Error: " + String(error));
  });
  
  ArduinoOTA.begin();
  LOG_INFO("OTA Ready");
}

// Utility functions
String EnhancedSystemBase::getSystemInfoJSON() {
  DynamicJsonDocument doc(1024);
  
  doc["system"]["name"] = systemName;
  doc["system"]["type"] = systemType;
  doc["system"]["version"] = FIRMWARE_VERSION;
  doc["system"]["author"] = FIRMWARE_AUTHOR;
  doc["system"]["uptime"] = systemStatus.uptime;
  doc["system"]["free_memory"] = systemStatus.free_memory;
  doc["system"]["total_memory"] = systemStatus.total_memory;
  doc["system"]["memory_usage"] = systemStatus.memory_usage_percent;
  doc["system"]["temperature"] = systemStatus.cpu_temperature;
  doc["system"]["state"] = (int)systemStatus.state;
  
  String output;
  serializeJson(doc, output);
  return output;
}

String EnhancedSystemBase::getNetworkInfoJSON() {
  DynamicJsonDocument doc(512);
  
  doc["wifi"]["connected"] = systemStatus.network.wifi_connected;
  doc["wifi"]["ip"] = systemStatus.network.local_ip;
  doc["wifi"]["gateway"] = systemStatus.network.gateway_ip;
  doc["wifi"]["rssi"] = systemStatus.network.wifi_signal_strength;
  
  doc["hotspot"]["active"] = systemStatus.network.hotspot_active;
  doc["hotspot"]["ip"] = systemStatus.network.hotspot_ip;
  doc["hotspot"]["clients"] = systemStatus.network.connected_clients;
  doc["hotspot"]["ssid"] = systemConfig.hotspot_ssid;
  
  doc["blink"]["connected"] = systemStatus.network.blink_connected;
  doc["blink"]["enabled"] = systemConfig.blink_enabled;
  doc["blink"]["server"] = systemConfig.blink_server;
  
  doc["internet"] = systemStatus.network.internet_available;
  
  String output;
  serializeJson(doc, output);
  return output;
}

void EnhancedSystemBase::printSystemInfo() {
  LOG_INFO("=== System Information ===");
  LOG_INFO("Name: " + systemName);
  LOG_INFO("Type: " + systemType);
  LOG_INFO("Version: " + String(FIRMWARE_VERSION));
  LOG_INFO("Uptime: " + formatUptime(systemStatus.uptime));
  LOG_INFO("Free Memory: " + formatBytes(systemStatus.free_memory));
  LOG_INFO("CPU Temperature: " + String(systemStatus.cpu_temperature) + "°C");
  LOG_INFO("WiFi Connected: " + String(systemStatus.network.wifi_connected ? "Yes" : "No"));
  LOG_INFO("Hotspot Active: " + String(systemStatus.network.hotspot_active ? "Yes" : "No"));
  LOG_INFO("Blink Connected: " + String(systemStatus.network.blink_connected ? "Yes" : "No"));
  LOG_INFO("========================");
}
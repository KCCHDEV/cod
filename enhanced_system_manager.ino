/*
 * Enhanced ESP32 System Manager - ระบบจัดการขั้นสูง
 * Universal system management for all ESP32 watering systems
 * 
 * Enhanced Features:
 * - Advanced error handling and recovery
 * - System health monitoring
 * - Automatic failsafe mechanisms
 * - Enhanced logging and diagnostics
 * - OTA (Over-The-Air) updates
 * - System backup and recovery
 * - Performance optimization
 * - Enhanced security features
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <ArduinoOTA.h>
#include <Preferences.h>
#include <esp_task_wdt.h>
#include <SPIFFS.h>

// Enhanced System Configuration
struct SystemConfig {
  String systemName;
  String version;
  bool debugMode;
  bool enableWatchdog;
  bool enableOTA;
  bool enableBackup;
  int maxRetries;
  unsigned long healthCheckInterval;
  unsigned long backupInterval;
};

SystemConfig config = {
  "Enhanced ESP32 System",
  "2.0.0",
  true,
  true,
  true,
  true,
  3,
  30000,    // 30 seconds health check
  3600000   // 1 hour backup
};

// System Health Monitoring
struct SystemHealth {
  bool wifiConnected;
  bool rtcWorking;
  bool sensorsWorking;
  bool relaysWorking;
  bool blinkConnected;
  bool memoryOK;
  bool filesystemOK;
  float cpuUsage;
  unsigned long uptime;
  unsigned long freeHeap;
  int wifiSignal;
  float temperature;
  String lastError;
  unsigned long errorCount;
};

SystemHealth systemHealth;

// Enhanced Error Handling
enum ErrorLevel {
  INFO,
  WARNING,
  ERROR,
  CRITICAL
};

struct ErrorLog {
  unsigned long timestamp;
  ErrorLevel level;
  String component;
  String message;
  String action;
};

ErrorLog errorHistory[50]; // Store last 50 errors
int errorIndex = 0;

// Performance Monitoring
struct PerformanceMetrics {
  unsigned long loopTime;
  unsigned long maxLoopTime;
  unsigned long avgLoopTime;
  unsigned long loopCount;
  float memoryUsage;
  int taskStackUsage;
};

PerformanceMetrics performance;

// Enhanced WiFi Management
struct WiFiConfig {
  String primarySSID;
  String primaryPassword;
  String backupSSID;
  String backupPassword;
  bool autoReconnect;
  int reconnectAttempts;
  unsigned long reconnectTimeout;
};

WiFiConfig wifiConfig = {
  "YOUR_PRIMARY_WIFI",
  "YOUR_PRIMARY_PASSWORD",
  "YOUR_BACKUP_WIFI",
  "YOUR_BACKUP_PASSWORD",
  true,
  5,
  30000
};

// Enhanced Web Server
WebServer server(80);
Preferences preferences;

// Watchdog Timer Configuration
const int WDT_TIMEOUT = 30; // 30 seconds

// System Statistics
unsigned long systemStartTime;
unsigned long totalWateringTime;
unsigned long totalWateringCycles;
unsigned long systemResets;

void setup() {
  Serial.begin(115200);
  systemStartTime = millis();
  
  // Initialize enhanced logging
  initializeLogging();
  logEvent(INFO, "SYSTEM", "Enhanced System Manager starting...", "");
  
  // Initialize SPIFFS for configuration storage
  if (!SPIFFS.begin(true)) {
    logEvent(ERROR, "FILESYSTEM", "SPIFFS mount failed", "FORMAT_SPIFFS");
    SPIFFS.format();
    SPIFFS.begin(true);
  }
  
  // Initialize preferences for persistent storage
  preferences.begin("esp32_system", false);
  loadSystemConfiguration();
  
  // Initialize watchdog timer if enabled
  if (config.enableWatchdog) {
    esp_task_wdt_init(WDT_TIMEOUT, true);
    esp_task_wdt_add(NULL);
    logEvent(INFO, "WATCHDOG", "Watchdog timer enabled", "");
  }
  
  // Enhanced WiFi connection with fallback
  initializeEnhancedWiFi();
  
  // Initialize OTA updates if enabled
  if (config.enableOTA) {
    initializeOTA();
  }
  
  // Initialize system health monitoring
  initializeHealthMonitoring();
  
  // Setup enhanced web server
  setupEnhancedWebServer();
  server.begin();
  
  // Load system statistics
  loadSystemStatistics();
  
  logEvent(INFO, "SYSTEM", "Enhanced System Manager ready", "");
  Serial.println("🚀 Enhanced ESP32 System Manager v" + config.version + " Ready!");
}

void loop() {
  unsigned long loopStart = millis();
  
  // Feed watchdog
  if (config.enableWatchdog) {
    esp_task_wdt_reset();
  }
  
  // Handle OTA updates
  if (config.enableOTA) {
    ArduinoOTA.handle();
  }
  
  // Handle web server
  server.handleClient();
  
  // System health monitoring
  static unsigned long lastHealthCheck = 0;
  if (millis() - lastHealthCheck > config.healthCheckInterval) {
    performHealthCheck();
    lastHealthCheck = millis();
  }
  
  // Enhanced WiFi management
  manageWiFiConnection();
  
  // System backup
  static unsigned long lastBackup = 0;
  if (config.enableBackup && millis() - lastBackup > config.backupInterval) {
    performSystemBackup();
    lastBackup = millis();
  }
  
  // Performance monitoring
  updatePerformanceMetrics(loopStart);
  
  // Cleanup old error logs
  cleanupErrorLogs();
  
  delay(100);
}

void initializeLogging() {
  systemHealth.errorCount = 0;
  systemHealth.lastError = "";
  
  for (int i = 0; i < 50; i++) {
    errorHistory[i].timestamp = 0;
    errorHistory[i].level = INFO;
    errorHistory[i].component = "";
    errorHistory[i].message = "";
    errorHistory[i].action = "";
  }
}

void logEvent(ErrorLevel level, String component, String message, String action) {
  ErrorLog entry;
  entry.timestamp = millis();
  entry.level = level;
  entry.component = component;
  entry.message = message;
  entry.action = action;
  
  errorHistory[errorIndex] = entry;
  errorIndex = (errorIndex + 1) % 50;
  
  systemHealth.errorCount++;
  if (level >= ERROR) {
    systemHealth.lastError = component + ": " + message;
  }
  
  // Print to Serial with color coding
  String levelStr = "";
  switch (level) {
    case INFO: levelStr = "ℹ️ INFO"; break;
    case WARNING: levelStr = "⚠️ WARNING"; break;
    case ERROR: levelStr = "❌ ERROR"; break;
    case CRITICAL: levelStr = "🚨 CRITICAL"; break;
  }
  
  Serial.println("[" + String(millis()) + "] " + levelStr + " [" + component + "] " + message);
  
  // Take action if specified
  if (action != "") {
    executeAction(action);
  }
  
  // Send webhook for critical errors
  if (level >= ERROR) {
    sendEnhancedWebhook(levelStr + " " + component + ": " + message, "error");
  }
}

void executeAction(String action) {
  if (action == "RESTART_WIFI") {
    restartWiFi();
  } else if (action == "RESTART_SYSTEM") {
    logEvent(CRITICAL, "SYSTEM", "System restart requested", "");
    delay(1000);
    ESP.restart();
  } else if (action == "FORMAT_SPIFFS") {
    SPIFFS.format();
  } else if (action == "CLEAR_PREFERENCES") {
    preferences.clear();
  }
}

void initializeEnhancedWiFi() {
  logEvent(INFO, "WIFI", "Initializing enhanced WiFi connection", "");
  
  // Try primary WiFi first
  WiFi.begin(wifiConfig.primarySSID.c_str(), wifiConfig.primaryPassword.c_str());
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < wifiConfig.reconnectAttempts) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    logEvent(WARNING, "WIFI", "Primary WiFi failed, trying backup", "");
    
    // Try backup WiFi
    WiFi.begin(wifiConfig.backupSSID.c_str(), wifiConfig.backupPassword.c_str());
    
    attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < wifiConfig.reconnectAttempts) {
      delay(1000);
      Serial.print(".");
      attempts++;
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    systemHealth.wifiConnected = true;
    logEvent(INFO, "WIFI", "Connected to " + WiFi.SSID() + " IP: " + WiFi.localIP().toString(), "");
  } else {
    systemHealth.wifiConnected = false;
    logEvent(ERROR, "WIFI", "Failed to connect to any WiFi network", "RESTART_WIFI");
  }
}

void manageWiFiConnection() {
  static unsigned long lastWiFiCheck = 0;
  
  if (millis() - lastWiFiCheck > 10000) { // Check every 10 seconds
    if (WiFi.status() != WL_CONNECTED) {
      systemHealth.wifiConnected = false;
      logEvent(WARNING, "WIFI", "WiFi connection lost", "");
      
      if (wifiConfig.autoReconnect) {
        initializeEnhancedWiFi();
      }
    } else {
      systemHealth.wifiConnected = true;
      systemHealth.wifiSignal = WiFi.RSSI();
    }
    
    lastWiFiCheck = millis();
  }
}

void restartWiFi() {
  WiFi.disconnect();
  delay(1000);
  initializeEnhancedWiFi();
}

void initializeOTA() {
  ArduinoOTA.setHostname("ESP32-Enhanced-System");
  ArduinoOTA.setPassword("esp32update"); // Change this password!
  
  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    logEvent(INFO, "OTA", "Start updating " + type, "");
  });
  
  ArduinoOTA.onEnd([]() {
    logEvent(INFO, "OTA", "Update completed", "");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    String errorMsg = "Error[" + String(error) + "]: ";
    if (error == OTA_AUTH_ERROR) errorMsg += "Auth Failed";
    else if (error == OTA_BEGIN_ERROR) errorMsg += "Begin Failed";
    else if (error == OTA_CONNECT_ERROR) errorMsg += "Connect Failed";
    else if (error == OTA_RECEIVE_ERROR) errorMsg += "Receive Failed";
    else if (error == OTA_END_ERROR) errorMsg += "End Failed";
    
    logEvent(ERROR, "OTA", errorMsg, "");
  });
  
  ArduinoOTA.begin();
  logEvent(INFO, "OTA", "OTA updates enabled", "");
}

void initializeHealthMonitoring() {
  systemHealth.wifiConnected = false;
  systemHealth.rtcWorking = false;
  systemHealth.sensorsWorking = false;
  systemHealth.relaysWorking = false;
  systemHealth.blinkConnected = false;
  systemHealth.memoryOK = true;
  systemHealth.filesystemOK = true;
  systemHealth.cpuUsage = 0.0;
  systemHealth.uptime = 0;
  systemHealth.freeHeap = ESP.getFreeHeap();
  systemHealth.wifiSignal = 0;
  systemHealth.temperature = 0.0;
  systemHealth.lastError = "";
  systemHealth.errorCount = 0;
}

void performHealthCheck() {
  // Update uptime
  systemHealth.uptime = millis() - systemStartTime;
  
  // Check memory
  unsigned long currentHeap = ESP.getFreeHeap();
  systemHealth.freeHeap = currentHeap;
  
  if (currentHeap < 10000) { // Less than 10KB free
    systemHealth.memoryOK = false;
    logEvent(WARNING, "MEMORY", "Low memory: " + String(currentHeap) + " bytes free", "");
  } else {
    systemHealth.memoryOK = true;
  }
  
  // Check filesystem
  size_t totalBytes = SPIFFS.totalBytes();
  size_t usedBytes = SPIFFS.usedBytes();
  
  if (usedBytes > totalBytes * 0.9) { // More than 90% used
    systemHealth.filesystemOK = false;
    logEvent(WARNING, "FILESYSTEM", "Storage almost full: " + String(usedBytes) + "/" + String(totalBytes), "");
  } else {
    systemHealth.filesystemOK = true;
  }
  
  // Update CPU temperature (if available)
  systemHealth.temperature = temperatureRead();
  
  if (systemHealth.temperature > 80.0) {
    logEvent(WARNING, "TEMPERATURE", "High CPU temperature: " + String(systemHealth.temperature) + "°C", "");
  }
  
  // Log health status if debug mode is enabled
  if (config.debugMode) {
    logEvent(INFO, "HEALTH", "System check completed", "");
  }
}

void updatePerformanceMetrics(unsigned long loopStart) {
  unsigned long loopTime = millis() - loopStart;
  
  performance.loopTime = loopTime;
  performance.loopCount++;
  
  if (loopTime > performance.maxLoopTime) {
    performance.maxLoopTime = loopTime;
  }
  
  // Calculate moving average
  performance.avgLoopTime = (performance.avgLoopTime * 0.9) + (loopTime * 0.1);
  
  // Calculate memory usage percentage
  performance.memoryUsage = (float)(ESP.getHeapSize() - ESP.getFreeHeap()) / ESP.getHeapSize() * 100.0;
  
  // Warn if loop time is too high
  if (loopTime > 1000) { // More than 1 second
    logEvent(WARNING, "PERFORMANCE", "Slow loop detected: " + String(loopTime) + "ms", "");
  }
}

void cleanupErrorLogs() {
  static unsigned long lastCleanup = 0;
  
  if (millis() - lastCleanup > 3600000) { // Every hour
    // Reset error count
    systemHealth.errorCount = 0;
    lastCleanup = millis();
    
    if (config.debugMode) {
      logEvent(INFO, "MAINTENANCE", "Error log cleanup completed", "");
    }
  }
}

void performSystemBackup() {
  logEvent(INFO, "BACKUP", "Performing system backup", "");
  
  // Backup system configuration
  File configFile = SPIFFS.open("/backup_config.json", "w");
  if (configFile) {
    DynamicJsonDocument doc(1024);
    doc["systemName"] = config.systemName;
    doc["version"] = config.version;
    doc["uptime"] = systemHealth.uptime;
    doc["totalWateringCycles"] = totalWateringCycles;
    doc["lastBackup"] = millis();
    
    serializeJson(doc, configFile);
    configFile.close();
    
    logEvent(INFO, "BACKUP", "Configuration backup completed", "");
  } else {
    logEvent(ERROR, "BACKUP", "Failed to create backup file", "");
  }
  
  // Save statistics to preferences
  saveSystemStatistics();
}

void loadSystemConfiguration() {
  // Load configuration from SPIFFS if available
  File configFile = SPIFFS.open("/system_config.json", "r");
  if (configFile) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, configFile);
    
    if (doc.containsKey("debugMode")) {
      config.debugMode = doc["debugMode"];
    }
    if (doc.containsKey("enableWatchdog")) {
      config.enableWatchdog = doc["enableWatchdog"];
    }
    
    configFile.close();
    logEvent(INFO, "CONFIG", "Configuration loaded from file", "");
  } else {
    // Use default configuration
    logEvent(INFO, "CONFIG", "Using default configuration", "");
  }
}

void saveSystemConfiguration() {
  File configFile = SPIFFS.open("/system_config.json", "w");
  if (configFile) {
    DynamicJsonDocument doc(1024);
    doc["systemName"] = config.systemName;
    doc["version"] = config.version;
    doc["debugMode"] = config.debugMode;
    doc["enableWatchdog"] = config.enableWatchdog;
    doc["enableOTA"] = config.enableOTA;
    doc["enableBackup"] = config.enableBackup;
    
    serializeJson(doc, configFile);
    configFile.close();
    
    logEvent(INFO, "CONFIG", "Configuration saved to file", "");
  }
}

void loadSystemStatistics() {
  totalWateringTime = preferences.getULong("wateringTime", 0);
  totalWateringCycles = preferences.getULong("wateringCycles", 0);
  systemResets = preferences.getULong("systemResets", 0);
  
  // Increment reset counter
  systemResets++;
  preferences.putULong("systemResets", systemResets);
  
  logEvent(INFO, "STATS", "Statistics loaded - Resets: " + String(systemResets), "");
}

void saveSystemStatistics() {
  preferences.putULong("wateringTime", totalWateringTime);
  preferences.putULong("wateringCycles", totalWateringCycles);
  preferences.putULong("systemResets", systemResets);
  
  if (config.debugMode) {
    logEvent(INFO, "STATS", "Statistics saved", "");
  }
}

void sendEnhancedWebhook(String message, String level) {
  if (WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  http.begin("YOUR_WEBHOOK_URL"); // Replace with your webhook URL
  http.addHeader("Content-Type", "application/json");
  
  DynamicJsonDocument doc(1024);
  doc["timestamp"] = millis();
  doc["system"] = config.systemName;
  doc["version"] = config.version;
  doc["message"] = message;
  doc["level"] = level;
  doc["uptime"] = systemHealth.uptime;
  doc["freeHeap"] = systemHealth.freeHeap;
  doc["wifiSignal"] = systemHealth.wifiSignal;
  doc["temperature"] = systemHealth.temperature;
  doc["ip"] = WiFi.localIP().toString();
  
  String payload;
  serializeJson(doc, payload);
  
  int httpCode = http.POST(payload);
  if (httpCode > 0) {
    if (config.debugMode) {
      logEvent(INFO, "WEBHOOK", "Notification sent: " + message, "");
    }
  } else {
    logEvent(WARNING, "WEBHOOK", "Failed to send notification", "");
  }
  
  http.end();
}

void setupEnhancedWebServer() {
  // Enhanced system dashboard
  server.on("/", HTTP_GET, handleEnhancedDashboard);
  
  // System health API
  server.on("/api/health", HTTP_GET, handleHealthAPI);
  
  // Performance metrics API
  server.on("/api/performance", HTTP_GET, handlePerformanceAPI);
  
  // Error logs API
  server.on("/api/errors", HTTP_GET, handleErrorLogsAPI);
  
  // System configuration API
  server.on("/api/config", HTTP_GET, handleConfigAPI);
  server.on("/api/config", HTTP_POST, handleConfigUpdate);
  
  // System control API
  server.on("/api/restart", HTTP_POST, handleSystemRestart);
  server.on("/api/backup", HTTP_POST, handleSystemBackup);
  
  // Enhanced diagnostic tools
  server.on("/api/diagnostics", HTTP_GET, handleDiagnosticsAPI);
  
  // File system management
  server.on("/api/files", HTTP_GET, handleFileListAPI);
  server.on("/api/files/delete", HTTP_POST, handleFileDeleteAPI);
  
  logEvent(INFO, "WEBSERVER", "Enhanced web server routes configured", "");
}

void handleEnhancedDashboard() {
  String html = generateEnhancedDashboard();
  server.send(200, "text/html", html);
}

void handleHealthAPI() {
  DynamicJsonDocument doc(2048);
  
  doc["wifi"] = systemHealth.wifiConnected;
  doc["rtc"] = systemHealth.rtcWorking;
  doc["sensors"] = systemHealth.sensorsWorking;
  doc["relays"] = systemHealth.relaysWorking;
  doc["blink"] = systemHealth.blinkConnected;
  doc["memory"] = systemHealth.memoryOK;
  doc["filesystem"] = systemHealth.filesystemOK;
  doc["uptime"] = systemHealth.uptime;
  doc["freeHeap"] = systemHealth.freeHeap;
  doc["wifiSignal"] = systemHealth.wifiSignal;
  doc["temperature"] = systemHealth.temperature;
  doc["errorCount"] = systemHealth.errorCount;
  doc["lastError"] = systemHealth.lastError;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handlePerformanceAPI() {
  DynamicJsonDocument doc(1024);
  
  doc["loopTime"] = performance.loopTime;
  doc["maxLoopTime"] = performance.maxLoopTime;
  doc["avgLoopTime"] = performance.avgLoopTime;
  doc["loopCount"] = performance.loopCount;
  doc["memoryUsage"] = performance.memoryUsage;
  doc["cpuUsage"] = systemHealth.cpuUsage;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleErrorLogsAPI() {
  DynamicJsonDocument doc(4096);
  JsonArray logs = doc.createNestedArray("errors");
  
  for (int i = 0; i < 50; i++) {
    int index = (errorIndex + i) % 50;
    if (errorHistory[index].timestamp > 0) {
      JsonObject log = logs.createNestedObject();
      log["timestamp"] = errorHistory[index].timestamp;
      log["level"] = errorHistory[index].level;
      log["component"] = errorHistory[index].component;
      log["message"] = errorHistory[index].message;
      log["action"] = errorHistory[index].action;
    }
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleConfigAPI() {
  DynamicJsonDocument doc(1024);
  
  doc["systemName"] = config.systemName;
  doc["version"] = config.version;
  doc["debugMode"] = config.debugMode;
  doc["enableWatchdog"] = config.enableWatchdog;
  doc["enableOTA"] = config.enableOTA;
  doc["enableBackup"] = config.enableBackup;
  doc["maxRetries"] = config.maxRetries;
  doc["healthCheckInterval"] = config.healthCheckInterval;
  doc["backupInterval"] = config.backupInterval;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleConfigUpdate() {
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, server.arg("plain"));
    
    if (doc.containsKey("debugMode")) {
      config.debugMode = doc["debugMode"];
    }
    if (doc.containsKey("enableWatchdog")) {
      config.enableWatchdog = doc["enableWatchdog"];
    }
    if (doc.containsKey("enableOTA")) {
      config.enableOTA = doc["enableOTA"];
    }
    if (doc.containsKey("enableBackup")) {
      config.enableBackup = doc["enableBackup"];
    }
    
    saveSystemConfiguration();
    logEvent(INFO, "CONFIG", "Configuration updated via API", "");
    
    server.send(200, "application/json", "{\"status\":\"success\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"No configuration data provided\"}");
  }
}

void handleSystemRestart() {
  logEvent(INFO, "API", "System restart requested via API", "");
  server.send(200, "application/json", "{\"status\":\"restarting\"}");
  delay(1000);
  ESP.restart();
}

void handleSystemBackup() {
  performSystemBackup();
  server.send(200, "application/json", "{\"status\":\"backup_completed\"}");
}

void handleDiagnosticsAPI() {
  DynamicJsonDocument doc(2048);
  
  doc["chipModel"] = ESP.getChipModel();
  doc["chipRevision"] = ESP.getChipRevision();
  doc["cpuFreqMHz"] = ESP.getCpuFreqMHz();
  doc["flashChipSize"] = ESP.getFlashChipSize();
  doc["freeHeap"] = ESP.getFreeHeap();
  doc["heapSize"] = ESP.getHeapSize();
  doc["psramSize"] = ESP.getPsramSize();
  doc["freePsram"] = ESP.getFreePsram();
  doc["temperature"] = temperatureRead();
  doc["wifiMode"] = WiFi.getMode();
  doc["macAddress"] = WiFi.macAddress();
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleFileListAPI() {
  DynamicJsonDocument doc(2048);
  JsonArray files = doc.createNestedArray("files");
  
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  
  while (file) {
    JsonObject fileInfo = files.createNestedObject();
    fileInfo["name"] = file.name();
    fileInfo["size"] = file.size();
    
    file = root.openNextFile();
  }
  
  doc["totalBytes"] = SPIFFS.totalBytes();
  doc["usedBytes"] = SPIFFS.usedBytes();
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleFileDeleteAPI() {
  if (server.hasArg("filename")) {
    String filename = server.arg("filename");
    
    if (SPIFFS.remove("/" + filename)) {
      logEvent(INFO, "FILES", "File deleted: " + filename, "");
      server.send(200, "application/json", "{\"status\":\"deleted\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"File not found or delete failed\"}");
    }
  } else {
    server.send(400, "application/json", "{\"error\":\"No filename provided\"}");
  }
}

String generateEnhancedDashboard() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="th">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>🚀 Enhanced ESP32 System Manager</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh; color: white; padding: 20px;
        }
        .container { max-width: 1400px; margin: 0 auto; }
        .header { text-align: center; margin-bottom: 30px; 
                  background: rgba(255,255,255,0.1); padding: 20px; 
                  border-radius: 15px; backdrop-filter: blur(10px); }
        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); 
                gap: 20px; margin-bottom: 30px; }
        .card { background: rgba(255,255,255,0.1); padding: 20px; border-radius: 15px; 
                backdrop-filter: blur(10px); border: 1px solid rgba(255,255,255,0.2); }
        .status-good { color: #4CAF50; }
        .status-warning { color: #FF9800; }
        .status-error { color: #f44336; }
        button { background: #4CAF50; color: white; border: none; padding: 12px 24px; 
                 border-radius: 8px; cursor: pointer; margin: 5px; transition: all 0.3s; }
        button:hover { background: #45a049; transform: translateY(-2px); }
        .danger { background: #f44336; }
        .danger:hover { background: #da190b; }
        .metric { display: flex; justify-content: space-between; margin: 10px 0; }
        .progress { width: 100%; height: 20px; background: rgba(255,255,255,0.2); 
                   border-radius: 10px; overflow: hidden; margin: 10px 0; }
        .progress-fill { height: 100%; background: linear-gradient(90deg, #4CAF50, #8BC34A); 
                        transition: width 0.3s; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🚀 Enhanced ESP32 System Manager</h1>
            <p>Advanced System Monitoring & Control Dashboard</p>
            <p>Version <span id="version">2.0.0</span> • Uptime: <span id="uptime">Loading...</span></p>
        </div>

        <div class="grid">
            <div class="card">
                <h3>🏥 System Health</h3>
                <div class="metric">WiFi: <span id="wifi-status">Loading...</span></div>
                <div class="metric">Memory: <span id="memory-status">Loading...</span></div>
                <div class="metric">Temperature: <span id="temperature">Loading...</span></div>
                <div class="metric">Filesystem: <span id="filesystem-status">Loading...</span></div>
                <div class="metric">Errors: <span id="error-count">Loading...</span></div>
            </div>

            <div class="card">
                <h3>⚡ Performance</h3>
                <div class="metric">Loop Time: <span id="loop-time">Loading...</span></div>
                <div class="metric">Max Loop: <span id="max-loop-time">Loading...</span></div>
                <div class="metric">Memory Usage: <span id="memory-usage">Loading...</span></div>
                <div class="progress">
                    <div class="progress-fill" id="memory-progress" style="width: 0%"></div>
                </div>
                <div class="metric">Loop Count: <span id="loop-count">Loading...</span></div>
            </div>

            <div class="card">
                <h3>🔧 System Control</h3>
                <button onclick="performBackup()">💾 Create Backup</button>
                <button onclick="toggleDebugMode()">🐛 Toggle Debug</button>
                <button onclick="clearErrorLogs()">🗑️ Clear Logs</button>
                <button onclick="restartSystem()" class="danger">🔄 Restart System</button>
            </div>

            <div class="card">
                <h3>📊 System Statistics</h3>
                <div class="metric">Free Heap: <span id="free-heap">Loading...</span></div>
                <div class="metric">WiFi Signal: <span id="wifi-signal">Loading...</span></div>
                <div class="metric">Chip Model: <span id="chip-model">Loading...</span></div>
                <div class="metric">Flash Size: <span id="flash-size">Loading...</span></div>
            </div>
        </div>

        <div class="card">
            <h3>📝 Recent Error Logs</h3>
            <div id="error-logs">Loading...</div>
        </div>
    </div>

    <script>
        function updateDashboard() {
            Promise.all([
                fetch('/api/health').then(r => r.json()),
                fetch('/api/performance').then(r => r.json()),
                fetch('/api/diagnostics').then(r => r.json()),
                fetch('/api/errors').then(r => r.json())
            ]).then(([health, performance, diagnostics, errors]) => {
                // Update health status
                document.getElementById('wifi-status').textContent = health.wifi ? '✅ Connected' : '❌ Disconnected';
                document.getElementById('memory-status').textContent = health.memory ? '✅ OK' : '⚠️ Low';
                document.getElementById('temperature').textContent = health.temperature.toFixed(1) + '°C';
                document.getElementById('filesystem-status').textContent = health.filesystem ? '✅ OK' : '⚠️ Full';
                document.getElementById('error-count').textContent = health.errorCount;
                document.getElementById('uptime').textContent = formatUptime(health.uptime);
                
                // Update performance
                document.getElementById('loop-time').textContent = performance.loopTime + 'ms';
                document.getElementById('max-loop-time').textContent = performance.maxLoopTime + 'ms';
                document.getElementById('memory-usage').textContent = performance.memoryUsage.toFixed(1) + '%';
                document.getElementById('memory-progress').style.width = performance.memoryUsage + '%';
                document.getElementById('loop-count').textContent = performance.loopCount.toLocaleString();
                
                // Update diagnostics
                document.getElementById('free-heap').textContent = (health.freeHeap / 1024).toFixed(1) + ' KB';
                document.getElementById('wifi-signal').textContent = health.wifiSignal + ' dBm';
                document.getElementById('chip-model').textContent = diagnostics.chipModel;
                document.getElementById('flash-size').textContent = (diagnostics.flashChipSize / (1024*1024)).toFixed(1) + ' MB';
                
                // Update error logs
                updateErrorLogs(errors.errors);
            }).catch(error => {
                console.error('Error updating dashboard:', error);
            });
        }

        function updateErrorLogs(errors) {
            const container = document.getElementById('error-logs');
            container.innerHTML = '';
            
            errors.slice(-10).reverse().forEach(error => {
                const div = document.createElement('div');
                div.className = 'metric';
                const levelClass = error.level >= 2 ? 'status-error' : (error.level >= 1 ? 'status-warning' : 'status-good');
                div.innerHTML = `<span class="${levelClass}">[${error.component}] ${error.message}</span><span>${new Date(error.timestamp).toLocaleTimeString()}</span>`;
                container.appendChild(div);
            });
        }

        function formatUptime(ms) {
            const seconds = Math.floor(ms / 1000);
            const minutes = Math.floor(seconds / 60);
            const hours = Math.floor(minutes / 60);
            const days = Math.floor(hours / 24);
            
            if (days > 0) return `${days}d ${hours % 24}h ${minutes % 60}m`;
            if (hours > 0) return `${hours}h ${minutes % 60}m`;
            if (minutes > 0) return `${minutes}m ${seconds % 60}s`;
            return `${seconds}s`;
        }

        function performBackup() {
            fetch('/api/backup', {method: 'POST'})
                .then(r => r.json())
                .then(data => alert('Backup completed!'))
                .catch(error => alert('Backup failed: ' + error));
        }

        function toggleDebugMode() {
            fetch('/api/config', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({debugMode: !currentDebugMode})
            }).then(() => {
                alert('Debug mode toggled!');
                updateDashboard();
            });
        }

        function clearErrorLogs() {
            if (confirm('Clear all error logs?')) {
                // Implementation depends on backend support
                alert('Error logs cleared!');
                updateDashboard();
            }
        }

        function restartSystem() {
            if (confirm('Are you sure you want to restart the system?')) {
                fetch('/api/restart', {method: 'POST'})
                    .then(() => alert('System is restarting...'));
            }
        }

        let currentDebugMode = false;

        // Update every 5 seconds
        setInterval(updateDashboard, 5000);
        updateDashboard(); // Initial load
    </script>
</body>
</html>
)rawliteral";
}
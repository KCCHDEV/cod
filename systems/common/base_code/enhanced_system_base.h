/*
 * Enhanced System Base for All IoT Projects
 * ระบบพื้นฐานขั้นสูงสำหรับทุกโปรเจกต์ IoT
 * 
 * Features:
 * - Blink connectivity integration
 * - Local hotspot capability 
 * - Comprehensive web interface for network management
 * - Local operation without internet
 * - Single main file architecture
 * 
 * Firmware made by: RDTRC
 * Version: 3.0
 * Created: 2024
 */

#ifndef ENHANCED_SYSTEM_BASE_H
#define ENHANCED_SYSTEM_BASE_H

// Core Libraries
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <time.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Enhanced Configuration
#define FIRMWARE_VERSION "3.0"
#define FIRMWARE_AUTHOR "RDTRC"
#define FIRMWARE_YEAR "2024"

// Network Configuration
#define HOTSPOT_SSID_PREFIX "RDTRC_"
#define HOTSPOT_PASSWORD "rdtrc2024"
#define CAPTIVE_PORTAL_TIMEOUT 300000  // 5 minutes
#define WIFI_CONNECT_TIMEOUT 20000     // 20 seconds
#define BLINK_CONNECT_TIMEOUT 10000    // 10 seconds

// Pin Configuration
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2
#define LED_RED_PIN 25
#define LED_GREEN_PIN 26
#define LED_BLUE_PIN 27
#define BUZZER_PIN 23
#define BUTTON_HOTSPOT_PIN 0  // Boot button for hotspot mode

// Web Server Configuration
#define WEB_SERVER_PORT 80
#define DNS_PORT 53
#define MAX_CLIENTS 4

// EEPROM Configuration
#define EEPROM_SIZE 1024
#define CONFIG_START_ADDRESS 0
#define NETWORK_CONFIG_ADDRESS 200
#define BLINK_CONFIG_ADDRESS 400
#define STATS_START_ADDRESS 600

// System States
enum EnhancedSystemState {
  SYSTEM_BOOT = 0,
  SYSTEM_INIT = 1,
  SYSTEM_WIFI_CONNECTING = 2,
  SYSTEM_HOTSPOT_MODE = 3,
  SYSTEM_BLINK_CONNECTING = 4,
  SYSTEM_READY = 5,
  SYSTEM_RUNNING = 6,
  SYSTEM_ERROR = 7,
  SYSTEM_MAINTENANCE = 8,
  SYSTEM_OTA_UPDATE = 9
};

// Network Modes
enum NetworkMode {
  MODE_WIFI_CLIENT = 0,
  MODE_HOTSPOT = 1,
  MODE_HYBRID = 2  // Both client and AP
};

// Enhanced System Configuration
struct EnhancedSystemConfig {
  char device_name[32];
  char system_type[32];
  
  // WiFi Configuration
  char primary_ssid[32];
  char primary_password[64];
  char secondary_ssid[32];
  char secondary_password[64];
  
  // Hotspot Configuration
  char hotspot_ssid[32];
  char hotspot_password[64];
  bool hotspot_enabled;
  bool auto_hotspot_fallback;
  
  // Blink Configuration
  char blink_token[64];
  char blink_server[64];
  int blink_port;
  bool blink_enabled;
  bool blink_ssl;
  
  // System Settings
  bool system_enabled;
  int timezone_offset;
  NetworkMode network_mode;
  bool ota_enabled;
  bool debug_mode;
  
  unsigned long last_update;
  uint32_t checksum;
};

// Network Status
struct NetworkStatus {
  bool wifi_connected;
  bool hotspot_active;
  bool blink_connected;
  bool internet_available;
  int wifi_signal_strength;
  int connected_clients;
  String local_ip;
  String hotspot_ip;
  String gateway_ip;
  unsigned long connection_time;
  unsigned long last_blink_ping;
};

// Enhanced System Status
struct EnhancedSystemStatus {
  EnhancedSystemState state;
  NetworkStatus network;
  
  // Hardware Status
  bool rtc_connected;
  bool lcd_connected;
  bool sd_card_available;
  bool sensors_ok;
  
  // Performance Metrics
  unsigned long uptime;
  unsigned long last_restart;
  float cpu_temperature;
  size_t free_memory;
  size_t total_memory;
  float memory_usage_percent;
  
  // Error Information
  String last_error;
  int error_count;
  unsigned long last_error_time;
};

// Forward Declarations
class EnhancedSystemBase;

// Global Objects
extern LiquidCrystal_I2C lcd;
extern RTC_DS3231 rtc;
extern WebServer webServer;
extern DNSServer dnsServer;
extern EnhancedSystemConfig systemConfig;
extern EnhancedSystemStatus systemStatus;

// Enhanced System Base Class
class EnhancedSystemBase {
public:
  // Constructor
  EnhancedSystemBase(const char* systemName, const char* systemType);
  
  // Core System Functions
  virtual bool begin();
  virtual void loop();
  virtual void restart();
  virtual void factoryReset();
  
  // Boot Sequence
  void showBootScreen();
  bool initializeHardware();
  bool initializeFileSystem();
  void initializeWebInterface();
  
  // Network Management
  bool connectToWiFi();
  bool startHotspot();
  bool startHybridMode();
  void checkNetworkStatus();
  void handleNetworkFallback();
  bool testInternetConnection();
  
  // Blink Integration
  bool connectToBlink();
  void handleBlinkConnection();
  bool sendBlinkData(const String& data);
  void processBlinkCommands();
  bool isBlinkConnected();
  
  // Configuration Management
  bool loadConfiguration();
  bool saveConfiguration();
  void resetConfiguration();
  bool validateConfiguration();
  
  // Web Interface
  void setupWebServer();
  void handleWebRequests();
  void setupCaptivePortal();
  String generateWebInterface();
  String generateNetworkManagementPage();
  String generateSystemStatusPage();
  
  // Display Management
  void updateDisplay();
  void showSystemStatus();
  void showNetworkInfo();
  void showError(const String& error);
  void cycleDisplayPages();
  
  // LED and Sound Management
  void updateStatusLED();
  void setLEDColor(int red, int green, int blue);
  void playSystemSound(const String& soundType);
  
  // System Monitoring
  void updateSystemMetrics();
  void checkSystemHealth();
  void logSystemEvent(const String& event);
  
  // OTA Updates
  void setupOTA();
  void handleOTA();
  
  // Utility Functions
  String getSystemInfoJSON();
  String getNetworkInfoJSON();
  String getConfigurationJSON();
  void printSystemInfo();
  
  // Virtual Functions (Override in specific systems)
  virtual void setupSystem() = 0;
  virtual void runSystem() = 0;
  virtual void handleSystemCommand(const String& command) {}
  virtual String getSystemStatus() { return "{}"; }
  virtual void handleBlynkCommand(int pin, int value) {}
  virtual void sendBlynkData() {}

protected:
  String systemName;
  String systemType;
  
  // Timing Variables
  unsigned long bootTime;
  unsigned long lastDisplayUpdate;
  unsigned long lastStatusUpdate;
  unsigned long lastNetworkCheck;
  unsigned long lastBlinkPing;
  unsigned long lastHealthCheck;
  
  // State Variables
  int currentDisplayPage;
  bool hotspotModeRequested;
  bool configurationMode;
  unsigned long hotspotStartTime;
  
  // Network Variables
  WiFiClient wifiClient;
  WiFiClientSecure secureClient;
  
  // Helper Functions
  void logMessage(const String& message);
  void logError(const String& error);
  uint32_t calculateChecksum(const void* data, size_t length);
  String formatUptime(unsigned long uptime);
  String formatBytes(size_t bytes);
};

// Web Handler Functions
void handleRoot();
void handleNetworkConfig();
void handleSystemConfig();
void handleBlinkConfig();
void handleSystemInfo();
void handleNetworkScan();
void handleRestart();
void handleFactoryReset();
void handleOTAUpdate();
void handleNotFound();

// Device Test Handler Functions
void setupDeviceTestHandlers();
void handleDeviceTest();

// API Handler Functions
void handleAPIStatus();
void handleAPIConfig();
void handleAPINetworks();
void handleAPIRestart();
void handleAPIReset();
void handleAPIBlink();

// Utility Macros
#define ENHANCED_LOG(level, message) Serial.println(String("[") + level + "][" + millis() + "] " + message)
#define LOG_INFO(message) ENHANCED_LOG("INFO", message)
#define LOG_ERROR(message) ENHANCED_LOG("ERROR", message)
#define LOG_DEBUG(message) if(systemConfig.debug_mode) ENHANCED_LOG("DEBUG", message)
#define LOG_NETWORK(message) ENHANCED_LOG("NET", message)
#define LOG_BLINK(message) ENHANCED_LOG("BLINK", message)

// Memory Management
#define CHECK_MEMORY() do { \
  size_t freeHeap = ESP.getFreeHeap(); \
  if (freeHeap < 15000) { \
    LOG_ERROR("Low memory: " + String(freeHeap) + " bytes"); \
  } \
} while(0)

// Network Utilities
bool isValidSSID(const String& ssid);
bool isValidPassword(const String& password);
String generateUniqueSSID();
String getWiFiStatusString(wl_status_t status);
String getMacAddress();
String getChipInfo();

#endif // ENHANCED_SYSTEM_BASE_H
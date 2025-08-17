/*
 * ระบบพื้นฐานสำหรับทุกโปรเจกต์ IoT
 * Common Base System for All IoT Projects
 * 
 * Firmware made by: RDTRC
 * Version: 2.0
 * Created: 2024
 */

#ifndef SYSTEM_BASE_H
#define SYSTEM_BASE_H

// ไลบรารีพื้นฐาน (Common Libraries)
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <time.h>

// การตั้งค่าพื้นฐาน (Base Configuration)
#define FIRMWARE_VERSION "2.0"
#define FIRMWARE_AUTHOR "RDTRC"
#define FIRMWARE_YEAR "2024"

// Pin Configuration สำหรับ LCD และ RTC (I2C)
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

// Pin Configuration สำหรับ Status LEDs
#define LED_RED_PIN 25
#define LED_GREEN_PIN 26
#define LED_BLUE_PIN 27
#define BUZZER_PIN 23

// WiFi และ Network Configuration
#define WIFI_TIMEOUT 20000        // 20 วินาที
#define WEB_SERVER_PORT 80
#define MAX_WIFI_RETRY 3

// EEPROM Configuration
#define EEPROM_SIZE 512
#define CONFIG_START_ADDRESS 0
#define STATS_START_ADDRESS 300

// System Status
enum SystemState {
  SYSTEM_BOOT = 0,
  SYSTEM_INIT = 1,
  SYSTEM_CONNECTING = 2,
  SYSTEM_READY = 3,
  SYSTEM_RUNNING = 4,
  SYSTEM_ERROR = 5,
  SYSTEM_MAINTENANCE = 6
};

// Error Codes
enum ErrorCode {
  ERROR_NONE = 0,
  ERROR_WIFI = 1,
  ERROR_RTC = 2,
  ERROR_LCD = 3,
  ERROR_SENSOR = 4,
  ERROR_ACTUATOR = 5,
  ERROR_BLYNK = 6,
  ERROR_MEMORY = 7
};

// Base System Structure
struct SystemConfig {
  char ssid[32];
  char password[64];
  char blynk_token[64];
  char device_name[32];
  bool system_enabled;
  int timezone_offset;
  unsigned long last_update;
  uint32_t checksum;
};

struct SystemStatus {
  SystemState state;
  ErrorCode last_error;
  unsigned long uptime;
  unsigned long last_restart;
  int wifi_signal;
  bool rtc_connected;
  bool lcd_connected;
  bool blynk_connected;
  float cpu_temperature;
  size_t free_memory;
};

// Global Objects
extern LiquidCrystal_I2C lcd;
extern RTC_DS3231 rtc;
extern WebServer server;
extern SystemConfig config;
extern SystemStatus status;

// Base System Functions
class SystemBase {
public:
  // Constructor
  SystemBase(const char* systemName);
  
  // Core Functions
  bool begin();
  void loop();
  void reset();
  void restart();
  
  // Boot Sequence
  void showBootScreen();
  void initializeHardware();
  bool connectWiFi();
  bool initializeRTC();
  bool initializeLCD();
  
  // Configuration
  bool loadConfig();
  bool saveConfig();
  void resetConfig();
  
  // Status Management
  void updateStatus();
  SystemState getState() { return status.state; }
  void setState(SystemState newState);
  void setError(ErrorCode error);
  void clearError();
  
  // Display Functions
  void showSystemInfo();
  void showError(ErrorCode error);
  void showStatus(const String& line1, const String& line2 = "");
  void updateDisplay();
  
  // LED Functions
  void setLEDColor(int red, int green, int blue);
  void blinkLED(int red, int green, int blue, int times = 3);
  void showStatusLED();
  
  // Sound Functions
  void playBeep(int frequency = 1000, int duration = 200);
  void playStartupSound();
  void playErrorSound();
  void playSuccessSound();
  
  // Network Functions
  bool isWiFiConnected();
  void reconnectWiFi();
  void setupWebServer();
  void handleWebRequests();
  
  // Time Functions
  DateTime getCurrentTime();
  String getTimeString();
  String getDateString();
  bool isTimeSet();
  
  // Utility Functions
  void printSystemInfo();
  void printMemoryInfo();
  String getSystemInfoJSON();
  uint32_t calculateChecksum(const void* data, size_t length);
  
  // Virtual Functions (to be overridden by specific systems)
  virtual void setupSystem() = 0;
  virtual void runSystem() = 0;
  virtual void handleSystemCommand(const String& command) {}
  virtual String getSystemStatus() { return "{}"; }

protected:
  String systemName;
  unsigned long lastDisplayUpdate;
  unsigned long lastStatusUpdate;
  unsigned long bootTime;
  int displayPage;
  
  // Protected helper functions
  void logMessage(const String& message);
  void logError(const String& error);
};

// Utility Macros
#define LOG(message) Serial.println(String("[") + millis() + "] " + message)
#define LOG_ERROR(error) Serial.println(String("[ERROR] ") + error)
#define LOG_INFO(info) Serial.println(String("[INFO] ") + info)
#define LOG_DEBUG(debug) Serial.println(String("[DEBUG] ") + debug)

// Memory Management
#define CHECK_MEMORY() do { \
  if (ESP.getFreeHeap() < 10000) { \
    LOG_ERROR("Low memory warning: " + String(ESP.getFreeHeap()) + " bytes"); \
  } \
} while(0)

// Common Web Handlers
void handleRoot();
void handleSystemInfo();
void handleRestart();
void handleReset();
void handleNotFound();

// Common API Endpoints
void setupCommonAPI();
void handleAPIStatus();
void handleAPIConfig();
void handleAPIRestart();
void handleAPIReset();

#endif // SYSTEM_BASE_H
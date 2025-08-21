/*
 * RDTRC Complete Orchid Watering System with LCD - Standalone Version
 * Version: 4.0 - Independent System with LCD I2C 16x2 Support
 * Firmware made by: RDTRC
 * Updated: 2024
 * 
 * Features:
 * - Complete standalone orchid watering system
 * - LCD I2C 16x2 display with auto address detection
 * - Multi-zone watering (6 zones for different orchid types)
 * - Advanced soil moisture monitoring for each zone
 * - Multiple soil moisture sensors for comprehensive monitoring
 * - Temperature and humidity monitoring (DHT22)
 * - Light sensor for day/night detection
 * - pH sensor for water quality monitoring
 * - EC sensor for nutrient monitoring
 * - Automatic watering schedule based on orchid type
 * - Weather-based watering adjustment
 * - Built-in web interface
 * - Blynk integration for mobile control
 * - Hotspot mode for direct access
 * - Data logging to SPIFFS
 * - EMAIL/LINE notifications
 * - OTA updates
 * - Pump protection and flow monitoring
 * - LCD debug and status display
 * - Offline sensor detection and graceful degradation
 */

// Blynk Configuration - MUST be defined BEFORE includes
#define BLYNK_TEMPLATE_ID "TMPL61Zdwsx9r"
#define BLYNK_TEMPLATE_NAME "Orchid_Watering_System"
#define BLYNK_AUTH_TOKEN "H4AnMNnYtDTRBl1qssnraGZbVmnKoC8e"

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <BlynkSimpleEsp32.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoOTA.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RDTRC_LCD_Library.h"
#include "RDTRC_Common_Library.h"

// System Configuration
#define FIRMWARE_VERSION "4.0"
#define FIRMWARE_MAKER "RDTRC"
#define SYSTEM_NAME "Orchid Watering"
#define DEVICE_ID "RDTRC_ORCHID_WATER"

// Network Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* hotspot_ssid = "RDTRC_OrchidWater";
const char* hotspot_password = "rdtrc123";

// LINE Notify Configuration
const char* lineToken = "YOUR_LINE_NOTIFY_TOKEN";

// Pin Definitions
#define DHT_PIN 32
#define DHT_TYPE DHT22
#define LIGHT_SENSOR_PIN 33
#define WATER_PUMP_PIN 18
#define FLOW_SENSOR_PIN 19
#define WATER_LEVEL_TRIG_PIN 25
#define WATER_LEVEL_ECHO_PIN 14
#define STATUS_LED_PIN 2
#define BUZZER_PIN 4
#define RESET_BUTTON_PIN 0
#define LCD_NEXT_BUTTON_PIN 26  // Button to navigate LCD pages

// Soil Moisture Sensors (6 zones + 2 additional sensors for redundancy)
#define SOIL_SENSOR_1_PIN 34
#define SOIL_SENSOR_2_PIN 35
#define SOIL_SENSOR_3_PIN 36
#define SOIL_SENSOR_4_PIN 39
#define SOIL_SENSOR_5_PIN 23
#define SOIL_SENSOR_6_PIN 27
#define SOIL_SENSOR_7_PIN 13  // Additional sensor for comprehensive monitoring
#define SOIL_SENSOR_8_PIN 12  // Additional sensor for comprehensive monitoring

// pH and EC Sensors
#define PH_SENSOR_PIN 15
#define EC_SENSOR_PIN 16

// Solenoid Valves (6 zones for different orchid types)
#define VALVE_1_PIN 5   // Phalaenopsis
#define VALVE_2_PIN 17  // Cattleya
#define VALVE_3_PIN 21  // Dendrobium
#define VALVE_4_PIN 22  // Vanda
#define VALVE_5_PIN 23  // Oncidium
#define VALVE_6_PIN 25  // Cymbidium

// I2C Pins
#define I2C_SDA 21
#define I2C_SCL 22

// Watering Configuration
#define NUM_ZONES 6
#define DEFAULT_WATERING_DURATION 20000 // 20 seconds per zone (orchids need less water)
#define MIN_WATERING_DURATION 5000      // 5 seconds minimum
#define MAX_WATERING_DURATION 60000     // 1 minute maximum
#define DRY_SOIL_THRESHOLD 25           // Below 25% moisture = dry (orchids prefer drier conditions)
#define WET_SOIL_THRESHOLD 60           // Above 60% moisture = wet
#define WATER_TANK_HEIGHT 50            // cm
#define LOW_WATER_THRESHOLD 10          // cm

// Orchid-specific thresholds
#define ORCHID_TEMP_MIN 18.0
#define ORCHID_TEMP_MAX 30.0
#define ORCHID_HUMIDITY_MIN 50.0
#define ORCHID_HUMIDITY_MAX 80.0
#define ORCHID_PH_MIN 5.5
#define ORCHID_PH_MAX 6.5
#define ORCHID_EC_MIN 0.5
#define ORCHID_EC_MAX 2.0

// Sensor offline detection
#define SENSOR_TIMEOUT 30000  // 30 seconds
#define SENSOR_RETRY_INTERVAL 60000  // 1 minute

// System Objects
WebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); // UTC+7 Thailand
DHT dht(DHT_PIN, DHT_TYPE);
RDTRC_LCD systemLCD;

// System Variables
bool isWiFiConnected = false;
bool isHotspotMode = false;
unsigned long lastHeartbeat = 0;
unsigned long lastDataLog = 0;
unsigned long lastStatusCheck = 0;
unsigned long lastLCDUpdate = 0;
unsigned long lastSensorCheck = 0;

// Sensor status tracking
struct SensorStatus {
  bool isOnline;
  unsigned long lastReading;
  float lastValue;
  int errorCount;
  String sensorName;
};

SensorStatus soilSensors[8];
SensorStatus dhtSensor;
SensorStatus lightSensor;
SensorStatus phSensor;
SensorStatus ecSensor;
SensorStatus waterLevelSensor;
SensorStatus flowSensor;

// Environmental data
struct OrchidEnvironmentalData {
  float temperature;
  float humidity;
  int lightLevel;
  float waterLevel;
  float phLevel;
  float ecLevel;
  int soilMoisture[8];
  bool isDaylight;
  unsigned long timestamp;
} envData;

// Watering schedule for different orchid types
struct OrchidWateringSchedule {
  String orchidType;
  int wateringInterval;  // hours
  int wateringDuration;  // seconds
  int soilMoistureThreshold;
  bool isActive;
};

OrchidWateringSchedule orchidSchedules[NUM_ZONES] = {
  {"Phalaenopsis", 168, 15000, 20, true},  // Weekly, 15 seconds
  {"Cattleya", 120, 12000, 15, true},      // 5 days, 12 seconds
  {"Dendrobium", 144, 18000, 18, true},    // 6 days, 18 seconds
  {"Vanda", 72, 25000, 12, true},          // 3 days, 25 seconds
  {"Oncidium", 96, 16000, 22, true},       // 4 days, 16 seconds
  {"Cymbidium", 168, 20000, 25, true}      // Weekly, 20 seconds
};

// System status
struct OrchidSystemStatus {
  bool wifiConnected;
  bool maintenanceMode;
  unsigned long uptime;
  int freeMemory;
  int wifiSignal;
  String systemName;
  String deviceId;
  String firmwareVersion;
  int onlineSensors;
  int totalSensors;
} systemStatus;

// Function prototypes
void setup();
void loop();
void initializeSensors();
void checkSensorStatus();
void readAllSensors();
void updateEnvironmentalData();
void handleWatering();
void checkWateringSchedule();
void waterZone(int zone, int duration);
void updateLCD();
void handleWebServer();
void sendLineNotification(String message);
void logData();
void setupWiFi();
void setupHotspot();
void setupOTA();
void setupWebServer();
void setupBlynk();
void checkSystemHealth();
void emergencyStop();
void playNotificationSound();
void displaySystemStatus();
void handleManualOverride();
void saveConfiguration();
void loadConfiguration();
void resetSystem();
void calibrateSensors();
void updateBlynk();
void handleSensorError(int sensorIndex, String sensorName);
void gracefulDegradation();


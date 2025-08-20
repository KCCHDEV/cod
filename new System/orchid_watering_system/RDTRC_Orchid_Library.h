/*
 * RDTRC Orchid Library - Orchid-specific Functions and Utilities
 * Version: 4.0
 * Firmware made by: RDTRC
 * Updated: 2024
 * 
 * This library contains orchid-specific functions and utilities for
 * the RDTRC orchid watering system
 * 
 * Usage:
 * #include "RDTRC_Orchid_Library.h"
 */

#ifndef RDTRC_ORCHID_LIBRARY_H
#define RDTRC_ORCHID_LIBRARY_H

#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Orchid-specific sensor management
class RDTRCOrchidSensors {
  private:
    // Sensor pin arrays
    int soilSensorPins[8];
    int valvePins[6];
    
    // Sensor status tracking
    struct SensorInfo {
      bool isOnline;
      unsigned long lastReading;
      float lastValue;
      int errorCount;
      String sensorName;
      int pin;
      float calibrationOffset;
      float calibrationMultiplier;
    };
    
    SensorInfo sensors[16]; // 8 soil + DHT + light + pH + EC + water level + flow + LCD
    int totalSensors;
    
  public:
    RDTRCOrchidSensors();
    
    // Sensor initialization
    void initializeSoilSensors(int pins[8]);
    void initializeValves(int pins[6]);
    void initializeDHT(int pin, int type);
    void initializeLightSensor(int pin);
    void initializePHSensor(int pin);
    void initializeECSensor(int pin);
    void initializeWaterLevelSensor(int trigPin, int echoPin);
    void initializeFlowSensor(int pin);
    void initializeLCD(int sdaPin, int sclPin);
    
    // Sensor reading functions
    float readSoilMoisture(int sensorIndex);
    float readTemperature();
    float readHumidity();
    int readLightLevel();
    float readPHLevel();
    float readECLevel();
    float readWaterLevel();
    float readFlowRate();
    
    // Sensor status management
    bool isSensorOnline(int sensorIndex);
    void updateSensorStatus(int sensorIndex, bool isOnline, float value);
    void handleSensorError(int sensorIndex);
    void resetSensorError(int sensorIndex);
    int getOnlineSensorCount();
    int getTotalSensorCount();
    String getSensorStatusString();
    
    // Calibration functions
    void calibrateSoilSensor(int sensorIndex, float dryValue, float wetValue);
    void calibratePHSensor(float ph4Value, float ph7Value, float ph10Value);
    void calibrateECSensor(float ec1Value, float ec2Value);
    
    // Graceful degradation
    void gracefulDegradation();
    bool canOperateWithOfflineSensors();
    String getDegradationStatus();
};

// Orchid watering management
class RDTRCOrchidWatering {
  private:
    struct WateringZone {
      String orchidType;
      int wateringInterval;  // hours
      int wateringDuration;  // seconds
      int soilMoistureThreshold;
      bool isActive;
      unsigned long lastWatering;
      int valvePin;
      int soilSensorIndex;
    };
    
    WateringZone zones[6];
    int numZones;
    
  public:
    RDTRCOrchidWatering();
    
    // Zone management
    void addZone(int zoneIndex, String orchidType, int interval, int duration, int threshold, int valvePin, int sensorIndex);
    void setZoneActive(int zoneIndex, bool active);
    void updateZoneSchedule(int zoneIndex, int interval, int duration, int threshold);
    
    // Watering logic
    bool shouldWaterZone(int zoneIndex, float soilMoisture, float temperature, float humidity);
    void waterZone(int zoneIndex, int duration);
    void emergencyWatering(int zoneIndex, int duration);
    void stopAllWatering();
    
    // Schedule management
    void checkWateringSchedule();
    unsigned long getTimeSinceLastWatering(int zoneIndex);
    String getZoneStatus(int zoneIndex);
    String getAllZonesStatus();
    
    // Orchid-specific adjustments
    void adjustForOrchidType(int zoneIndex, float temperature, float humidity, int lightLevel);
    bool isOptimalConditions(float temperature, float humidity, int lightLevel);
    String getOrchidCareRecommendations(int zoneIndex);
};

// Environmental monitoring for orchids
class RDTRCOrchidEnvironment {
  private:
    struct EnvironmentalThresholds {
      float tempMin, tempMax;
      float humidityMin, humidityMax;
      float phMin, phMax;
      float ecMin, ecMax;
      int lightMin, lightMax;
    };
    
    EnvironmentalThresholds orchidThresholds;
    
  public:
    RDTRCOrchidEnvironment();
    
    // Environmental assessment
    bool isTemperatureOptimal(float temperature);
    bool isHumidityOptimal(float humidity);
    bool isPHOptimal(float ph);
    bool isECOptimal(float ec);
    bool isLightOptimal(int lightLevel);
    bool isEnvironmentOptimal(float temp, float humidity, float ph, float ec, int light);
    
    // Orchid health assessment
    String assessOrchidHealth(float temp, float humidity, float ph, float ec, int light, int soilMoisture);
    String getEnvironmentalRecommendations(float temp, float humidity, float ph, float ec, int light);
    int getEnvironmentalScore(float temp, float humidity, float ph, float ec, int light);
    
    // Threshold management
    void setTemperatureThresholds(float min, float max);
    void setHumidityThresholds(float min, float max);
    void setPHThresholds(float min, float max);
    void setECThresholds(float min, float max);
    void setLightThresholds(int min, int max);
    
    // Data logging
    String getEnvironmentalDataString(float temp, float humidity, float ph, float ec, int light, int soilMoisture);
    void logEnvironmentalData(float temp, float humidity, float ph, float ec, int light, int soilMoisture);
};

// LCD display management for orchids
class RDTRCOrchidLCD {
  private:
    LiquidCrystal_I2C* lcd;
    int currentPage;
    int totalPages;
    unsigned long lastUpdate;
    
  public:
    RDTRCOrchidLCD(LiquidCrystal_I2C* lcdInstance);
    
    // Display management
    void initializeDisplay();
    void nextPage();
    void previousPage();
    void setPage(int page);
    
    // Content display
    void displaySystemStatus(bool wifiConnected, int onlineSensors, int totalSensors);
    void displayEnvironmentalData(float temp, float humidity, int light, float ph, float ec);
    void displayWateringStatus(int activeZones, String lastWatering);
    void displaySensorStatus(String sensorStatus);
    void displayOrchidHealth(String healthStatus);
    void displayError(String errorMessage);
    void displayOfflineSensors(int offlineCount);
    
    // Specialized orchid displays
    void displayOrchidCareTips(int zoneIndex);
    void displayWateringSchedule();
    void displayEnvironmentalRecommendations();
    void displaySensorCalibration();
    
    // Utility functions
    void clearDisplay();
    void showLoading();
    void showSuccess();
    void showError();
    void showWarning();
};

// Notification system for orchids
class RDTRCOrchidNotifications {
  private:
    String lineToken;
    bool notificationsEnabled;
    
  public:
    RDTRCOrchidNotifications(String token);
    
    // Notification types
    void sendWateringNotification(int zoneIndex, int duration);
    void sendSensorOfflineNotification(int sensorIndex, String sensorName);
    void sendEnvironmentalAlert(float temp, float humidity, float ph, float ec, int light);
    void sendLowWaterAlert(float waterLevel);
    void sendSystemErrorNotification(String error);
    void sendMaintenanceReminder();
    void sendOrchidHealthAlert(int zoneIndex, String healthStatus);
    
    // LINE Notify integration
    void sendLineNotification(String message);
    void sendLineImage(String imageUrl, String message);
    
    // Email notifications (if configured)
    void sendEmailNotification(String subject, String message);
    
    // System notifications
    void enableNotifications(bool enable);
    bool areNotificationsEnabled();
    
    // Custom notifications
    void sendCustomNotification(String title, String message, String type);
};

// Data logging for orchids
class RDTRCOrchidLogger {
  private:
    String logFileName;
    unsigned long lastLogTime;
    int logInterval;
    
  public:
    RDTRCOrchidLogger(String fileName);
    
    // Logging functions
    void logEnvironmentalData(float temp, float humidity, float ph, float ec, int light, int soilMoisture[8]);
    void logWateringEvent(int zoneIndex, int duration, float soilMoisture);
    void logSensorError(int sensorIndex, String error);
    void logSystemEvent(String event);
    void logOrchidHealth(int zoneIndex, String healthStatus);
    
    // Data management
    void setLogInterval(int interval);
    void clearLogs();
    String getLogData(int lines);
    void exportLogs();
    
    // Analysis functions
    String getDailySummary();
    String getWeeklyReport();
    String getMonthlyReport();
    float getAverageTemperature(int days);
    float getAverageHumidity(int days);
    int getWateringFrequency(int zoneIndex, int days);
};

// Configuration management for orchids
class RDTRCOrchidConfig {
  private:
    String configFileName;
    
  public:
    RDTRCOrchidConfig(String fileName);
    
    // Configuration functions
    void saveConfiguration();
    void loadConfiguration();
    void resetToDefaults();
    void backupConfiguration();
    void restoreConfiguration();
    
    // Settings management
    void setWateringSchedule(int zoneIndex, int interval, int duration, int threshold);
    void setEnvironmentalThresholds(float tempMin, float tempMax, float humidityMin, float humidityMax, 
                                   float phMin, float phMax, float ecMin, float ecMax);
    void setNotificationSettings(bool enabled, String lineToken);
    void setSystemSettings(String deviceName, String location, int logInterval);
    
    // Validation
    bool validateConfiguration();
    String getConfigurationErrors();
};

#endif // RDTRC_ORCHID_LIBRARY_H

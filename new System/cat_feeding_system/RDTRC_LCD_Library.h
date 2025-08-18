/*
 * RDTRC LCD Library - I2C LCD 16x2 Support with Auto Address Detection
 * Version: 4.0
 * Firmware made by: RDTRC
 * Updated: 2024
 * 
 * Features:
 * - Auto I2C address detection
 * - Status display management
 * - Debug information display
 * - Scrolling text support
 * - Multiple page display
 */

#ifndef RDTRC_LCD_LIBRARY_H
#define RDTRC_LCD_LIBRARY_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

class RDTRC_LCD {
  private:
    LiquidCrystal_I2C* lcd;
    uint8_t address;
    bool isConnected;
    unsigned long lastUpdate;
    int currentPage;
    int totalPages;
    bool autoScroll;
    unsigned long scrollInterval;
    unsigned long lastScroll;
    
    // Status data structure
    struct StatusData {
      String systemName;
      float temperature;
      float humidity;
      int moisture;
      String growthPhase;
      bool wifiConnected;
      bool maintenanceMode;
      int alerts;
      String lastAction;
    } status;
    
  public:
    RDTRC_LCD() {
      lcd = nullptr;
      address = 0x00;
      isConnected = false;
      lastUpdate = 0;
      currentPage = 0;
      totalPages = 4;
      autoScroll = true;
      scrollInterval = 3000; // 3 seconds per page
      lastScroll = 0;
    }
    
    ~RDTRC_LCD() {
      if (lcd) {
        delete lcd;
      }
    }
    
    // Initialize LCD with auto address detection
    bool begin() {
      Serial.println("Scanning for I2C LCD...");
      
      // Common I2C LCD addresses to try
      uint8_t addresses[] = {0x27, 0x3F, 0x26, 0x20, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E};
      int numAddresses = sizeof(addresses) / sizeof(addresses[0]);
      
      Wire.begin();
      
      for (int i = 0; i < numAddresses; i++) {
        Wire.beginTransmission(addresses[i]);
        if (Wire.endTransmission() == 0) {
          address = addresses[i];
          Serial.println("LCD found at address: 0x" + String(address, HEX));
          
          // Try to initialize LCD
          if (lcd) delete lcd;
          lcd = new LiquidCrystal_I2C(address, 16, 2);
          lcd->init();
          lcd->backlight();
          
          // Test LCD
          lcd->clear();
          lcd->setCursor(0, 0);
          lcd->print("RDTRC LCD Test");
          lcd->setCursor(0, 1);
          lcd->print("Addr: 0x" + String(address, HEX));
          
          delay(2000);
          isConnected = true;
          showBootScreen();
          return true;
        }
      }
      
      Serial.println("No I2C LCD found");
      isConnected = false;
      return false;
    }
    
    // Check if LCD is connected
    bool isLCDConnected() {
      return isConnected;
    }
    
    // Get LCD address
    uint8_t getLCDAddress() {
      return address;
    }
    
    // Show boot screen
    void showBootScreen() {
      if (!isConnected) return;
      
      lcd->clear();
      lcd->setCursor(0, 0);
      lcd->print("RDTRC System");
      lcd->setCursor(0, 1);
      lcd->print("Starting...");
      delay(2000);
      
      lcd->clear();
      lcd->setCursor(0, 0);
      lcd->print("Version 4.0");
      lcd->setCursor(0, 1);
      lcd->print("LCD Ready!");
      delay(2000);
    }
    
    // Update status data
    void updateStatus(String systemName, float temp, float humidity, int moisture, 
                     String phase, bool wifi, bool maintenance, int alerts, String action = "") {
      status.systemName = systemName;
      status.temperature = temp;
      status.humidity = humidity;
      status.moisture = moisture;
      status.growthPhase = phase;
      status.wifiConnected = wifi;
      status.maintenanceMode = maintenance;
      status.alerts = alerts;
      if (action != "") {
        status.lastAction = action;
      }
    }
    
    // Display current page
    void displayPage(int page) {
      if (!isConnected) return;
      
      lcd->clear();
      
      switch (page) {
        case 0: // System Info
          lcd->setCursor(0, 0);
          lcd->print(status.systemName.substring(0, 16));
          lcd->setCursor(0, 1);
          if (status.maintenanceMode) {
            lcd->print("MAINTENANCE MODE");
          } else if (status.wifiConnected) {
            lcd->print("WiFi: Connected");
          } else {
            lcd->print("WiFi: Hotspot");
          }
          break;
          
        case 1: // Environmental Data
          lcd->setCursor(0, 0);
          lcd->print("T:" + String(status.temperature, 1) + "C H:" + String(status.humidity, 0) + "%");
          lcd->setCursor(0, 1);
          lcd->print("Soil: " + String(status.moisture) + "%");
          break;
          
        case 2: // Growth Phase
          lcd->setCursor(0, 0);
          lcd->print("Phase:");
          lcd->setCursor(0, 1);
          lcd->print(status.growthPhase.substring(0, 16));
          break;
          
        case 3: // Status & Alerts
          lcd->setCursor(0, 0);
          if (status.alerts > 0) {
            lcd->print("Alerts: " + String(status.alerts));
          } else {
            lcd->print("Status: OK");
          }
          lcd->setCursor(0, 1);
          if (status.lastAction != "") {
            lcd->print(status.lastAction.substring(0, 16));
          } else {
            lcd->print("System Running");
          }
          break;
          
        default:
          currentPage = 0;
          displayPage(0);
          break;
      }
    }
    
    // Auto scroll through pages
    void autoScrollPages() {
      if (!isConnected || !autoScroll) return;
      
      if (millis() - lastScroll > scrollInterval) {
        currentPage = (currentPage + 1) % totalPages;
        displayPage(currentPage);
        lastScroll = millis();
      }
    }
    
    // Manual page navigation
    void nextPage() {
      if (!isConnected) return;
      currentPage = (currentPage + 1) % totalPages;
      displayPage(currentPage);
      autoScroll = false; // Disable auto scroll when manually navigating
    }
    
    void prevPage() {
      if (!isConnected) return;
      currentPage = (currentPage - 1 + totalPages) % totalPages;
      displayPage(currentPage);
      autoScroll = false;
    }
    
    // Enable/disable auto scroll
    void setAutoScroll(bool enable, unsigned long interval = 3000) {
      autoScroll = enable;
      scrollInterval = interval;
      if (enable) {
        lastScroll = millis();
      }
    }
    
    // Show debug message
    void showDebug(String line1, String line2 = "", unsigned long displayTime = 2000) {
      if (!isConnected) return;
      
      bool wasAutoScroll = autoScroll;
      autoScroll = false; // Temporarily disable auto scroll
      
      lcd->clear();
      lcd->setCursor(0, 0);
      lcd->print(line1.substring(0, 16));
      if (line2 != "") {
        lcd->setCursor(0, 1);
        lcd->print(line2.substring(0, 16));
      }
      
      delay(displayTime);
      
      autoScroll = wasAutoScroll; // Restore auto scroll setting
      displayPage(currentPage); // Return to current page
    }
    
    // Show alert message
    void showAlert(String message, unsigned long displayTime = 5000) {
      if (!isConnected) return;
      
      bool wasAutoScroll = autoScroll;
      autoScroll = false;
      
      lcd->clear();
      lcd->setCursor(0, 0);
      lcd->print("ALERT!");
      lcd->setCursor(0, 1);
      lcd->print(message.substring(0, 16));
      
      // Blink backlight for attention
      for (int i = 0; i < 3; i++) {
        lcd->noBacklight();
        delay(200);
        lcd->backlight();
        delay(200);
      }
      
      delay(displayTime - 1200); // Subtract blink time
      
      autoScroll = wasAutoScroll;
      displayPage(currentPage);
    }
    
    // Show custom message
    void showMessage(String line1, String line2 = "", unsigned long displayTime = 2000) {
      if (!isConnected) return;
      
      bool wasAutoScroll = autoScroll;
      autoScroll = false;
      
      lcd->clear();
      lcd->setCursor(0, 0);
      lcd->print(line1.substring(0, 16));
      if (line2 != "") {
        lcd->setCursor(0, 1);
        lcd->print(line2.substring(0, 16));
      }
      
      delay(displayTime);
      
      autoScroll = wasAutoScroll;
      displayPage(currentPage);
    }
    
    // Update display (call this in main loop)
    void update() {
      if (!isConnected) return;
      
      autoScrollPages();
    }
    
    // Clear display
    void clear() {
      if (!isConnected) return;
      lcd->clear();
    }
    
    // Turn backlight on/off
    void setBacklight(bool on) {
      if (!isConnected) return;
      if (on) {
        lcd->backlight();
      } else {
        lcd->noBacklight();
      }
    }
    
    // Get current page
    int getCurrentPage() {
      return currentPage;
    }
    
    // Set current page
    void setCurrentPage(int page) {
      if (page >= 0 && page < totalPages) {
        currentPage = page;
        displayPage(currentPage);
      }
    }
    
    // Scan and list all I2C devices
    void scanI2C() {
      Serial.println("Scanning I2C bus...");
      int deviceCount = 0;
      
      for (uint8_t address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
          Serial.println("I2C device found at address 0x" + String(address, HEX));
          deviceCount++;
        }
      }
      
      if (deviceCount == 0) {
        Serial.println("No I2C devices found");
      } else {
        Serial.println("Found " + String(deviceCount) + " I2C device(s)");
      }
    }
};

// Global LCD instance
extern RDTRC_LCD systemLCD;

#endif // RDTRC_LCD_LIBRARY_H
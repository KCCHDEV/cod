/*
 * RDTRC I2C LCD Scanner and Tester
 * Version: 4.0
 * Firmware made by: RDTRC
 * Updated: 2024
 * 
 * This utility scans for I2C devices and tests LCD displays
 * Use this to find your LCD I2C address before using the main systems
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// I2C Pins (ESP32 default)
#define I2C_SDA 21
#define I2C_SCL 22

// Test button pin
#define TEST_BUTTON_PIN 26

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n" + String("=").repeat(60));
  Serial.println("RDTRC I2C LCD Scanner and Tester");
  Serial.println("Version: 4.0");
  Serial.println("=".repeat(60));
  
  pinMode(TEST_BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.println("I2C initialized with SDA=" + String(I2C_SDA) + ", SCL=" + String(I2C_SCL));
  
  // Scan for devices
  scanI2CDevices();
  
  // Test LCD addresses
  testLCDAddresses();
  
  Serial.println("\nPress the test button (Pin 26) to run tests again");
  Serial.println("Monitor this output to find your LCD address");
}

void loop() {
  // Check test button
  static bool lastButtonState = HIGH;
  static unsigned long lastButtonPress = 0;
  
  bool currentButtonState = digitalRead(TEST_BUTTON_PIN);
  
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    if (millis() - lastButtonPress > 1000) { // Debounce
      Serial.println("\n" + String("-").repeat(40));
      Serial.println("Button pressed - Running tests again...");
      Serial.println(String("-").repeat(40));
      
      scanI2CDevices();
      testLCDAddresses();
      
      lastButtonPress = millis();
    }
  }
  
  lastButtonState = currentButtonState;
  delay(100);
}

void scanI2CDevices() {
  Serial.println("\nScanning I2C bus for devices...");
  Serial.println("Address  | Status    | Device Type (Possible)");
  Serial.println("---------|-----------|----------------------");
  
  int deviceCount = 0;
  
  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.print("     | Found     | ");
      
      // Identify common device types
      switch (address) {
        case 0x27:
        case 0x3F:
          Serial.println("LCD I2C (PCF8574)");
          break;
        case 0x48:
        case 0x49:
        case 0x4A:
        case 0x4B:
          Serial.println("ADS1115 ADC");
          break;
        case 0x68:
          Serial.println("DS3231 RTC / MPU6050");
          break;
        case 0x76:
        case 0x77:
          Serial.println("BMP280/BME280");
          break;
        case 0x40:
          Serial.println("PCA9685 PWM");
          break;
        default:
          Serial.println("Unknown device");
          break;
      }
      deviceCount++;
    } else if (error == 4) {
      Serial.print("0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("     | Error     | Unknown error");
    }
  }
  
  if (deviceCount == 0) {
    Serial.println("No I2C devices found!");
    Serial.println("\nTroubleshooting:");
    Serial.println("1. Check wiring: SDA→21, SCL→22, VCC→3.3V, GND→GND");
    Serial.println("2. Check power supply");
    Serial.println("3. Check pull-up resistors (usually built into I2C backpack)");
    Serial.println("4. Try different I2C pins if needed");
  } else {
    Serial.println("\nFound " + String(deviceCount) + " I2C device(s)");
  }
}

void testLCDAddresses() {
  Serial.println("\nTesting LCD I2C addresses...");
  
  // Common LCD I2C addresses
  uint8_t lcdAddresses[] = {0x27, 0x3F, 0x26, 0x20, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E};
  int numAddresses = sizeof(lcdAddresses) / sizeof(lcdAddresses[0]);
  
  bool lcdFound = false;
  
  for (int i = 0; i < numAddresses; i++) {
    uint8_t addr = lcdAddresses[i];
    
    Serial.print("Testing address 0x");
    if (addr < 16) Serial.print("0");
    Serial.print(addr, HEX);
    Serial.print("... ");
    
    // Check if device exists at this address
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("Device found! ");
      
      // Try to initialize LCD
      LiquidCrystal_I2C testLCD(addr, 16, 2);
      testLCD.init();
      testLCD.backlight();
      
      // Test display
      testLCD.clear();
      testLCD.setCursor(0, 0);
      testLCD.print("RDTRC Test");
      testLCD.setCursor(0, 1);
      testLCD.print("Addr: 0x" + String(addr, HEX));
      
      Serial.println("LCD test successful!");
      Serial.println("*** USE THIS ADDRESS: 0x" + String(addr, HEX) + " ***");
      
      delay(3000);
      
      // Show success message
      testLCD.clear();
      testLCD.setCursor(0, 0);
      testLCD.print("LCD Working!");
      testLCD.setCursor(0, 1);
      testLCD.print("Address: 0x" + String(addr, HEX));
      
      lcdFound = true;
      
      // Wait a bit more to see the result
      delay(2000);
      
      // Clear for next test or normal operation
      testLCD.clear();
      testLCD.setCursor(0, 0);
      testLCD.print("Test Complete");
      testLCD.setCursor(0, 1);
      testLCD.print("Check Serial");
      
      break; // Stop testing after first successful LCD
      
    } else {
      Serial.println("No device");
    }
  }
  
  if (!lcdFound) {
    Serial.println("\nNo working LCD found at common addresses");
    Serial.println("Your LCD might use a different address or need different wiring");
  } else {
    Serial.println("\nLCD test completed successfully!");
    Serial.println("You can now use this address in your growing system");
  }
}

String repeat(String str, int count) {
  String result = "";
  for (int i = 0; i < count; i++) {
    result += str;
  }
  return result;
}
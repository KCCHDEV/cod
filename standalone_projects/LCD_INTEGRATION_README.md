# RDTRC Growing Systems - LCD I2C 16x2 Integration

## Overview

All RDTRC growing systems now support **LCD I2C 16x2 displays** with **automatic address detection** for debugging and status monitoring. The LCD provides real-time system information without needing a computer or phone.

## 📁 Files Created

### LCD Library
- **`RDTRC_LCD_Library.h`** - Complete LCD management library with auto-detection

### Enhanced Systems with LCD
1. **`mushroom_system_with_lcd.ino`** - Mushroom system with LCD support
2. **`cilantro_system_with_lcd.ino`** - Cilantro system with LCD support  
3. **`combined_system_with_lcd.ino`** - Dual-zone system with LCD support

## 🔧 Hardware Requirements

### Required Components
- **ESP32 Development Board**
- **LCD I2C 16x2 Display** (with I2C backpack)
- **4.7kΩ Pull-up resistors** (usually built into I2C backpack)
- **Jumper wires**

### Wiring Connections

```
ESP32 Pin  →  LCD I2C Pin
GND        →  GND
3.3V       →  VCC
GPIO 21    →  SDA
GPIO 22    →  SCL
```

### Additional Control Pin
- **GPIO 26** → LCD Navigation Button (with pull-up resistor)

## 📊 LCD Display Features

### 🔍 Auto Address Detection
The system automatically scans and detects LCD I2C addresses:
- **Common addresses tested**: 0x27, 0x3F, 0x26, 0x20, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E
- **Automatic initialization** when LCD is found
- **Graceful fallback** if no LCD is detected

### 📱 Display Pages

#### Single Zone Systems (Mushroom/Cilantro)
**Page 0: System Info**
```
Mushroom System 
WiFi: Connected
```

**Page 1: Environmental Data**
```
T:22.1C H:85%
Soil: 72%
```

**Page 2: Growth Phase**
```
Phase:
Colonization
```

**Page 3: Status & Alerts**
```
Status: OK
System Running
```

#### Dual Zone System
**Auto-switches between zones every 6 seconds:**
- **Zone 0**: Mushroom data (6 seconds)
- **Zone 1**: Cilantro data (6 seconds)
- **Manual switch**: Button on GPIO 26

### 🎛️ LCD Controls

#### Auto Scroll
- **Default**: Enabled with 4-second intervals
- **Pages cycle**: 0 → 1 → 2 → 3 → 0...
- **Dual zone**: Switches between mushroom and cilantro data

#### Manual Navigation
- **Button**: GPIO 26 (with pull-up)
- **Action**: Press to go to next page/zone
- **Debounce**: 200ms
- **Auto-scroll**: Disabled when manually navigating

#### Debug Messages
- **Temporary display**: Shows debug info for 2 seconds
- **Auto-return**: Returns to normal display after timeout
- **Priority**: Debug messages override normal display

#### Alert Display
- **Visual alert**: Backlight blinks 3 times
- **Extended display**: 5-second alert message
- **Priority**: Alerts have highest priority

## 🚀 Usage Instructions

### 1. Hardware Setup
```cpp
// Connect LCD I2C to ESP32:
// VCC → 3.3V
// GND → GND  
// SDA → GPIO 21
// SCL → GPIO 22

// Optional: LCD navigation button
// Button → GPIO 26 (with pull-up resistor to 3.3V)
```

### 2. Software Integration
```cpp
#include "RDTRC_LCD_Library.h"

RDTRC_LCD systemLCD;

void setup() {
  // Initialize LCD with auto-detection
  systemLCD.begin();
  
  // Enable auto-scroll
  systemLCD.setAutoScroll(true, 4000);
}

void loop() {
  // Update LCD data
  systemLCD.updateStatus(
    "System Name",
    temperature,
    humidity, 
    moisture,
    growthPhase,
    wifiConnected,
    maintenanceMode,
    alertCount
  );
  
  // Update display
  systemLCD.update();
}
```

### 3. Library Functions

#### Initialization
```cpp
bool begin()                    // Auto-detect and initialize LCD
void scanI2C()                  // Scan and list all I2C devices
bool isLCDConnected()           // Check if LCD is connected
uint8_t getLCDAddress()         // Get detected LCD address
```

#### Display Control
```cpp
void updateStatus(...)          // Update status data
void displayPage(int page)      // Show specific page
void update()                   // Call in main loop
void setAutoScroll(bool, int)   // Enable/disable auto-scroll
```

#### Navigation
```cpp
void nextPage()                 // Go to next page
void prevPage()                 // Go to previous page
int getCurrentPage()            // Get current page number
void setCurrentPage(int)        // Set specific page
```

#### Messages
```cpp
void showDebug(line1, line2, time)     // Show debug message
void showAlert(message, time)          // Show alert with blink
void showMessage(line1, line2, time)   // Show custom message
```

#### Utility
```cpp
void clear()                    // Clear display
void setBacklight(bool)         // Control backlight
```

## 🔍 Debug Information Displayed

### System Status
- **System name** and **WiFi status**
- **Environmental data** (temperature, humidity)
- **Soil moisture** percentage
- **Growth phase** information
- **Alert count** and **system status**

### Real-time Feedback
- **Control actions** (misting, heating, fan, watering, lights)
- **Blynk commands** from mobile app
- **System events** (WiFi reconnect, OTA updates)
- **Error conditions** (sensor failures, low water, etc.)

### Network Information
- **WiFi connection** status and IP address
- **Hotspot mode** when WiFi fails
- **Blynk connectivity** status
- **Web server** status

## 📋 LCD Page Details

### Mushroom System Pages
1. **System Info**: Name, WiFi/Hotspot status
2. **Environment**: Temperature, humidity, soil moisture
3. **Growth**: Current growth phase
4. **Status**: Alerts, last action, system status

### Cilantro System Pages
1. **System Info**: Name, WiFi/Hotspot status
2. **Environment**: Temperature, humidity, soil moisture  
3. **Growth**: Current growth phase, light schedule
4. **Status**: Alerts, last action, system status

### Dual Zone System
- **Auto-alternates** between mushroom and cilantro data
- **Zone indicator**: "M:" for mushroom, "C:" for cilantro
- **Manual switch**: Button press changes zone immediately

## ⚙️ Configuration Options

### I2C Configuration
```cpp
#define I2C_SDA 21              // SDA pin (default)
#define I2C_SCL 22              // SCL pin (default)
#define LCD_NEXT_BUTTON_PIN 26  // Navigation button
```

### Timing Configuration
```cpp
scrollInterval = 4000;          // Auto-scroll timing (4 seconds)
zoneSwitchInterval = 6000;      // Dual-zone switch (6 seconds)
lastLCDUpdate = 2000;          // LCD update frequency (2 seconds)
```

### Display Customization
```cpp
// Modify in RDTRC_LCD_Library.h
totalPages = 4;                 // Number of pages
scrollInterval = 3000;          // Page scroll timing
```

## 🛠️ Troubleshooting

### LCD Not Detected
1. **Check wiring**: Verify SDA/SCL connections
2. **Check power**: Ensure 3.3V or 5V supply (depends on LCD module)
3. **I2C scan**: Monitor Serial output for detected devices
4. **Address conflict**: Try different I2C addresses manually

### Display Issues
1. **Contrast**: Adjust potentiometer on I2C backpack
2. **Backlight**: Check if backlight is enabled
3. **Character corruption**: Verify power supply stability
4. **Flickering**: Check for loose connections

### Common I2C Addresses
- **0x27** - Most common for PCF8574 backpack
- **0x3F** - Alternative PCF8574 address  
- **0x26, 0x20** - Other common addresses
- **Custom**: Can be set via jumpers on some modules

## 📈 Performance Impact

### Memory Usage
- **LCD Library**: ~8KB additional flash memory
- **RAM usage**: ~2KB for display buffers
- **Minimal impact**: <5% additional resource usage

### Timing
- **I2C communication**: ~2ms per display update
- **Auto-scroll**: Updates every 2-4 seconds
- **Non-blocking**: Doesn't interfere with main system operation

## 🔮 Advanced Features

### Custom Messages
```cpp
// Show temporary debug message
systemLCD.showDebug("Sensor Read", "DHT22 OK", 2000);

// Show alert with blinking
systemLCD.showAlert("LOW WATER", 5000);

// Show custom message
systemLCD.showMessage("Custom", "Message", 3000);
```

### Zone-Specific Display (Dual System)
```cpp
// Get current zone (0=mushroom, 1=cilantro)
int zone = dualLCD.getCurrentZone();

// Manually switch zone
dualLCD.switchZone();
```

### Backlight Control
```cpp
// Turn backlight off during night
systemLCD.setBacklight(false);

// Turn backlight on
systemLCD.setBacklight(true);
```

## 📚 Library Dependencies

### Required Libraries
```cpp
#include <Wire.h>               // I2C communication
#include <LiquidCrystal_I2C.h>  // LCD control
```

### Installation
1. **Arduino IDE**: Install "LiquidCrystal I2C" library by Frank de Brabander
2. **PlatformIO**: Add `marcoschwartz/LiquidCrystal_I2C@^1.1.2` to lib_deps

## 🎯 Benefits

### For Users
- **No computer needed** for basic system monitoring
- **Real-time feedback** on system operations
- **Quick diagnostics** when troubleshooting
- **Growth phase tracking** at a glance

### For Developers
- **Debug information** displayed locally
- **System status** without serial monitor
- **Network status** visible immediately
- **Error conditions** clearly indicated

### For Operations
- **Standalone monitoring** without network dependency
- **Immediate alerts** visible on display
- **Manual control feedback** for button presses
- **System health** monitoring

---

## 🔧 Quick Start

1. **Wire LCD I2C** to ESP32 (SDA→21, SCL→22, VCC→3.3V, GND→GND)
2. **Install library**: LiquidCrystal_I2C in Arduino IDE
3. **Upload code**: Choose appropriate system file
4. **Power on**: LCD will auto-detect and initialize
5. **Monitor**: Watch real-time data on 16x2 display

The LCD will automatically start displaying system information and cycle through different pages showing environmental data, growth phases, and system status!

**Created by**: RDTRC  
**Date**: 2024  
**Version**: 4.0
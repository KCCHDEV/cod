# LCD I2C 16x2 Installation Guide for RDTRC Growing Systems

## 🎯 Quick Start

1. **Connect LCD I2C to ESP32**
2. **Upload `i2c_lcd_scanner.ino`** to find your LCD address
3. **Upload your chosen system** (mushroom, cilantro, or combined)
4. **Enjoy real-time display!**

## 🔌 Hardware Wiring

### Standard Wiring (Most Common)
```
LCD I2C Module  →  ESP32 Pin
VCC             →  3.3V (or 5V if module requires it)
GND             →  GND
SDA             →  GPIO 21
SCL             →  GPIO 22
```

### Optional Navigation Button
```
Push Button     →  ESP32 Pin
One terminal    →  GPIO 26
Other terminal  →  GND
Pull-up         →  Built-in (INPUT_PULLUP)
```

### Visual Wiring Diagram
```
ESP32                    LCD I2C Module
┌─────────────┐         ┌──────────────┐
│         3.3V├─────────┤VCC           │
│          GND├─────────┤GND           │
│      GPIO 21├─────────┤SDA           │
│      GPIO 22├─────────┤SCL           │
│             │         │              │
│      GPIO 26├──[BTN]──┤              │
│             │    │    │              │
│          GND├────┘    │              │
└─────────────┘         └──────────────┘
```

## 📋 Step-by-Step Installation

### Step 1: Hardware Preparation
1. **Get components**:
   - ESP32 development board
   - LCD I2C 16x2 module (with I2C backpack)
   - Jumper wires (4 minimum, 6 with button)
   - Push button (optional, for manual navigation)
   - Breadboard or PCB (optional)

2. **Check LCD module**:
   - Verify it has I2C backpack (small PCB on back)
   - Look for potentiometer (contrast adjustment)
   - Note any jumpers for address selection

### Step 2: Wiring Connection
1. **Power connections**:
   ```
   LCD VCC → ESP32 3.3V (or 5V if module requires)
   LCD GND → ESP32 GND
   ```

2. **I2C connections**:
   ```
   LCD SDA → ESP32 GPIO 21
   LCD SCL → ESP32 GPIO 22
   ```

3. **Optional button** (for manual page navigation):
   ```
   Button → ESP32 GPIO 26 and GND
   ```

### Step 3: Software Setup
1. **Install required libraries** in Arduino IDE:
   - Search for "LiquidCrystal I2C" by Frank de Brabander
   - Install the library

2. **Upload scanner** to find LCD address:
   - Open `i2c_lcd_scanner.ino`
   - Upload to ESP32
   - Open Serial Monitor (115200 baud)
   - Note the detected LCD address (e.g., 0x27)

3. **Upload your chosen system**:
   - `mushroom_system_with_lcd.ino` - Mushroom only
   - `cilantro_system_with_lcd.ino` - Cilantro only
   - `combined_system_with_lcd.ino` - Both systems

### Step 4: Testing and Verification
1. **Power on system** and check Serial Monitor
2. **LCD should show**: Boot sequence and system info
3. **Verify auto-scroll**: Pages should change every 4 seconds
4. **Test button**: Press to manually navigate (if wired)
5. **Check web interface**: Should show LCD status

## 🔍 LCD Address Detection

### Automatic Detection Process
The system automatically scans these common addresses:
- **0x27** - Most common (PCF8574A)
- **0x3F** - Alternative (PCF8574)
- **0x26, 0x20** - Other variations
- **0x38-0x3E** - Extended range

### Manual Address Finding
If auto-detection fails:
1. **Upload scanner**: `i2c_lcd_scanner.ino`
2. **Check Serial Monitor**: Look for "LCD found at address: 0x??"
3. **Note the address**: Use this in your system code
4. **Verify with test**: LCD should show test message

### Common Address Issues
- **0x27**: Most PCF8574A modules
- **0x3F**: Most PCF8574 modules  
- **No detection**: Check wiring and power
- **Multiple devices**: Each will be listed separately

## 📱 LCD Display Information

### Page Layout (16x2 Characters)
```
┌────────────────┐
│System Name     │ ← Line 1 (16 chars max)
│Status Info     │ ← Line 2 (16 chars max)
└────────────────┘
```

### Display Pages

#### Single Zone Systems
**Page 0: System Info**
```
┌────────────────┐
│Mushroom System │
│WiFi: Connected │
└────────────────┘
```

**Page 1: Environmental**
```
┌────────────────┐
│T:22.1C H:85%   │
│Soil: 72%       │
└────────────────┘
```

**Page 2: Growth Phase**
```
┌────────────────┐
│Phase:          │
│Colonization    │
└────────────────┘
```

**Page 3: Status**
```
┌────────────────┐
│Status: OK      │
│Misting ON      │
└────────────────┘
```

#### Dual Zone System
**Mushroom Mode (6 seconds)**
```
┌────────────────┐
│Mushroom Zone   │
│T:22C M:70% OK  │
└────────────────┘
```

**Cilantro Mode (6 seconds)**
```
┌────────────────┐
│Cilantro Zone   │
│T:20C M:50% OK  │
└────────────────┘
```

## ⚙️ Configuration Options

### Timing Settings
```cpp
// In the main .ino file, you can adjust:
scrollInterval = 4000;      // Page scroll timing (4 seconds)
zoneSwitchInterval = 6000;  // Zone switch timing (6 seconds)
lastLCDUpdate = 2000;      // LCD update frequency (2 seconds)
```

### Display Customization
```cpp
// In RDTRC_LCD_Library.h:
totalPages = 4;            // Number of pages to cycle through
autoScroll = true;         // Enable automatic page scrolling
```

### I2C Pin Customization
```cpp
// If you need different pins:
#define I2C_SDA 18    // Change SDA pin
#define I2C_SCL 19    // Change SCL pin

// In setup():
Wire.begin(I2C_SDA, I2C_SCL);
```

## 🛠️ Troubleshooting

### LCD Not Detected
**Problem**: Scanner shows "No I2C devices found"

**Solutions**:
1. **Check power**: Verify VCC connection (3.3V or 5V)
2. **Check ground**: Ensure GND is connected
3. **Check data lines**: Verify SDA→21, SCL→22
4. **Check module**: Some modules need 5V instead of 3.3V
5. **Check backpack**: Ensure I2C backpack is properly soldered

### LCD Detected But Not Working
**Problem**: Device found but display doesn't work

**Solutions**:
1. **Adjust contrast**: Turn potentiometer on I2C backpack
2. **Check backlight**: Module might have backlight disabled
3. **Power supply**: Ensure stable power (some need 5V)
4. **Address jumpers**: Check if address jumpers are set correctly

### Display Corruption
**Problem**: Strange characters or flickering

**Solutions**:
1. **Power supply**: Use stable 3.3V or 5V
2. **Wiring**: Check for loose connections
3. **Interference**: Keep I2C wires short and away from power lines
4. **Pull-up resistors**: Ensure 4.7kΩ pull-ups on SDA/SCL (usually built-in)

### Address Conflicts
**Problem**: Multiple devices at same address

**Solutions**:
1. **Change address**: Use jumpers on I2C backpack
2. **Different modules**: Use modules with different default addresses
3. **Address selection**: Some modules have A0, A1, A2 jumpers

## 📊 Common LCD I2C Modules

### PCF8574 Based (Most Common)
- **Default addresses**: 0x27, 0x3F
- **Voltage**: 5V (some work with 3.3V)
- **Backlight**: Usually controllable
- **Contrast**: Potentiometer adjustment

### PCF8574A Based
- **Default addresses**: 0x3F, 0x27
- **Similar to PCF8574** but different address range
- **Address jumpers**: A0, A1, A2 for customization

### Module Identification
```cpp
// In scanner output, look for:
0x27 | Found | LCD I2C (PCF8574)  ← Use this address
0x3F | Found | LCD I2C (PCF8574)  ← Or this address
```

## 🔧 Advanced Configuration

### Custom I2C Pins
If GPIO 21/22 are used for other purposes:
```cpp
// Change in your main .ino file:
#define I2C_SDA 18
#define I2C_SCL 19

// In setupLCD():
Wire.begin(I2C_SDA, I2C_SCL);
```

### Multiple LCDs
For systems with multiple LCDs:
```cpp
// Different addresses for each LCD
LiquidCrystal_I2C lcd1(0x27, 16, 2);  // First LCD
LiquidCrystal_I2C lcd2(0x3F, 16, 2);  // Second LCD
```

### Custom Display Content
```cpp
// Modify in RDTRC_LCD_Library.h displayPage() function:
case 0: // Custom page
  lcd->setCursor(0, 0);
  lcd->print("Custom Line 1");
  lcd->setCursor(0, 1);
  lcd->print("Custom Line 2");
  break;
```

## 📚 Library Dependencies

### Arduino IDE Installation
1. **Open Arduino IDE**
2. **Go to**: Tools → Manage Libraries
3. **Search**: "LiquidCrystal I2C"
4. **Install**: "LiquidCrystal I2C" by Frank de Brabander
5. **Version**: 1.1.2 or newer

### PlatformIO Installation
Add to `platformio.ini`:
```ini
lib_deps = 
    marcoschwartz/LiquidCrystal_I2C@^1.1.2
```

### Manual Installation
If automatic installation fails:
1. **Download**: LiquidCrystal_I2C library from GitHub
2. **Extract**: To Arduino/libraries/ folder
3. **Restart**: Arduino IDE
4. **Verify**: Library appears in Include menu

## 🎮 Usage Examples

### Basic Status Display
```cpp
#include "RDTRC_LCD_Library.h"
RDTRC_LCD myLCD;

void setup() {
  myLCD.begin();  // Auto-detect and initialize
}

void loop() {
  myLCD.updateStatus("My System", 25.5, 60.0, 75, "Growing", true, false, 0);
  myLCD.update();
  delay(1000);
}
```

### Debug Messages
```cpp
// Show temporary debug info
myLCD.showDebug("Sensor Read", "Temperature OK");

// Show alert with blinking
myLCD.showAlert("LOW WATER");

// Show custom message
myLCD.showMessage("Custom", "Message", 3000);
```

### Manual Navigation
```cpp
// In button handler:
if (buttonPressed) {
  myLCD.nextPage();  // Go to next page
  myLCD.setAutoScroll(false);  // Disable auto-scroll
}
```

## 📈 Performance Notes

### Resource Usage
- **Flash memory**: +8KB for LCD library
- **RAM usage**: +2KB for display buffers
- **CPU impact**: <1% (I2C communication)
- **Update frequency**: Every 2 seconds (configurable)

### Optimization Tips
1. **Reduce updates**: Increase update interval if needed
2. **Disable auto-scroll**: For manual control only
3. **Shorter messages**: Keep text under 16 characters
4. **Minimal delays**: Use non-blocking display updates

## 🔍 Testing Checklist

### ✅ Hardware Test
- [ ] LCD powers on (backlight visible)
- [ ] Contrast adjustment works
- [ ] No loose connections
- [ ] Stable power supply

### ✅ Software Test  
- [ ] I2C scanner detects LCD
- [ ] LCD test displays correctly
- [ ] Auto-scroll works
- [ ] Button navigation works (if wired)
- [ ] Debug messages appear
- [ ] Alert blinking works

### ✅ Integration Test
- [ ] System boots with LCD
- [ ] Environmental data displays
- [ ] Control actions show on LCD
- [ ] Growth phase updates appear
- [ ] Alerts trigger LCD messages
- [ ] Web interface shows LCD status

## 🆘 Support

### Common Questions
**Q: LCD shows garbage characters**  
A: Adjust contrast potentiometer or check power supply

**Q: LCD detected but blank**  
A: Check backlight connection and contrast setting

**Q: Multiple addresses detected**  
A: Normal if you have multiple I2C devices - test each LCD address

**Q: System works without LCD**  
A: Yes! LCD is optional - system gracefully handles missing LCD

### Getting Help
1. **Check Serial Monitor**: Detailed debug information
2. **Run I2C Scanner**: Use provided scanner tool
3. **Check wiring**: Verify all connections
4. **Test with simple sketch**: Use basic LCD examples first

---

## 🎉 Success!

Once working, your LCD will show:
- **Real-time environmental data**
- **System status and alerts**  
- **Growth phase information**
- **Network connectivity status**
- **Control action feedback**

**No more guessing** - everything is visible at a glance on your 16x2 display!

**Created by**: RDTRC  
**Date**: 2024  
**Version**: 4.0
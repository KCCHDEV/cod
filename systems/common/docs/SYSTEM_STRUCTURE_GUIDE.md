# 📁 คู่มือโครงสร้างระบบใหม่
## New System Structure Guide

> **Firmware made by: RDTRC**  
> **Version: 2.0**  
> **Updated: 2024**

---

## 🎯 การปรับปรุงครั้งใหญ่

### ✨ สิ่งที่เปลี่ยนแปลง

1. **📁 โครงสร้างแบบ Modular**
   - แยกระบบแต่ละตัวเป็น folder เดียวกัน
   - มีโครงสร้างพื้นฐานร่วมกัน (Common Base)
   - จัดเก็บเอกสารและไฟล์แยกตามประเภท

2. **🖥️ Boot Screen มาตรฐาน**
   - ทุกระบบแสดง "FW make by RDTRC" เมื่อเริ่มต้น
   - แสดงเวอร์ชัน firmware และชื่อระบบ
   - Loading animation แบบเดียวกัน

3. **🔧 SystemBase Class**
   - คลาสพื้นฐานที่ใช้ร่วมกันทุกระบบ
   - จัดการ WiFi, RTC, LCD, LED อัตโนมัติ
   - API endpoints และ Web interface มาตรฐาน

4. **📱 Blynk Integration มาตรฐาน**
   - Virtual pins จัดระเบียบแบบเดียวกัน
   - การแจ้งเตือนแบบมาตรฐาน
   - Dashboard layout ที่สอดคล้องกัน

---

## 📂 โครงสร้าง Folder ใหม่

```
systems/
├── common/                    # ระบบพื้นฐานร่วม
│   ├── base_code/
│   │   ├── system_base.h      # Header file หลัก
│   │   └── system_base.cpp    # Implementation
│   ├── docs/
│   └── libraries/
│
├── cat_feeding/               # ระบบให้อาหารแมว
│   ├── code/
│   │   ├── cat_feeding_system.ino      # โค้ดใหม่ (ใช้ base)
│   │   └── cat_feeding_system_old.ino  # โค้ดเดิม
│   ├── docs/
│   │   ├── CAT_FEEDING_README_TH.md
│   │   └── CAT_FEEDING_SETUP_GUIDE_TH.md
│   ├── wiring/
│   │   └── wiring_diagram_cat_feeding.txt
│   └── libraries/
│       └── libraries_cat_feeding.txt
│
├── bird_feeding/              # ระบบให้อาหารนก
│   ├── code/
│   │   ├── bird_feeding_system.ino     # โค้ดใหม่
│   │   ├── bird_feeding_system_old.ino # โค้ดเดิม
│   │   ├── bird_feeding_quick_test.ino
│   │   └── bird_feeding_test.ino
│   ├── docs/
│   ├── wiring/
│   └── libraries/
│
└── tomato_watering/           # ระบบรดน้ำมะเขือเทศ
    ├── code/
    │   ├── tomato_watering_system.ino  # โค้ดใหม่
    │   ├── tomato_system_old.ino       # โค้ดเดิม
    │   ├── cilantro_watering_system.ino
    │   ├── rose_watering_system.ino
    │   └── mushroom_system.ino
    ├── docs/
    ├── wiring/
    └── libraries/
```

---

## 🔧 SystemBase Class อธิบาย

### 📋 คุณสมบัติหลัก

#### 1. Boot Sequence
```cpp
void showBootScreen() {
  // แสดง "FW make by RDTRC"
  // แสดงเวอร์ชัน firmware
  // แสดงชื่อระบบ
  // Loading animation
}
```

#### 2. Hardware Management
```cpp
bool initializeLCD();    // เริ่มต้น LCD
bool initializeRTC();    // เริ่มต้น RTC
bool connectWiFi();      // เชื่อมต่อ WiFi
void setLEDColor();      // ควบคุม RGB LED
void playBeep();         // เสียงแจ้งเตือน
```

#### 3. Status Management
```cpp
enum SystemState {
  SYSTEM_BOOT,      // กำลัง boot
  SYSTEM_INIT,      // กำลังเริ่มต้น
  SYSTEM_CONNECTING,// กำลังเชื่อมต่อ
  SYSTEM_READY,     // พร้อมใช้งาน
  SYSTEM_RUNNING,   // กำลังทำงาน
  SYSTEM_ERROR,     // มีข้อผิดพลาด
  SYSTEM_MAINTENANCE // อยู่ในโหมดบำรุงรักษา
};
```

#### 4. Web Server
```cpp
void setupWebServer();   // ตั้งค่า web server
void handleRoot();       // หน้าหลัก
void handleSystemInfo(); // ข้อมูลระบบ
void setupCommonAPI();   // API endpoints ร่วม
```

### 🔄 Virtual Functions

แต่ละระบบต้อง override functions เหล่านี้:

```cpp
class MySystem : public SystemBase {
public:
  MySystem() : SystemBase("My System Name") {}
  
  // ฟังก์ชันที่ต้อง implement
  void setupSystem() override {
    // ตั้งค่าเฉพาะระบบ (pins, sensors, actuators)
  }
  
  void runSystem() override {
    // ลูปการทำงานหลักของระบบ
  }
  
  // ฟังก์ชันเสริม (optional)
  String getSystemStatus() override {
    // ส่งคืนสถานะเฉพาะระบบสำหรับแสดงบน LCD
    return "Custom Status";
  }
  
  void handleSystemCommand(const String& command) override {
    // จัดการคำสั่งเฉพาะระบบ
    if (command == "my_action") {
      doMyAction();
    }
  }
};
```

---

## 🎨 Boot Screen Design

### 📺 ลำดับการแสดงผล

1. **Screen 1: Firmware Info (1.5 วินาที)**
   ```
   FW make by RDTRC
   Version 2.0
   ```

2. **Screen 2: System Name (1.5 วินาที)**
   ```
   [System Name]
   Initializing...
   ```

3. **Screen 3: Loading Animation (1.6 วินาที)**
   ```
   [System Name]
   ................
   ```

4. **Screen 4: Ready Status (2 วินาที)**
   ```
   [System Name]
   Ready - v2.0
   ```

### 🔊 Sound Effects

- **Boot Start**: 800Hz, 200ms
- **System Name**: 1000Hz, 200ms  
- **Loading Complete**: 1200Hz, 200ms
- **Startup Melody**: 8-note ascending scale

---

## 🔌 Pin Configuration มาตรฐาน

### 📍 Pins ร่วมทุกระบบ

```cpp
// I2C สำหรับ LCD และ RTC
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

// Status LEDs
#define LED_RED_PIN 25
#define LED_GREEN_PIN 26  
#define LED_BLUE_PIN 27

// Buzzer
#define BUZZER_PIN 23

// LCD Address (ลองใช้ 0x27 หรือ 0x3F)
#define LCD_ADDRESS 0x27
```

### 📍 Pins เฉพาะระบบ

**Cat Feeding:**
```cpp
#define SERVO_PIN 18
#define LOADCELL_DOUT_PIN 19
#define LOADCELL_SCK_PIN 20
#define PIR_PIN 4
#define BUTTON_MANUAL_PIN 32
#define BUTTON_RESET_PIN 33
```

**Bird Feeding:**
```cpp
#define SERVO_FOOD_PIN 18
#define SERVO_WATER_PIN 19
#define PUMP_PIN 5
#define ULTRASONIC_TRIG_PIN 16
#define ULTRASONIC_ECHO_PIN 17
#define WATER_LEVEL_PIN 34
#define PIR_PIN 4
```

**Tomato Watering:**
```cpp
#define PUMP_PIN 5
#define VALVE_PIN 18
#define SOIL_MOISTURE_PIN 34
#define WATER_LEVEL_PIN 35
#define DHT_PIN 4
#define FLOW_SENSOR_PIN 2
```

---

## 🔄 การ Migrate จากระบบเดิม

### ขั้นตอนการย้าย

1. **สำรองข้อมูลเดิม**
   ```bash
   # ไฟล์เดิมถูกเปลี่ยนชื่อเป็น *_old.ino
   # การตั้งค่าจะถูกโหลดจาก EEPROM อัตโนมัติ
   ```

2. **อัพเดทการตั้งค่า**
   ```cpp
   // ไฟล์ใหม่ใช้โครงสร้างเดียวกัน
   // แต่มี SystemBase Class เพิ่มเติม
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   const char* blynk_token = "YOUR_BLYNK_TOKEN";
   ```

3. **ทดสอบระบบใหม่**
   ```
   ✅ Boot screen แสดงถูกต้อง
   ✅ เชื่อมต่อ WiFi ได้
   ✅ Web interface ทำงานได้
   ✅ Blynk app เชื่อมต่อได้
   ✅ ฟีเจอร์หลักทำงานปกติ
   ```

### 🔄 การเปรียบเทียบ

| ฟีเจอร์ | ระบบเดิม | ระบบใหม่ |
|---------|-----------|-----------|
| **Boot Screen** | ❌ ไม่มี | ✅ มี "FW make by RDTRC" |
| **Error Handling** | ⚠️ พื้นฐาน | ✅ ครบครัน |
| **Web Interface** | ✅ มี | ✅ ปรับปรุงแล้ว |
| **Code Structure** | ❌ แยกกัน | ✅ ใช้ Base Class |
| **API Endpoints** | ⚠️ จำกัด | ✅ ครบครัน |
| **Memory Management** | ❌ ไม่มี | ✅ มีการตรวจสอบ |
| **Status LED** | ⚠️ พื้นฐาน | ✅ สีและรูปแบบมาตรฐาน |

---

## 🚀 การใช้งานระบบใหม่

### 🔧 สำหรับผู้พัฒนา

1. **สร้างระบบใหม่**
   ```cpp
   // 1. Include base system
   #include "../../common/base_code/system_base.h"
   
   // 2. Create class ที่ inherit จาก SystemBase
   class MySystem : public SystemBase {
     // Implementation
   };
   
   // 3. สร้าง instance และใช้งาน
   MySystem mySystem;
   
   void setup() {
     mySystem.begin();
   }
   
   void loop() {
     mySystem.loop();
   }
   ```

2. **เพิ่มฟีเจอร์ใหม่**
   ```cpp
   void setupSystem() override {
     // ตั้งค่า pins และ sensors
     pinMode(MY_SENSOR_PIN, INPUT);
     
     // เรียก base functions
     LOG_INFO("My system setup complete");
   }
   
   void runSystem() override {
     // ลูปการทำงานหลัก
     checkMySensors();
     handleMyActions();
   }
   ```

### 👤 สำหรับผู้ใช้งาน

1. **เลือกระบบที่ต้องการ**
   ```
   systems/cat_feeding/     → ระบบให้อาหารแมว
   systems/bird_feeding/    → ระบบให้อาหารนก
   systems/tomato_watering/ → ระบบรดน้ำมะเขือเทศ
   ```

2. **เปิดไฟล์ในระบบที่เลือก**
   ```
   เข้าไปใน folder code/
   เปิดไฟล์ *_system.ino (ไม่ใช่ *_old.ino)
   ```

3. **แก้ไขการตั้งค่าเพียงไม่กี่บรรทัด**
   ```cpp
   // WiFi Settings (เหมือนเดิม)
   const char* ssid = "ชื่อ_WiFi_ของคุณ";
   const char* password = "รหัสผ่าน_WiFi";
   const char* blynk_token = "Token_จาก_Blynk";
   ```

4. **อัพโหลดและใช้งาน**
   - ระบบจะแสดง boot screen "FW make by RDTRC"
   - ทำงานเหมือนเดิมแต่มีฟีเจอร์เพิ่มเติม

---

## 🔍 การเปรียบเทียบโค้ดเดิมกับใหม่

### 📜 โค้ดเดิม (Old Structure)
```cpp
// ทุกอย่างอยู่ในไฟล์เดียว
#include <WiFi.h>
#include <WebServer.h>
// ... includes อื่นๆ

// ตัวแปร global ทั้งหมด
WiFiClient client;
WebServer server(80);
// ... variables

void setup() {
  // ตั้งค่าทุกอย่างใน setup()
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  // ... setup code
}

void loop() {
  // ทุกอย่างใน loop()
  server.handleClient();
  checkSensors();
  // ... loop code
}
```

### ✨ โค้ดใหม่ (New Structure)
```cpp
// ใช้ base system
#include "../../common/base_code/system_base.h"

// สร้าง class เฉพาะระบบ
class MySystem : public SystemBase {
public:
  MySystem() : SystemBase("My System") {}
  
  void setupSystem() override {
    // ตั้งค่าเฉพาะระบบเท่านั้น
  }
  
  void runSystem() override {
    // ลูปการทำงานเฉพาะระบบ
  }
};

MySystem mySystem;

void setup() {
  mySystem.begin(); // ทำทุกอย่างอัตโนมัติ
}

void loop() {
  mySystem.loop(); // จัดการทุกอย่างอัตโนมัติ
}
```

---

## 🎯 ประโยชน์ของโครงสร้างใหม่

### 👨‍💻 สำหรับผู้พัฒนา

1. **Code Reuse** - ไม่ต้องเขียนโค้ดพื้นฐานซ้ำ
2. **Consistency** - ทุกระบบทำงานแบบเดียวกัน  
3. **Maintainability** - แก้ไขง่าย อัพเดทง่าย
4. **Scalability** - เพิ่มระบบใหม่ได้ง่าย
5. **Error Handling** - จัดการข้อผิดพลาดแบบมาตรฐาน

### 👤 สำหรับผู้ใช้งาน

1. **User Experience** - ใช้งานคล้ายกันทุกระบบ
2. **Boot Screen** - ดูโปรเฟสชันนัลมากขึ้น
3. **Web Interface** - หน้าตาและการใช้งานเหมือนกัน
4. **Mobile App** - Blynk layout สอดคล้องกัน
5. **Troubleshooting** - วิธีแก้ปัญหาใช้ได้กับทุกระบบ

### 🔧 สำหรับการบำรุงรักษา

1. **Centralized Updates** - อัพเดท base system ครั้งเดียว
2. **Standardized Logs** - Log format เหมือนกันทุกระบบ
3. **Common APIs** - API structure เดียวกัน
4. **Unified Documentation** - เอกสารมีโครงสร้างเดียวกัน

---

## 📱 การใช้งาน Web Interface

### 🌐 หน้าหลักมาตรฐาน

ทุกระบบจะมีหน้าเว็บที่มีโครงสร้างเดียวกัน:

```html
<!DOCTYPE html>
<html>
<head>
    <title>RDTRC IoT System</title>
    <!-- Consistent styling -->
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🤖 RDTRC IoT System</h1>
            <p>Firmware made by RDTRC - Version 2.0</p>
        </div>
        
        <div class="status-card">
            <!-- System status -->
        </div>
        
        <div class="control-panel">
            <!-- System controls -->
        </div>
    </div>
</body>
</html>
```

### 🔧 API Endpoints มาตรฐาน

**Common APIs (ทุกระบบ):**
```
GET  /                 → หน้าหลัก
GET  /info            → ข้อมูลระบบ JSON
GET  /api/status      → สถานะระบบ
GET  /api/config      → การตั้งค่า
POST /api/restart     → รีสตาร์ท
POST /api/reset       → รีเซ็ต
```

**System-specific APIs:**
```
GET  /api/[system]/status     → สถานะเฉพาะระบบ
POST /api/[system]/action    → สั่งการเฉพาะระบบ
GET  /api/[system]/schedule  → ตารางเวลา
POST /api/[system]/schedule  → อัพเดทตารางเวลา
```

---

## 🔧 คำแนะนำการพัฒนา

### 🎯 Best Practices

1. **ใช้ LOG macros**
   ```cpp
   LOG_INFO("Information message");
   LOG_ERROR("Error message");
   LOG_DEBUG("Debug message");
   ```

2. **ตรวจสอบ Memory**
   ```cpp
   CHECK_MEMORY(); // ใช้ macro นี้เป็นระยะ
   ```

3. **จัดการ State อย่างถูกต้อง**
   ```cpp
   setState(SYSTEM_RUNNING);
   setError(ERROR_SENSOR);
   clearError();
   ```

4. **ใช้ Virtual Functions**
   ```cpp
   // Override เฉพาะที่จำเป็น
   void setupSystem() override { /* required */ }
   void runSystem() override { /* required */ }
   String getSystemStatus() override { /* optional */ }
   ```

### ⚠️ ข้อควรระวัง

1. **Memory Usage**
   - ESP32 มี RAM จำกัด (~320KB)
   - ใช้ `String` อย่างระวัง
   - เลี่ยงการสร้าง object ใหญ่ใน loop()

2. **Timing Issues**
   - ใช้ `millis()` แทน `delay()` ในลูปหลัก
   - ระวัง blocking operations
   - ตั้งค่า timeout สำหรับ network operations

3. **Pin Conflicts**
   - ตรวจสอบ pin assignment ไม่ซ้ำกัน
   - ระวัง pins ที่ใช้สำหรับ boot/flash
   - ใช้ pullup/pulldown ตามความเหมาะสม

---

## 📈 Performance Optimization

### 💾 Memory Management

```cpp
// ตรวจสอบ memory usage
void printMemoryInfo() {
  Serial.println("Free Heap: " + String(ESP.getFreeHeap()));
  Serial.println("Min Free Heap: " + String(ESP.getMinFreeHeap()));
}

// ใช้ใน development mode
#define DEBUG_MEMORY 1
#if DEBUG_MEMORY
  CHECK_MEMORY();
#endif
```

### ⚡ CPU Optimization

```cpp
// ใช้ timer แทน delay
unsigned long lastCheck = 0;
if (millis() - lastCheck > 5000) {
  checkSensors();
  lastCheck = millis();
}

// ใช้ interrupt สำหรับ time-critical tasks
attachInterrupt(digitalPinToInterrupt(PIN), ISR, RISING);
```

### 🌐 Network Optimization

```cpp
// Batch Blynk updates
static unsigned long lastBlynkUpdate = 0;
if (millis() - lastBlynkUpdate > 10000) {
  // Update multiple values at once
  Blynk.virtualWrite(V0, value1);
  Blynk.virtualWrite(V1, value2);
  lastBlynkUpdate = millis();
}
```

---

<div align="center">

## 🎉 ระบบใหม่พร้อมใช้งาน!

**โครงสร้างที่เป็นระเบียบ | Boot Screen ที่สวยงาม | ฟีเจอร์ครบครัน**

**Firmware made by RDTRC - Professional IoT Solutions**

[![Version](https://img.shields.io/badge/Version-2.0-blue.svg)](https://github.com)
[![Status](https://img.shields.io/badge/Status-Ready-green.svg)](https://github.com)
[![Support](https://img.shields.io/badge/Support-Active-orange.svg)](https://github.com)

</div>
# 🐦 ระบบให้อาหารนกอัตโนมัติ - เอกสารสมบูรณ์
## RDTRC Bird Feeding System - Complete Documentation

> **Firmware สร้างโดย: RDTRC**  
> **เวอร์ชัน: 4.0 - Enhanced System with LCD Support**  
> **อัพเดท: 2024**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)](https://www.espressif.com/)
[![Blynk](https://img.shields.io/badge/Blynk-IoT-orange.svg)](https://blynk.io/)

---

## 📋 สารบัญ

- [ภาพรวมระบบ](#ภาพรวมระบบ)
- [คุณสมบัติเด่น](#คุณสมบัติเด่น)
- [ข้อมูลทางเทคนิค](#ข้อมูลทางเทคนิค)
- [รายการอุปกรณ์](#รายการอุปกรณ์)
- [การเชื่อมต่อวงจร](#การเชื่อมต่อวงจร)
- [การติดตั้งซอฟต์แวร์](#การติดตั้งซอฟต์แวร์)
- [การตั้งค่าระบบ](#การตั้งค่าระบบ)
- [การใช้งาน](#การใช้งาน)
- [Web Interface](#web-interface)
- [Blynk App Integration](#blynk-app-integration)
- [API Reference](#api-reference)
- [การแก้ปัญหา](#การแก้ปัญหา)
- [การบำรุงรักษา](#การบำรุงรักษา)
- [การพัฒนาต่อ](#การพัฒนาต่อ)

---

## 🎯 ภาพรวมระบบ

ระบบให้อาหารนกอัตโนมัติ RDTRC เป็นระบบ IoT ที่ออกแบบมาเฉพาะสำหรับการดูแลนกป่าและนกเลี้ยง ด้วยการตรวจจับแสงธรรมชาติและการนับจำนวนนกที่มาเยี่ยม

### 🏗️ สถาปัตยกรรมระบบ

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Mobile App    │    │  Web Interface  │    │  Blynk Cloud    │
│   (Blynk)       │    │   (Browser)     │    │   Platform      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         └───────────────────────┼───────────────────────┘
                                 │
         ┌───────────────────────▼───────────────────────┐
         │              ESP32 Controller                 │
         │     (WiFi + Weather Resistant)                │
         └───────────────────────┬───────────────────────┘
                                 │
    ┌────────────────────────────┼────────────────────────────┐
    │                            │                            │
    ▼                            ▼                            ▼
┌─────────┐              ┌─────────────┐              ┌─────────────┐
│Sensors  │              │ Actuators   │              │ Display &   │
│- HX711  │              │ - Servo     │              │ Interface   │
│- PIR    │              │ - Buzzer    │              │ - LCD 16x2  │
│- Light  │              │ - LED       │              │ - Buttons   │
└─────────┘              └─────────────┘              └─────────────┘
```

### 🦆 การออกแบบเฉพาะสำหรับนก

- **ขนาดที่เหมาะสม**: ปริมาณอาหาร 5-50 กรัม
- **ตรวจจับแสง**: ให้อาหารเฉพาะเวลากลางวัน
- **การนับนก**: ติดตามจำนวนนกที่มาเยี่ยม
- **ทนทานต่อสภาพอากาศ**: ออกแบบสำหรับติดตั้งกลางแจ้ง

---

## 🌟 คุณสมบัติเด่น

### ⏰ การให้อาหารตามธรรมชาติ
- **ตรวจจับแสงธรรมชาติ**: ให้อาหารเฉพาะเวลากลางวัน
- **ตารางเวลาที่ยืดหยุ่น**: ตั้งได้สูงสุด 4 เวลาต่อวัน
- **ปริมาณเหมาะสำหรับนก**: 5-50 กรัมต่อครั้ง
- **ป้องกันการให้ซ้ำ**: ระบบป้องกันการให้อาหารติดต่อกัน

### 🐦 เฉพาะสำหรับนก
- **การนับนกที่มาเยี่ยม**: ติดตามจำนวนนกที่มากิน
- **ตรวจจับขนาดนก**: แยกแยะนกเล็ก-ใหญ่
- **ปรับปริมาณตามนก**: ให้อาหารตามขนาดนก
- **สถิติการเยี่ยม**: บันทึกเวลาและความถี่

### 📱 ควบคุมผ่านมือถือ
- **Blynk IoT Platform**: ควบคุมและติดตามผ่านแอปมือถือ
- **Web Interface**: เข้าถึงผ่านเบราว์เซอร์
- **แจ้งเตือนแบบเรียลไทม์**: แจ้งเมื่อให้อาหาร ตรวจพบนก หรืออาหารหมด
- **การตั้งค่าระยะไกล**: เปลี่ยนตารางเวลาและปริมาณได้ทุกที่

### 🔍 เซ็นเซอร์อัจฉริยะ
- **วัดน้ำหนักอาหาร**: เซ็นเซอร์ Load Cell ความแม่นยำสูง
- **ตรวจจับนก**: PIR Sensor ตรวจจับการเคลื่อนไหว
- **เซ็นเซอร์แสง**: ตรวจจับแสงธรรมชาติ
- **แสดงผลข้อมูล**: LCD 16x2 แสดงสถานะปัจจุบัน

### 🌦️ ทนทานต่อสภาพอากาศ
- **การป้องกันน้ำ**: IP65 Water Resistant
- **ทนต่ออุณหภูมิ**: -10°C ถึง 60°C
- **การระบายอากาศ**: ป้องกันความชื้น
- **วัสดุทนทาน**: UV Resistant Plastic

---

## 🔧 ข้อมูลทางเทคนิค

### 📊 ข้อกำหนดระบบ

| ข้อมูล | รายละเอียด |
|--------|------------|
| **ไมโครคอนโทรลเลอร์** | ESP32 Development Board |
| **แรงดันไฟฟ้า** | 5V DC / 2A หรือ 12V DC / 1A |
| **การเชื่อมต่อ** | WiFi 802.11 b/g/n |
| **ความจุหน่วยความจำ** | 4MB Flash, 320KB RAM |
| **ความแม่นยำการชั่ง** | ±0.5 กรัม |
| **ขนาดปริมาณอาหาร** | 5-50 กรัม |
| **ระยะตรวจจับนก** | 0-5 เมตร |
| **เซ็นเซอร์แสง** | 0-4095 ADC (12-bit) |
| **อุณหภูมิการทำงาน** | -10°C ถึง 60°C |
| **ความชื้นสัมพัทธ์** | 10-95% |
| **ระดับการป้องกัน** | IP65 (กลางแจ้ง) |

### 🎛️ การกำหนดค่า Pin

```cpp
// Pin Definitions for Bird Feeding System
#define SERVO_PIN 18                    // เซอร์โวมอเตอร์
#define LOAD_CELL_DOUT_PIN 19          // HX711 Data
#define LOAD_CELL_SCK_PIN 5            // HX711 Clock
#define PIR_SENSOR_PIN 23              // PIR Motion Sensor (Bird Detection)
#define LIGHT_SENSOR_PIN 32            // Light Sensor (Day/Night)
#define BUZZER_PIN 4                   // Buzzer
#define STATUS_LED_PIN 2               // Status LED
#define RESET_BUTTON_PIN 0             // Reset Button
#define MANUAL_FEED_BUTTON_PIN 27      // Manual Feed Button
#define LCD_NEXT_BUTTON_PIN 26         // LCD Navigation Button

// I2C Pins for LCD
#define I2C_SDA 21                     // I2C Data Line
#define I2C_SCL 22                     // I2C Clock Line

// Light Sensor Thresholds
#define DAYLIGHT_THRESHOLD 500         // ADC value for daylight
#define NIGHT_THRESHOLD 100            // ADC value for night
```

---

## 🛍️ รายการอุปกรณ์

### 📋 อุปกรณ์หลัก

| อุปกรณ์ | จำนวน | ราคาประมาณ | รายละเอียด |
|---------|--------|-------------|------------|
| **ESP32 Development Board** | 1 | ฿350-500 | ตัวควบคุมหลัก |
| **HX711 Load Cell Amplifier** | 1 | ฿120-180 | วงจรขยายสัญญาณเซ็นเซอร์ชั่งน้ำหนัก |
| **Load Cell 2kg** | 1 | ฿180-250 | เซ็นเซอร์ชั่งน้ำหนัก (เหมาะสำหรับนก) |
| **SG90 Servo Motor** | 1 | ฿80-120 | มอเตอร์ควบคุมการให้อาหาร |
| **PIR Motion Sensor** | 1 | ฿60-100 | เซ็นเซอร์ตรวจจับการเคลื่อนไหวนก |
| **LDR Light Sensor** | 1 | ฿30-50 | เซ็นเซอร์ตรวจจับแสง |
| **LCD 16x2 I2C** | 1 | ฿120-180 | หน้าจอแสดงผล |
| **Buzzer Module** | 1 | ฿30-50 | เสียงแจ้งเตือน |
| **LED Module** | 1 | ฿20-40 | ไฟแสดงสถานะ |
| **Push Button** | 3 | ฿30-60 | ปุ่มควบคุม |

### 🌦️ อุปกรณ์กันน้ำ

| อุปกรณ์ | จำนวน | ราคาประมาณ | รายละเอียด |
|---------|--------|-------------|------------|
| **Waterproof Enclosure** | 1 | ฿300-500 | กล่องกันน้ำ IP65 |
| **Cable Glands** | 5 | ฿50-100 | ตัวกันน้ำสายไฟ |
| **Silicone Sealant** | 1 | ฿40-80 | ซิลิโคนกันน้ำ |
| **Drainage Holes** | - | ฿20-40 | ช่องระบายน้ำ |

### 🔌 อุปกรณ์เสริม

| อุปกรณ์ | จำนวน | ราคาประมาณ | รายละเอียด |
|---------|--------|-------------|------------|
| **Solar Panel 5W** | 1 | ฿400-600 | แผงโซลาร์เซลล์ (ตัวเลือก) |
| **Battery 18650** | 2 | ฿200-300 | แบตเตอรี่สำรอง |
| **Power Supply 12V/1A** | 1 | ฿200-350 | แหล่งจ่ายไฟ |
| **Jumper Wires** | 1 ชุด | ฿50-100 | สายจัมเปอร์ |
| **Resistors (10kΩ)** | 3 | ฿10-20 | ตัวต้านทาน |
| **Capacitors (100μF)** | 2 | ฿20-40 | ตัวเก็บประจุ |

### 💰 สรุปค่าใช้จ่าย

| ประเภท | ราคา (ต่ำสุด) | ราคา (สูงสุด) |
|--------|---------------|---------------|
| **อุปกรณ์หลัก** | ฿1,020 | ฿1,530 |
| **อุปกรณ์กันน้ำ** | ฿410 | ฿720 |
| **อุปกรณ์เสริม** | ฿880 | ฿1,410 |
| **รวม** | **฿2,310** | **฿3,660** |

---

## 🔌 การเชื่อมต่อวงจร

### 📐 Wiring Diagram

```
ESP32 Pin    │ Component           │ Description
─────────────┼─────────────────────┼─────────────────────
3.3V         │ LCD VCC             │ Power for LCD I2C
GND          │ LCD GND             │ Ground for LCD
GPIO21       │ LCD SDA             │ I2C Data Line
GPIO22       │ LCD SCL             │ I2C Clock Line
             │                     │
GPIO18       │ Servo Signal        │ Servo Motor Control
5V           │ Servo VCC           │ Power for Servo
GND          │ Servo GND           │ Ground for Servo
             │                     │
GPIO19       │ HX711 DT            │ Load Cell Data
GPIO5        │ HX711 SCK           │ Load Cell Clock
5V           │ HX711 VCC           │ Power for HX711
GND          │ HX711 GND           │ Ground for HX711
             │                     │
GPIO23       │ PIR Signal          │ Bird Motion Detection
5V           │ PIR VCC             │ Power for PIR
GND          │ PIR GND             │ Ground for PIR
             │                     │
GPIO32       │ LDR Analog          │ Light Sensor Input
3.3V         │ LDR VCC (via 10kΩ)  │ Pull-up for LDR
GND          │ LDR GND             │ Ground for LDR
             │                     │
GPIO4        │ Buzzer +            │ Buzzer Positive
GND          │ Buzzer -            │ Buzzer Negative
             │                     │
GPIO2        │ LED +               │ Status LED Positive
GND          │ LED - (220Ω)        │ LED Negative with resistor
             │                     │
GPIO0        │ Reset Button        │ System Reset
GPIO27       │ Manual Feed         │ Manual Feed Button
GPIO26       │ LCD Next            │ LCD Navigation Button
```

### ⚡ การต่อไฟฟ้าแบบกลางแจ้ง

```
Outdoor Power Setup:
┌─────────────────┐
│ Solar Panel 5W  │
└─────┬───────────┘
      │
┌─────▼───────────┐
│ Charge Controller│
│ + Battery Pack  │
└─────┬───────────┘
      │
┌─────▼───────────┐
│ DC-DC Converter │
│ 12V → 5V/3.3V   │
└─────┬───────────┘
      │
      ├── ESP32 (3.3V)
      ├── Servo Motor (5V)
      ├── HX711 Module (5V)
      ├── PIR Sensor (5V)
      └── LCD Module (3.3V)
```

### 🌦️ การป้องกันน้ำ

```
Waterproofing Layout:

┌─────────────────────────────────────┐
│ IP65 Waterproof Enclosure          │
│                                     │
│  ┌─────────┐    ┌──────────────┐    │
│  │ ESP32   │    │ Power Supply │    │
│  │ Board   │    │ Module       │    │
│  └─────────┘    └──────────────┘    │
│                                     │
│  ┌─────────┐    ┌──────────────┐    │
│  │ HX711   │    │ Relay Board  │    │
│  │ Module  │    │ (if needed)  │    │
│  └─────────┘    └──────────────┘    │
│                                     │
│  Cable Glands for external sensors │
└─────────────────────────────────────┘
        │         │         │
        ▼         ▼         ▼
   PIR Sensor  Servo    Load Cell
   (Outdoor)  (Outdoor)  (Outdoor)
```

---

## 💻 การติดตั้งซอฟต์แวร์

### 🔧 Arduino IDE Setup

1. **ติดตั้ง Arduino IDE**
   ```
   ดาวน์โหลดจาก: https://www.arduino.cc/en/software
   เวอร์ชันแนะนำ: 2.0 หรือใหม่กว่า
   ```

2. **เพิ่ม ESP32 Board Manager**
   ```
   File → Preferences → Additional Board Manager URLs
   เพิ่ม: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```

3. **ติดตั้ง ESP32 Board**
   ```
   Tools → Board → Boards Manager
   ค้นหา "ESP32" โดย Espressif Systems
   ติดตั้งเวอร์ชันล่าสุด
   ```

4. **เลือก Board และ Port**
   ```
   Tools → Board → ESP32 Arduino → ESP32 Dev Module
   Tools → Port → เลือกพอร์ตที่ ESP32 เชื่อมต่อ
   ```

### 📚 ไลบรารีที่จำเป็น

```cpp
// Core Libraries (Built-in)
#include <WiFi.h>              // WiFi connectivity
#include <WebServer.h>         // Web server functionality
#include <SPIFFS.h>            // File system
#include <Wire.h>              // I2C communication
#include <HTTPClient.h>        // HTTP client
#include <ArduinoOTA.h>        // Over-the-air updates
#include <ESPmDNS.h>           // mDNS service

// External Libraries (Install via Library Manager)
#include <ArduinoJson.h>       // JSON handling
#include <HX711.h>             // Load cell amplifier
#include <ESP32Servo.h>        // Servo motor control
#include <LiquidCrystal_I2C.h> // LCD I2C display
#include <BlynkSimpleEsp32.h>  // Blynk IoT platform
#include <NTPClient.h>         // Network time protocol
#include <WiFiUdp.h>           // UDP communication

// Custom Libraries (Included in project)
#include "RDTRC_LCD_Library.h" // Custom LCD functions
```

### 📦 การติดตั้งไลบรารี

```bash
Arduino IDE → Tools → Manage Libraries

ติดตั้งไลบรารีต่อไปนี้:

1. ArduinoJson (version 6.21.0 หรือใหม่กว่า)
   - ผู้พัฒนา: Benoit Blanchon
   - ใช้สำหรับ: JSON data handling

2. HX711 Arduino Library (version 0.7.5 หรือใหม่กว่า)
   - ผู้พัฒนา: Rob Tillaart
   - ใช้สำหรับ: Load cell weight sensor

3. ESP32Servo (version 0.13.0 หรือใหม่กว่า)
   - ผู้พัฒนา: Kevin Harrington
   - ใช้สำหรับ: Servo motor control

4. LiquidCrystal I2C (version 1.1.2 หรือใหม่กว่า)
   - ผู้พัฒนา: Frank de Brabander
   - ใช้สำหรับ: I2C LCD display

5. Blynk (version 1.3.2 หรือใหม่กว่า)
   - ผู้พัฒนา: Volodymyr Shymanskyy
   - ใช้สำหรับ: IoT platform integration

6. NTPClient (version 3.2.1 หรือใหม่กว่า)
   - ผู้พัฒนา: Fabrice Weinberg
   - ใช้สำหรับ: Network time synchronization
```

### 🔧 การคอมไพล์และอัพโหลด

```cpp
// Board Settings
Board: "ESP32 Dev Module"
Upload Speed: "921600"
CPU Frequency: "240MHz (WiFi/BT)"
Flash Frequency: "80MHz"
Flash Mode: "QIO"
Flash Size: "4MB (32Mb)"
Partition Scheme: "Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)"
Core Debug Level: "None"
PSRAM: "Disabled"
```

---

## ⚙️ การตั้งค่าระบบ

### 🌐 การตั้งค่า WiFi

```cpp
// Network Configuration
const char* ssid = "YOUR_WIFI_SSID";           // ชื่อ WiFi ของคุณ
const char* password = "YOUR_WIFI_PASSWORD";   // รหัสผ่าน WiFi
const char* hotspot_ssid = "RDTRC_BirdFeeder"; // ชื่อ Hotspot
const char* hotspot_password = "rdtrc123";     // รหัสผ่าน Hotspot

// WiFi Connection Settings
#define WIFI_TIMEOUT 30000        // 30 seconds timeout
#define WIFI_RETRY_INTERVAL 5000  // 5 seconds retry interval
#define MAX_WIFI_RETRIES 3        // Maximum retry attempts
```

### 📱 การตั้งค่า Blynk

```cpp
// Blynk Configuration
#define BLYNK_TEMPLATE_ID "TMPL61Zdwsx9r"
#define BLYNK_TEMPLATE_NAME "Bird_Feeding_System"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN_HERE"

// Blynk Virtual Pins
#define BLYNK_VPIN_STATUS 0       // System status LED
#define BLYNK_VPIN_FOOD_LEVEL 1   // Food level gauge
#define BLYNK_VPIN_BIRD_COUNT 2   // Bird visit counter
#define BLYNK_VPIN_LIGHT_LEVEL 3  // Light sensor value
#define BLYNK_VPIN_MANUAL_FEED 4  // Manual feed button
#define BLYNK_VPIN_PORTION_SIZE 5 // Portion size slider
#define BLYNK_VPIN_SCHEDULE 6     // Feeding schedule
#define BLYNK_VPIN_MESSAGES 7     // System messages terminal
```

### 🔔 การตั้งค่า LINE Notify

```cpp
// LINE Notify Configuration
const char* lineToken = "YOUR_LINE_NOTIFY_TOKEN";
const char* lineAPI = "https://notify-api.line.me/api/notify";

// Notification Settings
bool notifyFeeding = true;        // แจ้งเตือนเมื่อให้อาหาร
bool notifyLowFood = true;        // แจ้งเตือนอาหารหมด
bool notifyBirdVisit = false;     // แจ้งเตือนเมื่อนกมาเยี่ยม
bool notifySystemError = true;    // แจ้งเตือนข้อผิดพลาดระบบ
```

### 🎛️ การปรับแต่งระบบสำหรับนก

```cpp
// Bird-Specific Configuration
#define DEFAULT_PORTION_SIZE 20   // กรัม (เหมาะสำหรับนก)
#define MIN_PORTION_SIZE 5        // กรัมต่ำสุด
#define MAX_PORTION_SIZE 50       // กรัมสูงสุด
#define FOOD_CONTAINER_HEIGHT 15  // ซม. (ถังเล็กกว่า)
#define LOW_FOOD_THRESHOLD 2      // ซม.
#define EMPTY_FEEDER_THRESHOLD 3  // กรัม

// Light Sensor Configuration
#define DAYLIGHT_THRESHOLD 500    // ADC value for daylight
#define NIGHT_THRESHOLD 100       // ADC value for night
#define LIGHT_CHECK_INTERVAL 60000 // Check every minute

// Bird Detection Settings
#define PIR_SENSITIVITY 3         // PIR trigger count
#define PIR_TIMEOUT 10000        // 10 seconds timeout
#define BIRD_COUNT_RESET_HOUR 6   // Reset count at 6 AM

// Weather-resistant Settings
#define SERVO_TIMEOUT 2000        // Servo operation timeout
#define SENSOR_READ_INTERVAL 5000 // Sensor reading interval
#define SYSTEM_HEALTH_CHECK 30000 // Health check every 30 seconds
```

---

## 🎮 การใช้งาน

### 🚀 การเริ่มต้นใช้งาน

1. **เชื่อมต่อไฟฟ้า**
   - ตรวจสอบการต่อวงจรให้ถูกต้อง
   - ใช้แหล่งจ่ายไฟ 5V/2A หรือ 12V/1A
   - สำหรับติดตั้งกลางแจ้ง ใช้ Solar Panel + Battery

2. **Boot Sequence**
   ```
   ╔════════════════╗
   ║ FW make by RDTRC║
   ║ Version 4.0     ║
   ╚════════════════╝
           ↓
   ╔════════════════╗
   ║ Bird Feeder    ║
   ║ Initializing...║
   ╚════════════════╝
           ↓
   ╔════════════════╗
   ║ Light: Day     ║
   ║ Birds: 0       ║
   ╚════════════════╝
           ↓
   ╔════════════════╗
   ║ WiFi: Connected║
   ║ IP: 192.168.1.x║
   ╚════════════════╝
           ↓
   ╔════════════════╗
   ║ System Ready   ║
   ║ Next: 07:00    ║
   ╚════════════════╝
   ```

3. **การตรวจจับแสง**
   - ระบบจะตรวจจับแสงธรรมชาติ
   - ให้อาหารเฉพาะเวลากลางวัน
   - หยุดให้อาหารเมื่อมืด

### 📱 การควบคุมผ่าน LCD

#### 🖥️ หน้าจอ LCD สำหรับนก

**หน้าหลัก (Main Screen)**
```
┌────────────────┐
│Bird Feeder v4.0│
│Light: Day 75%  │
└────────────────┘
```

**หน้าสถานะนก (Bird Status Screen)**
```
┌────────────────┐
│Birds: 3 visits │
│Food: 1.2kg     │
└────────────────┘
```

**หน้าตารางเวลา (Schedule Screen)**
```
┌────────────────┐
│Next: 07:00 20g │
│Auto: ON        │
└────────────────┘
```

**หน้าแสง (Light Screen)**
```
┌────────────────┐
│Light: 756 ADC  │
│Status: Daylight│
└────────────────┘
```

#### 🎛️ การใช้ปุ่ม

| ปุ่ม | การทำงาน |
|------|----------|
| **Manual Feed** | ให้อาหารทันที (เฉพาะเวลากลางวัน) |
| **LCD Next** | เปลี่ยนหน้าจอ LCD |
| **Reset** | รีสตาร์ทระบบ |

### ⏰ การตั้งเวลาให้อาหารสำหรับนก

```cpp
// Bird Feeding Schedule (Daylight Hours Only)
feedingTimes[0] = {7, 0, 20};   // 07:00 น. 20g (Early morning)
feedingTimes[1] = {11, 0, 20};  // 11:00 น. 20g (Mid morning)
feedingTimes[2] = {15, 0, 20};  // 15:00 น. 20g (Afternoon)
feedingTimes[3] = {17, 30, 20}; // 17:30 น. 20g (Early evening)

// Night feeding disabled automatically
bool isDaylight() {
  int lightLevel = analogRead(LIGHT_SENSOR_PIN);
  return (lightLevel > DAYLIGHT_THRESHOLD);
}
```

### 🐦 การนับและติดตามนก

```cpp
// Bird Visit Tracking
struct BirdVisit {
  unsigned long timestamp;
  int duration;        // seconds
  bool fedDuringVisit;
};

BirdVisit dailyVisits[50];  // Store up to 50 visits per day
int todayBirdCount = 0;
int totalBirdVisits = 0;

void recordBirdVisit() {
  if (todayBirdCount < 50) {
    dailyVisits[todayBirdCount].timestamp = millis();
    dailyVisits[todayBirdCount].duration = 0;
    dailyVisits[todayBirdCount].fedDuringVisit = false;
    todayBirdCount++;
    totalBirdVisits++;
    
    // Send notification
    if (notifyBirdVisit) {
      sendLineNotification("🐦 นกมาเยี่ยมแล้ว! จำนวนวันนี้: " + String(todayBirdCount));
    }
  }
}
```

---

## 🌐 Web Interface

### 🏠 หน้าหลัก (Bird Dashboard)

เข้าถึงได้ที่: `http://[ESP32_IP_ADDRESS]/`

**คุณสมบัติหลัก:**
- แสดงสถานะระบบแบบเรียลไทม์
- ติดตามจำนวนนกที่มาเยี่ยม
- ตรวจสอบระดับแสงปัจจุบัน
- ควบคุมการให้อาหารด้วยตนเอง
- ตั้งค่าตารางเวลา

### 📊 หน้าต่างๆ ของ Web Interface

#### 1. **Bird Dashboard (หน้าหลัก)**
```html
┌─────────────────────────────────────┐
│ 🐦 RDTRC Bird Feeding System       │
├─────────────────────────────────────┤
│ Status: ● Online                    │
│ Light Level: ☀️ Daylight (756)     │
│ Bird Visits Today: 🐦 12 visits     │
│ Next Feeding: 15:00 (20g)          │
│ Food Level: 1.2kg                   │
│                                     │
│ [Feed Now] [Settings] [Bird Stats]  │
└─────────────────────────────────────┘
```

#### 2. **Bird Statistics (สถิตินก)**
```html
┌─────────────────────────────────────┐
│ 📊 Bird Visit Statistics            │
├─────────────────────────────────────┤
│ Today's Visits: 12                  │
│ Peak Hour: 07:00-08:00 (4 visits)   │
│ Average Visit Duration: 3.2 min     │
│ Fed During Visit: 8/12 (67%)        │
│                                     │
│ Weekly Summary:                     │
│ Mon: 15 visits  Thu: 8 visits       │
│ Tue: 12 visits  Fri: 14 visits      │
│ Wed: 9 visits   Sat: 18 visits      │
│                                     │
│ [Export Data] [Reset Counter]       │
└─────────────────────────────────────┘
```

#### 3. **Light Monitoring (การติดตามแสง)**
```html
┌─────────────────────────────────────┐
│ ☀️ Light Level Monitoring           │
├─────────────────────────────────────┤
│ Current Light: 756 ADC              │
│ Status: ☀️ Daylight                 │
│ Sunrise Today: 06:23                │
│ Sunset Today: 18:45                 │
│                                     │
│ Light History (24h):                │
│ ████████████░░░░░░░░████████████     │
│ 00:00    06:00    12:00    18:00    │
│                                     │
│ Auto Feeding: ✅ Enabled            │
│ Night Feeding: ❌ Disabled          │
│                                     │
│ [Calibrate Sensor] [Settings]       │
└─────────────────────────────────────┘
```

#### 4. **Settings (การตั้งค่า)**
```html
┌─────────────────────────────────────┐
│ ⚙️ Bird Feeder Settings             │
├─────────────────────────────────────┤
│ Feeding Schedule:                   │
│ Time 1: [07:00] Amount: [20]g ✅    │
│ Time 2: [11:00] Amount: [20]g ✅    │
│ Time 3: [15:00] Amount: [20]g ✅    │
│ Time 4: [17:30] Amount: [20]g ✅    │
│                                     │
│ Light Settings:                     │
│ Daylight Threshold: [500] ADC       │
│ Night Threshold: [100] ADC          │
│ Auto Light Detection: ✅            │
│                                     │
│ Bird Detection:                     │
│ PIR Sensitivity: [3] triggers       │
│ Visit Timeout: [10] seconds         │
│                                     │
│ Notifications:                      │
│ ☑ Low Food Alert                   │
│ ☑ Feeding Notifications            │
│ ☐ Bird Visit Alerts               │
│ ☑ System Errors                   │
│                                     │
│ [Save Settings] [Reset to Default]  │
└─────────────────────────────────────┘
```

### 🔗 Bird-Specific API Endpoints

```javascript
// Bird Visit Statistics
GET /api/birds/stats
Response: {
  "today": {
    "visits": 12,
    "peakHour": "07:00-08:00",
    "avgDuration": 192,
    "fedVisits": 8
  },
  "weekly": [
    {"day": "Mon", "visits": 15},
    {"day": "Tue", "visits": 12},
    // ...
  ]
}

// Light Level Monitoring
GET /api/light
Response: {
  "current": 756,
  "status": "daylight",
  "sunrise": "06:23",
  "sunset": "18:45",
  "autoFeeding": true
}

// Bird Detection Settings
POST /api/birds/settings
Body: {
  "pirSensitivity": 3,
  "visitTimeout": 10000,
  "enableVisitNotifications": false
}
```

---

## 📱 Blynk App Integration

### 🔧 การตั้งค่า Blynk App สำหรับนก

1. **ดาวน์โหลด Blynk App**
   - iOS: App Store
   - Android: Google Play Store

2. **สร้างโปรเจค**
   - เลือก "ESP32"
   - ใส่ชื่อโปรเจค "Bird Feeding System"

3. **เพิ่ม Widgets เฉพาะนก**

#### 📊 Widget Layout สำหรับนก

```
┌─────────────────────────────────────┐
│ 🐦 Bird Feeding System              │
├─────────────────────────────────────┤
│ ● System Status (V0)                │
│   ○ Green: Online & Daylight        │
│   ○ Yellow: Online & Night          │
│   ○ Red: Offline                    │
├─────────────────────────────────────┤
│ 📊 Food Level (V1)                  │
│   [██████░░░░] 60%                  │
├─────────────────────────────────────┤
│ 🐦 Bird Visits Today (V2)           │
│   12 visits                         │
├─────────────────────────────────────┤
│ ☀️ Light Level (V3)                 │
│   756 ADC - Daylight                │
├─────────────────────────────────────┤
│ [Feed Now] Button (V4)              │
│ (Disabled at night)                 │
├─────────────────────────────────────┤
│ 🎚️ Portion Size (V5)               │
│   [5g ●───── 50g] Currently: 20g   │
├─────────────────────────────────────┤
│ ⏰ Next Feeding (V6)                │
│   15:00 - 20g                       │
├─────────────────────────────────────┤
│ 📊 Daily Stats (V7)                 │
│   Fed: 60g | Visits: 12            │
├─────────────────────────────────────┤
│ 💬 System Messages (V8)             │
│   [Terminal Widget]                 │
└─────────────────────────────────────┘
```

#### 🎛️ Virtual Pins สำหรับนก

| Virtual Pin | Widget | Function |
|-------------|--------|----------|
| V0 | LED | System Status (Green=Day/Online, Yellow=Night/Online, Red=Offline) |
| V1 | Gauge | Food Level (0-100%) |
| V2 | Value Display | Bird Visits Today |
| V3 | Value Display | Light Level (ADC + Status) |
| V4 | Button | Manual Feed (Auto-disabled at night) |
| V5 | Slider | Portion Size (5-50g) |
| V6 | Value Display | Next Feeding Time |
| V7 | Value Display | Daily Summary |
| V8 | Terminal | System Messages |
| V9 | LED | Light Status (Day/Night) |
| V10 | Graph | Bird Visit History |

### 📱 Blynk Code สำหรับนก

```cpp
// Blynk Virtual Pin Handlers for Bird System

// System Status LED
BLYNK_WRITE(V0) {
  // This is read-only, controlled by system
}

// Manual Feed Button (with daylight check)
BLYNK_WRITE(V4) {
  int buttonState = param.asInt();
  if (buttonState == 1) {
    if (isDaylight()) {
      manualFeed(currentPortionSize);
      Blynk.virtualWrite(V8, "🐦 Manual feeding completed");
    } else {
      Blynk.virtualWrite(V8, "🌙 Night time - feeding disabled");
    }
  }
}

// Portion Size Slider
BLYNK_WRITE(V5) {
  currentPortionSize = param.asInt();
  if (currentPortionSize < MIN_PORTION_SIZE) currentPortionSize = MIN_PORTION_SIZE;
  if (currentPortionSize > MAX_PORTION_SIZE) currentPortionSize = MAX_PORTION_SIZE;
  
  Blynk.virtualWrite(V8, "Portion size set to: " + String(currentPortionSize) + "g");
}

// Update Blynk with bird-specific data
void updateBlynkBirdData() {
  // System status with light consideration
  if (isWiFiConnected && Blynk.connected()) {
    if (isDaylight()) {
      Blynk.virtualWrite(V0, 255); // Green - Day & Online
    } else {
      Blynk.virtualWrite(V0, 128); // Yellow - Night & Online  
    }
  } else {
    Blynk.virtualWrite(V0, 0); // Red - Offline
  }
  
  // Food level
  float foodPercent = (getCurrentFoodLevel() / (FOOD_CONTAINER_HEIGHT * 100.0)) * 100;
  Blynk.virtualWrite(V1, foodPercent);
  
  // Bird visits today
  Blynk.virtualWrite(V2, todayBirdCount);
  
  // Light level with status
  int lightLevel = analogRead(LIGHT_SENSOR_PIN);
  String lightStatus = isDaylight() ? "☀️ Daylight" : "🌙 Night";
  Blynk.virtualWrite(V3, String(lightLevel) + " - " + lightStatus);
  
  // Light status LED
  Blynk.virtualWrite(V9, isDaylight() ? 255 : 0);
  
  // Next feeding time
  String nextFeed = getNextFeedingTime();
  if (!isDaylight()) nextFeed += " (Waiting for daylight)";
  Blynk.virtualWrite(V6, nextFeed);
  
  // Daily summary
  int dailyFed = getTotalFedToday();
  Blynk.virtualWrite(V7, "Fed: " + String(dailyFed) + "g | Visits: " + String(todayBirdCount));
}
```

---

## 🔌 API Reference

### 📡 Bird-Specific REST API Endpoints

#### 1. **Bird System Status**
```http
GET /api/status
Content-Type: application/json

Response:
{
  "system": {
    "name": "Bird Feeder",
    "version": "4.0",
    "uptime": 86400,
    "deviceId": "RDTRC_BIRD_FEEDER"
  },
  "environment": {
    "lightLevel": 756,
    "isDaylight": true,
    "sunrise": "06:23",
    "sunset": "18:45"
  },
  "birds": {
    "visitsToday": 12,
    "totalVisits": 847,
    "lastVisit": "2024-12-19T14:32:00Z",
    "peakHour": "07:00-08:00"
  },
  "feeding": {
    "nextTime": "15:00",
    "nextAmount": 20,
    "lastFed": "11:00",
    "dailyTotal": 60,
    "autoEnabled": true,
    "nightFeedingDisabled": true
  },
  "sensors": {
    "foodLevel": 1200,
    "feedingEnabled": true
  }
}
```

#### 2. **Bird Visit Statistics**
```http
GET /api/birds/stats?days=7
Response:
{
  "summary": {
    "totalVisits": 84,
    "averagePerDay": 12,
    "peakDay": "Saturday",
    "avgVisitDuration": 192
  },
  "daily": [
    {
      "date": "2024-12-19",
      "visits": 12,
      "peakHour": "07:00-08:00",
      "fedVisits": 8,
      "totalDuration": 2304
    }
  ],
  "hourly": {
    "00": 0, "01": 0, "02": 0, "03": 0,
    "04": 0, "05": 0, "06": 1, "07": 4,
    "08": 3, "09": 2, "10": 1, "11": 3,
    "12": 2, "13": 1, "14": 2, "15": 3,
    "16": 2, "17": 4, "18": 1, "19": 0,
    "20": 0, "21": 0, "22": 0, "23": 0
  }
}
```

#### 3. **Light Level Monitoring**
```http
GET /api/light
Response:
{
  "current": {
    "level": 756,
    "status": "daylight",
    "percentage": 75.6
  },
  "thresholds": {
    "daylight": 500,
    "night": 100
  },
  "schedule": {
    "sunrise": "06:23",
    "sunset": "18:45",
    "daylightHours": 12.37
  },
  "history24h": [
    {"hour": 0, "avgLevel": 45},
    {"hour": 1, "avgLevel": 38},
    // ... 24 hours of data
  ]
}

POST /api/light/calibrate
Body: {
  "daylightThreshold": 500,
  "nightThreshold": 100
}
```

#### 4. **Manual Feeding with Daylight Check**
```http
POST /api/feed
Content-Type: application/json

Request Body:
{
  "amount": 20,
  "force": false,
  "ignoreDaylight": false
}

Response (Success - Daylight):
{
  "success": true,
  "message": "Feeding completed successfully",
  "timestamp": "2024-12-19T14:30:00Z",
  "amount": 20,
  "lightLevel": 756,
  "isDaylight": true,
  "birdPresent": true
}

Response (Blocked - Night):
{
  "success": false,
  "message": "Feeding blocked - night time",
  "timestamp": "2024-12-19T22:30:00Z",
  "lightLevel": 45,
  "isDaylight": false,
  "canForce": true
}
```

#### 5. **Bird Detection Settings**
```http
GET /api/birds/detection
Response:
{
  "pirSensitivity": 3,
  "visitTimeout": 10000,
  "minVisitDuration": 5000,
  "enableNotifications": false,
  "countResetHour": 6
}

POST /api/birds/detection
Body: {
  "pirSensitivity": 3,
  "visitTimeout": 10000,
  "enableNotifications": false
}
```

### 📊 WebSocket Events สำหรับนก

```javascript
// Connect to Bird Feeder WebSocket
const ws = new WebSocket('ws://192.168.1.50/ws');

ws.onmessage = function(event) {
  const data = JSON.parse(event.data);
  
  switch(data.type) {
    case 'bird_detected':
      // New bird visit detected
      console.log(`Bird visit #${data.visitNumber} detected`);
      break;
      
    case 'light_changed':
      // Light level changed (day/night transition)
      console.log(`Light status: ${data.status}, Level: ${data.level}`);
      break;
      
    case 'feeding_blocked':
      // Feeding attempt blocked due to night time
      console.log(`Feeding blocked: ${data.reason}`);
      break;
      
    case 'daily_reset':
      // Daily bird counter reset
      console.log(`Daily stats reset. Yesterday: ${data.yesterdayVisits} visits`);
      break;
      
    case 'feeding_completed':
      // Feeding completed successfully
      console.log(`Fed ${data.amount}g. Birds present: ${data.birdsPresent}`);
      break;
  }
};
```

---

## 🛠️ การแก้ปัญหา

### ❌ ปัญหาเฉพาะระบบนก

#### 1. **เซ็นเซอร์แสงทำงานผิดพลาด**

**อาการ:**
- ระบบคิดว่าเป็นกลางคืนตลอดเวลา
- ให้อาหารในเวลากลางคืน
- แสดงค่าแสงผิดพลาด

**การแก้ไข:**
```cpp
// ปรับเทียบเซ็นเซอร์แสง
void calibrateLightSensor() {
  Serial.println("Light Sensor Calibration");
  Serial.println("Place sensor in bright daylight...");
  delay(5000);
  
  int daylightReading = 0;
  for (int i = 0; i < 10; i++) {
    daylightReading += analogRead(LIGHT_SENSOR_PIN);
    delay(100);
  }
  daylightReading /= 10;
  
  Serial.println("Now cover sensor completely...");
  delay(5000);
  
  int nightReading = 0;
  for (int i = 0; i < 10; i++) {
    nightReading += analogRead(LIGHT_SENSOR_PIN);
    delay(100);
  }
  nightReading /= 10;
  
  // Set thresholds based on readings
  DAYLIGHT_THRESHOLD = nightReading + ((daylightReading - nightReading) * 0.3);
  NIGHT_THRESHOLD = nightReading + ((daylightReading - nightReading) * 0.1);
  
  Serial.printf("Calibrated - Day: %d, Night: %d\n", DAYLIGHT_THRESHOLD, NIGHT_THRESHOLD);
}
```

#### 2. **PIR Sensor ตรวจจับนกไม่ถูกต้อง**

**อาการ:**
- นับนกผิดพลาด (มากเกินไป หรือ น้อยเกินไป)
- ตรวจจับสัตว์อื่นที่ไม่ใช่นก

**การแก้ไข:**
```cpp
// ปรับปรุงการตรวจจับนก
#define PIR_DEBOUNCE_TIME 2000  // 2 seconds
#define MIN_VISIT_DURATION 5000 // 5 seconds minimum

unsigned long lastPIRTrigger = 0;
unsigned long visitStartTime = 0;
bool birdCurrentlyPresent = false;

void improvedBirdDetection() {
  bool pirState = digitalRead(PIR_SENSOR_PIN);
  unsigned long currentTime = millis();
  
  if (pirState && !birdCurrentlyPresent) {
    // Bird detected
    if (currentTime - lastPIRTrigger > PIR_DEBOUNCE_TIME) {
      birdCurrentlyPresent = true;
      visitStartTime = currentTime;
      lastPIRTrigger = currentTime;
      
      DEBUG_PRINTLN("Bird visit started");
    }
  } else if (!pirState && birdCurrentlyPresent) {
    // Bird left
    unsigned long visitDuration = currentTime - visitStartTime;
    if (visitDuration > MIN_VISIT_DURATION) {
      // Valid visit - count it
      recordBirdVisit();
      DEBUG_PRINTF("Bird visit ended - Duration: %lu ms\n", visitDuration);
    }
    birdCurrentlyPresent = false;
  }
}
```

#### 3. **ระบบให้อาหารในเวลากลางคืน**

**อาการ:**
- ให้อาหารแม้ว่าจะมืดแล้ว
- เซ็นเซอร์แสงไม่ทำงาน

**การแก้ไข:**
```cpp
// ตรวจสอบแสงก่อนให้อาหารทุกครั้ง
bool canFeedNow() {
  // Check if it's daylight
  if (!isDaylight()) {
    DEBUG_PRINTLN("Feeding blocked - Night time");
    systemLCD.clear();
    systemLCD.print("Night Time");
    systemLCD.setCursor(0, 1);
    systemLCD.print("No Feeding");
    return false;
  }
  
  // Check food level
  if (getCurrentFoodLevel() < EMPTY_FEEDER_THRESHOLD) {
    DEBUG_PRINTLN("Feeding blocked - No food");
    return false;
  }
  
  return true;
}

void scheduledFeeding() {
  if (!canFeedNow()) {
    return;
  }
  
  // Proceed with feeding
  performFeeding(DEFAULT_PORTION_SIZE);
}
```

#### 4. **การนับนกไม่แม่นยำ**

**อาการ:**
- นับนกซ้ำ
- ไม่นับนกที่มาจริง

**การแก้ไข:**
```cpp
// ปรับปรุงระบบนับนก
struct BirdVisitData {
  unsigned long startTime;
  unsigned long endTime;
  bool counted;
  int triggerCount;
};

BirdVisitData currentVisit;

void accurateBirdCounting() {
  bool pirState = digitalRead(PIR_SENSOR_PIN);
  unsigned long now = millis();
  
  if (pirState) {
    if (!currentVisit.counted) {
      if (currentVisit.startTime == 0) {
        // Start of new visit
        currentVisit.startTime = now;
        currentVisit.triggerCount = 1;
        currentVisit.counted = false;
      } else {
        // Continuing visit
        currentVisit.triggerCount++;
      }
    }
  } else {
    // No motion detected
    if (currentVisit.startTime > 0 && !currentVisit.counted) {
      unsigned long visitDuration = now - currentVisit.startTime;
      
      // Valid visit criteria
      if (visitDuration > 3000 && // At least 3 seconds
          visitDuration < 300000 && // Less than 5 minutes
          currentVisit.triggerCount >= 3) { // Multiple triggers
        
        // Count this as a valid bird visit
        todayBirdCount++;
        currentVisit.counted = true;
        currentVisit.endTime = now;
        
        // Log the visit
        DEBUG_PRINTF("Bird visit counted: Duration %lu ms, Triggers: %d\n", 
                    visitDuration, currentVisit.triggerCount);
        
        // Send notification if enabled
        if (notifyBirdVisit) {
          sendLineNotification("🐦 นกมาเยี่ยม ครั้งที่ " + String(todayBirdCount) + 
                               " วันนี้ (อยู่ " + String(visitDuration/1000) + " วินาที)");
        }
      }
      
      // Reset for next visit
      memset(&currentVisit, 0, sizeof(currentVisit));
    }
  }
}
```

### 🔍 การ Debug เฉพาะนก

#### 📊 Serial Monitor Output สำหรับนก

```
RDTRC Bird Feeding System v4.0
==============================
[INIT] Initializing Bird Feeder... OK
[INIT] Light sensor calibration... OK
[INIT] PIR sensitivity set to: 3
[INIT] Daylight threshold: 500 ADC
[INIT] Night threshold: 100 ADC

[LIGHT] Current level: 756 ADC (Daylight)
[BIRDS] Visit counter reset (new day)
[FEED] Scheduled feeding enabled (daylight detected)

[SYSTEM] Ready for bird feeding!
[SENSOR] Light: 756 (Day) | Food: 1.2kg | Birds today: 0

[BIRD] Motion detected! Visit #1 started
[BIRD] Visit #1 ended - Duration: 4.2s (Valid)
[FEED] Auto feeding triggered - 20g dispensed
[BIRD] Total visits today: 1

Loop running... Free heap: 234567
[LIGHT] Status: Daylight | Level: 742 ADC
```

### ⚡ การรีเซ็ตข้อมูลนก

```cpp
// Reset bird statistics
void resetBirdStats() {
  todayBirdCount = 0;
  totalBirdVisits = 0;
  memset(dailyVisits, 0, sizeof(dailyVisits));
  
  // Clear SPIFFS bird data
  if (SPIFFS.exists("/bird_stats.json")) {
    SPIFFS.remove("/bird_stats.json");
  }
  
  systemLCD.clear();
  systemLCD.print("Bird Stats");
  systemLCD.setCursor(0, 1);
  systemLCD.print("Reset Complete");
  
  DEBUG_PRINTLN("Bird statistics reset");
}

// Daily automatic reset (at 6 AM)
void checkDailyReset() {
  timeClient.update();
  int currentHour = timeClient.getHours();
  
  if (currentHour == 6 && !dailyResetDone) {
    // Save yesterday's data
    saveDailyBirdData();
    
    // Reset counters
    resetBirdStats();
    
    // Send daily report
    sendDailyBirdReport();
    
    dailyResetDone = true;
  } else if (currentHour != 6) {
    dailyResetDone = false;
  }
}
```

---

## 🔧 การบำรุงรักษา

### 📅 การบำรุงรักษาประจำสำหรับระบบนก

#### รายวัน
- ✅ ตรวจสอบระดับอาหารในถัง
- ✅ ทำความสะอาดที่ให้อาหาร (ป้องกันเชื้อโรค)
- ✅ ตรวจสอบการทำงานของเซ็นเซอร์แสง
- ✅ ดูสถิติการเยี่ยมของนกผ่าน LCD/Web

#### รายสัปดาห์
- ✅ ทำความสะอาด PIR Sensor (ฝุ่นละออง)
- ✅ ตรวจสอบการเชื่อมต่อสายไฟกลางแจ้ง
- ✅ อัพเดทข้อมูลสถิตินกผ่าน Web Interface
- ✅ ตรวจสอบการป้องกันน้ำของกล่อง

#### รายเดือน
- ✅ ปรับเทียบเซ็นเซอร์ชั่งน้ำหนัก
- ✅ ทำความสะอาดระบบให้อาหาร (ป้องกันอาหารเสีย)
- ✅ ตรวจสอบ Servo Motor (การทำงานกลางแจ้ง)
- ✅ สำรองข้อมูลสถิตินก

#### รายฤดูกาล
- ✅ ตรวจสอบซีลกันน้ำทั้งหมด
- ✅ ทำความสะอาดแผงโซลาร์ (ถ้ามี)
- ✅ เปลี่ยนแบตเตอรี่สำรอง
- ✅ ปรับตารางเวลาตามฤดูกาล (เวลาแสง)

### 🧹 การทำความสะอาดเฉพาะนก

```cpp
// Bird Feeder Cleaning Mode
void enterBirdCleaningMode() {
  systemLCD.clear();
  systemLCD.print("Cleaning Mode");
  systemLCD.setCursor(0, 1);
  systemLCD.print("Birds Disabled");
  
  // Disable all bird-related functions
  feedingEnabled = false;
  birdDetectionEnabled = false;
  
  // Move servo to full open position for cleaning
  feedingServo.attach(SERVO_PIN);
  feedingServo.write(180); // Full open
  delay(2000);
  feedingServo.detach();
  
  // Turn on status LED for visibility
  digitalWrite(STATUS_LED_PIN, HIGH);
  
  // Send notification
  sendLineNotification("🧹 ระบบให้อาหารนกเข้าสู่โหมดทำความสะอาด");
  
  DEBUG_PRINTLN("Bird feeder in cleaning mode");
}

void exitBirdCleaningMode() {
  // Re-enable bird functions
  feedingEnabled = true;
  birdDetectionEnabled = true;
  
  // Return servo to closed position
  feedingServo.attach(SERVO_PIN);
  feedingServo.write(0); // Closed
  delay(1000);
  feedingServo.detach();
  
  // Turn off status LED
  digitalWrite(STATUS_LED_PIN, LOW);
  
  systemLCD.clear();
  systemLCD.print("Cleaning Done");
  systemLCD.setCursor(0, 1);
  systemLCD.print("System Active");
  
  sendLineNotification("✅ ระบบให้อาหารนกพร้อมใช้งานแล้ว");
  
  DEBUG_PRINTLN("Bird feeder cleaning mode ended");
}
```

### 🌦️ การบำรุงรักษากลางแจ้ง

```cpp
// Weather Resistance Check
void weatherResistanceCheck() {
  // Check enclosure temperature
  float internalTemp = getInternalTemperature();
  if (internalTemp > 50.0) {
    // Too hot - activate cooling
    digitalWrite(COOLING_FAN_PIN, HIGH);
    sendLineNotification("⚠️ อุณหภูมิภายในสูง: " + String(internalTemp) + "°C");
  } else if (internalTemp < -5.0) {
    // Too cold - activate heater
    digitalWrite(HEATER_PIN, HIGH);
    sendLineNotification("❄️ อุณหภูมิภายในต่ำ: " + String(internalTemp) + "°C");
  }
  
  // Check humidity level
  float humidity = getInternalHumidity();
  if (humidity > 80.0) {
    // High humidity - activate dehumidifier
    digitalWrite(DEHUMIDIFIER_PIN, HIGH);
    sendLineNotification("💧 ความชื้นสูง: " + String(humidity) + "%");
  }
  
  // Check water ingress
  bool waterDetected = digitalRead(WATER_SENSOR_PIN);
  if (waterDetected) {
    // Water detected inside enclosure
    systemLCD.clear();
    systemLCD.print("WATER ALERT!");
    systemLCD.setCursor(0, 1);
    systemLCD.print("Check Seals");
    
    sendLineNotification("🚨 ตรวจพบน้ำรั่วเข้าในกล่อง! ตรวจสอบซีลกันน้ำ");
    
    // Disable system for safety
    feedingEnabled = false;
  }
}
```

### 📊 การสำรองข้อมูลนก

```cpp
// Bird Data Backup System
void backupBirdData() {
  File backupFile = SPIFFS.open("/bird_backup.json", "w");
  if (backupFile) {
    StaticJsonDocument<4096> backupDoc;
    
    // Current stats
    JsonObject current = backupDoc.createNestedObject("current");
    current["date"] = getCurrentDate();
    current["visits"] = todayBirdCount;
    current["totalVisits"] = totalBirdVisits;
    
    // Daily history
    JsonArray dailyHistory = backupDoc.createNestedArray("dailyHistory");
    for (int i = 0; i < 30; i++) { // Last 30 days
      if (dailyBirdHistory[i].date != "") {
        JsonObject day = dailyHistory.createNestedObject();
        day["date"] = dailyBirdHistory[i].date;
        day["visits"] = dailyBirdHistory[i].visits;
        day["peakHour"] = dailyBirdHistory[i].peakHour;
        day["totalFed"] = dailyBirdHistory[i].totalFed;
      }
    }
    
    // Settings backup
    JsonObject settings = backupDoc.createNestedObject("settings");
    settings["daylightThreshold"] = DAYLIGHT_THRESHOLD;
    settings["nightThreshold"] = NIGHT_THRESHOLD;
    settings["pirSensitivity"] = PIR_SENSITIVITY;
    settings["defaultPortion"] = DEFAULT_PORTION_SIZE;
    
    serializeJson(backupDoc, backupFile);
    backupFile.close();
    
    DEBUG_PRINTLN("Bird data backup completed");
    sendLineNotification("💾 สำรองข้อมูลนกเรียบร้อย");
  }
}

// Restore bird data from backup
void restoreBirdData() {
  File backupFile = SPIFFS.open("/bird_backup.json", "r");
  if (backupFile) {
    StaticJsonDocument<4096> backupDoc;
    deserializeJson(backupDoc, backupFile);
    
    // Restore current stats
    todayBirdCount = backupDoc["current"]["visits"];
    totalBirdVisits = backupDoc["current"]["totalVisits"];
    
    // Restore settings
    DAYLIGHT_THRESHOLD = backupDoc["settings"]["daylightThreshold"];
    NIGHT_THRESHOLD = backupDoc["settings"]["nightThreshold"];
    PIR_SENSITIVITY = backupDoc["settings"]["pirSensitivity"];
    
    backupFile.close();
    DEBUG_PRINTLN("Bird data restored from backup");
  }
}
```

---

## 🚀 การพัฒนาต่อ

### 💡 ไอเดียการพัฒนาเฉพาะนก

#### 📷 การเพิ่ม ESP32-CAM สำหรับถ่ายภาพนก
```cpp
// Bird Photography System
#include "esp_camera.h"

struct BirdPhoto {
  String filename;
  unsigned long timestamp;
  int birdCount;
  String species; // Future: AI bird identification
};

void takeBirdPhoto() {
  if (!birdCurrentlyPresent) return;
  
  camera_fb_t * fb = esp_camera_fb_get();
  if (fb) {
    // Generate filename with timestamp
    String filename = "/bird_" + String(millis()) + ".jpg";
    
    // Save to SPIFFS or SD card
    File photoFile = SPIFFS.open(filename, "w");
    if (photoFile) {
      photoFile.write(fb->buf, fb->len);
      photoFile.close();
      
      // Log photo
      BirdPhoto photo;
      photo.filename = filename;
      photo.timestamp = millis();
      photo.birdCount = todayBirdCount;
      
      // Send notification with photo
      sendPhotoNotification(filename);
      
      DEBUG_PRINTLN("Bird photo captured: " + filename);
    }
    
    esp_camera_fb_return(fb);
  }
}

void sendPhotoNotification(String filename) {
  // Send LINE notification with photo
  HTTPClient http;
  http.begin("https://notify-api.line.me/api/notify");
  http.addHeader("Authorization", "Bearer " + String(lineToken));
  http.addHeader("Content-Type", "multipart/form-data");
  
  // Prepare multipart data with photo
  // ... implementation for sending photo via LINE Notify
}
```

#### 🤖 AI การจำแนกชนิดนก
```cpp
// Bird Species Recognition (Future Implementation)
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"

struct BirdSpecies {
  String name;
  String scientificName;
  int confidence;
  String feedingPreference;
};

BirdSpecies identifyBird(camera_fb_t* fb) {
  BirdSpecies species;
  
  // Preprocess image for AI model
  preprocessImage(fb);
  
  // Run inference
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    species.name = "Unknown";
    species.confidence = 0;
    return species;
  }
  
  // Get results
  TfLiteTensor* output = interpreter->output(0);
  
  // Parse species identification
  int maxIndex = 0;
  float maxValue = 0;
  for (int i = 0; i < NUM_BIRD_SPECIES; i++) {
    if (output->data.f[i] > maxValue) {
      maxValue = output->data.f[i];
      maxIndex = i;
    }
  }
  
  species.name = birdSpeciesNames[maxIndex];
  species.confidence = maxValue * 100;
  species.feedingPreference = getFeedingPreference(maxIndex);
  
  return species;
}

void adaptFeedingToBird(BirdSpecies species) {
  // Adjust feeding amount based on bird species
  if (species.name == "Sparrow") {
    currentPortionSize = 10; // Small birds
  } else if (species.name == "Crow") {
    currentPortionSize = 40; // Large birds
  } else if (species.name == "Pigeon") {
    currentPortionSize = 25; // Medium birds
  }
  
  DEBUG_PRINTF("Adjusted feeding for %s: %dg\n", 
               species.name.c_str(), currentPortionSize);
}
```

#### 🌤️ การบูรณาการข้อมูลสภาพอากาศ
```cpp
// Weather Integration for Bird Feeding
struct WeatherData {
  float temperature;
  float humidity;
  String condition; // "sunny", "rainy", "cloudy"
  float windSpeed;
  int pressure;
};

WeatherData getWeatherData() {
  WeatherData weather;
  
  HTTPClient http;
  http.begin("http://api.openweathermap.org/data/2.5/weather?q=Bangkok&appid=YOUR_API_KEY");
  int httpResponseCode = http.GET();
  
  if (httpResponseCode == 200) {
    String payload = http.getString();
    StaticJsonDocument<1024> weatherDoc;
    deserializeJson(weatherDoc, payload);
    
    weather.temperature = weatherDoc["main"]["temp"];
    weather.humidity = weatherDoc["main"]["humidity"];
    weather.condition = weatherDoc["weather"][0]["main"];
    weather.windSpeed = weatherDoc["wind"]["speed"];
    weather.pressure = weatherDoc["main"]["pressure"];
  }
  
  http.end();
  return weather;
}

void adjustFeedingForWeather(WeatherData weather) {
  // Increase feeding in cold weather
  if (weather.temperature < 20.0) {
    for (int i = 0; i < 4; i++) {
      feedingTimes[i].amount += 5; // +5g in cold weather
    }
    sendLineNotification("❄️ อากาศหนาว เพิ่มปริมาณอาหารนก");
  }
  
  // Reduce feeding in very hot weather
  if (weather.temperature > 35.0) {
    for (int i = 0; i < 4; i++) {
      feedingTimes[i].amount -= 3; // -3g in hot weather
    }
    sendLineNotification("🔥 อากาศร้อน ลดปริมาณอาหารนก");
  }
  
  // Skip feeding in heavy rain
  if (weather.condition == "Rain" && weather.windSpeed > 10.0) {
    feedingEnabled = false;
    sendLineNotification("🌧️ ฝนตกหนัก หยุดให้อาหารชั่วคราว");
  }
}
```

#### 🌐 ระบบเครือข่ายนกหลายจุด
```cpp
// Multi-Feeder Network System
struct FeederNetwork {
  String feederId;
  String location;
  bool isOnline;
  int dailyBirds;
  float foodLevel;
};

FeederNetwork networkFeeders[10]; // Support up to 10 feeders
int networkFeederCount = 0;

void joinFeederNetwork() {
  // Register this feeder with central server
  HTTPClient http;
  http.begin("http://bird-network.rdtrc.com/api/register");
  http.addHeader("Content-Type", "application/json");
  
  StaticJsonDocument<512> registerDoc;
  registerDoc["feederId"] = DEVICE_ID;
  registerDoc["location"] = "Bangkok Park #1";
  registerDoc["version"] = FIRMWARE_VERSION;
  
  String registerData;
  serializeJson(registerDoc, registerData);
  
  int httpResponseCode = http.POST(registerData);
  if (httpResponseCode == 200) {
    DEBUG_PRINTLN("Successfully joined feeder network");
    sendLineNotification("🌐 เข้าร่วมเครือข่ายที่ให้อาหารนกแล้ว");
  }
  
  http.end();
}

void shareNetworkData() {
  // Share bird statistics with network
  StaticJsonDocument<1024> shareDoc;
  shareDoc["feederId"] = DEVICE_ID;
  shareDoc["timestamp"] = getCurrentTimestamp();
  shareDoc["dailyBirds"] = todayBirdCount;
  shareDoc["foodLevel"] = getCurrentFoodLevel();
  shareDoc["lightLevel"] = analogRead(LIGHT_SENSOR_PIN);
  
  // Add bird species data if available
  JsonArray species = shareDoc.createNestedArray("species");
  // ... add identified bird species
  
  HTTPClient http;
  http.begin("http://bird-network.rdtrc.com/api/data");
  http.addHeader("Content-Type", "application/json");
  
  String shareData;
  serializeJson(shareDoc, shareData);
  http.POST(shareData);
  http.end();
}

void getNetworkInsights() {
  // Get insights from network data
  HTTPClient http;
  http.begin("http://bird-network.rdtrc.com/api/insights/" + String(DEVICE_ID));
  int httpResponseCode = http.GET();
  
  if (httpResponseCode == 200) {
    String payload = http.getString();
    StaticJsonDocument<2048> insightsDoc;
    deserializeJson(insightsDoc, payload);
    
    // Process network insights
    String recommendation = insightsDoc["recommendation"];
    int suggestedAmount = insightsDoc["suggestedAmount"];
    String bestFeedingTime = insightsDoc["bestFeedingTime"];
    
    // Apply network recommendations
    if (suggestedAmount > 0 && suggestedAmount <= MAX_PORTION_SIZE) {
      DEFAULT_PORTION_SIZE = suggestedAmount;
      sendLineNotification("🤖 AI แนะนำปริมาณอาหาร: " + String(suggestedAmount) + "g");
    }
  }
  
  http.end();
}
```

### 📱 Mobile App Development สำหรับนก

#### React Native App เฉพาะนก
```javascript
// BirdFeedingApp.js
import React, { useState, useEffect } from 'react';
import { View, Text, StyleSheet, ScrollView } from 'react-native';
import { LineChart } from 'react-native-chart-kit';

const BirdFeedingApp = () => {
  const [birdStats, setBirdStats] = useState({
    todayVisits: 0,
    totalVisits: 0,
    peakHour: '',
    species: []
  });
  
  const [lightLevel, setLightLevel] = useState(0);
  const [feedingEnabled, setFeedingEnabled] = useState(true);
  
  useEffect(() => {
    // Connect to bird feeder WebSocket
    const ws = new WebSocket('ws://192.168.1.50/ws');
    
    ws.onmessage = (event) => {
      const data = JSON.parse(event.data);
      
      switch(data.type) {
        case 'bird_detected':
          setBirdStats(prev => ({
            ...prev,
            todayVisits: data.todayVisits
          }));
          break;
          
        case 'light_changed':
          setLightLevel(data.level);
          setFeedingEnabled(data.isDaylight);
          break;
      }
    };
    
    return () => ws.close();
  }, []);
  
  return (
    <ScrollView style={styles.container}>
      <View style={styles.header}>
        <Text style={styles.title}>🐦 Bird Feeder</Text>
        <Text style={styles.status}>
          {feedingEnabled ? '☀️ Active' : '🌙 Night Mode'}
        </Text>
      </View>
      
      <View style={styles.statsContainer}>
        <View style={styles.statBox}>
          <Text style={styles.statNumber}>{birdStats.todayVisits}</Text>
          <Text style={styles.statLabel}>Visits Today</Text>
        </View>
        
        <View style={styles.statBox}>
          <Text style={styles.statNumber}>{lightLevel}</Text>
          <Text style={styles.statLabel}>Light Level</Text>
        </View>
      </View>
      
      <View style={styles.chartContainer}>
        <Text style={styles.chartTitle}>Bird Visits (24h)</Text>
        <LineChart
          data={{
            labels: ['6', '9', '12', '15', '18'],
            datasets: [{
              data: [2, 4, 3, 5, 2]
            }]
          }}
          width={350}
          height={200}
          chartConfig={{
            backgroundColor: '#e26a00',
            backgroundGradientFrom: '#fb8c00',
            backgroundGradientTo: '#ffa726',
            decimalPlaces: 0,
            color: (opacity = 1) => `rgba(255, 255, 255, ${opacity})`
          }}
        />
      </View>
    </ScrollView>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#f5f5f5'
  },
  header: {
    padding: 20,
    backgroundColor: '#2196F3',
    alignItems: 'center'
  },
  title: {
    fontSize: 24,
    fontWeight: 'bold',
    color: 'white'
  },
  status: {
    fontSize: 16,
    color: 'white',
    marginTop: 5
  },
  statsContainer: {
    flexDirection: 'row',
    padding: 20,
    justifyContent: 'space-around'
  },
  statBox: {
    alignItems: 'center',
    backgroundColor: 'white',
    padding: 20,
    borderRadius: 10,
    minWidth: 100
  },
  statNumber: {
    fontSize: 32,
    fontWeight: 'bold',
    color: '#2196F3'
  },
  statLabel: {
    fontSize: 14,
    color: '#666',
    marginTop: 5
  },
  chartContainer: {
    padding: 20,
    backgroundColor: 'white',
    margin: 20,
    borderRadius: 10
  },
  chartTitle: {
    fontSize: 18,
    fontWeight: 'bold',
    marginBottom: 10,
    textAlign: 'center'
  }
});

export default BirdFeedingApp;
```

---

## 📞 การสนับสนุน

### 🆘 ช่องทางการติดต่อ

- **GitHub Issues**: [สร้าง Issue ใหม่](https://github.com/RDTRC/bird-feeding-system/issues)
- **Email**: bird-support@rdtrc.com
- **Discord**: RDTRC Bird Watchers Community
- **Line Official**: @rdtrc-bird-support
- **Facebook Group**: RDTRC Bird Feeding Systems Thailand

### 📚 เอกสารเพิ่มเติม

- [Bird Species Guide](./BIRD_SPECIES_GUIDE.md)
- [Weather Integration Setup](./WEATHER_SETUP.md)
- [Outdoor Installation Guide](./OUTDOOR_INSTALLATION.md)
- [Bird Photography Setup](./CAMERA_SETUP.md)
- [Network Configuration](./NETWORK_SETUP.md)

### 🤝 ชุมชนผู้ใช้งาน

เข้าร่วมชุมชนผู้ใช้งานระบบให้อาหารนก RDTRC:

- 🐦 **แบ่งปันภาพนก**: โพสต์ภาพนกที่มากินอาหาร
- 📊 **แชร์สถิติ**: เปรียบเทียบจำนวนนกในแต่ละพื้นที่
- 💡 **แลกเปลี่ยนเทคนิค**: เทคนิคการติดตั้งและดูแลระบบ
- 🌍 **การอนุรักษ์**: ร่วมกิจกรรมอนุรักษ์นกและสิ่งแวดล้อม

### 🔬 การวิจัยและพัฒนา

RDTRC ร่วมมือกับ:
- **มหาวิทยาลัยเกษตรศาสตร์**: วิจัยพฤติกรรมนก
- **สมาคมนกแห่งประเทศไทย**: ข้อมูลชนิดนกและการอนุรักษ์
- **National Geographic Thailand**: โครงการติดตามนกโยกย้าย

### 📄 ลิขสิทธิ์

```
MIT License

Copyright (c) 2024 RDTRC - Bird Conservation Technology

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

<div align="center">

## 🐦 ขอบคุณที่เลือกใช้ RDTRC Bird Feeding System!

**ระบบให้อาหารนกอัตโนมัติที่ทันสมัย | เพื่อการอนุรักษ์นกและธรรมชาติ**

[![Get Started](https://img.shields.io/badge/Get%20Started-Now-green.svg?style=for-the-badge)](#การติดตั้งซอฟต์แวร์)
[![Join Community](https://img.shields.io/badge/Join%20Community-blue.svg?style=for-the-badge)](#ชุมชนผู้ใช้งาน)
[![Support Birds](https://img.shields.io/badge/Support%20Conservation-orange.svg?style=for-the-badge)](#การวิจัยและพัฒนา)

**Made with ❤️ for Birds by RDTRC Team**

*"เทคโนโลยีเพื่อธรรมชาติ - Technology for Nature"*

</div>
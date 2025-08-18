# 🐱 ระบบให้อาหารแมวอัตโนมัติ - เอกสารสมบูรณ์
## RDTRC Cat Feeding System - Complete Documentation

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

ระบบให้อาหารแมวอัตโนมัติ RDTRC เป็นระบบ IoT ที่สมบูรณ์แบบสำหรับการให้อาหารแมวแบบอัตโนมัติ ด้วยการควบคุมผ่านมือถือและเว็บอินเตอร์เฟซ

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
         │        (WiFi + Hotspot Mode)                  │
         └───────────────────────┬───────────────────────┘
                                 │
    ┌────────────────────────────┼────────────────────────────┐
    │                            │                            │
    ▼                            ▼                            ▼
┌─────────┐              ┌─────────────┐              ┌─────────────┐
│Sensors  │              │ Actuators   │              │ Display &   │
│- HX711  │              │ - Servo     │              │ Interface   │
│- PIR    │              │ - Buzzer    │              │ - LCD 16x2  │
│- Ultra  │              │ - LED       │              │ - Buttons   │
└─────────┘              └─────────────┘              └─────────────┘
```

---

## 🌟 คุณสมบัติเด่น

### ⏰ การให้อาหารอัตโนมัติ
- **ตารางเวลาที่ยืดหยุ่น**: ตั้งได้สูงสุด 6 เวลาต่อวัน
- **ปรับปริมาณได้**: 5-100 กรัมต่อครั้ง
- **ป้องกันการให้ซ้ำ**: ระบบป้องกันการให้อาหารซ้ำในนาทีเดียวกัน
- **การให้อาหารด้วยตนเอง**: กดปุ่มหรือควบคุมผ่านแอป

### 📱 ควบคุมผ่านมือถือ
- **Blynk IoT Platform**: ควบคุมและติดตามผ่านแอปมือถือ
- **Web Interface**: เข้าถึงผ่านเบราว์เซอร์
- **แจ้งเตือนแบบเรียลไทม์**: แจ้งเมื่อให้อาหาร ตรวจพบแมว หรืออาหารหมด
- **การตั้งค่าระยะไกล**: เปลี่ยนตารางเวลาและปริมาณได้ทุกที่

### 🔍 เซ็นเซอร์อัจฉริยะ
- **วัดน้ำหนักอาหาร**: เซ็นเซอร์ Load Cell ความแม่นยำสูง
- **ตรวจจับแมว**: PIR Sensor ตรวจจับการเคลื่อนไหว
- **ตรวจระดับอาหาร**: Ultrasonic Sensor วัดระดับอาหารในถัง
- **แสดงผลข้อมูล**: LCD 16x2 แสดงสถานะปัจจุบัน

### 🛡️ ความปลอดภัย
- **ระบบป้องกันการให้มากเกินไป**: จำกัดปริมาณสูงสุดต่อวัน
- **แจ้งเตือนอาหารหมด**: เตือนเมื่อน้ำหนักต่ำกว่าที่กำหนด
- **การทำงานแบบออฟไลน์**: ใช้งานได้แม้ไม่มีอินเทอร์เน็ต
- **โหมด Hotspot**: สร้าง WiFi ของตัวเองเมื่อเชื่อมต่อไม่ได้

---

## 🔧 ข้อมูลทางเทคนิค

### 📊 ข้อกำหนดระบบ

| ข้อมูล | รายละเอียด |
|--------|------------|
| **ไมโครคอนโทรลเลอร์** | ESP32 Development Board |
| **แรงดันไฟฟ้า** | 5V DC / 2A |
| **การเชื่อมต่อ** | WiFi 802.11 b/g/n |
| **ความจุหน่วยความจำ** | 4MB Flash, 320KB RAM |
| **ความแม่นยำการชั่ง** | ±1 กรัม |
| **ขนาดปริมาณอาหาร** | 5-100 กรัม |
| **ระยะตรวจจับแมว** | 0-7 เมตร |
| **อุณหภูมิการทำงาน** | 0-60°C |
| **ความชื้นสัมพัทธ์** | 10-90% |

### 🎛️ การกำหนดค่า Pin

```cpp
// Pin Definitions
#define SERVO_PIN 18                    // เซอร์โวมอเตอร์
#define LOAD_CELL_DOUT_PIN 19          // HX711 Data
#define LOAD_CELL_SCK_PIN 5            // HX711 Clock
#define PIR_SENSOR_PIN 22              // PIR Motion Sensor
#define ULTRASONIC_TRIG_PIN 23         // Ultrasonic Trigger
#define ULTRASONIC_ECHO_PIN 25         // Ultrasonic Echo
#define BUZZER_PIN 4                   // Buzzer
#define STATUS_LED_PIN 2               // Status LED
#define RESET_BUTTON_PIN 0             // Reset Button
#define MANUAL_FEED_BUTTON_PIN 27      // Manual Feed Button
#define LCD_NEXT_BUTTON_PIN 26         // LCD Navigation Button

// I2C Pins for LCD
#define I2C_SDA 21                     // I2C Data Line
#define I2C_SCL 22                     // I2C Clock Line
```

---

## 🛍️ รายการอุปกรณ์

### 📋 อุปกรณ์หลัก

| อุปกรณ์ | จำนวน | ราคาประมาณ | รายละเอียด |
|---------|--------|-------------|------------|
| **ESP32 Development Board** | 1 | ฿350-500 | ตัวควบคุมหลัก |
| **HX711 Load Cell Amplifier** | 1 | ฿120-180 | วงจรขยายสัญญาณเซ็นเซอร์ชั่งน้ำหนัก |
| **Load Cell 5kg** | 1 | ฿200-300 | เซ็นเซอร์ชั่งน้ำหนัก |
| **SG90 Servo Motor** | 1 | ฿80-120 | มอเตอร์ควบคุมการให้อาหาร |
| **PIR Motion Sensor** | 1 | ฿60-100 | เซ็นเซอร์ตรวจจับการเคลื่อนไหว |
| **HC-SR04 Ultrasonic** | 1 | ฿50-80 | เซ็นเซอร์วัดระยะทาง |
| **LCD 16x2 I2C** | 1 | ฿120-180 | หน้าจอแสดงผล |
| **Buzzer Module** | 1 | ฿30-50 | เสียงแจ้งเตือน |
| **LED Module** | 1 | ฿20-40 | ไฟแสดงสถานะ |
| **Push Button** | 3 | ฿30-60 | ปุ่มควบคุม |

### 🔌 อุปกรณ์เสริม

| อุปกรณ์ | จำนวน | ราคาประมาณ | รายละเอียด |
|---------|--------|-------------|------------|
| **Power Supply 5V/2A** | 1 | ฿150-250 | แหล่งจ่ายไฟ |
| **Jumper Wires** | 1 ชุด | ฿50-100 | สายจัมเปอร์ |
| **Breadboard** | 1 | ฿80-150 | เบรดบอร์ด |
| **Resistors (10kΩ)** | 3 | ฿10-20 | ตัวต้านทาน |
| **Capacitors (100μF)** | 2 | ฿20-40 | ตัวเก็บประจุ |
| **Plastic Enclosure** | 1 | ฿200-400 | กล่องบรรจุ |

### 💰 สรุปค่าใช้จ่าย

| ประเภท | ราคา (ต่ำสุด) | ราคา (สูงสุด) |
|--------|---------------|---------------|
| **อุปกรณ์หลัก** | ฿1,060 | ฿1,650 |
| **อุปกรณ์เสริม** | ฿510 | ฿960 |
| **รวม** | **฿1,570** | **฿2,610** |

---

## 🔌 การเชื่อมต่อวงจร

### 📐 Wiring Diagram

```
ESP32 Pin    │ Component      │ Description
─────────────┼────────────────┼─────────────────────
3.3V         │ LCD VCC        │ Power for LCD I2C
GND          │ LCD GND        │ Ground for LCD
GPIO21       │ LCD SDA        │ I2C Data Line
GPIO22       │ LCD SCL        │ I2C Clock Line
             │                │
GPIO18       │ Servo Signal   │ Servo Motor Control
5V           │ Servo VCC      │ Power for Servo
GND          │ Servo GND      │ Ground for Servo
             │                │
GPIO19       │ HX711 DT       │ Load Cell Data
GPIO5        │ HX711 SCK      │ Load Cell Clock
5V           │ HX711 VCC      │ Power for HX711
GND          │ HX711 GND      │ Ground for HX711
             │                │
GPIO22       │ PIR Signal     │ Motion Detection
5V           │ PIR VCC        │ Power for PIR
GND          │ PIR GND        │ Ground for PIR
             │                │
GPIO23       │ Ultra Trig     │ Ultrasonic Trigger
GPIO25       │ Ultra Echo     │ Ultrasonic Echo
5V           │ Ultra VCC      │ Power for Ultrasonic
GND          │ Ultra GND      │ Ground for Ultrasonic
             │                │
GPIO4        │ Buzzer +       │ Buzzer Positive
GND          │ Buzzer -       │ Buzzer Negative
             │                │
GPIO2        │ LED +          │ Status LED Positive
GND          │ LED -          │ LED Negative (with 220Ω resistor)
             │                │
GPIO0        │ Reset Button   │ System Reset
GPIO27       │ Manual Feed    │ Manual Feed Button
GPIO26       │ LCD Next       │ LCD Navigation Button
```

### ⚡ การต่อไฟฟ้า

```
Power Distribution:
┌─────────────┐
│ 5V/2A PSU   │
└─────┬───────┘
      │
      ├── ESP32 (5V/VIN)
      ├── Servo Motor (5V)
      ├── HX711 Module (5V)
      ├── PIR Sensor (5V)
      ├── Ultrasonic Sensor (5V)
      └── LCD Module (3.3V from ESP32)

Ground Distribution:
All GND pins connected to common ground rail
```

---

## 💻 การติดตั้งซอฟต์แวร์

### 🔧 Arduino IDE Setup

1. **ติดตั้ง Arduino IDE**
   ```
   ดาวน์โหลดจาก: https://www.arduino.cc/en/software
   ```

2. **เพิ่ม ESP32 Board Manager**
   ```
   File → Preferences → Additional Board Manager URLs
   เพิ่ม: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```

3. **ติดตั้ง ESP32 Board**
   ```
   Tools → Board → Boards Manager
   ค้นหา "ESP32" และติดตั้ง
   ```

### 📚 ไลบรารีที่จำเป็น

```cpp
// Core Libraries
#include <WiFi.h>              // Built-in
#include <WebServer.h>         // Built-in
#include <ArduinoJson.h>       // Install via Library Manager
#include <SPIFFS.h>            // Built-in

// Hardware Libraries
#include <HX711.h>             // Install: "HX711 Arduino Library"
#include <ESP32Servo.h>        // Install: "ESP32Servo"
#include <LiquidCrystal_I2C.h> // Install: "LiquidCrystal I2C"

// IoT Libraries
#include <BlynkSimpleEsp32.h>  // Install: "Blynk"
#include <NTPClient.h>         // Install: "NTPClient"
#include <WiFiUdp.h>           // Built-in

// Additional Libraries
#include <HTTPClient.h>        // Built-in
#include <ArduinoOTA.h>        // Built-in
#include <ESPmDNS.h>           // Built-in
```

### 📦 การติดตั้งไลบรารี

```bash
Arduino IDE → Tools → Manage Libraries

ติดตั้งไลบรารีต่อไปนี้:
1. ArduinoJson (version 6.x)
2. HX711 Arduino Library
3. ESP32Servo
4. LiquidCrystal I2C
5. Blynk
6. NTPClient
```

---

## ⚙️ การตั้งค่าระบบ

### 🌐 การตั้งค่า WiFi

```cpp
// Network Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* hotspot_ssid = "RDTRC_CatFeeder";
const char* hotspot_password = "rdtrc123";
```

### 📱 การตั้งค่า Blynk

```cpp
// Blynk Configuration
#define BLYNK_TEMPLATE_ID "TMPL61Zdwsx9r"
#define BLYNK_TEMPLATE_NAME "Cat_Feeding_System"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"
```

### 🔔 การตั้งค่า LINE Notify

```cpp
// LINE Notify Configuration
const char* lineToken = "YOUR_LINE_NOTIFY_TOKEN";
```

### 🎛️ การปรับแต่งระบบ

```cpp
// Feeding Configuration
#define DEFAULT_PORTION_SIZE 30  // กรัม
#define MIN_PORTION_SIZE 5
#define MAX_PORTION_SIZE 100
#define FOOD_CONTAINER_HEIGHT 20  // ซม.
#define LOW_FOOD_THRESHOLD 3      // ซม.
#define EMPTY_BOWL_THRESHOLD 5    // กรัม
```

---

## 🎮 การใช้งาน

### 🚀 การเริ่มต้นใช้งาน

1. **เชื่อมต่อไฟฟ้า**
   - ตรวจสอบการต่อวงจรให้ถูกต้อง
   - เสียบปลั๊กไฟ

2. **Boot Sequence**
   ```
   ╔════════════════╗
   ║ FW make by RDTRC║
   ║ Version 4.0     ║
   ╚════════════════╝
           ↓
   ╔════════════════╗
   ║ Cat Feeder     ║
   ║ Initializing...║
   ╚════════════════╝
           ↓
   ╔════════════════╗
   ║ WiFi: Connecting║
   ║ IP: 192.168.1.x║
   ╚════════════════╝
           ↓
   ╔════════════════╗
   ║ System Ready   ║
   ║ Next: 08:00    ║
   ╚════════════════╝
   ```

3. **การเชื่อมต่อ WiFi**
   - ระบบจะพยายามเชื่อมต่อ WiFi อัตโนมัติ
   - หากไม่สำเร็จจะสร้าง Hotspot "RDTRC_CatFeeder"

### 📱 การควบคุมผ่าน LCD

#### 🖥️ หน้าจอ LCD

**หน้าหลัก (Main Screen)**
```
┌────────────────┐
│Cat Feeder v4.0 │
│Next: 08:00 30g │
└────────────────┘
```

**หน้าสถานะ (Status Screen)**
```
┌────────────────┐
│Food: 2.5kg     │
│Cat: Detected   │
└────────────────┘
```

**หน้าเครือข่าย (Network Screen)**
```
┌────────────────┐
│WiFi: Connected │
│IP: 192.168.1.50│
└────────────────┘
```

#### 🎛️ การใช้ปุ่ม

| ปุ่ม | การทำงาน |
|------|----------|
| **Manual Feed** | ให้อาหารทันที |
| **LCD Next** | เปลี่ยนหน้าจอ LCD |
| **Reset** | รีสตาร์ทระบบ |

### ⏰ การตั้งเวลาให้อาหาร

```cpp
// Default Feeding Schedule
feedingTimes[0] = {6, 0, 30};   // 06:00 น. 30g
feedingTimes[1] = {8, 0, 30};   // 08:00 น. 30g
feedingTimes[2] = {12, 0, 30};  // 12:00 น. 30g
feedingTimes[3] = {16, 0, 30};  // 16:00 น. 30g
feedingTimes[4] = {18, 0, 30};  // 18:00 น. 30g
feedingTimes[5] = {20, 0, 30};  // 20:00 น. 30g
```

---

## 🌐 Web Interface

### 🏠 หน้าหลัก (Dashboard)

เข้าถึงได้ที่: `http://[ESP32_IP_ADDRESS]/`

**คุณสมบัติหลัก:**
- แสดงสถานะระบบแบบเรียลไทม์
- ควบคุมการให้อาหารด้วยตนเอง
- ตั้งค่าตารางเวลา
- ดูประวัติการให้อาหาร

### 📊 หน้าต่างๆ ของ Web Interface

#### 1. **Dashboard (หน้าหลัก)**
```html
┌─────────────────────────────────────┐
│ 🐱 RDTRC Cat Feeding System        │
├─────────────────────────────────────┤
│ Status: ● Online                    │
│ Next Feeding: 08:00 (30g)          │
│ Food Level: 2.5kg                   │
│ Cat Detected: Yes                   │
│                                     │
│ [Feed Now] [Settings] [History]     │
└─────────────────────────────────────┘
```

#### 2. **Settings (การตั้งค่า)**
```html
┌─────────────────────────────────────┐
│ ⚙️ System Settings                  │
├─────────────────────────────────────┤
│ Feeding Schedule:                   │
│ Time 1: [06:00] Amount: [30]g       │
│ Time 2: [08:00] Amount: [30]g       │
│ Time 3: [12:00] Amount: [30]g       │
│                                     │
│ Notifications:                      │
│ ☑ Low Food Alert                   │
│ ☑ Feeding Notifications            │
│                                     │
│ [Save Settings] [Reset]             │
└─────────────────────────────────────┘
```

#### 3. **History (ประวัติ)**
```html
┌─────────────────────────────────────┐
│ 📊 Feeding History                  │
├─────────────────────────────────────┤
│ 2024-12-19 08:00 - 30g ✅          │
│ 2024-12-19 06:00 - 30g ✅          │
│ 2024-12-18 20:00 - 30g ✅          │
│ 2024-12-18 18:00 - 30g ✅          │
│                                     │
│ Total Today: 180g                   │
│ Average: 30g per feeding            │
│                                     │
│ [Export Data] [Clear History]       │
└─────────────────────────────────────┘
```

### 🔗 API Endpoints

```javascript
// System Status
GET /api/status
Response: {
  "status": "online",
  "nextFeeding": "08:00",
  "foodLevel": 2500,
  "catDetected": true,
  "uptime": 86400
}

// Manual Feeding
POST /api/feed
Body: {"amount": 30}
Response: {"success": true, "message": "Feeding completed"}

// Get Settings
GET /api/settings
Response: {
  "feedingTimes": [
    {"hour": 6, "minute": 0, "amount": 30},
    {"hour": 8, "minute": 0, "amount": 30}
  ],
  "notifications": {
    "lowFood": true,
    "feeding": true
  }
}

// Update Settings
POST /api/settings
Body: {
  "feedingTimes": [...],
  "notifications": {...}
}
```

---

## 📱 Blynk App Integration

### 🔧 การตั้งค่า Blynk App

1. **ดาวน์โหลด Blynk App**
   - iOS: App Store
   - Android: Google Play Store

2. **สร้างโปรเจค**
   - เลือก "ESP32"
   - ใส่ชื่อโปรเจค "Cat Feeding System"

3. **เพิ่ม Widgets**

#### 📊 Widget Layout

```
┌─────────────────────────────────────┐
│ 🐱 Cat Feeding System               │
├─────────────────────────────────────┤
│ ● Status LED (V0)                   │
│   ○ Green: Online                   │
│   ○ Red: Offline                    │
├─────────────────────────────────────┤
│ 📊 Food Level (V1)                  │
│   [████████░░] 80%                  │
├─────────────────────────────────────┤
│ ⏰ Next Feeding (V2)                │
│   08:00 - 30g                       │
├─────────────────────────────────────┤
│ 🐱 Cat Detection (V3)               │
│   ● Detected: Yes                   │
├─────────────────────────────────────┤
│ [Feed Now] Button (V4)              │
├─────────────────────────────────────┤
│ 🎚️ Portion Size (V5)               │
│   [5g ●────────── 100g]            │
├─────────────────────────────────────┤
│ 📊 Daily Total (V6)                 │
│   Today: 180g                       │
└─────────────────────────────────────┘
```

#### 🎛️ Virtual Pins

| Virtual Pin | Widget | Function |
|-------------|--------|----------|
| V0 | LED | System Status |
| V1 | Gauge | Food Level |
| V2 | Value Display | Next Feeding Time |
| V3 | LED | Cat Detection |
| V4 | Button | Manual Feed |
| V5 | Slider | Portion Size |
| V6 | Value Display | Daily Total |
| V7 | Terminal | System Messages |
| V8 | Time Input | Schedule Settings |

---

## 🔌 API Reference

### 📡 REST API Endpoints

#### 1. **System Status**
```http
GET /api/status
Content-Type: application/json

Response:
{
  "system": {
    "name": "Cat Feeder",
    "version": "4.0",
    "uptime": 86400,
    "freeHeap": 234567,
    "chipId": "ABC123"
  },
  "network": {
    "wifi": {
      "connected": true,
      "ssid": "MyWiFi",
      "ip": "192.168.1.50",
      "rssi": -45
    },
    "hotspot": {
      "active": false
    }
  },
  "feeding": {
    "nextTime": "08:00",
    "nextAmount": 30,
    "lastFed": "06:00",
    "dailyTotal": 180
  },
  "sensors": {
    "foodLevel": 2500,
    "catDetected": true,
    "bowlWeight": 25
  }
}
```

#### 2. **Manual Feeding**
```http
POST /api/feed
Content-Type: application/json

Request Body:
{
  "amount": 30,
  "force": false
}

Response:
{
  "success": true,
  "message": "Feeding completed successfully",
  "timestamp": "2024-12-19T08:00:00Z",
  "amount": 30,
  "remainingFood": 2470
}
```

#### 3. **Schedule Management**
```http
GET /api/schedule
Response:
{
  "schedule": [
    {"id": 1, "hour": 6, "minute": 0, "amount": 30, "enabled": true},
    {"id": 2, "hour": 8, "minute": 0, "amount": 30, "enabled": true},
    {"id": 3, "hour": 12, "minute": 0, "amount": 30, "enabled": true}
  ],
  "timezone": "Asia/Bangkok"
}

POST /api/schedule
Request Body:
{
  "schedule": [
    {"hour": 6, "minute": 0, "amount": 30, "enabled": true}
  ]
}
```

#### 4. **Settings Management**
```http
GET /api/settings
Response:
{
  "feeding": {
    "minPortion": 5,
    "maxPortion": 100,
    "defaultPortion": 30,
    "maxDailyAmount": 300
  },
  "notifications": {
    "lowFood": true,
    "feeding": true,
    "catDetection": false,
    "lineNotify": true
  },
  "sensors": {
    "pirSensitivity": 5,
    "scaleCalibration": 2280.0,
    "ultrasonicOffset": 2
  }
}

POST /api/settings
Request Body: (same structure as GET response)
```

#### 5. **History Data**
```http
GET /api/history?days=7
Response:
{
  "history": [
    {
      "timestamp": "2024-12-19T08:00:00Z",
      "type": "scheduled",
      "amount": 30,
      "success": true,
      "catPresent": true
    }
  ],
  "summary": {
    "totalFeedings": 24,
    "totalAmount": 720,
    "averagePerFeeding": 30,
    "successRate": 100
  }
}
```

### 📊 WebSocket Events

```javascript
// Connect to WebSocket
const ws = new WebSocket('ws://192.168.1.50/ws');

// Event Types
ws.onmessage = function(event) {
  const data = JSON.parse(event.data);
  
  switch(data.type) {
    case 'status_update':
      // Real-time status updates
      break;
    case 'feeding_started':
      // Feeding process started
      break;
    case 'feeding_completed':
      // Feeding process completed
      break;
    case 'cat_detected':
      // Cat motion detected
      break;
    case 'low_food_alert':
      // Food level is low
      break;
    case 'error':
      // System error occurred
      break;
  }
};
```

---

## 🛠️ การแก้ปัญหา

### ❌ ปัญหาที่พบบ่อย

#### 1. **ไม่สามารถเชื่อมต่อ WiFi ได้**

**อาการ:**
- LCD แสดง "WiFi: Failed"
- ระบบสร้าง Hotspot

**การแก้ไข:**
```cpp
// ตรวจสอบการตั้งค่า WiFi
const char* ssid = "YOUR_ACTUAL_WIFI_NAME";
const char* password = "YOUR_ACTUAL_WIFI_PASSWORD";

// ลองรีเซ็ต WiFi credentials
void resetWiFiSettings() {
  WiFi.disconnect(true);
  delay(1000);
  WiFi.begin(ssid, password);
}
```

#### 2. **เซ็นเซอร์ชั่งน้ำหนักไม่แม่นยำ**

**อาการ:**
- น้ำหนักแสดงผลผิดพลาด
- การชั่งไม่สม่ำเสมอ

**การแก้ไข:**
```cpp
// Calibrate Load Cell
void calibrateScale() {
  scale.set_scale();
  scale.tare();  // Reset to zero
  
  // Put known weight (e.g., 100g) and adjust
  float calibration_factor = 2280.0; // Adjust this value
  scale.set_scale(calibration_factor);
}
```

#### 3. **PIR Sensor ทำงานผิดพลาด**

**อาการ:**
- ตรวจจับแมวตลอดเวลา
- ไม่ตรวจจับแมวเลย

**การแก้ไข:**
```cpp
// Adjust PIR sensitivity
#define PIR_DELAY 5000  // 5 seconds delay
#define PIR_THRESHOLD 3 // Minimum detections

bool catDetected = false;
int pirCount = 0;

void checkPIR() {
  if (digitalRead(PIR_SENSOR_PIN)) {
    pirCount++;
    if (pirCount >= PIR_THRESHOLD) {
      catDetected = true;
      pirCount = 0;
    }
  } else {
    if (pirCount > 0) pirCount--;
  }
}
```

#### 4. **Servo Motor ไม่หมุน**

**อาการ:**
- มอเตอร์ไม่เคลื่อนไหว
- เสียงแปลกๆ จากมอเตอร์

**การแก้ไข:**
```cpp
// Check servo connections and power
void testServo() {
  feedingServo.attach(SERVO_PIN);
  
  // Test movement
  feedingServo.write(0);
  delay(1000);
  feedingServo.write(90);
  delay(1000);
  feedingServo.write(0);
  
  feedingServo.detach();
}
```

### 🔍 การ Debug

#### 📊 Serial Monitor Output

```
RDTRC Cat Feeding System v4.0
==============================
[INIT] Initializing SPIFFS... OK
[INIT] Initializing LCD... OK
[INIT] Initializing Scale... OK
[INIT] Initializing Servo... OK
[INIT] Initializing Sensors... OK

[WIFI] Connecting to MyWiFi...
[WIFI] Connected! IP: 192.168.1.50
[WEB]  Web server started
[BLYNK] Connecting to Blynk...
[BLYNK] Connected!

[SYSTEM] Ready!
[FEED] Next feeding: 08:00 (30g)
[SENSOR] Food level: 2.5kg
[SENSOR] Cat detected: No

Loop running... Free heap: 234567
```

#### 🐛 Debug Commands

```cpp
// Enable debug mode
#define DEBUG_MODE 1

#if DEBUG_MODE
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// Debug functions
void printSystemStatus() {
  DEBUG_PRINTLN("=== System Status ===");
  DEBUG_PRINT("Free Heap: "); DEBUG_PRINTLN(ESP.getFreeHeap());
  DEBUG_PRINT("Uptime: "); DEBUG_PRINTLN(millis());
  DEBUG_PRINT("WiFi RSSI: "); DEBUG_PRINTLN(WiFi.RSSI());
  DEBUG_PRINT("Food Level: "); DEBUG_PRINTLN(getCurrentFoodLevel());
}
```

### ⚡ การรีเซ็ตระบบ

```cpp
// Factory Reset Function
void factoryReset() {
  // Clear SPIFFS
  SPIFFS.format();
  
  // Reset WiFi settings
  WiFi.disconnect(true);
  
  // Reset EEPROM
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  
  // Restart system
  ESP.restart();
}
```

---

## 🔧 การบำรุงรักษา

### 📅 การบำรุงรักษาประจำ

#### รายวัน
- ✅ ตรวจสอบระดับอาหารในถัง
- ✅ ทำความสะอาดถ้วยอาหาร
- ✅ ตรวจสอบการทำงานของระบบผ่าน LCD

#### รายสัปดาห์
- ✅ ทำความสะอาด Load Cell และ Servo
- ✅ ตรวจสอบการเชื่อมต่อสายไฟ
- ✅ อัพเดทข้อมูลผ่าน Web Interface

#### รายเดือน
- ✅ ปรับเทียบเซ็นเซอร์ชั่งน้ำหนัก
- ✅ ตรวจสอบการทำงานของ PIR Sensor
- ✅ สำรองข้อมูลประวัติการให้อาหาร

### 🧹 การทำความสะอาด

```cpp
// Cleaning Mode Function
void enterCleaningMode() {
  systemLCD.clear();
  systemLCD.print("Cleaning Mode");
  systemLCD.setCursor(0, 1);
  systemLCD.print("System Disabled");
  
  // Disable all automated functions
  feedingEnabled = false;
  sensorReadingEnabled = false;
  
  // Move servo to cleaning position
  feedingServo.attach(SERVO_PIN);
  feedingServo.write(180); // Open position
  delay(2000);
  feedingServo.detach();
}
```

### 📊 การสำรองข้อมูล

```cpp
// Data Backup Function
void backupData() {
  File backupFile = SPIFFS.open("/backup.json", "w");
  if (backupFile) {
    StaticJsonDocument<2048> backupDoc;
    
    // Backup feeding history
    JsonArray history = backupDoc.createNestedArray("history");
    // Add history data...
    
    // Backup settings
    JsonObject settings = backupDoc.createNestedObject("settings");
    // Add settings data...
    
    serializeJson(backupDoc, backupFile);
    backupFile.close();
    
    DEBUG_PRINTLN("Data backup completed");
  }
}
```

---

## 🚀 การพัฒนาต่อ

### 💡 ไอเดียการพัฒนา

#### 📷 การเพิ่ม ESP32-CAM
```cpp
// ESP32-CAM Integration
#include "esp_camera.h"

void initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  // ... camera configuration
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    DEBUG_PRINTLN("Camera init failed");
    return;
  }
}

void takeFeedingPhoto() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (fb) {
    // Save photo or send to server
    esp_camera_fb_return(fb);
  }
}
```

#### 🎵 Voice Control
```cpp
// Voice Command Processing
void processVoiceCommand(String command) {
  command.toLowerCase();
  
  if (command.indexOf("feed cat") != -1) {
    manualFeed(DEFAULT_PORTION_SIZE);
  } else if (command.indexOf("status") != -1) {
    speakStatus();
  } else if (command.indexOf("schedule") != -1) {
    speakSchedule();
  }
}
```

#### 🤖 AI Recommendations
```cpp
// AI-based feeding recommendations
struct AIRecommendation {
  int recommendedAmount;
  String reason;
  float confidence;
};

AIRecommendation getAIRecommendation() {
  AIRecommendation rec;
  
  // Analyze cat behavior patterns
  float avgConsumption = calculateAvgConsumption(7); // 7 days
  int catVisits = getCatVisitCount(24); // 24 hours
  
  if (avgConsumption < 20 && catVisits > 10) {
    rec.recommendedAmount = 25;
    rec.reason = "Cat visits frequently but eats less";
    rec.confidence = 0.85;
  }
  
  return rec;
}
```

### 🔌 การขยายระบบ

#### Multi-Pet Support
```cpp
// Multiple Pet Management
struct PetProfile {
  String name;
  int chipId;  // RFID chip
  int dailyAmount;
  bool isActive;
};

PetProfile pets[4]; // Support up to 4 pets

void identifyPet() {
  int chipId = readRFID();
  for (int i = 0; i < 4; i++) {
    if (pets[i].chipId == chipId && pets[i].isActive) {
      currentPet = i;
      break;
    }
  }
}
```

#### Weather Integration
```cpp
// Weather-based feeding adjustments
void adjustFeedingForWeather() {
  String weather = getWeatherData();
  
  if (weather.indexOf("cold") != -1) {
    // Increase portion size in cold weather
    for (int i = 0; i < 6; i++) {
      feedingTimes[i].amount += 5;
    }
  } else if (weather.indexOf("hot") != -1) {
    // Decrease portion size in hot weather
    for (int i = 0; i < 6; i++) {
      feedingTimes[i].amount -= 3;
    }
  }
}
```

### 📱 Mobile App Development

#### React Native App Structure
```javascript
// App.js
import React from 'react';
import { NavigationContainer } from '@react-navigation/native';
import { createStackNavigator } from '@react-navigation/stack';

import DashboardScreen from './screens/DashboardScreen';
import SettingsScreen from './screens/SettingsScreen';
import HistoryScreen from './screens/HistoryScreen';

const Stack = createStackNavigator();

export default function App() {
  return (
    <NavigationContainer>
      <Stack.Navigator initialRouteName="Dashboard">
        <Stack.Screen name="Dashboard" component={DashboardScreen} />
        <Stack.Screen name="Settings" component={SettingsScreen} />
        <Stack.Screen name="History" component={HistoryScreen} />
      </Stack.Navigator>
    </NavigationContainer>
  );
}
```

---

## 📞 การสนับสนุน

### 🆘 ช่องทางการติดต่อ

- **GitHub Issues**: [สร้าง Issue ใหม่](https://github.com/RDTRC/cat-feeding-system/issues)
- **Email**: support@rdtrc.com
- **Discord**: RDTRC Community Server
- **Line Official**: @rdtrc-support

### 📚 เอกสารเพิ่มเติม

- [Installation Guide](./INSTALLATION_GUIDE.md)
- [Troubleshooting Guide](./TROUBLESHOOTING.md)
- [API Documentation](./API_DOCS.md)
- [Hardware Assembly Guide](./HARDWARE_GUIDE.md)

### 🤝 การมีส่วนร่วม

เราต้อนรับการมีส่วนร่วมจากชุมชน! สามารถช่วยได้ในด้าน:

- 🔧 **Code Development**: ปรับปรุงโค้ดและเพิ่มฟีเจอร์
- 📝 **Documentation**: เขียนเอกสารและคู่มือ
- 🐛 **Testing**: ทดสอบและรายงานปัญหา
- 🎨 **Design**: ออกแบบ UI/UX
- 🌐 **Translation**: แปลเอกสารเป็นภาษาอื่น

### 📄 ลิขสิทธิ์

```
MIT License

Copyright (c) 2024 RDTRC

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

## 🎉 ขอบคุณที่เลือกใช้ RDTRC Cat Feeding System!

**ระบบให้อาหารแมวอัตโนมัติที่ทันสมัย | สร้างโดย RDTRC**

[![Get Started](https://img.shields.io/badge/Get%20Started-Now-green.svg?style=for-the-badge)](#การติดตั้งซอฟต์แวร์)
[![Documentation](https://img.shields.io/badge/Read%20Docs-blue.svg?style=for-the-badge)](#สารบัญ)
[![Support](https://img.shields.io/badge/Get%20Support-orange.svg?style=for-the-badge)](#การสนับสนุน)

**Made with ❤️ by RDTRC Team**

</div>
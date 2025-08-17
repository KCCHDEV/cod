# 🐱 ระบบให้อาหารแมวอัตโนมัติ
## Automatic Cat Feeding System

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)](https://www.espressif.com/)
[![Blynk](https://img.shields.io/badge/Blynk-IoT-orange.svg)](https://blynk.io/)

> ระบบให้อาหารแมวอัตโนมัติที่ทันสมัย ควบคุมผ่านมือถือ พร้อมเซ็นเซอร์ตรวจจับแมวและวัดน้ำหนักอาหาร

---

## 📋 สารบัญ

- [คุณสมบัติเด่น](#คุณสมบัติเด่น)
- [ภาพรวมระบบ](#ภาพรวมระบบ)
- [รายการอุปกรณ์](#รายการอุปกรณ์)
- [การติดตั้งเบื้องต้น](#การติดตั้งเบื้องต้น)
- [การใช้งาน](#การใช้งาน)
- [API Reference](#api-reference)
- [การแก้ปัญหา](#การแก้ปัญหา)
- [การพัฒนาต่อ](#การพัฒนาต่อ)
- [การสนับสนุน](#การสนับสนุน)
- [ลิขสิทธิ์](#ลิขสิทธิ์)

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
- **นาฬิกาเรียลไทม์**: RTC DS3231 แม่นยำ
- **แสดงผลข้อมูล**: LCD 16x2 แสดงสถานะปัจจุบัน

### 🛡️ ความปลอดภัย
- **ระบบป้องกันการให้มากเกินไป**: จำกัดปริมาณสูงสุดต่อวัน
- **แจ้งเตือนอาหารหมด**: เตือนเมื่อน้ำหนักต่ำกว่าที่กำหนด
- **การทำงานแบบ Fail-Safe**: ปุ่มฉุกเฉินและโหมดด้วยตนเอง
- **บันทึกประวัติ**: เก็บข้อมูลการให้อาหารใน EEPROM

### 🎨 ส่วนต่อประสานที่ใช้งานง่าย
- **LED RGB**: แสดงสถานะด้วยสี (เขียว=ปกติ, แดง=ข้อผิดพลาด, น้ำเงิน=ตรวจพบแมว)
- **เสียงแจ้งเตือน**: Buzzer แจ้งเตือนก่อนให้อาหาร
- **ปุ่มควบคุม**: ปุ่มให้อาหารด้วยตนเองและรีเซ็ตระบบ
- **หน้าเว็บที่สวยงาม**: UI ที่ตอบสนองและใช้งานง่าย

---

## 🏗️ ภาพรวมระบบ

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Mobile App    │    │   Web Browser   │    │  Physical Ctrl  │
│   (Blynk IoT)   │    │  (HTTP Server)  │    │   (Buttons)     │
└─────────┬───────┘    └─────────┬───────┘    └─────────┬───────┘
          │                      │                      │
          └──────────────────────┼──────────────────────┘
                                 │
                     ┌───────────▼───────────┐
                     │      ESP32 MCU       │
                     │   (Main Controller)   │
                     └───────────┬───────────┘
                                 │
        ┌────────────────────────┼────────────────────────┐
        │                        │                        │
┌───────▼───────┐    ┌───────────▼───────────┐    ┌───────▼───────┐
│   Sensors     │    │      Actuators        │    │   Display     │
│               │    │                       │    │               │
│• Load Cell    │    │• Servo Motor          │    │• LCD 16x2     │
│• PIR Motion   │    │• Buzzer               │    │• RGB LED      │
│• RTC Clock    │    │• RGB LED Status       │    │               │
└───────────────┘    └───────────────────────┘    └───────────────┘
```

### การทำงานของระบบ

1. **การตั้งเวลา**: RTC ติดตามเวลาปัจจุบันและเปรียบเทียบกับตารางที่กำหนด
2. **การตรวจสอบอาหาร**: Load Cell วัดน้ำหนักอาหารอย่างต่อเนื่อง
3. **การตรวจจับแมว**: PIR Sensor ตรวจจับการเคลื่อนไหวของแมว
4. **การให้อาหาร**: Servo Motor เปิด-ปิดช่องให้อาหารตามปริมาณที่กำหนด
5. **การแจ้งเตือน**: ส่งข้อมูลผ่าน WiFi ไปยัง Blynk และ Web Interface
6. **การบันทึก**: เก็บสถิติและการตั้งค่าใน EEPROM

---

## 📦 รายการอุปกรณ์

### 🔧 อุปกรณ์หลัก (Required)

| อุปกรณ์ | จำนวน | ราคาประมาณ | หมายเหตุ |
|---------|--------|-------------|-----------|
| ESP32 Development Board | 1 | ฿300-500 | ใจกลางของระบบ |
| Servo Motor SG90 | 1 | ฿80-120 | ควบคุมการให้อาหาร |
| Load Cell 1kg + HX711 | 1 ชุด | ฿150-250 | วัดน้ำหนักอาหาร |
| RTC DS3231 | 1 | ฿80-150 | นาฬิกาเรียลไทม์ |
| PIR Sensor HC-SR501 | 1 | ฿50-100 | ตรวจจับแมว |
| LCD 16x2 + I2C | 1 | ฿120-200 | แสดงข้อมูล |
| Buzzer 5V | 1 | ฿20-40 | เสียงแจ้งเตือน |
| RGB LED | 1 | ฿10-20 | แสดงสถานะ |
| Push Button | 2 | ฿10-20 | ควบคุมด้วยตนเอง |
| Resistor 10kΩ | 2 | ฿5-10 | Pull-up สำหรับปุ่ม |
| Resistor 220Ω | 3 | ฿5-10 | จำกัดกระแส LED |
| Power Supply 5V/2A | 1 | ฿150-300 | จ่ายไฟให้ระบบ |
| Breadboard/PCB | 1 | ฿50-150 | เชื่อมต่ออุปกรณ์ |
| Jumper Wires | 1 ชุด | ฿50-100 | สายเชื่อมต่อ |

**รวมค่าใช้จ่าย: ประมาณ ฿1,080-1,970**

### 🎯 อุปกรณ์เสริม (Optional)

| อุปกรณ์ | จำนวน | ราคาประมาณ | ประโยชน์ |
|---------|--------|-------------|-----------|
| ESP32-CAM | 1 | ฿200-350 | ถ่ายรูปแมวขณะกิน |
| SD Card Module | 1 | ฿50-100 | บันทึกข้อมูลระยะยาว |
| Water Level Sensor | 1 | ฿30-80 | ตรวจสอบระดับน้ำ |
| กล่องกันน้ำ | 1 | ฿200-500 | ป้องกันอุปกรณ์ |
| ขาตั้งและแท่น | 1 ชุด | ฿300-800 | โครงสร้างระบบ |

---

## 🚀 การติดตั้งเบื้องต้น

### ขั้นตอนที่ 1: เตรียมสภาพแวดล้อม

1. **ดาวน์โหลดและติดตั้ง Arduino IDE**
   ```bash
   # ไปที่ https://www.arduino.cc/en/software
   # เลือกเวอร์ชัน 2.x.x สำหรับระบบปฏิบัติการของคุณ
   ```

2. **ติดตั้ง ESP32 Board Package**
   ```
   Arduino IDE → File → Preferences
   Additional Board Manager URLs:
   https://dl.espressif.com/dl/package_esp32_index.json
   
   Tools → Board → Boards Manager
   ค้นหา: "ESP32" → Install
   ```

3. **ติดตั้งไลบรารีที่จำเป็น**
   ```
   Tools → Manage Libraries
   
   ติดตั้งไลบรารีต่อไปนี้:
   - ArduinoJson (v6.21.3+)
   - ESP32Servo (v0.13.0+)
   - HX711 Arduino Library (v0.7.5+)
   - RTClib (v2.1.1+)
   - LiquidCrystal I2C (v1.1.2+)
   - Blynk (v1.3.2+)
   - Time (v1.6.1+)
   ```

### ขั้นตอนที่ 2: เชื่อมต่อฮาร์ดแวร์

ดูรายละเอียดการเชื่อมต่อใน [`wiring_diagram_cat_feeding.txt`](wiring_diagram_cat_feeding.txt)

**การเชื่อมต่อหลัก:**
```
ESP32 PIN → อุปกรณ์
PIN 18    → Servo Signal
PIN 19    → HX711 DOUT
PIN 21    → HX711 SCK + I2C SDA
PIN 22    → PIR OUT + I2C SCL
PIN 23    → Buzzer
PIN 25-27 → RGB LED (R,G,B)
PIN 32-33 → Push Buttons
```

### ขั้นตอนที่ 3: ตั้งค่าโค้ด

1. **ดาวน์โหลดโค้ด**
   ```bash
   # คัดลอกไฟล์ cat_feeding_system.ino
   # เปิดใน Arduino IDE
   ```

2. **แก้ไขการตั้งค่า**
   ```cpp
   // WiFi Settings
   const char* ssid = "ชื่อ_WiFi_ของคุณ";
   const char* password = "รหัสผ่าน_WiFi";
   
   // Blynk Settings
   const char* blynk_token = "Auth_Token_จาก_Blynk";
   ```

3. **อัพโหลดโค้ด**
   ```
   Tools → Board → ESP32 Dev Module
   Tools → Port → เลือกพอร์ต ESP32
   กด Upload (→)
   ```

### ขั้นตอนที่ 4: ตั้งค่า Blynk App

1. **สร้างบัญชี Blynk**
   - ดาวน์โหลดแอป Blynk IoT
   - สมัครสมาชิกฟรี
   - สร้าง Template ใหม่

2. **ตั้งค่า Virtual Pins**
   ```
   V0  → Weight Display (น้ำหนักอาหาร)
   V1  → Feed Count (จำนวนครั้ง)
   V2  → Total Food (ปริมาณรวม)
   V3  → Cat Detection (ตรวจพบแมว)
   V4  → System Status (สถานะระบบ)
   V10 → Manual Feed Button (ปุ่มให้อาหาร)
   V11 → System ON/OFF (เปิด/ปิดระบบ)
   V12 → Feed Amount Slider (ปรับปริมาณ)
   ```

---

## 📱 การใช้งาน

### การใช้งานผ่าน Web Interface

1. **เข้าถึงหน้าเว็บ**
   ```
   เปิดเบราว์เซอร์
   ไปที่: http://[IP_Address_ของ_ESP32]
   ```

2. **ฟีเจอร์หลัก**
   - 📊 **Dashboard**: ดูสถานะปัจจุบัน น้ำหนักอาหาร จำนวนครั้งที่ให้
   - 🍽️ **Manual Feed**: ให้อาหารทันทีด้วยปริมาณที่กำหนด
   - ⏰ **Schedule**: ตั้งค่าตารางเวลาการให้อาหาร
   - ⚙️ **Settings**: ปรับแต่งระบบและเซ็นเซอร์

### การใช้งานผ่าน Blynk App

1. **เปิดแอป Blynk**
   - เลือก Device "Cat Feeder System"
   - ตรวจสอบสถานะ Online/Offline

2. **Widget การควบคุม**
   - 🔘 **Feed Now**: ปุ่มให้อาหารทันที
   - 🎚️ **Amount Slider**: ปรับปริมาณอาหาร (5-100g)
   - 🔄 **System Switch**: เปิด/ปิดระบบ
   - 📊 **Status Display**: ดูข้อมูลเรียลไทม์

### การใช้งานผ่านปุ่มฟิสิคัล

1. **ปุ่มให้อาหารด้วยตนเอง (PIN 32)**
   - กดเพื่อให้อาหาร 30g ทันที
   - มีเสียงและไฟแจ้งเตือน

2. **ปุ่มรีเซ็ตระบบ (PIN 33)**
   - กดเพื่อรีเซ็ตสถิติและการตั้งค่า
   - ปรับเทียบเซ็นเซอร์น้ำหนักใหม่

### การตีความสัญญาณ LED

```
🟢 เขียว    → ระบบทำงานปกติ
🔴 แดง      → ข้อผิดพลาดหรือเตือนอาหารหมด
🔵 น้ำเงิน   → ตรวจพบแมวที่ชามอาหาร
🟠 ส้ม      → กำลังให้อาหาร
🟡 เหลือง   → คำเตือน (WiFi หลุด, เซ็นเซอร์ผิดปกติ)
```

---

## 🔧 API Reference

### REST API Endpoints

#### GET /api/status
ดึงข้อมูลสถานะปัจจุบันของระบบ

**Response:**
```json
{
  "weight": 450.5,
  "feedingCount": 12,
  "totalFood": 360.0,
  "catDetected": false,
  "systemActive": true,
  "lastFeedingTime": "14:30 15/12",
  "nextFeedingTime": "18:00",
  "currentTime": "14:45:23"
}
```

#### POST /api/feed
สั่งให้อาหารด้วยตนเอง

**Request Body:**
```json
{
  "amount": 50.0
}
```

**Response:**
```json
{
  "message": "ให้อาหารเสร็จสิ้น",
  "success": true
}
```

#### GET /api/schedule
ดึงข้อมูลตารางเวลาการให้อาหาร

**Response:**
```json
{
  "schedules": [
    {"hour": 7, "minute": 0, "amount": 50.0, "enabled": true},
    {"hour": 12, "minute": 0, "amount": 30.0, "enabled": true},
    {"hour": 18, "minute": 0, "amount": 50.0, "enabled": true}
  ]
}
```

#### POST /api/schedule
อัพเดทตารางเวลาการให้อาหาร

**Request Body:**
```json
{
  "schedules": [
    {"hour": 7, "minute": 0, "amount": 50.0, "enabled": true},
    {"hour": 12, "minute": 0, "amount": 30.0, "enabled": true},
    {"hour": 18, "minute": 0, "amount": 50.0, "enabled": true}
  ]
}
```

#### POST /api/calibrate
ปรับเทียบเซ็นเซอร์น้ำหนัก (รีเซ็ตเป็น 0)

**Response:**
```json
{
  "message": "ปรับเทียบเซ็นเซอร์เสร็จสิ้น",
  "success": true
}
```

### Blynk Virtual Pins

| Pin | Type | Direction | Description | Range |
|-----|------|-----------|-------------|-------|
| V0 | Float | ESP32→App | น้ำหนักอาหารปัจจุบัน | 0-5000g |
| V1 | Integer | ESP32→App | จำนวนครั้งที่ให้อาหาร | 0-9999 |
| V2 | Float | ESP32→App | ปริมาณอาหารรวม | 0-99999g |
| V3 | Integer | ESP32→App | สถานะตรวจพบแมว | 0-1 |
| V4 | Integer | ESP32→App | สถานะระบบ | 0-1 |
| V10 | Integer | App→ESP32 | ปุ่มให้อาหารด้วยตนเอง | 0-1 |
| V11 | Integer | App→ESP32 | เปิด/ปิดระบบ | 0-1 |
| V12 | Integer | App→ESP32 | ปริมาณอาหารที่จะให้ | 5-100g |

---

## 🔧 การแก้ปัญหา

### ปัญหาที่พบบ่อย

#### 1. ESP32 เชื่อมต่อ WiFi ไม่ได้

**อาการ:**
- LCD แสดง "WiFi Failed"
- Serial Monitor แสดงข้อผิดพลาดการเชื่อมต่อ

**การแก้ไข:**
```cpp
// ตรวจสอบการตั้งค่า WiFi
const char* ssid = "ชื่อ_WiFi_ที่ถูกต้อง";
const char* password = "รหัสผ่าน_ที่ถูกต้อง";

// ตรวจสอบสัญญาณ WiFi และใช้ 2.4GHz (ไม่ใช่ 5GHz)
```

#### 2. เซ็นเซอร์น้ำหนักอ่านค่าผิดปกติ

**อาการ:**
- อ่านค่า 0 หรือค่าติดลบ
- ค่าไม่เปลี่ยนแปลงเมื่อมีน้ำหนัก

**การแก้ไข:**
```cpp
// ปรับค่า calibrationFactor
float calibrationFactor = -7050.0; // ลองปรับค่านี้

// ตรวจสอบการเชื่อมต่อ HX711
// Red → E+, Black → E-, White → A-, Green → A+
```

#### 3. Servo Motor ไม่หมุน

**อาการ:**
- Servo ไม่เคลื่อนไหวเมื่อสั่งให้อาหาร
- มีเสียงแต่ไม่หมุน

**การแก้ไข:**
```cpp
// ตรวจสอบแหล่งจ่ายไฟ 5V
// ตรวจสอบการเชื่อมต่อ PIN 18

// ทดสอบ Servo แยกต่างหาก
#include <ESP32Servo.h>
Servo testServo;
void setup() {
  testServo.attach(18);
  testServo.write(90); // ทดสอบหมุน
}
```

#### 4. Blynk แสดงสถานะ Offline

**อาการ:**
- แอป Blynk แสดง Device offline
- ไม่สามารถควบคุมผ่านแอปได้

**การแก้ไข:**
```cpp
// ตรวจสอบ Auth Token
const char* blynk_token = "Token_ที่ถูกต้อง_จาก_Blynk";

// ตรวจสอบการเชื่อมต่ออินเทอร์เน็ต
// ลองใช้ Blynk Server อื่น
```

### การ Debug ขั้นสูง

#### เปิดใช้งาน Debug Mode

```cpp
// เพิ่มในส่วนต้นของโค้ด
#define DEBUG_MODE 1

#if DEBUG_MODE
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif
```

#### ตรวจสอบ I2C Devices

```cpp
// โค้ดสแกนหา I2C Address
#include <Wire.h>

void scanI2C() {
  Wire.begin(21, 22);
  Serial.println("Scanning I2C devices...");
  
  for(byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if(Wire.endTransmission() == 0) {
      Serial.print("Found device at: 0x");
      Serial.println(address, HEX);
    }
  }
}
```

#### ตรวจสอบ Memory Usage

```cpp
void printMemoryInfo() {
  Serial.print("Free Heap: ");
  Serial.println(ESP.getFreeHeap());
  Serial.print("Heap Size: ");
  Serial.println(ESP.getHeapSize());
  Serial.print("Min Free Heap: ");
  Serial.println(ESP.getMinFreeHeap());
}
```

---

## 🚀 การพัฒนาต่อ

### ฟีเจอร์ที่สามารถเพิ่มได้

#### 1. ESP32-CAM Integration
```cpp
// เพิ่มการถ่ายรูปแมวขณะกิน
#include "esp_camera.h"

void capturePhoto() {
  camera_fb_t * fb = esp_camera_fb_get();
  if(fb) {
    // บันทึกรูปหรือส่งผ่าน HTTP
    esp_camera_fb_return(fb);
  }
}
```

#### 2. SD Card Data Logging
```cpp
// บันทึกข้อมูลลง SD Card
#include "SD.h"

void logFeedingData(float amount) {
  File dataFile = SD.open("feeding_log.txt", FILE_APPEND);
  if (dataFile) {
    dataFile.print(rtc.now().timestamp());
    dataFile.print(",");
    dataFile.println(amount);
    dataFile.close();
  }
}
```

#### 3. Email Notifications
```cpp
// ส่งอีเมลแจ้งเตือน
#include "ESP_Mail_Client.h"

void sendEmailAlert(String message) {
  SMTPSession smtp;
  ESP_Mail_Session session;
  
  session.server.host_name = "smtp.gmail.com";
  session.server.port = 587;
  session.login.email = "your_email@gmail.com";
  session.login.password = "your_password";
  
  // ส่งอีเมล
}
```

#### 4. Telegram Bot Integration
```cpp
// แจ้งเตือนผ่าน Telegram
#include "UniversalTelegramBot.h"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

void sendTelegramMessage(String message) {
  bot.sendMessage(CHAT_ID, message, "");
}
```

### การปรับปรุงประสิทธิภาพ

#### 1. Power Management
```cpp
// ประหยัดพลังงาน
#include "esp_sleep.h"

void enterDeepSleep(int seconds) {
  esp_sleep_enable_timer_wakeup(seconds * 1000000);
  esp_deep_sleep_start();
}
```

#### 2. OTA Updates
```cpp
// อัพเดทโค้ดผ่าน WiFi
#include <ArduinoOTA.h>

void setupOTA() {
  ArduinoOTA.setHostname("cat-feeder");
  ArduinoOTA.setPassword("admin");
  
  ArduinoOTA.onStart([]() {
    Serial.println("Start updating");
  });
  
  ArduinoOTA.begin();
}
```

#### 3. Web Dashboard Enhancement
```html
<!-- เพิ่มกราฟแสดงสถิติ -->
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<canvas id="feedingChart"></canvas>

<script>
// สร้างกราฟแสดงประวัติการให้อาหาร
const ctx = document.getElementById('feedingChart').getContext('2d');
const chart = new Chart(ctx, {
  type: 'line',
  data: {
    labels: ['Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat', 'Sun'],
    datasets: [{
      label: 'Food Amount (g)',
      data: [130, 120, 140, 125, 135, 130, 128],
      borderColor: 'rgb(75, 192, 192)',
      tension: 0.1
    }]
  }
});
</script>
```

---

## 📞 การสนับสนุน

### 📚 เอกสารเพิ่มเติม

- 📋 [คู่มือการติดตั้ง](CAT_FEEDING_SETUP_GUIDE_TH.md)
- 🔌 [แผนผังการเชื่อมต่อ](wiring_diagram_cat_feeding.txt)
- 📦 [รายการไลบรารี](libraries_cat_feeding.txt)

### 🌐 แหล่งข้อมูลออนไลน์

- **Arduino Forum**: https://forum.arduino.cc/
- **ESP32 Documentation**: https://docs.espressif.com/
- **Blynk Documentation**: https://docs.blynk.io/
- **GitHub Repository**: [สำหรับ Source Code และ Issues]

### 💬 ชุมชนและการสนับสนุน

- **Facebook Group**: [ชุมชน Arduino Thailand]
- **Discord Server**: [ESP32 Developers]
- **YouTube Channel**: [สำหรับ Video Tutorials]

### 🐛 การรายงานปัญหา

หากพบปัญหาหรือต้องการแนะนำฟีเจอร์ใหม่:

1. ตรวจสอบ [Issues](https://github.com/your-repo/issues) ที่มีอยู่แล้ว
2. สร้าง Issue ใหม่พร้อมรายละเอียด:
   - รุ่นฮาร์ดแวร์ที่ใช้
   - เวอร์ชันโค้ด
   - ขั้นตอนการทำซ้ำปัญหา
   - ข้อความ Error (ถ้ามี)
   - ภาพหน้าจอ (ถ้าเกี่ยวข้อง)

### 🤝 การมีส่วนร่วม

ยินดีรับการมีส่วนร่วมจากชุมชน:

- 🔧 **Code Contribution**: ส่ง Pull Request
- 📝 **Documentation**: ปรับปรุงเอกสาร
- 🐛 **Bug Reports**: รายงานปัญหา
- 💡 **Feature Requests**: แนะนำฟีเจอร์ใหม่
- 🌐 **Translation**: แปลเอกสารเป็นภาษาอื่น

---

## 📄 ลิขสิทธิ์

### MIT License

```
Copyright (c) 2024 Cat Feeding System Contributors

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

### Third-Party Libraries

โปรเจกต์นี้ใช้ไลบรารีโอเพนซอร์สต่อไปนี้:

- **ArduinoJson** - MIT License
- **ESP32Servo** - MIT License  
- **HX711** - MIT License
- **RTClib** - MIT License
- **LiquidCrystal_I2C** - MIT License
- **Blynk** - MIT License
- **Time** - LGPL License

---

## 📊 สถิติโปรเจกต์

![GitHub stars](https://img.shields.io/github/stars/your-username/cat-feeding-system?style=social)
![GitHub forks](https://img.shields.io/github/forks/your-username/cat-feeding-system?style=social)
![GitHub issues](https://img.shields.io/github/issues/your-username/cat-feeding-system)
![GitHub pull requests](https://img.shields.io/github/issues-pr/your-username/cat-feeding-system)

### ข้อมูลการพัฒนา

- **วันที่เริ่มโปรเจกต์**: 2024
- **เวอร์ชันปัจจุบัน**: 2.0
- **ภาษาหลัก**: C++ (Arduino)
- **แพลตฟอร์ม**: ESP32
- **ใบอนุญาต**: MIT License
- **Contributors**: [รายชื่อผู้มีส่วนร่วม]

---

## 🎯 Roadmap

### Version 2.1 (Q2 2024)
- [ ] ESP32-CAM integration
- [ ] SD Card data logging
- [ ] Email notifications
- [ ] Improved web dashboard

### Version 2.2 (Q3 2024)
- [ ] Telegram bot integration
- [ ] Voice control (Google Assistant/Alexa)
- [ ] Mobile app (React Native)
- [ ] Multi-pet support

### Version 3.0 (Q4 2024)
- [ ] AI-powered feeding recommendations
- [ ] Health monitoring integration
- [ ] Cloud-based analytics
- [ ] Commercial hardware kit

---

## 🙏 ขอบคุณ

ขอบคุณผู้ที่มีส่วนร่วมในโปรเจกต์นี้:

- **Core Contributors**: [รายชื่อ]
- **Beta Testers**: [รายชื่อ]
- **Documentation**: [รายชื่อ]
- **Community Support**: [รายชื่อ]

### สนับสนุนโปรเจกต์

หากโปรเจกต์นี้มีประโยชน์สำหรับคุณ:

- ⭐ ให้ Star บน GitHub
- 🔄 Share ให้เพื่อนๆ
- 💰 [Donate](https://example.com/donate) สนับสนุนการพัฒนา
- 📝 เขียน Review หรือ Blog post
- 🤝 เข้าร่วมชุมชน

---

<div align="center">

## 🐱 ขอให้แมวของคุณมีความสุขกับระบบให้อาหารใหม่! 🐱

**Made with ❤️ for Cat Lovers**

[![Built with Arduino](https://img.shields.io/badge/Built%20with-Arduino-teal.svg)](https://www.arduino.cc/)
[![Powered by ESP32](https://img.shields.io/badge/Powered%20by-ESP32-red.svg)](https://www.espressif.com/)
[![IoT with Blynk](https://img.shields.io/badge/IoT%20with-Blynk-blue.svg)](https://blynk.io/)

</div>
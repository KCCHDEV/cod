# 🍅 ระบบรดน้ำมะเขือเทศอัตโนมัติ - เอกสารสมบูรณ์
## RDTRC Tomato Watering System - Complete Documentation

> **Firmware สร้างโดย: RDTRC**  
> **เวอร์ชัน: 4.0 - Enhanced Agricultural System**  
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
- [คู่มือการปลูกมะเขือเทศ](#คู่มือการปลูกมะเขือเทศ)

---

## 🎯 ภาพรวมระบบ

ระบบรดน้ำมะเขือเทศอัตโนมัติ RDTRC เป็นระบบ IoT ที่ออกแบบมาเฉพาะสำหรับการปลูกมะเขือเทศ ด้วยการควบคุมสภาพแวดล้อมที่แม่นยำและการรดน้ำอัจฉริยะ

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
         │        (Agricultural IoT Hub)                 │
         └───────────────────────┬───────────────────────┘
                                 │
    ┌────────────────────────────┼────────────────────────────┐
    │                            │                            │
    ▼                            ▼                            ▼
┌─────────┐              ┌─────────────┐              ┌─────────────┐
│Sensors  │              │ Actuators   │              │ Display &   │
│- DHT22  │              │ - Pump      │              │ Interface   │
│- Soil   │              │ - Valve     │              │ - LCD 16x2  │
│- Light  │              │ - Buzzer    │              │ - Buttons   │
└─────────┘              └─────────────┘              └─────────────┘
```

### 🌱 การออกแบบเฉพาะสำหรับมะเขือเทศ

- **ควบคุมความชื้นดิน**: 60-75% เหมาะสำหรับมะเขือเทศ
- **ตรวจสอบอุณหภูมิ**: 20-25°C ช่วงเจริญเติบโต
- **การรดน้ำแบบหยด**: ประหยัดน้ำและป้องกันโรคใบ
- **ติดตามการเจริญเติบโต**: บันทึกข้อมูลเพื่อวิเคราะห์

---

## 🌟 คุณสมบัติเด่น

### 🌡️ การควบคุมสภาพแวดล้อม
- **วัดอุณหภูมิและความชื้นอากาศ**: DHT22 sensor ความแม่นยำสูง
- **วัดความชื้นดิน**: Capacitive soil moisture sensor
- **ตรวจจับแสง**: วัดความเข้มแสงสำหรับการสังเคราะห์แสง
- **บันทึกข้อมูลสภาพอากาศ**: เก็บข้อมูลเพื่อวิเคราะห์

### 💧 ระบบรดน้ำอัจฉริยะ
- **รดน้ำตามความชื้นดิน**: รดเมื่อดินแห้งเกินกำหนด
- **รดน้ำตามตารางเวลา**: กำหนดเวลารดน้ำได้
- **ควบคุมปริมาณน้ำ**: ปรับปริมาณตามขนาดต้น
- **ป้องกันน้ำท่วม**: หยุดรดเมื่อดินชื้นเกินไป

### 📱 ควบคุมผ่านมือถือ
- **Blynk IoT Platform**: ควบคุมและติดตามผ่านแอปมือถือ
- **Web Interface**: เข้าถึงผ่านเบราว์เซอร์
- **แจ้งเตือนแบบเรียลไทม์**: แจ้งสภาพอากาศ, การรดน้ำ, และปัญหา
- **การตั้งค่าระยะไกล**: ปรับเปลี่ยนการตั้งค่าได้ทุกที่

### 🌱 เฉพาะสำหรับมะเขือเทศ
- **ช่วงค่าที่เหมาะสม**: กำหนดค่าเฉพาะมะเขือเทศ
- **การเจริญเติบโตตามระยะ**: ปรับการดูแลตามอายุต้น
- **ป้องกันโรค**: ควบคุมความชื้นป้องกันโรคราน้ำค้าง
- **เพิ่มผลผลิต**: เพิ่มประสิทธิภาพการเจริญเติบโต

### 🔄 ระบบอัตโนมัติ
- **ทำงานแบบอัตโนมัติ**: ไม่ต้องดูแลตลอดเวลา
- **โหมดประหยัดน้ำ**: ใช้น้ำอย่างมีประสิทธิภาพ
- **การทำงานแบบออฟไลน์**: ใช้งานได้แม้ไม่มีอินเทอร์เน็ต
- **ระบบสำรอง**: มีแบตเตอรี่สำรองเมื่อไฟดับ

---

## 🔧 ข้อมูลทางเทคนิค

### 📊 ข้อกำหนดระบบ

| ข้อมูล | รายละเอียด |
|--------|------------|
| **ไมโครคอนโทรลเลอร์** | ESP32 Development Board |
| **แรงดันไฟฟ้า** | 12V DC / 2A (สำหรับปั๊มน้ำ) |
| **การเชื่อมต่อ** | WiFi 802.11 b/g/n |
| **ความจุหน่วยความจำ** | 4MB Flash, 320KB RAM |
| **ความแม่นยำอุณหภูมิ** | ±0.5°C |
| **ความแม่นยำความชื้น** | ±2% |
| **ช่วงการทำงาน** | 0-50°C, 10-95% RH |
| **ปริมาณน้ำ** | 100-2000ml ต่อครั้ง |
| **ความดัน** | 1-3 บาร์ |

### 🎛️ การกำหนดค่า Pin

```cpp
// Pin Definitions for Tomato Watering System
#define DHT_PIN 4                       // DHT22 Temperature & Humidity
#define DHT_TYPE DHT22                  // DHT22 sensor type
#define SOIL_MOISTURE_PIN 36            // Soil moisture sensor (ADC)
#define LIGHT_SENSOR_PIN 32             // Light sensor (ADC)
#define WATER_PUMP_RELAY_PIN 5          // Water pump relay
#define SOLENOID_VALVE_PIN 18           // Solenoid valve
#define WATER_LEVEL_TRIG_PIN 23         // Ultrasonic trigger
#define WATER_LEVEL_ECHO_PIN 25         // Ultrasonic echo
#define BUZZER_PIN 19                   // Buzzer
#define STATUS_LED_PIN 2                // Status LED
#define RESET_BUTTON_PIN 0              // Reset button
#define MANUAL_WATER_BUTTON_PIN 27      // Manual watering button
#define LCD_NEXT_BUTTON_PIN 26          // LCD navigation button

// I2C Pins for LCD
#define I2C_SDA 21                      // I2C Data Line
#define I2C_SCL 22                      // I2C Clock Line

// Tomato-specific thresholds
#define TOMATO_OPTIMAL_TEMP 22.5        // °C
#define TOMATO_MIN_TEMP 18.0            // °C
#define TOMATO_MAX_TEMP 28.0            // °C
#define TOMATO_OPTIMAL_HUMIDITY 65.0    // %
#define TOMATO_OPTIMAL_SOIL_MOISTURE 70 // %
#define TOMATO_MIN_SOIL_MOISTURE 50     // %
```

---

## 🛍️ รายการอุปกรณ์

### 📋 อุปกรณ์หลัก

| อุปกรณ์ | จำนวน | ราคาประมาณ | รายละเอียด |
|---------|--------|-------------|------------|
| **ESP32 Development Board** | 1 | ฿350-500 | ตัวควบคุมหลัก |
| **DHT22 Sensor** | 1 | ฿150-250 | วัดอุณหภูมิและความชื้นอากาศ |
| **Capacitive Soil Moisture Sensor** | 2 | ฿200-300 | วัดความชื้นดิน (ทนต่อการกัดกร่อน) |
| **Light Sensor (BH1750)** | 1 | ฿80-120 | วัดความเข้มแสง |
| **Water Pump 12V** | 1 | ฿300-500 | ปั๊มน้ำแรงดันสูง |
| **Solenoid Valve 12V** | 2 | ฿400-600 | วาล์วควบคุมน้ำ |
| **Relay Module 4-Channel** | 1 | ฿150-250 | ควบคุมอุปกรณ์ไฟฟ้า |
| **HC-SR04 Ultrasonic** | 1 | ฿50-80 | วัดระดับน้ำในถัง |
| **LCD 16x2 I2C** | 1 | ฿120-180 | หน้าจอแสดงผล |
| **Buzzer Module** | 1 | ฿30-50 | เสียงแจ้งเตือน |

### 🌿 อุปกรณ์การเกษตร

| อุปกรณ์ | จำนวน | ราคาประมาณ | รายละเอียด |
|---------|--------|-------------|------------|
| **Drip Irrigation Kit** | 1 ชุด | ฿400-800 | ชุดท่อหยดน้ำ |
| **Water Tank 50L** | 1 | ฿300-500 | ถังเก็บน้ำ |
| **Water Filter** | 1 | ฿150-300 | กรองน้ำป้องกันสิ่งสกปรก |
| **Pressure Regulator** | 1 | ฿200-350 | ควบคุมความดันน้ำ |
| **Timer Valve** | 1 | ฿250-400 | วาล์วตั้งเวลา (สำรอง) |
| **pH Test Kit** | 1 | ฿100-200 | ชุดทดสอบ pH ดิน |
| **Fertilizer Injector** | 1 | ฿300-600 | เครื่องฉีดปุ๋ย |

### 🔌 อุปกรณ์เสริม

| อุปกรณ์ | จำนวน | ราคาประมาณ | รายละเอียด |
|---------|--------|-------------|------------|
| **Power Supply 12V/3A** | 1 | ฿250-400 | แหล่งจ่ายไฟ |
| **UPS Battery 12V/7Ah** | 1 | ฿800-1200 | แบตเตอรี่สำรอง |
| **Waterproof Enclosure** | 1 | ฿300-500 | กล่องกันน้ำ |
| **Cable Glands** | 5 | ฿50-100 | ตัวกันน้ำสายไฟ |
| **Jumper Wires** | 1 ชุด | ฿50-100 | สายจัมเปอร์ |
| **Breadboard** | 1 | ฿80-150 | เบรดบอร์ด |
| **Resistors Pack** | 1 | ฿50-100 | ตัวต้านทานหลายค่า |

### 💰 สรุปค่าใช้จ่าย

| ประเภท | ราคา (ต่ำสุด) | ราคา (สูงสุด) |
|--------|---------------|---------------|
| **อุปกรณ์หลัก** | ฿1,830 | ฿2,730 |
| **อุปกรณ์การเกษตร** | ฿1,700 | ฿3,150 |
| **อุปกรณ์เสริม** | ฿1,580 | ฿2,550 |
| **รวม** | **฿5,110** | **฿8,430** |

---

## 🔌 การเชื่อมต่อวงจร

### 📐 Wiring Diagram

```
ESP32 Pin    │ Component                │ Description
─────────────┼──────────────────────────┼─────────────────────
3.3V         │ LCD VCC                  │ Power for LCD I2C
GND          │ LCD GND                  │ Ground for LCD
GPIO21       │ LCD SDA                  │ I2C Data Line
GPIO22       │ LCD SCL                  │ I2C Clock Line
             │                          │
GPIO4        │ DHT22 Data               │ Temperature & Humidity
3.3V         │ DHT22 VCC                │ Power for DHT22
GND          │ DHT22 GND                │ Ground for DHT22
             │                          │
GPIO36       │ Soil Moisture Analog     │ Soil moisture reading
3.3V         │ Soil Sensor VCC          │ Power for soil sensor
GND          │ Soil Sensor GND          │ Ground for soil sensor
             │                          │
GPIO32       │ Light Sensor SDA         │ Light sensor I2C data
GPIO22       │ Light Sensor SCL         │ Light sensor I2C clock
3.3V         │ Light Sensor VCC         │ Power for light sensor
GND          │ Light Sensor GND         │ Ground for light sensor
             │                          │
GPIO5        │ Relay IN1 (Pump)         │ Water pump control
GPIO18       │ Relay IN2 (Valve1)       │ Main valve control
GPIO17       │ Relay IN3 (Valve2)       │ Zone valve control
GPIO16       │ Relay IN4 (Fertilizer)   │ Fertilizer pump control
5V           │ Relay VCC                │ Power for relay module
GND          │ Relay GND                │ Ground for relay module
             │                          │
GPIO23       │ Ultrasonic Trig          │ Water level trigger
GPIO25       │ Ultrasonic Echo          │ Water level echo
5V           │ Ultrasonic VCC           │ Power for ultrasonic
GND          │ Ultrasonic GND           │ Ground for ultrasonic
             │                          │
GPIO19       │ Buzzer +                 │ Buzzer positive
GND          │ Buzzer -                 │ Buzzer negative
             │                          │
GPIO2        │ LED +                    │ Status LED positive
GND          │ LED - (220Ω)             │ LED negative with resistor
             │                          │
GPIO0        │ Reset Button             │ System reset
GPIO27       │ Manual Water             │ Manual watering button
GPIO26       │ LCD Next                 │ LCD navigation button
```

### ⚡ การต่อไฟฟ้าสำหรับการเกษตร

```
Agricultural Power Setup:
┌─────────────────┐
│ 12V/3A PSU      │
└─────┬───────────┘
      │
      ├── ESP32 (via 5V regulator)
      ├── Water Pump 12V/1A
      ├── Solenoid Valves 12V
      └── UPS Battery 12V/7Ah
          (for power backup)

Water System Layout:
┌─────────────────┐
│ Water Tank 50L  │
│ with Level      │
│ Sensor          │
└─────┬───────────┘
      │
┌─────▼───────────┐
│ Water Filter    │
└─────┬───────────┘
      │
┌─────▼───────────┐
│ Water Pump      │
│ + Pressure      │
│ Regulator       │
└─────┬───────────┘
      │
┌─────▼───────────┐
│ Solenoid Valve  │
│ Main Control    │
└─────┬───────────┘
      │
      ├── Zone 1 (Tomato Plot A)
      ├── Zone 2 (Tomato Plot B)
      └── Zone 3 (Fertilizer Line)
```

### 🌱 การติดตั้งเซ็นเซอร์ในแปลง

```
Sensor Placement in Tomato Garden:

Plot Layout (Top View):
┌─────────────────────────────────────┐
│ 🍅  🍅  🍅  🍅  🍅  🍅  🍅  🍅 │ Row 1
│                                     │
│ 🍅  🍅  🍅  🍅  🍅  🍅  🍅  🍅 │ Row 2
│         📊 Soil 1                   │
│ 🍅  🍅  🍅  🍅  🍅  🍅  🍅  🍅 │ Row 3
│                                     │
│ 🍅  🍅  🍅  🍅  🍅  🍅  🍅  🍅 │ Row 4
│         📊 Soil 2                   │
└─────────────────────────────────────┘
         🌡️ DHT22 (Center, 1.5m high)
         ☀️ Light Sensor (Open area)
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

### 📚 ไลบรารีที่จำเป็น

```cpp
// Core Libraries (Built-in)
#include <WiFi.h>              // WiFi connectivity
#include <WebServer.h>         // Web server functionality
#include <SPIFFS.h>            // File system
#include <Wire.h>              // I2C communication
#include <HTTPClient.h>        // HTTP client
#include <ArduinoOTA.h>        // Over-the-air updates

// Sensor Libraries
#include <DHT.h>               // DHT22 temperature/humidity sensor
#include <BH1750.h>            // Light sensor (optional)

// Display and Control
#include <LiquidCrystal_I2C.h> // LCD I2C display

// IoT Platform
#include <BlynkSimpleEsp32.h>  // Blynk IoT platform
#include <NTPClient.h>         // Network time protocol
#include <WiFiUdp.h>           // UDP communication

// Data Handling
#include <ArduinoJson.h>       // JSON handling

// Custom Libraries
#include "RDTRC_LCD_Library.h" // Custom LCD functions
```

### 📦 การติดตั้งไลบรารี

```bash
Arduino IDE → Tools → Manage Libraries

ติดตั้งไลบรารีต่อไปนี้:

1. DHT sensor library (version 1.4.4 หรือใหม่กว่า)
   - ผู้พัฒนา: Adafruit
   - ใช้สำหรับ: DHT22 temperature and humidity sensor

2. ArduinoJson (version 6.21.0 หรือใหม่กว่า)
   - ผู้พัฒนา: Benoit Blanchon
   - ใช้สำหรับ: JSON data handling

3. LiquidCrystal I2C (version 1.1.2 หรือใหม่กว่า)
   - ผู้พัฒนา: Frank de Brabander
   - ใช้สำหรับ: I2C LCD display

4. Blynk (version 1.3.2 หรือใหม่กว่า)
   - ผู้พัฒนา: Volodymyr Shymanskyy
   - ใช้สำหรับ: IoT platform integration

5. NTPClient (version 3.2.1 หรือใหม่กว่า)
   - ผู้พัฒนา: Fabrice Weinberg
   - ใช้สำหรับ: Network time synchronization

6. BH1750 (version 1.3.0 หรือใหม่กว่า) [ตัวเลือก]
   - ผู้พัฒนา: Christopher Laws
   - ใช้สำหรับ: Digital light sensor
```

---

## ⚙️ การตั้งค่าระบบ

### 🌐 การตั้งค่า WiFi

```cpp
// Network Configuration
const char* ssid = "YOUR_WIFI_SSID";           // ชื่อ WiFi ของคุณ
const char* password = "YOUR_WIFI_PASSWORD";   // รหัสผ่าน WiFi
const char* hotspot_ssid = "RDTRC_TomatoFarm"; // ชื่อ Hotspot
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
#define BLYNK_TEMPLATE_NAME "Tomato_Watering_System"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN_HERE"

// Blynk Virtual Pins
#define BLYNK_VPIN_TEMPERATURE 0     // Temperature gauge
#define BLYNK_VPIN_HUMIDITY 1        // Humidity gauge
#define BLYNK_VPIN_SOIL_MOISTURE 2   // Soil moisture gauge
#define BLYNK_VPIN_LIGHT_LEVEL 3     // Light level gauge
#define BLYNK_VPIN_PUMP_STATUS 4     // Pump status LED
#define BLYNK_VPIN_MANUAL_WATER 5    // Manual watering button
#define BLYNK_VPIN_AUTO_MODE 6       // Auto mode switch
#define BLYNK_VPIN_WATER_AMOUNT 7    // Water amount slider
#define BLYNK_VPIN_SCHEDULE 8        // Watering schedule
#define BLYNK_VPIN_MESSAGES 9        // System messages terminal
```

### 🍅 การตั้งค่าเฉพาะมะเขือเทศ

```cpp
// Tomato-Specific Configuration
#define TOMATO_OPTIMAL_TEMP 22.5     // °C - อุณหภูมิที่เหมาะสม
#define TOMATO_MIN_TEMP 18.0         // °C - อุณหภูมิต่ำสุด
#define TOMATO_MAX_TEMP 28.0         // °C - อุณหภูมิสูงสุด
#define TOMATO_OPTIMAL_HUMIDITY 65.0 // % - ความชื้นอากาศที่เหมาะสม
#define TOMATO_MAX_HUMIDITY 80.0     // % - ความชื้นสูงสุด (ป้องกันโรค)

// Soil Configuration
#define TOMATO_OPTIMAL_SOIL_MOISTURE 70 // % - ความชื้นดินที่เหมาะสม
#define TOMATO_MIN_SOIL_MOISTURE 50     // % - ความชื้นต่ำสุด (เริ่มรดน้ำ)
#define TOMATO_MAX_SOIL_MOISTURE 85     // % - ความชื้นสูงสุด (หยุดรดน้ำ)

// Watering Configuration
#define DEFAULT_WATER_AMOUNT 500        // ml - ปริมาณน้ำมาตรฐาน
#define MIN_WATER_AMOUNT 100           // ml - ปริมาณน้ำต่ำสุด
#define MAX_WATER_AMOUNT 2000          // ml - ปริมาณน้ำสูงสุด
#define WATERING_DURATION 30           // seconds - ระยะเวลารดน้ำ

// Growth Stage Settings
enum TomatoGrowthStage {
  SEEDLING,     // 0-2 weeks
  VEGETATIVE,   // 2-6 weeks  
  FLOWERING,    // 6-10 weeks
  FRUITING      // 10+ weeks
};

TomatoGrowthStage currentGrowthStage = SEEDLING;

// Growth stage specific settings
struct GrowthStageConfig {
  int minSoilMoisture;
  int optimalSoilMoisture;
  int waterAmount;
  int wateringFrequency; // times per day
};

GrowthStageConfig stageConfigs[4] = {
  {60, 75, 200, 2}, // Seedling: higher moisture, less water, less frequent
  {55, 70, 400, 3}, // Vegetative: moderate moisture, more water
  {50, 65, 500, 4}, // Flowering: lower moisture, regular water
  {45, 60, 600, 5}  // Fruiting: lowest moisture, most water
};
```

### 🔔 การตั้งค่า LINE Notify

```cpp
// LINE Notify Configuration
const char* lineToken = "YOUR_LINE_NOTIFY_TOKEN";
const char* lineAPI = "https://notify-api.line.me/api/notify";

// Notification Settings
bool notifyWatering = true;          // แจ้งเตือนเมื่อรดน้ำ
bool notifyLowWater = true;          // แจ้งเตือนน้ำในถังหมด
bool notifyHighTemperature = true;   // แจ้งเตือนอุณหภูมิสูง
bool notifyLowSoilMoisture = true;   // แจ้งเตือนดินแห้ง
bool notifySystemError = true;       // แจ้งเตือนข้อผิดพลาดระบบ
bool notifyGrowthStage = true;       // แจ้งเตือนเปลี่ยนระยะการเจริญเติบโต
```

---

## 🎮 การใช้งาน

### 🚀 การเริ่มต้นใช้งาน

1. **เชื่อมต่อไฟฟ้า**
   - ตรวจสอบการต่อวงจรให้ถูกต้อง
   - ใช้แหล่งจ่ายไฟ 12V/3A
   - เชื่อมต่อแบตเตอรี่สำรอง UPS

2. **Boot Sequence**
   ```
   ╔════════════════╗
   ║ FW make by RDTRC║
   ║ Version 4.0     ║
   ╚════════════════╝
           ↓
   ╔════════════════╗
   ║ Tomato Farm    ║
   ║ Initializing...║
   ╚════════════════╝
           ↓
   ╔════════════════╗
   ║ Sensors: OK    ║
   ║ Pump: Ready    ║
   ╚════════════════╝
           ↓
   ╔════════════════╗
   ║ WiFi: Connected║
   ║ IP: 192.168.1.x║
   ╚════════════════╝
           ↓
   ╔════════════════╗
   ║ Auto Mode: ON  ║
   ║ Stage: Seedling║
   ╚════════════════╝
   ```

3. **การตรวจสอบเซ็นเซอร์**
   - ระบบจะตรวจสอบเซ็นเซอร์ทั้งหมด
   - แสดงค่าเริ่มต้นบน LCD
   - ส่งข้อมูลไปยัง Blynk App

### 📱 การควบคุมผ่าน LCD

#### 🖥️ หน้าจอ LCD สำหรับมะเขือเทศ

**หน้าหลัก (Main Screen)**
```
┌────────────────┐
│Tomato Farm v4.0│
│T:24°C H:65% S:ON│
└────────────────┘
```

**หน้าความชื้นดิน (Soil Screen)**
```
┌────────────────┐
│Soil: 68% (OK)  │
│Next: 14:00     │
└────────────────┘
```

**หน้าสถานะปั๊ม (Pump Screen)**
```
┌────────────────┐
│Pump: OFF       │
│Last: 500ml 12:00│
└────────────────┘
```

**หน้าระยะการเจริญเติบโต (Growth Screen)**
```
┌────────────────┐
│Stage: Flowering│
│Age: 45 days    │
└────────────────┘
```

**หน้าสภาพอากาศ (Weather Screen)**
```
┌────────────────┐
│Temp: 24.5°C    │
│Humidity: 65%   │
└────────────────┘
```

#### 🎛️ การใช้ปุ่ม

| ปุ่ม | การทำงาน |
|------|----------|
| **Manual Water** | รดน้ำทันที (ตามปริมาณที่ตั้งไว้) |
| **LCD Next** | เปลี่ยนหน้าจอ LCD |
| **Reset** | รีสตาร์ทระบบ |

### ⏰ การตั้งเวลารดน้ำตามระยะการเจริญเติบโต

```cpp
// Growth Stage Based Watering Schedule
struct WateringSchedule {
  int hour;
  int minute;
  int amount; // ml
  bool enabled;
};

// Seedling Stage (0-2 weeks)
WateringSchedule seedlingSchedule[2] = {
  {8, 0, 200, true},   // 08:00 น. 200ml
  {18, 0, 200, true}   // 18:00 น. 200ml
};

// Vegetative Stage (2-6 weeks)
WateringSchedule vegetativeSchedule[3] = {
  {7, 0, 400, true},   // 07:00 น. 400ml
  {13, 0, 300, true},  // 13:00 น. 300ml
  {19, 0, 400, true}   // 19:00 น. 400ml
};

// Flowering Stage (6-10 weeks)
WateringSchedule floweringSchedule[4] = {
  {6, 30, 500, true},  // 06:30 น. 500ml
  {11, 0, 400, true},  // 11:00 น. 400ml
  {15, 30, 400, true}, // 15:30 น. 400ml
  {20, 0, 500, true}   // 20:00 น. 500ml
};

// Fruiting Stage (10+ weeks)
WateringSchedule fruitingSchedule[5] = {
  {6, 0, 600, true},   // 06:00 น. 600ml
  {10, 0, 500, true},  // 10:00 น. 500ml
  {14, 0, 500, true},  // 14:00 น. 500ml
  {17, 30, 500, true}, // 17:30 น. 500ml
  {21, 0, 600, true}   // 21:00 น. 600ml
};

void updateWateringSchedule() {
  WateringSchedule* currentSchedule;
  int scheduleSize;
  
  switch (currentGrowthStage) {
    case SEEDLING:
      currentSchedule = seedlingSchedule;
      scheduleSize = 2;
      break;
    case VEGETATIVE:
      currentSchedule = vegetativeSchedule;
      scheduleSize = 3;
      break;
    case FLOWERING:
      currentSchedule = floweringSchedule;
      scheduleSize = 4;
      break;
    case FRUITING:
      currentSchedule = fruitingSchedule;
      scheduleSize = 5;
      break;
  }
  
  // Update main watering schedule
  for (int i = 0; i < scheduleSize; i++) {
    wateringTimes[i] = currentSchedule[i];
  }
}
```

### 🌱 การเปลี่ยนระยะการเจริญเติบโต

```cpp
// Growth Stage Management
void checkGrowthStage() {
  unsigned long plantAge = (millis() - plantStartTime) / (1000 * 60 * 60 * 24); // days
  TomatoGrowthStage newStage = currentGrowthStage;
  
  if (plantAge <= 14) {
    newStage = SEEDLING;
  } else if (plantAge <= 42) {
    newStage = VEGETATIVE;
  } else if (plantAge <= 70) {
    newStage = FLOWERING;
  } else {
    newStage = FRUITING;
  }
  
  if (newStage != currentGrowthStage) {
    currentGrowthStage = newStage;
    updateWateringSchedule();
    updateGrowthStageSettings();
    
    String stageName[] = {"Seedling", "Vegetative", "Flowering", "Fruiting"};
    systemLCD.clear();
    systemLCD.print("Growth Stage:");
    systemLCD.setCursor(0, 1);
    systemLCD.print(stageName[currentGrowthStage]);
    
    if (notifyGrowthStage) {
      sendLineNotification("🌱 มะเขือเทศเข้าสู่ระยะ: " + stageName[currentGrowthStage] + 
                          " (อายุ " + String(plantAge) + " วัน)");
    }
    
    DEBUG_PRINTLN("Growth stage changed to: " + stageName[currentGrowthStage]);
  }
}

void updateGrowthStageSettings() {
  GrowthStageConfig config = stageConfigs[currentGrowthStage];
  
  TOMATO_MIN_SOIL_MOISTURE = config.minSoilMoisture;
  TOMATO_OPTIMAL_SOIL_MOISTURE = config.optimalSoilMoisture;
  DEFAULT_WATER_AMOUNT = config.waterAmount;
  
  DEBUG_PRINTF("Updated settings for stage %d: Min=%d%%, Opt=%d%%, Amount=%dml\n",
               currentGrowthStage, config.minSoilMoisture, 
               config.optimalSoilMoisture, config.waterAmount);
}
```

---

## 🌐 Web Interface

### 🏠 หน้าหลัก (Tomato Dashboard)

เข้าถึงได้ที่: `http://[ESP32_IP_ADDRESS]/`

**คุณสมบัติหลัก:**
- แสดงสถานะสภาพแวดล้อมแบบเรียลไทม์
- ติดตามการเจริญเติบโตของมะเขือเทศ
- ควบคุมระบบรดน้ำ
- ตั้งค่าตารางเวลาตามระยะการเจริญเติบโต

### 📊 หน้าต่างๆ ของ Web Interface

#### 1. **Tomato Dashboard (หน้าหลัก)**
```html
┌─────────────────────────────────────┐
│ 🍅 RDTRC Tomato Watering System     │
├─────────────────────────────────────┤
│ Status: ● Online                    │
│ Growth Stage: 🌸 Flowering (45 days)│
│ Temperature: 24.5°C (Optimal)       │
│ Humidity: 65% (Good)                │
│ Soil Moisture: 68% (Good)           │
│ Light Level: ☀️ 850 lux             │
│                                     │
│ Last Watering: 500ml at 11:00       │
│ Next Watering: 15:30 (400ml)        │
│ Water Tank: 78% (39L)               │
│                                     │
│ [Water Now] [Settings] [History]     │
└─────────────────────────────────────┘
```

#### 2. **Growth Monitoring (การติดตามการเจริญเติบโต)**
```html
┌─────────────────────────────────────┐
│ 🌱 Growth Monitoring                │
├─────────────────────────────────────┤
│ Plant Age: 45 days                  │
│ Current Stage: 🌸 Flowering         │
│ Next Stage: 🍅 Fruiting (in 25 days)│
│                                     │
│ Growth Progress:                    │
│ ████████████████░░░░ 64%            │
│                                     │
│ Stage Timeline:                     │
│ ✅ Seedling (0-14 days)             │
│ ✅ Vegetative (15-42 days)          │
│ 🔄 Flowering (43-70 days)           │
│ ⏳ Fruiting (71+ days)              │
│                                     │
│ Optimal Conditions for Flowering:   │
│ Temperature: 20-25°C ✅             │
│ Humidity: 60-70% ✅                 │
│ Soil Moisture: 60-75% ✅            │
│ Light: 800-1000 lux ✅              │
│                                     │
│ [Change Stage] [Growth Tips]        │
└─────────────────────────────────────┘
```

#### 3. **Environmental Control (การควบคุมสภาพแวดล้อม)**
```html
┌─────────────────────────────────────┐
│ 🌡️ Environmental Control            │
├─────────────────────────────────────┤
│ Current Conditions:                 │
│ Temperature: 24.5°C                 │
│ [████████████░░░] 18°C ← → 28°C     │
│                                     │
│ Humidity: 65%                       │
│ [██████████░░░░░] 40% ← → 80%       │
│                                     │
│ Soil Moisture Zone 1: 68%           │
│ [███████████░░░░] 50% ← → 85%       │
│                                     │
│ Soil Moisture Zone 2: 72%           │
│ [████████████░░░] 50% ← → 85%       │
│                                     │
│ Light Level: 850 lux                │
│ [████████████░░░] 500 ← → 1200 lux  │
│                                     │
│ Alerts:                             │
│ ⚠️ Zone 2 moisture slightly high    │
│ ✅ All other parameters optimal     │
│                                     │
│ [Manual Controls] [Set Thresholds]  │
└─────────────────────────────────────┘
```

#### 4. **Watering System (ระบบรดน้ำ)**
```html
┌─────────────────────────────────────┐
│ 💧 Watering System Control          │
├─────────────────────────────────────┤
│ System Status: 🟢 Auto Mode Active  │
│ Water Tank: 78% (39L remaining)     │
│ Pump Status: ⏸️ Standby             │
│                                     │
│ Today's Watering:                   │
│ 06:30 - 500ml ✅ Zone 1&2           │
│ 11:00 - 400ml ✅ Zone 1&2           │
│ 15:30 - 400ml ⏳ Scheduled          │
│ 20:00 - 500ml ⏳ Scheduled          │
│                                     │
│ Total Today: 900ml / 1800ml planned │
│ Weekly Average: 1650ml/day          │
│                                     │
│ Quick Actions:                      │
│ [Water Zone 1] [Water Zone 2]       │
│ [Water Both Zones] [Fill Tank]      │
│                                     │
│ Manual Control:                     │
│ Zone: [Zone 1 ▼] Amount: [500]ml    │
│ [Start Watering] Duration: 30s      │
│                                     │
│ [Schedule Settings] [Fertilizer]    │
└─────────────────────────────────────┘
```

#### 5. **Analytics (การวิเคราะห์)**
```html
┌─────────────────────────────────────┐
│ 📈 Growth Analytics                 │
├─────────────────────────────────────┤
│ 7-Day Environmental Trends:         │
│                                     │
│ Temperature (°C):                   │
│ 30°C ████░░░░░░████░░░░░░████       │
│ 20°C ░░░░████████░░████████░░       │
│ 10°C ░░░░░░░░░░░░░░░░░░░░░░░░       │
│      Mon  Tue  Wed  Thu  Fri       │
│                                     │
│ Soil Moisture (%):                  │
│ 90%  ░░░░░░░░░░░░░░░░░░░░░░░░       │
│ 70%  ████████████████████████       │
│ 50%  ░░░░░░░░░░░░░░░░░░░░░░░░       │
│      Mon  Tue  Wed  Thu  Fri       │
│                                     │
│ Water Usage (L/day):                │
│ 3L   ░░░░████░░░░████░░░░████       │
│ 2L   ████░░░░████░░░░████░░░░       │
│ 1L   ░░░░░░░░░░░░░░░░░░░░░░░░       │
│      Mon  Tue  Wed  Thu  Fri       │
│                                     │
│ Growth Insights:                    │
│ • Optimal watering frequency: 4x/day│
│ • Best growth temp range: 22-26°C   │
│ • Water efficiency: 95%             │
│ • Predicted harvest: 25 days        │
│                                     │
│ [Export Data] [Generate Report]     │
└─────────────────────────────────────┘
```

### 🔗 Tomato-Specific API Endpoints

```javascript
// Growth Stage Management
GET /api/tomato/growth
Response: {
  "currentStage": "flowering",
  "plantAge": 45,
  "daysInStage": 3,
  "nextStage": "fruiting",
  "daysToNextStage": 25,
  "stageProgress": 64.3,
  "optimalConditions": {
    "temperature": {"min": 20, "max": 25},
    "humidity": {"min": 60, "max": 70},
    "soilMoisture": {"min": 60, "max": 75},
    "lightLevel": {"min": 800, "max": 1000}
  }
}

// Environmental Monitoring
GET /api/tomato/environment
Response: {
  "temperature": 24.5,
  "humidity": 65.0,
  "soilMoisture": {
    "zone1": 68,
    "zone2": 72,
    "average": 70
  },
  "lightLevel": 850,
  "waterTankLevel": 78,
  "conditions": {
    "temperature": "optimal",
    "humidity": "good",
    "soilMoisture": "good",
    "lightLevel": "optimal"
  },
  "alerts": [
    {
      "type": "warning",
      "message": "Zone 2 moisture slightly high",
      "value": 72,
      "threshold": 70
    }
  ]
}

// Watering Control
POST /api/tomato/water
Body: {
  "zone": "both", // "zone1", "zone2", "both"
  "amount": 500,  // ml
  "force": false
}

Response: {
  "success": true,
  "message": "Watering started",
  "zones": ["zone1", "zone2"],
  "amount": 500,
  "duration": 30,
  "startTime": "2024-12-19T15:30:00Z"
}
```

---

## 📱 Blynk App Integration

### 🔧 การตั้งค่า Blynk App สำหรับมะเขือเทศ

#### 📊 Widget Layout สำหรับมะเขือเทศ

```
┌─────────────────────────────────────┐
│ 🍅 Tomato Farm System               │
├─────────────────────────────────────┤
│ ● System Status (V0)                │
│   ○ Green: All systems optimal      │
│   ○ Yellow: Some parameters off     │
│   ○ Red: Critical issues            │
├─────────────────────────────────────┤
│ 🌡️ Temperature (V1)                 │
│   24.5°C [████████████░░] 18-28°C   │
├─────────────────────────────────────┤
│ 💧 Humidity (V2)                    │
│   65% [██████████░░░░] 40-80%       │
├─────────────────────────────────────┤
│ 🌱 Soil Moisture (V3)               │
│   Zone 1: 68% Zone 2: 72%          │
├─────────────────────────────────────┤
│ ☀️ Light Level (V4)                 │
│   850 lux - Optimal                 │
├─────────────────────────────────────┤
│ 💧 Water Tank (V5)                  │
│   [████████████░░░░] 78% (39L)      │
├─────────────────────────────────────┤
│ 🌸 Growth Stage (V6)                │
│   Flowering - Day 45                │
├─────────────────────────────────────┤
│ [Water Now] Button (V7)             │
│ Amount: [500]ml Zone: [Both ▼]      │
├─────────────────────────────────────┤
│ 🔄 Auto Mode Switch (V8)            │
│   ☑️ Automatic Watering Enabled     │
├─────────────────────────────────────┤
│ ⏰ Next Watering (V9)               │
│   15:30 - 400ml (Zone 1&2)         │
├─────────────────────────────────────┤
│ 💬 System Messages (V10)            │
│   [Terminal Widget]                 │
└─────────────────────────────────────┘
```

#### 🎛️ Virtual Pins สำหรับมะเขือเทศ

| Virtual Pin | Widget | Function |
|-------------|--------|----------|
| V0 | LED | Overall System Status |
| V1 | Gauge | Temperature (°C) |
| V2 | Gauge | Air Humidity (%) |
| V3 | Value Display | Soil Moisture Zones |
| V4 | Value Display | Light Level (lux) |
| V5 | Gauge | Water Tank Level (%) |
| V6 | Value Display | Growth Stage & Age |
| V7 | Button | Manual Watering |
| V8 | Switch | Auto Mode On/Off |
| V9 | Value Display | Next Watering Schedule |
| V10 | Terminal | System Messages & Alerts |
| V11 | Slider | Water Amount (100-2000ml) |
| V12 | Menu | Zone Selection |
| V13 | Graph | Temperature History |
| V14 | Graph | Soil Moisture History |
| V15 | Graph | Daily Water Usage |

### 📱 Blynk Code สำหรับมะเขือเทศ

```cpp
// Blynk Virtual Pin Handlers for Tomato System

// Manual Watering Button
BLYNK_WRITE(V7) {
  int buttonState = param.asInt();
  if (buttonState == 1 && autoModeEnabled) {
    int waterAmount = currentWaterAmount;
    String targetZone = currentZone;
    
    if (performWatering(targetZone, waterAmount)) {
      Blynk.virtualWrite(V10, "🍅 Manual watering: " + String(waterAmount) + 
                         "ml to " + targetZone);
    } else {
      Blynk.virtualWrite(V10, "❌ Watering failed - check system");
    }
  } else if (!autoModeEnabled) {
    Blynk.virtualWrite(V10, "⚠️ Auto mode disabled - enable first");
  }
}

// Auto Mode Switch
BLYNK_WRITE(V8) {
  autoModeEnabled = param.asInt();
  
  systemLCD.clear();
  systemLCD.print("Auto Mode:");
  systemLCD.setCursor(0, 1);
  systemLCD.print(autoModeEnabled ? "ENABLED" : "DISABLED");
  
  String status = autoModeEnabled ? "เปิด" : "ปิด";
  Blynk.virtualWrite(V10, "🔄 โหมดอัตโนมัติ: " + status);
  
  if (notifySystemError) {
    sendLineNotification("🍅 โหมดอัตโนมัติ: " + status);
  }
}

// Water Amount Slider
BLYNK_WRITE(V11) {
  currentWaterAmount = param.asInt();
  if (currentWaterAmount < MIN_WATER_AMOUNT) currentWaterAmount = MIN_WATER_AMOUNT;
  if (currentWaterAmount > MAX_WATER_AMOUNT) currentWaterAmount = MAX_WATER_AMOUNT;
  
  Blynk.virtualWrite(V10, "💧 ตั้งปริมาณน้ำ: " + String(currentWaterAmount) + "ml");
}

// Zone Selection Menu
BLYNK_WRITE(V12) {
  int zoneSelection = param.asInt();
  switch (zoneSelection) {
    case 1: currentZone = "zone1"; break;
    case 2: currentZone = "zone2"; break;
    case 3: currentZone = "both"; break;
    default: currentZone = "both"; break;
  }
  
  Blynk.virtualWrite(V10, "🌱 เลือกโซน: " + currentZone);
}

// Update Blynk with tomato-specific data
void updateBlynkTomatoData() {
  // System status based on all parameters
  bool allOptimal = true;
  String statusMessage = "🍅 ";
  
  // Check temperature
  if (currentTemperature < TOMATO_MIN_TEMP || currentTemperature > TOMATO_MAX_TEMP) {
    allOptimal = false;
    statusMessage += "Temp ";
  }
  
  // Check humidity
  if (currentHumidity > TOMATO_MAX_HUMIDITY) {
    allOptimal = false;
    statusMessage += "Humid ";
  }
  
  // Check soil moisture
  if (avgSoilMoisture < TOMATO_MIN_SOIL_MOISTURE) {
    allOptimal = false;
    statusMessage += "Soil ";
  }
  
  // Set status LED
  if (allOptimal) {
    Blynk.virtualWrite(V0, 255); // Green - All optimal
    statusMessage += "Optimal";
  } else {
    Blynk.virtualWrite(V0, 128); // Yellow - Some issues
    statusMessage += "Needs attention";
  }
  
  // Update individual sensors
  Blynk.virtualWrite(V1, currentTemperature);
  Blynk.virtualWrite(V2, currentHumidity);
  Blynk.virtualWrite(V3, "Zone1: " + String(soilMoisture1) + "% Zone2: " + String(soilMoisture2) + "%");
  Blynk.virtualWrite(V4, String(currentLightLevel) + " lux");
  Blynk.virtualWrite(V5, waterTankLevel);
  
  // Growth stage info
  String stageNames[] = {"Seedling", "Vegetative", "Flowering", "Fruiting"};
  unsigned long plantAge = (millis() - plantStartTime) / (1000 * 60 * 60 * 24);
  Blynk.virtualWrite(V6, stageNames[currentGrowthStage] + " - Day " + String(plantAge));
  
  // Next watering info
  String nextWatering = getNextWateringTime();
  Blynk.virtualWrite(V9, nextWatering);
  
  // Send status message
  Blynk.virtualWrite(V10, statusMessage);
}

// Scheduled watering notification
void notifyScheduledWatering(String zone, int amount) {
  String message = "💧 รดน้ำตามกำหนด: " + String(amount) + "ml (" + zone + ")";
  Blynk.virtualWrite(V10, message);
  
  if (notifyWatering) {
    sendLineNotification("🍅 " + message);
  }
}

// Growth stage change notification
void notifyGrowthStageChange(TomatoGrowthStage newStage, int plantAge) {
  String stageNames[] = {"ต้นกล้า", "ใบ", "ดอก", "ผล"};
  String message = "🌱 เข้าสู่ระยะ" + stageNames[newStage] + " (อายุ " + String(plantAge) + " วัน)";
  
  Blynk.virtualWrite(V10, message);
  
  if (notifyGrowthStage) {
    sendLineNotification("🍅 " + message);
  }
}
```

---

I'll continue with the remaining systems documentation. Let me create the Cilantro System documentation:

<function_calls>
<invoke name="todo_write">
<parameter name="merge">true
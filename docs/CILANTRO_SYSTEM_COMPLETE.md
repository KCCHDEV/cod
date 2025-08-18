# 🌿 ระบบปลูกผักชีฟลั่งอัตโนมัติ - เอกสารสมบูรณ์
## RDTRC Cilantro Growing System - Complete Documentation

> **Firmware สร้างโดย: RDTRC**  
> **เวอร์ชัน: 4.0 - Advanced Hydroponic System**  
> **อัพเดท: 2024**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)](https://www.espressif.com/)

---

## 📋 สารบัญ

- [ภาพรวมระบบ](#ภาพรวมระบบ)
- [คุณสมบัติเด่น](#คุณสมบัติเด่น)
- [ข้อมูลทางเทคนิค](#ข้อมูลทางเทคนิค)
- [รายการอุปกรณ์](#รายการอุปกรณ์)
- [การเชื่อมต่อวงจร](#การเชื่อมต่อวงจร)
- [การใช้งาน](#การใช้งาน)
- [Web Interface](#web-interface)
- [การแก้ปัญหา](#การแก้ปัญหา)

---

## 🎯 ภาพรวมระบบ

ระบบปลูกผักชีฟลั่งอัตโนมัติ RDTRC เป็นระบบไฮโดรโปนิกส์ที่ออกแบบมาเฉพาะสำหรับการปลูกผักชีฟลั่ง ด้วยการควบคุมสภาพแวดล้อมที่แม่นยำและระบบโภชนาการอัตโนมัติ

### 🏗️ สถาปัตยกรรมระบบ

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Mobile App    │    │  Web Interface  │    │  Blynk Cloud    │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         └───────────────────────┼───────────────────────┘
                                 │
         ┌───────────────────────▼───────────────────────┐
         │              ESP32 Controller                 │
         │        (Hydroponic IoT Hub)                   │
         └───────────────────────┬───────────────────────┘
                                 │
    ┌────────────────────────────┼────────────────────────────┐
    │                            │                            │
    ▼                            ▼                            ▼
┌─────────┐              ┌─────────────┐              ┌─────────────┐
│Sensors  │              │ Actuators   │              │ Display &   │
│- DHT22  │              │ - Pumps     │              │ Interface   │
│- pH     │              │ - LED       │              │ - LCD 16x2  │
│- EC     │              │ - Fans      │              │ - Buttons   │
│- CO2    │              │ - Heater    │              │             │
└─────────┘              └─────────────┘              └─────────────┘
```

---

## 🌟 คุณสมบัติเด่น

### 🌡️ การควบคุมสภาพแวดล้อมขั้นสูง
- **อุณหภูมิและความชื้น**: DHT22 sensor ความแม่นยำสูง
- **วัดค่า pH**: ควบคุมความเป็นกรด-ด่างของน้ำ (6.0-7.0)
- **วัดค่า EC**: ตรวจสอบความเข้มข้นของธาตุอาหาร
- **วัดค่า CO2**: เพิ่มประสิทธิภาพการสังเคราะห์แสง
- **วัดความเข้มแสง**: ควบคุม LED grow lights

### 💧 ระบบไฮโดรโปนิกส์อัจฉริยะ
- **ระบบหมุนเวียนน้ำ**: ปั๊มน้ำอัตโนมัติ
- **การผสมปุ๋ย**: ระบบฉีดปุ๋ยอัตโนมัติ
- **ควบคุม pH**: ปรับค่า pH ด้วยสารละลาย
- **ระบายอากาศ**: พัดลมหมุนเวียนอากาศ

### 🌿 เฉพาะสำหรับผักชีฟลั่ง
- **ช่วงค่าที่เหมาะสม**: กำหนดค่าเฉพาะผักชีฟลั่ง
- **ระยะการเจริญเติบโต**: 25-30 วัน เก็บเกี่ยวได้
- **ความต้องการแสง**: 12-16 ชั่วโมง/วัน
- **อุณหภูมิที่เหมาะสม**: 18-24°C

---

## 🔧 ข้อมูลทางเทคนิค

### 📊 ข้อกำหนดระบบ

| ข้อมูล | รายละเอียด |
|--------|------------|
| **ไมโครคอนโทรลเลอร์** | ESP32 Development Board |
| **แรงดันไฟฟ้า** | 12V DC / 5A |
| **การเชื่อมต่อ** | WiFi 802.11 b/g/n |
| **ช่วงอุณหภูมิ** | 18-24°C (เหมาะสำหรับผักชีฟลั่ง) |
| **ช่วงความชื้น** | 50-70% |
| **ช่วงค่า pH** | 6.0-7.0 |
| **ค่า EC** | 1.2-1.8 mS/cm |
| **ความเข้มแสง** | 200-400 μmol/m²/s |

### 🎛️ การกำหนดค่า Pin

```cpp
// Pin Definitions for Cilantro System
#define DHT_PIN 22                      // DHT22 Temperature & Humidity
#define CO2_SENSOR_PIN 23               // CO2 sensor (MH-Z19)
#define PH_SENSOR_PIN 32                // pH sensor (analog)
#define EC_SENSOR_PIN 33                // EC sensor (analog)
#define LIGHT_SENSOR_PIN 34             // Light sensor

// Actuator Pins
#define WATER_PUMP_PIN 5                // Main water pump
#define NUTRIENT_PUMP_PIN 17            // Nutrient pump
#define PH_UP_PUMP_PIN 16               // pH up solution pump
#define PH_DOWN_PUMP_PIN 4              // pH down solution pump
#define GROW_LIGHT_PIN 18               // LED grow lights
#define EXHAUST_FAN_PIN 19              // Exhaust fan
#define CIRCULATION_FAN_PIN 21          // Circulation fan

// System Control
#define STATUS_LED_PIN 2                // Status LED
#define BUZZER_PIN 15                   // Buzzer
#define RESET_BUTTON_PIN 0              // Reset button

// I2C for LCD
#define I2C_SDA 21
#define I2C_SCL 22

// Cilantro optimal values
#define CILANTRO_OPTIMAL_TEMP 21.0      // °C
#define CILANTRO_OPTIMAL_HUMIDITY 60.0  // %
#define CILANTRO_OPTIMAL_PH 6.5         // pH
#define CILANTRO_OPTIMAL_EC 1.5         // mS/cm
#define CILANTRO_OPTIMAL_CO2 800        // ppm
```

---

## 🛍️ รายการอุปกรณ์

### 📋 อุปกรณ์หลัก

| อุปกรณ์ | จำนวน | ราคาประมาณ | รายละเอียด |
|---------|--------|-------------|------------|
| **ESP32 Development Board** | 1 | ฿350-500 | ตัวควบคุมหลัก |
| **DHT22 Sensor** | 1 | ฿150-250 | วัดอุณหภูมิและความชื้น |
| **pH Sensor Kit** | 1 | ฿800-1200 | วัดค่าความเป็นกรด-ด่าง |
| **EC Sensor** | 1 | ฿600-900 | วัดความเข้มข้นธาตุอาหาร |
| **MH-Z19 CO2 Sensor** | 1 | ฿1000-1500 | วัดค่าคาร์บอนไดออกไซด์ |
| **BH1750 Light Sensor** | 1 | ฿80-120 | วัดความเข้มแสง |

### 🌿 อุปกรณ์ไฮโดรโปนิกส์

| อุปกรณ์ | จำนวน | ราคาประมาณ | รายละเอียด |
|---------|--------|-------------|------------|
| **Water Pump 12V** | 2 | ฿400-600 | ปั๊มน้ำหลัก + ปั๊มปุ๋ย |
| **Peristaltic Pump** | 3 | ฿900-1500 | ปั๊มสารละลาย pH |
| **LED Grow Light 50W** | 2 | ฿1000-1800 | ไฟ LED สำหรับปลูกพืช |
| **Exhaust Fan 12V** | 1 | ฿200-350 | พัดลมดูดอากาศ |
| **Circulation Fan** | 1 | ฿150-250 | พัดลมหมุนเวียนอากาศ |

### 💰 สรุปค่าใช้จ่าย

| ประเภท | ราคา (ต่ำสุด) | ราคา (สูงสุด) |
|--------|---------------|---------------|
| **อุปกรณ์หลัก** | ฿2,980 | ฿4,470 |
| **อุปกรณ์ไฮโดรโปนิกส์** | ฿2,650 | ฿4,550 |
| **รวม** | **฿5,630** | **฿9,020** |

---

## 🔌 การเชื่อมต่อวงจร

### 📐 Wiring Diagram

```
ESP32 Pin    │ Component           │ Description
─────────────┼─────────────────────┼─────────────────────
GPIO22       │ DHT22 Data          │ Temperature & Humidity
GPIO23       │ CO2 Sensor RX       │ CO2 level reading
GPIO32       │ pH Sensor Analog    │ pH level reading
GPIO33       │ EC Sensor Analog    │ EC level reading
GPIO34       │ Light Sensor        │ Light intensity

GPIO5        │ Water Pump Relay    │ Main water circulation
GPIO17       │ Nutrient Pump       │ Nutrient solution
GPIO16       │ pH Up Pump          │ pH adjustment up
GPIO4        │ pH Down Pump        │ pH adjustment down
GPIO18       │ LED Grow Lights     │ Plant lighting
GPIO19       │ Exhaust Fan         │ Air circulation out
GPIO21       │ Circulation Fan     │ Air circulation in

GPIO2        │ Status LED          │ System status
GPIO15       │ Buzzer              │ Alerts and notifications
```

---

## 🎮 การใช้งาน

### 🚀 การเริ่มต้นใช้งาน

**Boot Sequence**
```
╔════════════════╗
║ FW make by RDTRC║
║ Version 4.0     ║
╚════════════════╝
        ↓
╔════════════════╗
║ Cilantro System║
║ Initializing...║
╚════════════════╝
        ↓
╔════════════════╗
║ pH: 6.5 EC: 1.5║
║ Temp: 21°C OK  ║
╚════════════════╝
```

### 📱 การควบคุมผ่าน LCD

**หน้าหลัก (Main Screen)**
```
┌────────────────┐
│Cilantro v4.0   │
│T:21°C pH:6.5 OK│
└────────────────┘
```

**หน้าโภชนาการ (Nutrition Screen)**
```
┌────────────────┐
│EC: 1.5 mS/cm   │
│Nutrient: Good  │
└────────────────┘
```

---

## 🌐 Web Interface

### 🏠 หน้าหลัก (Cilantro Dashboard)

```html
┌─────────────────────────────────────┐
│ 🌿 RDTRC Cilantro Growing System    │
├─────────────────────────────────────┤
│ Status: ● Online                    │
│ Growth Day: 15/30                   │
│ Temperature: 21.0°C (Optimal)       │
│ Humidity: 60% (Good)                │
│ pH Level: 6.5 (Perfect)             │
│ EC Level: 1.5 mS/cm (Good)          │
│ CO2 Level: 800 ppm (Optimal)        │
│ Light: 300 μmol/m²/s (Good)         │
│                                     │
│ System Status:                      │
│ Water Pump: ● Running               │
│ Grow Lights: ● On (12h remaining)   │
│ Circulation: ● Active               │
│                                     │
│ [Manual Controls] [Settings]        │
└─────────────────────────────────────┘
```

---

## 🛠️ การแก้ปัญหา

### ❌ ปัญหาที่พบบ่อย

#### 1. **ค่า pH ไม่คงที่**
**การแก้ไข:**
```cpp
void stabilizePH() {
  float targetPH = 6.5;
  float currentPH = readPHSensor();
  
  if (currentPH < targetPH - 0.2) {
    // pH too low, add pH up solution
    digitalWrite(PH_UP_PUMP_PIN, HIGH);
    delay(5000); // 5 seconds
    digitalWrite(PH_UP_PUMP_PIN, LOW);
  } else if (currentPH > targetPH + 0.2) {
    // pH too high, add pH down solution
    digitalWrite(PH_DOWN_PUMP_PIN, HIGH);
    delay(3000); // 3 seconds
    digitalWrite(PH_DOWN_PUMP_PIN, LOW);
  }
}
```

#### 2. **ผักชีเจริญเติบโตช้า**
- ตรวจสอบค่า EC (ธาตุอาหาร)
- เพิ่มความเข้มแสง
- ตรวจสอบอุณหภูมิ

#### 3. **ใบเหลือง**
- ตรวจสอบค่า pH
- เพิ่มธาตุอาหารไนโตรเจน
- ตรวจสอบการระบายอากาศ

### 🔧 การบำรุงรักษา

#### รายวัน
- ✅ ตรวจสอบค่า pH และ EC
- ✅ ดูการเจริญเติบโตของผักชี
- ✅ ทำความสะอาดถาดปลูก

#### รายสัปดาห์
- ✅ เปลี่ยนน้ำในระบบ
- ✅ ทำความสะอาดเซ็นเซอร์
- ✅ ตรวจสอบปั๊มและพัดลม

---

## 🚀 การพัฒนาต่อ

### 💡 ไอเดียการพัฒนา

#### 🤖 AI การวิเคราะห์การเจริญเติบโต
```cpp
struct GrowthAnalysis {
  float growthRate;
  String healthStatus;
  int daysToHarvest;
  String recommendations;
};

GrowthAnalysis analyzeGrowth() {
  GrowthAnalysis analysis;
  
  // Calculate growth rate based on sensor data
  float currentBiomass = estimateBiomass();
  analysis.growthRate = (currentBiomass - previousBiomass) / 24; // per hour
  
  // Health assessment
  if (currentPH >= 6.0 && currentPH <= 7.0 && 
      currentEC >= 1.2 && currentEC <= 1.8 &&
      currentTemp >= 18 && currentTemp <= 24) {
    analysis.healthStatus = "Excellent";
  } else {
    analysis.healthStatus = "Needs attention";
  }
  
  // Harvest prediction
  analysis.daysToHarvest = calculateHarvestDate();
  
  return analysis;
}
```

---

<div align="center">

## 🌿 ขอบคุณที่เลือกใช้ RDTRC Cilantro System!

**ระบบปลูกผักชีฟลั่งไฮโดรโปนิกส์ที่ทันสมัย**

[![Get Started](https://img.shields.io/badge/Get%20Started-Now-green.svg?style=for-the-badge)](#การใช้งาน)
[![Documentation](https://img.shields.io/badge/Read%20Docs-blue.svg?style=for-the-badge)](#สารบัญ)

**Made with ❤️ for Sustainable Agriculture by RDTRC Team**

</div>
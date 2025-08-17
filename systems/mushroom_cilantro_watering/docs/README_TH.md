# 🍄🌿 ระบบรดน้ำเห็ดนางฟ้าและผักชีฟลั่งอัตโนมัติ

> **Firmware สร้างโดย: RDTRC**  
> **เวอร์ชัน: 3.0 - Enhanced with Blink Integration & Hotspot Capability**  
> **อัพเดท: 2024**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)](https://www.espressif.com/)

---

## 🎯 ภาพรวมระบบ

ระบบรดน้ำอัตโนมัติสำหรับการปลูกเห็ดนางฟ้าและผักชีฟลั่งพร้อมกัน ด้วยเทคโนโลยี IoT และการควบคุมผ่านมือถือ

### ✨ คุณสมบัติเด่น

- 🍄 **ระบบพ่นฝอยสำหรับเห็ดนางฟ้า** - ควบคุมความชื้นอัตโนมัติ 80-95%
- 🌿 **ระบบรดน้ำหยดสำหรับผักชีฟลั่ง** - ควบคุมความชื้นดิน 60-75%
- 📱 **ควบคุมผ่านมือถือ** - เชื่อมต่อ Blink Cloud
- 🌐 **เว็บอินเตอร์เฟซ** - จัดการระบบผ่านเบราว์เซอร์
- 🔄 **ทำงานแบบออฟไลน์** - ใช้งานได้แม้ไม่มีอินเทอร์เน็ต
- 🌡️ **ควบคุมสภาพแวดล้อม** - อุณหภูมิ, ความชื้น, แสง
- ⏰ **ตารางเวลาอัตโนมัติ** - รดน้ำตามเวลาที่กำหนด
- 📊 **ติดตามการเจริญเติบโต** - บันทึกข้อมูลและสถิติ

---

## 🛠️ ฮาร์ดแวร์ที่ใช้

### 📋 รายการอุปกรณ์หลัก

| อุปกรณ์ | จำนวน | รายละเอียด |
|---------|--------|------------|
| ESP32 Development Board | 1 | ตัวควบคุมหลัก |
| DHT22 Sensor | 2 | วัดอุณหภูมิและความชื้น |
| Soil Moisture Sensor | 1 | วัดความชื้นดิน |
| Water Level Sensor | 2 | วัดระดับน้ำในถัง |
| DS18B20 Temperature Probe | 1 | วัดอุณหภูมิน้ำ |
| Light Sensor (LDR) | 1 | วัดแสง |
| Water Pump (Misting) | 1 | ปั๊มพ่นฝอยสำหรับเห็ด |
| Water Pump (Drip) | 1 | ปั๊มรดน้ำสำหรับผักชี |
| Solenoid Valve | 2 | วาล์วควบคุมน้ำ |
| Relay Module 8-Channel | 1 | ควบคุมอุปกรณ์ไฟฟ้า |
| LED Grow Lights | 2 | ไฟ LED สำหรับปลูกพืช |
| Exhaust Fan | 1 | พัดลมดูดอากาศ |
| Circulation Fan | 1 | พัดลมหมุนเวียนอากาศ |
| Heating Element | 1 | ฮีตเตอร์ |
| Buzzer | 1 | สัญญาณเตือน |

### 🔌 การต่อสาย (Wiring Diagram)

```
ESP32 Pin Connections:
├── GPIO 18 → Mushroom Misting Pump Relay
├── GPIO 19 → Cilantro Water Pump Relay  
├── GPIO 20 → Mushroom Valve Relay
├── GPIO 21 → Cilantro Valve Relay
├── GPIO 22 → Humidity Fan Relay
├── GPIO 23 → Circulation Fan Relay
├── GPIO 25 → Mushroom LED Lights Relay
├── GPIO 26 → Cilantro LED Lights Relay
├── GPIO 27 → Heater Relay
├── GPIO 4  → DHT22 (Mushroom Area)
├── GPIO 5  → DHT22 (Cilantro Area)
├── GPIO 2  → DS18B20 Temperature Probe
├── A0      → Soil Moisture Sensor
├── A1      → Water Level (Mushroom Tank)
├── A2      → Water Level (Cilantro Tank)
├── A3      → Light Sensor (LDR)
├── GPIO 32 → Manual Control Button
├── GPIO 33 → Reset Button
├── GPIO 14 → Buzzer
└── GPIO 13 → Status LED
```

---

## 🚀 การติดตั้งและใช้งาน

### 1. 📥 การติดตั้ง Arduino IDE

1. ดาวน์โหลด [Arduino IDE](https://www.arduino.cc/en/software)
2. เพิ่ม ESP32 Board Manager:
   - File → Preferences
   - Additional Boards Manager URLs: `https://dl.espressif.com/dl/package_esp32_index.json`
   - Tools → Board → Boards Manager → ค้นหา "ESP32" และติดตั้ง

### 2. 📚 ติดตั้ง Libraries

เปิด Library Manager (Ctrl+Shift+I) และติดตั้ง:

```
- WiFi (ESP32) - Built-in
- WebServer (ESP32) - Built-in  
- ArduinoJson by Benoit Blanchon
- DHT sensor library by Adafruit
- OneWire by Jim Studt
- DallasTemperature by Miles Burton
- ArduinoOTA (ESP32) - Built-in
```

### 3. 🔧 การอัพโหลด Firmware

1. เชื่อมต่อ ESP32 เข้ากับคอมพิวเตอร์
2. เลือก Board: "ESP32 Dev Module"
3. เลือก Port ที่ถูกต้อง
4. อัพโหลด `enhanced_mushroom_cilantro_system.ino`

### 4. 🌐 การตั้งค่า WiFi

**วิธีที่ 1: ผ่าน Hotspot**
1. หา WiFi ชื่อ "MushroomCilantro_System"
2. รหัสผ่าน: "RDTRC2024"
3. เปิดเบราว์เซอร์ไป 192.168.4.1
4. ตั้งค่า WiFi ในหน้าเว็บ

**วิธีที่ 2: ผ่าน Serial Monitor**
1. เปิด Serial Monitor (115200 baud)
2. ระบบจะแสดง IP Address เมื่อเชื่อมต่อ WiFi สำเร็จ

---

## 🎛️ การใช้งาน

### 🖥️ Web Interface

เข้าถึงผ่าน IP Address ของ ESP32:

**หน้าหลัก:**
- 📊 แสดงสถานะเห็ดนางฟ้าและผักชีฟลั่ง
- 🎮 ควบคุมระบบพ่นฝอย/รดน้ำ
- 💡 เปิด/ปิดไฟ LED
- ⚠️ แสดงการแจ้งเตือน

**การควบคุม:**
```
🍄 เห็ดนางฟ้า:
├── Start Misting - เริ่มพ่นฝอย 30 วินาที
├── Stop Misting - หยุดพ่นฝอย
└── Toggle Lights - เปิด/ปิดไฟ

🌿 ผักชีฟลั่ง:
├── Start Watering - เริ่มรดน้ำ 3 นาที  
├── Stop Watering - หยุดรดน้ำ
└── Toggle Lights - เปิด/ปิดไฟ
```

### 📱 Mobile App (Blink)

1. ดาวน์โหลด Blink App
2. สมัครสมาชิกและเข้าสู่ระบบ
3. เพิ่มอุปกรณ์ใหม่
4. ควบคุมระบบผ่านมือถือ

### ⏰ ตารางเวลาอัตโนมัติ

**เห็ดนางฟ้า (Misting Schedule):**
- 06:00 - พ่นฝอย 30 วินาที + เปิดไฟ
- 10:00 - พ่นฝอย 30 วินาที
- 14:00 - พ่นฝอย 30 วินาที  
- 18:00 - พ่นฝอย 30 วินาที + ปิดไฟ

**ผักชีฟลั่ง (Watering Schedule):**
- 07:00 - รดน้ำ 3 นาที + เปิดไฟ
- 19:00 - ปิดไฟ

---

## 🌱 ข้อมูลการปลูก

### 🍄 เห็ดนางฟ้า (Oyster Mushrooms)

**สภาพแวดล้อมที่เหมาะสม:**
- 🌡️ อุณหภูมิ: 20-25°C
- 💧 ความชื้น: 80-95%
- 💡 แสง: แสงอ่อน 12 ชั่วโมง/วัน
- 🌬️ การระบายอากาศ: ดี

**ขั้นตอนการเจริญเติบโต:**
1. **Incubation (0-7 วัน)** - ระยะบ่มเชื้อ
2. **Pinning (7-14 วัน)** - เริ่มมีหัวเห็ดเล็ก
3. **Fruiting (14-21 วัน)** - เห็ดเจริญเติบโต
4. **Harvest (21+ วัน)** - พร้อมเก็บเกี่ยว

### 🌿 ผักชีฟลั่ง (Cilantro)

**สภาพแวดล้อมที่เหมาะสม:**
- 🌡️ อุณหภูมิ: 17-27°C
- 💧 ความชื้นดิน: 60-75%
- 💡 แสง: แสงแรง 6-8 ชั่วโมง/วัน
- 🌱 pH ดิน: 6.0-7.0

**ขั้นตอนการเจริญเติบโต:**
1. **Seed (0-7 วัน)** - ระยะเมล็ด
2. **Sprout (7-14 วัน)** - ระยะงอก
3. **Vegetative (14-21 วัน)** - ระยะใบ
4. **Mature (21+ วัน)** - พร้อมเก็บเกี่ยว

---

## 🔧 การบำรุงรักษา

### 📅 รายวัน
- ✅ ตรวจสอบระดับน้ำในถัง
- ✅ ตรวจสอบสถานะระบบผ่านเว็บ
- ✅ ทำความสะอาดหัวพ่นฝอย

### 📅 รายสัปดาห์  
- ✅ ทำความสะอาดเซนเซอร์
- ✅ ตรวจสอบการทำงานของปั๊ม
- ✅ เปลี่ยนน้ำในถัง

### 📅 รายเดือน
- ✅ ตรวจสอบการเชื่อมต่อสาย
- ✅ ทำความสะอาดพัดลม
- ✅ อัพเดท firmware (ถ้ามี)

---

## ⚠️ การแก้ไขปัญหา

### 🔴 ปัญหาที่พบบ่อย

**1. ระบบไม่เชื่อมต่อ WiFi**
```
- ตรวจสอบชื่อและรหัส WiFi
- รีสตาร์ทระบบ (กดปุ่ม Reset)
- ใช้ Hotspot Mode แทน
```

**2. เซนเซอร์อ่านค่าไม่ถูกต้อง**
```
- ทำความสะอาดเซนเซอร์
- ตรวจสอบการต่อสาย
- เปลี่ยนเซนเซอร์ใหม่
```

**3. ปั๊มไม่ทำงาน**
```
- ตรวจสอบระดับน้ำ
- ตรวจสอบ Relay
- ทำความสะอาดปั๊ม
```

**4. ระบบแจ้งเตือนตลอดเวลา**
```
- กด "Clear Alert" ในเว็บ
- ตรวจสอบสาเหตุการแจ้งเตือน
- รีสตาร์ทระบบ
```

### 🔧 Serial Monitor Debug

เปิด Serial Monitor (115200 baud) เพื่อดูข้อมูล Debug:

```
=====================================
     FW made by RDTRC
  Enhanced Mushroom & Cilantro System
         Version 3.0.0
=====================================

Sensors initialized:
- DHT22 sensors for humidity/temperature
- DS18B20 temperature probe
- Soil moisture sensor
- Water level sensors
- Light sensor

WiFi connected!
IP address: 192.168.1.100

System initialization complete!
Ready to grow mushrooms and cilantro!
```

---

## 📊 API Reference

### GET /api/status
ดึงสถานะปัจจุบันของระบบ

```json
{
  "mushroom": {
    "humidity": 85.5,
    "temperature": 22.3,
    "mistingActive": false,
    "lightOn": true,
    "waterLevel": 75.2,
    "fruitingStage": 2,
    "nextMisting": "Next: 2 hours"
  },
  "cilantro": {
    "soilMoisture": 68.1,
    "temperature": 24.1,
    "humidity": 65.3,
    "wateringActive": false,
    "lightOn": true,
    "waterLevel": 82.4,
    "growthStage": 1,
    "nextWatering": "Next: Tomorrow 7:00 AM"
  },
  "system": {
    "running": true,
    "manualMode": false,
    "alertActive": false,
    "alertMessage": "",
    "uptime": 86400000,
    "heaterOn": false,
    "exhaustFanOn": false,
    "circulationFanOn": true
  }
}
```

### POST /api/control
ควบคุมระบบ

**Parameters:**
- `action`: คำสั่งที่ต้องการ

**คำสั่งที่รддерж:**
```
- start_misting: เริ่มพ่นฝอย
- stop_misting: หยุดพ่นฝอย  
- start_watering: เริ่มรดน้ำ
- stop_watering: หยุดรดน้ำ
- toggle_mushroom_lights: เปิด/ปิดไฟเห็ด
- toggle_cilantro_lights: เปิด/ปิดไฟผักชี
- clear_alert: ล้างการแจ้งเตือน
```

---

## 🎯 การปรับแต่งขั้นสูง

### 🔧 แก้ไขค่าพารามิเตอร์

แก้ไขค่าในไฟล์ `.ino`:

```cpp
// เห็ดนางฟ้า
#define MUSHROOM_TARGET_HUMIDITY_MIN 80.0    // ความชื้นต่ำสุด
#define MUSHROOM_TARGET_HUMIDITY_MAX 95.0    // ความชื้นสูงสุด
#define MUSHROOM_TARGET_TEMP_MIN 20.0        // อุณหภูมิต่ำสุด
#define MUSHROOM_TARGET_TEMP_MAX 25.0        // อุณหภูมิสูงสุด

// ผักชีฟลั่ง  
#define CILANTRO_TARGET_MOISTURE_MIN 60.0    // ความชื้นดินต่ำสุด
#define CILANTRO_TARGET_MOISTURE_MAX 75.0    // ความชื้นดินสูงสุด
#define CILANTRO_TARGET_TEMP_MIN 17.0        // อุณหภูมิต่ำสุด
#define CILANTRO_TARGET_TEMP_MAX 27.0        // อุณหภูมิสูงสุด

// ระยะเวลา
#define MISTING_DURATION 30000               // พ่นฝอย 30 วินาที
#define WATERING_DURATION 180000             // รดน้ำ 3 นาที
```

### 📅 แก้ไขตารางเวลา

```cpp
void initializeSchedules() {
  // เห็ดนางฟ้า - พ่นฝอยทุก 4 ชั่วโมง
  schedules[0] = {6, 0, 0, 30, true, true, "Morning misting + lights on"};
  schedules[1] = {10, 0, 0, 30, true, false, "Mid-morning misting"};
  schedules[2] = {14, 0, 0, 30, true, false, "Afternoon misting"};
  schedules[3] = {18, 0, 0, 30, true, true, "Evening misting + lights off"};
  
  // ผักชีฟลั่ง - รดน้ำทุกวัน
  schedules[4] = {7, 0, 1, 180, true, true, "Morning cilantro watering + lights on"};
  schedules[5] = {19, 0, 1, 0, true, true, "Evening lights off for cilantro"};
}
```

---

## 🤝 การสนับสนุน

### 📞 ติดต่อ
- **Email:** support@rdtrc.com
- **GitHub:** [RDTRC Repository](https://github.com/rdtrc)
- **Facebook:** RDTRC Thailand

### 🐛 รายงานบัค
กรุณารายงานปัญหาผ่าน GitHub Issues พร้อมข้อมูล:
- เวอร์ชัน Firmware
- ข้อความ Error จาก Serial Monitor
- ขั้นตอนการทำซ้ำปัญหา

### 💡 ขอคุณสมบัติใหม่
เสนอแนะคุณสมบัติใหม่ผ่าน GitHub Discussions

---

## 📄 License

MIT License - ใช้งานได้ฟรีสำหรับทุกวัตถุประสงค์

---

## 🙏 กิตติกรรมประกาศ

**พัฒนาโดย:** RDTRC Team  
**ขอบคุณ:** Arduino Community, ESP32 Community, และเกษตรกรไทยทุกท่าน

---

*🌱 ปลูกอย่างชาญฉลาด เก็บเกี่ยวอย่างยั่งยืน 🌱*
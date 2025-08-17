# 🤖 RDTRC IoT Systems Collection
## ชุดระบบ IoT ครบครัน สำหรับการเกษตรและการเลี้ยงสัตว์

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)](https://www.espressif.com/)
[![Firmware](https://img.shields.io/badge/Firmware-RDTRC-red.svg)](https://github.com)

> **Firmware made by: RDTRC**  
> **Version: 2.0**  
> **Created: 2024**

---

## 📋 สารบัญ

- [ภาพรวมโปรเจกต์](#ภาพรวมโปรเจกต์)
- [ระบบที่รวมอยู่](#ระบบที่รวมอยู่)
- [โครงสร้างโปรเจกต์](#โครงสร้างโปรเจกต์)
- [คุณสมบัติร่วม](#คุณสมบัติร่วม)
- [การติดตั้งเบื้องต้น](#การติดตั้งเบื้องต้น)
- [คู่มือการใช้งาน](#คู่มือการใช้งาน)
- [การพัฒนาและขยายระบบ](#การพัฒนาและขยายระบบ)
- [การสนับสนุน](#การสนับสนุน)

---

## 🌟 ภาพรวมโปรเจกต์

ชุดระบบ IoT ครบครันที่ออกแบบมาเพื่อการใช้งานในด้านการเกษตรและการเลี้ยงสัตว์ ทุกระบบใช้โครงสร้างพื้นฐานเดียวกัน พร้อมด้วย boot screen ที่แสดง **"FW make by RDTRC"** และมีฟีเจอร์ครบครันสำหรับการควบคุมผ่านมือถือและเว็บไซต์

### 🎯 วัตถุประสงค์
- **อัตโนมัติ**: ลดการดูแลด้วยตนเองในงานประจำวัน
- **อัจฉริยะ**: ใช้เซ็นเซอร์ตัดสินใจอัตโนมัติ
- **เชื่อมต่อ**: ควบคุมและติดตามผ่านมือถือได้ทุกที่
- **ประหยัด**: ใช้ทรัพยากรอย่างมีประสิทธิภาพ
- **ขยายได้**: สามารถเพิ่มฟีเจอร์และระบบใหม่ได้ง่าย

---

## 🎛️ ระบบที่รวมอยู่

### 1. 🐱 ระบบให้อาหารแมวอัตโนมัติ
**Path:** `systems/cat_feeding/`

**คุณสมบัติหลัก:**
- ✅ ให้อาหารตามเวลาที่กำหนด (สูงสุด 6 เวลา/วัน)
- ✅ วัดน้ำหนักอาหารที่เหลือด้วย Load Cell
- ✅ ตรวจจับแมวด้วย PIR Sensor
- ✅ ควบคุมปริมาณอาหาร 5-100 กรัม
- ✅ แจ้งเตือนเมื่ออาหารเหลือน้อย
- ✅ ป้องกันการให้อาหารซ้ำ

**ฮาร์ดแวร์:**
- ESP32, Servo SG90, Load Cell + HX711
- PIR HC-SR501, LCD 16x2, RGB LED
- Buzzer, ปุ่มควบคุม 2 ปุ่ม

### 2. 🐦 ระบบให้อาหารนกอัตโนมัติ  
**Path:** `systems/bird_feeding/`

**คุณสมบัติหลัก:**
- ✅ ให้อาหารและน้ำแยกกัน
- ✅ ตรวจสอบระดับอาหารด้วย Ultrasonic
- ✅ ตรวจสอบระดับน้ำด้วย ADC
- ✅ ตรวจจับนกด้วย PIR Sensor
- ✅ ปั๊มน้ำอัตโนมัติ
- ✅ กำหนดเวลาให้อาหาร/น้ำแยกกัน

**ฮาร์ดแวร์:**
- ESP32, Servo x2, ปั๊มน้ำ
- Ultrasonic HC-SR04, เซ็นเซอร์น้ำ
- PIR, LCD 16x2, RGB LED, Buzzer

### 3. 🍅 ระบบรดน้ำมะเขือเทศอัตโนมัติ
**Path:** `systems/tomato_watering/`

**คุณสมบัติหลัก:**
- ✅ รดน้ำตามเวลาและความชื้นดิน
- ✅ ตรวจสอบความชื้นดินด้วย Soil Moisture Sensor
- ✅ ตรวจสอบระดับน้ำในถัง
- ✅ วัดอุณหภูมิและความชื้นอากาศ (DHT22)
- ✅ ปั๊มน้ำและวาล์วควบคุม
- ✅ เซ็นเซอร์วัดการไหลของน้ำ

**ฮาร์ดแวร์:**
- ESP32, ปั๊มน้ำ, วาล์วน้ำ
- Soil Moisture Sensor, DHT22
- เซ็นเซอร์ระดับน้ำ, Flow Sensor
- LCD 16x2, RGB LED, Buzzer

---

## 📁 โครงสร้างโปรเจกต์

```
RDTRC-IoT-Systems/
├── systems/
│   ├── common/
│   │   ├── base_code/
│   │   │   ├── system_base.h          # ไฟล์หลักระบบพื้นฐาน
│   │   │   └── system_base.cpp        # Implementation
│   │   ├── docs/
│   │   └── libraries/
│   │
│   ├── cat_feeding/
│   │   ├── code/
│   │   │   ├── cat_feeding_system.ino # โค้ดหลัก (ใหม่)
│   │   │   └── cat_feeding_system_old.ino # โค้ดเดิม
│   │   ├── docs/
│   │   │   ├── CAT_FEEDING_README_TH.md
│   │   │   └── CAT_FEEDING_SETUP_GUIDE_TH.md
│   │   ├── wiring/
│   │   │   └── wiring_diagram_cat_feeding.txt
│   │   └── libraries/
│   │       └── libraries_cat_feeding.txt
│   │
│   ├── bird_feeding/
│   │   ├── code/
│   │   │   ├── bird_feeding_system.ino # โค้ดหลัก (ใหม่)
│   │   │   ├── bird_feeding_system_old.ino
│   │   │   ├── bird_feeding_quick_test.ino
│   │   │   └── bird_feeding_test.ino
│   │   ├── docs/
│   │   │   └── README_bird_feeding.md
│   │   ├── wiring/
│   │   │   └── wiring_diagram_bird_feeding.txt
│   │   └── libraries/
│   │       └── libraries_bird_feeding.txt
│   │
│   └── tomato_watering/
│       ├── code/
│       │   ├── tomato_watering_system.ino # โค้ดหลัก (ใหม่)
│       │   ├── tomato_system_old.ino
│       │   ├── cilantro_watering_system.ino
│       │   ├── rose_watering_system.ino
│       │   └── mushroom_system.ino
│       ├── docs/
│       │   └── README_tomato_system.md
│       ├── wiring/
│       │   └── wiring_diagram_tomato.txt
│       └── libraries/
│           └── libraries_tomato.txt
│
├── MASTER_README.md                    # เอกสารนี้
├── BIRD_FEEDING_CHECKLIST_TH.md
├── CONNECTION_DIAGRAMS_TH.md
├── ENHANCED_FEATURES_TH.md
├── LIBRARIES_INSTALLATION_TH.md
├── README_COMPLETE_SETUP_TH.md
├── SETUP_GUIDE_TH.md
└── WORKING_DOCUMENTATION_TH.md
```

---

## 🔧 คุณสมบัติร่วม

### 🖥️ Boot Screen
ทุกระบบจะแสดง boot screen เมื่อเริ่มต้น:
```
FW make by RDTRC
Version 2.0

[System Name]
Initializing...
................
```

### 📱 การควบคุมแบบครบครัน
1. **Web Interface** - เข้าถึงผ่าน IP Address
2. **Blynk Mobile App** - ควบคุมผ่านมือถือ
3. **Physical Buttons** - ปุ่มควบคุมบนตัวเครื่อง
4. **API Endpoints** - สำหรับการพัฒนาต่อ

### 🔄 ระบบพื้นฐานร่วม
- **SystemBase Class** - คลาสพื้นฐานสำหรับทุกระบบ
- **WiFi Management** - การจัดการการเชื่อมต่อ WiFi
- **RTC Support** - นาฬิกาเรียลไทม์
- **LCD Display** - แสดงข้อมูลบนหน้าจอ
- **LED Status** - ไฟแสดงสถานะ RGB
- **Sound Alerts** - เสียงแจ้งเตือน
- **EEPROM Storage** - บันทึกการตั้งค่า
- **Error Handling** - จัดการข้อผิดพลาด

### 🌐 API Endpoints ร่วม
```
GET  /                 - หน้าหลัก
GET  /info            - ข้อมูลระบบ
POST /restart         - รีสตาร์ทระบบ
POST /reset           - รีเซ็ตการตั้งค่า
GET  /api/status      - สถานะระบบ (JSON)
GET  /api/config      - การตั้งค่า (JSON)
```

---

## 🚀 การติดตั้งเบื้องต้น

### ขั้นตอนที่ 1: เตรียมสภาพแวดล้อม

1. **ติดตั้ง Arduino IDE 2.x**
   ```
   ดาวน์โหลดจาก: https://www.arduino.cc/en/software
   ```

2. **เพิ่ม ESP32 Board Package**
   ```
   File → Preferences
   Additional Board Manager URLs:
   https://dl.espressif.com/dl/package_esp32_index.json
   
   Tools → Board → Boards Manager
   ค้นหา "ESP32" → Install
   ```

3. **ติดตั้งไลบรารีพื้นฐาน**
   ```
   Tools → Manage Libraries
   
   ไลบรารีที่จำเป็น:
   - ArduinoJson (v6.21.3+)
   - RTClib (v2.1.1+)
   - LiquidCrystal I2C (v1.1.2+)
   - BlynkSimpleEsp32 (v1.3.2+)
   ```

### ขั้นตอนที่ 2: เลือกระบบที่ต้องการ

1. **ระบบให้อาหารแมว**
   ```
   เปิดไฟล์: systems/cat_feeding/code/cat_feeding_system.ino
   ไลบรารีเพิ่มเติม: ESP32Servo, HX711
   ```

2. **ระบบให้อาหารนก**
   ```
   เปิดไฟล์: systems/bird_feeding/code/bird_feeding_system.ino
   ไลบรารีเพิ่มเติม: ESP32Servo
   ```

3. **ระบบรดน้ำมะเขือเทศ**
   ```
   เปิดไฟล์: systems/tomato_watering/code/tomato_watering_system.ino
   ไลบรารีเพิ่มเติม: DHT sensor library
   ```

### ขั้นตอนที่ 3: แก้ไขการตั้งค่า

```cpp
// แก้ไขใน code แต่ละระบบ
const char* ssid = "ชื่อ_WiFi_ของคุณ";
const char* password = "รหัสผ่าน_WiFi";
const char* blynk_token = "Auth_Token_จาก_Blynk";
```

### ขั้นตอนที่ 4: อัพโหลดและทดสอบ

1. **ตั้งค่า Board**
   ```
   Board: ESP32 Dev Module
   Port: เลือกพอร์ตที่เชื่อมต่อ
   Upload Speed: 921600
   ```

2. **อัพโหลดโค้ด**
   ```
   กด Verify (✓) → กด Upload (→)
   ```

3. **ตรวจสอบ Serial Monitor**
   ```
   Baud Rate: 115200
   ดูข้อความ boot sequence
   ```

---

## 📖 คู่มือการใช้งาน

### 🌐 Web Interface

1. **เข้าถึงหน้าเว็บ**
   ```
   http://[IP_Address_ของ_ESP32]
   ```

2. **ฟีเจอร์หลัก**
   - 📊 **Dashboard**: ดูสถานะปัจจุบัน
   - 🎛️ **Control Panel**: ควบคุมระบบ
   - ⚙️ **Settings**: ตั้งค่าต่างๆ
   - 📈 **Status**: ข้อมูลระบบแบบละเอียด

### 📱 Blynk Mobile App

1. **ตั้งค่า Template**
   - ไปที่ https://blynk.cloud/
   - สร้าง Template ใหม่
   - เลือก Hardware: ESP32
   - Connection: WiFi

2. **Virtual Pins แต่ละระบบ**
   
   **Cat Feeding:**
   ```
   V0  → น้ำหนักอาหาร (Value Display)
   V1  → จำนวนครั้ง (Value Display)  
   V3  → ตรวจพบแมว (LED)
   V10 → ปุ่มให้อาหาร (Button)
   V11 → เปิด/ปิดระบบ (Switch)
   ```
   
   **Bird Feeding:**
   ```
   V0  → ระยะอาหาร (Value Display)
   V1  → ระดับน้ำ (Value Display)
   V3  → ตรวจพบนก (LED)
   V10 → ปุ่มให้อาหาร (Button)
   V11 → ปุ่มให้น้ำ (Button)
   ```
   
   **Tomato Watering:**
   ```
   V0  → ความชื้นดิน (Value Display)
   V1  → ระดับน้ำ (Value Display)
   V2  → อุณหภูมิ (Value Display)
   V10 → ปุ่มรดน้ำ (Button)
   V11 → ปุ่มหยุด (Button)
   ```

### 🔘 Physical Controls

**ปุ่มควบคุมทุกระบบ:**
- **Manual Action Button**: ทำงานหลักด้วยตนเอง
- **Stop/Reset Button**: หยุดหรือรีเซ็ต
- **System Reset**: รีเซ็ตการตั้งค่าทั้งหมด

### 📊 การตีความสัญญาณ LED

```
🟢 เขียว    → ระบบทำงานปกติ
🔴 แดง      → ข้อผิดพลาดหรือเตือน
🔵 น้ำเงิน   → ตรวจพบสัตว์/การทำงาน
🟠 ส้ม      → กำลังดำเนินการ
🟡 เหลือง   → คำเตือน (WiFi, เซ็นเซอร์)
```

---

## 🛠️ การพัฒนาและขยายระบบ

### 🔧 การสร้างระบบใหม่

1. **สร้าง Class ใหม่**
   ```cpp
   #include "../../common/base_code/system_base.h"
   
   class MyNewSystem : public SystemBase {
   public:
     MyNewSystem() : SystemBase("My System") {}
     
     void setupSystem() override {
       // ตั้งค่าเฉพาะระบบ
     }
     
     void runSystem() override {
       // ลูปการทำงานหลัก
     }
     
     String getSystemStatus() override {
       // ส่งคืนสถานะระบบ
       return "{}";
     }
   };
   ```

2. **เพิ่ม API Endpoints**
   ```cpp
   void setupMyWebServer() {
     server.on("/api/my/status", HTTP_GET, []() {
       // Handle status request
     });
     
     server.on("/api/my/action", HTTP_POST, []() {
       // Handle action request
     });
   }
   ```

### 📦 การเพิ่มฟีเจอร์

1. **เซ็นเซอร์ใหม่**
   ```cpp
   void checkNewSensor() {
     // อ่านค่าเซ็นเซอร์
     float value = analogRead(SENSOR_PIN);
     
     // ประมวลผล
     processValue(value);
     
     // ส่งแจ้งเตือนถ้าจำเป็น
     if (value > THRESHOLD) {
       sendAlert("Sensor alert!");
     }
   }
   ```

2. **การแจ้งเตือนใหม่**
   ```cpp
   void sendCustomAlert(String message) {
     // Blynk notification
     if (status.blynk_connected) {
       Blynk.notify(message);
     }
     
     // LED และเสียง
     blinkLED(255, 0, 0, 3);
     playErrorSound();
   }
   ```

### 🌐 การขยาย Web Interface

1. **เพิ่มหน้าใหม่**
   ```cpp
   server.on("/custom", HTTP_GET, []() {
     String html = R"(
     <!DOCTYPE html>
     <html>
     <head><title>Custom Page</title></head>
     <body>
       <h1>My Custom Page</h1>
       <!-- Custom content -->
     </body>
     </html>
     )";
     server.send(200, "text/html", html);
   });
   ```

2. **API สำหรับข้อมูลเพิ่มเติม**
   ```cpp
   server.on("/api/custom/data", HTTP_GET, []() {
     DynamicJsonDocument doc(512);
     doc["custom_value"] = myCustomValue;
     doc["timestamp"] = millis();
     
     String output;
     serializeJson(doc, output);
     server.send(200, "application/json", output);
   });
   ```

---

## 📞 การสนับสนุน

### 📚 เอกสารเพิ่มเติม

- **การติดตั้งรายละเอียด**: `LIBRARIES_INSTALLATION_TH.md`
- **คู่มือการเชื่อมต่อ**: `CONNECTION_DIAGRAMS_TH.md`
- **ฟีเจอร์ขั้นสูง**: `ENHANCED_FEATURES_TH.md`
- **การตั้งค่าครบครัน**: `README_COMPLETE_SETUP_TH.md`

### 🌐 แหล่งข้อมูลออนไลน์

- **Arduino Forum**: https://forum.arduino.cc/
- **ESP32 Documentation**: https://docs.espressif.com/
- **Blynk Documentation**: https://docs.blynk.io/
- **GitHub Repository**: [สำหรับ Source Code และ Issues]

### 🐛 การรายงานปัญหา

หากพบปัญหาหรือต้องการแนะนำฟีเจอร์ใหม่:

1. ตรวจสอบเอกสารที่เกี่ยวข้อง
2. ลองแก้ไขตามคู่มือ Troubleshooting
3. รายงานปัญหาพร้อมรายละเอียด:
   - รุ่นฮาร์ดแวร์ที่ใช้
   - เวอร์ชันโค้ด
   - ขั้นตอนการทำซ้ำปัญหา
   - ข้อความ Error
   - ภาพหน้าจอ (ถ้าเกี่ยวข้อง)

### 🤝 การมีส่วนร่วม

ยินดีรับการมีส่วนร่วมจากชุมชน:

- 🔧 **Code Contribution**: ส่ง Pull Request
- 📝 **Documentation**: ปรับปรุงเอกสาร
- 🐛 **Bug Reports**: รายงานปัญหา
- 💡 **Feature Requests**: แนะนำฟีเจอร์ใหม่
- 🌐 **Translation**: แปลเอกสารเป็นภาษาอื่น

---

## 📊 สรุปคุณสมบัติแต่ละระบบ

| ระบบ | เซ็นเซอร์หลัก | Actuator | ฟีเจอร์พิเศษ | ราคาประมาณ |
|------|---------------|----------|---------------|-------------|
| **Cat Feeding** | Load Cell, PIR | Servo Motor | วัดน้ำหนัก, ตรวจจับแมว | ฿1,500-2,500 |
| **Bird Feeding** | Ultrasonic, Water Level, PIR | Servo x2, Pump | อาหาร+น้ำ, ตรวจจับนก | ฿1,800-2,800 |
| **Tomato Watering** | Soil Moisture, DHT22, Water Level | Pump, Valve | ตรวจดิน, วัดสภาพอากาศ | ฿2,000-3,000 |

---

## 🎯 Roadmap

### Version 2.1 (Q2 2024)
- [ ] ESP32-CAM integration สำหรับทุกระบบ
- [ ] SD Card data logging
- [ ] Email notifications
- [ ] Improved web dashboard

### Version 2.2 (Q3 2024)
- [ ] Telegram bot integration
- [ ] Voice control (Google Assistant/Alexa)
- [ ] Mobile app (React Native)
- [ ] Multi-system dashboard

### Version 3.0 (Q4 2024)
- [ ] AI-powered recommendations
- [ ] Weather integration
- [ ] Cloud-based analytics
- [ ] Commercial hardware kit

---

## 📄 ลิขสิทธิ์

### MIT License

```
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

## 🙏 ขอบคุณ

ขอบคุณผู้ที่มีส่วนร่วมในการพัฒนาระบบเหล่านี้:

- **Core Development**: RDTRC Team
- **Testing**: Community Beta Testers
- **Documentation**: Contributors
- **Hardware Design**: Electronics Engineers

### สนับสนุนโปรเจกต์

หากโปรเจกต์นี้มีประโยชน์สำหรับคุณ:

- ⭐ ให้ Star บน GitHub
- 🔄 Share ให้เพื่อนๆ ที่สนใจ
- 💰 [Donate](https://example.com/donate) สนับสนุนการพัฒนา
- 📝 เขียน Review หรือ Blog post
- 🤝 เข้าร่วมชุมชน

---

<div align="center">

## 🚀 เริ่มต้นใช้งานระบบของคุณวันนี้!

**Made with ❤️ by RDTRC Team**

[![Built with Arduino](https://img.shields.io/badge/Built%20with-Arduino-teal.svg)](https://www.arduino.cc/)
[![Powered by ESP32](https://img.shields.io/badge/Powered%20by-ESP32-red.svg)](https://www.espressif.com/)
[![IoT with Blynk](https://img.shields.io/badge/IoT%20with-Blynk-blue.svg)](https://blynk.io/)

**Firmware made by RDTRC - Empowering Smart Agriculture & Pet Care**

</div>
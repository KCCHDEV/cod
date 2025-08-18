# 🤖 RDTRC Complete Standalone IoT Systems
## ระบบ IoT แยกส่วน แต่ละระบบสมบูรณ์ในตัวเอง - เวอร์ชัน 4.0

> **Firmware made by: RDTRC**  
> **Version: 4.0 - Complete Standalone Systems**  
> **Updated: 2024**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)](https://www.espressif.com/)
[![Firmware](https://img.shields.io/badge/Firmware-RDTRC_v4.0-red.svg)](https://github.com)

---

## 🎉 **สิ่งที่เปลี่ยนแปลงใหม่ในเวอร์ชัน 4.0**

### ✨ Complete Standalone Architecture
- 🏗️ **แต่ละระบบเป็นอิสระสมบูรณ์** - ไม่ต้องพึ่งพา Central Coordinator
- 📱 **Web Interface ในตัว** - แต่ละระบบมีหน้าเว็บควบคุมเป็นของตัวเอง
- 🔗 **Blynk Integration** - เชื่อมต่อ Mobile App โดยตรง
- 🌐 **Hotspot Mode** - สร้าง WiFi เป็นของตัวเองเมื่อไม่มีเน็ต
- 💾 **Data Logging** - บันทึกข้อมูลใน SPIFFS ภายในเครื่อง
- 📱 **LINE Notifications** - แจ้งเตือนผ่าน LINE Notify
- 🔄 **OTA Updates** - อัพเดทผ่านเครือข่าย
- ⚙️ **Advanced Controls** - ควบคุมขั้นสูงและการตั้งค่าละเอียด

---

## 🎛️ ระบบที่พร้อมใช้งาน

### 1. 🐱 Cat Feeding Complete System
**📂 ไฟล์:** [`cat_feeding_complete_system.ino`](cat_feeding_complete_system.ino)

**✨ Features:**
- ✅ ระบบให้อาหารแมวอัตโนมัติครบครัน
- ✅ ตารางให้อาหาร 6 เวลา/วัน (ปรับได้)
- ✅ Load Cell วัดน้ำหนักอาหาร (HX711)
- ✅ PIR Sensor ตรวจจับแมว
- ✅ Ultrasonic วัดระดับอาหารในกล่อง
- ✅ Web Interface สำหรับควบคุม
- ✅ Blynk App Integration
- ✅ LINE Notifications
- ✅ Hotspot Mode (RDTRC_CatFeeder)
- ✅ Data Logging & Statistics
- ✅ OTA Updates
- ✅ Manual Override Controls
- ✅ Scale Calibration System
- ✅ Maintenance Mode

**🔧 Hardware Requirements:**
- ESP32 Development Board
- Servo Motor (SG90)
- Load Cell + HX711 Module
- PIR Motion Sensor
- Ultrasonic Sensor (HC-SR04)
- Buzzer
- LED
- Push Buttons (2x)

**🌐 Access:**
- Web: `http://cat-feeder.local`
- Hotspot: `RDTRC_CatFeeder` (Password: rdtrc123)

---

### 2. 🐦 Bird Feeding Complete System
**📂 ไฟล์:** [`bird_feeding_complete_system.ino`](bird_feeding_complete_system.ino)

**✨ Features:**
- ✅ ระบบให้อาหารนกอัตโนมัติครบครัน
- ✅ ตารางให้อาหาร 4 เวลา/วัน (เฉพาะกลางวัน)
- ✅ Load Cell วัดน้ำหนักอาหาร
- ✅ PIR Sensor ตรวจจับนก
- ✅ Light Sensor แยกกลางวัน/กลางคืน
- ✅ Weather-Resistant Design
- ✅ Web Interface สำหรับควบคุม
- ✅ Blynk App Integration
- ✅ LINE Notifications
- ✅ Hotspot Mode (RDTRC_BirdFeeder)
- ✅ Bird Visit Tracking
- ✅ Daylight-Only Feeding
- ✅ Data Logging & Statistics
- ✅ OTA Updates

**🔧 Hardware Requirements:**
- ESP32 Development Board
- Servo Motor (SG90)
- Load Cell + HX711 Module
- PIR Motion Sensor
- Light Sensor (LDR)
- Buzzer
- LED
- Push Buttons (2x)

**🌐 Access:**
- Web: `http://bird-feeder.local`
- Hotspot: `RDTRC_BirdFeeder` (Password: rdtrc123)

---

### 3. 🍅 Tomato Watering Complete System
**📂 ไฟล์:** [`tomato_watering_complete_system.ino`](tomato_watering_complete_system.ino)

**✨ Features:**
- ✅ ระบบรดน้ำมะเขือเทศแบบ Multi-Zone (4 โซน)
- ✅ Soil Moisture Sensors แต่ละโซน
- ✅ DHT22 วัดอุณหภูมิและความชื้น
- ✅ Light Sensor แยกกลางวัน/กลางคืน
- ✅ Flow Sensor ตรวจสอบการไหลของน้ำ
- ✅ Ultrasonic วัดระดับน้ำในถัง
- ✅ Weather-Based Watering Control
- ✅ Web Interface สำหรับควบคุม
- ✅ Blynk App Integration
- ✅ LINE Notifications
- ✅ Hotspot Mode (RDTRC_TomatoWater)
- ✅ Pump Protection System
- ✅ Individual Zone Control
- ✅ Scheduling System
- ✅ Data Logging & Statistics
- ✅ OTA Updates

**🔧 Hardware Requirements:**
- ESP32 Development Board
- Water Pump (12V)
- Solenoid Valves (4x)
- DHT22 Temperature/Humidity Sensor
- Soil Moisture Sensors (4x)
- Flow Sensor
- Ultrasonic Sensor (HC-SR04)
- Light Sensor (LDR)
- Relay Modules
- Buzzer
- LED
- Push Buttons

**🌐 Access:**
- Web: `http://tomato-water.local`
- Hotspot: `RDTRC_TomatoWater` (Password: rdtrc123)

---

### 4. 🍄🌿 Mushroom & Cilantro Growing Complete System
**📂 ไฟล์:** [`mushroom_cilantro_complete_system.ino`](mushroom_cilantro_complete_system.ino)

**✨ Features:**
- ✅ ระบบปลูกเห็ดและผักชีฝรั่งแบบ Dual-Zone
- ✅ Advanced Environmental Control
- ✅ DHT22 วัดอุณหภูมิและความชื้น
- ✅ CO2 Level Monitoring
- ✅ pH Level Monitoring
- ✅ Soil Moisture Sensors (2 โซน)
- ✅ Automated Misting System (เห็ด)
- ✅ LED Grow Lights (ผักชีฝรั่ง)
- ✅ Heating System
- ✅ Ventilation Control
- ✅ Growth Phase Management
- ✅ Web Interface สำหรับควบคุม
- ✅ Blynk App Integration
- ✅ LINE Notifications
- ✅ Hotspot Mode (RDTRC_Growing)
- ✅ Automated Growth Cycles
- ✅ Environmental Alerts
- ✅ Data Logging & Analytics
- ✅ OTA Updates

**🔧 Hardware Requirements:**
- ESP32 Development Board
- DHT22 Temperature/Humidity Sensor
- CO2 Sensor Module
- pH Sensor Module
- Soil Moisture Sensors (2x)
- Water Pump (Misting)
- LED Grow Lights
- Heating Element
- Ventilation Fans (2x)
- Ultrasonic Sensor (HC-SR04)
- Light Sensor (LDR)
- Relay Modules
- Buzzer
- LED
- Push Buttons

**🌐 Access:**
- Web: `http://growing-system.local`
- Hotspot: `RDTRC_Growing` (Password: rdtrc123)

---

## 🔧 การติดตั้งและใช้งาน

### 📋 ขั้นตอนการติดตั้ง

1. **เตรียมฮาร์ดแวร์**
   - ESP32 Development Board
   - Sensors และ Actuators ตามระบบที่เลือก
   - Power Supply ที่เหมาะสม

2. **ติดตั้ง Libraries**
   ```cpp
   // ติดตั้ง Libraries เหล่านี้ใน Arduino IDE
   - WiFi (Built-in)
   - WebServer (Built-in)
   - ArduinoJson
   - SPIFFS (Built-in)
   - ESPmDNS (Built-in)
   - BlynkSimpleEsp32
   - NTPClient
   - HTTPClient (Built-in)
   - ArduinoOTA (Built-in)
   - DHT sensor library (สำหรับ DHT22)
   - HX711 library (สำหรับ Load Cell)
   - ESP32Servo
   ```

3. **แก้ไขการตั้งค่า**
   ```cpp
   // แก้ไขใน Code
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   #define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"
   const char* lineToken = "YOUR_LINE_NOTIFY_TOKEN";
   ```

4. **อัพโหลด Code**
   - เลือกไฟล์ระบบที่ต้องการ
   - Compile และ Upload ไปยัง ESP32

### 🌐 การเข้าถึงระบบ

**หลังจากติดตั้งเสร็จ:**

1. **ผ่าน WiFi Network:**
   - เปิดเว็บเบราว์เซอร์
   - ไปที่ `http://[system-name].local`
   - หรือใช้ IP Address ที่แสดงใน Serial Monitor

2. **ผ่าน Hotspot Mode:**
   - หาก WiFi ไม่ได้ ระบบจะสร้าง Hotspot อัตโนมัติ
   - เชื่อมต่อ WiFi: `RDTRC_[SystemName]`
   - Password: `rdtrc123`
   - ไปที่ `http://192.168.4.1`

3. **ผ่าน Blynk Mobile App:**
   - ดาวน์โหลด Blynk App
   - สร้าง Project ใหม่
   - ใส่ Auth Token ที่ได้
   - เพิ่ม Widgets ตามต้องการ

4. **ผ่าน LINE Notifications:**
   - สร้าง LINE Notify Token
   - ใส่ Token ใน Code
   - รับแจ้งเตือนผ่าน LINE

### ⚙️ การตั้งค่าและปรับแต่ง

**ผ่าน Web Interface:**
- ปรับตารางเวลา
- ตั้งค่า Threshold ต่างๆ
- Calibrate Sensors
- ดู Statistics และ Logs
- อัพเดท Firmware (OTA)

**ผ่าน Blynk App:**
- ควบคุมระบบแบบ Real-time
- รับ Notifications
- ดูกราฟข้อมูล
- Remote Control

---

## 📊 Features Comparison

| Feature | Cat Feeding | Bird Feeding | Tomato Watering | Mushroom/Cilantro |
|---------|-------------|--------------|-----------------|-------------------|
| **Feeding/Watering** | ✅ 6 times/day | ✅ 4 times/day | ✅ Multi-zone | ✅ Dual-zone |
| **Weight Monitoring** | ✅ Load Cell | ✅ Load Cell | ❌ | ❌ |
| **Moisture Monitoring** | ❌ | ❌ | ✅ 4 sensors | ✅ 2 sensors |
| **Temperature Control** | ❌ | ❌ | ❌ | ✅ Heating |
| **Humidity Control** | ❌ | ❌ | ❌ | ✅ Misting |
| **Light Control** | ❌ | ❌ | ❌ | ✅ LED Grow |
| **Motion Detection** | ✅ PIR | ✅ PIR | ❌ | ❌ |
| **Light Detection** | ❌ | ✅ Day/Night | ✅ Day/Night | ✅ Day/Night |
| **Flow Monitoring** | ❌ | ❌ | ✅ Flow Sensor | ❌ |
| **CO2 Monitoring** | ❌ | ❌ | ❌ | ✅ CO2 Sensor |
| **pH Monitoring** | ❌ | ❌ | ❌ | ✅ pH Sensor |
| **Growth Phases** | ❌ | ❌ | ❌ | ✅ Auto Phases |
| **Weather Integration** | ❌ | ❌ | ✅ Weather-based | ✅ Environmental |
| **Web Interface** | ✅ Full | ✅ Full | ✅ Full | ✅ Full |
| **Blynk Integration** | ✅ Complete | ✅ Complete | ✅ Complete | ✅ Complete |
| **LINE Notifications** | ✅ Smart | ✅ Smart | ✅ Smart | ✅ Smart |
| **OTA Updates** | ✅ | ✅ | ✅ | ✅ |
| **Data Logging** | ✅ SPIFFS | ✅ SPIFFS | ✅ SPIFFS | ✅ SPIFFS |
| **Hotspot Mode** | ✅ | ✅ | ✅ | ✅ |

---

## 🛠️ การบำรุงรักษา

### 📝 Daily Maintenance
- ตรวจสอบระดับอาหาร/น้ำ
- ดู Notifications ใน LINE
- ตรวจสอบสถานะผ่าน Web Interface

### 🔧 Weekly Maintenance
- ทำความสะอาด Sensors
- ตรวจสอบการทำงานของ Actuators
- ดู Logs และ Statistics
- ตรวจสอบการเชื่อมต่อ WiFi

### 🔄 Monthly Maintenance
- Calibrate Sensors (Load Cell, pH, etc.)
- อัพเดท Firmware หากมี
- Backup ข้อมูล Logs
- ตรวจสอบ Hardware connections

---

## 🚨 Troubleshooting

### 🌐 WiFi Connection Issues
```
ปัญหา: ไม่สามารถเชื่อมต่อ WiFi ได้
แก้ไข: 
1. ตรวจสอบ SSID และ Password
2. ใช้ Hotspot Mode แทน
3. Reset ESP32 และลองใหม่
```

### 📡 Blynk Connection Issues
```
ปัญหา: Blynk App ไม่เชื่อมต่อ
แก้ไข:
1. ตรวจสอบ Auth Token
2. ตรวจสอบการเชื่อมต่ออินเทอร์เน็ต
3. Restart Blynk App
```

### 🔧 Sensor Issues
```
ปัญหา: Sensor อ่านค่าไม่ถูกต้อง
แก้ไข:
1. ตรวจสอบการเชื่อมต่อสาย
2. ทำความสะอาด Sensor
3. Calibrate ใหม่
4. เปลี่ยน Sensor หากเสีย
```

### 💾 Memory Issues
```
ปัญหา: ระบบทำงานช้าหรือ Restart บ่อย
แก้ไข:
1. ลบ Log files เก่า
2. ลด Refresh Rate
3. เพิ่ม delay ในโค้ด
4. ใช้ ESP32 ที่มี RAM มากกว่า
```

---

## 📞 การสนับสนุน

### 🔧 Technical Support
- **Email:** support@rdtrc.com
- **LINE:** @rdtrc_support
- **GitHub Issues:** [Repository Issues](https://github.com/rdtrc/iot-systems/issues)

### 📚 Documentation
- **ดูเพิ่มเติม:** [Wiki Page](https://github.com/rdtrc/iot-systems/wiki)
- **Video Tutorials:** [YouTube Channel](https://youtube.com/rdtrc)
- **Community:** [Discord Server](https://discord.gg/rdtrc)

---

## 📄 License

MIT License - ใช้งานได้อย่างอิสระ ทั้งเชิงพาณิชย์และส่วนตัว

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
```

---

## 🙏 Credits

**Developed by:** RDTRC Team  
**Version:** 4.0 - Complete Standalone Systems  
**Year:** 2024  

**Special Thanks:**
- Arduino Community
- ESP32 Developers
- Blynk Team
- Open Source Contributors

---

**🎯 Ready to Deploy! แต่ละระบบพร้อมใช้งานทันที - เพียงแค่อัพโหลดและตั้งค่า!** 🚀
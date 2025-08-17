# 🚀 RDTRC Enhanced IoT Systems - Version 3.0

> **Firmware made by: RDTRC**  
> **Version: 3.0 - Enhanced with Blink Integration & Hotspot Capability**  
> **Updated: 2024**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)](https://www.espressif.com/)
[![Firmware](https://img.shields.io/badge/Firmware-RDTRC_v3.0-red.svg)](https://github.com)

---

## 🎉 **สิ่งที่เปลี่ยนแปลงใหม่ในเวอร์ชัน 3.0**

### ✨ Enhanced Features

- 🔗 **Blink Integration** - เชื่อมต่อกับ Blink Cloud สำหรับควบคุมผ่านมือถือ
- 📡 **Hotspot Capability** - สร้าง WiFi Hotspot สำหรับการเข้าถึงโดยตรง
- 🌐 **Comprehensive Web Interface** - หน้าเว็บสำหรับจัดการเครือข่ายและระบบ
- 🏠 **Local Operation** - ทำงานได้แม้ไม่มีอินเทอร์เน็ต
- 📱 **Single Main File** - แต่ละระบบมีไฟล์หลักเดียวตามที่ร้องขอ
- 🎨 **Modern UI/UX** - ดีไซน์เว็บอินเตอร์เฟซที่ทันสมัยและใช้งานง่าย

### 🔧 Technical Improvements

- **Enhanced System Base Class** - ระบบพื้นฐานที่ปรับปรุงใหม่
- **Automatic Network Fallback** - เปลี่ยนเป็น Hotspot อัตโนมัติเมื่อ WiFi ขัดข้อง
- **Real-time Status Updates** - อัพเดทสถานะแบบเรียลไทม์
- **Advanced Error Handling** - จัดการข้อผิดพลาดอย่างชาญฉลาด
- **OTA Update Support** - อัพเดทผ่านเครือข่าย
- **Comprehensive Logging** - ระบบบันทึกที่ละเอียดครบถ้วน

---

## 🎛️ ระบบที่มีให้เลือก

### 1. 🐱 Enhanced Cat Feeding System
**📂 Main File:** [`systems/cat_feeding/enhanced_cat_feeding_system.ino`](systems/cat_feeding/enhanced_cat_feeding_system.ino)

**✨ Features:**
- ✅ ให้อาหารตามเวลา (6 เวลา/วัน)
- ✅ วัดน้ำหนักอาหาร (Load Cell HX711)
- ✅ ตรวจจับแมว (PIR Sensor)
- ✅ ควบคุมปริมาณ 5-100g
- ✅ แจ้งเตือนอาหารหมด
- ✅ **Blink Integration** - ควบคุมผ่านมือถือ
- ✅ **Hotspot Mode** - เข้าถึงโดยตรงผ่าน WiFi
- ✅ **Web Interface** - จัดการผ่านเว็บเบราว์เซอร์
- ✅ **Local Operation** - ทำงานแม้ไม่มีเน็ต
- ✅ **Boot Screen: "FW make by RDTRC"**

**🔌 Pin Configuration:**
- Servo: Pin 18
- Load Cell DOUT: Pin 19, SCK: Pin 20
- PIR Sensor: Pin 4
- Manual Button: Pin 32
- Reset Button: Pin 33

---

### 2. 🐦 Enhanced Bird Feeding System
**📂 Main File:** [`systems/bird_feeding/enhanced_bird_feeding_system.ino`](systems/bird_feeding/enhanced_bird_feeding_system.ino)

**✨ Features:**
- ✅ ให้อาหารและน้ำตามตารางเวลา
- ✅ ตรวจจับนก (PIR Sensor)
- ✅ วัดระดับอาหารและน้ำ
- ✅ เซ็นเซอร์ฝนและแสง
- ✅ ปรับการให้อาหารตามสภาพอากาศ
- ✅ **Blink Integration** - ควบคุมและรับแจ้งเตือน
- ✅ **Smart Weather Detection** - หยุดให้อาหารเมื่อฝนตก
- ✅ **Multi-sensor Monitoring** - ตรวจสอบสภาพแวดล้อม
- ✅ **Web Interface** - จัดการผ่านเว็บ
- ✅ **Local Operation** - ทำงานแบบออฟไลน์

**🔌 Pin Configuration:**
- Food Servo: Pin 18, Water Servo: Pin 19
- PIR Sensor: Pin 4
- Food Level: A0, Water Level: A1
- Rain Sensor: A2, Light Sensor: A3
- Manual Button: Pin 32, Reset Button: Pin 33

---

### 3. 🍅 Enhanced Tomato Watering System
**📂 Main File:** [`systems/tomato_watering/enhanced_tomato_watering_system.ino`](systems/tomato_watering/enhanced_tomato_watering_system.ino)

**✨ Features:**
- ✅ รดน้ำหลายโซน (4 โซน)
- ✅ เซ็นเซอร์ความชื้นดินแต่ละโซน
- ✅ ปั๊มน้ำพร้อมเซ็นเซอร์อัตราการไหล
- ✅ ตรวจสอบระดับน้ำและฝน
- ✅ ปรับการรดน้ำตามสภาพอากาศ
- ✅ **Multi-zone Control** - ควบคุม 4 โซนแยกกัน
- ✅ **Flow Monitoring** - ตรวจสอบการใช้น้ำ
- ✅ **Smart Scheduling** - ตารางรดน้ำอัจฉริยะ
- ✅ **Blink Integration** - ควบคุมและตรวจสอบระยะไกล
- ✅ **Emergency Stop** - หยุดฉุกเฉินเมื่อน้ำหมด

**🔌 Pin Configuration:**
- Pump Relay: Pin 18
- Zone Valves: Pin 19-22
- Soil Sensors: A0-A3
- Water Level: A4, Rain Sensor: A5
- Flow Sensor: Pin 2 (Interrupt)
- Manual Button: Pin 32, Reset Button: Pin 33

---

## 🌐 Network Features

### 📡 WiFi & Hotspot Management
- **Dual WiFi Support** - Primary และ Secondary WiFi
- **Automatic Hotspot Fallback** - เปลี่ยนเป็น Hotspot อัตโนมัติ
- **Hybrid Mode** - ทำงาน WiFi และ Hotspot พร้อมกัน
- **Captive Portal** - หน้าเว็บปรากฏอัตโนมัติเมื่อเชื่อมต่อ Hotspot

### 🔗 Blink Integration
- **Real-time Control** - ควบคุมแบบเรียลไทม์
- **Status Monitoring** - ตรวจสอบสถานะต่างๆ
- **Event Notifications** - แจ้งเตือนเหตุการณ์สำคัญ
- **Virtual Pin Mapping** - กำหนด Virtual Pin สำหรับแต่ละฟีเจอร์
- **Offline Resilience** - ทำงานต่อได้แม้ Blink ขัดข้อง

### 🌐 Web Interface
- **Responsive Design** - รองรับทุกขนาดหน้าจอ
- **Thai Language Support** - รองรับภาษาไทยครบถ้วน
- **Real-time Updates** - อัพเดทข้อมูลแบบเรียลไทม์
- **Network Scanner** - สแกนและเลือก WiFi
- **System Configuration** - ตั้งค่าระบบผ่านเว็บ

---

## 🚀 การติดตั้งและใช้งาน

### 1. 📋 ความต้องการของระบบ
- **Hardware:** ESP32 Development Board
- **IDE:** Arduino IDE 1.8.19+ หรือ PlatformIO
- **Libraries:** ดูรายการใน `libraries/` ของแต่ละระบบ

### 2. 📦 การติดตั้ง Libraries
```bash
# ติดตั้ง Libraries ที่จำเป็น
- WiFi (Built-in)
- WebServer (Built-in)
- ArduinoJson
- BlynkSimpleEsp32
- RTClib
- LiquidCrystal_I2C
- ESP32Servo
- HX711 (สำหรับระบบแมว)
- DNSServer (Built-in)
- SPIFFS (Built-in)
```

### 3. 🔧 การอัพโลดโค้ด
1. เปิด Arduino IDE
2. เลือกไฟล์ `.ino` ของระบบที่ต้องการ
3. เลือก Board: "ESP32 Dev Module"
4. เลือก Port ที่ถูกต้อง
5. กด Upload

### 4. 🌐 การตั้งค่าเครือข่าย

#### วิธีที่ 1: ผ่าน Hotspot (แนะนำสำหรับครั้งแรก)
1. เปิดระบบ จะเห็นหน้า Boot Screen "FW make by RDTRC"
2. ค้นหา WiFi ชื่อ "RDTRC_[SystemType]_[MAC]"
3. เชื่อมต่อด้วยรหัส "rdtrc2024"
4. เบราว์เซอร์จะเปิดหน้าเว็บอัตโนมัติ
5. ไปที่ "ตั้งค่าเครือข่าย" เพื่อกำหนด WiFi

#### วิธีที่ 2: กดปุ่ม Boot เพื่อเข้า Hotspot Mode
1. กดค้างปุ่ม Boot (Pin 0) เป็นเวลา 3 วินาที
2. ระบบจะเข้าสู่ Hotspot Mode
3. ทำตามขั้นตอนเดียวกับวิธีที่ 1

### 5. 🔗 การตั้งค่า Blink
1. ดาวน์โหลดแอป Blink จาก App Store/Google Play
2. สร้างบัญชีและ Project ใหม่
3. คัดลอก Auth Token
4. ไปที่หน้า "Blink Integration" ในเว็บอินเตอร์เฟซ
5. ใส่ Token และกด "บันทึก"
6. ระบบจะเชื่อมต่อ Blink อัตโนมัติ

---

## 📱 การใช้งาน Web Interface

### 🏠 หน้าแรก
- ภาพรวมสถานะระบบ
- การเชื่อมต่อเครือข่าย
- ข้อมูลระบบพื้นฐาน
- การจัดการระบบด่วน

### 📶 ตั้งค่าเครือข่าย
- สแกนและเลือก WiFi
- ตั้งค่า Primary/Secondary WiFi
- กำหนด Hotspot SSID และรหัสผ่าน
- เลือกโหมดการทำงานเครือข่าย

### ⚙️ ตั้งค่าระบบ
- เปลี่ยนชื่อระบบ
- ตั้งค่าเขตเวลา
- เปิด/ปิด OTA Update
- เปิด/ปิด Debug Mode

### 🔗 Blink Integration
- ตั้งค่า Auth Token
- เลือก Blink Server
- ทดสอบการเชื่อมต่อ
- ดูสถานะการเชื่อมต่อ

### 📊 สถานะระบบ
- ข้อมูลฮาร์ดแวร์
- การใช้หน่วยความจำ
- สถานะเครือข่าย
- ข้อมูลระบบโดยละเอียด

---

## 🔧 Virtual Pin Mapping สำหรับ Blink

### 🐱 Cat Feeding System
- **V0:** น้ำหนักอาหารปัจจุบัน
- **V1:** ปุ่มให้อาหารด้วยตนเอง
- **V2:** ปรับปริมาณอาหาร (10-100g)
- **V3:** อาหารที่ให้ไปแล้ววันนี้
- **V4:** จำนวนครั้งที่ให้อาหารวันนี้
- **V5:** สถานะการตรวจพบแมว
- **V6:** เวลาให้อาหารครั้งต่อไป
- **V7:** สถานะระบบโดยรวม
- **V9:** แจ้งเตือนอาหารหมด

### 🐦 Bird Feeding System
- **V0:** ระดับอาหาร (%)
- **V1:** ปุ่มให้อาหารด้วยตนเอง
- **V2:** ระดับน้ำ (%)
- **V3:** ระดับแสง (%)
- **V4:** จำนวนครั้งที่ให้อาหารวันนี้
- **V5:** สถานะการตรวจพบนก
- **V6:** เวลาให้อาหารครั้งต่อไป
- **V7:** สถานะสภาพอากาศ
- **V9:** แจ้งเตือนต่างๆ
- **V10:** เซ็นเซอร์ฝน

### 🍅 Tomato Watering System
- **V0-V3:** ความชื้นดิน Zone 1-4
- **V1:** ปุ่มรดน้ำด้วยตนเอง
- **V5:** ระดับน้ำ (%)
- **V6:** การใช้น้ำรายวัน (ลิตร)
- **V7:** สถานะระบบโดยรวม
- **V8:** สถานะปั๊มน้ำ
- **V9:** เซ็นเซอร์ฝน
- **V10:** อัตราการไหลของน้ำ
- **V11:** หยุด/เริ่มระบบ

---

## 🛠️ การแก้ไขปัญหา

### ❓ ปัญหาที่พบบ่อย

#### 🔴 ระบบไม่เชื่อมต่อ WiFi
1. ตรวจสอบ SSID และรหัสผ่าน
2. ตรวจสอบความแรงสัญญาณ
3. ลองใช้ Secondary WiFi
4. เข้าสู่ Hotspot Mode เพื่อตั้งค่าใหม่

#### 🟡 Blink ไม่เชื่อมต่อ
1. ตรวจสอบ Auth Token
2. ตรวจสอบการเชื่อมต่ออินเทอร์เน็ต
3. ลองเปลี่ยน Blink Server
4. รีสตาร์ทระบบ

#### 🟠 เซ็นเซอร์อ่านค่าผิด
1. ตรวจสอบการเชื่อมต่อสายไฟ
2. ทำความสะอาดเซ็นเซอร์
3. ตรวจสอบ Pin Configuration
4. เปิด Debug Mode เพื่อดูค่าที่อ่านได้

#### 🔵 หน้าเว็บไม่แสดง
1. ตรวจสอบการเชื่อมต่อเครือข่าย
2. ลองเข้าผ่าน IP Address โดยตรง
3. ล้าง Cache เบราว์เซอร์
4. ลองใช้เบราว์เซอร์อื่น

### 🔄 การรีเซ็ตระบบ

#### Factory Reset ผ่านปุ่ม
1. กดค้างปุ่ม Reset (Pin 33) เป็นเวลา 5 วินาที
2. ระบบจะรีเซ็ตเป็นค่าเริ่มต้น
3. ข้อมูลทั้งหมดจะถูกลบ

#### Factory Reset ผ่านเว็บ
1. เข้าไปที่หน้า "ตั้งค่าระบบ"
2. กดปุ่ม "รีเซ็ตเป็นค่าเริ่มต้น"
3. ยืนยันการรีเซ็ต

---

## 📈 Performance & Monitoring

### 📊 System Metrics
- **Memory Usage:** แสดงการใช้หน่วยความจำแบบเรียลไทม์
- **CPU Temperature:** ตรวจสอบอุณหภูมิ CPU
- **Network Signal:** ความแรงสัญญาณ WiFi
- **Uptime:** เวลาที่ระบบทำงานต่อเนื่อง
- **Error Count:** จำนวนข้อผิดพลาดที่เกิดขึ้น

### 🔍 Debug Features
- **Real-time Logging:** บันทึกการทำงานแบบเรียลไทม์
- **Sensor Readings:** แสดงค่าเซ็นเซอร์ในโหมด Debug
- **Network Diagnostics:** ตรวจสอบสถานะเครือข่าย
- **Memory Analysis:** วิเคราะห์การใช้หน่วยความจำ

---

## 🔒 Security Features

### 🛡️ Network Security
- **WPA2 Encryption:** การเข้ารหัสระดับ WPA2
- **Access Control:** จำกัดจำนวนผู้เชื่อมต่อ Hotspot
- **Secure Web Interface:** การเข้าถึงเว็บอินเตอร์เฟซที่ปลอดภัย

### 🔐 Authentication
- **Hotspot Password:** รหัสผ่านสำหรับ Hotspot
- **Blink Token:** การยืนยันตัวตนผ่าน Blink Token
- **OTA Password:** รหัสผ่านสำหรับการอัพเดท OTA

---

## 🆕 Future Enhancements

### 🔮 Planned Features
- **Mobile App:** แอพมือถือเฉพาะสำหรับระบบ RDTRC
- **Cloud Dashboard:** แดชบอร์ดบนคลาวด์
- **Machine Learning:** การเรียนรู้รูปแบบการใช้งาน
- **Voice Control:** การควบคุมด้วยเสียง
- **Advanced Analytics:** การวิเคราะห์ข้อมูลขั้นสูง

### 🎯 Roadmap
- **Q1 2024:** Mobile App Development
- **Q2 2024:** Cloud Integration
- **Q3 2024:** AI/ML Features
- **Q4 2024:** Voice Control & Advanced Analytics

---

## 📞 Support & Contact

### 🆘 Getting Help
- **Documentation:** อ่านคู่มือนี้อย่างละเอียด
- **Debug Mode:** เปิดใช้งานเพื่อดูข้อมูลเพิ่มเติม
- **System Logs:** ตรวจสอบ Serial Monitor
- **Community:** แชร์ปัญหาในชุมชน Arduino/ESP32

### 📧 Contact Information
- **Firmware Developer:** RDTRC Team
- **Version:** 3.0 Enhanced
- **License:** MIT License
- **Year:** 2024

---

## 📄 License

MIT License - ใช้งานและแก้ไขได้อย่างอิสระ

Copyright (c) 2024 RDTRC

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software.

---

**🎉 ขอบคุณที่ใช้ระบบ RDTRC Enhanced IoT Systems!**

> Made with ❤️ by RDTRC Team - Empowering Smart Agriculture & Pet Care
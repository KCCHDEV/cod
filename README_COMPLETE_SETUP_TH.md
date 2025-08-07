# 🌱 เอกสารการติดตั้งและใช้งานระบบ ESP32 ฉบับสมบูรณ์

## 🎯 ยินดีต้อนรับสู่ระบบอัตโนมัติ ESP32!

โปรเจ็กต์นี้ประกอบด้วยระบบอัตโนมัติต่างๆ ที่ใช้ ESP32 เป็นตัวควบคุมหลัก พร้อมเอกสารการใช้งานแบบครบถ้วนในภาษาไทย

---

## 📚 เอกสารทั้งหมด

### 📖 เอกสารหลัก
| เอกสาร | คำอธิบาย | ลิงก์ |
|--------|----------|------|
| **คู่มือการติดตั้งหลัก** | คำแนะนำแบบครบวงจรสำหรับการติดตั้งทุกระบบ | [SETUP_GUIDE_TH.md](./SETUP_GUIDE_TH.md) |
| **แผนผังการเชื่อมต่อ** | แผนผังการต่อวงจรสำหรับทุกระบบ | [CONNECTION_DIAGRAMS_TH.md](./CONNECTION_DIAGRAMS_TH.md) |
| **เอกสารการทำงาน** | อธิบายหลักการทำงานของแต่ละระบบ | [WORKING_DOCUMENTATION_TH.md](./WORKING_DOCUMENTATION_TH.md) |
| **คู่มือ Libraries** | วิธีการติดตั้งและจัดการ Libraries | [LIBRARIES_INSTALLATION_TH.md](./LIBRARIES_INSTALLATION_TH.md) |

### 📋 เอกสารเฉพาะระบบ
| ระบบ | เอกสาร | คำอธิบาย |
|------|--------|----------|
| **ระบบให้อาหารนก** | [README_bird_feeding.md](./README_bird_feeding.md) | ระบบให้น้ำและอาหารนกอัตโนมัติ |
| **ระบบมะเขือเทศ** | [README_tomato_system.md](./README_tomato_system.md) | ระบบรดน้ำมะเขือเทศแบบอัจฉริยะ |
| **ระบบขั้นสูง** | [README_advanced.md](./README_advanced.md) | ระบบควบคุมขั้นสูงด้วย Sensors |

### 🔧 เอกสารการตั้งค่า
| หัวข้อ | เอกสาร | คำอธิบาย |
|-------|--------|----------|
| **Webhook Setup** | [webhook_setup_guide.md](./webhook_setup_guide.md) | การตั้งค่า Webhook สำหรับการแจ้งเตือน |
| **Blink App Setup** | [blink_app_setup.md](./blink_app_setup.md) | การเชื่อมต่อกับ Blink Camera |

---

## 🚀 เริ่มต้นอย่างรวดเร็ว

### ขั้นตอนที่ 1: เตรียมความพร้อม
1. อ่าน **[คู่มือการติดตั้งหลัก](./SETUP_GUIDE_TH.md)** เพื่อติดตั้ง Arduino IDE และ ESP32 Board Support
2. ติดตั้ง Libraries ตาม **[คู่มือ Libraries](./LIBRARIES_INSTALLATION_TH.md)**

### ขั้นตอนที่ 2: เลือกระบบที่ต้องการ
- **ระบบรดน้ำพื้นฐาน**: `examples/watering_system.ino`
- **ระบบกุหลาบ**: `rose_watering_system.ino`
- **ระบบมะเขือเทศ**: `tomato_system.ino`
- **ระบบให้อาหารนก**: `bird_feeding_system.ino`
- **ระบบปลูกเห็ด**: `mushroom_system.ino`

### ขั้นตอนที่ 3: เชื่อมต่ออุปกรณ์
ใช้ **[แผนผังการเชื่อมต่อ](./CONNECTION_DIAGRAMS_TH.md)** ในการต่อวงจร

### ขั้นตอนที่ 4: อัพโหลดและทดสอบ
ปฏิบัติตามขั้นตอนใน **[คู่มือการติดตั้งหลัก](./SETUP_GUIDE_TH.md)**

---

## 🌟 ไฮไลท์ของระบบ

### 🌱 ระบบรดน้ำพื้นฐาน
- รดน้ำอัตโนมัติตามเวลาที่กำหนด
- ควบคุมผ่าน Web Interface
- รองรับปั๊มน้ำ 4 ช่อง

### 🌹 ระบบกุหลาบขั้นสูง
- ตรวจสอบความชื้นดิน 4 จุด
- เชื่อมต่อกับ Blink Camera
- ติดตามการเจริญเติบโต

### 🍅 ระบบมะเขือเทศ
- เซ็นเซอร์ DHT22 สำหรับอุณหภูมิ/ความชื้น
- เซ็นเซอร์ความชื้นดิน
- รดน้ำอัจฉริยะตามสภาพแวดล้อม

### 🐦 ระบบให้อาหารนก
- ให้น้ำด้วย Water Pump
- ให้อาหารด้วย Servo Motor
- ตั้งเวลาได้หลายช่วง

### 🍄 ระบบปลูกเห็ด
- ควบคุมความชื้นสูง (70-95%)
- ระบบพ่นหมอก 4 ช่อง
- ติดตามวงจรการเจริญเติบโต

---

## 📋 รายการอุปกรณ์ที่ต้องการ

### อุปกรณ์พื้นฐาน (ทุกระบบ)
- ESP32 Development Board
- สาย USB Micro
- Breadboard
- สาย Jumper
- แหล่งจ่ายไฟ 5V/2A

### อุปกรณ์เฉพาะระบบ
| ระบบ | อุปกรณ์เพิ่มเติม |
|------|------------------|
| **รดน้ำพื้นฐาน** | RTC DS3231, 4-Ch Relay, Water Pumps |
| **กุหลาบ** | RTC DS3231, 4-Ch Relay, Soil Sensors |
| **มะเขือเทศ** | DHT22, Soil Sensor, 1-Ch Relay |
| **ให้อาหารนก** | RTC DS3231, Servo SG90, 1-Ch Relay |
| **ปลูกเห็ด** | RTC DS3231, 4-Ch Relay, Humidity Sensors |

---

## 🔧 Libraries ที่จำเป็น

### Core Libraries (ติดตั้งอัตโนมัติ)
- WiFi
- WebServer
- Wire (I2C)
- HTTPClient

### External Libraries (ต้องติดตั้ง)
- **RTClib** by Adafruit (v2.1.1+)
- **ArduinoJson** by Benoit Blanchon (v6.21.3+)
- **DHT sensor library** by Adafruit (v1.4.4+)
- **ESP32Servo** by Kevin Harrington (v0.13.0+)

**[ดูรายละเอียดการติดตั้งที่นี่](./LIBRARIES_INSTALLATION_TH.md)**

---

## 🌐 Web Interface

ทุกระบบมี Web Interface ในตัวที่สามารถ:
- ดูสถานะปัจจุบัน
- ควบคุม Relay/Servo
- ตั้งค่าตารางเวลา
- ดูประวัติการทำงาน

**การเข้าถึง**: เปิดเว็บเบราว์เซอร์ไปที่ IP Address ของ ESP32

---

## 🔗 การเชื่อมต่อพิเศษ

### Webhook Integration
ส่งการแจ้งเตือนไปยัง:
- Discord
- Slack  
- LINE Notify
- Custom API

**[ดูการตั้งค่า Webhook](./webhook_setup_guide.md)**

### Blink Camera Integration
เชื่อมต่อกับกล้อง Blink เพื่อ:
- ถ่ายภาพอัตโนมัติ
- บันทึกวิดีโอ
- ตรวจสอบระยะไกล

**[ดูการตั้งค่า Blink App](./blink_app_setup.md)**

---

## 🛠️ การแก้ไขปัญหาเบื้องต้น

### ปัญหาที่พบบ่อย
| ปัญหา | สาเหตุ | แก้ไข |
|-------|-------|------|
| **Compile Error** | Library ไม่ครบ | ติดตั้งตาม [คู่มือ Libraries](./LIBRARIES_INSTALLATION_TH.md) |
| **Upload Failed** | Port ไม่ถูกต้อง | เลือก Port ใน Tools > Port |
| **WiFi ไม่เชื่อมต่อ** | SSID/Password ผิด | ตรวจสอบการตั้งค่าในโค้ด |
| **RTC ไม่ทำงาน** | การเชื่อมต่อผิด | ดู [แผนผังการเชื่อมต่อ](./CONNECTION_DIAGRAMS_TH.md) |

**[ดูแนวทางแก้ไขครบถ้วนที่นี่](./SETUP_GUIDE_TH.md#การแก้ไขปัญหา)**

---

## 📈 การอัพเกรดและขยายระบบ

### การเพิ่มเซ็นเซอร์
- เซ็นเซอร์ความชื้นดิน
- เซ็นเซอร์แสง (LDR)
- เซ็นเซอร์ pH
- กล้องสำหรับ AI Vision

### การเชื่อมต่อ Cloud Services
- ThingSpeak
- Firebase
- AWS IoT
- Google Cloud IoT

### การพัฒนา Mobile App
- React Native
- Flutter
- Progressive Web App (PWA)

---

## 🤝 การสนับสนุนและชุมชน

### แหล่งข้อมูลเพิ่มเติม
- **Arduino Forum**: https://forum.arduino.cc/
- **ESP32 Community**: https://reddit.com/r/esp32/
- **Adafruit Learning**: https://learn.adafruit.com/

### การรายงานปัญหา
หากพบปัญหาหรือต้องการความช่วยเหลือ:
1. ตรวจสอบ Serial Monitor
2. อ่านเอกสารที่เกี่ยวข้อง
3. ค้นหาใน GitHub Issues
4. สร้าง Issue ใหม่พร้อมรายละเอียด

---

## 📄 สรุปไฟล์เอกสาร

### 📚 เอกสารหลัก
- [`SETUP_GUIDE_TH.md`](./SETUP_GUIDE_TH.md) - คู่มือการติดตั้งแบบครบวงจร
- [`CONNECTION_DIAGRAMS_TH.md`](./CONNECTION_DIAGRAMS_TH.md) - แผนผังการเชื่อมต่อทุกระบบ  
- [`WORKING_DOCUMENTATION_TH.md`](./WORKING_DOCUMENTATION_TH.md) - เอกสารการทำงานของระบบ
- [`LIBRARIES_INSTALLATION_TH.md`](./LIBRARIES_INSTALLATION_TH.md) - คู่มือการติดตั้ง Libraries

### 🔧 เอกสารการตั้งค่า
- [`webhook_setup_guide.md`](./webhook_setup_guide.md) - การตั้งค่า Webhook
- [`blink_app_setup.md`](./blink_app_setup.md) - การตั้งค่า Blink Camera

### 📝 เอกสารเฉพาะระบบ
- [`README_bird_feeding.md`](./README_bird_feeding.md) - ระบบให้อาหารนก
- [`README_tomato_system.md`](./README_tomato_system.md) - ระบบมะเขือเทศ
- [`README_advanced.md`](./README_advanced.md) - ระบบขั้นสูง

### 📦 รายการ Libraries และการเชื่อมต่อ
- [`libraries.txt`](./libraries.txt) - รายการ Libraries พื้นฐาน
- [`libraries_bird_feeding.txt`](./libraries_bird_feeding.txt) - Libraries สำหรับระบบนก
- [`libraries_tomato.txt`](./libraries_tomato.txt) - Libraries สำหรับระบบมะเขือเทศ
- [`wiring_diagram.txt`](./wiring_diagram.txt) - แผนผังการเชื่อมต่อพื้นฐาน
- [`wiring_diagram_bird_feeding.txt`](./wiring_diagram_bird_feeding.txt) - แผนผังระบบนก
- [`wiring_diagram_tomato.txt`](./wiring_diagram_tomato.txt) - แผนผังระบบมะเขือเทศ

---

## ✅ Checklist การเริ่มต้น

### เตรียมความพร้อม
- [ ] อ่าน [คู่มือการติดตั้งหลัก](./SETUP_GUIDE_TH.md)
- [ ] เตรียมอุปกรณ์ตามรายการ
- [ ] ติดตั้ง Arduino IDE และ ESP32 Support
- [ ] ติดตั้ง Libraries ที่จำเป็น

### การติดตั้ง
- [ ] เลือกระบบที่ต้องการใช้งาน
- [ ] ต่อวงจรตาม [แผนผังการเชื่อมต่อ](./CONNECTION_DIAGRAMS_TH.md)
- [ ] แก้ไขการตั้งค่า WiFi ในโค้ด
- [ ] Upload โค้ดไปยัง ESP32

### การทดสอบ
- [ ] ตรวจสอบ Serial Monitor
- [ ] ทดสอบการเชื่อมต่อ WiFi
- [ ] เปิด Web Interface
- [ ] ทดสอบการทำงานของ Relay/Servo
- [ ] ตรวจสอบการอ่านค่า Sensor

### การใช้งาน
- [ ] ตั้งค่าตารางเวลา (หากมี)
- [ ] ตั้งค่า Webhook (หากต้องการ)
- [ ] ตั้งค่า Blink Camera (หากใช้ระบบกุหลาบ)
- [ ] ทดสอบการทำงานรอบเดียวเต็ม

---

## 🎉 ยินดีด้วย!

หากคุณติดตั้งและใช้งานสำเร็จแล้ว ยินดีด้วย! 🎊

ขณะนี้คุณมีระบบอัตโนมัติที่สามารถ:
- 🌱 รดน้ำต้นไม้อัตโนมัติ
- 📱 ควบคุมผ่านมือถือ
- 📊 ติดตามข้อมูลแบบ Real-time
- 🔔 รับการแจ้งเตือนผ่าน Webhook
- 📸 ตรวจสอบผ่านกล้อง (หากมี)

**ขั้นตอนต่อไป**: ลองปรับแต่งระบบให้เหมาะสมกับการใช้งานของคุณ และอย่าลืมแชร์ผลงานกับชุมชน! 

---

🚀 **สนุกกับการใช้งานระบบอัตโนมัติของคุณ!**
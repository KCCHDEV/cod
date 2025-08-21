# 🤖 RDTRC IoT Systems Collection
## ชุดระบบ IoT ครบครัน - เวอร์ชัน 3.0

> **Firmware made by: RDTRC**  
> **Version: 3.0 - Advanced Sensor Suite with Offline Detection**  
> **Updated: 2024**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)](https://www.espressif.com/)
[![Firmware](https://img.shields.io/badge/Firmware-RDTRC-red.svg)](https://github.com)
[![Sensors](https://img.shields.io/badge/Sensors-Advanced-orange.svg)](https://github.com)

---

## 🎉 **อัพเดทใหม่! ระบบ Sensor ครบครัน + Offline Detection**

### ✨ สิ่งที่เปลี่ยนแปลงในเวอร์ชัน 3.0
- 🌡️ **Sensor Suite ครบครัน** - ทุกระบบมี sensor ที่เกี่ยวข้องครบถ้วน
- 🔍 **Offline Detection** - ตรวจสอบ sensor ที่ไม่ทำงานและข้ามไป
- 📺 **LCD Skip Logic** - ถ้า LCD ไม่เชื่อมต่อจะข้ามการใช้งาน
- 🛡️ **Graceful Degradation** - ระบบยังทำงานได้แม้ sensor บางตัวเสีย
- 🎯 **Multi-Zone Control** - ควบคุมหลายโซนพร้อมกัน
- 📊 **Real-time Monitoring** - ตรวจสอบข้อมูลแบบเรียลไทม์

---

## 🎛️ ระบบที่มีให้เลือก

### 1. 🌸 ระบบรดน้ำกล้วยไม้อัตโนมัติ (NEW!)
**📂 Path:** [`new System/orchid_watering_system/`](new System/orchid_watering_system/)

**🔧 Main Code:** [`orchid_watering_system.ino`](new System/orchid_watering_system/orchid_watering_system.ino)

**✨ Features:**
- ✅ รดน้ำ 6 โซนแยกกัน (Capacitive Soil Moisture)
- ✅ วัดอุณหภูมิ/ความชื้น (DHT22)
- ✅ วัดแสง (LDR)
- ✅ วัด pH และ EC ของดิน
- ✅ วัดคุณภาพอากาศ (CO2, Air Quality)
- ✅ ตรวจระดับน้ำและ Flow Rate
- ✅ **Offline Detection** - ข้าม sensor ที่เสีย
- ✅ **LCD Skip** - ถ้า LCD ไม่เชื่อมต่อ
- ✅ **Graceful Degradation** - ทำงานต่อได้แม้ sensor เสีย

**📖 Documentation:**
- [📋 คู่มือการใช้งาน](new System/orchid_watering_system/README_orchid_system.md)
- [⚙️ คู่มือการติดตั้ง](new System/orchid_watering_system/ORCHID_SETUP_GUIDE_TH.md)
- [📊 สรุประบบ](new System/orchid_watering_system/ORCHID_SUMMARY_TH.md)

---

### 2. 🍅 ระบบรดน้ำมะเขือเทศอัตโนมัติ (Enhanced!)
**📂 Path:** [`new System/tomato_watering/`](new System/tomato_watering/)

**🔧 Main Code:** [`tomato_watering.ino`](new System/tomato_watering/tomato_watering.ino)

**✨ Features:**
- ✅ รดน้ำ 4 โซนแยกกัน (Soil Moisture x6)
- ✅ วัดอุณหภูมิ/ความชื้น (DHT22)
- ✅ วัดแสง (LDR)
- ✅ วัด pH และ EC ของดิน
- ✅ วัดคุณภาพอากาศ (CO2, Air Quality)
- ✅ ตรวจระดับน้ำและ Flow Rate
- ✅ **Offline Detection** - ข้าม sensor ที่เสีย
- ✅ **LCD Skip** - ถ้า LCD ไม่เชื่อมต่อ
- ✅ **Graceful Degradation** - ทำงานต่อได้แม้ sensor เสีย

**📖 Documentation:**
- [📋 คู่มือการใช้งาน](new System/tomato_watering/README_tomato_system.md)

---

### 3. 🐱 ระบบให้อาหารแมวอัตโนมัติ (Enhanced!)
**📂 Path:** [`new System/cat_feeding_system/`](new System/cat_feeding_system/)

**🔧 Main Code:** [`cat_feeding_system.ino`](new System/cat_feeding_system/cat_feeding_system.ino)

**✨ Features:**
- ✅ วัดน้ำหนักอาหาร (Load Cell HX711)
- ✅ ตรวจจับแมว (PIR Sensor)
- ✅ ตรวจระยะห่าง (Ultrasonic HC-SR04)
- ✅ วัดอุณหภูมิ/ความชื้น (DHT22)
- ✅ วัดแสง (LDR)
- ✅ วัด pH และ EC ของน้ำ
- ✅ วัดคุณภาพอากาศ (CO2, Air Quality)
- ✅ ตรวจระดับน้ำและ Flow Rate
- ✅ **Offline Detection** - ข้าม sensor ที่เสีย
- ✅ **LCD Skip** - ถ้า LCD ไม่เชื่อมต่อ
- ✅ **Graceful Degradation** - ทำงานต่อได้แม้ sensor เสีย

**📖 Documentation:**
- [📋 คู่มือการใช้งาน](new System/cat_feeding_system/CAT_FEEDING_README_TH.md)
- [⚙️ คู่มือการติดตั้ง](new System/cat_feeding_system/CAT_FEEDING_SETUP_GUIDE_TH.md)

---

### 4. 🐦 ระบบให้อาหารนกอัตโนมัติ (Enhanced!)
**📂 Path:** [`new System/BirdFeedingSystem/`](new System/BirdFeedingSystem/)

**🔧 Main Code:** [`BirdFeedingSystem.ino`](new System/BirdFeedingSystem/BirdFeedingSystem.ino)

**✨ Features:**
- ✅ วัดน้ำหนักอาหาร (Load Cell HX711)
- ✅ ตรวจจับนก (PIR Sensor)
- ✅ วัดอุณหภูมิ/ความชื้น (DHT22)
- ✅ วัดแสง (LDR)
- ✅ วัด pH และ EC ของน้ำ
- ✅ วัดคุณภาพอากาศ (CO2, Air Quality)
- ✅ ตรวจระดับน้ำและ Flow Rate
- ✅ **Offline Detection** - ข้าม sensor ที่เสีย
- ✅ **LCD Skip** - ถ้า LCD ไม่เชื่อมต่อ
- ✅ **Graceful Degradation** - ทำงานต่อได้แม้ sensor เสีย

**📖 Documentation:**
- [📋 คู่มือการใช้งาน](new System/BirdFeedingSystem/README_bird_feeding.md)

---

### 5. 🌿 ระบบปลูกผักชีอัตโนมัติ (Enhanced!)
**📂 Path:** [`new System/cilantro_system/`](new System/cilantro_system/)

**🔧 Main Code:** [`cilantro_system.ino`](new System/cilantro_system/cilantro_system.ino)

**✨ Features:**
- ✅ รดน้ำ 5 โซนแยกกัน (Soil Moisture x5)
- ✅ วัดอุณหภูมิ/ความชื้น (DHT22)
- ✅ วัดแสง (LDR)
- ✅ วัด pH และ EC ของดิน
- ✅ วัดคุณภาพอากาศ (CO2, Air Quality)
- ✅ ตรวจระดับน้ำและ Flow Rate
- ✅ **Offline Detection** - ข้าม sensor ที่เสีย
- ✅ **LCD Skip** - ถ้า LCD ไม่เชื่อมต่อ
- ✅ **Graceful Degradation** - ทำงานต่อได้แม้ sensor เสีย

---

### 6. 🍄 ระบบปลูกเห็ดอัตโนมัติ (Enhanced!)
**📂 Path:** [`new System/standalone_projects_mushroom/`](new System/standalone_projects_mushroom/)

**🔧 Main Code:** [`standalone_projects_mushroom.ino`](new System/standalone_projects_mushroom/standalone_projects_mushroom.ino)

**✨ Features:**
- ✅ รดน้ำ 1 โซน (Soil Moisture)
- ✅ วัดอุณหภูมิ/ความชื้น (DHT22)
- ✅ วัดแสง (LDR)
- ✅ วัด pH และ EC ของดิน
- ✅ วัดคุณภาพอากาศ (CO2, Air Quality)
- ✅ ตรวจระดับน้ำและ Flow Rate
- ✅ **Offline Detection** - ข้าม sensor ที่เสีย
- ✅ **LCD Skip** - ถ้า LCD ไม่เชื่อมต่อ
- ✅ **Graceful Degradation** - ทำงานต่อได้แม้ sensor เสีย

---

## 🔧 ระบบ Sensor และ Offline Detection

### 📊 Sensor Suite ที่ใช้ในทุกระบบ

| Sensor | หน้าที่ | ระบบที่ใช้ | Offline Detection |
|--------|--------|------------|-------------------|
| **DHT22** | อุณหภูมิ/ความชื้น | ทุกระบบ | ✅ |
| **LDR** | วัดแสง | ทุกระบบ | ✅ |
| **pH Sensor** | วัดความเป็นกรด-ด่าง | ทุกระบบ | ✅ |
| **EC Sensor** | วัดค่าการนำไฟฟ้า | ทุกระบบ | ✅ |
| **CO2 Sensor** | วัดคาร์บอนไดออกไซด์ | ทุกระบบ | ✅ |
| **Air Quality** | วัดคุณภาพอากาศ | ทุกระบบ | ✅ |
| **Water Level** | ตรวจระดับน้ำ | ทุกระบบ | ✅ |
| **Flow Sensor** | วัดอัตราการไหล | ทุกระบบ | ✅ |
| **Soil Moisture** | วัดความชื้นดิน | ระบบรดน้ำ | ✅ |
| **Load Cell** | วัดน้ำหนัก | ระบบให้อาหาร | ✅ |
| **PIR** | ตรวจจับการเคลื่อนไหว | ระบบให้อาหาร | ✅ |
| **Ultrasonic** | วัดระยะห่าง | ระบบให้อาหาร | ✅ |
| **LCD I2C** | แสดงผล | ทุกระบบ | ✅ Skip |

### 🛡️ Offline Detection System

```cpp
struct SensorStatus {
  bool isOnline;
  unsigned long lastReading;
  float lastValue;
  int errorCount;
  String sensorName;
};
```

**✨ Features:**
- 🔍 **Timeout Detection** - 30 วินาที
- 🔄 **Retry Mechanism** - ทดลองใหม่ทุก 1 นาที
- ❌ **Error Counting** - 3 ครั้งแล้วข้าม
- 🛡️ **Graceful Degradation** - ทำงานต่อได้
- 📺 **LCD Skip** - ข้ามถ้าไม่เชื่อมต่อ

---

## 🚀 เริ่มต้นใช้งาน (Quick Start)

### 1. 📥 ดาวน์โหลดและเตรียม
```bash
git clone [repository-url]
cd cod
```

### 2. 🛠️ เลือกระบบที่ต้องการ
```bash
# ระบบรดน้ำกล้วยไม้ (ใหม่!)
cd "new System/orchid_watering_system/"
open orchid_watering_system.ino

# ระบบรดน้ำมะเขือเทศ
cd "new System/tomato_watering/"
open tomato_watering.ino

# ระบบให้อาหารแมว
cd "new System/cat_feeding_system/"
open cat_feeding_system.ino

# ระบบให้อาหารนก
cd "new System/BirdFeedingSystem/"
open BirdFeedingSystem.ino

# ระบบปลูกผักชี
cd "new System/cilantro_system/"
open cilantro_system.ino

# ระบบปลูกเห็ด
cd "new System/standalone_projects_mushroom/"
open standalone_projects_mushroom.ino
```

### 3. ⚙️ แก้ไขการตั้งค่า
```cpp
// แก้ไขในไฟล์ .ino ที่เลือก
const char* ssid = "ชื่อ_WiFi_ของคุณ";
const char* password = "รหัสผ่าน_WiFi";
const char* blynk_token = "Auth_Token_จาก_Blynk";
```

### 4. 🔧 ติดตั้งไลบรารี
```
Arduino IDE → Tools → Manage Libraries

ไลบรารีพื้นฐาน:
- ArduinoJson
- NTPClient
- HX711
- ESP32Servo
- Wire
- LiquidCrystal_I2C
- DHT sensor library

ไลบรารีเฉพาะระบบ:
- RDTRC_Common_Library.h
- RDTRC_LCD_Library.h
- RDTRC_Orchid_Library.h (สำหรับระบบกล้วยไม้)
```

### 5. 📤 อัพโหลดและทดสอบ
```
Board: ESP32 Dev Module
Port: เลือกพอร์ต ESP32
Upload Speed: 921600

กด Upload (→)
ดู Serial Monitor (115200 baud)
```

---

## 📊 สรุปการปรับปรุงเวอร์ชัน 3.0

| ด้าน | เวอร์ชัน 2.0 | เวอร์ชัน 3.0 |
|------|---------------|---------------|
| **จำนวนระบบ** | 3 ระบบ | 6 ระบบ |
| **Sensor Suite** | ⚠️ พื้นฐาน | ✅ ครบครัน |
| **Offline Detection** | ❌ ไม่มี | ✅ ครบครัน |
| **LCD Skip Logic** | ❌ ไม่มี | ✅ ครบครัน |
| **Graceful Degradation** | ❌ ไม่มี | ✅ ครบครัน |
| **Multi-Zone Control** | ⚠️ จำกัด | ✅ ครบครัน |
| **Real-time Monitoring** | ⚠️ พื้นฐาน | ✅ ครบครัน |
| **Error Handling** | ⚠️ พื้นฐาน | ✅ ครบครัน |

---

## 📚 เอกสารเพิ่มเติม

### 📖 คู่มือหลัก
- [📊 **RDTRC_Sensor_Offline_Detection_Summary.md**](new System/RDTRC_Sensor_Offline_Detection_Summary.md) - สรุประบบ Sensor และ Offline Detection
- [📋 **BIRD_FEEDING_SYSTEM_COMPLETE.md**](docs/BIRD_FEEDING_SYSTEM_COMPLETE.md)
- [📋 **CAT_FEEDING_SYSTEM_COMPLETE.md**](docs/CAT_FEEDING_SYSTEM_COMPLETE.md)
- [📋 **CILANTRO_SYSTEM_COMPLETE.md**](docs/CILANTRO_SYSTEM_COMPLETE.md)
- [📋 **TOMATO_WATERING_SYSTEM_COMPLETE.md**](docs/TOMATO_WATERING_SYSTEM_COMPLETE.md)

### 🔧 คู่มือขั้นสูง
- [⭐ ฟีเจอร์ขั้นสูง](systems/common/docs/ENHANCED_FEATURES_TH.md)
- [📱 การตั้งค่า Blynk](systems/common/docs/blink_app_setup.md)
- [🌐 Webhook Setup](systems/common/docs/webhook_setup_guide.md)

---

## 🎯 การเลือกระบบที่เหมาะสม

### 🌸 สำหรับการปลูกกล้วยไม้
➡️ **[ระบบรดน้ำกล้วยไม้](new System/orchid_watering_system/)**
- รดน้ำ 6 โซนแยกกัน
- วัด pH และ EC ของดิน
- ควบคุมแสงและอุณหภูมิ
- เหมาะสำหรับกล้วยไม้หลายชนิด

### 🍅 สำหรับการปลูกมะเขือเทศ
➡️ **[ระบบรดน้ำมะเขือเทศ](new System/tomato_watering/)**
- รดน้ำ 4 โซนแยกกัน
- วัดความชื้นดิน 6 จุด
- ควบคุมสภาพแวดล้อม
- เหมาะสำหรับการปลูกเชิงพาณิชย์

### 🐱 สำหรับการเลี้ยงแมว
➡️ **[ระบบให้อาหารแมว](new System/cat_feeding_system/)**
- วัดน้ำหนักอาหารแม่นยำ
- ตรวจจับแมวอัตโนมัติ
- ป้องกันการให้อาหารมากเกินไป
- เหมาะสำหรับแมวหลายตัว

### 🐦 สำหรับการเลี้ยงนก
➡️ **[ระบบให้อาหารนก](new System/BirdFeedingSystem/)**
- วัดน้ำหนักอาหาร
- ตรวจจับนก
- ควบคุมปริมาณอาหาร
- เหมาะสำหรับนกหลายตัว

### 🌿 สำหรับการปลูกผักชี
➡️ **[ระบบปลูกผักชี](new System/cilantro_system/)**
- รดน้ำ 5 โซนแยกกัน
- ควบคุมสภาพแวดล้อม
- เหมาะสำหรับการปลูกผักชี

### 🍄 สำหรับการปลูกเห็ด
➡️ **[ระบบปลูกเห็ด](new System/standalone_projects_mushroom/)**
- ควบคุมความชื้น
- ควบคุมอุณหภูมิ
- เหมาะสำหรับการปลูกเห็ด

---

## 💰 ค่าใช้จ่ายประมาณการ

| ระบบ | อุปกรณ์หลัก | ราคาประมาณ | ความซับซ้อน |
|------|-------------|-------------|-------------|
| 🌸 **Orchid Watering** | ESP32 + Sensors + 6 Valves | **฿3,500-5,000** | ⭐⭐⭐⭐⭐ |
| 🍅 **Tomato Watering** | ESP32 + Sensors + 4 Valves | **฿3,000-4,500** | ⭐⭐⭐⭐⭐ |
| 🐱 **Cat Feeding** | ESP32 + Load Cell + Sensors | **฿2,500-3,500** | ⭐⭐⭐⭐ |
| 🐦 **Bird Feeding** | ESP32 + Load Cell + Sensors | **฿2,500-3,500** | ⭐⭐⭐⭐ |
| 🌿 **Cilantro Growing** | ESP32 + Sensors + 5 Valves | **฿2,800-4,000** | ⭐⭐⭐⭐⭐ |
| 🍄 **Mushroom Growing** | ESP32 + Sensors + 1 Valve | **฿2,000-3,000** | ⭐⭐⭐⭐ |

---

## 🔧 วิธีใช้งานแบบเร็ว

### 1️⃣ เลือกระบบ
```bash
# เข้าไปใน folder ระบบที่ต้องการ
cd "new System/[system_name]/"
```

### 2️⃣ เปิดโค้ด
```bash
# เปิดไฟล์หลัก
open [system_name].ino
```

### 3️⃣ แก้ไขการตั้งค่า
```cpp
// แก้ไขเพียง 3 บรรทัด
const char* ssid = "ชื่อ_WiFi_ของคุณ";
const char* password = "รหัสผ่าน_WiFi";  
const char* blynk_token = "Token_จาก_Blynk";
```

### 4️⃣ อัพโหลดและใช้งาน
- ติดตั้งไลบรารีตามรายการ
- อัพโหลดไปยัง ESP32
- ดู boot screen "FW make by RDTRC"
- เข้าใช้งานผ่าน web หรือ mobile app

---

## 🌟 ไฮไลท์ของเวอร์ชัน 3.0

### 🛡️ Offline Detection System
ทุกระบบสามารถ:
- ตรวจสอบ sensor ที่เสีย
- ข้าม sensor ที่ไม่ทำงาน
- ทำงานต่อได้แม้ sensor บางตัวเสีย
- แสดงสถานะ sensor แบบเรียลไทม์

### 📺 LCD Skip Logic
- ตรวจสอบการเชื่อมต่อ LCD
- ข้ามการใช้งานถ้าไม่เชื่อมต่อ
- ลดการใช้พลังงาน
- ป้องกันข้อผิดพลาด

### 🌡️ Comprehensive Sensor Suite
ทุกระบบมี sensor ที่เกี่ยวข้องครบถ้วน:
- **Environmental**: DHT22, LDR, CO2, Air Quality
- **Soil/Water**: pH, EC, Soil Moisture, Water Level, Flow
- **Control**: Load Cell, PIR, Ultrasonic
- **Display**: LCD I2C

### 🔄 Graceful Degradation
- ระบบยังทำงานได้แม้ sensor บางตัวเสีย
- ปรับการทำงานตาม sensor ที่ใช้งานได้
- แจ้งเตือนเมื่อ sensor เสีย
- บันทึกข้อมูลการทำงาน

---

## 📞 การสนับสนุน

### 🆘 ต้องการความช่วยเหลือ?

1. **ดูเอกสาร** - เริ่มจาก README ของระบบที่เลือก
2. **ตรวจสอบ Troubleshooting** - ในคู่มือการติดตั้ง
3. **ดู Sensor Status** - ตรวจสอบ sensor ที่เสีย
4. **รายงานปัญหา** - สร้าง GitHub Issue

### 🤝 ต้องการมีส่วนร่วม?

- 🔧 **Code**: ส่ง Pull Request
- 📝 **Documentation**: ปรับปรุงเอกสาร
- 🐛 **Testing**: รายงานปัญหา
- 💡 **Ideas**: แนะนำฟีเจอร์ใหม่

---

## 🎯 Roadmap

### 📅 Version 3.1 (Q2 2024)
- [ ] Machine Learning integration
- [ ] Advanced analytics dashboard
- [ ] Multi-language support
- [ ] Cloud data sync

### 📅 Version 3.2 (Q3 2024)
- [ ] AI-powered recommendations
- [ ] Weather integration
- [ ] Mobile app development
- [ ] Commercial deployment

### 📅 Version 4.0 (Q4 2024)
- [ ] Edge AI processing
- [ ] Blockchain integration
- [ ] IoT marketplace
- [ ] Enterprise solutions

---

<div align="center">

## 🎉 ยินดีต้อนรับสู่ RDTRC IoT Systems v3.0!

**Sensor Suite ครบครัน | Offline Detection | Graceful Degradation**

[![Get Started](https://img.shields.io/badge/Get%20Started-Now-green.svg?style=for-the-badge)](new System/)
[![Documentation](https://img.shields.io/badge/Read%20Docs-blue.svg?style=for-the-badge)](docs/)
[![Support](https://img.shields.io/badge/Get%20Support-orange.svg?style=for-the-badge)](https://github.com)

**Firmware made by RDTRC - Advanced IoT Solutions with Smart Sensor Management**

### 🔗 Quick Links

| 🌸 [Orchid](new System/orchid_watering_system/) | 🍅 [Tomato](new System/tomato_watering/) | 🐱 [Cat](new System/cat_feeding_system/) |
|:---:|:---:|:---:|
| ระบบรดน้ำกล้วยไม้ | ระบบรดน้ำมะเขือเทศ | ระบบให้อาหารแมว |
| 6 โซน + Sensor ครบ | 4 โซน + Sensor ครบ | Load Cell + Sensor ครบ |
| ฿3,500-5,000 | ฿3,000-4,500 | ฿2,500-3,500 |

| 🐦 [Bird](new System/BirdFeedingSystem/) | 🌿 [Cilantro](new System/cilantro_system/) | 🍄 [Mushroom](new System/standalone_projects_mushroom/) |
|:---:|:---:|:---:|
| ระบบให้อาหารนก | ระบบปลูกผักชี | ระบบปลูกเห็ด |
| Load Cell + Sensor ครบ | 5 โซน + Sensor ครบ | 1 โซน + Sensor ครบ |
| ฿2,500-3,500 | ฿2,800-4,000 | ฿2,000-3,000 |

</div>
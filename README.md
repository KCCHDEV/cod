# 🤖 RDTRC IoT Systems Collection
## ชุดระบบ IoT ครบครัน - เวอร์ชัน 2.0

> **Firmware made by: RDTRC**  
> **Version: 2.0 - Organized Structure**  
> **Updated: 2024**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)](https://www.espressif.com/)
[![Firmware](https://img.shields.io/badge/Firmware-RDTRC-red.svg)](https://github.com)

---

## 🎉 **อัพเดทใหม่! โครงสร้างระบบถูกจัดระเบียบใหม่**

### ✨ สิ่งที่เปลี่ยนแปลง
- 📁 **โครงสร้างแบบ Modular** - แยกระบบเป็น folder เดียวกัน
- 🖥️ **Boot Screen มาตรฐาน** - แสดง "FW make by RDTRC" ทุกระบบ
- 🔧 **SystemBase Class** - คลาสพื้นฐานร่วมกัน
- 📱 **Web Interface ปรับปรุง** - หน้าตาและฟีเจอร์ดีขึ้น

---

## 🎛️ ระบบที่มีให้เลือก

### 1. 🐱 ระบบให้อาหารแมวอัตโนมัติ
**📂 Path:** [`systems/cat_feeding/`](systems/cat_feeding/)

**🔧 Main Code:** [`cat_feeding_system.ino`](systems/cat_feeding/code/cat_feeding_system.ino)

**✨ Features:**
- ✅ ให้อาหารตามเวลา (6 เวลา/วัน)
- ✅ วัดน้ำหนักอาหาร (Load Cell)
- ✅ ตรวจจับแมว (PIR Sensor)
- ✅ ควบคุมปริมาณ 5-100g
- ✅ แจ้งเตือนอาหารหมด
- ✅ **Boot Screen: "FW make by RDTRC"**

**📖 Documentation:**
- [📋 คู่มือการใช้งาน](systems/cat_feeding/docs/CAT_FEEDING_README_TH.md)
- [⚙️ คู่มือการติดตั้ง](systems/cat_feeding/docs/CAT_FEEDING_SETUP_GUIDE_TH.md)
- [🔌 แผนผังการเชื่อมต่อ](systems/cat_feeding/wiring/wiring_diagram_cat_feeding.txt)
- [📦 รายการไลบรารี](systems/cat_feeding/libraries/libraries_cat_feeding.txt)

---

### 2. 🐦 ระบบให้อาหารนกอัตโนมัติ
**📂 Path:** [`systems/bird_feeding/`](systems/bird_feeding/)

**🔧 Main Code:** [`bird_feeding_system.ino`](systems/bird_feeding/code/bird_feeding_system.ino)

**✨ Features:**
- ✅ ให้อาหารและน้ำแยกกัน
- ✅ ตรวจระดับอาหาร (Ultrasonic)
- ✅ ตรวจระดับน้ำ (ADC)
- ✅ ตรวจจับนก (PIR Sensor)
- ✅ ปั๊มน้ำอัตโนมัติ
- ✅ **Boot Screen: "FW make by RDTRC"**

**📖 Documentation:**
- [📋 คู่มือการใช้งาน](systems/bird_feeding/docs/README_bird_feeding.md)
- [🔌 แผนผังการเชื่อมต่อ](systems/bird_feeding/wiring/wiring_diagram_bird_feeding.txt)
- [📦 รายการไลบรารี](systems/bird_feeding/libraries/libraries_bird_feeding.txt)

**🧪 Test Files:**
- [⚡ Quick Test](systems/bird_feeding/code/bird_feeding_quick_test.ino)
- [🔬 Full Test](systems/bird_feeding/code/bird_feeding_test.ino)

---

### 3. 🍅 ระบบรดน้ำมะเขือเทศอัตโนมัติ
**📂 Path:** [`systems/tomato_watering/`](systems/tomato_watering/)

**🔧 Main Code:** [`tomato_watering_system.ino`](systems/tomato_watering/code/tomato_watering_system.ino)

**✨ Features:**
- ✅ รดน้ำตามเวลาและความชื้นดิน
- ✅ ตรวจความชื้นดิน (Soil Moisture)
- ✅ ตรวจระดับน้ำในถัง
- ✅ วัดอุณหภูมิ/ความชื้นอากาศ (DHT22)
- ✅ ปั๊มน้ำและวาล์วควบคุม
- ✅ **Boot Screen: "FW make by RDTRC"**

**📖 Documentation:**
- [📋 คู่มือการใช้งาน](systems/tomato_watering/docs/README_tomato_system.md)
- [🔌 แผนผังการเชื่อมต่อ](systems/tomato_watering/wiring/wiring_diagram_tomato.txt)
- [📦 รายการไลบรารี](systems/tomato_watering/libraries/libraries_tomato.txt)

**🌿 Variant Systems:**
- [🌿 Cilantro Watering](systems/tomato_watering/code/cilantro_watering_system.ino)
- [🌹 Rose Watering](systems/tomato_watering/code/rose_watering_system.ino)
- [🍄 Mushroom System](systems/tomato_watering/code/mushroom_system.ino)

---

## 🔧 ระบบพื้นฐานร่วม (Common Base System)

### 📂 Path: [`systems/common/`](systems/common/)

**🔧 Core Files:**
- [`system_base.h`](systems/common/base_code/system_base.h) - Header file หลัก
- [`system_base.cpp`](systems/common/base_code/system_base.cpp) - Implementation

**✨ Features:**
- 🖥️ **Boot Screen**: แสดง "FW make by RDTRC" Version 2.0
- 🌐 **WiFi Management**: การเชื่อมต่อและ reconnect อัตโนมัติ
- 📱 **Web Server**: หน้าเว็บมาตรฐานสำหรับทุกระบบ
- 🔧 **API Endpoints**: REST API ครบครัน
- 📊 **Status Management**: จัดการสถานะระบบ
- 💾 **EEPROM Storage**: บันทึกการตั้งค่า
- 🚨 **Error Handling**: จัดการข้อผิดพลาดแบบมาตรฐาน
- 🎵 **Sound & LED**: เสียงและไฟแสดงสถานะ

---

## 🚀 เริ่มต้นใช้งาน (Quick Start)

### 1. 📥 ดาวน์โหลดและเตรียม
```bash
git clone [repository-url]
cd RDTRC-IoT-Systems
```

### 2. 🛠️ เลือกระบบที่ต้องการ
```bash
# ระบบให้อาหารแมว
cd systems/cat_feeding/code/
open cat_feeding_system.ino

# ระบบให้อาหารนก  
cd systems/bird_feeding/code/
open bird_feeding_system.ino

# ระบบรดน้ำมะเขือเทศ
cd systems/tomato_watering/code/
open tomato_watering_system.ino
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
- RTClib  
- LiquidCrystal I2C
- BlynkSimpleEsp32

ไลบรารีเฉพาะระบบ:
- Cat: ESP32Servo, HX711
- Bird: ESP32Servo
- Tomato: DHT sensor library
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

## 🎨 Boot Screen Preview

เมื่อระบบเริ่มต้น จะเห็น:

```
╔════════════════╗
║ FW make by RDTRC║
║ Version 2.0     ║
╚════════════════╝
        ↓
╔════════════════╗
║ [System Name]  ║
║ Initializing...║
╚════════════════╝
        ↓
╔════════════════╗
║ [System Name]  ║
║ ................║
╚════════════════╝
        ↓
╔════════════════╗
║ [System Name]  ║
║ Ready - v2.0   ║
╚════════════════╝
```

พร้อมเสียงประกอบ: 🎵 Boot melody

---

## 📊 สรุปการปรับปรุง

| ด้าน | ก่อนปรับปรุง | หลังปรับปรุง |
|------|---------------|---------------|
| **โครงสร้าง** | ❌ ไฟล์กระจัด | ✅ จัดระเบียบเป็น folder |
| **Boot Screen** | ❌ ไม่มี | ✅ "FW make by RDTRC" |
| **Code Reuse** | ❌ เขียนซ้ำ | ✅ SystemBase Class |
| **Documentation** | ⚠️ กระจัด | ✅ จัดเป็นหมวดหมู่ |
| **API Structure** | ⚠️ ไม่เหมือนกัน | ✅ มาตรฐานเดียวกัน |
| **Error Handling** | ⚠️ พื้นฐาน | ✅ ครบครันทุกระบบ |
| **User Experience** | ⚠️ แตกต่างกัน | ✅ สอดคล้องกันทุกระบบ |

---

## 📚 เอกสารเพิ่มเติม

### 📖 คู่มือหลัก
- [📁 **MASTER_README.md**](systems/common/docs/MASTER_README.md) - ภาพรวมทั้งหมด
- [🔧 **SYSTEM_STRUCTURE_GUIDE.md**](systems/common/docs/SYSTEM_STRUCTURE_GUIDE.md) - คู่มือโครงสร้างใหม่

### 📋 คู่มือการติดตั้ง
- [📦 การติดตั้งไลบรารี](systems/common/docs/LIBRARIES_INSTALLATION_TH.md)
- [🔌 แผนผังการเชื่อมต่อ](systems/common/docs/CONNECTION_DIAGRAMS_TH.md)
- [⚙️ คู่มือการติดตั้งครบครัน](systems/common/docs/README_COMPLETE_SETUP_TH.md)

### 🔧 คู่มือขั้นสูง
- [⭐ ฟีเจอร์ขั้นสูง](systems/common/docs/ENHANCED_FEATURES_TH.md)
- [📱 การตั้งค่า Blynk](systems/common/docs/blink_app_setup.md)
- [🌐 Webhook Setup](systems/common/docs/webhook_setup_guide.md)

---

## 🎯 การเลือกระบบที่เหมาะสม

### 🐱 สำหรับการเลี้ยงแมว
➡️ **[ระบบให้อาหารแมว](systems/cat_feeding/)**
- วัดน้ำหนักอาหารแม่นยำ
- ตรวจจับแมวอัตโนมัติ
- ป้องกันการให้อาหารมากเกินไป

### 🐦 สำหรับการเลี้ยงนก
➡️ **[ระบบให้อาหารนก](systems/bird_feeding/)**
- ให้อาหารและน้ำแยกกัน
- ตรวจระดับอาหาร/น้ำ
- เหมาะสำหรับนกหลายตัว

### 🍅 สำหรับการปลูกผัก
➡️ **[ระบบรดน้ำมะเขือเทศ](systems/tomato_watering/)**
- ตรวจความชื้นดินอัตโนมัติ
- วัดสภาพอากาศ
- ควบคุมปั๊มน้ำและวาล์ว

---

## 💰 ค่าใช้จ่ายประมาณการ

| ระบบ | อุปกรณ์หลัก | ราคาประมาณ | ความซับซ้อน |
|------|-------------|-------------|-------------|
| 🐱 **Cat Feeding** | ESP32 + Servo + Load Cell + PIR | **฿1,500-2,500** | ⭐⭐⭐ |
| 🐦 **Bird Feeding** | ESP32 + Servo x2 + Ultrasonic + Pump | **฿1,800-2,800** | ⭐⭐⭐⭐ |
| 🍅 **Tomato Watering** | ESP32 + Pump + Sensors + DHT22 | **฿2,000-3,000** | ⭐⭐⭐⭐⭐ |

---

## 🔧 วิธีใช้งานแบบเร็ว

### 1️⃣ เลือกระบบ
```bash
# เข้าไปใน folder ระบบที่ต้องการ
cd systems/[system_name]/code/
```

### 2️⃣ เปิดโค้ด
```bash
# เปิดไฟล์หลัก (ไม่ใช่ *_old.ino)
open [system_name]_system.ino
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

## 🌟 ไฮไลท์ของเวอร์ชัน 2.0

### 🖥️ Boot Screen ที่สวยงาม
ทุกระบบจะแสดง:
```
FW make by RDTRC
Version 2.0
    ↓
[System Name]
Initializing...
    ↓
Loading Animation
................
    ↓
System Ready!
```

### 🔧 SystemBase Class
- ไม่ต้องเขียนโค้ดพื้นฐานซ้ำ
- Error handling อัตโนมัติ
- Web interface มาตรฐาน
- API endpoints ครบครัน

### 📱 Web Interface ปรับปรุง
- หน้าตาสวยงามและใช้งานง่าย
- ตอบสนองบนมือถือ
- แสดงข้อมูลแบบเรียลไทม์
- ควบคุมระบบได้ครบครัน

### 🔄 การจัดการที่ดีขึ้น
- จัดเก็บไฟล์เป็นระเบียบ
- เอกสารครบครันแยกตามหมวด
- ไฟล์เดิมยังคงไว้ (*_old.ino)
- อัพเกรดได้ง่าย

---

## 📞 การสนับสนุน

### 🆘 ต้องการความช่วยเหลือ?

1. **ดูเอกสาร** - เริ่มจาก README ของระบบที่เลือก
2. **ตรวจสอบ Troubleshooting** - ในคู่มือการติดตั้ง
3. **ดู Common Issues** - ใน SYSTEM_STRUCTURE_GUIDE.md
4. **รายงานปัญหา** - สร้าง GitHub Issue

### 🤝 ต้องการมีส่วนร่วม?

- 🔧 **Code**: ส่ง Pull Request
- 📝 **Documentation**: ปรับปรุงเอกสาร
- 🐛 **Testing**: รายงานปัญหา
- 💡 **Ideas**: แนะนำฟีเจอร์ใหม่

---

## 🎯 Roadmap

### 📅 Version 2.1 (Q2 2024)
- [ ] ESP32-CAM integration
- [ ] SD Card data logging  
- [ ] Email notifications
- [ ] Advanced web dashboard

### 📅 Version 2.2 (Q3 2024)
- [ ] Telegram bot
- [ ] Voice control
- [ ] React Native mobile app
- [ ] Multi-system dashboard

### 📅 Version 3.0 (Q4 2024)
- [ ] AI recommendations
- [ ] Weather integration
- [ ] Cloud analytics
- [ ] Commercial kit

---

<div align="center">

## 🎉 ยินดีต้อนรับสู่ RDTRC IoT Systems v2.0!

**โครงสร้างใหม่ | Boot Screen สวยงาม | ระบบมาตรฐาน**

[![Get Started](https://img.shields.io/badge/Get%20Started-Now-green.svg?style=for-the-badge)](systems/)
[![Documentation](https://img.shields.io/badge/Read%20Docs-blue.svg?style=for-the-badge)](systems/common/docs/)
[![Support](https://img.shields.io/badge/Get%20Support-orange.svg?style=for-the-badge)](https://github.com)

**Firmware made by RDTRC - Professional IoT Solutions for Agriculture & Pet Care**

### 🔗 Quick Links

| 🐱 [Cat Feeding](systems/cat_feeding/) | 🐦 [Bird Feeding](systems/bird_feeding/) | 🍅 [Tomato Watering](systems/tomato_watering/) |
|:---:|:---:|:---:|
| ให้อาหารแมวอัตโนมัติ | ให้อาหารนกอัตโนมัติ | รดน้ำมะเขือเทศอัตโนมัติ |
| Load Cell + PIR | Ultrasonic + Pump | Soil Sensor + DHT22 |
| ฿1,500-2,500 | ฿1,800-2,800 | ฿2,000-3,000 |

</div>
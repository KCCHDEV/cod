# 🌱 Universal Watering System - Plant Configuration Guide

## วิธีการตั้งค่าสำหรับพืชต่างๆ

เพียงแค่เปลี่ยน 3 บรรทัดในโค้ด คุณก็สามารถใช้ระบบเดียวกันกับพืชหลายชนิดได้!

### 🔧 การตั้งค่า

เปิดไฟล์ `universal_watering_system.ino` แล้วแก้ไขบรรทัดที่ 32-34:

```cpp
#define PLANT_TYPE "CILANTRO"    // เปลี่ยนตรงนี้
#define SYSTEM_NAME "ผักชีฟลั่ง"   // เปลี่ยนตรงนี้  
#define SYSTEM_EMOJI "🌿"        // เปลี่ยนตรงนี้
```

---

## 🌿 1. ผักชีฟลั่ง (Cilantro)
```cpp
#define PLANT_TYPE "CILANTRO"
#define SYSTEM_NAME "ผักชีฟลั่ง"
#define SYSTEM_EMOJI "🌿"
```

**การตั้งค่า:**
- 💧 รดน้ำเมื่อ: < 30%
- 🛑 หยุดเมื่อ: 70%
- ⏰ ตารางรดน้ำ: 6:00, 18:00 (2 ครั้ง/วัน)
- 🚰 ระยะห่าง: 30 นาที
- 📊 จำกัด: 6 ครั้ง/วัน
- 🕐 เวลารดน้ำ: 3 นาที

**WiFi Hotspot:** `CilantroWatering-Setup` / `cilantro123`

---

## 🍄 2. เห็ดนางฟ้า (Mushroom)
```cpp
#define PLANT_TYPE "MUSHROOM"
#define SYSTEM_NAME "เห็ดนางฟ้า"
#define SYSTEM_EMOJI "🍄"
```

**การตั้งค่า:**
- 💧 รดน้ำเมื่อ: < 60%
- 🛑 หยุดเมื่อ: 85%
- ⏰ ตารางรดน้ำ: 6:00, 12:00, 18:00 (3 ครั้ง/วัน)
- 🚰 ระยะห่าง: 20 นาที
- 📊 จำกัด: 8 ครั้ง/วัน
- 🕐 เวลารดน้ำ: 2 นาที

**WiFi Hotspot:** `MushroomWatering-Setup` / `mushroom123`

---

## 🥬 3. ผักสลัด (Lettuce)
```cpp
#define PLANT_TYPE "LETTUCE"
#define SYSTEM_NAME "ผักสลัด"
#define SYSTEM_EMOJI "🥬"
```

**การตั้งค่า:**
- 💧 รดน้ำเมื่อ: < 40%
- 🛑 หยุดเมื่อ: 75%
- ⏰ ตารางรดน้ำ: 7:00, 17:00 (2 ครั้ง/วัน)
- 🚰 ระยะห่าง: 25 นาที
- 📊 จำกัด: 5 ครั้ง/วัน
- 🕐 เวลารดน้ำ: 4 นาที

**WiFi Hotspot:** `LettuceWatering-Setup` / `lettuce123`

---

## 🍅 4. มะเขือเทศ (Tomato)
```cpp
#define PLANT_TYPE "TOMATO"
#define SYSTEM_NAME "มะเขือเทศ"
#define SYSTEM_EMOJI "🍅"
```

**การตั้งค่า:**
- 💧 รดน้ำเมื่อ: < 35%
- 🛑 หยุดเมื่อ: 65%
- ⏰ ตารางรดน้ำ: 6:30, 18:30 (2 ครั้ง/วัน)
- 🚰 ระยะห่าง: 40 นาที
- 📊 จำกัด: 4 ครั้ง/วัน
- 🕐 เวลารดน้ำ: 5 นาที

**WiFi Hotspot:** `TomatoWatering-Setup` / `tomato123`

---

## 🌶️ 5. พริก (Pepper)
```cpp
#define PLANT_TYPE "PEPPER"
#define SYSTEM_NAME "พริก"
#define SYSTEM_EMOJI "🌶️"
```

**การตั้งค่า:**
- 💧 รดน้ำเมื่อ: < 25%
- 🛑 หยุดเมื่อ: 60%
- ⏰ ตารางรดน้ำ: 6:00, 19:00 (2 ครั้ง/วัน)
- 🚰 ระยะห่าง: 45 นาที
- 📊 จำกัด: 4 ครั้ง/วัน
- 🕐 เวลารดน้ำ: 4 นาที

**WiFi Hotspot:** `PepperWatering-Setup` / `pepper123`

---

## 🔧 คุณสมบัติที่เหมือนกันทุกระบบ

### 📱 **LCD Auto-Detection**
- ตรวจหา LCD 16x2 อัตโนมัติ
- ที่อยู่ I2C: 0x27, 0x3F, 0x20, 0x38
- แสดง: ความชื้น, สถานะ WiFi, เวลา

### 📷 **Blink Integration**
- เชื่อมต่อกล้อง Blink
- อัพเดทสถานะทุก 5 นาที
- แสดงสถานะการรดน้ำ

### 🌐 **WiFi Manager**
- Portal ตั้งค่า WiFi อัตโนมัติ
- ตั้งค่า Blink ผ่านเว็บ
- ตั้งค่า Webhook

### 📊 **Web Interface**
- แสดงสถานะแบบเรียลไทม์
- ควบคุมการรดน้ำแมนนวล
- แสดงสถานะอุปกรณ์ทั้งหมด

### ⚙️ **Hardware Requirements**
- ESP32 Development Board
- MH Soil Moisture Sensor (AO → GPIO 36, DO → GPIO 35)
- Relay Module (GPIO 5)
- Status LED (GPIO 2)
- WiFi Reset Button (GPIO 0)
- RTC DS3231 (optional - GPIO 21, 22)
- LCD 16x2 I2C (optional - auto-detect)

---

## 🚀 ขั้นตอนการใช้งาน

1. **เลือกพืช** - แก้ไข `PLANT_TYPE` ในโค้ด
2. **อัพโหลด** - Upload โค้ดลง ESP32
3. **ตั้งค่า WiFi** - เชื่อมต่อ hotspot แล้วเลือกเครือข่าย
4. **ตั้งค่า Blink** (ทางเลือก) - กรอกข้อมูล Blink ในหน้าตั้งค่า
5. **เสร็จสิ้น** - ระบบจะทำงานอัตโนมัติ!

## 📋 Status Levels

แต่ละพืชจะมีระดับสถานะที่แตกต่างกัน:

- **Optimal** 🟢: ความชื้นเหมาะสม
- **Good** 🟡: ความชื้นดี
- **Slightly Dry** 🟠: แห้งเล็กน้อย
- **Critical** 🔴: แห้งมาก - ต้องรดน้ำทันที

## 💡 เคล็ดลับ

- ใช้โค้ดเดียวกันได้กับพืชหลายชนิด
- เปลี่ยนแค่ 3 บรรทัดแรก
- ระบบจะปรับค่าทั้งหมดอัตโนมัติ
- รองรับอุปกรณ์เสริม (RTC, LCD, Blink) แบบ optional
- Web interface ปรับธีมตามชนิดพืช
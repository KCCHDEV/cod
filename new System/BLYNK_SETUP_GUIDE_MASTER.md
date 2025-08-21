# 📱 คู่มือการตั้งค่า Blynk สำหรับทุกโปรเจค RDTRC

## 🎯 **ภาพรวม**

คู่มือนี้รวมการตั้งค่า Blynk สำหรับทุกระบบ IoT ของ RDTRC ที่มีการอัพเดทระบบ Sensor และ Offline Detection ครบครัน

---

## 📋 **รายการโปรเจคทั้งหมด**

### 1. 🌸 **ระบบรดน้ำกล้วยไม้** (`orchid_watering_system/`)
- **ไฟล์คู่มือ:** `BLYNK_SETUP_GUIDE.md`
- **จำนวนโซน:** 6 โซน
- **Sensor หลัก:** DHT22, Light, pH, EC, Soil Moisture (8 ตัว), Water Level, Flow
- **Virtual Pins:** V0-V12 (Input), V20-V25, V30-V32 (Output)

### 2. 🍅 **ระบบรดน้ำมะเขือเทศ** (`tomato_watering/`)
- **ไฟล์คู่มือ:** `BLYNK_SETUP_GUIDE.md`
- **จำนวนโซน:** 4 โซน
- **Sensor หลัก:** DHT22, pH, EC, CO2, Air Quality, Soil Moisture (4 ตัว), Water Level, Flow
- **Virtual Pins:** V0-V11 (Input), V20-V23, V30-V32 (Output)

### 3. 🐱 **ระบบให้อาหารแมว** (`cat_feeding_system/`)
- **ไฟล์คู่มือ:** `BLYNK_SETUP_GUIDE.md`
- **ฟีเจอร์หลัก:** ให้อาหารอัตโนมัติ
- **Sensor หลัก:** DHT22, Light, pH, EC, CO2, Air Quality, Load Cell, Water Level, Flow
- **Virtual Pins:** V0-V9 (Input), V20-V24 (Output)

### 4. 🐦 **ระบบให้อาหารนก** (`BirdFeedingSystem/`)
- **ไฟล์คู่มือ:** `BLYNK_SETUP_GUIDE.md`
- **ฟีเจอร์หลัก:** ให้อาหารอัตโนมัติ
- **Sensor หลัก:** DHT22, pH, EC, CO2, Air Quality, Load Cell, Water Level, Flow
- **Virtual Pins:** V0-V8 (Input), V20-V24 (Output)

### 5. 🌿 **ระบบปลูกผักชี** (`cilantro_system/`)
- **ไฟล์คู่มือ:** `BLYNK_SETUP_GUIDE.md`
- **จำนวนโซน:** 5 โซน
- **Sensor หลัก:** DHT22, EC, Air Quality, Soil Moisture (5 ตัว), Water Level, Flow
- **Virtual Pins:** V0-V10 (Input), V20-V24, V30-V32 (Output)

### 6. 🍄 **ระบบปลูกเห็ด** (`standalone_projects_mushroom/`)
- **ไฟล์คู่มือ:** `BLYNK_SETUP_GUIDE.md`
- **ฟีเจอร์หลัก:** ระบบหมอก, ควบคุมความชื้น
- **Sensor หลัก:** DHT22, EC, Air Quality, Water Level, Flow
- **Virtual Pins:** V0-V5 (Input), V20-V24 (Output)

---

## 🚀 **ขั้นตอนการตั้งค่าทั่วไป**

### 1. **การติดตั้ง Blynk App**
```bash
# ดาวน์โหลดจาก App Store หรือ Google Play
# ค้นหา: "Blynk IoT"
```

### 2. **การสร้าง Device**
1. สร้างบัญชีใหม่ใน Blynk
2. สร้าง Device ใหม่
3. เลือก **ESP32 Dev Board**
4. เก็บ **Auth Token** ไว้ใช้ในโค้ด

### 3. **การตั้งค่า WiFi ในโค้ด**
```cpp
// ใส่ข้อมูล WiFi และ Blynk Token
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* blynkAuthToken = "YOUR_BLYNK_AUTH_TOKEN";
```

---

## 📊 **Virtual Pin Mapping มาตรฐาน**

### **Input Pins (Sensor Data)**
| Pin | ข้อมูล | ช่วงค่า | หน่วย |
|-----|--------|---------|-------|
| V0 | Soil Moisture Zone 1 | 0-100 | % |
| V1 | Soil Moisture Zone 2 | 0-100 | % |
| V2 | Soil Moisture Zone 3 | 0-100 | % |
| V3 | Soil Moisture Zone 4 | 0-100 | % |
| V4 | Soil Moisture Zone 5 | 0-100 | % |
| V5 | Temperature | 15-35 | °C |
| V6 | Humidity | 40-95 | % |
| V7 | Light Level | 0-100 | % |
| V8 | pH Level | 5.5-7.5 | pH |
| V9 | EC Level | 0.5-2.0 | mS/cm |
| V10 | CO2 Level | 400-2000 | ppm |
| V11 | Air Quality | 0-500 | AQI |
| V12 | Water Level | 0-100 | % |
| V13 | Flow Rate | 0-10 | L/min |
| V14 | Food Weight | 0-5000 | g |

### **Output Pins (Control)**
| Pin | ฟังก์ชัน | ค่า | คำอธิบาย |
|-----|---------|-----|----------|
| V20 | Water Zone 1 | 0/1 | ควบคุมรดน้ำโซน 1 |
| V21 | Water Zone 2 | 0/1 | ควบคุมรดน้ำโซน 2 |
| V22 | Water Zone 3 | 0/1 | ควบคุมรดน้ำโซน 3 |
| V23 | Water Zone 4 | 0/1 | ควบคุมรดน้ำโซน 4 |
| V24 | Water Zone 5 | 0/1 | ควบคุมรดน้ำโซน 5 |
| V25 | Water Zone 6 | 0/1 | ควบคุมรดน้ำโซน 6 |
| V30 | System Power | 0/1 | เปิด/ปิดระบบ |
| V31 | Auto Mode | 0/1 | โหมดอัตโนมัติ |
| V32 | Watering Frequency | 1-24 | ชั่วโมง |

---

## 🎛️ **Widget Types ที่ใช้**

### **Display Widgets**
- **Value Display** - แสดงค่าตัวเลขจาก Sensor
- **Gauge** - แสดงค่าในรูปแบบเข็มวัด
- **LCD** - แสดงข้อความหลายบรรทัด

### **Control Widgets**
- **Button** - ควบคุมการเปิด/ปิด
- **Slider** - ปรับค่าตัวแปร
- **Switch** - สวิตช์เปิด/ปิด
- **Timer** - ตั้งเวลาการทำงาน

### **Chart Widgets**
- **SuperChart** - แสดงกราฟข้อมูล
- **Chart** - กราฟพื้นฐาน

### **Notification Widgets**
- **Notification** - แจ้งเตือนผ่าน Push
- **Email** - ส่งอีเมลแจ้งเตือน

---

## 🔧 **การตั้งค่า Widget Properties**

### **Value Display Widget**
```yaml
Input/Output: Input
Pin: V0-V14 (ตาม Sensor)
Label: "ชื่อข้อมูล"
Min/Max: 0-100 (สำหรับเปอร์เซ็นต์)
Decimal Places: 1-2
```

### **Button Widget**
```yaml
Input/Output: Output
Pin: V20-V32 (ตามฟังก์ชัน)
Mode: Switch
Label: "ชื่อปุ่ม"
```

### **Slider Widget**
```yaml
Input/Output: Output
Pin: V32 (ความถี่การรดน้ำ)
Min/Max: 1-24
Label: "ความถี่ (ชั่วโมง)"
```

---

## 🔔 **การตั้งค่า Notifications**

### **การแจ้งเตือนเมื่อดินแห้ง**
```yaml
Trigger: V0-V5 < 30%
Message: "ดินโซน X แห้งแล้ว กรุณารดน้ำ"
```

### **การแจ้งเตือนเมื่อน้ำหมด**
```yaml
Trigger: V12 < 20%
Message: "น้ำในถังใกล้หมดแล้ว กรุณาเติมน้ำ"
```

### **การแจ้งเตือนเมื่อ Sensor Offline**
```yaml
Trigger: เมื่อ Sensor ไม่ส่งข้อมูล
Message: "Sensor X ทำงานผิดปกติ กรุณาตรวจสอบ"
```

---

## 📈 **การตั้งค่า Charts**

### **กราฟความชื้นดิน**
```yaml
Data Streams: V0, V1, V2, V3, V4, V5
Update Interval: 30 วินาที
Chart Type: Line Chart
```

### **กราฟอุณหภูมิและความชื้น**
```yaml
Data Streams: V5, V6
Update Interval: 30 วินาที
Chart Type: Line Chart
```

### **กราฟค่า pH และ EC**
```yaml
Data Streams: V8, V9
Update Interval: 30 วินาที
Chart Type: Line Chart
```

---

## 🧪 **การทดสอบระบบ**

### **1. ทดสอบการเชื่อมต่อ**
- เปิด Blynk App
- ตรวจสอบว่า Device เชื่อมต่อแล้ว
- ดูค่า Sensor ต่างๆ แสดงผล

### **2. ทดสอบการควบคุม**
- กดปุ่มควบคุมต่างๆ
- ตรวจสอบการทำงานของ Actuator
- ทดสอบการปรับค่า Slider

### **3. ทดสอบการแจ้งเตือน**
- จำลองสถานการณ์ต่างๆ
- ตรวจสอบการแจ้งเตือน
- ทดสอบการส่ง Push Notification

---

## ❌ **การแก้ไขปัญหา**

### **ไม่สามารถเชื่อมต่อได้**
```bash
# ตรวจสอบ:
1. WiFi credentials
2. Blynk Auth Token
3. การเชื่อมต่ออินเทอร์เน็ต
4. Firewall settings
```

### **ค่า Sensor ไม่แสดง**
```bash
# ตรวจสอบ:
1. การเชื่อมต่อ Sensor
2. Virtual Pin mapping
3. โค้ดใน Arduino IDE
4. Sensor calibration
```

### **การควบคุมไม่ทำงาน**
```bash
# ตรวจสอบ:
1. Virtual Pin สำหรับ Output
2. การเชื่อมต่อ Relay/Actuator
3. โค้ดการควบคุม
4. Power supply
```

---

## 🔒 **การตั้งค่าความปลอดภัย**

### **1. การเปลี่ยน Auth Token**
- เปลี่ยน Blynk Auth Token เป็นประจำ
- ใช้ Token ที่ซับซ้อน
- เก็บ Token ไว้ในที่ปลอดภัย

### **2. การตั้งค่า WiFi**
- ใช้ WiFi ที่มี WPA2/WPA3
- เปลี่ยนรหัสผ่าน WiFi เป็นประจำ
- ใช้ Guest Network สำหรับ IoT

### **3. การตั้งค่า App**
- เปิดใช้งาน Two-Factor Authentication
- ใช้รหัสผ่านที่แข็งแกร่ง
- ตรวจสอบการเข้าสู่ระบบเป็นประจำ

---

## 📱 **ฟีเจอร์พิเศษ**

### **Offline Detection**
- ระบบตรวจจับ Sensor ที่ไม่ทำงาน
- แสดงสถานะ "Offline" ใน App
- ข้ามการใช้งาน LCD เมื่อไม่เชื่อมต่อ

### **Graceful Degradation**
- ระบบทำงานต่อได้แม้ Sensor บางตัวเสีย
- ปรับการทำงานตาม Sensor ที่เหลือ
- แจ้งเตือนเมื่อประสิทธิภาพลดลง

### **Multi-Zone Control**
- ควบคุมแต่ละโซนแยกกัน
- ตั้งค่าต่างกันสำหรับแต่ละโซน
- แสดงสถานะแต่ละโซน

---

## 📞 **การติดต่อและสนับสนุน**

### **ช่องทางการติดต่อ**
- **Email:** support@rdtrc.com
- **Line:** @rdtrc_support
- **Facebook:** RDTRC IoT Systems
- **Website:** www.rdtrc.com

### **เอกสารเพิ่มเติม**
- **GitHub:** https://github.com/rdtrc/iot-systems
- **Documentation:** https://docs.rdtrc.com
- **Video Tutorials:** https://youtube.com/rdtrc

---

## 📝 **บันทึกการอัพเดท**

### **Version 3.0 (2024)**
- ✅ เพิ่มระบบ Sensor ครบครัน
- ✅ เพิ่ม Offline Detection
- ✅ เพิ่ม Graceful Degradation
- ✅ อัพเดทคู่มือ Blynk สำหรับทุกโปรเจค
- ✅ เพิ่มระบบแจ้งเตือนอัจฉริยะ

### **Version 2.0 (2023)**
- ✅ เพิ่มระบบรดน้ำอัตโนมัติ
- ✅ เพิ่มการควบคุมผ่าน Blynk
- ✅ เพิ่มระบบแจ้งเตือนพื้นฐาน

### **Version 1.0 (2022)**
- ✅ ระบบพื้นฐาน
- ✅ การควบคุมผ่าน Serial Monitor

---

*คู่มือนี้จะอัพเดทเมื่อมีการปรับปรุงระบบ*

**© 2024 RDTRC IoT Systems. All rights reserved.**


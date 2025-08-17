# 🐱 คู่มือการติดตั้งระบบให้อาหารแมวอัตโนมัติ
## Automatic Cat Feeding System Setup Guide

---

## 📋 สารบัญ (Table of Contents)

1. [รายการอุปกรณ์ที่ต้องใช้](#รายการอุปกรณ์ที่ต้องใช้)
2. [การเตรียมซอฟต์แวร์](#การเตรียมซอฟต์แวร์)
3. [การเชื่อมต่อฮาร์ดแวร์](#การเชื่อมต่อฮาร์ดแวร์)
4. [การติดตั้งโค้ด](#การติดตั้งโค้ด)
5. [การตั้งค่าเครือข่าย](#การตั้งค่าเครือข่าย)
6. [การตั้งค่า Blynk App](#การตั้งค่า-blynk-app)
7. [การปรับแต่งระบบ](#การปรับแต่งระบบ)
8. [การทดสอบระบบ](#การทดสอบระบบ)
9. [การแก้ปัญหา](#การแก้ปัญหา)
10. [การบำรุงรักษา](#การบำรุงรักษา)

---

## 🛠️ รายการอุปกรณ์ที่ต้องใช้

### อุปกรณ์หลัก (Required Components)
- ✅ **ESP32 Development Board** (1 ตัว)
- ✅ **Servo Motor SG90** (1 ตัว) - สำหรับควบคุมการให้อาหาร
- ✅ **Load Cell 1kg + HX711 Amplifier** (1 ชุด) - วัดน้ำหนักอาหาร
- ✅ **RTC DS3231** (1 ตัว) - นาฬิกาเรียลไทม์
- ✅ **PIR Motion Sensor HC-SR501** (1 ตัว) - ตรวจจับแมว
- ✅ **LCD 16x2 with I2C Backpack** (1 ตัว) - แสดงข้อมูล
- ✅ **Buzzer 5V** (1 ตัว) - เสียงแจ้งเตือน
- ✅ **RGB LED Common Anode** (1 ตัว) - ไฟแสดงสถานะ
- ✅ **Push Button** (2 ตัว) - ปุ่มควบคุม
- ✅ **Resistors 10kΩ** (2 ตัว) - สำหรับ pull-up button
- ✅ **Resistors 220Ω** (3 ตัว) - สำหรับ LED
- ✅ **Power Supply 5V 2A** (1 ตัว)
- ✅ **Breadboard หรือ PCB** (1 แผ่น)
- ✅ **Jumper Wires** (ชุด)

### อุปกรณ์เสริม (Optional Components)
- 🔹 **ESP32-CAM** - สำหรับถ่ายรูปแมว
- 🔹 **SD Card Module** - บันทึกข้อมูล
- 🔹 **Water Level Sensor** - ตรวจสอบน้ำ
- 🔹 **กล่องกันน้ำ** - ป้องกันอุปกรณ์
- 🔹 **ขาตั้งและแท่นวางอาหาร**

### เครื่องมือที่ต้องใช้ (Required Tools)
- 🔧 ไขควงชุด
- 🔧 คีมปอกสาย
- 🔧 มัลติมิเตอร์
- 🔧 เครื่องบัดกรี (ถ้าต้องการติดตั้งถาวร)
- 🔧 สายUSB สำหรับเชื่อมต่อ ESP32

---

## 💻 การเตรียมซอฟต์แวร์

### ขั้นตอนที่ 1: ติดตั้ง Arduino IDE

1. **ดาวน์โหลด Arduino IDE**
   ```
   ไปที่: https://www.arduino.cc/en/software
   เลือกเวอร์ชัน: 2.x.x (ล่าสุด)
   ระบบปฏิบัติการ: Windows/Mac/Linux
   ```

2. **ติดตั้งโปรแกรม**
   - รันไฟล์ installer ที่ดาวน์โหลด
   - ทำตามขั้นตอนการติดตั้ง
   - เปิดโปรแกรม Arduino IDE

### ขั้นตอนที่ 2: ติดตั้ง ESP32 Board Package

1. **เพิ่ม Board Manager URL**
   ```
   ไปที่: File → Preferences
   ใส่ URL: https://dl.espressif.com/dl/package_esp32_index.json
   ใน Additional Board Manager URLs
   กด OK
   ```

2. **ติดตั้ง ESP32 Package**
   ```
   ไปที่: Tools → Board → Boards Manager
   ค้นหา: "ESP32"
   เลือก: "ESP32 by Espressif Systems"
   กด Install
   รอให้ติดตั้งเสร็จ
   ```

### ขั้นตอนที่ 3: ติดตั้งไลบรารีที่จำเป็น

เปิด Library Manager: `Tools → Manage Libraries`

**ติดตั้งไลบรารีต่อไปนี้:**

1. **ArduinoJson** (v6.21.3+)
   ```
   ค้นหา: "ArduinoJson"
   ผู้พัฒนา: Benoit Blanchon
   กด Install
   ```

2. **ESP32Servo** (v0.13.0+)
   ```
   ค้นหา: "ESP32Servo"
   ผู้พัฒนา: Kevin Harrington
   กด Install
   ```

3. **HX711 Arduino Library** (v0.7.5+)
   ```
   ค้นหา: "HX711 Arduino Library"
   ผู้พัฒนา: Bogdan Necula
   กด Install
   ```

4. **RTClib** (v2.1.1+)
   ```
   ค้นหา: "RTClib"
   ผู้พัฒนา: Adafruit
   กด Install
   ```

5. **LiquidCrystal I2C** (v1.1.2+)
   ```
   ค้นหา: "LiquidCrystal I2C"
   ผู้พัฒนา: Frank de Brabander
   กด Install
   ```

6. **Blynk** (v1.3.2+)
   ```
   ค้นหา: "Blynk"
   ผู้พัฒนา: Volodymyr Shymanskyy
   กด Install
   ```

7. **Time** (v1.6.1+)
   ```
   ค้นหา: "Time"
   ผู้พัฒนา: Michael Margolis
   กด Install
   ```

---

## 🔌 การเชื่อมต่อฮาร์ดแวร์

### ขั้นตอนที่ 1: เตรียมอุปกรณ์

⚠️ **คำเตือน: ตัดไฟก่อนเชื่อมต่อทุกครั้ง**

### ขั้นตอนที่ 2: การเชื่อมต่อหลัก

#### ESP32 Pinout Configuration:
```
PIN 18  → Servo Signal (สีส้ม/เหลือง)
PIN 19  → HX711 DOUT
PIN 21  → HX711 SCK / I2C SDA
PIN 22  → PIR OUT / I2C SCL  
PIN 23  → Buzzer Positive
PIN 25  → RGB LED Red
PIN 26  → RGB LED Green
PIN 27  → RGB LED Blue
PIN 32  → Manual Feed Button
PIN 33  → Reset Button
3.3V    → เซ็นเซอร์ต่างๆ
5V      → Servo และ PIR
GND     → Ground ทั้งหมด
```

### ขั้นตอนที่ 3: การเชื่อมต่อแต่ละอุปกรณ์

#### 1. Servo Motor SG90
```
Servo → ESP32
Red (VCC)    → 5V
Brown (GND)  → GND
Orange (Signal) → PIN 18
```

#### 2. Load Cell + HX711
```
Load Cell → HX711:
Red Wire    → E+
Black Wire  → E-
White Wire  → A-
Green Wire  → A+

HX711 → ESP32:
VCC  → 3.3V
GND  → GND
DOUT → PIN 19
SCK  → PIN 21
```

#### 3. PIR Motion Sensor
```
PIR → ESP32
VCC → 5V
GND → GND
OUT → PIN 22
```

#### 4. LCD 16x2 with I2C
```
LCD I2C → ESP32
VCC → 3.3V
GND → GND
SDA → PIN 21
SCL → PIN 22
```

#### 5. RTC DS3231
```
RTC → ESP32
VCC → 3.3V
GND → GND
SDA → PIN 21
SCL → PIN 22
```

#### 6. RGB LED
```
RGB LED → ESP32 (ผ่าน Resistor 220Ω)
Common Anode → 3.3V
Red   → PIN 25
Green → PIN 26
Blue  → PIN 27
```

#### 7. Buzzer
```
Buzzer → ESP32
Positive → PIN 23
Negative → GND
```

#### 8. Push Buttons
```
Manual Feed Button:
One Side → PIN 32
Other Side → GND
Pull-up Resistor 10kΩ: PIN 32 → 3.3V

Reset Button:
One Side → PIN 33
Other Side → GND
Pull-up Resistor 10kΩ: PIN 33 → 3.3V
```

### ขั้นตอนที่ 4: ตรวจสอบการเชื่อมต่อ

1. **ตรวจสอบ Power Supply**
   - วัดแรงดันที่ ESP32: ควรได้ 3.3V และ 5V
   - ตรวจสอบ GND ทั่วระบบ

2. **ตรวจสอบ I2C Devices**
   ```cpp
   // ใช้โค้ดนี้สแกน I2C Address
   #include <Wire.h>
   
   void setup() {
     Wire.begin(21, 22); // SDA, SCL
     Serial.begin(115200);
     
     for(byte address = 1; address < 127; address++) {
       Wire.beginTransmission(address);
       if(Wire.endTransmission() == 0) {
         Serial.print("Found I2C device at: 0x");
         Serial.println(address, HEX);
       }
     }
   }
   ```

3. **ตรวจสอบ Sensor Readings**
   - Load Cell: ควรอ่านค่าได้เมื่อมีน้ำหนัก
   - PIR: ควรเปลี่ยนสถานะเมื่อมีการเคลื่อนไหว
   - RTC: ควรแสดงเวลาที่ถูกต้อง

---

## 📱 การตั้งค่า Blynk App

### ขั้นตอนที่ 1: สร้างบัญชี Blynk

1. **ดาวน์โหลดแอป**
   ```
   iOS: App Store → ค้นหา "Blynk IoT"
   Android: Google Play → ค้นหา "Blynk IoT"
   ```

2. **สมัครสมาชิก**
   - เปิดแอป Blynk
   - กด "Create Account"
   - ใส่อีเมลและรหัสผ่าน
   - ยืนยันอีเมล

### ขั้นตอนที่ 2: สร้าง Template

1. **สร้าง Template ใหม่**
   ```
   ไปที่: https://blynk.cloud/
   เข้าสู่ระบบ
   กด "New Template"
   
   Template Settings:
   Name: Cat Feeder System
   Hardware: ESP32
   Connection Type: WiFi
   ```

2. **ตั้งค่า Datastreams**
   ```
   Virtual Pin V0: Weight (Double, 0-5000, g)
   Virtual Pin V1: Feed Count (Integer, 0-9999)
   Virtual Pin V2: Total Food (Double, 0-99999, g)
   Virtual Pin V3: Cat Detection (Integer, 0-1)
   Virtual Pin V4: System Status (Integer, 0-1)
   Virtual Pin V10: Manual Feed (Integer, 0-1)
   Virtual Pin V11: System Enable (Integer, 0-1)
   Virtual Pin V12: Feed Amount (Integer, 5-100, g)
   ```

### ขั้นตอนที่ 3: สร้าง Device

1. **เพิ่ม Device ใหม่**
   ```
   กด "New Device"
   เลือก Template: "Cat Feeder System"
   ตั้งชื่อ: "My Cat Feeder"
   กด "Create"
   ```

2. **รับ Device Credentials**
   ```
   คลิกที่ Device ที่สร้าง
   ไปที่แท็บ "Device Info"
   คัดลอก:
   - Template ID
   - Template Name  
   - Auth Token
   ```

### ขั้นตอนที่ 4: ออกแบบ Dashboard

1. **เพิ่ม Widgets**
   ```
   Weight Display:
   - Widget: Gauge
   - Datastream: V0 (Weight)
   - Min: 0, Max: 2000
   - Label: "Food Weight (g)"
   
   Feed Counter:
   - Widget: Value Display
   - Datastream: V1 (Feed Count)
   - Label: "Feed Count"
   
   Cat Detection:
   - Widget: LED
   - Datastream: V3 (Cat Detection)
   - Color: Blue
   - Label: "Cat Detected"
   
   Manual Feed Button:
   - Widget: Button
   - Datastream: V10 (Manual Feed)
   - Mode: Push
   - Label: "Feed Now"
   
   System Control:
   - Widget: Switch
   - Datastream: V11 (System Enable)
   - Label: "System ON/OFF"
   
   Feed Amount:
   - Widget: Slider
   - Datastream: V12 (Feed Amount)
   - Min: 5, Max: 100
   - Label: "Feed Amount (g)"
   ```

---

## 💾 การติดตั้งโค้ด

### ขั้นตอนที่ 1: เตรียมโค้ด

1. **ดาวน์โหลดโค้ด**
   - คัดลอกโค้ดจากไฟล์ `cat_feeding_system.ino`
   - เปิด Arduino IDE
   - สร้างไฟล์ใหม่: `File → New`
   - วางโค้ดลงไป

### ขั้นตอนที่ 2: แก้ไขการตั้งค่า

**แก้ไขข้อมูลการเชื่อมต่อ:**

```cpp
// การตั้งค่า WiFi
const char* ssid = "ชื่อ_WiFi_ของคุณ";
const char* password = "รหัสผ่าน_WiFi_ของคุณ";

// การตั้งค่า Blynk
const char* blynk_token = "Auth_Token_จาก_Blynk";
```

**แก้ไขการตั้งค่าเซ็นเซอร์:**

```cpp
// ปรับค่า Calibration สำหรับ Load Cell
float calibrationFactor = -7050.0; // ปรับตามเซ็นเซอร์ของคุณ

// ตั้งค่าน้ำหนักอาหารขั้นต่ำ
float minimumFoodWeight = 100.0; // กรัม

// I2C Address สำหรับ LCD (ลองใช้ 0x27 หรือ 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);
```

### ขั้นตอนที่ 3: อัพโหลดโค้ด

1. **ตั้งค่า Board**
   ```
   Tools → Board → ESP32 Dev Module
   Tools → Port → เลือกพอร์ตที่เชื่อมต่อ ESP32
   Tools → Upload Speed → 921600
   ```

2. **คอมไพล์และอัพโหลด**
   ```
   กด Verify (✓) เพื่อตรวจสอบโค้ด
   กด Upload (→) เพื่ออัพโหลดไปยัง ESP32
   รอจนกว่าจะเสร็จสิ้น
   ```

3. **ตรวจสอบ Serial Monitor**
   ```
   เปิด Serial Monitor: Tools → Serial Monitor
   ตั้ง Baud Rate: 115200
   ดูข้อความที่แสดง:
   - "เริ่มต้นระบบให้อาหารแมว..."
   - "เชื่อมต่อ WiFi สำเร็จ!"
   - "IP Address: xxx.xxx.xxx.xxx"
   - "ระบบพร้อมใช้งาน!"
   ```

---

## 🌐 การตั้งค่าเครือข่าย

### ขั้นตอนที่ 1: ตรวจสอบการเชื่อมต่อ WiFi

1. **ดู Serial Monitor**
   ```
   หากเชื่อมต่อสำเร็จจะแสดง:
   "WiFi Connected"
   "IP Address: 192.168.x.x"
   ```

2. **ทดสอบ Web Server**
   ```
   เปิดเบราว์เซอร์
   ใส่ IP Address ที่แสดงใน Serial Monitor
   เช่น: http://192.168.1.100
   ควรเห็นหน้าเว็บระบบให้อาหารแมว
   ```

### ขั้นตอนที่ 2: ตั้งค่า Port Forwarding (ถ้าต้องการเข้าถึงจากภายนอก)

1. **เข้าสู่ Router Admin**
   ```
   เปิดเบราว์เซอร์
   ไปที่: 192.168.1.1 (หรือ IP ของ Router)
   ใส่ username/password ของ Router
   ```

2. **ตั้งค่า Port Forwarding**
   ```
   หา Port Forwarding หรือ Virtual Server
   เพิ่มกฎใหม่:
   Service Name: Cat Feeder
   External Port: 8080
   Internal IP: IP ของ ESP32
   Internal Port: 80
   Protocol: TCP
   Enable: Yes
   ```

---

## ⚙️ การปรับแต่งระบบ

### ขั้นตอนที่ 1: ปรับเทียบเซ็นเซอร์น้ำหนัก

1. **ปรับเทียบ Zero Point**
   ```
   วางชามอาหารเปล่าบนเซ็นเซอร์
   กดปุ่ม Reset หรือเรียกฟังก์ชัน scale.tare()
   ผ่าน Web Interface: /api/calibrate
   ```

2. **ปรับเทียบ Scale Factor**
   ```cpp
   // ทดสอบด้วยน้ำหนักที่ทราบค่าแน่นอน
   // วางน้ำหนัก 100g
   // อ่านค่าที่ได้จาก Serial Monitor
   // ปรับค่า calibrationFactor ให้ได้ค่าที่ถูกต้อง
   
   float calibrationFactor = -7050.0; // ปรับค่านี้
   ```

### ขั้นตอนที่ 2: ตั้งค่าตารางเวลาให้อาหาร

**ผ่าน Web Interface:**
```
เปิดเบราว์เซอร์ไปที่ IP ของ ESP32
ในส่วน "ตารางให้อาหาร"
ตั้งค่าเวลาที่ต้องการ:
- เช้า: 07:00 น. (50g)
- เที่ยง: 12:00 น. (30g)  
- เย็น: 18:00 น. (50g)
```

**ผ่านโค้ด:**
```cpp
FeedingSchedule schedules[6] = {
  {7, 0, 50.0, true},   // เช้า 07:00 น. 50g
  {12, 0, 30.0, true},  // เที่ยง 12:00 น. 30g
  {18, 0, 50.0, true},  // เย็น 18:00 น. 50g
  {0, 0, 0.0, false},   // ปิดใช้งาน
  {0, 0, 0.0, false},   // ปิดใช้งาน
  {0, 0, 0.0, false}    // ปิดใช้งาน
};
```

### ขั้นตอนที่ 3: ปรับแต่ง PIR Sensor

1. **ปรับความไว (Sensitivity)**
   ```
   หมุนตัวปรับ Sensitivity บน PIR
   ตามเข็มนาฬิกา = เพิ่มความไว
   ทวนเข็มนาฬิกา = ลดความไว
   ```

2. **ปรับระยะเวลา (Time Delay)**
   ```
   หมุนตัวปรับ Time Delay บน PIR
   ตามเข็มนาฬิกา = เพิ่มเวลา (3วินาที - 5นาที)
   ทวนเข็มนาฬิกา = ลดเวลา
   ```

### ขั้นตอนที่ 4: ปรับแต่ง Servo Motor

1. **ทดสอบการเคลื่อนไหว**
   ```cpp
   // ทดสอบ Servo แยกต่างหาก
   #include <ESP32Servo.h>
   Servo testServo;
   
   void setup() {
     testServo.attach(18);
     testServo.write(0);   // ปิด
     delay(1000);
     testServo.write(90);  // เปิด
     delay(1000);
   }
   ```

2. **ปรับมุมการทำงาน**
   ```cpp
   // ใน dispenseFeed() function
   feedingServo.write(90); // ปรับมุมเปิด (0-180)
   delay(servoTime);
   feedingServo.write(0);  // ปรับมุมปิด (0-180)
   ```

---

## 🧪 การทดสอบระบบ

### ขั้นตอนที่ 1: ทดสอบแต่ละส่วน

1. **ทดสอบ WiFi Connection**
   ```
   ✅ ESP32 เชื่อมต่อ WiFi ได้
   ✅ แสดง IP Address ใน Serial Monitor
   ✅ เข้าถึง Web Interface ได้
   ```

2. **ทดสอบ Blynk Connection**
   ```
   ✅ แอป Blynk แสดงสถานะ "Online"
   ✅ ข้อมูลใน Widget อัพเดทได้
   ✅ ควบคุมผ่านแอปได้
   ```

3. **ทดสอบ Sensors**
   ```
   ✅ Load Cell อ่านน้ำหนักถูกต้อง
   ✅ PIR ตรวจจับการเคลื่อนไหวได้
   ✅ RTC แสดงเวลาถูกต้อง
   ✅ LCD แสดงข้อมูลได้
   ```

4. **ทดสอบ Actuators**
   ```
   ✅ Servo เคลื่อนไหวได้
   ✅ Buzzer ส่งเสียงได้
   ✅ RGB LED เปลี่ยนสีได้
   ```

### ขั้นตอนที่ 2: ทดสอบการทำงานรวม

1. **ทดสอบการให้อาหารด้วยตนเอง**
   ```
   กดปุ่ม Manual Feed บน ESP32
   หรือกดปุ่มในแอป Blynk
   หรือใช้ Web Interface
   
   ตรวจสอบ:
   ✅ Servo หมุนเปิดช่อง
   ✅ อาหารออกมา
   ✅ Servo หมุนปิดช่อง
   ✅ นับจำนวนครั้งเพิ่มขึ้น
   ✅ มีเสียงแจ้งเตือน
   ✅ LED เปลี่ยนสี
   ```

2. **ทดสอบการให้อาหารตามเวลา**
   ```
   ตั้งเวลาทดสอบใกล้เคียง (เช่น 1-2 นาทีข้างหน้า)
   รอจนถึงเวลาที่กำหนด
   
   ตรวจสอบ:
   ✅ ระบบให้อาหารอัตโนมัติ
   ✅ แจ้งเตือนผ่าน Blynk
   ✅ บันทึกเวลาล่าสุด
   ✅ คำนวณเวลาถัดไป
   ```

3. **ทดสอบการตรวจจับแมว**
   ```
   เคลื่อนไหวหน้า PIR Sensor
   
   ตรวจสอบ:
   ✅ LED เปลี่ยนเป็นสีน้ำเงิน
   ✅ แจ้งเตือน "ตรวจพบแมว" ใน Blynk
   ✅ แสดงสถานะ "CAT" บน LCD
   ✅ มีเสียงแจ้งเตือนสั้น
   ```

### ขั้นตอนที่ 3: ทดสอบระยะยาว

1. **ทดสอบ 24 ชั่วโมง**
   ```
   เฝ้าสังเกตการทำงาน 1 วันเต็ม
   ตรวจสอบ:
   ✅ ให้อาหารตามเวลาที่กำหนด
   ✅ ไม่มีการให้อาหารซ้ำ
   ✅ ระบบเสียบไฟไว้ได้
   ✅ WiFi ไม่หลุด
   ✅ Blynk เชื่อมต่อต่อเนื่อง
   ```

2. **ทดสอบความแม่นยำ**
   ```
   วัดน้ำหนักอาหารที่ให้จริง
   เปรียบเทียบกับที่ตั้งค่าไว้
   ความผิดพลาด: ควรไม่เกิน ±5g
   ```

---

## 🔧 การแก้ปัญหา

### ปัญหา WiFi

**ปัญหา: เชื่อมต่อ WiFi ไม่ได้**
```
อาการ: แสดง "WiFi Failed" บน LCD
การแก้ไข:
1. ตรวจสอบ SSID และ Password
2. ตรวจสอบสัญญาณ WiFi
3. ลอง Reset ESP32
4. ตรวจสอบ Router ไม่บล็อก MAC Address
```

**ปัญหา: WiFi หลุดบ่อย**
```
อาการ: เชื่อมต่อได้แล้วหลุด
การแก้ไข:
1. ตรวจสอบแหล่งจ่ายไฟ
2. ย้ายให้ใกล้ Router มากขึ้น
3. เพิ่มโค้ด WiFi reconnect
4. ใช้ WiFi 2.4GHz (ไม่ใช่ 5GHz)
```

### ปัญหา Blynk

**ปัญหา: Blynk แสดง Offline**
```
อาการ: แอปแสดงสถานะ "Offline"
การแก้ไข:
1. ตรวจสอบ Auth Token
2. ตรวจสอบการเชื่อมต่ออินเทอร์เน็ต
3. ตรวจสอบ Template และ Device ID
4. ลอง Blynk Server อื่น
```

### ปัญหา Hardware

**ปัญหา: Servo ไม่หมุน**
```
อาการ: Servo ไม่เคลื่อนไหว
การแก้ไข:
1. ตรวจสอบแหล่งจ่าย 5V
2. ตรวจสอบสาย Signal (PIN 18)
3. ทดสอบ Servo แยกต่างหาก
4. เปลี่ยน Servo ใหม่
```

**ปัญหา: เซ็นเซอร์น้ำหนักไม่ทำงาน**
```
อาการ: อ่านค่า 0 หรือค่าผิดปกติ
การแก้ไข:
1. ตรวจสอบการเชื่อมต่อ HX711
2. ตรวจสอบสาย Load Cell
3. ปรับเทียบ calibrationFactor
4. ตรวจสอบแหล่งจ่าย 3.3V
```

**ปัญหา: PIR ไม่ตรวจจับ**
```
อาการ: ไม่แสดงสถานะ "ตรวจพบแมว"
การแก้ไข:
1. ตรวจสอบแหล่งจ่าย 5V
2. ปรับความไว PIR
3. ตรวจสอบตำแหน่งติดตั้ง
4. ทดสอบด้วยการเคลื่อนไหวช้าๆ
```

**ปัญหา: LCD ไม่แสดงผล**
```
อาการ: หน้าจอมืด หรือแสดงสัญลักษณ์แปลกๆ
การแก้ไข:
1. ตรวจสอบ I2C Address (0x27 หรือ 0x3F)
2. ตรวจสอบสาย SDA/SCL
3. ปรับ Contrast
4. ตรวจสอบแหล่งจ่าย 3.3V
```

### ปัญหา Software

**ปัญหา: โค้ดคอมไพล์ไม่ผ่าน**
```
อาการ: Error ขณะคอมไพล์
การแก้ไข:
1. ตรวจสอบไลบรารีที่ติดตั้ง
2. อัพเดทไลบรารีเป็นเวอร์ชันล่าสุด
3. ตรวจสอบ ESP32 Board Package
4. ลบไลบรารีเก่าแล้วติดตั้งใหม่
```

**ปัญหา: อัพโหลดไม่สำเร็จ**
```
อาการ: Upload Failed
การแก้ไข:
1. กดปุ่ม BOOT บน ESP32 ขณะอัพโหลด
2. เลือก Port ที่ถูกต้อง
3. ลดความเร็วอัพโหลด
4. ตรวจสอบสาย USB
```

---

## 🔄 การบำรุงรักษา

### การบำรุงรักษารายวัน

1. **ตรวจสอบอาหาร**
   ```
   ✅ เติมอาหารเมื่อเหลือน้อย (< 100g)
   ✅ ตรวจสอบคุณภาพอาหาร
   ✅ ทำความสะอาดชามอาหาร
   ```

2. **ตรวจสอบระบบ**
   ```
   ✅ ดูสถานะบน LCD
   ✅ ตรวจสอบการเชื่อมต่อ WiFi
   ✅ ตรวจสอบ Blynk App
   ```

### การบำรุงรักษารายสัปดาห์

1. **ทำความสะอาด**
   ```
   ✅ ทำความสะอาดชามอาหารและน้ำ
   ✅ ทำความสะอาดกลไกการให้อาหาร
   ✅ เช็ดฝุ่นออกจากเซ็นเซอร์
   ```

2. **ตรวจสอบการทำงาน**
   ```
   ✅ ทดสอบการให้อาหารด้วยตนเอง
   ✅ ตรวจสอบความแม่นยำน้ำหนัก
   ✅ ทดสอบ PIR Sensor
   ```

### การบำรุงรักษารายเดือน

1. **ปรับเทียบเซ็นเซอร์**
   ```
   ✅ ปรับเทียบเซ็นเซอร์น้ำหนัก
   ✅ ตรวจสอบเวลา RTC
   ✅ ปรับแต่ง PIR Sensitivity
   ```

2. **อัพเดทระบบ**
   ```
   ✅ ตรวจสอบอัพเดทไลบรารี
   ✅ สำรองข้อมูลการตั้งค่า
   ✅ ตรวจสอบ Log Files
   ```

### การบำรุงรักษารายปี

1. **เปลี่ยนอุปกรณ์**
   ```
   ✅ เปลี่ยนแบตเตอรี่ RTC
   ✅ ตรวจสอบสภาพ Servo Motor
   ✅ ตรวจสอบสายไฟและการเชื่อมต่อ
   ```

2. **อัพเกรดระบบ**
   ```
   ✅ อัพเกรด Firmware
   ✅ เพิ่มฟีเจอร์ใหม่
   ✅ ปรับปรุงประสิทธิภาพ
   ```

---

## 📞 การติดต่อและสนับสนุน

### ข้อมูลเวอร์ชัน
```
System Version: 2.0
Created: 2024
Compatible: ESP32, Arduino IDE 2.x
License: Open Source
```

### แหล่งข้อมูลเพิ่มเติม
- **Arduino Forum**: https://forum.arduino.cc/
- **ESP32 Documentation**: https://docs.espressif.com/
- **Blynk Documentation**: https://docs.blynk.io/
- **GitHub Issues**: สำหรับรายงานปัญหา

### คำแนะนำความปลอดภัย

⚠️ **ข้อควรระวัง:**
- ใช้อุปกรณ์ที่ปลอดภัยสำหรับสัตว์เลี้ยง
- ตรวจสอบกลไกให้อาหารไม่ให้ติดขัด
- ใส่ระบบป้องกันการให้อาหารมากเกินไป
- มีแผนสำรองเมื่อระบบขัดข้อง

🎯 **เป้าหมาย:**
สร้างระบบให้อาหารแมวที่ปลอดภัย เชื่อถือได้ และใช้งานง่าย
เพื่อความสุขของแมวและความสบายใจของเจ้าของ

---

## 📝 บันทึกการติดตั้ง

**วันที่ติดตั้ง:** ___________
**ผู้ติดตั้ง:** ___________
**เวอร์ชันโค้ด:** ___________

### Checklist การติดตั้ง
- [ ] ติดตั้งฮาร์ดแวร์เสร็จสิ้น
- [ ] อัพโหลดโค้ดสำเร็จ
- [ ] เชื่อมต่อ WiFi ได้
- [ ] Blynk App ทำงานได้
- [ ] ทดสอบการให้อาหารแล้ว
- [ ] ตั้งค่าตารางเวลาแล้ว
- [ ] ทดสอบเซ็นเซอร์ทั้งหมดแล้ว

### หมายเหตุ
```
IP Address: ___________
Blynk Auth Token: ___________
WiFi SSID: ___________
ปัญหาที่พบ: ___________
การแก้ไข: ___________
```

---

**🐱 ขอให้แมวของคุณมีความสุขกับระบบให้อาหารใหม่! 🐱**
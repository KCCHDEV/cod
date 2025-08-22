# 🍄 ระบบรดน้ำเห็ดนางฟ้าอัตโนมัติ

ระบบรดน้ำเห็ดนางฟ้าที่ใช้ ESP32 พร้อมเซ็นเซอร์วัดอุณหภูมิ ความชื้น และความชื้นดิน พร้อมระบบตั้งเวลารดน้ำอัตโนมัติ

## 📋 คุณสมบัติ

- 🌡️ **วัดอุณหภูมิและความชื้น** ด้วย DHT11
- 🌱 **วัดความชื้นดิน** ด้วย Soil Moisture Sensor
- ⏰ **ตั้งเวลารดน้ำ** ด้วย RTC DS3231
- 💧 **รดน้ำอัตโนมัติ** ตามความชื้นดิน
- 📱 **ควบคุมผ่าน Blynk** แบบ real-time
- 🌐 **ตั้งค่า WiFi** ผ่านเว็บเบราว์เซอร์
- 📺 **แสดงผล LCD** 16x2 I2C
- 🔄 **Auto/Manual Mode** สลับได้

## 🔧 อุปกรณ์ที่ใช้

### Hardware
- **ESP32 Development Board**
- **DHT11 Sensor** (วัดอุณหภูมิและความชื้น)
- **Soil Moisture Sensor** (วัดความชื้นดิน)
- **Water Pump** (ปั๊มน้ำ 5V)
- **Relay Module** (ควบคุมปั๊มน้ำ)
- **RTC DS3231** (นาฬิกาจับเวลา)
- **LCD 16x2 I2C** (แสดงผล)
- **Breadboard และสายไฟ**

### Libraries
```
WiFi.h
BlynkSimpleEsp32.h
DHT.h
WebServer.h
Wire.h
LiquidCrystal_I2C.h
RTClib.h
```

## 🔌 การเชื่อมต่อ

### Pin Connections

| Component | ESP32 Pin | Description |
|-----------|-----------|-------------|
| DHT11 Data | GPIO 4 | วัดอุณหภูมิและความชื้น |
| Soil Sensor | GPIO 34 | วัดความชื้นดิน |
| Pump Relay | GPIO 26 | ควบคุมปั๊มน้ำ |
| LCD SDA | GPIO 21 | I2C Data |
| LCD SCL | GPIO 22 | I2C Clock |
| RTC SDA | GPIO 21 | I2C Data (ร่วมกับ LCD) |
| RTC SCL | GPIO 22 | I2C Clock (ร่วมกับ LCD) |

### Power Connections

| Component | Power | Ground |
|-----------|-------|--------|
| ESP32 | 5V/USB | GND |
| DHT11 | 3.3V | GND |
| Soil Sensor | 3.3V | GND |
| Relay Module | 5V | GND |
| LCD I2C | 5V | GND |
| RTC DS3231 | 3.3V | GND |
| Water Pump | 5V (ผ่าน Relay) | GND |

## ⚙️ การตั้งค่า

### 1. ติดตั้ง Libraries
เปิด Arduino IDE และติดตั้ง libraries ต่อไปนี้:
- **BlynkSimpleEsp32** (ผ่าน Library Manager)
- **DHT sensor library** (ผ่าน Library Manager)
- **LiquidCrystal_I2C** (ผ่าน Library Manager)
- **RTClib** (ผ่าน Library Manager)

### 2. ตั้งค่า Blynk
1. สร้างบัญชี Blynk
2. สร้าง Device ใหม่
3. เลือก ESP32 Dev Board
4. คัดลอก Auth Token
5. แก้ไขโค้ด:
```cpp
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"
```

### 3. อัพโหลดโค้ด
1. เชื่อมต่อ ESP32 กับคอมพิวเตอร์
2. เลือก Board: ESP32 Dev Module
3. เลือก Port ที่ถูกต้อง
4. อัพโหลดโค้ด

## 🚀 วิธีการใช้งาน

### การเริ่มต้นระบบ

1. **เปิดไฟ ESP32** - ระบบจะเริ่มต้นและปล่อย WiFi AP
2. **เชื่อมต่อ WiFi** - เชื่อมต่อกับ WiFi ชื่อ `Mushroom_Setup` พาสเวิร์ด `12345678`
3. **เปิดเว็บเบราว์เซอร์** - ไปที่ `192.168.4.1`
4. **ตั้งค่า WiFi** - ใส่ WiFi SSID และ Password ของคุณ
5. **กด Connect** - ระบบจะเชื่อมต่อและเริ่มทำงาน

### การควบคุมผ่าน Blynk

#### Virtual Pins
- **V0**: อุณหภูมิ (แสดงค่า)
- **V1**: ความชื้นอากาศ (แสดงค่า)
- **V2**: ความชื้นดิน (แสดงค่า)
- **V3**: ควบคุมปั๊ม (Button)
- **V4**: Auto/Manual Mode (Switch)
- **V5**: ตั้งค่าความชื้นดิน (Slider)

#### การตั้งค่า Blynk App
1. **Temperature Display** (V0): แสดงอุณหภูมิ
2. **Humidity Display** (V1): แสดงความชื้นอากาศ
3. **Soil Moisture Display** (V2): แสดงความชื้นดิน
4. **Pump Control** (V3): ปุ่มควบคุมปั๊ม
5. **Auto Mode** (V4): สวิตช์ Auto/Manual
6. **Soil Threshold** (V5): ตั้งค่าความชื้นดิน

### การตั้งเวลารดน้ำ

ระบบจะรดน้ำอัตโนมัติในกรณีต่อไปนี้:

1. **ความชื้นดินต่ำ** - เมื่อความชื้นดิน < 30% (ปรับได้)
2. **ตามเวลา** - ทุกวันเวลา 6:00 น. (ปรับได้)
3. **ป้องกันการรดน้ำซ้ำ** - รอ 5 นาทีระหว่างการรดน้ำ

### การปรับแต่งค่า

แก้ไขค่าในโค้ด:
```cpp
// ความชื้นดินขั้นต่ำ
int soilThreshold = 30;  // %

// ระยะเวลาปั๊มทำงาน
int pumpDuration = 5000; // 5 วินาที

// เวลารดน้ำ
int wateringHour = 6;    // 6:00 น.
int wateringMinute = 0;  // 0 นาที

// ระยะห่างระหว่างการรดน้ำ
const unsigned long wateringInterval = 300000; // 5 นาที
```

## 📊 การแสดงผล

### LCD Display

#### ตอน WiFi Setup Mode:
```
WiFi Setup Mode   (Line 1)
Connect to AP     (Line 2)
```

#### ตอนทำงานปกติ:
```
T:25.5C H:65%    (Line 1)
S:45% PUMP OFF   (Line 2)
```

### Serial Monitor
```
🍄 Mushroom Watering System Starting...
✅ LCD found at address: 0x27
✅ RTC DS3231 found
🕐 Current time: 6:0
✅ DHT11 sensor ready
📡 WiFi AP started: Mushroom_Setup
🌐 IP Address: 192.168.4.1
🌐 Web server started
📊 Sensors - Temp: 25.5°C, Humidity: 65%, Soil: 45%
💧 Auto watering - Soil too dry: 25%
🚰 Pump started
🚰 Pump stopped
```

## 🔧 การแก้ไขปัญหา

### LCD ไม่แสดงผล
1. ตรวจสอบการเชื่อมต่อ SDA/SCL
2. ตรวจสอบแรงดันไฟ 5V
3. ปรับ Contrast ของ LCD (ถ้ามี)

### DHT11 อ่านค่าไม่ได้
1. ตรวจสอบการเชื่อมต่อ Data pin
2. ตรวจสอบแรงดันไฟ 3.3V
3. เพิ่ม Pull-up resistor 4.7kΩ

### Soil Sensor ค่าไม่ถูกต้อง
1. ปรับค่า calibration ในโค้ด:
```cpp
soilMoisture = map(rawSoil, 4095, 0, 0, 100);
```
2. ทดสอบค่า raw ใน Serial Monitor
3. ปรับตามสภาพแวดล้อม

### RTC ไม่ทำงาน
1. ตรวจสอบการเชื่อมต่อ I2C
2. ตรวจสอบแบตเตอรี่ CR2032
3. ตั้งเวลาใหม่ในโค้ด:
```cpp
rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
```

### WiFi เชื่อมต่อไม่ได้
1. ตรวจสอบ SSID และ Password
2. ตรวจสอบสัญญาณ WiFi
3. รีเซ็ตระบบและลองใหม่

## 📱 Blynk App Setup

### การสร้าง Dashboard
1. **เปิด Blynk App**
2. **สร้าง Device ใหม่**
3. **เลือก ESP32 Dev Board**
4. **คัดลอก Auth Token**

### การเพิ่ม Widgets
1. **Temperature Display** (V0)
2. **Humidity Display** (V1)
3. **Soil Moisture Display** (V2)
4. **Button** (V3) - ควบคุมปั๊ม
5. **Switch** (V4) - Auto/Manual
6. **Slider** (V5) - ตั้งค่าความชื้นดิน

## 🔄 การอัพเดทระบบ

### การเพิ่มฟีเจอร์ใหม่
1. **เพิ่มเซ็นเซอร์ใหม่** - แก้ไขโค้ดในส่วน sensors
2. **เพิ่มการควบคุม** - เพิ่ม virtual pins
3. **ปรับปรุง UI** - แก้ไข HTML ใน handleRoot()

### การปรับแต่งการทำงาน
1. **เปลี่ยนเวลารดน้ำ** - แก้ไขค่า wateringHour/wateringMinute
2. **ปรับความไว** - แก้ไขค่า soilThreshold
3. **เปลี่ยนระยะเวลา** - แก้ไขค่า pumpDuration

## 📞 การติดต่อ

หากมีปัญหาหรือต้องการความช่วยเหลือ:
- **Email**: support@example.com
- **Line**: @mushroom_support
- **Facebook**: Mushroom Watering System

---

**🍄 ระบบรดน้ำเห็ดนางฟ้าอัตโนมัติ - ดูแลเห็ดของคุณให้เติบโตอย่างสมบูรณ์แบบ**

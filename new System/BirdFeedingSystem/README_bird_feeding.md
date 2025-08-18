# 🐦 Bird Feeding System

ระบบให้น้ำและอาหารนกอัตโนมัติด้วย ESP32 พร้อมระบบควบคุมผ่าน Blink App และ Web Interface

## ✨ Features

- **🕐 Real-time Clock**: ใช้ RTC DS3231 สำหรับเวลาที่แม่นยำ
- **💧 Water Pump Control**: ควบคุมปั๊มน้ำผ่าน Relay
- **🌾 Food Dispenser**: ควบคุมถาดอาหารผ่าน Servo Motor
- **📱 Blink App Control**: ควบคุมผ่าน Blink App ได้
- **🌐 Web Interface**: ควบคุมผ่านเว็บไซต์ได้
- **⏰ Schedule System**: ตั้งเวลาทำงานอัตโนมัติ
- **📊 Daily Limits**: จำกัดจำนวนครั้งต่อวัน
- **💡 Status LED**: แสดงสถานะการทำงาน
- **🔧 Manual Control**: ควบคุมด้วยตนเองได้

## 📋 Requirements

### Hardware
- ESP32 Development Board
- DS3231 RTC Module
- SG90 Servo Motor (สำหรับถาดอาหาร)
- 1-Channel Relay Module (สำหรับปั๊มน้ำ)
- Water Pump (5V/12V)
- Food Dispenser Mechanism
- LED Status Indicator
- Power Supply (5V/2A)
- Jumper Wires
- Breadboard (optional)

### Software Libraries
- `RTClib` by Adafruit
- `ArduinoJson` by Benoit Blanchon
- `ESP32Servo` by Kevin Harrington
- `WiFi` (built-in)
- `WebServer` (built-in)
- `Wire` (built-in)

## 🔌 Wiring Diagram

```
ESP32 Pin    Component    Description
--------     --------    -----------
3.3V    -->  RTC VCC     Power for RTC module
GND     -->  RTC GND     Ground for RTC module
GPIO21  -->  RTC SDA     I2C Data line
GPIO22  -->  RTC SCL     I2C Clock line

3.3V    -->  Servo VCC   Power for Servo
GND     -->  Servo GND   Ground for Servo
GPIO13  -->  Servo Signal Control signal for Servo

5V      -->  Relay VCC   Power for Relay module
GND     -->  Relay GND   Ground for Relay module
GPIO5   -->  Relay IN    Control signal for Relay

3.3V    -->  LED +       Power for Status LED
GPIO2   -->  LED -       Control for Status LED (ผ่าน resistor 220Ω)
```

## 📦 Installation

### 1. Install Libraries
เปิด Arduino IDE แล้วติดตั้ง libraries ต่อไปนี้:
```
Tools > Manage Libraries
- RTClib by Adafruit
- ArduinoJson by Benoit Blanchon
- ESP32Servo by Kevin Harrington
```

### 2. Configure WiFi
แก้ไขไฟล์ `bird_feeding_system.ino`:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### 3. Set RTC Time (Optional)
ถ้าต้องการตั้งเวลา RTC ให้ uncomment บรรทัดนี้:
```cpp
rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
```

### 4. Upload Code
- เลือก Board: ESP32 Dev Module
- เลือก Port ที่ถูกต้อง
- กด Upload

## 🌐 Web Interface

หลังจาก upload code แล้ว:
1. เปิด Serial Monitor (115200 baud)
2. ดู IP address ที่ได้จาก WiFi
3. เปิดเว็บเบราว์เซอร์ไปที่ IP address

### Features ใน Web Interface:
- **Real-time Status**: แสดงสถานะน้ำและอาหารแบบ real-time
- **Manual Control**: ควบคุมน้ำและอาหารด้วยตนเอง
- **Schedule Display**: แสดงตารางเวลาทั้งหมด
- **Progress Bars**: แสดงความคืบหน้าการทำงาน
- **Daily Statistics**: แสดงสถิติประจำวัน

## 📱 Blink App Control

ระบบรองรับการควบคุมผ่าน Blink App:

### API Endpoints สำหรับ Blink:
```
POST /api/blink/water    - ให้น้ำ 10 วินาที
POST /api/blink/food     - ให้อาหาร 1 ครั้ง
GET  /api/blink/status   - ดูสถานะระบบ
```

### การตั้งค่าใน Blink App:
1. สร้าง HTTP Request
2. ตั้งค่า URL: `http://YOUR_ESP32_IP/api/blink/water`
3. เลือก Method: POST
4. ตั้งค่า Trigger ตามต้องการ

## ⏰ Schedule Configuration

แก้ไขตารางเวลาในโค้ด:
```cpp
FeedingSchedule schedules[] = {
  // เช้า - ให้น้ำและอาหาร
  {6, 0, true, true, 15, true},    // 6:00 - น้ำ 15 วินาที + อาหาร
  {6, 30, false, true, 1, true},   // 6:30 - อาหารเพิ่ม
  
  // เที่ยง - ให้น้ำ
  {12, 0, true, false, 10, true},  // 12:00 - น้ำ 10 วินาที
  
  // เย็น - ให้น้ำและอาหาร
  {18, 0, true, true, 15, true},   // 18:00 - น้ำ 15 วินาที + อาหาร
  {18, 30, false, true, 1, true},  // 18:30 - อาหารเพิ่ม
  
  // กลางคืน - ให้น้ำเล็กน้อย
  {22, 0, true, false, 5, true}    // 22:00 - น้ำ 5 วินาที
};
```

## 🔧 API Endpoints

### GET /api/status
```json
{
  "currentTime": "14:30:25",
  "waterActive": false,
  "foodActive": false,
  "dailyWaterCount": 2,
  "dailyFoodCount": 3,
  "lastWaterTime": "12:00",
  "lastFoodTime": "18:30"
}
```

### POST /api/water
```json
{
  "duration": 10
}
```

### POST /api/food
```json
{
  "amount": 2
}
```

### GET /api/schedule
```json
{
  "schedules": [
    {
      "time": "6:00",
      "isWater": true,
      "isFood": true,
      "duration": 15,
      "enabled": true
    }
  ]
}
```

## 🚀 Usage Examples

### การให้น้ำอัตโนมัติ
```cpp
// ให้น้ำตอนเช้า 15 วินาที
{6, 0, true, false, 15, true},   // 6:00 - น้ำ 15 วินาที

// ให้น้ำตอนเที่ยง 10 วินาที
{12, 0, true, false, 10, true},  // 12:00 - น้ำ 10 วินาที
```

### การให้อาหารอัตโนมัติ
```cpp
// ให้อาหารตอนเช้า
{6, 0, false, true, 1, true},    // 6:00 - อาหาร 1 ครั้ง

// ให้อาหารเพิ่มตอนเช้า
{6, 30, false, true, 1, true},   // 6:30 - อาหารเพิ่ม 1 ครั้ง
```

### การให้น้ำและอาหารพร้อมกัน
```cpp
// ให้น้ำและอาหารพร้อมกัน
{18, 0, true, true, 15, true},   // 18:00 - น้ำ 15 วินาที + อาหาร
```

## 🔍 Troubleshooting

### Servo ไม่ทำงาน
- ตรวจสอบการเชื่อมต่อ GPIO13
- ตรวจสอบแรงดันไฟฟ้า 5V
- ตรวจสอบ library ESP32Servo

### Relay ไม่ทำงาน
- ตรวจสอบการเชื่อมต่อ GPIO5
- ตรวจสอบแรงดันไฟฟ้า 5V สำหรับ relay module
- ตรวจสอบ logic level (HIGH = OFF, LOW = ON)

### RTC ไม่ทำงาน
- ตรวจสอบการเชื่อมต่อ I2C (SDA, SCL)
- ตรวจสอบแรงดัน 3.3V
- ตรวจสอบ library RTClib

### WiFi ไม่เชื่อมต่อ
- ตรวจสอบ SSID และ Password
- ตรวจสอบสัญญาณ WiFi
- รอการเชื่อมต่อ (อาจใช้เวลาสักครู่)

## 📊 Daily Limits

ระบบมีการจำกัดจำนวนครั้งต่อวัน:
- **น้ำ**: สูงสุด 5 ครั้งต่อวัน
- **อาหาร**: สูงสุด 6 ครั้งต่อวัน

การนับจะรีเซ็ตทุกวันเวลาเที่ยงคืน

## 🔧 Customization

### เปลี่ยน Pin Configuration
```cpp
const int SERVO_PIN = 13;        // เปลี่ยน pin สำหรับ Servo
const int RELAY_PIN = 5;         // เปลี่ยน pin สำหรับ Relay
const int STATUS_LED = 2;        // เปลี่ยน pin สำหรับ LED
```

### เปลี่ยน Servo Angles
```cpp
const int SERVO_OPEN_ANGLE = 90;   // มุมเปิดถาดอาหาร
const int SERVO_CLOSE_ANGLE = 0;   // มุมปิดถาดอาหาร
```

### เปลี่ยน Timing
```cpp
const int WATER_PUMP_DURATION = 10; // วินาที
const unsigned long FOOD_DISPENSER_TIMEOUT = 5000; // 5 วินาที
```

## 📝 License

MIT License - ใช้งานได้อย่างอิสระ

## 🤝 Contributing

หากมีข้อเสนอแนะหรือพบปัญหา สามารถแจ้งได้เลยครับ

---

**สร้างโดย เนย - Senior Developer** 🌟 
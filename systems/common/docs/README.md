# 🌱 ESP32 RTC Relay Controller

ระบบควบคุม Relay แบบอัตโนมัติด้วย ESP32 และ RTC DS3231 สำหรับการรดน้ำอัตโนมัติ

## ✨ Features

- **Real-time Clock**: ใช้ RTC DS3231 สำหรับเวลาที่แม่นยำ
- **4 Channel Relay**: ควบคุมอุปกรณ์ได้ 4 ช่อง
- **Web Interface**: ควบคุมผ่านเว็บไซต์ได้
- **Schedule System**: ตั้งเวลาทำงานอัตโนมัติ
- **WiFi Control**: ควบคุมผ่าน WiFi ได้
- **Real-time Monitoring**: ดูสถานะแบบ Real-time

## 📋 Requirements

### Hardware
- ESP32 Development Board
- DS3231 RTC Module
- 4-Channel Relay Module
- Jumper Wires
- Breadboard (optional)
- Power Supply (5V/2A)

### Software Libraries
- `RTClib` by Adafruit
- `ArduinoJson` by Benoit Blanchon
- `WiFi` (built-in)
- `WebServer` (built-in)
- `Wire` (built-in)

## 🔌 Wiring Diagram

```
ESP32 Pin    RTC DS3231    Relay Module
--------     ----------    ------------
3.3V    -->  VCC      -->  VCC
GND     -->  GND      -->  GND
GPIO21  -->  SDA      -->  -
GPIO22  -->  SCL      -->  -
GPIO5   -->  -        -->  IN1 (Relay 1)
GPIO18  -->  -        -->  IN2 (Relay 2)
GPIO19  -->  -        -->  IN3 (Relay 3)
GPIO21  -->  -        -->  IN4 (Relay 4)
```

## 📦 Installation

### 1. Install Libraries
เปิด Arduino IDE แล้วติดตั้ง libraries ต่อไปนี้:
```
Tools > Manage Libraries
- RTClib by Adafruit
- ArduinoJson by Benoit Blanchon
```

### 2. Configure WiFi
แก้ไขไฟล์ `esp32_rtc_relay.ino`:
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
- **Real-time Clock**: แสดงเวลาปัจจุบัน
- **Relay Control**: เปิด/ปิด relay แต่ละช่อง
- **Schedule Display**: แสดงตารางเวลาทั้งหมด
- **Status Monitoring**: ดูสถานะ relay แบบ real-time

## ⏰ Schedule Configuration

แก้ไขตารางเวลาในโค้ด:
```cpp
Schedule schedules[] = {
  {8, 0, 0, true},   // เปิด Relay 1 เวลา 8:00
  {18, 0, 0, false}, // ปิด Relay 1 เวลา 18:00
  {9, 0, 1, true},   // เปิด Relay 2 เวลา 9:00
  {17, 0, 1, false}, // ปิด Relay 2 เวลา 17:00
  // เพิ่มตารางเวลาอื่นๆ ตามต้องการ
};
```

## 🔧 API Endpoints

### GET /api/time
```json
{
  "time": "14:30:25"
}
```

### GET /api/relays
```json
{
  "relays": [
    {"index": 0, "state": true},
    {"index": 1, "state": false},
    {"index": 2, "state": true},
    {"index": 3, "state": false}
  ]
}
```

### POST /api/relay
```json
{
  "index": 0,
  "state": true
}
```

### GET /api/schedule
```json
{
  "schedules": [
    {
      "time": "8:00",
      "relayIndex": 0,
      "state": true
    }
  ]
}
```

## 🚀 Usage Examples

### การรดน้ำอัตโนมัติ
```cpp
// รดน้ำตอนเช้า 30 นาที
{6, 0, 0, true},   // เปิดปั๊มน้ำ 6:00
{6, 30, 0, false}, // ปิดปั๊มน้ำ 6:30

// รดน้ำตอนเย็น 30 นาที
{18, 0, 0, true},  // เปิดปั๊มน้ำ 18:00
{18, 30, 0, false} // ปิดปั๊มน้ำ 18:30
```

### ระบบแสงสว่าง
```cpp
// เปิดไฟตอนค่ำ ปิดตอนเช้า
{18, 0, 1, true},  // เปิดไฟ 18:00
{6, 0, 1, false}   // ปิดไฟ 6:00
```

## 🔍 Troubleshooting

### RTC ไม่ทำงาน
- ตรวจสอบการเชื่อมต่อ I2C (SDA, SCL)
- ตรวจสอบแรงดัน 3.3V
- ตรวจสอบ library RTClib

### Relay ไม่ทำงาน
- ตรวจสอบการเชื่อมต่อ GPIO
- ตรวจสอบแรงดัน 5V สำหรับ relay module
- ตรวจสอบ logic level (HIGH = OFF, LOW = ON)

### WiFi ไม่เชื่อมต่อ
- ตรวจสอบ SSID และ Password
- ตรวจสอบสัญญาณ WiFi
- รอการเชื่อมต่อ (อาจใช้เวลาสักครู่)

## 📝 License

MIT License - ใช้งานได้อย่างอิสระ

## 🤝 Contributing

หากมีข้อเสนอแนะหรือพบปัญหา สามารถแจ้งได้เลยครับ

---

**สร้างโดย เนย - Senior Developer** 🌟 
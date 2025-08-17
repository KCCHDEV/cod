# 🌱 Smart Watering System with Moisture Sensors & Webhook Notifications

ระบบรดน้ำอัตโนมัติขั้นสูงด้วย ESP32 ที่รวมการวัดความชื้นดินและระบบแจ้งเตือนผ่าน Webhook

## ✨ Features หลัก

### 🌿 Smart Watering System
- **Real-time Clock**: ใช้ RTC DS3231 สำหรับเวลาที่แม่นยำ
- **4-Channel Relay**: ควบคุมปั๊มน้ำได้ 4 ช่อง
- **Soil Moisture Sensors**: วัดความชื้นดิน 4 จุด
- **Moisture-Based Watering**: รดน้ำตามความชื้นดินจริง
- **Schedule System**: ตั้งเวลาทำงานอัตโนมัติ
- **Daily Limits**: จำกัดการรดน้ำไม่เกิน 5 ครั้งต่อวัน

### 🔔 Webhook Notifications
- **Multi-Platform Support**: Discord, Line Notify, Slack, Telegram, IFTTT
- **Real-time Alerts**: แจ้งเตือนเมื่อความชื้นเปลี่ยนแปลง
- **Watering Notifications**: แจ้งเตือนเมื่อเริ่ม/หยุดรดน้ำ
- **System Status**: แจ้งเตือนสถานะระบบ
- **Daily Reports**: รายงานสรุปประจำวัน

### 🌐 Web Interface
- **Real-time Monitoring**: ดูสถานะแบบ Real-time
- **Moisture Visualization**: แสดงความชื้นด้วย Progress Bar
- **Manual Control**: ควบคุมการรดน้ำด้วยตนเอง
- **Statistics Dashboard**: แสดงสถิติการใช้งาน
- **Webhook Management**: เปิด/ปิดการแจ้งเตือน

## 📋 Hardware Requirements

### Essential Components
- **ESP32 Development Board**
- **DS3231 RTC Module**
- **4-Channel Relay Module**
- **4x Soil Moisture Sensors**
- **4x Water Pumps (5V-12V)**
- **Power Supply (5V/2A)**
- **Jumper Wires**
- **Breadboard (optional)**

### Optional Components
- **LCD Display (I2C)** - สำหรับแสดงผลแบบ Local
- **DHT22 Sensor** - วัดอุณหภูมิและความชื้นอากาศ
- **Light Sensor** - ตรวจจับแสงสว่าง
- **Rain Sensor** - ตรวจจับฝน
- **Buzzer** - แจ้งเตือนเสียง
- **LED Indicators** - แสดงสถานะ

## 🔌 Wiring Diagram

### Pin Connections
```
ESP32 Pin    Component
--------     ---------
3.3V    -->  RTC VCC, Moisture Sensors VCC
GND     -->  All components GND
GPIO21  -->  RTC SDA
GPIO22  -->  RTC SCL
GPIO5   -->  Relay IN1 (Water Pump 1)
GPIO18  -->  Relay IN2 (Water Pump 2)
GPIO19  -->  Relay IN3 (Water Pump 3)
GPIO21  -->  Relay IN4 (Water Pump 4)
GPIO34  -->  Moisture Sensor 1 (ADC)
GPIO35  -->  Moisture Sensor 2 (ADC)
GPIO36  -->  Moisture Sensor 3 (ADC)
GPIO39  -->  Moisture Sensor 4 (ADC)
```

### Power Distribution
```
5V Power Supply
├── Relay Module VCC
├── Water Pumps
└── GND to all components

3.3V from ESP32
├── RTC VCC
└── Moisture Sensors VCC
```

## 📦 Installation & Setup

### 1. Install Required Libraries
```bash
# Arduino IDE > Tools > Manage Libraries
- RTClib by Adafruit
- ArduinoJson by Benoit Blanchon
```

### 2. Configure WiFi Settings
แก้ไขในโค้ด:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### 3. Configure Webhook
เลือกแพลตฟอร์มที่ต้องการ:

#### Discord Webhook
```cpp
const char* webhookUrl = "https://discord.com/api/webhooks/YOUR_WEBHOOK_ID/YOUR_WEBHOOK_TOKEN";
```

#### Line Notify
```cpp
const char* webhookUrl = "https://notify-api.line.me/api/notify";
const char* webhookToken = "YOUR_LINE_NOTIFY_TOKEN";
```

#### Telegram Bot
```cpp
const char* webhookUrl = "https://api.telegram.org/botYOUR_BOT_TOKEN/sendMessage";
const char* chatId = "YOUR_CHAT_ID";
```

### 4. Calibrate Moisture Sensors
1. วัดค่าดินแห้ง (Air): ~4095
2. วัดค่าดินเปียก: ~0
3. ปรับ `MOISTURE_THRESHOLD` ในโค้ด

### 5. Upload Code
- เลือก Board: ESP32 Dev Module
- เลือก Port ที่ถูกต้อง
- กด Upload

## 🌐 Web Interface Features

### Dashboard
- **Real-time Clock**: แสดงเวลาปัจจุบัน
- **System Status**: สถานะการเชื่อมต่อ
- **WiFi Signal**: ความแรงสัญญาณ WiFi

### Plant Control
- **Moisture Display**: แสดงความชื้นเป็นเปอร์เซ็นต์
- **Status Indicators**: แสดงสถานะ (Very Wet, Wet, Normal, Dry, Very Dry)
- **Progress Bars**: แสดงความชื้นด้วย Visual Bar
- **Manual Control**: ปุ่มควบคุมการรดน้ำ
- **Watering Status**: แสดงสถานะการรดน้ำ

### Statistics
- **Daily Count**: จำนวนครั้งที่รดน้ำวันนี้
- **Last Watered**: เวลาที่รดน้ำล่าสุด
- **Moisture History**: ประวัติความชื้น

### Webhook Management
- **Toggle Webhook**: เปิด/ปิดการแจ้งเตือน
- **Test Notification**: ทดสอบการส่งข้อความ
- **Status Display**: แสดงสถานะ webhook

## ⏰ Schedule Configuration

### Default Schedule
```cpp
// Morning watering - 6:00 AM
{6, 0, 0, 15, true, true},   // Plant 1 - 15 minutes, moisture-based
{6, 5, 1, 20, true, true},   // Plant 2 - 20 minutes, moisture-based
{6, 10, 2, 10, true, true},  // Plant 3 - 10 minutes, moisture-based
{6, 15, 3, 25, true, true},  // Plant 4 - 25 minutes, moisture-based

// Evening watering - 6:00 PM
{18, 0, 0, 10, true, true},  // Plant 1 - 10 minutes, moisture-based
{18, 5, 1, 15, true, true},  // Plant 2 - 15 minutes, moisture-based
{18, 10, 2, 8, true, true},  // Plant 3 - 8 minutes, moisture-based
{18, 15, 3, 20, true, true}  // Plant 4 - 20 minutes, moisture-based
```

### Customize Schedule
แก้ไขในโค้ด:
```cpp
WateringSchedule wateringSchedules[] = {
  {hour, minute, relayIndex, duration, enabled, moistureBased},
  // เพิ่มตารางเวลาตามต้องการ
};
```

## 🔔 Notification Types

### System Notifications
- **System Started**: เมื่อระบบเริ่มทำงาน
- **Daily Reset**: เมื่อรีเซ็ตตัวนับรายวัน
- **WiFi Connected**: เมื่อเชื่อมต่อ WiFi สำเร็จ

### Moisture Alerts
- **Moisture Changed**: เมื่อความชื้นเปลี่ยนแปลง
- **Low Moisture**: เมื่อความชื้นต่ำ
- **High Moisture**: เมื่อความชื้นสูง

### Watering Notifications
- **Watering Started**: เมื่อเริ่มรดน้ำ
- **Watering Completed**: เมื่อหยุดรดน้ำ
- **Watering Skipped**: เมื่อข้ามการรดน้ำ (ดินชื้นพอ)

### Daily Reports
- **Daily Summary**: สรุปการรดน้ำประจำวัน
- **Statistics Report**: รายงานสถิติ

## 🔧 API Endpoints

### GET /api/time
```json
{
  "time": "14:30:25"
}
```

### GET /api/plants
```json
{
  "plants": [
    {
      "index": 0,
      "watering": true,
      "dailyCount": 2,
      "moisturePercent": 45,
      "moistureStatus": "Normal",
      "lastWatered": "6:15"
    }
  ]
}
```

### POST /api/water
```json
{
  "index": 0,
  "duration": 10
}
```

### GET /api/moisture
```json
{
  "sensors": [
    {
      "id": 0,
      "value": 2048,
      "percent": 50,
      "status": "Normal",
      "needsWater": false
    }
  ]
}
```

### POST /api/webhook/toggle
```json
{
  "enabled": true
}
```

## 🚀 Usage Examples

### การปลูกพืชในบ้าน
```cpp
// รดน้ำตอนเช้าและเย็น
{6, 0, 0, 15, true, true},   // 6:00 AM - 15 min
{18, 0, 0, 10, true, true},  // 6:00 PM - 10 min
```

### การปลูกพืชในสวน
```cpp
// รดน้ำหลายครั้งต่อวัน
{6, 0, 0, 20, true, true},   // 6:00 AM
{12, 0, 0, 15, true, true},  // 12:00 PM
{18, 0, 0, 20, true, true},  // 6:00 PM
```

### การปลูกพืชในเรือนกระจก
```cpp
// รดน้ำตามความชื้นเท่านั้น
{0, 0, 0, 0, false, true},   // Disable time-based, enable moisture-based
```

## 🔍 Troubleshooting

### Moisture Sensor Issues
- **ค่าผิดปกติ**: ตรวจสอบการเชื่อมต่อและแรงดัน
- **ค่าคงที่**: ตรวจสอบ sensor ว่าชำรุดหรือไม่
- **ค่าผันผวน**: เพิ่มการกรองข้อมูล (averaging)

### Webhook Issues
- **ไม่ส่งข้อความ**: ตรวจสอบ URL และ Token
- **ส่งซ้ำ**: ตรวจสอบ cooldown timer
- **ข้อความผิด**: ตรวจสอบ JSON format

### Relay Issues
- **ไม่ทำงาน**: ตรวจสอบการเชื่อมต่อและแรงดัน
- **ทำงานผิด**: ตรวจสอบ logic level (HIGH/LOW)
- **ร้อนเกิน**: ตรวจสอบ current rating

### WiFi Issues
- **เชื่อมต่อไม่ได้**: ตรวจสอบ SSID และ Password
- **หลุดบ่อย**: ตรวจสอบสัญญาณ WiFi
- **ช้า**: ตรวจสอบ bandwidth

## 📊 Performance Optimization

### Memory Usage
- ใช้ `StaticJsonDocument` แทน `DynamicJsonDocument`
- ลดขนาด HTML/CSS
- ใช้ `String` แทน `char*` เมื่อจำเป็น

### Power Consumption
- ปิด WiFi เมื่อไม่ใช้
- ลดการตรวจสอบความถี่
- ใช้ sleep mode เมื่อเหมาะสม

### Network Efficiency
- ใช้ cooldown timer สำหรับ webhook
- ลดขนาด JSON payload
- ใช้ compression เมื่อเป็นไปได้

## 🔒 Security Considerations

### Network Security
- ใช้ HTTPS เมื่อเป็นไปได้
- เปลี่ยน default passwords
- ใช้ firewall rules

### Physical Security
- ใช้ waterproof enclosures
- ตั้งค่าในที่ปลอดภัย
- ใช้ surge protection

### Data Privacy
- ไม่ส่งข้อมูลส่วนตัว
- เข้ารหัสข้อมูลสำคัญ
- ลบข้อมูลที่ไม่ใช้

## 📝 License

MIT License - ใช้งานได้อย่างอิสระ

## 🤝 Contributing

หากมีข้อเสนอแนะหรือพบปัญหา สามารถแจ้งได้เลยครับ

---

**สร้างโดย เนย - Senior Developer** 🌟

*ระบบรดน้ำอัตโนมัติที่ฉลาด ดูแลพืชของคุณอย่างมืออาชีพ* 💧🌱 
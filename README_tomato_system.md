# 🍅 Tomato Watering System

ระบบให้น้ำต้นมะเขือเทศอัตโนมัติด้วย ESP32 พร้อมเซ็นเซอร์วัดอุณหภูมิ ความชื้น และความชื้นในดิน

## ✨ Features

- **🌡️ Temperature Monitoring**: วัดอุณหภูมิด้วย DHT22
- **💧 Humidity Monitoring**: วัดความชื้นอากาศด้วย DHT22
- **🌱 Soil Moisture**: วัดความชื้นในดิน
- **💧 Smart Watering**: ให้น้ำอัตโนมัติตามเงื่อนไข
- **📱 Blink App Control**: ควบคุมผ่าน Blink App ได้
- **🌐 Web Interface**: ควบคุมผ่านเว็บไซต์ได้
- **⏰ Auto Mode**: ทำงานอัตโนมัติ
- **📊 Daily Limits**: จำกัดจำนวนครั้งต่อวัน
- **💡 Status LED**: แสดงสถานะการทำงาน

## 📋 Requirements

### Hardware
- ESP32 Development Board
- DHT22 Temperature & Humidity Sensor
- Soil Moisture Sensor (Capacitive/Resistive)
- 1-Channel Relay Module
- Water Pump (5V/12V)
- LED Status Indicator
- Power Supply (5V/2A)
- Jumper Wires
- Breadboard (optional)

### Software Libraries
- `DHT sensor library` by Adafruit
- `ArduinoJson` by Benoit Blanchon
- `WiFi` (built-in)
- `WebServer` (built-in)

## 🔌 Wiring Diagram

```
ESP32 Pin    Component    Description
--------     --------    -----------
3.3V    -->  DHT22 VCC   Power for DHT22 sensor
GND     -->  DHT22 GND   Ground for DHT22 sensor
GPIO4   -->  DHT22 DATA  Data line for DHT22

3.3V    -->  Soil Sensor VCC   Power for soil sensor
GND     -->  Soil Sensor GND   Ground for soil sensor
GPIO36  -->  Soil Sensor AOUT  Analog output from soil sensor

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
- DHT sensor library by Adafruit
- ArduinoJson by Benoit Blanchon
```

### 2. Configure WiFi
แก้ไขไฟล์ `tomato_system.ino`:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### 3. Upload Code
- เลือก Board: ESP32 Dev Module
- เลือก Port ที่ถูกต้อง
- กด Upload

## 🌐 Web Interface

หลังจาก upload code แล้ว:
1. เปิด Serial Monitor (115200 baud)
2. ดู IP address ที่ได้จาก WiFi
3. เปิดเว็บเบราว์เซอร์ไปที่ IP address

### Features ใน Web Interface:
- **Real-time Sensors**: แสดงค่าอุณหภูมิ ความชื้น และความชื้นในดิน
- **Manual Control**: ควบคุมการให้น้ำด้วยตนเอง
- **Status Monitoring**: ดูสถานะการทำงานแบบ real-time
- **Statistics**: แสดงสถิติการให้น้ำประจำวัน

## 📱 Blink App Control

ระบบรองรับการควบคุมผ่าน Blink App:

### API Endpoints สำหรับ Blink:
```
POST /api/blink/water    - ให้น้ำ 30 วินาที
GET  /api/blink/status   - ดูสถานะระบบ
```

### การตั้งค่าใน Blink App:
1. สร้าง HTTP Request
2. ตั้งค่า URL: `http://YOUR_ESP32_IP/api/blink/water`
3. เลือก Method: POST
4. ตั้งค่า Trigger ตามต้องการ

## 🌱 Watering Logic

ระบบจะให้น้ำอัตโนมัติเมื่อ:

### เงื่อนไขการให้น้ำ:
1. **ดินแห้ง**: ค่าความชื้นในดิน > 2000
2. **อากาศร้อนและแห้ง**: อุณหภูมิ > 35°C และความชื้น < 30%
3. **ดินแห้งมาก**: ค่าความชื้นในดิน > 2500

### การป้องกัน:
- จำกัดจำนวนครั้งต่อวัน: สูงสุด 8 ครั้ง
- ระยะเวลาการให้น้ำ: 30 วินาทีต่อครั้ง
- Auto Mode สามารถเปิด/ปิดได้

## 🔧 API Endpoints

### GET /api/status
```json
{
  "temperature": 28.5,
  "humidity": 65.2,
  "soilMoisture": 1800,
  "soilIsDry": false,
  "waterActive": false,
  "dailyWaterCount": 2,
  "autoMode": true
}
```

### POST /api/water
```json
{
  "duration": 30
}
```

### GET /api/blink/status
```json
{
  "water_active": false,
  "temperature": 28.5,
  "humidity": 65.2,
  "soil_moisture": 1800,
  "soil_is_dry": false,
  "daily_water": 2
}
```

## 🚀 Usage Examples

### การให้น้ำอัตโนมัติ
```cpp
// ระบบจะให้น้ำเมื่อดินแห้ง
if (soilIsDry) {
  startWaterPump(30);
}

// ระบบจะให้น้ำเมื่ออากาศร้อนและแห้ง
if (temperature > 35 && humidity < 30) {
  startWaterPump(30);
}
```

### การควบคุมด้วยตนเอง
```cpp
// ให้น้ำ 15 วินาที
manualWater(15);

// ให้น้ำ 30 วินาที
manualWater(30);

// ให้น้ำ 1 นาที
manualWater(60);
```

## 🔍 Troubleshooting

### DHT22 ไม่ทำงาน
- ตรวจสอบการเชื่อมต่อ GPIO4
- ตรวจสอบแรงดันไฟฟ้า 3.3V
- ตรวจสอบ library DHT sensor

### Soil Sensor ไม่ทำงาน
- ตรวจสอบการเชื่อมต่อ GPIO36
- ตรวจสอบแรงดันไฟฟ้า 3.3V
- ตรวจสอบการฝัง sensor ในดิน

### Relay ไม่ทำงาน
- ตรวจสอบการเชื่อมต่อ GPIO5
- ตรวจสอบแรงดันไฟฟ้า 5V สำหรับ relay module
- ตรวจสอบ logic level (HIGH = OFF, LOW = ON)

### WiFi ไม่เชื่อมต่อ
- ตรวจสอบ SSID และ Password
- ตรวจสอบสัญญาณ WiFi
- รอการเชื่อมต่อ (อาจใช้เวลาสักครู่)

## 📊 Sensor Calibration

### การปรับแต่ง Soil Sensor:
1. วัดค่าดินแห้ง (ไม่ให้น้ำ 2-3 วัน)
2. วัดค่าดินชุ่ม (ให้น้ำจนชุ่ม)
3. ปรับค่า threshold ในโค้ด:
```cpp
const int SOIL_MOISTURE_DRY = 2000;  // ค่าดินแห้ง
const int SOIL_MOISTURE_WET = 1500;  // ค่าดินชุ่ม
```

### การปรับแต่ง Temperature Threshold:
```cpp
const float TEMP_MAX = 35.0;         // อุณหภูมิสูงสุด
const float HUMIDITY_MIN = 30.0;     // ความชื้นต่ำสุด
```

## 📈 Daily Limits

ระบบมีการจำกัดจำนวนครั้งต่อวัน:
- **น้ำ**: สูงสุด 8 ครั้งต่อวัน
- การนับจะรีเซ็ตทุกวันเวลาเที่ยงคืน

## 🔧 Customization

### เปลี่ยน Pin Configuration
```cpp
#define DHT_PIN 4        // เปลี่ยน pin สำหรับ DHT22
#define SOIL_PIN 36      // เปลี่ยน pin สำหรับ Soil Sensor
#define RELAY_PIN 5      // เปลี่ยน pin สำหรับ Relay
#define LED_PIN 2        // เปลี่ยน pin สำหรับ LED
```

### เปลี่ยน Watering Duration
```cpp
#define WATER_PUMP_DURATION 30  // วินาที
```

### เปลี่ยน Daily Limits
```cpp
const int MAX_DAILY_WATER = 8;  // จำนวนครั้งต่อวัน
```

## 🌱 Plant Care Tips

### สำหรับต้นมะเขือเทศ:
- **อุณหภูมิที่เหมาะสม**: 20-30°C
- **ความชื้นที่เหมาะสม**: 60-80%
- **ความชื้นในดิน**: ควรชุ่มแต่ไม่แฉะ
- **การให้น้ำ**: 1-2 ครั้งต่อวัน

### การติดตั้ง:
1. วาง DHT22 ในที่ที่มีอากาศถ่ายเท
2. ฝัง soil sensor ในดินใกล้โคนต้น
3. ใช้ drip irrigation สำหรับการให้น้ำ
4. ตรวจสอบการรั่วซึมของน้ำ

## 📝 License

MIT License - ใช้งานได้อย่างอิสระ

## 🤝 Contributing

หากมีข้อเสนอแนะหรือพบปัญหา สามารถแจ้งได้เลยครับ

---

**สร้างโดย เนย - Senior Developer** 🌟 
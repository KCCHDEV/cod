# RDTRC Orchid Watering System

## ระบบรดน้ำกล้วยไม้อัตโนมัติแบบครบวงจร

### คุณสมบัติหลัก

#### 🌸 ระบบรดน้ำอัจฉริยะ
- **6 โซนสำหรับกล้วยไม้ประเภทต่างๆ**
  - Phalaenopsis (ฟาแลน)
  - Cattleya (แคทลียา)
  - Dendrobium (หวาย)
  - Vanda (แวนด้า)
  - Oncidium (ออนซิเดียม)
  - Cymbidium (ซิมบิเดียม)

- **ตารางการรดน้ำอัตโนมัติ**
  - ปรับตามประเภทกล้วยไม้
  - ตรวจสอบความชื้นดิน
  - ปรับตามสภาพอากาศ

#### 📊 การตรวจวัดหลาย Sensor
- **Soil Moisture Sensors (8 ตัว)**
  - วัดความชื้นดินแต่ละโซน
  - ระบบ redundancy สำหรับความแม่นยำ
  - Offline detection และ graceful degradation

- **Environmental Sensors**
  - DHT22: อุณหภูมิและความชื้นอากาศ
  - Light Sensor: ระดับแสง
  - pH Sensor: ความเป็นกรด-ด่างของน้ำ
  - EC Sensor: ความเข้มข้นของสารละลาย
  - Water Level Sensor: ระดับน้ำในถัง
  - Flow Sensor: อัตราการไหลของน้ำ

#### 🖥️ ระบบแสดงผลและควบคุม
- **LCD I2C 16x2**
  - แสดงสถานะระบบ
  - ข้อมูลสิ่งแวดล้อม
  - สถานะการรดน้ำ
  - ข้อผิดพลาดและคำเตือน

- **Web Interface**
  - ควบคุมผ่านเบราว์เซอร์
  - แสดงข้อมูลแบบ real-time
  - ตั้งค่าต่างๆ

- **Blynk Mobile App**
  - ควบคุมผ่านมือถือ
  - การแจ้งเตือน
  - ดูข้อมูลสถิติ

#### 🔧 ระบบความปลอดภัย
- **Offline Sensor Detection**
  - ตรวจสอบ sensor ที่ไม่ทำงาน
  - แสดงสถานะ "OFFLINE"
  - ทำงานต่อได้แม้ sensor บางตัวเสีย

- **Pump Protection**
  - ตรวจสอบระดับน้ำ
  - ป้องกันการทำงานเมื่อน้ำหมด
  - ตรวจสอบการไหลของน้ำ

- **Emergency Stop**
  - หยุดการทำงานฉุกเฉิน
  - การแจ้งเตือนอัตโนมัติ

### การติดตั้ง

#### อุปกรณ์ที่จำเป็น

##### Hardware
- ESP32 Development Board
- Soil Moisture Sensors (8 ตัว)
- DHT22 Temperature & Humidity Sensor
- Light Sensor (LDR)
- pH Sensor
- EC Sensor
- Water Level Sensor (Ultrasonic)
- Flow Sensor
- Solenoid Valves (6 ตัว)
- Water Pump
- LCD I2C 16x2
- Relay Module
- Breadboard และ Jumper Wires
- Power Supply 12V
- Water Tank

##### Software Libraries
```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <BlynkSimpleEsp32.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoOTA.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
```

#### การเชื่อมต่อ

##### Pin Configuration
```cpp
// Soil Moisture Sensors
#define SOIL_SENSOR_1_PIN 34  // Zone 1
#define SOIL_SENSOR_2_PIN 35  // Zone 2
#define SOIL_SENSOR_3_PIN 36  // Zone 3
#define SOIL_SENSOR_4_PIN 39  // Zone 4
#define SOIL_SENSOR_5_PIN 23  // Zone 5
#define SOIL_SENSOR_6_PIN 27  // Zone 6
#define SOIL_SENSOR_7_PIN 13  // Additional
#define SOIL_SENSOR_8_PIN 12  // Additional

// Environmental Sensors
#define DHT_PIN 32
#define LIGHT_SENSOR_PIN 33
#define PH_SENSOR_PIN 15
#define EC_SENSOR_PIN 16
#define WATER_LEVEL_TRIG_PIN 25
#define WATER_LEVEL_ECHO_PIN 14
#define FLOW_SENSOR_PIN 19

// Control Outputs
#define WATER_PUMP_PIN 18
#define VALVE_1_PIN 5   // Phalaenopsis
#define VALVE_2_PIN 17  // Cattleya
#define VALVE_3_PIN 21  // Dendrobium
#define VALVE_4_PIN 22  // Vanda
#define VALVE_5_PIN 23  // Oncidium
#define VALVE_6_PIN 25  // Cymbidium

// System
#define STATUS_LED_PIN 2
#define BUZZER_PIN 4
#define RESET_BUTTON_PIN 0
#define LCD_NEXT_BUTTON_PIN 26

// I2C
#define I2C_SDA 21
#define I2C_SCL 22
```

#### การตั้งค่า

##### 1. การตั้งค่า WiFi
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

##### 2. การตั้งค่า Blynk
```cpp
#define BLYNK_TEMPLATE_ID "TMPL61Zdwsx9r"
#define BLYNK_TEMPLATE_NAME "Orchid_Watering_System"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"
```

##### 3. การตั้งค่า LINE Notify
```cpp
const char* lineToken = "YOUR_LINE_NOTIFY_TOKEN";
```

### การใช้งาน

#### การรดน้ำอัตโนมัติ
ระบบจะรดน้ำตามตารางที่กำหนดไว้สำหรับแต่ละประเภทกล้วยไม้:

| ประเภทกล้วยไม้ | ความถี่ | ระยะเวลา | ความชื้นดิน |
|---------------|--------|----------|------------|
| Phalaenopsis | 7 วัน | 15 วินาที | 20% |
| Cattleya | 5 วัน | 12 วินาที | 15% |
| Dendrobium | 6 วัน | 18 วินาที | 18% |
| Vanda | 3 วัน | 25 วินาที | 12% |
| Oncidium | 4 วัน | 16 วินาที | 22% |
| Cymbidium | 7 วัน | 20 วินาที | 25% |

#### การตรวจสอบ Sensor
ระบบจะตรวจสอบสถานะของ sensor ทุก 30 วินาที:

- **Online**: Sensor ทำงานปกติ
- **Offline**: Sensor ไม่ตอบสนอง
- **Error**: Sensor มีข้อผิดพลาด

#### การแจ้งเตือน
ระบบจะส่งการแจ้งเตือนในกรณีต่อไปนี้:
- การรดน้ำเสร็จสิ้น
- Sensor ทำงานผิดปกติ
- ระดับน้ำต่ำ
- สภาพแวดล้อมไม่เหมาะสม
- ข้อผิดพลาดของระบบ

### การบำรุงรักษา

#### การตรวจสอบประจำวัน
1. ตรวจสอบระดับน้ำในถัง
2. ตรวจสอบการทำงานของ sensor
3. ตรวจสอบการไหลของน้ำ
4. ตรวจสอบข้อมูลสิ่งแวดล้อม

#### การบำรุงรักษารายสัปดาห์
1. ทำความสะอาด sensor
2. ตรวจสอบการทำงานของ valve
3. ตรวจสอบการเชื่อมต่อ WiFi
4. สำรองข้อมูล

#### การบำรุงรักษารายเดือน
1. เปลี่ยนน้ำในถัง
2. ทำความสะอาดระบบท่อ
3. ตรวจสอบการทำงานของ pump
4. อัปเดต firmware

### การแก้ไขปัญหา

#### Sensor Offline
1. ตรวจสอบการเชื่อมต่อสายไฟ
2. ตรวจสอบแหล่งจ่ายไฟ
3. รีเซ็ต sensor
4. ตรวจสอบ pin configuration

#### การรดน้ำไม่ทำงาน
1. ตรวจสอบระดับน้ำ
2. ตรวจสอบการทำงานของ pump
3. ตรวจสอบ valve
4. ตรวจสอบการตั้งค่าตารางเวลา

#### WiFi ไม่เชื่อมต่อ
1. ตรวจสอบ SSID และ Password
2. ตรวจสอบสัญญาณ WiFi
3. รีเซ็ต ESP32
4. ใช้โหมด Hotspot

### การพัฒนาเพิ่มเติม

#### การเพิ่ม Sensor
```cpp
// เพิ่ม sensor ใหม่
#define NEW_SENSOR_PIN 28

// เพิ่มในฟังก์ชัน initializeSensors()
pinMode(NEW_SENSOR_PIN, INPUT);
```

#### การเพิ่มโซนการรดน้ำ
```cpp
// เพิ่ม valve ใหม่
#define VALVE_7_PIN 28

// เพิ่มในอาร์เรย์ orchidSchedules
{"New_Orchid_Type", 120, 15000, 20, true}
```

#### การปรับแต่งการตั้งค่า
```cpp
// ปรับ threshold สำหรับกล้วยไม้
#define ORCHID_TEMP_MIN 18.0
#define ORCHID_TEMP_MAX 30.0
#define ORCHID_HUMIDITY_MIN 50.0
#define ORCHID_HUMIDITY_MAX 80.0
```

### ข้อมูลเทคนิค

#### ความต้องการพลังงาน
- **ESP32**: 3.3V, ~200mA
- **Sensors**: 3.3V-5V, ~100mA
- **Pump**: 12V, ~500mA
- **Valves**: 12V, ~200mA each

#### ความต้องการพื้นที่จัดเก็บ
- **SPIFFS**: 1MB สำหรับข้อมูลและ logs
- **RAM**: ~50KB สำหรับการทำงาน

#### ความแม่นยำ
- **Temperature**: ±0.5°C
- **Humidity**: ±2%
- **Soil Moisture**: ±3%
- **pH**: ±0.1
- **EC**: ±0.1 mS/cm

### การสนับสนุน

สำหรับคำถามหรือปัญหาการใช้งาน กรุณาติดต่อ:
- **Email**: support@rdtrc.com
- **Line**: @rdtrc_support
- **Website**: https://rdtrc.com

### License

This project is licensed under the MIT License - see the LICENSE file for details.

---

**RDTRC Orchid Watering System v4.0**  
*Made with ❤️ for orchid lovers*


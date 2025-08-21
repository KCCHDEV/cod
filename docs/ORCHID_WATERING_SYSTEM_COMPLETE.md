# RDTRC Orchid Watering System - Complete Documentation

## ระบบรดน้ำกล้วยไม้อัตโนมัติแบบครบวงจร

### 🌸 ภาพรวมระบบ

ระบบรดน้ำกล้วยไม้ RDTRC เป็นระบบอัตโนมัติที่ออกแบบมาเฉพาะสำหรับการดูแลกล้วยไม้ประเภทต่างๆ โดยใช้เทคโนโลยี IoT และ AI เพื่อให้การดูแลกล้วยไม้เป็นไปอย่างแม่นยำและมีประสิทธิภาพ

### 📊 คุณสมบัติหลัก

#### 1. ระบบรดน้ำอัจฉริยะ
- **6 โซนสำหรับกล้วยไม้ประเภทต่างๆ**
  - Phalaenopsis (ฟาแลน) - รดน้ำทุก 7 วัน
  - Cattleya (แคทลียา) - รดน้ำทุก 5 วัน
  - Dendrobium (หวาย) - รดน้ำทุก 6 วัน
  - Vanda (แวนด้า) - รดน้ำทุก 3 วัน
  - Oncidium (ออนซิเดียม) - รดน้ำทุก 4 วัน
  - Cymbidium (ซิมบิเดียม) - รดน้ำทุก 7 วัน

- **การปรับแต่งอัตโนมัติ**
  - ปรับตามความชื้นดิน
  - ปรับตามอุณหภูมิและความชื้นอากาศ
  - ปรับตามระดับแสง
  - ปรับตามคุณภาพน้ำ (pH, EC)

#### 2. ระบบตรวจวัดหลาย Sensor
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

#### 3. ระบบความปลอดภัย
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

### 🖥️ ระบบแสดงผลและควบคุม

#### 1. LCD I2C 16x2
- แสดงสถานะระบบ
- ข้อมูลสิ่งแวดล้อม
- สถานะการรดน้ำ
- ข้อผิดพลาดและคำเตือน

#### 2. Web Interface
- ควบคุมผ่านเบราว์เซอร์
- แสดงข้อมูลแบบ real-time
- ตั้งค่าต่างๆ

#### 3. Blynk Mobile App
- ควบคุมผ่านมือถือ
- การแจ้งเตือน
- ดูข้อมูลสถิติ

### 🔧 ระบบ Offline Detection

#### 1. Sensor Status Monitoring
```cpp
struct SensorStatus {
  bool isOnline;
  unsigned long lastReading;
  float lastValue;
  int errorCount;
  String sensorName;
};
```

#### 2. Graceful Degradation
- ระบบจะทำงานต่อได้แม้ sensor บางตัวเสีย
- แสดงสถานะ "OFFLINE" สำหรับ sensor ที่ไม่ทำงาน
- ปรับการทำงานตามจำนวน sensor ที่ใช้งานได้

#### 3. Error Handling
- ตรวจสอบ sensor ทุก 30 วินาที
- ลองใหม่ทุก 1 นาที
- แจ้งเตือนเมื่อ sensor หลายตัวเสีย

### 📁 โครงสร้างไฟล์

```
new System/orchid_watering_system/
├── orchid_watering_system.ino          # ไฟล์หลัก
├── RDTRC_Orchid_Library.h              # Header file
├── RDTRC_Orchid_Library.cpp            # Implementation
├── RDTRC_Common_Library.h              # Common library
├── RDTRC_LCD_Library.h                 # LCD library
├── README_orchid_system.md             # คู่มือการใช้งาน
└── ORCHID_SETUP_GUIDE_TH.md           # คู่มือการติดตั้ง
```

### 🔌 การเชื่อมต่อ Hardware

#### Pin Configuration
```cpp
// Soil Moisture Sensors
#define SOIL_SENSOR_1_PIN 34  // Zone 1 - Phalaenopsis
#define SOIL_SENSOR_2_PIN 35  // Zone 2 - Cattleya
#define SOIL_SENSOR_3_PIN 36  // Zone 3 - Dendrobium
#define SOIL_SENSOR_4_PIN 39  // Zone 4 - Vanda
#define SOIL_SENSOR_5_PIN 23  // Zone 5 - Oncidium
#define SOIL_SENSOR_6_PIN 27  // Zone 6 - Cymbidium
#define SOIL_SENSOR_7_PIN 13  // Additional sensor
#define SOIL_SENSOR_8_PIN 12  // Additional sensor

// Environmental Sensors
#define DHT_PIN 32            // DHT22
#define LIGHT_SENSOR_PIN 33   // Light Sensor
#define PH_SENSOR_PIN 15      // pH Sensor
#define EC_SENSOR_PIN 16      // EC Sensor
#define WATER_LEVEL_TRIG_PIN 25  // Ultrasonic Trig
#define WATER_LEVEL_ECHO_PIN 14  // Ultrasonic Echo
#define FLOW_SENSOR_PIN 19    // Flow Sensor

// Control Outputs
#define WATER_PUMP_PIN 18     // Water Pump
#define VALVE_1_PIN 5         // Phalaenopsis Valve
#define VALVE_2_PIN 17        // Cattleya Valve
#define VALVE_3_PIN 21        // Dendrobium Valve
#define VALVE_4_PIN 22        // Vanda Valve
#define VALVE_5_PIN 23        // Oncidium Valve
#define VALVE_6_PIN 25        // Cymbidium Valve

// System
#define STATUS_LED_PIN 2      // Status LED
#define BUZZER_PIN 4          // Buzzer
#define RESET_BUTTON_PIN 0    // Reset Button
#define LCD_NEXT_BUTTON_PIN 26 // LCD Navigation Button

// I2C
#define I2C_SDA 21           // I2C Data
#define I2C_SCL 22           // I2C Clock
```

### 🌡️ การตั้งค่าสิ่งแวดล้อม

#### Orchid-specific Thresholds
```cpp
#define ORCHID_TEMP_MIN 18.0
#define ORCHID_TEMP_MAX 30.0
#define ORCHID_HUMIDITY_MIN 50.0
#define ORCHID_HUMIDITY_MAX 80.0
#define ORCHID_PH_MIN 5.5
#define ORCHID_PH_MAX 6.5
#define ORCHID_EC_MIN 0.5
#define ORCHID_EC_MAX 2.0
```

#### Watering Schedule
```cpp
OrchidWateringSchedule orchidSchedules[NUM_ZONES] = {
  {"Phalaenopsis", 168, 15000, 20, true},  // 7 วัน, 15 วินาที, 20% ความชื้น
  {"Cattleya", 120, 12000, 15, true},      // 5 วัน, 12 วินาที, 15% ความชื้น
  {"Dendrobium", 144, 18000, 18, true},    // 6 วัน, 18 วินาที, 18% ความชื้น
  {"Vanda", 72, 25000, 12, true},          // 3 วัน, 25 วินาที, 12% ความชื้น
  {"Oncidium", 96, 16000, 22, true},       // 4 วัน, 16 วินาที, 22% ความชื้น
  {"Cymbidium", 168, 20000, 25, true}      // 7 วัน, 20 วินาที, 25% ความชื้น
};
```

### 📊 การตรวจสอบและแจ้งเตือน

#### 1. การแจ้งเตือนอัตโนมัติ
- การรดน้ำเสร็จสิ้น
- Sensor ทำงานผิดปกติ
- ระดับน้ำต่ำ
- สภาพแวดล้อมไม่เหมาะสม
- ข้อผิดพลาดของระบบ

#### 2. การบันทึกข้อมูล
- บันทึกข้อมูลสิ่งแวดล้อม
- บันทึกการรดน้ำ
- บันทึกข้อผิดพลาด
- บันทึกสถานะ sensor

#### 3. การวิเคราะห์ข้อมูล
- สถิติการรดน้ำ
- แนวโน้มสภาพแวดล้อม
- ประสิทธิภาพการทำงาน
- การบำรุงรักษา

### 🛠️ การบำรุงรักษา

#### 1. การตรวจสอบประจำวัน
- ตรวจสอบระดับน้ำในถัง
- ตรวจสอบการทำงานของ sensor
- ตรวจสอบการไหลของน้ำ
- ตรวจสอบข้อมูลสิ่งแวดล้อม

#### 2. การบำรุงรักษารายสัปดาห์
- ทำความสะอาด sensor
- ตรวจสอบการทำงานของ valve
- ตรวจสอบการเชื่อมต่อ WiFi
- สำรองข้อมูล

#### 3. การบำรุงรักษารายเดือน
- เปลี่ยนน้ำในถัง
- ทำความสะอาดระบบท่อ
- ตรวจสอบการทำงานของ pump
- อัปเดต firmware

### 🔄 การพัฒนาเพิ่มเติม

#### 1. การเพิ่ม Sensor
```cpp
// เพิ่ม sensor ใหม่
#define NEW_SENSOR_PIN 28

// เพิ่มในฟังก์ชัน initializeSensors()
pinMode(NEW_SENSOR_PIN, INPUT);
```

#### 2. การเพิ่มโซนการรดน้ำ
```cpp
// เพิ่ม valve ใหม่
#define VALVE_7_PIN 28

// เพิ่มในอาร์เรย์ orchidSchedules
{"New_Orchid_Type", 120, 15000, 20, true}
```

#### 3. การปรับแต่งการตั้งค่า
```cpp
// ปรับ threshold สำหรับกล้วยไม้
#define ORCHID_TEMP_MIN 18.0
#define ORCHID_TEMP_MAX 30.0
#define ORCHID_HUMIDITY_MIN 50.0
#define ORCHID_HUMIDITY_MAX 80.0
```

### 📈 ประสิทธิภาพระบบ

#### 1. ความแม่นยำ
- **Temperature**: ±0.5°C
- **Humidity**: ±2%
- **Soil Moisture**: ±3%
- **pH**: ±0.1
- **EC**: ±0.1 mS/cm

#### 2. ความต้องการพลังงาน
- **ESP32**: 3.3V, ~200mA
- **Sensors**: 3.3V-5V, ~100mA
- **Pump**: 12V, ~500mA
- **Valves**: 12V, ~200mA each

#### 3. ความต้องการพื้นที่จัดเก็บ
- **SPIFFS**: 1MB สำหรับข้อมูลและ logs
- **RAM**: ~50KB สำหรับการทำงาน

### 🌟 ข้อดีของระบบ

#### 1. ความแม่นยำสูง
- ใช้ sensor หลายตัวเพื่อความแม่นยำ
- ระบบ calibration อัตโนมัติ
- การปรับแต่งตามประเภทกล้วยไม้

#### 2. ความน่าเชื่อถือ
- ระบบ offline detection
- Graceful degradation
- การสำรองข้อมูล

#### 3. ความสะดวกในการใช้งาน
- ควบคุมผ่านมือถือ
- การแจ้งเตือนอัตโนมัติ
- Web interface ที่ใช้งานง่าย

#### 4. ความยืดหยุ่น
- ปรับแต่งได้ตามต้องการ
- เพิ่ม sensor และโซนได้
- รองรับกล้วยไม้หลายประเภท

### 🎯 การใช้งานจริง

#### 1. การติดตั้ง
- ตามคู่มือการติดตั้งใน `ORCHID_SETUP_GUIDE_TH.md`
- ตรวจสอบการเชื่อมต่อ hardware
- ทดสอบการทำงานของ sensor

#### 2. การตั้งค่า
- กำหนด WiFi และ Blynk
- ตั้งค่า LINE Notify
- ปรับแต่งตารางการรดน้ำ

#### 3. การใช้งาน
- ตรวจสอบข้อมูลผ่าน LCD
- ควบคุมผ่านมือถือ
- รับการแจ้งเตือนอัตโนมัติ

### 📞 การสนับสนุน

สำหรับคำถามหรือปัญหาการใช้งาน กรุณาติดต่อ:
- **Email**: support@rdtrc.com
- **Line**: @rdtrc_support
- **Website**: https://rdtrc.com

### 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

---

**RDTRC Orchid Watering System v4.0**  
*Complete documentation for orchid lovers*  
*Made with ❤️ by RDTRC Team*


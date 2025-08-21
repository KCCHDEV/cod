# คู่มือการติดตั้งระบบรดน้ำกล้วยไม้ RDTRC

## 📋 สิ่งที่ต้องเตรียม

### อุปกรณ์ Hardware

#### 1. ไมโครคอนโทรลเลอร์
- **ESP32 Development Board** (แนะนำ ESP32-WROOM-32)
- **USB Cable** สำหรับอัปโหลดโค้ด

#### 2. Sensors
- **Soil Moisture Sensors** (8 ตัว) - Capacitive Soil Moisture Sensor
- **DHT22** - Temperature & Humidity Sensor
- **Light Sensor** (LDR) - Photoresistor
- **pH Sensor** - Analog pH Sensor
- **EC Sensor** - Electrical Conductivity Sensor
- **Water Level Sensor** - Ultrasonic HC-SR04
- **Flow Sensor** - YF-S201 Water Flow Sensor

#### 3. Actuators
- **Solenoid Valves** (6 ตัว) - 12V DC Solenoid Valve
- **Water Pump** - 12V DC Submersible Pump
- **Relay Module** - 8-Channel Relay Module

#### 4. Display & Control
- **LCD I2C 16x2** - Liquid Crystal Display
- **Push Buttons** (2 ตัว) - สำหรับควบคุม
- **Buzzer** - สำหรับการแจ้งเตือน
- **LED** - Status Indicator

#### 5. Power & Wiring
- **Power Supply 12V** - สำหรับ pump และ valves
- **Breadboard** - สำหรับทดสอบ
- **Jumper Wires** - Male-to-Male, Male-to-Female
- **Water Tank** - ภาชนะเก็บน้ำ
- **Tubing** - ท่อน้ำ

### Software Libraries

#### 1. Arduino IDE Libraries
```cpp
// ติดตั้งผ่าน Library Manager
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

#### 2. การติดตั้ง Libraries
1. เปิด Arduino IDE
2. ไปที่ **Tools > Manage Libraries**
3. ค้นหาและติดตั้ง libraries ต่อไปนี้:
   - **WiFi** (built-in)
   - **WebServer** (built-in)
   - **ArduinoJson** by Benoit Blanchon
   - **SPIFFS** (built-in)
   - **ESPmDNS** (built-in)
   - **Blynk** by Volodymyr Shymanskyy
   - **NTPClient** by Fabrice Weinberg
   - **HTTPClient** (built-in)
   - **ArduinoOTA** (built-in)
   - **DHT sensor library** by Adafruit
   - **Wire** (built-in)
   - **LiquidCrystal I2C** by Frank de Brabander

## 🔧 การเชื่อมต่อ Hardware

### 1. การเชื่อมต่อ ESP32

#### Pin Mapping
```cpp
// Soil Moisture Sensors
SOIL_SENSOR_1_PIN = 34  // Zone 1 - Phalaenopsis
SOIL_SENSOR_2_PIN = 35  // Zone 2 - Cattleya
SOIL_SENSOR_3_PIN = 36  // Zone 3 - Dendrobium
SOIL_SENSOR_4_PIN = 39  // Zone 4 - Vanda
SOIL_SENSOR_5_PIN = 23  // Zone 5 - Oncidium
SOIL_SENSOR_6_PIN = 27  // Zone 6 - Cymbidium
SOIL_SENSOR_7_PIN = 13  // Additional sensor
SOIL_SENSOR_8_PIN = 12  // Additional sensor

// Environmental Sensors
DHT_PIN = 32            // DHT22
LIGHT_SENSOR_PIN = 33   // Light Sensor
PH_SENSOR_PIN = 15      // pH Sensor
EC_SENSOR_PIN = 16      // EC Sensor
WATER_LEVEL_TRIG_PIN = 25  // Ultrasonic Trig
WATER_LEVEL_ECHO_PIN = 14  // Ultrasonic Echo
FLOW_SENSOR_PIN = 19    // Flow Sensor

// Control Outputs
WATER_PUMP_PIN = 18     // Water Pump
VALVE_1_PIN = 5         // Phalaenopsis Valve
VALVE_2_PIN = 17        // Cattleya Valve
VALVE_3_PIN = 21        // Dendrobium Valve
VALVE_4_PIN = 22        // Vanda Valve
VALVE_5_PIN = 23        // Oncidium Valve
VALVE_6_PIN = 25        // Cymbidium Valve

// System
STATUS_LED_PIN = 2      // Status LED
BUZZER_PIN = 4          // Buzzer
RESET_BUTTON_PIN = 0    // Reset Button
LCD_NEXT_BUTTON_PIN = 26 // LCD Navigation Button

// I2C
I2C_SDA = 21           // I2C Data
I2C_SCL = 22           // I2C Clock
```

### 2. การเชื่อมต่อ Sensors

#### Soil Moisture Sensors
```
ESP32 Pin 34 → Soil Sensor 1 VCC
ESP32 Pin 35 → Soil Sensor 2 VCC
ESP32 Pin 36 → Soil Sensor 3 VCC
ESP32 Pin 39 → Soil Sensor 4 VCC
ESP32 Pin 23 → Soil Sensor 5 VCC
ESP32 Pin 27 → Soil Sensor 6 VCC
ESP32 Pin 13 → Soil Sensor 7 VCC
ESP32 Pin 12 → Soil Sensor 8 VCC

GND → All Soil Sensors GND
3.3V → All Soil Sensors VCC
```

#### DHT22 Sensor
```
ESP32 Pin 32 → DHT22 Data
3.3V → DHT22 VCC
GND → DHT22 GND
```

#### Light Sensor (LDR)
```
ESP32 Pin 33 → LDR (with 10kΩ resistor to GND)
3.3V → LDR VCC
GND → LDR GND
```

#### pH Sensor
```
ESP32 Pin 15 → pH Sensor Signal
3.3V → pH Sensor VCC
GND → pH Sensor GND
```

#### EC Sensor
```
ESP32 Pin 16 → EC Sensor Signal
3.3V → EC Sensor VCC
GND → EC Sensor GND
```

#### Water Level Sensor (Ultrasonic)
```
ESP32 Pin 25 → HC-SR04 Trig
ESP32 Pin 14 → HC-SR04 Echo
5V → HC-SR04 VCC
GND → HC-SR04 GND
```

#### Flow Sensor
```
ESP32 Pin 19 → YF-S201 Signal
5V → YF-S201 VCC
GND → YF-S201 GND
```

### 3. การเชื่อมต่อ Actuators

#### Solenoid Valves (ผ่าน Relay)
```
Relay 1 → Valve 1 (Phalaenopsis)
Relay 2 → Valve 2 (Cattleya)
Relay 3 → Valve 3 (Dendrobium)
Relay 4 → Valve 4 (Vanda)
Relay 5 → Valve 5 (Oncidium)
Relay 6 → Valve 6 (Cymbidium)

ESP32 Pin 5 → Relay 1 IN
ESP32 Pin 17 → Relay 2 IN
ESP32 Pin 21 → Relay 3 IN
ESP32 Pin 22 → Relay 4 IN
ESP32 Pin 23 → Relay 5 IN
ESP32 Pin 25 → Relay 6 IN

12V Power Supply → Relay VCC
GND → Relay GND
```

#### Water Pump
```
ESP32 Pin 18 → Pump Relay IN
12V Power Supply → Pump VCC
GND → Pump GND
```

### 4. การเชื่อมต่อ Display & Control

#### LCD I2C 16x2
```
ESP32 Pin 21 (SDA) → LCD SDA
ESP32 Pin 22 (SCL) → LCD SCL
3.3V → LCD VCC
GND → LCD GND
```

#### Push Buttons
```
ESP32 Pin 0 → Reset Button (with 10kΩ pull-up)
ESP32 Pin 26 → LCD Navigation Button (with 10kΩ pull-up)
GND → Button GND
```

#### Buzzer & LED
```
ESP32 Pin 4 → Buzzer +
ESP32 Pin 2 → LED + (with 220Ω resistor)
GND → Buzzer - & LED -
```

## 💻 การตั้งค่า Software

### 1. การตั้งค่า Arduino IDE

#### Board Configuration
1. เปิด Arduino IDE
2. ไปที่ **File > Preferences**
3. เพิ่ม URL ใน **Additional Board Manager URLs**:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. ไปที่ **Tools > Board > Boards Manager**
5. ค้นหา "ESP32" และติดตั้ง **ESP32 by Espressif Systems**
6. เลือก Board: **Tools > Board > ESP32 Arduino > ESP32 Dev Module**

#### Port Configuration
1. เชื่อมต่อ ESP32 กับคอมพิวเตอร์
2. เลือก Port: **Tools > Port > COM3** (หรือ port ที่เหมาะสม)

### 2. การตั้งค่า WiFi

#### แก้ไขไฟล์ `orchid_watering_system.ino`
```cpp
// Network Configuration
const char* ssid = "YOUR_WIFI_SSID";           // เปลี่ยนเป็น WiFi ของคุณ
const char* password = "YOUR_WIFI_PASSWORD";   // เปลี่ยนเป็นรหัส WiFi ของคุณ
```

### 3. การตั้งค่า Blynk

#### สร้าง Blynk Account
1. ไปที่ [Blynk.io](https://blynk.io)
2. สร้างบัญชีใหม่
3. สร้าง Device ใหม่
4. เลือก **ESP32** และ **WiFi**
5. รับ **Auth Token**

#### แก้ไขไฟล์ `orchid_watering_system.ino`
```cpp
// Blynk Configuration
#define BLYNK_TEMPLATE_ID "TMPL61Zdwsx9r"
#define BLYNK_TEMPLATE_NAME "Orchid_Watering_System"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"  // เปลี่ยนเป็น token ของคุณ
```

### 4. การตั้งค่า LINE Notify

#### สร้าง LINE Notify Token
1. ไปที่ [LINE Notify](https://notify-bot.line.me/)
2. เข้าสู่ระบบด้วย LINE
3. ไปที่ **My Page**
4. คลิก **Generate Token**
5. ตั้งชื่อ Token และเลือกกลุ่มที่ต้องการส่งข้อความ
6. รับ **Token**

#### แก้ไขไฟล์ `orchid_watering_system.ino`
```cpp
// LINE Notify Configuration
const char* lineToken = "YOUR_LINE_NOTIFY_TOKEN";  // เปลี่ยนเป็น token ของคุณ
```

## 🚀 การอัปโหลดโค้ด

### 1. การเตรียมไฟล์
1. คัดลอกไฟล์ทั้งหมดไปยังโฟลเดอร์ Arduino:
   - `orchid_watering_system.ino`
   - `RDTRC_Orchid_Library.h`
   - `RDTRC_Orchid_Library.cpp`
   - `RDTRC_Common_Library.h`
   - `RDTRC_LCD_Library.h`

### 2. การอัปโหลด
1. เปิดไฟล์ `orchid_watering_system.ino` ใน Arduino IDE
2. ตรวจสอบการตั้งค่า Board และ Port
3. คลิก **Verify** เพื่อตรวจสอบโค้ด
4. คลิก **Upload** เพื่ออัปโหลดโค้ด

### 3. การตรวจสอบการทำงาน
1. เปิด **Serial Monitor** (Tools > Serial Monitor)
2. ตั้งค่า Baud Rate เป็น **115200**
3. รีเซ็ต ESP32
4. ตรวจสอบข้อความใน Serial Monitor

## 🔧 การทดสอบระบบ

### 1. การทดสอบ Sensors

#### Soil Moisture Sensors
```cpp
// ทดสอบใน Serial Monitor
Serial.println("Testing Soil Moisture Sensors:");
for (int i = 0; i < 8; i++) {
  int value = analogRead(soilSensorPins[i]);
  Serial.printf("Soil %d: %d\n", i+1, value);
}
```

#### Environmental Sensors
```cpp
// ทดสอบ DHT22
float temp = dht.readTemperature();
float humidity = dht.readHumidity();
Serial.printf("Temperature: %.1f°C, Humidity: %.1f%%\n", temp, humidity);

// ทดสอบ Light Sensor
int light = analogRead(LIGHT_SENSOR_PIN);
Serial.printf("Light Level: %d\n", light);
```

### 2. การทดสอบ Actuators

#### Solenoid Valves
```cpp
// ทดสอบแต่ละ valve
for (int i = 0; i < 6; i++) {
  Serial.printf("Testing Valve %d\n", i+1);
  digitalWrite(valvePins[i], HIGH);
  delay(1000);
  digitalWrite(valvePins[i], LOW);
  delay(1000);
}
```

#### Water Pump
```cpp
// ทดสอบ pump
Serial.println("Testing Water Pump");
digitalWrite(WATER_PUMP_PIN, HIGH);
delay(3000);
digitalWrite(WATER_PUMP_PIN, LOW);
```

### 3. การทดสอบ LCD
```cpp
// ทดสอบ LCD
systemLCD.begin();
systemLCD.displaySystemStatus(true, 8, 8);
delay(2000);
systemLCD.displayEnvironmentalData(25.5, 65.2, 45, 6.2, 1.5);
```

## 🛠️ การแก้ไขปัญหา

### 1. Sensor ไม่ทำงาน

#### ตรวจสอบการเชื่อมต่อ
1. ตรวจสอบสายไฟ
2. ตรวจสอบแหล่งจ่ายไฟ
3. ตรวจสอบ pin configuration

#### ตรวจสอบใน Serial Monitor
```cpp
// เพิ่ม debug code
Serial.println("Sensor Debug:");
Serial.printf("Soil 1: %d\n", analogRead(SOIL_SENSOR_1_PIN));
Serial.printf("DHT Temp: %.1f\n", dht.readTemperature());
Serial.printf("Light: %d\n", analogRead(LIGHT_SENSOR_PIN));
```

### 2. WiFi ไม่เชื่อมต่อ

#### ตรวจสอบการตั้งค่า
1. ตรวจสอบ SSID และ Password
2. ตรวจสอบสัญญาณ WiFi
3. ตรวจสอบการตั้งค่า IP

#### ใช้โหมด Hotspot
```cpp
// ระบบจะสร้าง WiFi hotspot อัตโนมัติ
// SSID: RDTRC_OrchidWater
// Password: rdtrc123
```

### 3. Valve ไม่ทำงาน

#### ตรวจสอบ Relay
1. ตรวจสอบการเชื่อมต่อ relay
2. ตรวจสอบแหล่งจ่ายไฟ 12V
3. ตรวจสอบการทำงานของ relay

#### ทดสอบ Relay
```cpp
// ทดสอบ relay แต่ละตัว
for (int i = 0; i < 6; i++) {
  Serial.printf("Testing Relay %d\n", i+1);
  digitalWrite(valvePins[i], HIGH);
  delay(500);
  digitalWrite(valvePins[i], LOW);
  delay(500);
}
```

## 📱 การใช้งานระบบ

### 1. การเข้าถึง Web Interface
1. เชื่อมต่อ WiFi กับ ESP32
2. เปิดเบราว์เซอร์
3. ไปที่ `http://192.168.4.1` (โหมด Hotspot)
4. หรือ `http://[ESP32_IP]` (โหมด WiFi)

### 2. การใช้งาน Blynk App
1. ดาวน์โหลด Blynk App
2. เข้าสู่ระบบด้วยบัญชี Blynk
3. สแกน QR Code หรือใส่ Auth Token
4. ควบคุมระบบผ่านมือถือ

### 3. การตั้งค่าตารางการรดน้ำ
```cpp
// แก้ไขในโค้ด
OrchidWateringSchedule orchidSchedules[NUM_ZONES] = {
  {"Phalaenopsis", 168, 15000, 20, true},  // 7 วัน, 15 วินาที
  {"Cattleya", 120, 12000, 15, true},      // 5 วัน, 12 วินาที
  // ... เพิ่มเติมตามต้องการ
};
```

## 🔄 การบำรุงรักษา

### 1. การตรวจสอบประจำวัน
- ตรวจสอบระดับน้ำในถัง
- ตรวจสอบการทำงานของ sensor
- ตรวจสอบข้อมูลใน LCD

### 2. การบำรุงรักษารายสัปดาห์
- ทำความสะอาด sensor
- ตรวจสอบการทำงานของ valve
- สำรองข้อมูล

### 3. การบำรุงรักษารายเดือน
- เปลี่ยนน้ำในถัง
- ทำความสะอาดระบบท่อ
- อัปเดต firmware

## 📞 การสนับสนุน

หากมีปัญหาหรือคำถาม กรุณาติดต่อ:
- **Email**: support@rdtrc.com
- **Line**: @rdtrc_support
- **Website**: https://rdtrc.com

---

**RDTRC Orchid Watering System Setup Guide v4.0**  
*Made with ❤️ for orchid lovers*


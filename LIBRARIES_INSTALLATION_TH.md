# 📚 คู่มือการติดตั้ง Libraries สำหรับระบบ ESP32

## 📋 สารบัญ
1. [ภาพรวม Libraries](#ภาพรวม-libraries)
2. [การติดตั้ง Arduino IDE](#การติดตั้ง-arduino-ide)
3. [Libraries หลักที่จำเป็น](#libraries-หลักที่จำเป็น)
4. [Libraries เสริมตามระบบ](#libraries-เสริมตามระบบ)
5. [การติดตั้งแบบ Manual](#การติดตั้งแบบ-manual)
6. [การแก้ไขปัญหา](#การแก้ไขปัญหา)
7. [การอัพเดต Libraries](#การอัพเดต-libraries)

---

## 🎯 ภาพรวม Libraries

### Libraries ที่ใช้ในโปรเจ็กต์
```
┌─────────────────────────────────────────┐
│            ESP32 LIBRARIES              │
├─────────────────────────────────────────┤
│ Core Libraries (Built-in):              │
│ • WiFi                                  │
│ • WebServer                             │
│ • Wire (I2C)                            │
│ • EEPROM                                │
│ • HTTPClient                            │
├─────────────────────────────────────────┤
│ External Libraries (Install Required):   │
│ • RTClib (Adafruit)                     │
│ • ArduinoJson (Benoit Blanchon)         │
│ • DHT sensor library (Adafruit)         │
│ • ESP32Servo (Kevin Harrington)         │
│ • Adafruit Unified Sensor               │
└─────────────────────────────────────────┘
```

### การใช้งานตามระบบ
```
ระบบ                    Libraries ที่ต้องการ
──────────────────────────────────────────────
Basic Watering      →  RTClib, ArduinoJson
Rose System         →  RTClib, ArduinoJson, HTTPClient
Tomato System       →  DHT, ArduinoJson  
Bird Feeding        →  RTClib, ESP32Servo, ArduinoJson
Mushroom System     →  RTClib, ArduinoJson, HTTPClient
```

---

## 🛠️ การติดตั้ง Arduino IDE

### ขั้นตอนเตรียมการ
1. **ดาวน์โหลด Arduino IDE 2.x**
   ```
   https://www.arduino.cc/en/software
   ```

2. **เพิ่ม ESP32 Board Support**
   - เปิด Arduino IDE
   - ไปที่ `File > Preferences`
   - เพิ่ม URL ใน "Additional Boards Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - กด OK

3. **ติดตั้ง ESP32 Board Package**
   - ไปที่ `Tools > Board > Boards Manager`
   - ค้นหา "ESP32"
   - เลือก "esp32 by Espressif Systems"
   - กด Install

---

## 📦 Libraries หลักที่จำเป็น

### 1. RTClib by Adafruit
**วัตถุประสงค์**: ควบคุม RTC DS3231 สำหรับการจับเวลา

**การติดตั้ง**:
1. เปิด Arduino IDE
2. ไปที่ `Tools > Manage Libraries`
3. ค้นหา "RTClib"
4. เลือก "RTClib by Adafruit"
5. กด Install

**การใช้งาน**:
```cpp
#include <RTClib.h>

RTC_DS3231 rtc;

void setup() {
  rtc.begin();
  
  // ตั้งเวลา (ครั้งแรกเท่านั้น)
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  DateTime now = rtc.now();
  Serial.println(now.timestamp());
}
```

**เวอร์ชันที่แนะนำ**: 2.1.1 หรือใหม่กว่า

---

### 2. ArduinoJson by Benoit Blanchon
**วัตถุประสงค์**: จัดการข้อมูล JSON สำหรับ Web API

**การติดตั้ง**:
1. ไปที่ `Tools > Manage Libraries`
2. ค้นหา "ArduinoJson"
3. เลือก "ArduinoJson by Benoit Blanchon"
4. กด Install

**การใช้งาน**:
```cpp
#include <ArduinoJson.h>

void createJsonResponse() {
  DynamicJsonDocument doc(1024);
  
  doc["time"] = "2024-01-01T12:00:00";
  doc["temperature"] = 25.5;
  doc["humidity"] = 60;
  
  JsonArray relays = doc.createNestedArray("relays");
  relays.add(true);
  relays.add(false);
  
  String output;
  serializeJson(doc, output);
  Serial.println(output);
}
```

**เวอร์ชันที่แนะนำ**: 6.21.3 หรือใหม่กว่า

---

### 3. DHT sensor library by Adafruit  
**วัตถุประสงค์**: อ่านค่าจากเซ็นเซอร์ DHT22/DHT11

**การติดตั้ง**:
1. ไปที่ `Tools > Manage Libraries`
2. ค้นหา "DHT sensor library"
3. เลือก "DHT sensor library by Adafruit"
4. กด Install
5. ติดตั้ง Dependencies ที่ขึ้นมา: "Adafruit Unified Sensor"

**การใช้งาน**:
```cpp
#include "DHT.h"

#define DHT_PIN 4
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  dht.begin();
}

void loop() {
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  if (!isnan(temp) && !isnan(humidity)) {
    Serial.println("Temperature: " + String(temp) + "°C");
    Serial.println("Humidity: " + String(humidity) + "%");
  }
}
```

**เวอร์ชันที่แนะนำ**: 1.4.4 หรือใหม่กว่า

---

### 4. ESP32Servo by Kevin Harrington
**วัตถุประสงค์**: ควบคุม Servo Motor บน ESP32

**การติดตั้ง**:
1. ไปที่ `Tools > Manage Libraries`
2. ค้นหา "ESP32Servo"
3. เลือก "ESP32Servo by Kevin Harrington"
4. กด Install

**การใช้งาน**:
```cpp
#include <ESP32Servo.h>

Servo myServo;
const int servoPin = 13;

void setup() {
  myServo.attach(servoPin);
}

void loop() {
  // เปิดฝา (90 องศา)
  myServo.write(90);
  delay(1000);
  
  // ปิดฝา (0 องศา)
  myServo.write(0);
  delay(1000);
}
```

**เวอร์ชันที่แนะนำ**: 0.13.0 หรือใหม่กว่า

---

## 🔌 Libraries เสริมตามระบบ

### สำหรับระบบขั้นสูง

#### 1. Adafruit Unified Sensor
**จำเป็นสำหรับ**: DHT sensor library

**การติดตั้ง**:
- มักจะติดตั้งอัตโนมัติเมื่อติดตั้ง DHT library
- หากไม่มี: ค้นหา "Adafruit Unified Sensor" และติดตั้ง

#### 2. WiFiClientSecure (Built-in)
**วัตถุประสงค์**: การเชื่อมต่อ HTTPS

**การใช้งาน**:
```cpp
#include <WiFiClientSecure.h>

WiFiClientSecure client;

void setup() {
  client.setInsecure(); // สำหรับ SSL ที่ไม่ต้องการ Certificate
}
```

#### 3. SPIFFS (Built-in)
**วัตถุประสงค์**: จัดเก็บไฟล์ใน Flash Memory

**การใช้งาน**:
```cpp
#include "SPIFFS.h"

void setup() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  
  File file = SPIFFS.open("/config.txt", "r");
  if (file) {
    String content = file.readString();
    Serial.println(content);
    file.close();
  }
}
```

---

## 💾 การติดตั้งแบบ Manual

### เมื่อ Library Manager ไม่ทำงาน

#### วิธีที่ 1: ดาวน์โหลดจาก GitHub
1. **RTClib**:
   ```
   https://github.com/adafruit/RTClib/archive/refs/heads/master.zip
   ```

2. **ArduinoJson**:
   ```
   https://github.com/bblanchon/ArduinoJson/releases
   ```

3. **DHT sensor library**:
   ```
   https://github.com/adafruit/DHT-sensor-library/archive/refs/heads/master.zip
   ```

#### วิธีที่ 2: ติดตั้งจาก ZIP
1. ดาวน์โหลดไฟล์ ZIP
2. ใน Arduino IDE: `Sketch > Include Library > Add .ZIP Library`
3. เลือกไฟล์ ZIP ที่ดาวน์โหลด
4. กด Open

#### วิธีที่ 3: Copy Manual
1. แตกไฟล์ ZIP
2. Copy โฟลเดอร์ไปยัง:
   ```
   Windows: Documents\Arduino\libraries\
   Mac: ~/Documents/Arduino/libraries/
   Linux: ~/Arduino/libraries/
   ```
3. รีสตาร์ท Arduino IDE

---

## 🔧 การแก้ไขปัญหา

### ปัญหาการติดตั้ง

#### Library Not Found
```
Error: library not found
```
**แก้ไข**:
1. ตรวจสอบชื่อ Library ให้ถูกต้อง
2. ลองรีเฟรช Library Manager
3. ติดตั้งแบบ Manual

#### Version Conflict
```
Error: multiple libraries found
```
**แก้ไข**:
1. ลบ Library เวอร์ชันเก่า:
   ```
   ไปที่โฟลเดอร์ libraries/
   ลบโฟลเดอร์ที่ซ้ำ
   ```
2. ติดตั้งเวอร์ชันล่าสุดใหม่

#### Compilation Error
```
Error: 'class' does not name a type
```
**แก้ไข**:
1. ตรวจสอบ #include statements
2. ตรวจสอบ Library dependencies
3. อัพเดต ESP32 Board Package

### ปัญหา Dependencies

#### ตรวจสอบ Dependencies ที่จำเป็น
```cpp
// สำหรับ DHT22
#include "DHT.h"           // Main library
#include <Adafruit_Sensor.h> // Dependency

// สำหรับ RTC
#include <RTClib.h>
#include <Wire.h>          // I2C dependency

// สำหรับ JSON
#include <ArduinoJson.h>   // Self-contained

// สำหรับ Servo
#include <ESP32Servo.h>    // Self-contained
```

### การตรวจสอบการติดตั้ง

#### Test Code สำหรับแต่ละ Library
```cpp
// Test RTClib
#ifdef RTCLIB_H
  Serial.println("✅ RTClib: OK");
#else
  Serial.println("❌ RTClib: Missing");
#endif

// Test ArduinoJson
#ifdef ARDUINOJSON_H
  Serial.println("✅ ArduinoJson: OK");
#else
  Serial.println("❌ ArduinoJson: Missing");
#endif

// Test DHT
#ifdef DHT_H
  Serial.println("✅ DHT: OK");
#else
  Serial.println("❌ DHT: Missing");
#endif
```

---

## 📈 การอัพเดต Libraries

### การอัพเดตแบบอัตโนมัติ
1. เปิด Arduino IDE
2. ไปที่ `Tools > Manage Libraries`
3. ดูรายการที่มี "UPDATE" ข้างหลัง
4. กดอัพเดตทีละตัว หรือ "Update All"

### การตรวจสอบเวอร์ชัน
```cpp
void printLibraryVersions() {
  Serial.println("=== Library Versions ===");
  
  #ifdef RTCLIB_VERSION
    Serial.println("RTClib: " + String(RTCLIB_VERSION));
  #endif
  
  #ifdef ARDUINOJSON_VERSION
    Serial.println("ArduinoJson: " + String(ARDUINOJSON_VERSION));
  #endif
  
  Serial.println("ESP32 Core: " + String(ESP.getSdkVersion()));
}
```

### Changelog และ Breaking Changes
เมื่ออัพเดต Libraries ควรตรวจสอบ:
1. **Breaking Changes**: การเปลี่ยนแปลงที่อาจทำให้โค้ดเก่าใช้ไม่ได้
2. **New Features**: ฟีเจอร์ใหม่ที่อาจเป็นประโยชน์
3. **Bug Fixes**: การแก้ไขบัคที่อาจส่งผลต่อโปรเจ็กต์

---

## 📋 Checklist การติดตั้ง

### ก่อนเริ่มโปรเจ็กต์
- [ ] ติดตั้ง Arduino IDE 2.x เรียบร้อย
- [ ] เพิ่ม ESP32 Board Support แล้ว
- [ ] ติดตั้ง ESP32 Board Package แล้ว

### Libraries พื้นฐาน
- [ ] RTClib by Adafruit (v2.1.1+)
- [ ] ArduinoJson by Benoit Blanchon (v6.21.3+)

### Libraries เสริม (ตามความต้องการ)
- [ ] DHT sensor library by Adafruit (v1.4.4+)
- [ ] Adafruit Unified Sensor
- [ ] ESP32Servo by Kevin Harrington (v0.13.0+)

### การทดสอบ
- [ ] Compile โค้ดตัวอย่างสำเร็จ
- [ ] Upload ไปยัง ESP32 ได้
- [ ] ไม่มี Error เกี่ยวกับ Library
- [ ] ระบบทำงานตามที่ออกแบบ

---

## 🎯 เคล็ดลับการใช้งาน

### การจัดการ Include Files
```cpp
// จัดเรียงตามความสำคัญ
#include <WiFi.h>           // Core libraries ก่อน
#include <WebServer.h>
#include <Wire.h>

#include <RTClib.h>         // External libraries ตาม
#include <ArduinoJson.h>
#include "DHT.h"

// Local includes สุดท้าย
#include "config.h"
```

### การตรวจสอบ Library Size
```cpp
void printMemoryUsage() {
  Serial.println("=== Memory Usage ===");
  Serial.println("Free Heap: " + String(ESP.getFreeHeap()));
  Serial.println("Total Heap: " + String(ESP.getHeapSize()));
  Serial.println("Free PSRAM: " + String(ESP.getFreePsram()));
}
```

### การใช้ Conditional Compilation
```cpp
// ใช้เมื่อต้องการ Library บางตัวเป็น Optional
#ifdef DHT_H
  DHT dht(DHT_PIN, DHT_TYPE);
  #define HAS_DHT true
#else
  #define HAS_DHT false
#endif

void readSensors() {
  #if HAS_DHT
    float temp = dht.readTemperature();
    // process temperature
  #endif
}
```

---

## 📞 แหล่งข้อมูลเพิ่มเติม

### Official Documentation
- **Arduino**: https://docs.arduino.cc/
- **ESP32**: https://docs.espressif.com/projects/esp-idf/
- **RTClib**: https://adafruit.github.io/RTClib/html/
- **ArduinoJson**: https://arduinojson.org/

### Community Resources
- **Arduino Forum**: https://forum.arduino.cc/
- **ESP32 Reddit**: https://reddit.com/r/esp32/
- **Adafruit Forum**: https://forums.adafruit.com/

### Troubleshooting
1. ตรวจสอบ Serial Monitor สำหรับ Error Messages
2. ใช้ "Verbose output" ใน Arduino IDE Preferences
3. ลองใช้โค้ดตัวอย่างจาก Library
4. ตรวจสอบ GitHub Issues ของ Library

---

## ⚠️ ข้อควรระวัง

### Memory Management
- ESP32 มี RAM จำกัด (~320KB)
- ใช้ `String` อย่างระมัดระวัง
- เลือก Library ที่เหมาะสมกับขนาดโปรเจ็กต์

### Version Compatibility
- ตรวจสอบความเข้ากันได้ระหว่าง Libraries
- ใช้เวอร์ชันที่ stable
- อ่าน Breaking Changes ก่อนอัพเดต

### Security Considerations
- อัพเดต Libraries เป็นประจำเพื่อความปลอดภัย
- ใช้ HTTPS เมื่อเป็นไปได้
- ไม่ hardcode sensitive data ในโค้ด

---

🚀 **Libraries ที่ติดตั้งครบถ้วนจะทำให้การพัฒนาราบรื่นและมีประสิทธิภาพ!**
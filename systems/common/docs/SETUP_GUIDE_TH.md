# 🌱 คู่มือการติดตั้งและใช้งานระบบอัตโนมัติด้วย ESP32

## 📋 สารบัญ
1. [ภาพรวมของระบบ](#ภาพรวมของระบบ)
2. [อุปกรณ์ที่จำเป็น](#อุปกรณ์ที่จำเป็น)
3. [การติดตั้ง Arduino IDE](#การติดตั้ง-arduino-ide)
4. [การติดตั้ง Libraries](#การติดตั้ง-libraries)
5. [การเชื่อมต่ออุปกรณ์](#การเชื่อมต่ออุปกรณ์)
6. [การอัพโหลดโค้ด](#การอัพโหลดโค้ด)
7. [การตั้งค่า WiFi](#การตั้งค่า-wifi)
8. [การใช้งานแต่ละระบบ](#การใช้งานแต่ละระบบ)
9. [การแก้ไขปัญหา](#การแก้ไขปัญหา)

---

## 🎯 ภาพรวมของระบบ

โปรเจ็กต์นี้ประกอบด้วยระบบอัตโนมัติ 5 ระบบหลัก:

### 1. ระบบรดน้ำพื้นฐาน (Watering System)
- **ไฟล์**: `examples/watering_system.ino`
- **จุดประสงค์**: รดน้ำต้นไม้อัตโนมัติตามเวลาที่กำหนด
- **อุปกรณ์**: ESP32, RTC DS3231, Relay 4 ช่อง

### 2. ระบบรดน้ำกุหลาบ (Rose Watering System)
- **ไฟล์**: `rose_watering_system.ino`
- **จุดประสงค์**: ระบบเฉพาะสำหรับปลูกกุหลาบ พร้อมการเชื่อมต่อ Blink Camera
- **อุปกรณ์**: ESP32, RTC DS3231, Relay 4 ช่อง, เซ็นเซอร์ความชื้น

### 3. ระบบรดน้ำมะเขือเทศ (Tomato System)
- **ไฟล์**: `tomato_system.ino` และ `tomato_watering_system.ino`
- **จุดประสงค์**: ระบบเฉพาะสำหรับปลูกมะเขือเทศ
- **อุปกรณ์**: ESP32, DHT22, เซ็นเซอร์ความชื้นดิน, Relay

### 4. ระบบให้อาหารนก (Bird Feeding System)
- **ไฟล์**: `bird_feeding_system.ino`
- **จุดประสงค์**: ให้น้ำและอาหารนกอัตโนมัติ
- **อุปกรณ์**: ESP32, RTC DS3231, Servo Motor, Relay

### 5. ระบบปลูกเห็ด (Mushroom System)
- **ไฟล์**: `mushroom_system.ino`
- **จุดประสงค์**: ควบคุมความชื้นสำหรับการปลูกเห็ด
- **อุปกรณ์**: ESP32, RTC DS3231, Relay 4 ช่อง

---

## 🛠️ อุปกรณ์ที่จำเป็น

### อุปกรณ์หลัก (ทุกระบบ)
- **ESP32 Development Board** × 1
- **สาย USB Micro** × 1
- **Breadboard** × 1
- **สาย Jumper** (ผู้-ผู้, ผู้-เมีย) × หลายเส้น
- **แหล่งจ่ายไฟ 5V/2A** × 1

### อุปกรณ์เฉพาะระบบ

#### สำหรับระบบรดน้ำ
- **RTC DS3231 Module** × 1
- **4-Channel Relay Module** × 1
- **Water Pump 12V** × 1-4
- **หลอด LED** × 1 (สถานะ)
- **ตัวต้านทาน 220Ω** × 1

#### สำหรับระบบมะเขือเทศ
- **DHT22 Temperature/Humidity Sensor** × 1
- **Soil Moisture Sensor** × 1
- **Single Relay Module** × 1

#### สำหรับระบบให้อาหารนก
- **RTC DS3231 Module** × 1
- **Servo Motor SG90** × 1
- **Single Relay Module** × 1

#### สำหรับระบบเซ็นเซอร์ขั้นสูง
- **เซ็นเซอร์ความชื้นดิน** × 4
- **เซ็นเซอร์อุณหภูมิ** × 1

---

## 💻 การติดตั้ง Arduino IDE

### ขั้นตอนที่ 1: ดาวน์โหลด Arduino IDE
1. ไปที่ https://www.arduino.cc/en/software
2. ดาวน์โหลดเวอร์ชันล่าสุดสำหรับระบบปฏิบัติการของคุณ
3. ติดตั้งตามขั้นตอนปกติ

### ขั้นตอนที่ 2: เพิ่ม ESP32 Board Support
1. เปิด Arduino IDE
2. ไปที่ `File > Preferences`
3. ในช่อง "Additional Board Manager URLs" ใส่:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. กด OK
5. ไปที่ `Tools > Board > Boards Manager`
6. ค้นหา "ESP32" และติดตั้ง "esp32 by Espressif Systems"

### ขั้นตอนที่ 3: เลือก Board
1. ไปที่ `Tools > Board > ESP32 Arduino`
2. เลือก "ESP32 Dev Module"

### ขั้นตอนที่ 4: ตั้งค่า Board
- **Upload Speed**: 115200
- **CPU Frequency**: 240MHz
- **Flash Frequency**: 80MHz
- **Flash Mode**: QIO
- **Flash Size**: 4MB (32Mb)
- **Partition Scheme**: Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)

---

## 📚 การติดตั้ง Libraries

### วิธีที่ 1: ผ่าน Library Manager (แนะนำ)

1. เปิด Arduino IDE
2. ไปที่ `Tools > Manage Libraries`
3. ค้นหาและติดตั้ง libraries ต่อไปนี้:

#### Libraries ที่จำเป็นสำหรับทุกระบบ:
- **RTClib** by Adafruit (เวอร์ชัน 2.1.1+)
- **ArduinoJson** by Benoit Blanchon (เวอร์ชัน 6.21.3+)

#### Libraries เพิ่มเติมตามระบบ:
- **DHT sensor library** by Adafruit (สำหรับระบบมะเขือเทศ)
- **ESP32Servo** by Kevin Harrington (สำหรับระบบให้อาหารนก)
- **HTTPClient** (built-in กับ ESP32)

### วิธีที่ 2: ติดตั้งแบบ Manual

หากติดตั้งผ่าน Library Manager ไม่ได้:

1. ดาวน์โหลด libraries จาก GitHub:
   - RTClib: https://github.com/adafruit/RTClib
   - ArduinoJson: https://github.com/bblanchon/ArduinoJson
   - DHT: https://github.com/adafruit/DHT-sensor-library
   - ESP32Servo: https://github.com/madhephaestus/ESP32Servo

2. แตกไฟล์ไปยังโฟลเดอร์ libraries:
   - **Windows**: `Documents\Arduino\libraries\`
   - **Mac**: `~/Documents/Arduino/libraries/`
   - **Linux**: `~/Arduino/libraries/`

3. รีสตาร์ท Arduino IDE

---

## 🔌 การเชื่อมต่ออุปกรณ์

### การเชื่อมต่อพื้นฐาน (ทุกระบบ)

```
ESP32 Pin    Function         Connection
---------    --------         ----------
3.3V      -> Power          -> VCC (RTC, Sensors)
GND       -> Ground         -> GND (All modules)
GPIO21    -> I2C SDA        -> SDA (RTC)
GPIO22    -> I2C SCL        -> SCL (RTC)
GPIO2     -> Status LED     -> LED + (ผ่านตัวต้านทาน 220Ω)
```

### ระบบรดน้ำพื้นฐาน + กุหลาบ + เห็ด

```
ESP32 Pin    4-Channel Relay   Water Pump
---------    ---------------   ----------
GPIO5     -> IN1           -> Pump 1
GPIO18    -> IN2           -> Pump 2
GPIO19    -> IN3           -> Pump 3
GPIO21    -> IN4           -> Pump 4
5V        -> VCC           -> Power Supply +
GND       -> GND           -> Power Supply -
```

### ระบบมะเขือเทศ

```
ESP32 Pin    DHT22    Soil Moisture    Relay
---------    -----    -------------    -----
GPIO4     -> Data    -> -             -> -
GPIO36    -> -       -> Signal        -> -
GPIO5     -> -       -> -             -> IN
3.3V      -> VCC     -> VCC           -> VCC
GND       -> GND     -> GND           -> GND
```

### ระบบให้อาหารนก

```
ESP32 Pin    Servo SG90    Relay (Water)
---------    ----------    -------------
GPIO13    -> Signal      -> -
GPIO5     -> -           -> IN
5V        -> VCC (Red)   -> VCC
GND       -> GND (Brown) -> GND
```

---

## 📤 การอัพโหลดโค้ด

### ขั้นตอนที่ 1: เตรียมโค้ด
1. เปิดไฟล์ `.ino` ที่ต้องการใช้งาน
2. แก้ไขการตั้งค่า WiFi:
   ```cpp
   const char* ssid = "ชื่อ_WiFi_ของคุณ";
   const char* password = "รหัสผ่าน_WiFi_ของคุณ";
   ```

### ขั้นตอนที่ 2: เชื่อมต่อ ESP32
1. เสียบสาย USB เชื่อมต่อ ESP32 กับคอมพิวเตอร์
2. เลือก Port ใน `Tools > Port`
   - **Windows**: COM3, COM4, ฯลฯ
   - **Mac/Linux**: /dev/ttyUSB0, /dev/cu.usbserial, ฯลฯ

### ขั้นตอนที่ 3: Compile และ Upload
1. กด `Verify` (✓) เพื่อตรวจสอบโค้ด
2. กด `Upload` (→) เพื่ออัพโหลดโค้ด
3. รอให้อัพโหลดเสร็จ (ดูที่ Progress Bar)

### ขั้นตอนที่ 4: ตรวจสอบการทำงาน
1. เปิด Serial Monitor (`Tools > Serial Monitor`)
2. ตั้งค่า Baud Rate เป็น 115200
3. ดูข้อความการเชื่อมต่อ WiFi และ IP Address

---

## 📶 การตั้งค่า WiFi

### การแก้ไขข้อมูล WiFi ในโค้ด
ในทุกไฟล์ `.ino` ให้แก้ไขบรรทัดเหล่านี้:

```cpp
const char* ssid = "YOUR_WIFI_SSID";         // แก้เป็นชื่อ WiFi
const char* password = "YOUR_WIFI_PASSWORD"; // แก้เป็นรหัสผ่าน WiFi
```

### ข้อกำหนดของ WiFi
- ใช้ได้เฉพาะ **2.4GHz** (ESP32 ไม่รองรับ 5GHz)
- รองรับ WPA/WPA2 Security
- ต้องมีการเชื่อมต่ออินเทอร์เน็ต (สำหรับ Webhook และ Blink)

### การตรวจสอบการเชื่อมต่อ
หลังอัพโหลดโค้ด ใน Serial Monitor จะแสดง:
```
WiFi connected!
IP address: 192.168.1.xxx
Web server started
```

---

## 🎮 การใช้งานแต่ละระบบ

### 1. ระบบรดน้ำพื้นฐาน
**การเข้าถึง**: เปิดเว็บเบราว์เซอร์ไปที่ IP ของ ESP32

**หน้าเว็บควบคุม**:
- ดูสถานะปัจจุบัน
- เปิด/ปิด Relay แต่ละช่อง
- ตั้งเวลารดน้ำ
- ดูประวัติการรดน้ำ

**การตั้งเวลา**:
```cpp
// แก้ไขในโค้ด - ตัวอย่างรดน้ำเวลา 6:00 น.
WateringSchedule wateringSchedules[] = {
  {6, 0, 0, 15, true},   // 6:00 น. รดช่อง 1 เป็นเวลา 15 นาที
  {18, 0, 1, 10, true},  // 18:00 น. รดช่อง 2 เป็นเวลา 10 นาที
};
```

### 2. ระบบกุหลาบ (Rose System)
**ความพิเศษ**:
- เชื่อมต่อกับ Blink Camera
- ตรวจสอบความชื้นดิน
- เหมาะสำหรับกุหลาบ (ความชื้น 60-85%)

**การตั้งค่า Blink**:
```cpp
const char* blinkEmail = "อีเมล_Blink_ของคุณ";
const char* blinkPassword = "รหัสผ่าน_Blink";
const char* blinkAccountId = "Account_ID";
const char* blinkNetworkId = "Network_ID";
```

### 3. ระบบมะเขือเทศ
**เซ็นเซอร์**:
- DHT22: วัดอุณหภูมิและความชื้นอากาศ
- Soil Moisture: วัดความชื้นดิน

**การใช้งาน**:
- ระบบจะรดน้ำอัตโนมัติเมื่อดินแห้ง
- จำกัดการรดน้ำไม่เกิน 8 ครั้งต่อวัน
- ดูข้อมูลผ่านเว็บ Interface

### 4. ระบบให้อาหารนก
**การทำงาน**:
- ให้น้ำด้วย Water Pump (Relay)
- ให้อาหารด้วย Servo Motor
- ตั้งเวลาได้ตามต้องการ

**การตั้งเวลา**:
```cpp
// เวลาให้อาหาร (ชั่วโมง, นาที)
int feedingTimes[][2] = {
  {7, 0},   // 7:00 น.
  {12, 0},  // 12:00 น.
  {18, 0}   // 18:00 น.
};
```

### 5. ระบบเห็ด
**ความพิเศษ**:
- ควบคุมความชื้นสูง (70-95%)
- เหมาะสำหรับเห็ดนางฟ้า
- ส่งข้อมูลผ่าน Webhook

**การตั้งค่า Webhook**:
```cpp
const char* webhookUrl = "https://your-webhook-url.com";
```

---

## 🔧 การแก้ไขปัญหา

### ปัญหาการ Compile
**Error: Library not found**
- ตรวจสอบการติดตั้ง Libraries
- รีสตาร์ท Arduino IDE
- ลบ Libraries เก่าที่ซ้ำ

**Error: Board not selected**
- เลือก Board: ESP32 Dev Module
- เลือก Port ที่ถูกต้อง

### ปัญหาการ Upload
**Error: Failed to connect**
- ตรวจสอบสาย USB
- กดปุ่ม BOOT บน ESP32 ขณะ Upload
- ลองเปลี่ยน Port

**Error: Timeout**
- ลดความเร็ว Upload เป็น 115200
- ตรวจสอบ Driver ของ ESP32

### ปัญหา WiFi
**ไม่สามารถเชื่อมต่อ WiFi**
- ตรวจสอบชื่อและรหัสผ่าน WiFi
- ใช้ WiFi 2.4GHz เท่านั้น
- ตรวจสอบระยะห่างจาก Router

**หาก IP ไม่แสดง**
- รีเซ็ต ESP32
- ตรวจสอบ Serial Monitor
- ลองเปลี่ยนเครือข่าย WiFi

### ปัญหา Hardware
**RTC ไม่ทำงาน**
- ตรวจสอบการเชื่อมต่อ I2C (SDA, SCL)
- ใส่ถ่าน CR2032 ใน RTC Module
- ตรวจสอบ Address (0x68)

**Relay ไม่ทำงาน**
- ตรวจสอบการจ่ายไฟ 5V
- ตรวจสอบการเชื่อมต่อ GPIO
- ลองเปลี่ยน Relay Module

**Sensor อ่านค่าผิด**
- ตรวจสอบการเชื่อมต่อ
- ทำความสะอาด Sensor
- เปลี่ยน Sensor ใหม่

---

## 📞 การติดต่อและ Support

หากมีปัญหาเพิ่มเติม:
1. ตรวจสอบ Serial Monitor สำหรับ Error Messages
2. อ่านไฟล์ README สำหรับระบบที่ใช้งาน
3. ตรวจสอบการเชื่อมต่อ Hardware อีกครั้ง
4. ลองใช้โค้ดตัวอย่างง่ายๆ ก่อน

**Files สำหรับแต่ละระบบ**:
- `README_bird_feeding.md` - ระบบให้อาหารนก
- `README_tomato_system.md` - ระบบมะเขือเทศ  
- `README_advanced.md` - ระบบขั้นสูง
- `webhook_setup_guide.md` - การตั้งค่า Webhook
- `blink_app_setup.md` - การตั้งค่า Blink App

---

## ✅ Checklist การติดตั้ง

### ก่อนเริ่มใช้งาน
- [ ] ติดตั้ง Arduino IDE เรียบร้อย
- [ ] เพิ่ม ESP32 Board Support
- [ ] ติดตั้ง Libraries ครบถ้วน
- [ ] เตรียมอุปกรณ์ Hardware
- [ ] เชื่อมต่อวงจรตามแผนผัง

### การตั้งค่าโค้ด
- [ ] แก้ไขชื่อและรหัสผ่าน WiFi
- [ ] ตั้งค่าเวลารดน้ำ (หากต้องการ)
- [ ] ใส่ข้อมูล Webhook/Blink (หากใช้)
- [ ] ตรวจสอบ Pin assignments

### การทดสอบ
- [ ] Upload โค้ดสำเร็จ
- [ ] เชื่อมต่อ WiFi ได้
- [ ] เปิดเว็บ Interface ได้
- [ ] ทดสอบ Relay/Servo ทำงาน
- [ ] ตรวจสอบ Sensor อ่านค่าได้

---

🎉 **ยินดีด้วย! คุณพร้อมใช้งานระบบอัตโนมัติแล้ว**
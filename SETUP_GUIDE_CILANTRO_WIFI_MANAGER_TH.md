# คู่มือการติดตั้งระบบรดน้ำผักชีฟลั่งอัตโนมัติ (WiFi Manager Version)

## 🌿 ระบบปรับปรุงใหม่ - ลบเซ็นเซอร์ DHT22 + เพิ่ม WiFi Manager

### ✨ คุณสมบัติใหม่
- 📶 **WiFi Manager**: เลือกเครือข่าย WiFi ผ่าน Captive Portal (Hotspot)
- 🚫 **ลบ DHT22**: ไม่ต้องใช้เซ็นเซอร์วัดอุณหภูมิและความชื้นอากาศแล้ว
- 📷 **ตั้งค่า Blink**: กำหนดค่า Blink Camera ผ่านเว็บอินเตอร์เฟส
- 💾 **บันทึกอัตโนมัติ**: บันทึกการตั้งค่าใน EEPROM
- 🔄 **รีเซ็ต WiFi**: กดปุ่ม Boot ค้างไว้ 5 วินาทีเพื่อรีเซ็ต WiFi

---

## 📦 อุปกรณ์ที่ต้องใช้

### ✅ อุปกรณ์หลัก
- **ESP32 Development Board** (1 ตัว)
- **DS3231 RTC Module** (1 ตัว)
- **Soil Moisture Sensor** (3 ตัว)
- **Light Sensor (LDR)** (1 ตัว)
- **Relay Module 5V** (3 ตัว)
- **Water Pump 12V** (3 ตัว)
- **LED สถานะ** (1 ตัว)
- **ตัวต้านทาน 220Ω** (1 ตัว)
- **ตัวต้านทาน 10kΩ** (1 ตัว)

### ❌ อุปกรณ์ที่ไม่ต้องใช้แล้ว
- ~~DHT22 Temperature & Humidity Sensor~~
- ~~สายต่อสำหรับ DHT22~~

### 🔌 อุปกรณ์เสริม
- **Power Supply 12V** (สำหรับปั๊มน้ำ)
- **Jumper Wires**
- **Breadboard** หรือ **PCB**
- **กล่องกันน้ำ** (สำหรับ ESP32)

---

## 📚 การติดตั้ง Library

### 1. เปิด Arduino IDE

### 2. ติดตั้ง ESP32 Board Support
```
File > Preferences > Additional Board Manager URLs:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

### 3. ติดตั้ง Libraries ที่จำเป็น
ไปที่ `Tools > Manage Libraries` และค้นหาติดตั้ง:

#### ✅ Libraries ที่ต้องติดตั้ง
1. **RTClib** by Adafruit (v2.1.1+)
2. **ArduinoJson** by Benoit Blanchon (v6.21.3+)
3. **WiFiManager** by tzapu (v2.0.16-rc.2+)

#### 🔧 Built-in Libraries (ติดตั้งอัตโนมัติ)
- WiFi (ESP32)
- WebServer (ESP32)
- Wire (I2C)
- EEPROM (ESP32)

#### ❌ Libraries ที่ไม่ต้องใช้แล้ว
- ~~DHT sensor library~~

---

## 🔌 การต่อสายไฟ

### ESP32 Pin Configuration

#### 📡 Digital Pins
```
GPIO 0  ──── Boot Button (รีเซ็ต WiFi)
GPIO 2  ──── Status LED (+ ตัวต้านทาน 220Ω ต่อ GND)
GPIO 4  ──── [FREE PIN] ว่างแล้ว (เดิมเป็น DHT22)
GPIO 5  ──── Relay 1 (Zone 1)
GPIO 18 ──── Relay 2 (Zone 2)
GPIO 19 ──── Relay 3 (Zone 3)
GPIO 21 ──── Flow Sensor (ถ้ามี)
GPIO 22 ──── RTC SDA (I2C Data)
GPIO 23 ──── RTC SCL (I2C Clock)
```

#### 📊 Analog Pins
```
GPIO 34 ──── Soil Moisture Sensor 3
GPIO 35 ──── Light Sensor (LDR)
GPIO 36 ──── Soil Moisture Sensor 1
GPIO 39 ──── Soil Moisture Sensor 2
```

### 🔌 รายละเอียดการต่อสาย

#### 1. DS3231 RTC Module
```
RTC VCC ──── ESP32 3.3V
RTC GND ──── ESP32 GND
RTC SDA ──── ESP32 GPIO 22
RTC SCL ──── ESP32 GPIO 23
```

#### 2. Soil Moisture Sensors (3 ตัว)
```
Sensor 1:  VCC ──── 3.3V,  GND ──── GND,  A0 ──── GPIO 36
Sensor 2:  VCC ──── 3.3V,  GND ──── GND,  A0 ──── GPIO 39
Sensor 3:  VCC ──── 3.3V,  GND ──── GND,  A0 ──── GPIO 34
```

#### 3. Light Sensor (LDR)
```
LDR ──── GPIO 35 ──── 10kΩ Resistor ──── GND
     └──── 3.3V
```

#### 4. Relay Modules (3 ตัว)
```
Relay 1:  VCC ──── 5V,  GND ──── GND,  IN ──── GPIO 5
Relay 2:  VCC ──── 5V,  GND ──── GND,  IN ──── GPIO 18
Relay 3:  VCC ──── 5V,  GND ──── GND,  IN ──── GPIO 19
```

#### 5. Water Pumps
```
Pump 1:  (+) ──── Relay 1 NO,  (-) ──── Power Supply GND
Pump 2:  (+) ──── Relay 2 NO,  (-) ──── Power Supply GND
Pump 3:  (+) ──── Relay 3 NO,  (-) ──── Power Supply GND
```

#### 6. Status LED
```
LED Anode ──── GPIO 2 ──── 220Ω Resistor ──── GND
```

---

## 💻 การติดตั้งโค้ด

### 1. เปิด Arduino IDE
### 2. เลือก Board: `ESP32 Dev Module`
### 3. Copy โค้ดจากไฟล์ `cilantro_watering_system.ino`
### 4. Upload ไปยัง ESP32

---

## 📶 การตั้งค่า WiFi (WiFi Manager)

### 🚀 การตั้งค่าครั้งแรก

1. **Upload โค้ด** ไปยัง ESP32
2. **รอ ESP32 สร้าง Hotspot**
   - ชื่อ: `CilantroWatering-Setup`
   - รหัสผ่าน: `cilantro123`

3. **เชื่อมต่อ WiFi**
   - ใช้มือถือหรือคอมพิวเตอร์เชื่อมต่อ Hotspot
   - เบราว์เซอร์จะเปิดหน้าตั้งค่าอัตโนมัติ
   - หรือไปที่ `http://192.168.4.1`

4. **เลือกเครือข่าย WiFi**
   - เลือกเครือข่าย WiFi ที่ต้องการ
   - ใส่รหัสผ่าน WiFi
   - (ทางเลือก) ใส่ข้อมูล Blink Camera และ Webhook

5. **บันทึกและรีสตาร์ท**
   - กด Save Configuration
   - ESP32 จะรีสตาร์ทและเชื่อมต่อ WiFi อัตโนมัติ

### 🔄 การรีเซ็ต WiFi

#### วิธีที่ 1: ใช้ปุ่ม Boot
1. **กดปุ่ม Boot** บน ESP32 ค้างไว้ **5 วินาที**
2. ESP32 จะรีสตาร์ทและเปิด Hotspot สำหรับตั้งค่าใหม่

#### วิธีที่ 2: ใช้เว็บอินเตอร์เฟส
1. เข้าเว็บอินเตอร์เฟสระบบ
2. กดปุ่ม **"🔄 รีเซ็ต WiFi"**
3. ยืนยันการรีเซ็ต

---

## 📷 การตั้งค่า Blink Camera

### 🌐 ตั้งค่าผ่านเว็บอินเตอร์เฟส

1. **เข้าเว็บระบบ** (ดู IP Address จาก Serial Monitor)
2. **กดปุ่ม "⚙️ ตั้งค่า Blink"**
3. **ใส่ข้อมูล Blink**:
   - Email: อีเมลบัญชี Blink
   - Password: รหัสผ่านบัญชี Blink
   - Account ID: (หาได้จาก Blink App)
   - Network ID: (หาได้จาก Blink App)
   - Camera ID: (หาได้จาก Blink App)
   - Webhook URL: (ถ้ามี)
4. **บันทึกการตั้งค่า**

### 🔍 วิธีหา Blink IDs

#### ใช้ Blink App:
1. เปิด Blink App บนมือถือ
2. ไปที่ Settings > Account
3. จดบันทึก Account ID
4. เลือก Sync Module > จดบันทึก Network ID
5. เลือกกล้อง > Settings > จดบันทึก Camera ID

---

## 🌐 การใช้งานเว็บอินเตอร์เฟส

### 📊 หน้าแสดงสถานะ
- **☀️ ระดับแสง**: แสดงค่าแสงจาก LDR (0-100%)
- **📷 Blink Status**: สถานะการเชื่อมต่อ Blink
- **📶 WiFi Signal**: ความแรงสัญญาณ WiFi
- **⏱️ System Uptime**: เวลาที่ระบบทำงาน

### 🌿 การแสดงโซน
แต่ละโซนจะแสดง:
- ชื่อโซนและสถานะ
- ความชื้นดินปัจจุบัน
- ความชื้นดินเป้าหมาย
- ระยะการเจริญเติบโต
- จำนวนครั้งที่รดน้ำวันนี้
- สถานะการรดน้ำ

### 🎛️ การควบคุม
- **💧 รดน้ำโซน 1-3**: รดน้ำด้วยตนเอง
- **📷 ทดสอบ Blink Camera**: ทดสอบการทำงานกล้อง
- **⚙️ ตั้งค่า Blink**: เปิดหน้าต่างตั้งค่า
- **🔄 รีเซ็ต WiFi**: รีเซ็ต WiFi และรีสตาร์ท
- **🚨 หยุดฉุกเฉิน**: หยุดการรดน้ำทั้งหมดทันที

---

## ⚙️ การตั้งค่าระบบ

### 🌱 พารามิเตอร์ผักชีฟลั่ง

#### ความชื้นดิน:
- **ขั้นต่ำ**: 45% (เริ่มรดน้ำ)
- **ขั้นสูง**: 75% (หยุดรดน้ำ)

#### ระดับแสง:
- **ต่ำ**: < 30% (ลดการรดน้ำ)
- **ปานกลาง**: 30-70% (ปกติ)
- **สูง**: > 70% (เพิ่มการรดน้ำ)

### ⏰ กำหนดการรดน้ำ
- **เช้า**: 6:00, 6:05, 6:10 น.
- **เย็น**: 18:00, 18:05, 18:10 น.
- **ระยะเวลา**: 2-8 นาที (ขึ้นอยู่กับโซน)

---

## 🔧 การแก้ไขปัญหา

### 📶 ปัญหา WiFi

#### ไม่เจอ Hotspot "CilantroWatering-Setup":
- ตรวจสอบการต่อสายปุ่ม Boot (GPIO 0)
- กดปุ่ม Reset บน ESP32
- ตรวจสอบว่า Upload โค้ดสำเร็จแล้ว

#### เชื่อมต่อ WiFi ไม่ได้:
- ตรวจสอบรหัสผ่าน WiFi
- ใช้เครือข่าย 2.4GHz (ESP32 ไม่รองรับ 5GHz)
- ตรวจสอบความแรงสัญญาณ

### 🌡️ ปัญหาเซ็นเซอร์

#### เซ็นเซอร์ความชื้นดินอ่านค่าไม่ได้:
- ตรวจสอบไฟ 3.3V ไปยังเซ็นเซอร์
- ตรวจสอบการต่อสาย Analog (GPIO 36, 39, 34)
- ตรวจสอบ GND ต่อครบทุกตัว

#### เซ็นเซอร์แสงอ่านค่าผิด:
- ตรวจสอบการต่อ LDR และตัวต้านทาน 10kΩ
- ตรวจสอบตำแหน่งติดตั้งเซ็นเซอร์
- ทดสอบด้วยการปิด-เปิดแสง

### ⚡ ปัญหารีเลย์

#### รีเลย์ไม่ทำงาน:
- ตรวจสอบไฟ 5V ไปยังรีเลย์
- ตรวจสอบสายสัญญาณควบคุม (GPIO 5, 18, 19)
- ทดสอบรีเลย์ด้วย Multimeter

#### ปั๊มน้ำไม่ทำงาน:
- ตรวจสอบไฟ 12V ไปยังปั๊ม
- ตรวจสอบการต่อสาย COM และ NO ของรีเลย์
- ทดสอบปั๊มแยกจากระบบ

### 🕒 ปัญหา RTC

#### เวลาไม่ถูกต้อง:
- ตรวจสอบการต่อ I2C (SDA=GPIO 22, SCL=GPIO 23)
- ตรวจสอบไฟ 3.3V ไปยัง RTC
- ใส่ถ่าน CR2032 ใน RTC Module

### 📷 ปัญหา Blink

#### เชื่อมต่อ Blink ไม่ได้:
- ตรวจสอบ Email/Password Blink
- ตรวจสอบ Account ID, Network ID, Camera ID
- ทดสอบการเชื่อมต่ออินเทอร์เน็ต

---

## 🛡️ ข้อควรระวัง

### ⚡ ความปลอดภัยไฟฟ้า
- ใช้ Power Supply ที่มีกำลังเพียงพอ
- ตรวจสอบการต่อสายให้แน่นหนา
- ใช้รีเลย์ที่มี Optocoupler
- ต่อ GND ร่วมกันทุกอุปกรณ์

### 💧 ความปลอดภัยน้ำ
- เก็บอุปกรณ์อิเล็กทรอนิกส์ให้ห่างจากน้ำ
- ใช้ข้อต่อกันน้ำสำหรับเซ็นเซอร์กลางแจ้ง
- ติดตั้งระบบระบายน้ำที่เหมาะสม
- ใช้ GFCI สำหรับการป้องกัน

### 🔧 การป้องกันอุปกรณ์
- ใส่ตัวต้านทาน 220Ω สำหรับ LED
- ใส่ตัวต้านทาน 10kΩ สำหรับ Light Sensor
- ตรวจสอบกำลังรีเลย์ให้รองรับปั๊ม
- ใส่ Flyback Diode ข้างปั๊มมอเตอร์

---

## 🚀 การปรับปรุงระบบ

### 📌 Pin ที่ว่างสำหรับขยายระบบ
- **GPIO 4**: ว่างแล้วหลังลบ DHT22
- **GPIO 12-17**: สำหรับเซ็นเซอร์เพิ่มเติม
- **GPIO 25-27**: สำหรับ PWM หรือ Analog

### 🌟 ไอเดียการปรับปรุง
- **เซ็นเซอร์ pH**: ติดตั้งที่ GPIO 4
- **เซ็นเซอร์ฝน**: ป้องกันการรดน้ำตอนฝนตก
- **เซ็นเซอร์ระดับน้ำ**: ตรวจสอบน้ำในถัง
- **ระบบ PWM**: ควบคุมความเร็วปั๊ม
- **Solar Panel**: ระบบพลังงานแสงอาทิตย์
- **MQTT**: เชื่อมต่อ IoT Platform

### 🌐 ฟีเจอร์เครือข่าย
- **OTA Update**: อัปเดตโค้ดผ่านอินเทอร์เน็ต
- **Cloud Monitoring**: ติดตามผ่านระบบคลาวด์
- **Home Automation**: เชื่อมต่อระบบบ้านอัจฉริยะ
- **Mobile App**: แอปมือถือควบคุมระบบ

---

## 📞 การสนับสนุน

### 🔍 การตรวจสอบระบบ
- **Serial Monitor**: ดูข้อความสถานะระบบ
- **Web Interface**: ตรวจสอบค่าเซ็นเซอร์
- **LED Status**: สังเกตการกระพริบของ LED
- **Webhook**: รับการแจ้งเตือนผ่านอินเทอร์เน็ต

### 📋 ข้อความสำคัญใน Serial Monitor
```
✅ WiFi connected! IP: xxx.xxx.xxx.xxx
🌐 Web server started
🌿 Cilantro Watering System Ready!
💧 Started watering Zone X for Y minutes
📷 Blink camera armed/disarmed
🔄 WiFi reset requested
```

### 🆘 ขอความช่วยเหลือ
หากพบปัญหาที่แก้ไขไม่ได้:
1. บันทึกข้อความจาก Serial Monitor
2. ถ่ายรูปการต่อสายไฟ
3. อธิบายปัญหาที่เกิดขึ้นโดยละเอียด
4. ระบุขั้นตอนที่ทำก่อนปัญหาเกิด

---

## ✅ Checklist การติดตั้ง

### 📦 เตรียมอุปกรณ์
- [ ] ESP32 Development Board
- [ ] DS3231 RTC Module  
- [ ] Soil Moisture Sensors (3 ตัว)
- [ ] Light Sensor (LDR)
- [ ] Relay Modules (3 ตัว)
- [ ] Water Pumps (3 ตัว)
- [ ] LED และตัวต้านทาน
- [ ] Power Supply 12V
- [ ] สายไฟและอุปกรณ์ต่อ

### 💻 ติดตั้งซอฟต์แวร์
- [ ] ติดตั้ง Arduino IDE
- [ ] ติดตั้ง ESP32 Board Support
- [ ] ติดตั้ง RTClib Library
- [ ] ติดตั้ง ArduinoJson Library
- [ ] ติดตั้ง WiFiManager Library
- [ ] Upload โค้ดไปยัง ESP32

### 🔌 ต่อสายไฟ
- [ ] ต่อ DS3231 RTC (I2C)
- [ ] ต่อ Soil Moisture Sensors (3 ตัว)
- [ ] ต่อ Light Sensor (LDR)
- [ ] ต่อ Relay Modules (3 ตัว)
- [ ] ต่อ Water Pumps
- [ ] ต่อ Status LED
- [ ] ต่อ Power Supply

### 📶 ตั้งค่า WiFi
- [ ] เชื่อมต่อ Hotspot "CilantroWatering-Setup"
- [ ] เลือกเครือข่าย WiFi
- [ ] ใส่รหัสผ่าน WiFi
- [ ] บันทึกการตั้งค่า
- [ ] ตรวจสอบการเชื่อมต่อ WiFi

### 📷 ตั้งค่า Blink (ทางเลือก)
- [ ] หา Account ID, Network ID, Camera ID
- [ ] เข้าเว็บอินเตอร์เฟสระบบ
- [ ] กรอกข้อมูล Blink ในหน้าตั้งค่า
- [ ] ทดสอบการเชื่อมต่อ Blink
- [ ] ทดสอบการควบคุมกล้อง

### ✅ ทดสอบระบบ
- [ ] ตรวจสอบค่าเซ็นเซอร์ในเว็บ
- [ ] ทดสอบการรดน้ำด้วยตนเอง
- [ ] ตรวจสอบการทำงานของรีเลย์
- [ ] ทดสอบการทำงานของปั๊มน้ำ
- [ ] ตรวจสอบ LED สถานะ
- [ ] ทดสอบการรีเซ็ต WiFi

---

## 🎯 สรุป

ระบบรดน้ำผักชีฟลั่งอัตโนมัติเวอร์ชันใหม่นี้:

### ✨ จุดเด่น
- **ง่ายต่อการติดตั้ง**: WiFi Manager ทำให้ตั้งค่า WiFi ง่ายขึ้น
- **ลดความซับซอน**: ลบ DHT22 ที่มีปัญหาบ่อย
- **ใช้งานง่าย**: ตั้งค่าทั้งหมดผ่านเว็บอินเตอร์เฟส
- **เสถียร**: ลดจุดเสียหายจากเซ็นเซอร์ที่ซับซอน
- **ยืดหยุ่น**: สามารถเปลี่ยน WiFi ได้ง่าย

### 🌿 เหมาะสำหรับ
- **ผักชีฟลั่ง**: ปรับแต่งเฉพาะสำหรับผักชีฟลั่ง
- **การใช้งานกลางแจ้ง**: ทนทานต่อสภาพแวดล้อม
- **ผู้เริ่มต้น**: ติดตั้งและใช้งานง่าย
- **การขยายระบบ**: มี Pin ว่างสำหรับเพิ่มเซ็นเซอร์

🌱 **ขอให้มีความสุขกับการปลูกผักชีฟลั่ง!** 🌱
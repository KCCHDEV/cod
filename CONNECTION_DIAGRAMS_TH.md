# 🔌 แผนผังการเชื่อมต่อระบบ ESP32 ทุกระบบ

## 📋 สารบัญ
1. [การเชื่อมต่อพื้นฐาน](#การเชื่อมต่อพื้นฐาน)
2. [ระบบรดน้ำพื้นฐาน](#ระบบรดน้ำพื้นฐาน)
3. [ระบบกุหลาบ](#ระบบกุหลาบ)
4. [ระบบมะเขือเทศ](#ระบบมะเขือเทศ)
5. [ระบบให้อาหารนก](#ระบบให้อาหารนก)
6. [ระบบเห็ด](#ระบบเห็ด)
7. [การเชื่อมต่อ Power Supply](#การเชื่อมต่อ-power-supply)
8. [เคล็ดลับการต่อวงจร](#เคล็ดลับการต่อวงจร)

---

## ⚡ การเชื่อมต่อพื้นฐาน

### ESP32 Pinout Reference
```
                    ESP32 DEVKIT V1
                   ┌─────────────────┐
               EN  │1             30│ GPIO23
           GPIO36  │2             29│ GPIO22 (SCL)
           GPIO39  │3             28│ GPIO1 (TX)
           GPIO34  │4             27│ GPIO3 (RX)
           GPIO35  │5             26│ GPIO21 (SDA)
           GPIO32  │6             25│ GPIO19
           GPIO33  │7             24│ GPIO18
           GPIO25  │8             23│ GPIO5
           GPIO26  │9             22│ GPIO17
           GPIO27  │10            21│ GPIO16
           GPIO14  │11            20│ GPIO4
           GPIO12  │12            19│ GPIO2 (LED)
              GND  │13            18│ GPIO15
              VIN  │14            17│ GND
                   │15            16│ 3.3V
                   └─────────────────┘
```

### การจ่ายไฟพื้นฐาน
```
Power Supply 5V/2A
       │
       ├─── ESP32 VIN (5V)
       ├─── Relay Module VCC (5V)
       ├─── RTC DS3231 VCC (3.3V) ←── ESP32 3.3V
       ├─── Servo VCC (5V)
       └─── GND ←→ ESP32 GND ←→ All Modules GND
```

---

## 🌱 ระบบรดน้ำพื้นฐาน

### อุปกรณ์ที่ใช้
- ESP32 Dev Board
- RTC DS3231 Module
- 4-Channel Relay Module
- Water Pump 12V (×4)
- LED Status
- Power Supply 12V/2A

### แผนผังการเชื่อมต่อ
```
┌─────────────┐    ┌─────────────┐    ┌─────────────────────┐
│    ESP32    │    │   RTC       │    │    4-Ch Relay       │
│             │    │   DS3231    │    │                     │
│        3.3V ├────┤ VCC         │    │              VCC ←──┼── 5V Supply
│         GND ├────┤ GND         │    │              GND ←──┼── GND Supply
│   GPIO21/SDA├────┤ SDA         │    │                     │
│   GPIO22/SCL├────┤ SCL         │    │             IN1 ←───┤ GPIO5
│             │    └─────────────┘    │             IN2 ←───┤ GPIO18
│       GPIO5 ├─────────────────────→ │             IN3 ←───┤ GPIO19
│      GPIO18 ├─────────────────────→ │             IN4 ←───┤ GPIO21
│      GPIO19 ├─────────────────────→ │                     │
│      GPIO21 ├─────────────────────→ │                     │
│             │                       └─────────────────────┘
│       GPIO2 ├─── LED ──── 220Ω ──── GND
│         GND ├─────────────────────────────── Common GND
│         VIN ├─────────────────────────────── 5V Supply
└─────────────┘

Water Pumps Connection:
Relay CH1 ═══ Pump 1 (12V) ═══ 12V Power Supply
Relay CH2 ═══ Pump 2 (12V) ═══ 12V Power Supply  
Relay CH3 ═══ Pump 3 (12V) ═══ 12V Power Supply
Relay CH4 ═══ Pump 4 (12V) ═══ 12V Power Supply
```

### การเชื่อมต่อ Water Pump แบบละเอียด
```
12V Power Supply
      (+)     (-)
       │       │
       │       └── All Pump (-) terminals
       │
   ┌───┴───┐
   │Relay  │
   │ NO  C │ 
   │  │  │ │
   └──┼──┼─┘
      │  │
   ┌──┴──┴──┐
   │ Pump 1 │ 12V
   │   (+)  │
   └────────┘
```

---

## 🌹 ระบบกุหลาบ

### อุปกรณ์เพิ่มเติม
- Soil Moisture Sensors (×4)
- DHT22 (optional)

### แผนผังการเชื่อมต่อ
```
┌─────────────┐    ┌─────────────┐    ┌─────────────────────┐
│    ESP32    │    │   RTC       │    │    4-Ch Relay       │
│             │    │   DS3231    │    │                     │
│        3.3V ├────┤ VCC         │    │              VCC ←──┼── 5V
│         GND ├────┤ GND         │    │              GND ←──┼── GND
│   GPIO21/SDA├────┤ SDA         │    │                     │
│   GPIO22/SCL├────┤ SCL         │    │             IN1 ←───┤ GPIO5
│             │    └─────────────┘    │             IN2 ←───┤ GPIO18
│       GPIO5 ├─────────────────────→ │             IN3 ←───┤ GPIO19
│      GPIO18 ├─────────────────────→ │             IN4 ←───┤ GPIO21
│      GPIO19 ├─────────────────────→ │                     │
│      GPIO21 ├─────────────────────→ └─────────────────────┘
│             │
│      GPIO36 ├──── Soil Sensor 1
│      GPIO39 ├──── Soil Sensor 2  
│      GPIO34 ├──── Soil Sensor 3
│      GPIO35 ├──── Soil Sensor 4
│             │
│       GPIO4 ├──── DHT22 Data Pin
└─────────────┘

Soil Moisture Sensors:
Sensor 1,2,3,4 
┌─────────────┐
│   VCC ──────┼── 3.3V
│   GND ──────┼── GND  
│   AO  ──────┼── GPIO36/39/34/35
└─────────────┘
```

---

## 🍅 ระบบมะเขือเทศ

### อุปกรณ์หลัก
- ESP32
- DHT22 Sensor
- Soil Moisture Sensor
- Single Relay Module

### แผนผังการเชื่อมต่อ
```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│    ESP32    │    │    DHT22    │    │  1-Ch Relay │
│             │    │             │    │             │
│        3.3V ├────┤ VCC (Pin 1) │    │      VCC ←──┼── 5V
│         GND ├────┤ GND (Pin 4) │    │      GND ←──┼── GND
│       GPIO4 ├────┤ Data(Pin 2) │    │       IN ←──┤ GPIO5
│             │    └─────────────┘    └─────────────┘
│      GPIO36 ├──── Soil Moisture AO
│             │
│       GPIO2 ├─── LED Status ──── 220Ω ──── GND
└─────────────┘

Soil Moisture Sensor:
┌─────────────┐
│   VCC ──────┼── 3.3V (ESP32)
│   GND ──────┼── GND (ESP32)
│   AO  ──────┼── GPIO36 (ESP32)
│   DO  ──────┼── (ไม่ใช้)
└─────────────┘

Water Pump Connection:
12V Supply ── Relay NO ── Pump (+)
GND Supply ──────────── Pump (-)
```

---

## 🐦 ระบบให้อาหารนก

### อุปกรณ์หลัก
- ESP32
- RTC DS3231
- Servo Motor SG90
- Single Relay (Water Pump)

### แผนผังการเชื่อมต่อ
```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│    ESP32    │    │   RTC       │    │  Servo SG90 │
│             │    │   DS3231    │    │             │
│        3.3V ├────┤ VCC         │    │  VCC(Red) ←─┼── 5V
│         GND ├────┤ GND         │    │  GND(Brown)─┼── GND
│   GPIO21/SDA├────┤ SDA         │    │  Signal ────┤ GPIO13
│   GPIO22/SCL├────┤ SCL         │    │  (Orange)   │
│             │    └─────────────┘    └─────────────┘
│      GPIO13 ├────────────────────────────┘
│             │
│       GPIO5 ├──── Relay IN (Water Pump)
│             │
│       GPIO2 ├─── LED Status ──── 220Ω ──── GND
└─────────────┘

┌─────────────┐
│  1-Ch Relay │
│      VCC ←──┼── 5V Supply
│      GND ←──┼── GND Supply  
│       IN ←──┤ GPIO5 (ESP32)
│             │
│   NO ───────┼── Water Pump (+)
│   COM ──────┼── 12V Supply (+)
└─────────────┘

Food Dispenser Mechanism:
Servo Horn ──── Food Container Lid
             ╭─ 0° = Closed
             ╰─ 90° = Open
```

---

## 🍄 ระบบเห็ด

### อุปกรณ์หลัก
- ESP32
- RTC DS3231  
- 4-Channel Relay
- Humidity Sensors (×4)

### แผนผังการเชื่อมต่อ
```
┌─────────────┐    ┌─────────────┐    ┌─────────────────────┐
│    ESP32    │    │   RTC       │    │    4-Ch Relay       │
│             │    │   DS3231    │    │    (Misting)        │
│        3.3V ├────┤ VCC         │    │              VCC ←──┼── 5V
│         GND ├────┤ GND         │    │              GND ←──┼── GND
│   GPIO21/SDA├────┤ SDA         │    │                     │
│   GPIO22/SCL├────┤ SCL         │    │             IN1 ←───┤ GPIO5
│             │    └─────────────┘    │             IN2 ←───┤ GPIO18
│       GPIO5 ├─────────────────────→ │             IN3 ←───┤ GPIO19
│      GPIO18 ├─────────────────────→ │             IN4 ←───┤ GPIO21
│      GPIO19 ├─────────────────────→ │                     │
│      GPIO21 ├─────────────────────→ └─────────────────────┘
│             │
│      GPIO36 ├──── Humidity Sensor 1
│      GPIO39 ├──── Humidity Sensor 2
│      GPIO34 ├──── Humidity Sensor 3  
│      GPIO35 ├──── Humidity Sensor 4
└─────────────┘

Misting System per Channel:
Relay CH1 ═══ Misting Pump 1 ═══ Water Tank
Relay CH2 ═══ Misting Pump 2 ═══ Water Tank
Relay CH3 ═══ Misting Pump 3 ═══ Water Tank 
Relay CH4 ═══ Misting Pump 4 ═══ Water Tank
```

---

## ⚡ การเชื่อมต่อ Power Supply

### Power Supply Requirements
```
System Type          Voltage    Current    Power Supply
─────────────────────────────────────────────────────
Basic Watering       12V        2A         12V/2A
Tomato System        5V         1A         5V/1A
Bird Feeding         12V        1A         12V/1A  
Rose System          12V        2A         12V/2A
Mushroom System      12V        2A         12V/2A
```

### Master Power Distribution
```
AC 220V Input
      │
   ┌──▼──┐
   │ PSU │ 12V/5A
   └──┬──┘
      │
   ┌──▼──┐
   │Buck │ 5V/2A (for ESP32 & Relay Logic)
   └──┬──┘
      │
      ├── ESP32 VIN (5V)
      ├── Relay Modules VCC (5V)
      ├── Servo VCC (5V)
      └── Sensor VCC (3.3V via ESP32)

12V Rail:
      ├── Water Pumps (12V)
      └── Additional Loads
```

### Power Supply Safety
```
┌─────────┐    ┌─────────┐    ┌─────────┐
│   Fuse  │────│ Switch  │────│   PSU   │
│   3A    │    │  SPST   │    │ 12V/2A  │
└─────────┘    └─────────┘    └─────────┘
                                   │
                            ┌──────▼──────┐
                            │  Protection │
                            │   Diodes    │
                            └──────┬──────┘
                                   │
                            ┌──────▼──────┐
                            │ Capacitors  │
                            │ 1000µF/25V  │
                            └─────────────┘
```

---

## 🔧 เคล็ดลับการต่อวงจร

### ข้อควรระวัง
1. **ตรวจสอบ Polarity**: VCC (+), GND (-)
2. **ใช้ Common Ground**: เชื่อมต่อ GND ทุกตัวเข้าด้วยกัน
3. **แยก Logic/Power**: ใช้ 3.3V สำหรับ Sensor, 5V สำหรับ Logic, 12V สำหรับ Motor
4. **ป้องกัน Back EMF**: ใส่ Diode ขนานกับ Motor/Relay Coil

### สี Jumper Wire Standard
```
สีแดง    = VCC/Power (+)
สีดำ     = GND (-)  
สีเหลือง  = Signal/Data
สีเขียว   = I2C SDA
สีฟ้า     = I2C SCL
สีขาว    = UART TX
สีส้ม    = UART RX
สีม่วง   = PWM/Analog
```

### การตรวจสอบวงจร
```
ขั้นตอนที่ 1: ตรวจสอบไฟ
├── วัด VCC = 3.3V/5V
├── วัด GND = 0V
└── วัด VCC-GND = 3.3V/5V

ขั้นตอนที่ 2: ตรวจสอบ Signal
├── GPIO Output = 3.3V (HIGH)
├── GPIO Output = 0V (LOW)  
└── GPIO Input = อ่านค่าได้

ขั้นตอนที่ 3: ตรวจสอบ I2C
├── SDA/SCL = Pull-up 4.7kΩ
├── Address Scan = พบ Device
└── Read/Write = ทำงานปกติ
```

### Breadboard Layout Tips
```
Power Rails (ด้านบน/ล่าง):
Red Line   = VCC (3.3V/5V)  
Blue Line  = GND

Component Placement:
ESP32      = กลาง Breadboard
Sensors    = ด้านซ้าย
Actuators  = ด้านขวา  
Power      = ด้านบน

Connection Priority:
1. Power (VCC/GND) ก่อน
2. I2C (SDA/SCL) 
3. GPIO Signals
4. Test & Debug
```

### Debugging Tools
```
Multimeter:
├── DC Voltage (VCC/GND)
├── Continuity (Connection)
└── Current (Load Test)

Oscilloscope:
├── Signal Quality
├── Timing Issues  
└── Noise Detection

Logic Analyzer:
├── I2C Communication
├── SPI Signals
└── Digital Protocols
```

---

## 📝 Checklist การเชื่อมต่อ

### Pre-Connection
- [ ] อ่านคู่มือ Component ทุกตัว
- [ ] ตรวจสอบ Voltage Requirements
- [ ] เตรียม Jumper Wires เพียงพอ
- [ ] วางแผน Layout บน Breadboard

### Connection Phase  
- [ ] เชื่อมต่อ Power (VCC/GND) ก่อน
- [ ] ตรวจสอบ Polarity ทุกตัว
- [ ] เชื่อมต่อ I2C Bus (SDA/SCL)
- [ ] เชื่อมต่อ GPIO Signals

### Testing Phase
- [ ] วัด Voltage ทุกจุด
- [ ] Upload Test Code
- [ ] ตรวจสอบ Serial Monitor  
- [ ] ทดสอบ Component ทีละตัว

### Final Check
- [ ] ระบบทำงานตามที่ออกแบบ
- [ ] ไม่มี Short Circuit
- [ ] สาย Wire ไม่หลวม
- [ ] ความปลอดภัยของ User

---

🔌 **การเชื่อมต่อที่ถูกต้องคือพื้นฐานของระบบที่ทำงานได้ดี!**
# 🌸 คู่มือการตั้งค่า Blynk สำหรับระบบรดน้ำกล้วยไม้

## 📱 การตั้งค่า Blynk App

### 1. การติดตั้ง Blynk App
1. ดาวน์โหลด **Blynk IoT** จาก App Store หรือ Google Play
2. สร้างบัญชีใหม่หรือเข้าสู่ระบบ
3. สร้าง Device ใหม่เลือก **ESP32 Dev Board**

### 2. การสร้าง Dashboard

#### 🎛️ **Widgets ที่ต้องใช้:**

**📊 Display Widgets:**
- **Value Display** (6 ตัว) - แสดงค่าความชื้นดินแต่ละโซน
- **Value Display** (2 ตัว) - แสดงอุณหภูมิและความชื้นอากาศ
- **Value Display** (1 ตัว) - แสดงระดับแสง
- **Value Display** (1 ตัว) - แสดงค่า pH
- **Value Display** (1 ตัว) - แสดงค่า EC
- **Value Display** (1 ตัว) - แสดงระดับน้ำ
- **Value Display** (1 ตัว) - แสดงอัตราการไหล

**🎮 Control Widgets:**
- **Button** (6 ตัว) - ควบคุมการรดน้ำแต่ละโซน
- **Button** (1 ตัว) - เปิด/ปิดระบบทั้งหมด
- **Button** (1 ตัว) - เปิด/ปิดโหมดอัตโนมัติ
- **Slider** (1 ตัว) - ปรับความถี่การรดน้ำ

**📈 Chart Widgets:**
- **SuperChart** (1 ตัว) - แสดงกราฟความชื้นดิน
- **SuperChart** (1 ตัว) - แสดงกราฟอุณหภูมิและความชื้น

**🔔 Notification Widgets:**
- **Notification** (1 ตัว) - แจ้งเตือนเมื่อดินแห้ง
- **Notification** (1 ตัว) - แจ้งเตือนเมื่อน้ำหมด

### 3. การตั้งค่า Virtual Pins

| Widget | Virtual Pin | ข้อมูลที่ส่ง | ตัวอย่างค่า |
|--------|-------------|-------------|------------|
| Soil Moisture Zone 1 | V0 | ความชื้นดินโซน 1 | 0-100% |
| Soil Moisture Zone 2 | V1 | ความชื้นดินโซน 2 | 0-100% |
| Soil Moisture Zone 3 | V2 | ความชื้นดินโซน 3 | 0-100% |
| Soil Moisture Zone 4 | V3 | ความชื้นดินโซน 4 | 0-100% |
| Soil Moisture Zone 5 | V4 | ความชื้นดินโซน 5 | 0-100% |
| Soil Moisture Zone 6 | V5 | ความชื้นดินโซน 6 | 0-100% |
| Temperature | V6 | อุณหภูมิอากาศ | 20-35°C |
| Humidity | V7 | ความชื้นอากาศ | 40-80% |
| Light Level | V8 | ระดับแสง | 0-100% |
| pH Level | V9 | ค่า pH | 5.5-7.5 |
| EC Level | V10 | ค่า EC | 0.5-2.0 mS/cm |
| Water Level | V11 | ระดับน้ำ | 0-100% |
| Flow Rate | V12 | อัตราการไหล | 0-10 L/min |
| Water Zone 1 | V20 | ควบคุมรดน้ำโซน 1 | 0/1 |
| Water Zone 2 | V21 | ควบคุมรดน้ำโซน 2 | 0/1 |
| Water Zone 3 | V22 | ควบคุมรดน้ำโซน 3 | 0/1 |
| Water Zone 4 | V23 | ควบคุมรดน้ำโซน 4 | 0/1 |
| Water Zone 5 | V24 | ควบคุมรดน้ำโซน 5 | 0/1 |
| Water Zone 6 | V25 | ควบคุมรดน้ำโซน 6 | 0/1 |
| System Power | V30 | เปิด/ปิดระบบ | 0/1 |
| Auto Mode | V31 | โหมดอัตโนมัติ | 0/1 |
| Watering Frequency | V32 | ความถี่การรดน้ำ | 1-24 ชั่วโมง |

### 4. การตั้งค่า Widget Properties

#### **Value Display Widgets:**
- **Input/Output:** Input
- **Pin:** V0-V12 (ตามตาราง)
- **Label:** ชื่อข้อมูลที่แสดง
- **Min/Max:** 0-100 (สำหรับเปอร์เซ็นต์)
- **Decimal Places:** 1-2 ตามความเหมาะสม

#### **Button Widgets:**
- **Input/Output:** Output
- **Pin:** V20-V25, V30-V31
- **Mode:** Switch
- **Label:** ชื่อปุ่ม

#### **Slider Widget:**
- **Input/Output:** Output
- **Pin:** V32
- **Min/Max:** 1-24
- **Label:** "ความถี่การรดน้ำ (ชั่วโมง)"

### 5. การตั้งค่า Notifications

#### **🔔 การแจ้งเตือนเมื่อดินแห้ง:**
- **Trigger:** เมื่อค่า V0-V5 < 30%
- **Message:** "ดินโซน X แห้งแล้ว กรุณารดน้ำ"

#### **💧 การแจ้งเตือนเมื่อน้ำหมด:**
- **Trigger:** เมื่อค่า V11 < 20%
- **Message:** "น้ำในถังใกล้หมดแล้ว กรุณาเติมน้ำ"

### 6. การตั้งค่า Charts

#### **📊 กราฟความชื้นดิน:**
- **Data Streams:** V0, V1, V2, V3, V4, V5
- **Update Interval:** 30 วินาที
- **Chart Type:** Line Chart

#### **🌡️ กราฟอุณหภูมิและความชื้น:**
- **Data Streams:** V6, V7
- **Update Interval:** 30 วินาที
- **Chart Type:** Line Chart

### 7. การทดสอบระบบ

1. **ทดสอบการเชื่อมต่อ:**
   - เปิด Blynk App
   - ตรวจสอบว่า Device เชื่อมต่อแล้ว
   - ดูค่า Sensor ต่างๆ แสดงผล

2. **ทดสอบการควบคุม:**
   - กดปุ่มรดน้ำแต่ละโซน
   - ตรวจสอบว่าน้ำไหลออกมา
   - ทดสอบปุ่มเปิด/ปิดระบบ

3. **ทดสอบการแจ้งเตือน:**
   - จำลองสถานการณ์ดินแห้ง
   - ตรวจสอบการแจ้งเตือน

### 8. การแก้ไขปัญหา

#### **❌ ไม่สามารถเชื่อมต่อได้:**
- ตรวจสอบ WiFi credentials
- ตรวจสอบ Blynk Auth Token
- ตรวจสอบการเชื่อมต่ออินเทอร์เน็ต

#### **❌ ค่า Sensor ไม่แสดง:**
- ตรวจสอบการเชื่อมต่อ Sensor
- ตรวจสอบ Virtual Pin mapping
- ตรวจสอบโค้ดใน Arduino IDE

#### **❌ การควบคุมไม่ทำงาน:**
- ตรวจสอบ Virtual Pin สำหรับ Output
- ตรวจสอบการเชื่อมต่อ Relay
- ตรวจสอบโค้ดการควบคุม

### 9. การตั้งค่าขั้นสูง

#### **🔄 Auto Refresh:**
- ตั้งค่าให้ App รีเฟรชทุก 30 วินาที
- เปิดใช้งาน Background Refresh

#### **📱 Widget Layout:**
- จัดเรียง Widget ให้ใช้งานง่าย
- ใช้สีที่แตกต่างกันสำหรับแต่ละโซน
- เพิ่มไอคอนที่เข้าใจง่าย

#### **🔒 Security:**
- เปลี่ยน Blynk Auth Token เป็นประจำ
- ใช้ WiFi ที่ปลอดภัย
- เปิดใช้งาน Two-Factor Authentication

---

## 📞 การติดต่อ

หากมีปัญหาหรือต้องการความช่วยเหลือ:
- **Email:** support@rdtrc.com
- **Line:** @rdtrc_support
- **Facebook:** RDTRC IoT Systems

---

*คู่มือนี้จะอัพเดทเมื่อมีการปรับปรุงระบบ*


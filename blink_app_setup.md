# 📱 Blink App Setup Guide

คู่มือการตั้งค่า Blink App สำหรับควบคุมระบบให้น้ำและอาหารนก

## 🎯 Overview

Blink App เป็นแอปพลิเคชันสำหรับสร้าง automation และ IoT control ที่ใช้งานง่าย สามารถควบคุมระบบให้น้ำและอาหารนกผ่าน HTTP requests ได้

## 📱 การติดตั้ง Blink App

### 1. ดาวน์โหลด Blink App
- **iOS**: App Store - "Blink - Home Automation"
- **Android**: Google Play Store - "Blink - Home Automation"

### 2. สร้าง Account
- เปิดแอป Blink
- สร้าง account ใหม่
- ยืนยัน email

## 🔧 การตั้งค่า Blink App

### Step 1: สร้าง HTTP Request
1. เปิด Blink App
2. กดปุ่ม "+" เพื่อสร้าง automation ใหม่
3. เลือก "HTTP Request"

### Step 2: ตั้งค่าให้น้ำ
```
Name: Bird Water
URL: http://YOUR_ESP32_IP/api/blink/water
Method: POST
Headers: Content-Type: application/json
Body: (เว้นว่าง)
```

### Step 3: ตั้งค่าให้อาหาร
```
Name: Bird Food
URL: http://YOUR_ESP32_IP/api/blink/food
Method: POST
Headers: Content-Type: application/json
Body: (เว้นว่าง)
```

### Step 4: ตั้งค่าเช็คสถานะ
```
Name: Check Status
URL: http://YOUR_ESP32_IP/api/blink/status
Method: GET
Headers: (เว้นว่าง)
Body: (เว้นว่าง)
```

## ⏰ การตั้งค่า Triggers

### 1. Time-based Triggers
```
Trigger: Time
Time: 6:00 AM
Action: Bird Water + Bird Food
```

### 2. Manual Triggers
```
Trigger: Manual Button
Action: Bird Water
```

### 3. Schedule Triggers
```
Trigger: Schedule
Days: Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday
Time: 18:00
Action: Bird Water + Bird Food
```

### 4. Conditional Triggers
```
Trigger: HTTP Response
Condition: Check Status
If: daily_water < 3
Then: Bird Water
```

## 🎮 Manual Control Buttons

### สร้าง Quick Access Buttons

#### 1. Water Button
```
Name: 💧 Give Water
Type: HTTP Request
URL: http://YOUR_ESP32_IP/api/blink/water
Method: POST
Icon: 💧
Color: Blue
```

#### 2. Food Button
```
Name: 🌾 Give Food
Type: HTTP Request
URL: http://YOUR_ESP32_IP/api/blink/food
Method: POST
Icon: 🌾
Color: Orange
```

#### 3. Status Button
```
Name: 📊 Check Status
Type: HTTP Request
URL: http://YOUR_ESP32_IP/api/blink/status
Method: GET
Icon: 📊
Color: Green
```

## 🔄 Advanced Automations

### 1. Smart Watering
```
Trigger: Time (6:00 AM)
Condition: Check Status
If: daily_water < 5
Then: Bird Water
Else: Send Notification "Daily water limit reached"
```

### 2. Emergency Feeding
```
Trigger: Manual Button
Condition: Check Status
If: daily_food < 6
Then: Bird Food
Else: Send Notification "Daily food limit reached"
```

### 3. Status Monitoring
```
Trigger: Schedule (Every 2 hours)
Action: Check Status
If: water_active = true OR food_active = true
Then: Send Notification "System is active"
```

## 📊 Response Handling

### Water Response
```json
{
  "status": "water_started"
}
```

### Food Response
```json
{
  "status": "food_dispensed"
}
```

### Status Response
```json
{
  "water_active": false,
  "food_active": false,
  "system_enabled": true,
  "daily_water": 2,
  "daily_food": 3
}
```

## 🔧 Troubleshooting

### HTTP Request Failed
1. ตรวจสอบ IP address ของ ESP32
2. ตรวจสอบการเชื่อมต่อ WiFi
3. ตรวจสอบ URL format
4. ตรวจสอบ ESP32 web server

### Response Not Received
1. ตรวจสอบ network connectivity
2. ตรวจสอบ ESP32 serial monitor
3. ตรวจสอบ API endpoint
4. ตรวจสอบ JSON format

### Automation Not Triggering
1. ตรวจสอบ trigger settings
2. ตรวจสอบ time zone
3. ตรวจสอบ schedule settings
4. ตรวจสอบ app permissions

## 📱 Widget Setup

### สร้าง Home Screen Widget

#### 1. Quick Actions Widget
```
Widget Type: Quick Actions
Actions:
- 💧 Water (Bird Water)
- 🌾 Food (Bird Food)
- 📊 Status (Check Status)
```

#### 2. Status Widget
```
Widget Type: Status Display
Data Source: Check Status
Display:
- Water Count: daily_water/5
- Food Count: daily_food/6
- System Status: system_enabled
```

## 🎯 Best Practices

### 1. Error Handling
- ตั้งค่า timeout สำหรับ HTTP requests
- เพิ่ม retry logic สำหรับ failed requests
- ส่ง notifications เมื่อเกิด error

### 2. Security
- ใช้ HTTPS ถ้าเป็นไปได้
- ตั้งค่า authentication ถ้าจำเป็น
- จำกัด access จาก specific IPs

### 3. Monitoring
- สร้าง daily reports
- ตั้งค่า alerts สำหรับ system issues
- เก็บ logs ของการใช้งาน

### 4. Optimization
- ใช้ appropriate intervals สำหรับ status checks
- จำกัดจำนวน requests ต่อวัน
- ใช้ caching สำหรับ status data

## 📞 Support

หากมีปัญหาการตั้งค่า Blink App:
1. ตรวจสอบ Blink App documentation
2. ตรวจสอบ ESP32 serial monitor
3. ทดสอบ HTTP requests ด้วย Postman
4. ตรวจสอบ network connectivity

---

**สร้างโดย เนย - Senior Developer** 🌟 
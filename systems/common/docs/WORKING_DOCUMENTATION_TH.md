# 📖 เอกสารการทำงานระบบ ESP32 อัตโนมัติ

## 📋 สารบัญ
1. [ภาพรวมการทำงาน](#ภาพรวมการทำงาน)
2. [หลักการทำงานพื้นฐาน](#หลักการทำงานพื้นฐาน)
3. [ระบบรดน้ำพื้นฐาน](#ระบบรดน้ำพื้นฐาน)
4. [ระบบกุหลาบขั้นสูง](#ระบบกุหลาบขั้นสูง)
5. [ระบบมะเขือเทศ](#ระบบมะเขือเทศ)
6. [ระบบให้อาหารนก](#ระบบให้อาหารนก)
7. [ระบบปลูกเห็ด](#ระบบปลูกเห็ด)
8. [การทำงานของ Web Interface](#การทำงานของ-web-interface)
9. [ระบบเตือนภัยและแจ้งเหตุ](#ระบบเตือนภัยและแจ้งเหตุ)
10. [การบำรุงรักษา](#การบำรุงรักษา)

---

## 🎯 ภาพรวมการทำงาน

### สถาปัตยกรรมระบบ
```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Input Layer   │    │  Control Layer  │    │  Output Layer   │
│                 │    │                 │    │                 │
│ • RTC Clock     │───→│ • ESP32 MCU     │───→│ • Relays        │
│ • Sensors       │    │ • WiFi Module   │    │ • Servo Motors  │
│ • User Commands │    │ • Web Server    │    │ • LED Indicators│
│ • Schedules     │    │ • Logic Control │    │ • Notifications │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### Flow การทำงานหลัก
```
Startup → WiFi Connect → RTC Sync → Sensor Read → Decision Logic → Action Execute → Web Update → Loop
    ↑                                                                                                ↓
    └─────────────────────────────── Error Handle ←─────────────────────────────────────────────────┘
```

---

## ⚙️ หลักการทำงานพื้นฐาน

### 1. การเริ่มต้นระบบ (System Initialization)
```cpp
void setup() {
  // 1. เริ่มต้น Serial Communication
  Serial.begin(115200);
  
  // 2. ตั้งค่า GPIO Pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // 3. เริ่มต้น I2C สำหรับ RTC
  Wire.begin();
  rtc.begin();
  
  // 4. เชื่อมต่อ WiFi
  WiFi.begin(ssid, password);
  
  // 5. เริ่มต้น Web Server
  server.begin();
  
  // 6. ตั้งค่าเริ่มต้น
  initializeSystem();
}
```

### 2. Loop หลัก (Main Loop)
```cpp
void loop() {
  // 1. อ่านเวลาปัจจุบัน
  DateTime now = rtc.now();
  
  // 2. อ่านค่า Sensors
  readSensors();
  
  // 3. ตรวจสอบเงื่อนไขการทำงาน
  checkConditions(now);
  
  // 4. ประมวลผล Web Requests
  server.handleClient();
  
  // 5. อัพเดต Status
  updateStatus();
  
  // 6. หน่วงเวลา
  delay(1000);
}
```

### 3. การตัดสินใจอัตโนมัติ (Decision Logic)
```
┌─────────────┐
│  เวลา        │ ──┐
└─────────────┘   │
┌─────────────┐   │    ┌─────────────┐    ┌─────────────┐
│  Sensor     │ ──┼───→│  Decision   │───→│   Action    │
└─────────────┘   │    │   Logic     │    │  Execute    │
┌─────────────┐   │    └─────────────┘    └─────────────┘
│  Schedule   │ ──┘
└─────────────┘
```

---

## 🌱 ระบบรดน้ำพื้นฐาน

### หลักการทำงาน
ระบบรดน้ำพื้นฐานใช้ RTC (Real-Time Clock) เป็นตัวกำหนดเวลา และ Relay เป็นตัวควบคุมปั๊มน้ำ

### โครงสร้างการทำงาน
```
RTC DS3231 → ESP32 → 4-Channel Relay → Water Pumps → Plants
     ↑                    ↑
Time Schedule      User Control (Web)
```

### ขั้นตอนการทำงาน
1. **การอ่านเวลา**: อ่านเวลาจาก RTC ทุก 1 วินาที
2. **การเปรียบเทียบ**: เปรียบเทียบเวลาปัจจุบันกับตารางรดน้ำ
3. **การตัดสินใจ**: หากตรงเวลา → เปิดปั๊มน้ำ
4. **การควบคุม**: ส่งสัญญาณ HIGH/LOW ไป Relay
5. **การหยุด**: หยุดรดน้ำเมื่อครบเวลาที่กำหนด

### ตัวอย่างโค้ดหลัก
```cpp
void checkWateringSchedule(DateTime now) {
  for (int i = 0; i < scheduleCount; i++) {
    WateringSchedule* schedule = &wateringSchedules[i];
    
    if (schedule->enabled && 
        now.hour() == schedule->hour && 
        now.minute() == schedule->minute) {
      
      // เริ่มรดน้ำ
      startWatering(schedule->relayIndex, schedule->duration);
    }
  }
}

void startWatering(int relayIndex, int duration) {
  digitalWrite(relayPins[relayIndex], LOW); // เปิด Relay
  wateringEndTime[relayIndex] = millis() + (duration * 60000); // คำนวณเวลาหยุด
  relayStates[relayIndex] = true;
  wateringCount[relayIndex]++;
}
```

### การทำงานของ Web Interface
```
User Browser ←→ ESP32 Web Server
     │
     ├── GET /        → แสดงหน้าหลัก
     ├── POST /relay  → ควบคุม Relay
     ├── GET /status  → ดูสถานะ JSON
     └── POST /schedule → ตั้งค่าตารางเวลา
```

---

## 🌹 ระบบกุหลาบขั้นสูง

### ความพิเศษของระบบ
- **Moisture Monitoring**: ตรวจสอบความชื้นดิน 4 จุด
- **Blink Integration**: เชื่อมต่อกับกล้อง Blink
- **Plant Tracking**: ติดตามการเจริญเติบโตของกุหลาบ

### การทำงานแบบอัจฉริยะ
```
Sensor Reading → Moisture Analysis → Smart Decision → Watering Action
      ↓                    ↓               ↓              ↓
   ทุก 30 วิ        ตรวจสอบระดับ     รดน้ำตามความจำเป็น   บันทึกข้อมูล
```

### ตรรกะการตัดสินใจ
```cpp
void smartWateringLogic() {
  for (int i = 0; i < 4; i++) {
    int moisture = readMoisture(i);
    
    if (moisture < ROSE_MOISTURE_MIN) {
      // ดินแห้งเกินไป → รดน้ำทันที
      startWatering(i, calculateDuration(moisture));
    }
    else if (moisture > ROSE_MOISTURE_MAX) {
      // ดินเปียกเกินไป → หยุดรดน้ำ
      stopWatering(i);
    }
    
    // บันทึกข้อมูลเพื่อวิเคราะห์แนวโน้ม
    logMoistureData(i, moisture);
  }
}
```

### Blink Camera Integration
```cpp
void capturePhotoIfNeeded() {
  DateTime now = rtc.now();
  
  // ถ่ายภาพทุกวันเวลา 12:00 น.
  if (now.hour() == 12 && now.minute() == 0) {
    triggerBlinkCapture();
  }
  
  // ถ่ายภาพเมื่อเริ่มรดน้ำ
  if (wateringStarted) {
    triggerBlinkCapture();
  }
}
```

---

## 🍅 ระบบมะเขือเทศ

### เซ็นเซอร์ที่ใช้
- **DHT22**: วัดอุณหภูมิและความชื้นอากาศ
- **Soil Moisture**: วัดความชื้นดิน

### การทำงานแบบปรับตัว
```
Environmental Monitoring → Plant Needs Analysis → Adaptive Watering
         ↓                        ↓                      ↓
    ทุก 5 นาที              วิเคราะห์ความต้องการ        รดน้ำตามสภาพ
```

### เงื่อนไขการรดน้ำ
```cpp
bool shouldWater() {
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();
  int soilMoisture = analogRead(SOIL_PIN);
  
  // เงื่อนไข 1: ดินแห้ง
  if (soilMoisture < DRY_THRESHOLD) return true;
  
  // เงื่อนไข 2: อากาศร้อนและแห้ง
  if (temp > 30 && humidity < 40) return true;
  
  // เงื่อนไข 3: เวลาเช้า (6-8 น.) และดินไม่เปียก
  DateTime now = rtc.now();
  if (now.hour() >= 6 && now.hour() <= 8 && soilMoisture < OPTIMAL_MOISTURE) {
    return true;
  }
  
  return false;
}
```

### ระบบป้องกันการรดน้ำเกิน
```cpp
void wateringControl() {
  // จำกัดการรดน้ำไม่เกิน 8 ครั้งต่อวัน
  if (dailyWaterCount >= MAX_DAILY_WATER) {
    Serial.println("ถึงขด จำกัดการรดน้ำประจำวันแล้ว");
    return;
  }
  
  // ห่างการรดน้ำอย่างน้อย 2 ชั่วโมง
  if (millis() - lastWateringTime < 2 * 60 * 60 * 1000) {
    Serial.println("รดน้ำไปแล้วเมื่อไม่นานนี้");
    return;
  }
  
  // ดำเนินการรดน้ำ
  if (shouldWater()) {
    startWatering();
  }
}
```

---

## 🐦 ระบบให้อาหารนก

### องค์ประกอบหลัก
- **Water System**: ให้น้ำด้วย Relay + Water Pump
- **Food System**: ให้อาหารด้วย Servo Motor

### การทำงานของ Servo Motor
```cpp
void feedBirds() {
  Serial.println("เริ่มให้อาหารนก");
  
  // เปิดฝาภาชนะอาหาร
  foodServo.write(SERVO_OPEN_ANGLE);
  delay(2000); // เปิดไว้ 2 วินาที
  
  // ปิดฝาภาชนะอาหาร
  foodServo.write(SERVO_CLOSE_ANGLE);
  
  feedingCount++;
  lastFeedingTime = millis();
}
```

### ตารางเวลาอัตโนมัติ
```cpp
struct FeedingSchedule {
  int hour;
  int minute;
  String action; // "water" หรือ "food"
};

FeedingSchedule dailySchedule[] = {
  {6, 0, "water"},   // ให้น้ำเวลา 6:00 น.
  {6, 5, "food"},    // ให้อาหารเวลา 6:05 น.
  {12, 0, "water"},  // ให้น้ำเวลา 12:00 น.
  {12, 5, "food"},   // ให้อาหารเวลา 12:05 น.
  {18, 0, "water"},  // ให้น้ำเวลา 18:00 น.
  {18, 5, "food"}    // ให้อาหารเวลา 18:05 น.
};
```

### การติดตามการใช้งาน
```cpp
void trackUsage() {
  // นับจำนวนครั้งที่ให้อาหาร
  static int dailyFeedings = 0;
  static int lastDay = 0;
  
  DateTime now = rtc.now();
  
  // รีเซ็ตตัวนับเมื่อเริ่มวันใหม่
  if (now.day() != lastDay) {
    dailyFeedings = 0;
    lastDay = now.day();
  }
  
  // แจ้งเตือนเมื่อให้อาหารครบ 3 ครั้ง
  if (dailyFeedings >= 3) {
    Serial.println("ให้อาหารครบตามกำหนดแล้ววันนี้");
  }
}
```

---

## 🍄 ระบบปลูกเห็ด

### ความต้องการพิเศษของเห็ด
- **ความชื้นสูง**: 70-95%
- **อุณหภูมิคงที่**: 20-30°C
- **การระบายอากาศ**: ควบคุมแบบช่วงเวลา

### ระบบพ่นหมอก (Misting System)
```cpp
void controlMisting() {
  for (int bed = 0; bed < 4; bed++) {
    int humidity = readHumidity(bed);
    
    if (humidity < MUSHROOM_MOISTURE_MIN) {
      // ความชื้นต่ำ → เปิดระบบพ่นหมอก
      startMisting(bed, 30); // พ่นหมอก 30 วินาที
    }
    else if (humidity > MUSHROOM_MOISTURE_MAX) {
      // ความชื้นสูงเกิน → เปิดพัดลม
      startVentilation(bed, 60); // เปิดพัดลม 1 นาที
    }
  }
}
```

### การติดตามการเจริญเติบโต
```cpp
struct MushroomBed {
  int growthStage;    // 0=Inoculation, 1=Colonization, 2=Fruiting, 3=Harvest
  int daysInStage;    // จำนวนวันในแต่ละระยะ
  float temperature;  // อุณหภูมิปัจจุบัน
  int humidity;       // ความชื้นปัจจุบัน
  bool needsAttention;// ต้องการความสนใจพิเศษ
  String status;      // สถานะปัจจุบัน
};

void updateGrowthStage(int bed) {
  MushroomBed* mushroom = &mushroomBeds[bed];
  mushroom->daysInStage++;
  
  switch(mushroom->growthStage) {
    case 0: // Inoculation (7-14 วัน)
      if (mushroom->daysInStage >= 14) {
        mushroom->growthStage = 1;
        mushroom->daysInStage = 0;
      }
      break;
      
    case 1: // Colonization (14-21 วัน)
      if (mushroom->daysInStage >= 21) {
        mushroom->growthStage = 2;
        mushroom->daysInStage = 0;
      }
      break;
      
    case 2: // Fruiting (7-10 วัน)
      if (mushroom->daysInStage >= 10) {
        mushroom->growthStage = 3;
        mushroom->daysInStage = 0;
        mushroom->needsAttention = true;
      }
      break;
  }
}
```

---

## 🌐 การทำงานของ Web Interface

### HTML Page Generation
```cpp
String generateMainPage() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>ระบบควบคุมการรดน้ำ</title>";
  html += "<meta charset='UTF-8'>";
  html += "<style>body{font-family:Arial;margin:20px;}</style>";
  html += "</head><body>";
  
  // Header
  html += "<h1>🌱 ระบบรดน้ำอัตโนมัติ</h1>";
  
  // Status Section
  html += "<div class='status'>";
  html += "<h2>สถานะปัจจุบัน</h2>";
  html += "<p>เวลา: " + getCurrentTime() + "</p>";
  html += "<p>WiFi: " + WiFi.localIP().toString() + "</p>";
  html += "</div>";
  
  // Control Section
  html += generateControlButtons();
  
  // Schedule Section
  html += generateScheduleTable();
  
  html += "</body></html>";
  return html;
}
```

### AJAX Updates
```javascript
function updateStatus() {
  fetch('/status')
    .then(response => response.json())
    .then(data => {
      document.getElementById('moisture').textContent = data.moisture + '%';
      document.getElementById('temperature').textContent = data.temperature + '°C';
      document.getElementById('relayStates').textContent = data.relays.join(', ');
    });
}

// อัพเดตทุก 5 วินาที
setInterval(updateStatus, 5000);
```

### API Endpoints
```cpp
void handleAPIRequests() {
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/relay", HTTP_POST, handleRelayControl);
  server.on("/schedule", HTTP_POST, handleScheduleUpdate);
  server.on("/manual", HTTP_POST, handleManualControl);
}

void handleStatus() {
  DynamicJsonDocument doc(1024);
  doc["time"] = getCurrentTime();
  doc["temperature"] = dht.readTemperature();
  doc["humidity"] = dht.readHumidity();
  doc["moisture"] = analogRead(SOIL_PIN);
  
  JsonArray relays = doc.createNestedArray("relays");
  for (int i = 0; i < 4; i++) {
    relays.add(relayStates[i]);
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}
```

---

## 🚨 ระบบเตือนภัยและแจ้งเหตุ

### Webhook Notifications
```cpp
void sendWebhook(String message, String level) {
  if (!webhookEnabled) return;
  
  HTTPClient http;
  http.begin(webhookUrl);
  http.addHeader("Content-Type", "application/json");
  
  DynamicJsonDocument doc(512);
  doc["timestamp"] = getCurrentTime();
  doc["system"] = "ESP32 Watering System";
  doc["message"] = message;
  doc["level"] = level; // "info", "warning", "error"
  doc["ip"] = WiFi.localIP().toString();
  
  String payload;
  serializeJson(doc, payload);
  
  int httpCode = http.POST(payload);
  if (httpCode > 0) {
    Serial.println("Webhook sent successfully");
  }
  
  http.end();
}
```

### Alert Conditions
```cpp
void checkAlerts() {
  // เตือนเมื่อดินแห้งมาก
  if (soilMoisture < CRITICAL_MOISTURE) {
    sendWebhook("ดินแห้งมาก! ต้องรดน้ำทันที", "warning");
  }
  
  // เตือนเมื่อปั๊มน้ำทำงานนานเกินไป
  if (pumpRuntime > MAX_PUMP_RUNTIME) {
    sendWebhook("ปั๊มน้ำทำงานเกิน " + String(MAX_PUMP_RUNTIME) + " นาที", "error");
    digitalWrite(RELAY_PIN, HIGH); // ปิดปั๊มฉุกเฉิน
  }
  
  // เตือนเมื่อ WiFi ขาดการเชื่อมต่อ
  if (WiFi.status() != WL_CONNECTED) {
    sendWebhook("WiFi ขาดการเชื่อมต่อ", "warning");
    reconnectWiFi();
  }
}
```

---

## 🔧 การบำรุงรักษา

### การทำความสะอาดเซ็นเซอร์
```cpp
void cleaningSensorRoutine() {
  // ทำความสะอาด Soil Moisture Sensor ทุก 7 วัน
  static unsigned long lastCleaning = 0;
  unsigned long now = millis();
  
  if (now - lastCleaning > 7 * 24 * 60 * 60 * 1000) { // 7 วัน
    Serial.println("⚠️ เตือน: ถึงเวลาทำความสะอาดเซ็นเซอร์");
    sendWebhook("ถึงเวลาทำความสะอาดเซ็นเซอร์", "info");
    lastCleaning = now;
  }
}
```

### การสำรองข้อมูล
```cpp
void backupSettings() {
  // บันทึกการตั้งค่าลง EEPROM
  EEPROM.begin(512);
  
  int addr = 0;
  EEPROM.put(addr, wateringSchedules);
  addr += sizeof(wateringSchedules);
  
  EEPROM.put(addr, systemSettings);
  addr += sizeof(systemSettings);
  
  EEPROM.commit();
  Serial.println("✅ สำรองข้อมูลเรียบร้อย");
}

void restoreSettings() {
  // กู้คืนการตั้งค่าจาก EEPROM
  EEPROM.begin(512);
  
  int addr = 0;
  EEPROM.get(addr, wateringSchedules);
  addr += sizeof(wateringSchedules);
  
  EEPROM.get(addr, systemSettings);
  
  Serial.println("📂 กู้คืนข้อมูลเรียบร้อย");
}
```

### การตรวจสอบระบบอัตโนมัติ
```cpp
void systemHealthCheck() {
  bool healthy = true;
  
  // ตรวจสอบ RTC
  if (!rtc.isrunning()) {
    Serial.println("❌ RTC ไม่ทำงาน");
    healthy = false;
  }
  
  // ตรวจสอบ WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi ไม่เชื่อมต่อ");
    healthy = false;
  }
  
  // ตรวจสอบเซ็นเซอร์
  float temp = dht.readTemperature();
  if (isnan(temp)) {
    Serial.println("❌ DHT22 ไม่ทำงาน");
    healthy = false;
  }
  
  // รายงานผล
  if (healthy) {
    Serial.println("✅ ระบบทำงานปกติ");
  } else {
    sendWebhook("ระบบมีปัญหา กรุณาตรวจสอบ", "error");
  }
}
```

---

## 📊 การติดตามประสิทธิภาพ

### Data Logging
```cpp
void logSystemData() {
  String logEntry = getCurrentTime() + ",";
  logEntry += String(dht.readTemperature()) + ",";
  logEntry += String(dht.readHumidity()) + ",";
  logEntry += String(analogRead(SOIL_PIN)) + ",";
  
  for (int i = 0; i < 4; i++) {
    logEntry += String(relayStates[i]) + ",";
  }
  
  logEntry += String(wateringCount[0]);
  
  Serial.println("LOG: " + logEntry);
  
  // ส่งไปยัง Cloud Service (optional)
  if (cloudLoggingEnabled) {
    sendToCloud(logEntry);
  }
}
```

---

## 🎛️ การปรับแต่งขั้นสูง

### Adaptive Algorithms
```cpp
class AdaptiveWatering {
private:
  float learningRate = 0.1;
  float moistureHistory[7] = {0}; // เก็บข้อมูล 7 วัน
  
public:
  int calculateOptimalDuration(int currentMoisture) {
    // วิเคราะห์แนวโน้มความชื้น
    float trend = calculateTrend();
    
    // ปรับระยะเวลารดน้ำตามแนวโน้ม
    int baseDuration = 10; // นาที
    
    if (trend < -5) { // ความชื้นลดลงเร็ว
      return baseDuration * 1.5;
    } else if (trend > 5) { // ความชื้นเพิ่มขึ้น
      return baseDuration * 0.7;
    }
    
    return baseDuration;
  }
};
```

---

## 📱 Mobile App Integration

### REST API สำหรับ Mobile
```cpp
void setupMobileAPI() {
  // CORS Headers
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE");
  
  // Mobile-specific endpoints
  server.on("/api/v1/status", handleMobileStatus);
  server.on("/api/v1/control", HTTP_POST, handleMobileControl);
  server.on("/api/v1/schedule", HTTP_GET, handleMobileSchedule);
}
```

---

🔧 **ระบบทำงานแบบอัตโนมัติ แต่ยังสามารถควบคุมและปรับแต่งได้ตามความต้องการ!**
/*
 * ระบบให้อาหารแมวอัตโนมัติ (Automatic Cat Feeding System)
 * 
 * คุณสมบัติ (Features):
 * - ให้อาหารตามเวลาที่กำหนด (Scheduled feeding)
 * - วัดน้ำหนักอาหารที่เหลือ (Weight monitoring)
 * - ควบคุมผ่านแอพมือถือ (Mobile app control)
 * - แจ้งเตือนเมื่ออาหารหมด (Low food alerts)
 * - บันทึกประวัติการให้อาหาร (Feeding history)
 * - ระบบป้องกันการให้อาหารซ้ำ (Duplicate feeding prevention)
 * - เซ็นเซอร์ตรวจจับแมว (Cat detection sensor)
 * - กล้องถ่ายภาพขณะให้อาหาร (Camera integration)
 * 
 * ฮาร์ดแวร์ที่ใช้ (Hardware Required):
 * - ESP32 หรือ NodeMCU
 * - Servo Motor SG90
 * - Load Cell + HX711 (เซ็นเซอร์วัดน้ำหนัก)
 * - RTC DS3231 (นาฬิกาเรียลไทม์)
 * - PIR Sensor (เซ็นเซอร์ตรวจจับการเคลื่อนไหว)
 * - Buzzer (เสียงแจ้งเตือน)
 * - LED RGB (ไฟแสดงสถานะ)
 * - LCD 16x2 (แสดงข้อมูล)
 * - ESP32-CAM (กล้อง - ตัวเลือก)
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <HX711.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp32.h>
#include <TimeLib.h>
#include <EEPROM.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// การตั้งค่า WiFi และ Blynk
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* blynk_token = "YOUR_BLYNK_TOKEN";

// การตั้งค่าพิน (Pin Configuration)
#define SERVO_PIN 18
#define LOADCELL_DOUT_PIN 19
#define LOADCELL_SCK_PIN 21
#define PIR_PIN 22
#define BUZZER_PIN 23
#define LED_RED_PIN 25
#define LED_GREEN_PIN 26
#define LED_BLUE_PIN 27
#define BUTTON_MANUAL_PIN 32
#define BUTTON_RESET_PIN 33

// การตั้งค่าเซ็นเซอร์และอุปกรณ์
Servo feedingServo;
HX711 scale;
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);
WebServer server(80);

// ตัวแปรระบบ (System Variables)
struct FeedingSchedule {
  int hour;
  int minute;
  float amount; // กรัม
  bool enabled;
};

struct SystemStatus {
  float currentWeight;
  float totalFoodDispensed;
  int feedingCount;
  bool catDetected;
  bool systemActive;
  String lastFeedingTime;
  String nextFeedingTime;
};

// การตั้งค่าเริ่มต้น
FeedingSchedule schedules[6] = {
  {7, 0, 50.0, true},   // เช้า 07:00 น.
  {12, 0, 30.0, true},  // เที่ยง 12:00 น.
  {18, 0, 50.0, true},  // เย็น 18:00 น.
  {0, 0, 0.0, false},   // ตัวเลือกเพิ่มเติม
  {0, 0, 0.0, false},
  {0, 0, 0.0, false}
};

SystemStatus status;
float calibrationFactor = -7050.0; // ปรับตามเซ็นเซอร์ของคุณ
float minimumFoodWeight = 100.0; // กรัม
bool manualFeedingMode = false;
unsigned long lastFeedingTime = 0;
unsigned long feedingInterval = 3600000; // 1 ชั่วโมง (มิลลิวินาที)

// ตัวแปรสำหรับการควบคุม
bool pirState = false;
bool lastPirState = false;
unsigned long lastMotionTime = 0;
unsigned long lastWeightCheck = 0;
unsigned long lastDisplayUpdate = 0;

// ฟังก์ชันเริ่มต้น
void setup() {
  Serial.begin(115200);
  Serial.println("เริ่มต้นระบบให้อาหารแมว...");
  
  // เริ่มต้น EEPROM
  EEPROM.begin(512);
  
  // ตั้งค่าพิน
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(BUTTON_MANUAL_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RESET_PIN, INPUT_PULLUP);
  
  // เริ่มต้นอุปกรณ์
  initializeDevices();
  
  // เชื่อมต่อ WiFi
  connectWiFi();
  
  // เริ่มต้น Blynk
  Blynk.begin(blynk_token, ssid, password);
  
  // เริ่มต้น Web Server
  setupWebServer();
  
  // โหลดการตั้งค่าจาก EEPROM
  loadSettings();
  
  // แสดงข้อมูลเริ่มต้น
  displaySystemInfo();
  
  Serial.println("ระบบพร้อมใช้งาน!");
  playStartupSound();
  setLEDColor(0, 255, 0); // เขียว = พร้อมใช้งาน
}

void loop() {
  Blynk.run();
  server.handleClient();
  
  // ตรวจสอบการเคลื่อนไหวของแมว
  checkCatDetection();
  
  // ตรวจสอบน้ำหนักอาหาร
  if (millis() - lastWeightCheck > 5000) { // ทุก 5 วินาที
    checkFoodWeight();
    lastWeightCheck = millis();
  }
  
  // ตรวจสอบเวลาให้อาหาร
  checkFeedingSchedule();
  
  // ตรวจสอบปุ่มกดด้วยตนเอง
  checkManualButtons();
  
  // อัพเดทจอแสดงผล
  if (millis() - lastDisplayUpdate > 2000) { // ทุก 2 วินาที
    updateDisplay();
    lastDisplayUpdate = millis();
  }
  
  // ส่งข้อมูลไปยัง Blynk
  sendDataToBlynk();
  
  delay(100);
}

void initializeDevices() {
  // เริ่มต้น Servo
  feedingServo.attach(SERVO_PIN);
  feedingServo.write(0); // ปิดช่องให้อาหาร
  
  // เริ่มต้นเซ็นเซอร์น้ำหนัก
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibrationFactor);
  scale.tare(); // รีเซ็ตค่าเป็น 0
  
  // เริ่มต้น RTC
  if (!rtc.begin()) {
    Serial.println("ไม่พบ RTC!");
    setLEDColor(255, 0, 0); // แดง = ข้อผิดพลาด
  }
  
  if (rtc.lostPower()) {
    Serial.println("ตั้งค่าเวลา RTC ใหม่");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  // เริ่มต้น LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Cat Feeder v2.0");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  
  delay(2000);
}

void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("เชื่อมต่อ WiFi");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    lcd.setCursor(attempts % 16, 1);
    lcd.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nเชื่อมต่อ WiFi สำเร็จ!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP().toString());
  } else {
    Serial.println("\nไม่สามารถเชื่อมต่อ WiFi ได้");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed");
    setLEDColor(255, 255, 0); // เหลือง = คำเตือน
  }
  
  delay(2000);
}

void setupWebServer() {
  // หน้าแรก
  server.on("/", HTTP_GET, handleRoot);
  
  // API สำหรับข้อมูลระบบ
  server.on("/api/status", HTTP_GET, handleGetStatus);
  server.on("/api/feed", HTTP_POST, handleManualFeed);
  server.on("/api/schedule", HTTP_GET, handleGetSchedule);
  server.on("/api/schedule", HTTP_POST, handleSetSchedule);
  server.on("/api/calibrate", HTTP_POST, handleCalibrate);
  
  // เริ่มต้น server
  server.begin();
  Serial.println("Web Server เริ่มทำงานแล้ว");
}

void checkCatDetection() {
  pirState = digitalRead(PIR_PIN);
  
  if (pirState != lastPirState) {
    if (pirState == HIGH) {
      status.catDetected = true;
      lastMotionTime = millis();
      Serial.println("ตรวจพบแมว!");
      
      // ส่งแจ้งเตือนไปยัง Blynk
      Blynk.notify("ตรวจพบแมวที่ชามอาหาร!");
      
      // เปิดไฟ LED สีน้ำเงิน
      setLEDColor(0, 0, 255);
      
      // เสียงแจ้งเตือนสั้นๆ
      tone(BUZZER_PIN, 1000, 200);
    }
    lastPirState = pirState;
  }
  
  // รีเซ็ตสถานะหากไม่มีการเคลื่อนไหวเกิน 30 วินาที
  if (millis() - lastMotionTime > 30000) {
    status.catDetected = false;
    setLEDColor(0, 255, 0); // กลับเป็นสีเขียว
  }
}

void checkFoodWeight() {
  if (scale.is_ready()) {
    status.currentWeight = scale.get_units(5); // เฉลี่ย 5 ครั้ง
    
    if (status.currentWeight < 0) {
      status.currentWeight = 0;
    }
    
    // ตรวจสอบอาหารหมด
    if (status.currentWeight < minimumFoodWeight) {
      Serial.println("เตือน: อาหารเหลือน้อย!");
      
      // ส่งแจ้งเตือนไปยัง Blynk
      Blynk.notify("อาหารแมวเหลือน้อย! กรุณาเติมอาหาร");
      
      // ไฟกระพริบสีแดง
      blinkLED(255, 0, 0, 3);
      
      // เสียงเตือน
      for (int i = 0; i < 3; i++) {
        tone(BUZZER_PIN, 800, 300);
        delay(400);
      }
    }
    
    Serial.print("น้ำหนักอาหารปัจจุบัน: ");
    Serial.print(status.currentWeight);
    Serial.println(" กรัม");
  }
}

void checkFeedingSchedule() {
  DateTime now = rtc.now();
  
  for (int i = 0; i < 6; i++) {
    if (schedules[i].enabled) {
      if (now.hour() == schedules[i].hour && 
          now.minute() == schedules[i].minute &&
          now.second() == 0) {
        
        // ป้องกันการให้อาหารซ้ำในนาทีเดียวกัน
        if (millis() - lastFeedingTime > 60000) {
          Serial.print("เวลาให้อาหาร: ");
          Serial.print(schedules[i].hour);
          Serial.print(":");
          Serial.println(schedules[i].minute);
          
          dispenseFeed(schedules[i].amount);
          lastFeedingTime = millis();
          
          // บันทึกเวลาให้อาหารครั้งล่าสุด
          status.lastFeedingTime = String(now.hour()) + ":" + 
                                  String(now.minute()) + " " +
                                  String(now.day()) + "/" +
                                  String(now.month());
          
          // คำนวณเวลาให้อาหารครั้งถัดไป
          calculateNextFeedingTime();
          
          break;
        }
      }
    }
  }
}

void dispenseFeed(float amount) {
  Serial.print("กำลังให้อาหาร ");
  Serial.print(amount);
  Serial.println(" กรัม");
  
  // แสดงสถานะบน LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Feeding...");
  lcd.setCursor(0, 1);
  lcd.print(String(amount) + "g");
  
  // เสียงแจ้งเตือนก่อนให้อาหาร
  for (int i = 0; i < 2; i++) {
    tone(BUZZER_PIN, 1200, 200);
    delay(300);
  }
  
  // คำนวณเวลาที่ต้องเปิด servo (ประมาณ 1 วินาทีต่อ 10 กรัม)
  int servoTime = (amount / 10.0) * 1000;
  
  // เปิดช่องให้อาหาร
  setLEDColor(255, 165, 0); // สีส้ม = กำลังให้อาหาร
  feedingServo.write(90); // เปิดช่อง
  
  delay(servoTime);
  
  // ปิดช่องให้อาหาร
  feedingServo.write(0); // ปิดช่อง
  
  // อัพเดทสถิติ
  status.totalFoodDispensed += amount;
  status.feedingCount++;
  
  // บันทึกลง EEPROM
  saveSettings();
  
  // ส่งแจ้งเตือนไปยัง Blynk
  Blynk.notify("ให้อาหารแมวแล้ว " + String(amount) + " กรัม");
  
  // เสียงแจ้งเตือนเสร็จสิ้น
  tone(BUZZER_PIN, 1500, 500);
  
  setLEDColor(0, 255, 0); // กลับเป็นสีเขียว
  
  Serial.println("ให้อาหารเสร็จสิ้น");
}

void checkManualButtons() {
  // ปุ่มให้อาหารด้วยตนเอง
  if (digitalRead(BUTTON_MANUAL_PIN) == LOW) {
    delay(50); // debounce
    if (digitalRead(BUTTON_MANUAL_PIN) == LOW) {
      Serial.println("ให้อาหารด้วยตนเอง");
      dispenseFeed(30.0); // ให้อาหาร 30 กรัม
      
      while (digitalRead(BUTTON_MANUAL_PIN) == LOW) {
        delay(10);
      }
    }
  }
  
  // ปุ่มรีเซ็ต
  if (digitalRead(BUTTON_RESET_PIN) == LOW) {
    delay(50); // debounce
    if (digitalRead(BUTTON_RESET_PIN) == LOW) {
      Serial.println("รีเซ็ตระบบ");
      resetSystem();
      
      while (digitalRead(BUTTON_RESET_PIN) == LOW) {
        delay(10);
      }
    }
  }
}

void updateDisplay() {
  DateTime now = rtc.now();
  
  lcd.clear();
  
  // บรรทัดที่ 1: เวลาปัจจุบันและสถานะแมว
  lcd.setCursor(0, 0);
  lcd.print(String(now.hour()).substring(0, 2) + ":" + 
           String(now.minute()).substring(0, 2));
  
  if (status.catDetected) {
    lcd.print(" CAT");
  } else {
    lcd.print("    ");
  }
  
  lcd.print(" ");
  lcd.print(String(status.currentWeight, 0) + "g");
  
  // บรรทัดที่ 2: ข้อมูลการให้อาหาร
  lcd.setCursor(0, 1);
  lcd.print("Fed:" + String(status.feedingCount));
  lcd.print(" Nxt:");
  
  // แสดงเวลาให้อาหารครั้งถัดไป
  String nextTime = getNextFeedingTime();
  lcd.print(nextTime);
}

void sendDataToBlynk() {
  static unsigned long lastBlynkUpdate = 0;
  
  if (millis() - lastBlynkUpdate > 10000) { // ทุก 10 วินาที
    Blynk.virtualWrite(V0, status.currentWeight);
    Blynk.virtualWrite(V1, status.feedingCount);
    Blynk.virtualWrite(V2, status.totalFoodDispensed);
    Blynk.virtualWrite(V3, status.catDetected ? 1 : 0);
    Blynk.virtualWrite(V4, status.systemActive ? 1 : 0);
    
    lastBlynkUpdate = millis();
  }
}

// Blynk Virtual Pins
BLYNK_WRITE(V10) { // ปุ่มให้อาหารด้วยตนเอง
  if (param.asInt() == 1) {
    dispenseFeed(30.0);
  }
}

BLYNK_WRITE(V11) { // เปิด/ปิดระบบ
  status.systemActive = param.asInt();
  saveSettings();
}

BLYNK_WRITE(V12) { // ตั้งค่าปริมาณอาหาร
  float amount = param.asFloat();
  if (amount > 0 && amount <= 100) {
    dispenseFeed(amount);
  }
}

// Web Server Handlers
void handleRoot() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>ระบบให้อาหารแมว</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background: #f0f0f0; }
        .container { max-width: 800px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .status-card { background: #e8f5e8; padding: 15px; border-radius: 5px; margin: 10px 0; }
        .control-panel { background: #f0f8ff; padding: 15px; border-radius: 5px; margin: 10px 0; }
        button { background: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; margin: 5px; }
        button:hover { background: #45a049; }
        .warning { background: #fff3cd; color: #856404; padding: 10px; border-radius: 5px; margin: 10px 0; }
        .error { background: #f8d7da; color: #721c24; padding: 10px; border-radius: 5px; margin: 10px 0; }
        input, select { padding: 8px; margin: 5px; border: 1px solid #ddd; border-radius: 4px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🐱 ระบบให้อาหารแมวอัตโนมัติ</h1>
        
        <div class="status-card">
            <h3>สถานะปัจจุบัน</h3>
            <p>น้ำหนักอาหาร: <strong id="weight">-</strong> กรัม</p>
            <p>ให้อาหารไปแล้ว: <strong id="feedCount">-</strong> ครั้ง</p>
            <p>ปริมาณรวม: <strong id="totalFood">-</strong> กรัม</p>
            <p>ตรวจพบแมว: <strong id="catStatus">-</strong></p>
            <p>ครั้งล่าสุด: <strong id="lastFeeding">-</strong></p>
        </div>
        
        <div class="control-panel">
            <h3>ควบคุมระบบ</h3>
            <button onclick="manualFeed()">ให้อาหารทันที</button>
            <button onclick="refreshStatus()">รีเฟรชข้อมูล</button>
            <br><br>
            <label>ปริมาณอาหาร (กรัม):</label>
            <input type="number" id="feedAmount" value="30" min="5" max="100">
            <button onclick="customFeed()">ให้อาหารตามปริมาณ</button>
        </div>
        
        <div class="control-panel">
            <h3>ตารางให้อาหาร</h3>
            <div id="scheduleList"></div>
            <button onclick="loadSchedule()">โหลดตารางเวลา</button>
        </div>
    </div>
    
    <script>
        function refreshStatus() {
            fetch('/api/status')
            .then(response => response.json())
            .then(data => {
                document.getElementById('weight').textContent = data.weight.toFixed(1);
                document.getElementById('feedCount').textContent = data.feedingCount;
                document.getElementById('totalFood').textContent = data.totalFood.toFixed(1);
                document.getElementById('catStatus').textContent = data.catDetected ? 'มี' : 'ไม่มี';
                document.getElementById('lastFeeding').textContent = data.lastFeedingTime || 'ยังไม่มี';
            })
            .catch(error => console.error('Error:', error));
        }
        
        function manualFeed() {
            fetch('/api/feed', { method: 'POST' })
            .then(response => response.json())
            .then(data => {
                alert(data.message);
                refreshStatus();
            });
        }
        
        function customFeed() {
            const amount = document.getElementById('feedAmount').value;
            fetch('/api/feed', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ amount: parseFloat(amount) })
            })
            .then(response => response.json())
            .then(data => {
                alert(data.message);
                refreshStatus();
            });
        }
        
        function loadSchedule() {
            fetch('/api/schedule')
            .then(response => response.json())
            .then(data => {
                let html = '';
                data.schedules.forEach((schedule, index) => {
                    if (schedule.enabled) {
                        html += `<p>${schedule.hour.toString().padStart(2,'0')}:${schedule.minute.toString().padStart(2,'0')} - ${schedule.amount}g</p>`;
                    }
                });
                document.getElementById('scheduleList').innerHTML = html || '<p>ไม่มีตารางเวลาที่เปิดใช้งาน</p>';
            });
        }
        
        // โหลดข้อมูลเมื่อเริ่มต้น
        refreshStatus();
        loadSchedule();
        
        // รีเฟรชอัตโนมัติทุก 30 วินาที
        setInterval(refreshStatus, 30000);
    </script>
</body>
</html>
)";
  
  server.send(200, "text/html", html);
}

void handleGetStatus() {
  DynamicJsonDocument doc(1024);
  
  doc["weight"] = status.currentWeight;
  doc["feedingCount"] = status.feedingCount;
  doc["totalFood"] = status.totalFoodDispensed;
  doc["catDetected"] = status.catDetected;
  doc["systemActive"] = status.systemActive;
  doc["lastFeedingTime"] = status.lastFeedingTime;
  doc["nextFeedingTime"] = status.nextFeedingTime;
  
  DateTime now = rtc.now();
  doc["currentTime"] = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleManualFeed() {
  float amount = 30.0; // ค่าเริ่มต้น
  
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, server.arg("plain"));
    
    if (doc.containsKey("amount")) {
      amount = doc["amount"];
    }
  }
  
  if (amount > 0 && amount <= 100) {
    dispenseFeed(amount);
    server.send(200, "application/json", "{\"message\":\"ให้อาหารเสร็จสิ้น\",\"success\":true}");
  } else {
    server.send(400, "application/json", "{\"message\":\"ปริมาณอาหารไม่ถูกต้อง\",\"success\":false}");
  }
}

void handleGetSchedule() {
  DynamicJsonDocument doc(2048);
  JsonArray schedulesArray = doc.createNestedArray("schedules");
  
  for (int i = 0; i < 6; i++) {
    JsonObject schedule = schedulesArray.createNestedObject();
    schedule["hour"] = schedules[i].hour;
    schedule["minute"] = schedules[i].minute;
    schedule["amount"] = schedules[i].amount;
    schedule["enabled"] = schedules[i].enabled;
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleSetSchedule() {
  if (server.hasArg("plain")) {
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, server.arg("plain"));
    
    if (doc.containsKey("schedules")) {
      JsonArray schedulesArray = doc["schedules"];
      
      for (int i = 0; i < min(6, (int)schedulesArray.size()); i++) {
        JsonObject schedule = schedulesArray[i];
        schedules[i].hour = schedule["hour"];
        schedules[i].minute = schedule["minute"];
        schedules[i].amount = schedule["amount"];
        schedules[i].enabled = schedule["enabled"];
      }
      
      saveSettings();
      calculateNextFeedingTime();
      
      server.send(200, "application/json", "{\"message\":\"บันทึกตารางเวลาเสร็จสิ้น\",\"success\":true}");
    } else {
      server.send(400, "application/json", "{\"message\":\"ข้อมูลไม่ถูกต้อง\",\"success\":false}");
    }
  } else {
    server.send(400, "application/json", "{\"message\":\"ไม่พบข้อมูล\",\"success\":false}");
  }
}

void handleCalibrate() {
  scale.tare();
  server.send(200, "application/json", "{\"message\":\"ปรับเทียบเซ็นเซอร์เสร็จสิ้น\",\"success\":true}");
}

// ฟังก์ชันช่วยเหลือ
void setLEDColor(int red, int green, int blue) {
  analogWrite(LED_RED_PIN, red);
  analogWrite(LED_GREEN_PIN, green);
  analogWrite(LED_BLUE_PIN, blue);
}

void blinkLED(int red, int green, int blue, int times) {
  for (int i = 0; i < times; i++) {
    setLEDColor(red, green, blue);
    delay(200);
    setLEDColor(0, 0, 0);
    delay(200);
  }
  setLEDColor(0, 255, 0); // กลับเป็นสีเขียว
}

void playStartupSound() {
  int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
  for (int i = 0; i < 8; i++) {
    tone(BUZZER_PIN, melody[i], 200);
    delay(250);
  }
}

void calculateNextFeedingTime() {
  DateTime now = rtc.now();
  int currentMinutes = now.hour() * 60 + now.minute();
  int nextMinutes = 24 * 60; // เริ่มต้นเป็นวันถัดไป
  
  for (int i = 0; i < 6; i++) {
    if (schedules[i].enabled) {
      int scheduleMinutes = schedules[i].hour * 60 + schedules[i].minute;
      if (scheduleMinutes > currentMinutes && scheduleMinutes < nextMinutes) {
        nextMinutes = scheduleMinutes;
      }
    }
  }
  
  if (nextMinutes == 24 * 60) {
    // หาเวลาแรกของวันถัดไป
    for (int i = 0; i < 6; i++) {
      if (schedules[i].enabled) {
        int scheduleMinutes = schedules[i].hour * 60 + schedules[i].minute;
        if (scheduleMinutes < nextMinutes) {
          nextMinutes = scheduleMinutes;
        }
      }
    }
  }
  
  int nextHour = (nextMinutes / 60) % 24;
  int nextMin = nextMinutes % 60;
  
  status.nextFeedingTime = String(nextHour) + ":" + 
                          (nextMin < 10 ? "0" : "") + String(nextMin);
}

String getNextFeedingTime() {
  return status.nextFeedingTime;
}

void saveSettings() {
  // บันทึกตารางเวลา
  for (int i = 0; i < 6; i++) {
    EEPROM.put(i * sizeof(FeedingSchedule), schedules[i]);
  }
  
  // บันทึกสถิติ
  EEPROM.put(300, status.totalFoodDispensed);
  EEPROM.put(304, status.feedingCount);
  EEPROM.put(308, status.systemActive);
  
  EEPROM.commit();
}

void loadSettings() {
  // โหลดตารางเวลา
  for (int i = 0; i < 6; i++) {
    EEPROM.get(i * sizeof(FeedingSchedule), schedules[i]);
  }
  
  // โหลดสถิติ
  EEPROM.get(300, status.totalFoodDispensed);
  EEPROM.get(304, status.feedingCount);
  EEPROM.get(308, status.systemActive);
  
  // ตรวจสอบค่าที่โหลดมา
  if (isnan(status.totalFoodDispensed)) status.totalFoodDispensed = 0.0;
  if (status.feedingCount < 0 || status.feedingCount > 10000) status.feedingCount = 0;
  
  calculateNextFeedingTime();
}

void resetSystem() {
  Serial.println("รีเซ็ตระบบ...");
  
  // รีเซ็ตสถิติ
  status.totalFoodDispensed = 0.0;
  status.feedingCount = 0;
  status.lastFeedingTime = "";
  
  // รีเซ็ตเซ็นเซอร์น้ำหนัก
  scale.tare();
  
  // บันทึกการตั้งค่า
  saveSettings();
  
  // แสดงข้อความ
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Reset");
  lcd.setCursor(0, 1);
  lcd.print("Complete!");
  
  // เสียงยืนยัน
  tone(BUZZER_PIN, 1000, 500);
  delay(600);
  tone(BUZZER_PIN, 1500, 500);
  
  delay(2000);
}

void displaySystemInfo() {
  Serial.println("=== ข้อมูลระบบ ===");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("น้ำหนักอาหารปัจจุบัน: ");
  Serial.print(status.currentWeight);
  Serial.println(" กรัม");
  Serial.print("จำนวนครั้งที่ให้อาหาร: ");
  Serial.println(status.feedingCount);
  Serial.print("ปริมาณอาหารรวม: ");
  Serial.print(status.totalFoodDispensed);
  Serial.println(" กรัม");
  Serial.println("==================");
}
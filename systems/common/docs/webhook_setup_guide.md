# 🔔 Webhook Setup Guide

คู่มือการตั้งค่า Webhook สำหรับระบบแจ้งเตือน Smart Watering System

## 📋 Supported Platforms

### 1. Discord Webhook
Discord เป็นแพลตฟอร์มที่นิยมใช้สำหรับการแจ้งเตือน

#### การตั้งค่า:
1. เปิด Discord Server ของคุณ
2. ไปที่ Server Settings > Integrations > Webhooks
3. คลิก "New Webhook"
4. ตั้งชื่อ webhook (เช่น "Smart Watering System")
5. เลือก Channel ที่ต้องการให้ส่งข้อความ
6. คลิก "Copy Webhook URL"
7. นำ URL ไปใส่ในโค้ด:

```cpp
const char* webhookUrl = "https://discord.com/api/webhooks/YOUR_WEBHOOK_ID/YOUR_WEBHOOK_TOKEN";
```

#### ตัวอย่างข้อความที่ส่ง:
```json
{
  "title": "💧 Moisture Alert",
  "message": "🌱 Plant 1 moisture changed: Dry (25%)",
  "timestamp": 1640995200,
  "moisture_data": [
    {
      "plant": 1,
      "percent": 25,
      "status": "Dry"
    }
  ]
}
```

### 2. Line Notify
Line Notify เป็นบริการแจ้งเตือนผ่าน Line ที่ใช้งานง่าย

#### การตั้งค่า:
1. ไปที่ https://notify-bot.line.me/
2. คลิก "Log in" และเข้าสู่ระบบ Line
3. ไปที่ "My Page" > "Generate Token"
4. ตั้งชื่อ Token (เช่น "Smart Watering System")
5. เลือก Group หรือ Chat ที่ต้องการส่งข้อความ
6. คลิก "Generate"
7. คัดลอก Token ที่ได้

#### การใช้งานในโค้ด:
```cpp
const char* webhookUrl = "https://notify-api.line.me/api/notify";
const char* webhookToken = "YOUR_LINE_NOTIFY_TOKEN";
```

#### แก้ไขฟังก์ชัน sendWebhookNotification:
```cpp
void sendLineNotifyNotification(String title, String message) {
  if (!webhookEnabled || WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  HTTPClient http;
  http.begin(webhookUrl);
  http.addHeader("Authorization", "Bearer " + String(webhookToken));
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  String postData = "message=" + title + "\n" + message;
  
  int httpResponseCode = http.POST(postData);
  
  if (httpResponseCode > 0) {
    Serial.println("Line Notify sent successfully");
  } else {
    Serial.println("Line Notify failed: " + http.errorToString(httpResponseCode));
  }
  
  http.end();
}
```

### 3. Slack Webhook
Slack เป็นแพลตฟอร์มสำหรับทีมที่นิยมใช้

#### การตั้งค่า:
1. เปิด Slack Workspace ของคุณ
2. ไปที่ Apps > Search "Incoming Webhooks"
3. คลิก "Add to Slack"
4. เลือก Channel ที่ต้องการส่งข้อความ
5. คลิก "Add Incoming WebHooks integration"
6. คัดลอก Webhook URL

#### การใช้งาน:
```cpp
const char* webhookUrl = "https://hooks.slack.com/services/YOUR_WEBHOOK_URL";
```

### 4. Telegram Bot
Telegram Bot เป็นตัวเลือกที่ดีสำหรับการแจ้งเตือนส่วนตัว

#### การตั้งค่า:
1. สร้าง Bot ผ่าน @BotFather ใน Telegram
2. รับ Bot Token
3. เริ่มการสนทนากับ Bot ของคุณ
4. รับ Chat ID จาก https://api.telegram.org/bot<YourBOTToken>/getUpdates

#### การใช้งาน:
```cpp
const char* webhookUrl = "https://api.telegram.org/botYOUR_BOT_TOKEN/sendMessage";
const char* chatId = "YOUR_CHAT_ID";
```

#### แก้ไขฟังก์ชัน:
```cpp
void sendTelegramNotification(String title, String message) {
  if (!webhookEnabled || WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  HTTPClient http;
  String url = String(webhookUrl) + "?chat_id=" + String(chatId) + "&text=" + 
               urlEncode(title + "\n" + message) + "&parse_mode=HTML";
  
  http.begin(url);
  int httpResponseCode = http.GET();
  
  if (httpResponseCode > 0) {
    Serial.println("Telegram notification sent successfully");
  } else {
    Serial.println("Telegram failed: " + http.errorToString(httpResponseCode));
  }
  
  http.end();
}

String urlEncode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
  }
  return encodedString;
}
```

### 5. IFTTT Webhook
IFTTT (If This Then That) สามารถเชื่อมต่อกับแพลตฟอร์มอื่นๆ ได้

#### การตั้งค่า:
1. ไปที่ https://ifttt.com/
2. สร้าง Applet ใหม่
3. เลือก "Webhooks" เป็น Trigger
4. รับ Webhook URL
5. ตั้งค่า Action ตามต้องการ (Email, SMS, Push Notification, etc.)

#### การใช้งาน:
```cpp
const char* webhookUrl = "https://maker.ifttt.com/trigger/YOUR_EVENT/with/key/YOUR_KEY";
```

## 🔧 การปรับแต่งโค้ด

### 1. เปลี่ยนประเภท Webhook
แก้ไขฟังก์ชัน `sendWebhookNotification` ในโค้ด:

```cpp
void sendWebhookNotification(String title, String message) {
  // สำหรับ Discord
  sendDiscordNotification(title, message);
  
  // สำหรับ Line Notify
  sendLineNotifyNotification(title, message);
  
  // สำหรับ Telegram
  sendTelegramNotification(title, message);
}
```

### 2. ปรับแต่งข้อความ
แก้ไขข้อความที่ส่งตามต้องการ:

```cpp
// ตัวอย่างข้อความที่ปรับแต่ง
String message = "🌱 Plant " + String(i + 1) + 
                "\n💧 Moisture: " + String(moistureSensors[i].moisturePercent) + "%" +
                "\n📊 Status: " + moistureSensors[i].status +
                "\n⏰ Time: " + getCurrentTime();
```

### 3. ตั้งค่าการแจ้งเตือน
ปรับแต่งเงื่อนไขการแจ้งเตือน:

```cpp
// แจ้งเตือนเมื่อความชื้นต่ำกว่า 30%
if (moistureSensors[i].moisturePercent < 30) {
  sendWebhookNotification("⚠️ Low Moisture Alert", message);
}

// แจ้งเตือนเมื่อเริ่มรดน้ำ
if (relayStates[i] == true) {
  sendWebhookNotification("💧 Watering Started", message);
}

// แจ้งเตือนเมื่อหยุดรดน้ำ
if (relayStates[i] == false && wasWatering[i] == true) {
  sendWebhookNotification("✅ Watering Completed", message);
}
```

## 🚨 การแก้ไขปัญหา

### Webhook ไม่ทำงาน
1. ตรวจสอบ URL และ Token
2. ตรวจสอบการเชื่อมต่อ WiFi
3. ตรวจสอบ Serial Monitor สำหรับ error messages
4. ทดสอบ webhook ด้วย Postman หรือ curl

### ข้อความไม่ส่ง
1. ตรวจสอบ webhook URL format
2. ตรวจสอบ Content-Type header
3. ตรวจสอบ JSON format
4. ตรวจสอบ rate limiting ของ webhook service

### การทดสอบ Webhook
ใช้ curl command เพื่อทดสอบ:

```bash
# Discord
curl -H "Content-Type: application/json" -X POST -d '{"content":"Test message"}' YOUR_WEBHOOK_URL

# Line Notify
curl -X POST -H 'Authorization: Bearer YOUR_TOKEN' -F 'message=Test message' https://notify-api.line.me/api/notify

# Telegram
curl -X GET "https://api.telegram.org/botYOUR_BOT_TOKEN/sendMessage?chat_id=YOUR_CHAT_ID&text=Test message"
```

## 📱 ตัวอย่างการใช้งานจริง

### การแจ้งเตือนแบบครบวงจร:
```cpp
// แจ้งเตือนเมื่อระบบเริ่มทำงาน
sendWebhookNotification("🚀 System Started", "Smart Watering System is now online!");

// แจ้งเตือนเมื่อความชื้นเปลี่ยนแปลง
sendWebhookNotification("💧 Moisture Alert", "Plant 1 needs water!");

// แจ้งเตือนเมื่อเริ่มรดน้ำ
sendWebhookNotification("🌱 Watering Started", "Plant 1 is being watered for 10 minutes");

// แจ้งเตือนเมื่อหยุดรดน้ำ
sendWebhookNotification("✅ Watering Completed", "Plant 1 watering completed");

// แจ้งเตือนรายวัน
sendWebhookNotification("📊 Daily Report", "Today's watering summary: Plant 1: 2 times, Plant 2: 1 time");
```

---

**สร้างโดย เนย - Senior Developer** 🌟 
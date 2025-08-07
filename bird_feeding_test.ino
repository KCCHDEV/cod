/*
 * Bird Feeding System - Component Test Program
 * โปรแกรมทดสอบระบบให้อาหารนก
 * 
 * ทดสอบทีละส่วน:
 * 1. RTC Module
 * 2. Servo Motor
 * 3. Relay
 * 4. LED Status
 * 5. WiFi & Web Interface
 */

#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// Pin Configuration
const int SERVO_PIN = 13;
const int RELAY_PIN = 5;
const int STATUS_LED = 2;

// Components
RTC_DS3231 rtc;
Servo foodServo;
WebServer server(80);

// WiFi Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Test status
String testResults = "";

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n========================================");
  Serial.println("🐦 Bird Feeding System - Component Test");
  Serial.println("========================================\n");
  
  // Initialize pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Relay off
  digitalWrite(STATUS_LED, LOW);
  
  // Test 1: RTC Module
  testRTC();
  
  // Test 2: Servo Motor
  testServo();
  
  // Test 3: Relay
  testRelay();
  
  // Test 4: LED
  testLED();
  
  // Test 5: WiFi
  testWiFi();
  
  Serial.println("\n========================================");
  Serial.println("📋 Test Summary:");
  Serial.println(testResults);
  Serial.println("========================================");
}

void loop() {
  // Handle web interface if WiFi is connected
  if (WiFi.status() == WL_CONNECTED) {
    server.handleClient();
  }
  
  // Blink LED to show system is running
  digitalWrite(STATUS_LED, millis() % 1000 < 500);
  delay(10);
}

void testRTC() {
  Serial.println("\n1️⃣ Testing RTC Module...");
  Serial.println("   Pins: SDA→GPIO21, SCL→GPIO22");
  
  Wire.begin();
  
  if (!rtc.begin()) {
    Serial.println("   ❌ RTC NOT FOUND!");
    Serial.println("   Check connections:");
    Serial.println("   - VCC → 3.3V");
    Serial.println("   - GND → GND");
    Serial.println("   - SDA → GPIO21");
    Serial.println("   - SCL → GPIO22");
    testResults += "RTC: FAILED\n";
    return;
  }
  
  Serial.println("   ✅ RTC Found!");
  
  DateTime now = rtc.now();
  Serial.print("   Current time: ");
  Serial.print(now.year());
  Serial.print('/');
  Serial.print(now.month());
  Serial.print('/');
  Serial.print(now.day());
  Serial.print(" ");
  Serial.print(now.hour());
  Serial.print(':');
  Serial.print(now.minute());
  Serial.print(':');
  Serial.println(now.second());
  
  // Check if time needs to be set
  if (now.year() < 2020) {
    Serial.println("   ⚠️  RTC time needs to be set!");
    Serial.println("   Uncomment line: rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));");
  }
  
  testResults += "RTC: OK\n";
}

void testServo() {
  Serial.println("\n2️⃣ Testing Servo Motor...");
  Serial.println("   Pin: Signal→GPIO13");
  
  // Initialize Servo
  ESP32PWM::allocateTimer(0);
  foodServo.setPeriodHertz(50);
  
  if (!foodServo.attach(SERVO_PIN)) {
    Serial.println("   ❌ Failed to attach servo!");
    testResults += "Servo: FAILED\n";
    return;
  }
  
  Serial.println("   ✅ Servo attached!");
  
  // Test movement
  Serial.println("   Moving to 0° (closed)...");
  foodServo.write(0);
  delay(1000);
  
  Serial.println("   Moving to 90° (open)...");
  foodServo.write(90);
  delay(1000);
  
  Serial.println("   Moving to 45° (middle)...");
  foodServo.write(45);
  delay(1000);
  
  Serial.println("   Moving back to 0° (closed)...");
  foodServo.write(0);
  delay(1000);
  
  Serial.println("   ✅ Servo test complete!");
  testResults += "Servo: OK\n";
}

void testRelay() {
  Serial.println("\n3️⃣ Testing Relay Module...");
  Serial.println("   Pin: IN→GPIO5");
  
  Serial.println("   Turning relay ON (pump active)...");
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("   💧 Listen for relay click sound");
  Serial.println("   💧 Check if water pump is running");
  delay(3000);
  
  Serial.println("   Turning relay OFF (pump inactive)...");
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("   🛑 Relay should click again");
  Serial.println("   🛑 Water pump should stop");
  delay(1000);
  
  Serial.println("   ✅ Relay test complete!");
  Serial.println("   If no click sound, check:");
  Serial.println("   - VCC → 5V");
  Serial.println("   - GND → GND");
  Serial.println("   - IN → GPIO5");
  
  testResults += "Relay: CHECK MANUALLY\n";
}

void testLED() {
  Serial.println("\n4️⃣ Testing Status LED...");
  Serial.println("   Pin: GPIO2 (with 220Ω resistor)");
  
  Serial.println("   LED Blinking test...");
  for (int i = 0; i < 10; i++) {
    digitalWrite(STATUS_LED, HIGH);
    delay(200);
    digitalWrite(STATUS_LED, LOW);
    delay(200);
  }
  
  Serial.println("   ✅ LED test complete!");
  Serial.println("   If LED didn't blink, check:");
  Serial.println("   - LED+ → 3.3V");
  Serial.println("   - LED- → GPIO2 (through 220Ω resistor)");
  
  testResults += "LED: CHECK MANUALLY\n";
}

void testWiFi() {
  Serial.println("\n5️⃣ Testing WiFi Connection...");
  Serial.print("   SSID: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  Serial.print("   Connecting");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("   ✅ WiFi Connected!");
    Serial.print("   IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println("   Open browser and go to this IP address");
    
    // Setup simple test web page
    server.on("/", HTTP_GET, []() {
      String html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Bird Feeding System Test</title>
    <meta charset="UTF-8">
    <style>
        body { font-family: Arial; margin: 20px; background: #f0f0f0; }
        .container { max-width: 600px; margin: auto; background: white; padding: 20px; border-radius: 10px; }
        button { padding: 10px 20px; margin: 5px; font-size: 16px; cursor: pointer; }
        .status { padding: 10px; margin: 10px 0; background: #e0e0e0; border-radius: 5px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🐦 Bird Feeding System Test</h1>
        <div class="status" id="status">Ready for testing</div>
        
        <h3>Component Tests:</h3>
        <button onclick="testServo()">Test Servo</button>
        <button onclick="testRelay()">Test Relay</button>
        <button onclick="testLED()">Test LED</button>
        
        <h3>Manual Control:</h3>
        <button onclick="moveServo(0)">Servo 0°</button>
        <button onclick="moveServo(45)">Servo 45°</button>
        <button onclick="moveServo(90)">Servo 90°</button>
        <br>
        <button onclick="relayOn()">Relay ON</button>
        <button onclick="relayOff()">Relay OFF</button>
    </div>
    
    <script>
        function updateStatus(msg) {
            document.getElementById('status').innerText = msg;
        }
        
        function testServo() {
            updateStatus('Testing servo...');
            fetch('/test/servo').then(() => updateStatus('Servo test complete'));
        }
        
        function testRelay() {
            updateStatus('Testing relay...');
            fetch('/test/relay').then(() => updateStatus('Relay test complete'));
        }
        
        function testLED() {
            updateStatus('Testing LED...');
            fetch('/test/led').then(() => updateStatus('LED test complete'));
        }
        
        function moveServo(angle) {
            updateStatus('Moving servo to ' + angle + '°');
            fetch('/servo/' + angle);
        }
        
        function relayOn() {
            updateStatus('Relay ON');
            fetch('/relay/on');
        }
        
        function relayOff() {
            updateStatus('Relay OFF');
            fetch('/relay/off');
        }
    </script>
</body>
</html>
      )";
      server.send(200, "text/html", html);
    });
    
    // Test endpoints
    server.on("/test/servo", HTTP_GET, []() {
      foodServo.write(0);
      delay(500);
      foodServo.write(90);
      delay(500);
      foodServo.write(0);
      server.send(200, "text/plain", "OK");
    });
    
    server.on("/test/relay", HTTP_GET, []() {
      digitalWrite(RELAY_PIN, LOW);
      delay(1000);
      digitalWrite(RELAY_PIN, HIGH);
      server.send(200, "text/plain", "OK");
    });
    
    server.on("/test/led", HTTP_GET, []() {
      for (int i = 0; i < 5; i++) {
        digitalWrite(STATUS_LED, HIGH);
        delay(200);
        digitalWrite(STATUS_LED, LOW);
        delay(200);
      }
      server.send(200, "text/plain", "OK");
    });
    
    server.on("/servo/0", HTTP_GET, []() {
      foodServo.write(0);
      server.send(200, "text/plain", "OK");
    });
    
    server.on("/servo/45", HTTP_GET, []() {
      foodServo.write(45);
      server.send(200, "text/plain", "OK");
    });
    
    server.on("/servo/90", HTTP_GET, []() {
      foodServo.write(90);
      server.send(200, "text/plain", "OK");
    });
    
    server.on("/relay/on", HTTP_GET, []() {
      digitalWrite(RELAY_PIN, LOW);
      server.send(200, "text/plain", "OK");
    });
    
    server.on("/relay/off", HTTP_GET, []() {
      digitalWrite(RELAY_PIN, HIGH);
      server.send(200, "text/plain", "OK");
    });
    
    server.begin();
    testResults += "WiFi: OK\n";
  } else {
    Serial.println("   ❌ WiFi connection failed!");
    Serial.println("   Check SSID and password");
    testResults += "WiFi: FAILED\n";
  }
}
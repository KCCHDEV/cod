/*
 * Quick Test - ทดสอบอุปกรณ์ทีละชิ้น
 * ใช้ Serial Monitor เพื่อควบคุม
 * 
 * Commands:
 * 1 - Test RTC
 * 2 - Test Servo
 * 3 - Test Relay
 * 4 - Test LED
 * 5 - Move Servo to 0°
 * 6 - Move Servo to 90°
 * 7 - Turn Relay ON
 * 8 - Turn Relay OFF
 */

#include <Wire.h>
#include <RTClib.h>
#include <ESP32Servo.h>

// Pin Configuration
const int SERVO_PIN = 13;
const int RELAY_PIN = 5;
const int STATUS_LED = 2;

// Components
RTC_DS3231 rtc;
Servo foodServo;

void setup() {
  Serial.begin(115200);
  
  // Initialize pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Relay off
  digitalWrite(STATUS_LED, LOW);
  
  // Initialize I2C
  Wire.begin();
  
  // Initialize Servo
  ESP32PWM::allocateTimer(0);
  foodServo.setPeriodHertz(50);
  foodServo.attach(SERVO_PIN);
  foodServo.write(0);
  
  printMenu();
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    executeCommand(cmd);
  }
  
  // Heartbeat LED
  digitalWrite(STATUS_LED, (millis() / 500) % 2);
}

void printMenu() {
  Serial.println("\n=== 🐦 Bird Feeding System Quick Test ===");
  Serial.println("Commands:");
  Serial.println("1 - Test RTC");
  Serial.println("2 - Test Servo (0° → 90° → 0°)");
  Serial.println("3 - Test Relay (ON 2s → OFF)");
  Serial.println("4 - Test LED (Blink 5 times)");
  Serial.println("5 - Move Servo to 0° (closed)");
  Serial.println("6 - Move Servo to 90° (open)");
  Serial.println("7 - Turn Relay ON");
  Serial.println("8 - Turn Relay OFF");
  Serial.println("9 - Show this menu");
  Serial.println("=====================================\n");
}

void executeCommand(char cmd) {
  switch(cmd) {
    case '1':
      testRTC();
      break;
    case '2':
      testServo();
      break;
    case '3':
      testRelay();
      break;
    case '4':
      testLED();
      break;
    case '5':
      moveServo(0);
      break;
    case '6':
      moveServo(90);
      break;
    case '7':
      relayOn();
      break;
    case '8':
      relayOff();
      break;
    case '9':
      printMenu();
      break;
    default:
      Serial.println("❌ Invalid command!");
      break;
  }
}

void testRTC() {
  Serial.println("\n🕐 Testing RTC...");
  
  if (!rtc.begin()) {
    Serial.println("❌ RTC NOT FOUND!");
    Serial.println("Check: VCC→3.3V, GND→GND, SDA→GPIO21, SCL→GPIO22");
    return;
  }
  
  Serial.println("✅ RTC Found!");
  
  DateTime now = rtc.now();
  Serial.print("Time: ");
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
  
  if (now.year() < 2020) {
    Serial.println("⚠️  Time needs to be set!");
  }
}

void testServo() {
  Serial.println("\n🔧 Testing Servo...");
  Serial.println("Moving to 0° (closed)...");
  foodServo.write(0);
  delay(1000);
  
  Serial.println("Moving to 90° (open)...");
  foodServo.write(90);
  delay(1000);
  
  Serial.println("Moving back to 0° (closed)...");
  foodServo.write(0);
  delay(1000);
  
  Serial.println("✅ Servo test complete!");
}

void testRelay() {
  Serial.println("\n💧 Testing Relay...");
  Serial.println("Relay ON (pump active)...");
  digitalWrite(RELAY_PIN, LOW);
  delay(2000);
  
  Serial.println("Relay OFF (pump inactive)...");
  digitalWrite(RELAY_PIN, HIGH);
  delay(500);
  
  Serial.println("✅ Relay test complete!");
}

void testLED() {
  Serial.println("\n💡 Testing LED...");
  for (int i = 0; i < 5; i++) {
    digitalWrite(STATUS_LED, HIGH);
    delay(200);
    digitalWrite(STATUS_LED, LOW);
    delay(200);
  }
  Serial.println("✅ LED test complete!");
}

void moveServo(int angle) {
  Serial.print("\n🔧 Moving servo to ");
  Serial.print(angle);
  Serial.println("°...");
  foodServo.write(angle);
  Serial.println("✅ Done!");
}

void relayOn() {
  Serial.println("\n💧 Relay ON");
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("✅ Water pump should be running");
}

void relayOff() {
  Serial.println("\n🛑 Relay OFF");
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("✅ Water pump should be stopped");
}
/*
 * WiFiManager Debug Version - Cilantro Watering System
 * ใช้สำหรับทดสอบ WiFiManager Hotspot เท่านั้น
 * ไม่ต้องใช้ RTC - ระบบจะทำงานได้แม้ไม่มี RTC
 */

#include <WiFi.h>
#include <WiFiManager.h>

// Pins
const int STATUS_LED = 2;
const int WIFI_RESET_BUTTON = 0; // Boot button

WiFiManager wifiManager;
bool shouldSaveConfig = false;

// Callback function for saving WiFi config
void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

// Callback function when entering config mode
void configModeCallback(WiFiManager *myWiFiManager) {
  Serial.println("🔥 Entered config mode!");
  Serial.print("📶 AP IP address: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("🌐 AP SSID: ");
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.println("🔗 Go to: http://192.168.4.1");
  
  // Blink LED to indicate config mode
  for (int i = 0; i < 10; i++) {
    digitalWrite(STATUS_LED, HIGH);
    delay(200);
    digitalWrite(STATUS_LED, LOW);
    delay(200);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println();
  Serial.println("🌿 WiFiManager Debug Test");
  Serial.println("========================");
  
  // Initialize pins
  pinMode(STATUS_LED, OUTPUT);
  pinMode(WIFI_RESET_BUTTON, INPUT_PULLUP);
  
  // Check for WiFi reset button
  bool forceConfigPortal = false;
  if (digitalRead(WIFI_RESET_BUTTON) == LOW) {
    Serial.println("🔄 WiFi Reset button pressed - Clearing saved WiFi credentials");
    wifiManager.resetSettings();
    forceConfigPortal = true;
    delay(3000);
  }
  
  // Setup WiFiManager
  Serial.println("🔧 Setting up WiFiManager...");
  
  // Set callback to save custom parameters
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  
  // Enable debug output
  wifiManager.setDebugOutput(true);
  
  // Set custom AP name and password
  wifiManager.setAPStaticIPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  
  // Customize portal
  wifiManager.setConfigPortalTimeout(300); // 5 minutes timeout
  wifiManager.setConnectTimeout(20); // 20 seconds to connect
  wifiManager.setAPCallback(configModeCallback);
  
  // Custom info
  String customInfo = "<p>🌿 ระบบรดน้ำผักชีฟลั่งอัตโนมัติ - Debug Mode</p>";
  customInfo += "<p>📱 เชื่อมต่อกับ WiFi เพื่อใช้งานระบบ</p>";
  customInfo += "<p>🔗 เข้าใช้งานที่: http://192.168.4.1</p>";
  wifiManager.setCustomHeadElement(customInfo.c_str());
  
  Serial.println("✅ WiFiManager setup completed");
  
  // Force config portal or try auto connect
  bool connected = false;
  if (forceConfigPortal) {
    Serial.println("🌐 Starting Config Portal (Forced)...");
    connected = wifiManager.startConfigPortal("CilantroWatering-Setup", "cilantro123");
  } else {
    Serial.println("🌐 Trying Auto Connect...");
    // Always force config portal for testing
    Serial.println("🔧 DEBUG: Forcing config portal for testing...");
    wifiManager.resetSettings(); // Clear settings for testing
    connected = wifiManager.startConfigPortal("CilantroWatering-Setup", "cilantro123");
  }
  
  if (connected) {
    Serial.println();
    Serial.print("✅ WiFi connected! IP: ");
    Serial.println(WiFi.localIP());
    Serial.println("🎉 Success! WiFiManager is working!");
  } else {
    Serial.println("❌ Failed to connect to WiFi - Restarting...");
    delay(3000);
    ESP.restart();
  }
}

void loop() {
  // Simple LED heartbeat
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 2000) {
    digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
    lastHeartbeat = millis();
    Serial.println("💓 System running... IP: " + WiFi.localIP().toString());
  }
  
  delay(100);
}
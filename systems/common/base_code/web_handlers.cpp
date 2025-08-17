/*
 * Web Handlers Implementation
 * ระบบจัดการเว็บอินเตอร์เฟซขั้นสูง
 * 
 * Comprehensive web interface for:
 * - Network management
 * - System configuration
 * - Blink integration
 * - Real-time monitoring
 * 
 * Firmware made by: RDTRC
 * Version: 3.0
 * Created: 2024
 */

#include "enhanced_system_base.h"

// External references
extern EnhancedSystemConfig systemConfig;
extern EnhancedSystemStatus systemStatus;
extern WebServer webServer;

// HTML Templates and CSS
const char* HTML_HEADER = R"(
<!DOCTYPE html>
<html lang='th'>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>RDTRC IoT System</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: #333; min-height: 100vh; padding: 20px;
        }
        .container { 
            max-width: 1200px; margin: 0 auto; 
            background: rgba(255,255,255,0.95); 
            border-radius: 15px; box-shadow: 0 10px 30px rgba(0,0,0,0.3);
            overflow: hidden;
        }
        .header { 
            background: linear-gradient(45deg, #2c3e50, #3498db); 
            color: white; padding: 30px; text-align: center;
        }
        .header h1 { font-size: 2.5em; margin-bottom: 10px; }
        .header p { font-size: 1.2em; opacity: 0.9; }
        .nav { 
            background: #34495e; padding: 0; 
            display: flex; justify-content: center; flex-wrap: wrap;
        }
        .nav a { 
            color: white; padding: 15px 25px; text-decoration: none; 
            transition: background 0.3s; display: block;
        }
        .nav a:hover, .nav a.active { background: #2c3e50; }
        .content { padding: 30px; }
        .card { 
            background: white; border-radius: 10px; 
            box-shadow: 0 5px 15px rgba(0,0,0,0.1); 
            margin: 20px 0; padding: 25px;
        }
        .card h3 { color: #2c3e50; margin-bottom: 15px; font-size: 1.4em; }
        .status-grid { 
            display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); 
            gap: 20px; margin: 20px 0;
        }
        .status-item { 
            background: #f8f9fa; padding: 15px; border-radius: 8px; 
            border-left: 4px solid #3498db;
        }
        .status-item.success { border-left-color: #27ae60; }
        .status-item.warning { border-left-color: #f39c12; }
        .status-item.error { border-left-color: #e74c3c; }
        .form-group { margin: 15px 0; }
        .form-group label { 
            display: block; margin-bottom: 5px; 
            font-weight: bold; color: #2c3e50;
        }
        .form-group input, .form-group select, .form-group textarea { 
            width: 100%; padding: 12px; border: 2px solid #ddd; 
            border-radius: 6px; font-size: 16px; transition: border-color 0.3s;
        }
        .form-group input:focus, .form-group select:focus, .form-group textarea:focus { 
            border-color: #3498db; outline: none; 
        }
        .btn { 
            background: #3498db; color: white; border: none; 
            padding: 12px 25px; border-radius: 6px; cursor: pointer; 
            font-size: 16px; transition: background 0.3s; margin: 5px;
        }
        .btn:hover { background: #2980b9; }
        .btn.success { background: #27ae60; }
        .btn.success:hover { background: #229954; }
        .btn.danger { background: #e74c3c; }
        .btn.danger:hover { background: #c0392b; }
        .btn.warning { background: #f39c12; }
        .btn.warning:hover { background: #e67e22; }
        .progress { 
            background: #ecf0f1; border-radius: 10px; 
            height: 20px; overflow: hidden; margin: 10px 0;
        }
        .progress-bar { 
            background: #3498db; height: 100%; 
            transition: width 0.3s ease;
        }
        .wifi-list { list-style: none; }
        .wifi-item { 
            background: #f8f9fa; margin: 5px 0; padding: 15px; 
            border-radius: 6px; cursor: pointer; 
            transition: background 0.3s; display: flex; 
            justify-content: space-between; align-items: center;
        }
        .wifi-item:hover { background: #e9ecef; }
        .wifi-item.selected { background: #d4edda; border: 2px solid #27ae60; }
        .signal-strength { font-weight: bold; }
        .signal-1 { color: #e74c3c; }
        .signal-2 { color: #f39c12; }
        .signal-3 { color: #f1c40f; }
        .signal-4 { color: #27ae60; }
        .signal-5 { color: #2ecc71; }
        .footer { 
            background: #2c3e50; color: white; 
            text-align: center; padding: 20px;
        }
        @media (max-width: 768px) {
            .nav { flex-direction: column; }
            .nav a { text-align: center; }
            .status-grid { grid-template-columns: 1fr; }
        }
        .loading { 
            display: inline-block; width: 20px; height: 20px; 
            border: 3px solid #f3f3f3; border-top: 3px solid #3498db; 
            border-radius: 50%; animation: spin 1s linear infinite;
        }
        @keyframes spin { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }
        .alert { 
            padding: 15px; margin: 15px 0; border-radius: 6px; 
            border: 1px solid transparent;
        }
        .alert.success { 
            color: #155724; background-color: #d4edda; 
            border-color: #c3e6cb;
        }
        .alert.error { 
            color: #721c24; background-color: #f8d7da; 
            border-color: #f5c6cb;
        }
        .alert.warning { 
            color: #856404; background-color: #fff3cd; 
            border-color: #ffeaa7;
        }
    </style>
    <script>
        function showLoading(elementId) {
            document.getElementById(elementId).innerHTML = '<span class="loading"></span> กำลังประมวลผล...';
        }
        function hideLoading(elementId, text) {
            document.getElementById(elementId).innerHTML = text;
        }
        function refreshPage() {
            location.reload();
        }
        function selectWiFi(ssid) {
            document.getElementById('primary_ssid').value = ssid;
            document.querySelectorAll('.wifi-item').forEach(item => item.classList.remove('selected'));
            event.target.closest('.wifi-item').classList.add('selected');
        }
        function scanNetworks() {
            showLoading('scan-btn');
            fetch('/scan')
                .then(response => response.json())
                .then(data => {
                    updateWiFiList(data.networks);
                    hideLoading('scan-btn', 'สแกนเครือข่าย');
                })
                .catch(error => {
                    hideLoading('scan-btn', 'สแกนเครือข่าย');
                    alert('เกิดข้อผิดพลาดในการสแกนเครือข่าย');
                });
        }
        function updateWiFiList(networks) {
            const list = document.getElementById('wifi-list');
            list.innerHTML = '';
            networks.forEach(network => {
                const item = document.createElement('li');
                item.className = 'wifi-item';
                item.onclick = () => selectWiFi(network.ssid);
                item.innerHTML = `
                    <span>${network.ssid}</span>
                    <span class="signal-strength signal-${network.strength}">${network.rssi} dBm</span>
                `;
                list.appendChild(item);
            });
        }
        function restartSystem() {
            if (confirm('คุณต้องการรีสตาร์ทระบบหรือไม่?')) {
                showLoading('restart-btn');
                fetch('/restart', {method: 'POST'})
                    .then(() => {
                        alert('ระบบกำลังรีสตาร์ท กรุณารอสักครู่...');
                        setTimeout(() => location.reload(), 10000);
                    });
            }
        }
        function factoryReset() {
            if (confirm('คุณต้องการรีเซ็ตระบบเป็นค่าเริ่มต้นหรือไม่? การกระทำนี้ไม่สามารถย้อนกลับได้!')) {
                if (confirm('คุณแน่ใจหรือไม่? ข้อมูลทั้งหมดจะถูกลบ!')) {
                    showLoading('reset-btn');
                    fetch('/reset', {method: 'POST'})
                        .then(() => {
                            alert('ระบบกำลังรีเซ็ต กรุณารอสักครู่...');
                            setTimeout(() => location.reload(), 15000);
                        });
                }
            }
        }
        setInterval(function() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => updateStatus(data))
                .catch(error => console.log('Status update failed'));
        }, 5000);
        function updateStatus(data) {
            // Update status indicators
            const elements = {
                'uptime': data.system?.uptime ? formatUptime(data.system.uptime) : 'N/A',
                'memory': data.system?.free_memory ? formatBytes(data.system.free_memory) : 'N/A',
                'temperature': data.system?.temperature ? data.system.temperature.toFixed(1) + '°C' : 'N/A'
            };
            Object.keys(elements).forEach(key => {
                const element = document.getElementById(key);
                if (element) element.textContent = elements[key];
            });
        }
        function formatUptime(ms) {
            const seconds = Math.floor(ms / 1000);
            const minutes = Math.floor(seconds / 60);
            const hours = Math.floor(minutes / 60);
            const days = Math.floor(hours / 24);
            if (days > 0) return days + 'd ' + (hours % 24) + 'h';
            if (hours > 0) return hours + 'h ' + (minutes % 60) + 'm';
            return minutes + 'm ' + (seconds % 60) + 's';
        }
        function formatBytes(bytes) {
            if (bytes < 1024) return bytes + 'B';
            if (bytes < 1024 * 1024) return Math.floor(bytes / 1024) + 'KB';
            return Math.floor(bytes / (1024 * 1024)) + 'MB';
        }
    </script>
</head>
<body>
)";

const char* HTML_FOOTER = R"(
    <div class="footer">
        <p>&copy; 2024 RDTRC - Firmware Version 3.0</p>
        <p>Enhanced IoT System with Blink Integration & Hotspot Capability</p>
    </div>
</body>
</html>
)";

// Main page handler
void handleRoot() {
  String html = String(HTML_HEADER);
  
  html += R"(
<div class="container">
    <div class="header">
        <h1>🤖 RDTRC IoT System</h1>
        <p>)" + String(systemConfig.device_name) + R"( - )" + String(systemConfig.system_type) + R"(</p>
    </div>
    
    <div class="nav">
        <a href="/" class="active">หน้าแรก</a>
        <a href="/network">ตั้งค่าเครือข่าย</a>
        <a href="/system">ตั้งค่าระบบ</a>
        <a href="/blink">Blink Integration</a>
        <a href="/status">สถานะระบบ</a>
        <a href="/device-test">ทดสอบอุปกรณ์</a>
    </div>
    
    <div class="content">
        <div class="card">
            <h3>🏠 ภาพรวมระบบ</h3>
            <div class="status-grid">
                <div class="status-item )" + String(systemStatus.network.wifi_connected ? "success" : "error") + R"(">
                    <strong>📶 WiFi Connection</strong><br>
                    สถานะ: )" + String(systemStatus.network.wifi_connected ? "เชื่อมต่อแล้ว" : "ไม่เชื่อมต่อ") + R"(<br>
                    )" + (systemStatus.network.wifi_connected ? "IP: " + systemStatus.network.local_ip : "") + R"(
                </div>
                
                <div class="status-item )" + String(systemStatus.network.hotspot_active ? "warning" : "error") + R"(">
                    <strong>📡 Hotspot</strong><br>
                    สถานะ: )" + String(systemStatus.network.hotspot_active ? "เปิดใช้งาน" : "ปิดใช้งาน") + R"(<br>
                    )" + (systemStatus.network.hotspot_active ? "Clients: " + String(systemStatus.network.connected_clients) : "") + R"(
                </div>
                
                <div class="status-item )" + String(systemStatus.network.blink_connected ? "success" : "warning") + R"(">
                    <strong>🔗 Blink Connection</strong><br>
                    สถานะ: )" + String(systemStatus.network.blink_connected ? "เชื่อมต่อแล้ว" : "ไม่เชื่อมต่อ") + R"(<br>
                    Server: )" + String(systemConfig.blink_server) + R"(
                </div>
                
                <div class="status-item success">
                    <strong>⏱️ Uptime</strong><br>
                    เวลาทำงาน: <span id="uptime">กำลังโหลด...</span><br>
                    เริ่มต้น: )" + String(millis() / 1000) + R"( วินาทีที่แล้ว
                </div>
                
                <div class="status-item )" + String(systemStatus.free_memory > 50000 ? "success" : "warning") + R"(">
                    <strong>💾 Memory</strong><br>
                    หน่วยความจำว่าง: <span id="memory">กำลังโหลด...</span><br>
                    การใช้งาน: )" + String(systemStatus.memory_usage_percent, 1) + R"(%
                </div>
                
                <div class="status-item )" + String(systemStatus.cpu_temperature < 70 ? "success" : "warning") + R"(">
                    <strong>🌡️ Temperature</strong><br>
                    อุณหภูมิ CPU: <span id="temperature">กำลังโหลด...</span><br>
                    สถานะ: )" + String(systemStatus.cpu_temperature < 70 ? "ปกติ" : "สูง") + R"(
                </div>
            </div>
        </div>
        
        <div class="card">
            <h3>⚡ การจัดการระบบด่วน</h3>
            <button class="btn success" onclick="refreshPage()">🔄 รีเฟรชหน้า</button>
            <button class="btn warning" onclick="restartSystem()" id="restart-btn">🔄 รีสตาร์ทระบบ</button>
            <button class="btn danger" onclick="factoryReset()" id="reset-btn">⚠️ รีเซ็ตเป็นค่าเริ่มต้น</button>
        </div>
        
        <div class="card">
            <h3>📋 ข้อมูลระบบ</h3>
            <p><strong>ชื่อระบบ:</strong> )" + String(systemConfig.device_name) + R"(</p>
            <p><strong>ประเภท:</strong> )" + String(systemConfig.system_type) + R"(</p>
            <p><strong>เวอร์ชัน Firmware:</strong> )" + String(FIRMWARE_VERSION) + R"(</p>
            <p><strong>ผู้พัฒนา:</strong> )" + String(FIRMWARE_AUTHOR) + R"(</p>
            <p><strong>MAC Address:</strong> )" + WiFi.macAddress() + R"(</p>
        </div>
    </div>
</div>
)";
  
  html += HTML_FOOTER;
  webServer.send(200, "text/html", html);
}

// Network configuration handler
void handleNetworkConfig() {
  if (webServer.method() == HTTP_POST) {
    // Handle form submission
    if (webServer.hasArg("primary_ssid")) {
      strcpy(systemConfig.primary_ssid, webServer.arg("primary_ssid").c_str());
    }
    if (webServer.hasArg("primary_password")) {
      strcpy(systemConfig.primary_password, webServer.arg("primary_password").c_str());
    }
    if (webServer.hasArg("secondary_ssid")) {
      strcpy(systemConfig.secondary_ssid, webServer.arg("secondary_ssid").c_str());
    }
    if (webServer.hasArg("secondary_password")) {
      strcpy(systemConfig.secondary_password, webServer.arg("secondary_password").c_str());
    }
    if (webServer.hasArg("hotspot_ssid")) {
      strcpy(systemConfig.hotspot_ssid, webServer.arg("hotspot_ssid").c_str());
    }
    if (webServer.hasArg("hotspot_password")) {
      strcpy(systemConfig.hotspot_password, webServer.arg("hotspot_password").c_str());
    }
    if (webServer.hasArg("network_mode")) {
      systemConfig.network_mode = (NetworkMode)webServer.arg("network_mode").toInt();
    }
    if (webServer.hasArg("auto_hotspot_fallback")) {
      systemConfig.auto_hotspot_fallback = webServer.arg("auto_hotspot_fallback") == "on";
    }
    
    // Save configuration
    extern EnhancedSystemBase* currentSystem;
    if (currentSystem) {
      currentSystem->saveConfiguration();
    }
    
    webServer.sendHeader("Location", "/network?saved=1");
    webServer.send(302);
    return;
  }
  
  String html = String(HTML_HEADER);
  
  html += R"(
<div class="container">
    <div class="header">
        <h1>📶 ตั้งค่าเครือข่าย</h1>
        <p>จัดการการเชื่อมต่อ WiFi และ Hotspot</p>
    </div>
    
         <div class="nav">
         <a href="/">หน้าแรก</a>
         <a href="/network" class="active">ตั้งค่าเครือข่าย</a>
         <a href="/system">ตั้งค่าระบบ</a>
         <a href="/blink">Blink Integration</a>
         <a href="/status">สถานะระบบ</a>
         <a href="/device-test">ทดสอบอุปกรณ์</a>
     </div>
    
    <div class="content">
)";

  if (webServer.hasArg("saved")) {
    html += R"(<div class="alert success">✅ บันทึกการตั้งค่าเครือข่ายเรียบร้อยแล้ว! ระบบจะใช้การตั้งค่าใหม่หลังจากรีสตาร์ท</div>)";
  }

  html += R"(
        <form method="POST">
            <div class="card">
                <h3>🔍 ค้นหาเครือข่าย WiFi</h3>
                <button type="button" class="btn" onclick="scanNetworks()" id="scan-btn">🔍 สแกนเครือข่าย</button>
                <ul id="wifi-list" class="wifi-list">
                    <li style="text-align: center; padding: 20px; color: #666;">กดปุ่ม "สแกนเครือข่าย" เพื่อค้นหา WiFi ที่ใช้ได้</li>
                </ul>
            </div>
            
            <div class="card">
                <h3>📶 การตั้งค่า WiFi หลัก</h3>
                <div class="form-group">
                    <label for="primary_ssid">ชื่อเครือข่าย (SSID):</label>
                    <input type="text" id="primary_ssid" name="primary_ssid" value=")" + String(systemConfig.primary_ssid) + R"(" placeholder="ชื่อ WiFi ที่ต้องการเชื่อมต่อ">
                </div>
                <div class="form-group">
                    <label for="primary_password">รหัสผ่าน:</label>
                    <input type="password" id="primary_password" name="primary_password" value=")" + String(systemConfig.primary_password) + R"(" placeholder="รหัสผ่าน WiFi">
                </div>
            </div>
            
            <div class="card">
                <h3>📶 การตั้งค่า WiFi สำรอง</h3>
                <div class="form-group">
                    <label for="secondary_ssid">ชื่อเครือข่ายสำรอง (SSID):</label>
                    <input type="text" id="secondary_ssid" name="secondary_ssid" value=")" + String(systemConfig.secondary_ssid) + R"(" placeholder="ชื่อ WiFi สำรอง (ไม่บังคับ)">
                </div>
                <div class="form-group">
                    <label for="secondary_password">รหัสผ่านสำรอง:</label>
                    <input type="password" id="secondary_password" name="secondary_password" value=")" + String(systemConfig.secondary_password) + R"(" placeholder="รหัสผ่าน WiFi สำรอง">
                </div>
            </div>
            
            <div class="card">
                <h3>📡 การตั้งค่า Hotspot</h3>
                <div class="form-group">
                    <label for="hotspot_ssid">ชื่อ Hotspot:</label>
                    <input type="text" id="hotspot_ssid" name="hotspot_ssid" value=")" + String(systemConfig.hotspot_ssid) + R"(" placeholder="ชื่อ Hotspot ที่จะแสดง">
                </div>
                <div class="form-group">
                    <label for="hotspot_password">รหัสผ่าน Hotspot:</label>
                    <input type="password" id="hotspot_password" name="hotspot_password" value=")" + String(systemConfig.hotspot_password) + R"(" placeholder="รหัสผ่าน Hotspot (อย่างน้อย 8 ตัวอักษร)">
                </div>
            </div>
            
            <div class="card">
                <h3>⚙️ โหมดการทำงาน</h3>
                <div class="form-group">
                    <label for="network_mode">โหมดเครือข่าย:</label>
                    <select id="network_mode" name="network_mode">
                        <option value="0" )" + String(systemConfig.network_mode == MODE_WIFI_CLIENT ? "selected" : "") + R"(>WiFi Client เท่านั้น</option>
                        <option value="1" )" + String(systemConfig.network_mode == MODE_HOTSPOT ? "selected" : "") + R"(>Hotspot เท่านั้น</option>
                        <option value="2" )" + String(systemConfig.network_mode == MODE_HYBRID ? "selected" : "") + R"(>Hybrid (WiFi + Hotspot)</option>
                    </select>
                </div>
                <div class="form-group">
                    <label>
                        <input type="checkbox" name="auto_hotspot_fallback" )" + String(systemConfig.auto_hotspot_fallback ? "checked" : "") + R"(>
                        เปิด Hotspot อัตโนมัติเมื่อ WiFi เชื่อมต่อไม่ได้
                    </label>
                </div>
            </div>
            
            <div class="card">
                <button type="submit" class="btn success">💾 บันทึกการตั้งค่า</button>
                <button type="button" class="btn warning" onclick="restartSystem()">🔄 บันทึกและรีสตาร์ท</button>
            </div>
        </form>
    </div>
</div>
)";
  
  html += HTML_FOOTER;
  webServer.send(200, "text/html", html);
}

// System configuration handler
void handleSystemConfig() {
  if (webServer.method() == HTTP_POST) {
    // Handle form submission
    if (webServer.hasArg("device_name")) {
      strcpy(systemConfig.device_name, webServer.arg("device_name").c_str());
    }
    if (webServer.hasArg("timezone_offset")) {
      systemConfig.timezone_offset = webServer.arg("timezone_offset").toInt();
    }
    if (webServer.hasArg("ota_enabled")) {
      systemConfig.ota_enabled = webServer.arg("ota_enabled") == "on";
    }
    if (webServer.hasArg("debug_mode")) {
      systemConfig.debug_mode = webServer.arg("debug_mode") == "on";
    }
    
    // Save configuration
    extern EnhancedSystemBase* currentSystem;
    if (currentSystem) {
      currentSystem->saveConfiguration();
    }
    
    webServer.sendHeader("Location", "/system?saved=1");
    webServer.send(302);
    return;
  }
  
  String html = String(HTML_HEADER);
  
  html += R"(
<div class="container">
    <div class="header">
        <h1>⚙️ ตั้งค่าระบบ</h1>
        <p>จัดการการตั้งค่าทั่วไปของระบบ</p>
    </div>
    
         <div class="nav">
         <a href="/">หน้าแรก</a>
         <a href="/network">ตั้งค่าเครือข่าย</a>
         <a href="/system" class="active">ตั้งค่าระบบ</a>
         <a href="/blink">Blink Integration</a>
         <a href="/status">สถานะระบบ</a>
         <a href="/device-test">ทดสอบอุปกรณ์</a>
     </div>
    
    <div class="content">
)";

  if (webServer.hasArg("saved")) {
    html += R"(<div class="alert success">✅ บันทึกการตั้งค่าระบบเรียบร้อยแล้ว!</div>)";
  }

  html += R"(
        <form method="POST">
            <div class="card">
                <h3>🏷️ ข้อมูลระบบ</h3>
                <div class="form-group">
                    <label for="device_name">ชื่ออุปกรณ์:</label>
                    <input type="text" id="device_name" name="device_name" value=")" + String(systemConfig.device_name) + R"(" placeholder="ชื่อที่ต้องการแสดง">
                </div>
                <div class="form-group">
                    <label>ประเภทระบบ: <strong>)" + String(systemConfig.system_type) + R"(</strong></label>
                </div>
                <div class="form-group">
                    <label>MAC Address: <strong>)" + WiFi.macAddress() + R"(</strong></label>
                </div>
            </div>
            
            <div class="card">
                <h3>🌍 การตั้งค่าเวลา</h3>
                <div class="form-group">
                    <label for="timezone_offset">เขตเวลา (GMT offset):</label>
                    <select id="timezone_offset" name="timezone_offset">
                        <option value="0" )" + String(systemConfig.timezone_offset == 0 ? "selected" : "") + R"(>GMT+0 (UTC)</option>
                        <option value="7" )" + String(systemConfig.timezone_offset == 7 ? "selected" : "") + R"(>GMT+7 (Thailand)</option>
                        <option value="8" )" + String(systemConfig.timezone_offset == 8 ? "selected" : "") + R"(>GMT+8 (Singapore)</option>
                        <option value="9" )" + String(systemConfig.timezone_offset == 9 ? "selected" : "") + R"(>GMT+9 (Japan)</option>
                    </select>
                </div>
            </div>
            
            <div class="card">
                <h3>🔧 ตัวเลือกขั้นสูง</h3>
                <div class="form-group">
                    <label>
                        <input type="checkbox" name="ota_enabled" )" + String(systemConfig.ota_enabled ? "checked" : "") + R"(>
                        เปิดใช้งาน OTA Update (อัพเดทผ่านเครือข่าย)
                    </label>
                </div>
                <div class="form-group">
                    <label>
                        <input type="checkbox" name="debug_mode" )" + String(systemConfig.debug_mode ? "checked" : "") + R"(>
                        เปิดใช้งาน Debug Mode (แสดงข้อมูลเพิ่มเติม)
                    </label>
                </div>
            </div>
            
            <div class="card">
                <h3>📊 ข้อมูลสถิติ</h3>
                <p><strong>เวลาทำงาน:</strong> <span id="uptime">กำลังโหลด...</span></p>
                <p><strong>จำนวนครั้งที่เกิดข้อผิดพลาด:</strong> )" + String(systemStatus.error_count) + R"(</p>
                <p><strong>ข้อผิดพลาดล่าสุด:</strong> )" + (systemStatus.last_error.length() > 0 ? systemStatus.last_error : "ไม่มี") + R"(</p>
                <p><strong>การใช้หน่วยความจำ:</strong> )" + String(systemStatus.memory_usage_percent, 1) + R"(%</p>
            </div>
            
            <div class="card">
                <button type="submit" class="btn success">💾 บันทึกการตั้งค่า</button>
                <button type="button" class="btn warning" onclick="restartSystem()">🔄 รีสตาร์ทระบบ</button>
                <button type="button" class="btn danger" onclick="factoryReset()">⚠️ รีเซ็ตเป็นค่าเริ่มต้น</button>
            </div>
        </form>
    </div>
</div>
)";
  
  html += HTML_FOOTER;
  webServer.send(200, "text/html", html);
}

// Blink configuration handler
void handleBlinkConfig() {
  if (webServer.method() == HTTP_POST) {
    // Handle form submission
    if (webServer.hasArg("blink_enabled")) {
      systemConfig.blink_enabled = webServer.arg("blink_enabled") == "on";
    } else {
      systemConfig.blink_enabled = false;
    }
    
    if (webServer.hasArg("blink_token")) {
      strcpy(systemConfig.blink_token, webServer.arg("blink_token").c_str());
    }
    if (webServer.hasArg("blink_server")) {
      strcpy(systemConfig.blink_server, webServer.arg("blink_server").c_str());
    }
    if (webServer.hasArg("blink_port")) {
      systemConfig.blink_port = webServer.arg("blink_port").toInt();
    }
    if (webServer.hasArg("blink_ssl")) {
      systemConfig.blink_ssl = webServer.arg("blink_ssl") == "on";
    } else {
      systemConfig.blink_ssl = false;
    }
    
    // Save configuration
    extern EnhancedSystemBase* currentSystem;
    if (currentSystem) {
      currentSystem->saveConfiguration();
    }
    
    webServer.sendHeader("Location", "/blink?saved=1");
    webServer.send(302);
    return;
  }
  
  String html = String(HTML_HEADER);
  
  html += R"(
<div class="container">
    <div class="header">
        <h1>🔗 Blink Integration</h1>
        <p>ตั้งค่าการเชื่อมต่อกับ Blink Cloud</p>
    </div>
    
         <div class="nav">
         <a href="/">หน้าแรก</a>
         <a href="/network">ตั้งค่าเครือข่าย</a>
         <a href="/system">ตั้งค่าระบบ</a>
         <a href="/blink" class="active">Blink Integration</a>
         <a href="/status">สถานะระบบ</a>
         <a href="/device-test">ทดสอบอุปกรณ์</a>
     </div>
    
    <div class="content">
)";

  if (webServer.hasArg("saved")) {
    html += R"(<div class="alert success">✅ บันทึกการตั้งค่า Blink เรียบร้อยแล้ว! ระบบจะเชื่อมต่อใหม่อัตโนมัติ</div>)";
  }

  html += R"(
        <div class="card">
            <h3>📊 สถานะการเชื่อมต่อ Blink</h3>
            <div class="status-item )" + String(systemStatus.network.blink_connected ? "success" : "error") + R"(">
                <strong>สถานะ:</strong> )" + String(systemStatus.network.blink_connected ? "เชื่อมต่อแล้ว ✅" : "ไม่เชื่อมต่อ ❌") + R"(<br>
                <strong>Server:</strong> )" + String(systemConfig.blink_server) + R"(:" + String(systemConfig.blink_port) + R"(<br>
                <strong>SSL:</strong> )" + String(systemConfig.blink_ssl ? "เปิดใช้งาน" : "ปิดใช้งาน") + R"(<br>
                <strong>Last Ping:</strong> )" + String((millis() - systemStatus.network.last_blink_ping) / 1000) + R"( วินาทีที่แล้ว
            </div>
        </div>
        
        <form method="POST">
            <div class="card">
                <h3>⚙️ การตั้งค่า Blink</h3>
                <div class="form-group">
                    <label>
                        <input type="checkbox" name="blink_enabled" )" + String(systemConfig.blink_enabled ? "checked" : "") + R"(>
                        เปิดใช้งาน Blink Integration
                    </label>
                </div>
                <div class="form-group">
                    <label for="blink_token">Blink Auth Token:</label>
                    <input type="text" id="blink_token" name="blink_token" value=")" + String(systemConfig.blink_token) + R"(" placeholder="ใส่ Auth Token ที่ได้จาก Blink App">
                    <small>💡 วิธีการหา Token: เปิด Blink App → Device Info → Auth Token</small>
                </div>
            </div>
            
            <div class="card">
                <h3>🌐 การตั้งค่า Server</h3>
                <div class="form-group">
                    <label for="blink_server">Blink Server:</label>
                    <input type="text" id="blink_server" name="blink_server" value=")" + String(systemConfig.blink_server) + R"(" placeholder="blynk.cloud">
                </div>
                <div class="form-group">
                    <label for="blink_port">Port:</label>
                    <input type="number" id="blink_port" name="blink_port" value=")" + String(systemConfig.blink_port) + R"(" placeholder="443">
                </div>
                <div class="form-group">
                    <label>
                        <input type="checkbox" name="blink_ssl" )" + String(systemConfig.blink_ssl ? "checked" : "") + R"(>
                        ใช้ SSL/TLS (แนะนำ)
                    </label>
                </div>
            </div>
            
            <div class="card">
                <h3>📱 คู่มือการใช้งาน Blink</h3>
                <ol>
                    <li>ดาวน์โหลดแอป Blink จาก App Store หรือ Google Play</li>
                    <li>สร้างบัญชีผู้ใช้และเข้าสู่ระบบ</li>
                    <li>สร้าง New Project</li>
                    <li>เพิ่ม Device และคัดลอก Auth Token</li>
                    <li>นำ Token มาใส่ในช่องด้านบนและกด "บันทึก"</li>
                    <li>ระบบจะเชื่อมต่อกับ Blink อัตโนมัติ</li>
                </ol>
                <p><strong>หมายเหตุ:</strong> ต้องมีการเชื่อมต่ออินเทอร์เน็ตเพื่อใช้งาน Blink</p>
            </div>
            
            <div class="card">
                <button type="submit" class="btn success">💾 บันทึกการตั้งค่า</button>
                <button type="button" class="btn" onclick="testBlinkConnection()">🧪 ทดสอบการเชื่อมต่อ</button>
            </div>
        </form>
    </div>
</div>

<script>
function testBlinkConnection() {
    alert('กำลังทดสอบการเชื่อมต่อ Blink...');
    fetch('/api/blink', {method: 'POST', body: 'test=1'})
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                alert('✅ เชื่อมต่อ Blink สำเร็จ!');
            } else {
                alert('❌ เชื่อมต่อ Blink ไม่สำเร็จ: ' + data.error);
            }
        })
        .catch(error => {
            alert('❌ เกิดข้อผิดพลาดในการทดสอบ');
        });
}
</script>
)";
  
  html += HTML_FOOTER;
  webServer.send(200, "text/html", html);
}

// System status handler
void handleSystemInfo() {
  String html = String(HTML_HEADER);
  
  html += R"(
<div class="container">
    <div class="header">
        <h1>📊 สถานะระบบ</h1>
        <p>ข้อมูลโดยละเอียดของระบบ</p>
    </div>
    
         <div class="nav">
         <a href="/">หน้าแรก</a>
         <a href="/network">ตั้งค่าเครือข่าย</a>
         <a href="/system">ตั้งค่าระบบ</a>
         <a href="/blink">Blink Integration</a>
         <a href="/status" class="active">สถานะระบบ</a>
         <a href="/device-test">ทดสอบอุปกรณ์</a>
     </div>
    
    <div class="content">
        <div class="card">
            <h3>💻 ข้อมูลฮาร์ดแวร์</h3>
            <div class="status-grid">
                <div class="status-item">
                    <strong>Chip Model:</strong><br>)" + String(ESP.getChipModel()) + R"(
                </div>
                <div class="status-item">
                    <strong>Chip Revision:</strong><br>)" + String(ESP.getChipRevision()) + R"(
                </div>
                <div class="status-item">
                    <strong>CPU Frequency:</strong><br>)" + String(ESP.getCpuFreqMHz()) + R"( MHz
                </div>
                <div class="status-item">
                    <strong>Flash Size:</strong><br>)" + String(ESP.getFlashChipSize() / 1024 / 1024) + R"( MB
                </div>
            </div>
        </div>
        
        <div class="card">
            <h3>📊 หน่วยความจำ</h3>
            <div class="progress">
                <div class="progress-bar" style="width: )" + String(systemStatus.memory_usage_percent) + R"(%;"></div>
            </div>
            <p>การใช้งาน: )" + String(systemStatus.memory_usage_percent, 1) + R"(% ()" + String((systemStatus.total_memory - systemStatus.free_memory) / 1024) + R"( KB / )" + String(systemStatus.total_memory / 1024) + R"( KB)</p>
            <p>หน่วยความจำว่าง: )" + String(systemStatus.free_memory / 1024) + R"( KB</p>
        </div>
        
        <div class="card">
            <h3>🌐 ข้อมูลเครือข่าย</h3>
            <div class="status-grid">
                <div class="status-item )" + String(systemStatus.network.wifi_connected ? "success" : "error") + R"(">
                    <strong>📶 WiFi Status</strong><br>
                    )" + String(systemStatus.network.wifi_connected ? "Connected" : "Disconnected") + R"(<br>
                    )" + (systemStatus.network.wifi_connected ? "IP: " + systemStatus.network.local_ip : "") + R"(
                </div>
                <div class="status-item )" + String(systemStatus.network.hotspot_active ? "warning" : "error") + R"(">
                    <strong>📡 Hotspot Status</strong><br>
                    )" + String(systemStatus.network.hotspot_active ? "Active" : "Inactive") + R"(<br>
                    )" + (systemStatus.network.hotspot_active ? "Clients: " + String(systemStatus.network.connected_clients) : "") + R"(
                </div>
                <div class="status-item )" + String(systemStatus.network.internet_available ? "success" : "warning") + R"(">
                    <strong>🌍 Internet</strong><br>
                    )" + String(systemStatus.network.internet_available ? "Available" : "Not Available") + R"(
                </div>
                <div class="status-item )" + String(systemStatus.network.blink_connected ? "success" : "error") + R"(">
                    <strong>🔗 Blink</strong><br>
                    )" + String(systemStatus.network.blink_connected ? "Connected" : "Disconnected") + R"(
                </div>
            </div>
        </div>
        
        <div class="card">
            <h3>🔧 ข้อมูลระบบ</h3>
            <table style="width: 100%; border-collapse: collapse;">
                <tr style="border-bottom: 1px solid #ddd;">
                    <td style="padding: 10px; font-weight: bold;">System Name:</td>
                    <td style="padding: 10px;">)" + String(systemConfig.device_name) + R"(</td>
                </tr>
                <tr style="border-bottom: 1px solid #ddd;">
                    <td style="padding: 10px; font-weight: bold;">System Type:</td>
                    <td style="padding: 10px;">)" + String(systemConfig.system_type) + R"(</td>
                </tr>
                <tr style="border-bottom: 1px solid #ddd;">
                    <td style="padding: 10px; font-weight: bold;">Firmware Version:</td>
                    <td style="padding: 10px;">)" + String(FIRMWARE_VERSION) + R"(</td>
                </tr>
                <tr style="border-bottom: 1px solid #ddd;">
                    <td style="padding: 10px; font-weight: bold;">MAC Address:</td>
                    <td style="padding: 10px;">)" + WiFi.macAddress() + R"(</td>
                </tr>
                <tr style="border-bottom: 1px solid #ddd;">
                    <td style="padding: 10px; font-weight: bold;">Uptime:</td>
                    <td style="padding: 10px;" id="uptime">กำลังโหลด...</td>
                </tr>
                <tr style="border-bottom: 1px solid #ddd;">
                    <td style="padding: 10px; font-weight: bold;">CPU Temperature:</td>
                    <td style="padding: 10px;" id="temperature">กำลังโหลด...</td>
                </tr>
                <tr style="border-bottom: 1px solid #ddd;">
                    <td style="padding: 10px; font-weight: bold;">Error Count:</td>
                    <td style="padding: 10px;">)" + String(systemStatus.error_count) + R"(</td>
                </tr>
                <tr>
                    <td style="padding: 10px; font-weight: bold;">Last Error:</td>
                    <td style="padding: 10px;">)" + (systemStatus.last_error.length() > 0 ? systemStatus.last_error : "None") + R"(</td>
                </tr>
            </table>
        </div>
        
        <div class="card">
            <h3>🔄 การจัดการระบบ</h3>
            <button class="btn" onclick="refreshPage()">🔄 รีเฟรชข้อมูล</button>
            <button class="btn warning" onclick="restartSystem()">🔄 รีสตาร์ทระบบ</button>
            <button class="btn danger" onclick="factoryReset()">⚠️ รีเซ็ตเป็นค่าเริ่มต้น</button>
        </div>
    </div>
</div>
)";
  
  html += HTML_FOOTER;
  webServer.send(200, "text/html", html);
}

// Network scan handler
void handleNetworkScan() {
  DynamicJsonDocument doc(2048);
  JsonArray networks = doc.createNestedArray("networks");
  
  int numNetworks = WiFi.scanNetworks();
  
  for (int i = 0; i < numNetworks; i++) {
    JsonObject network = networks.createNestedObject();
    network["ssid"] = WiFi.SSID(i);
    network["rssi"] = WiFi.RSSI(i);
    network["encryption"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Secured";
    
    // Calculate signal strength (1-5 scale)
    int rssi = WiFi.RSSI(i);
    int strength;
    if (rssi > -50) strength = 5;
    else if (rssi > -60) strength = 4;
    else if (rssi > -70) strength = 3;
    else if (rssi > -80) strength = 2;
    else strength = 1;
    
    network["strength"] = strength;
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

// Restart handler
void handleRestart() {
  webServer.send(200, "text/plain", "System restarting...");
  delay(1000);
  ESP.restart();
}

// Factory reset handler
void handleFactoryReset() {
  webServer.send(200, "text/plain", "Factory reset in progress...");
  
  // Clear EEPROM
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  
  // Clear SPIFFS
  SPIFFS.format();
  
  delay(2000);
  ESP.restart();
}

// OTA Update handler (placeholder)
void handleOTAUpdate() {
  String html = String(HTML_HEADER);
  html += R"(
<div class="container">
    <div class="header">
        <h1>🔄 OTA Update</h1>
        <p>อัพเดท Firmware ผ่านเครือข่าย</p>
    </div>
    <div class="content">
        <div class="card">
            <h3>⚠️ OTA Update</h3>
            <p>ฟีเจอร์นี้จะพัฒนาในเวอร์ชันถัดไป</p>
            <p>ขณะนี้สามารถใช้ Arduino IDE หรือ PlatformIO เพื่ออัพเดท OTA ได้</p>
        </div>
    </div>
</div>
)";
  html += HTML_FOOTER;
  webServer.send(200, "text/html", html);
}

// Not found handler (for captive portal)
void handleNotFound() {
  if (systemStatus.network.hotspot_active) {
    // Redirect to main page for captive portal
    webServer.sendHeader("Location", "http://" + systemStatus.network.hotspot_ip);
    webServer.send(302);
  } else {
    webServer.send(404, "text/plain", "Not Found");
  }
}

// API Handlers
void handleAPIStatus() {
  extern EnhancedSystemBase* currentSystem;
  
  DynamicJsonDocument doc(1024);
  
  // System info
  doc["system"]["name"] = systemConfig.device_name;
  doc["system"]["type"] = systemConfig.system_type;
  doc["system"]["version"] = FIRMWARE_VERSION;
  doc["system"]["uptime"] = systemStatus.uptime;
  doc["system"]["free_memory"] = systemStatus.free_memory;
  doc["system"]["total_memory"] = systemStatus.total_memory;
  doc["system"]["memory_usage"] = systemStatus.memory_usage_percent;
  doc["system"]["temperature"] = systemStatus.cpu_temperature;
  doc["system"]["state"] = (int)systemStatus.state;
  doc["system"]["error_count"] = systemStatus.error_count;
  doc["system"]["last_error"] = systemStatus.last_error;
  
  // Network info
  doc["network"]["wifi_connected"] = systemStatus.network.wifi_connected;
  doc["network"]["wifi_ip"] = systemStatus.network.local_ip;
  doc["network"]["wifi_rssi"] = systemStatus.network.wifi_signal_strength;
  doc["network"]["hotspot_active"] = systemStatus.network.hotspot_active;
  doc["network"]["hotspot_ip"] = systemStatus.network.hotspot_ip;
  doc["network"]["hotspot_clients"] = systemStatus.network.connected_clients;
  doc["network"]["blink_connected"] = systemStatus.network.blink_connected;
  doc["network"]["internet_available"] = systemStatus.network.internet_available;
  
  // Hardware info
  doc["hardware"]["rtc_connected"] = systemStatus.rtc_connected;
  doc["hardware"]["lcd_connected"] = systemStatus.lcd_connected;
  doc["hardware"]["sensors_ok"] = systemStatus.sensors_ok;
  
  // Get system-specific status
  if (currentSystem) {
    String systemStatus = currentSystem->getSystemStatus();
    if (systemStatus.length() > 0) {
      DynamicJsonDocument systemDoc(512);
      deserializeJson(systemDoc, systemStatus);
      doc["specific"] = systemDoc;
    }
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

void handleAPIConfig() {
  if (webServer.method() == HTTP_GET) {
    // Return current configuration
    DynamicJsonDocument doc(1024);
    
    doc["device_name"] = systemConfig.device_name;
    doc["system_type"] = systemConfig.system_type;
    doc["primary_ssid"] = systemConfig.primary_ssid;
    doc["secondary_ssid"] = systemConfig.secondary_ssid;
    doc["hotspot_ssid"] = systemConfig.hotspot_ssid;
    doc["network_mode"] = (int)systemConfig.network_mode;
    doc["auto_hotspot_fallback"] = systemConfig.auto_hotspot_fallback;
    doc["blink_enabled"] = systemConfig.blink_enabled;
    doc["blink_server"] = systemConfig.blink_server;
    doc["blink_port"] = systemConfig.blink_port;
    doc["blink_ssl"] = systemConfig.blink_ssl;
    doc["timezone_offset"] = systemConfig.timezone_offset;
    doc["ota_enabled"] = systemConfig.ota_enabled;
    doc["debug_mode"] = systemConfig.debug_mode;
    
    String response;
    serializeJson(doc, response);
    webServer.send(200, "application/json", response);
  } else {
    webServer.send(405, "text/plain", "Method Not Allowed");
  }
}

void handleAPINetworks() {
  handleNetworkScan(); // Reuse the network scan handler
}

void handleAPIRestart() {
  DynamicJsonDocument doc(256);
  doc["success"] = true;
  doc["message"] = "System restart initiated";
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
  
  delay(1000);
  ESP.restart();
}

void handleAPIReset() {
  DynamicJsonDocument doc(256);
  doc["success"] = true;
  doc["message"] = "Factory reset initiated";
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
  
  // Clear EEPROM
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  
  // Clear SPIFFS
  SPIFFS.format();
  
  delay(2000);
  ESP.restart();
}

void handleAPIBlink() {
  DynamicJsonDocument doc(256);
  
  if (webServer.hasArg("test")) {
    // Test Blink connection
    extern EnhancedSystemBase* currentSystem;
    if (currentSystem && currentSystem->isBlinkConnected()) {
      doc["success"] = true;
      doc["message"] = "Blink connection test successful";
    } else {
      doc["success"] = false;
      doc["error"] = "Blink connection failed";
    }
  } else {
    doc["success"] = false;
    doc["error"] = "Invalid request";
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}
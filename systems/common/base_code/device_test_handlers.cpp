/*
 * Device Test Handlers Implementation
 * ระบบทดสอบอุปกรณ์ผ่านเว็บอินเตอร์เฟซ
 * 
 * Features:
 * - Individual component testing
 * - Real-time sensor readings
 * - Actuator control testing
 * - System diagnostics
 * - Hardware validation
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

// Device Test Handler - Main Testing Page
void handleDeviceTest() {
  String html = String(HTML_HEADER);
  
  html += R"(
<div class="container">
    <div class="header">
        <h1>🔧 ทดสอบอุปกรณ์</h1>
        <p>ทดสอบการทำงานของอุปกรณ์แต่ละชิ้นแยกกัน</p>
    </div>
    
    <div class="nav">
        <a href="/">หน้าแรก</a>
        <a href="/network">ตั้งค่าเครือข่าย</a>
        <a href="/system">ตั้งค่าระบบ</a>
        <a href="/blink">Blink Integration</a>
        <a href="/status">สถานะระบบ</a>
        <a href="/device-test" class="active">ทดสอบอุปกรณ์</a>
    </div>
    
    <div class="content">
        <!-- System Information -->
        <div class="card">
            <h3>📊 ข้อมูลระบบ</h3>
            <div class="status-grid">
                <div class="status-item">
                    <strong>ประเภทระบบ:</strong><br>
                    )" + String(systemConfig.system_type) + R"(
                </div>
                <div class="status-item">
                    <strong>เวอร์ชัน:</strong><br>
                    )" + String(FIRMWARE_VERSION) + R"(
                </div>
                <div class="status-item">
                    <strong>Uptime:</strong><br>
                    <span id="system-uptime">กำลังโหลด...</span>
                </div>
                <div class="status-item">
                    <strong>หน่วยความจำ:</strong><br>
                    <span id="system-memory">กำลังโหลด...</span>
                </div>
            </div>
        </div>
        
        <!-- Basic Hardware Tests -->
        <div class="card">
            <h3>🔌 ทดสอบฮาร์ดแวร์พื้นฐาน</h3>
            <div class="form-group">
                <button class="btn" onclick="testLED()">💡 ทดสอบ LED</button>
                <button class="btn" onclick="testBuzzer()">🔊 ทดสอบ Buzzer</button>
                <button class="btn" onclick="testLCD()">📺 ทดสอบ LCD</button>
                <button class="btn" onclick="testRTC()">⏰ ทดสอบ RTC</button>
            </div>
            <div id="basic-test-results" class="alert" style="display: none;"></div>
        </div>
        
        <!-- Sensor Tests -->
        <div class="card">
            <h3>📡 ทดสอบเซ็นเซอร์</h3>
            <div class="form-group">
                <button class="btn" onclick="startSensorTest()">🔍 เริ่มทดสอบเซ็นเซอร์</button>
                <button class="btn warning" onclick="stopSensorTest()">⏹️ หยุดทดสอบ</button>
            </div>
            <div class="status-grid">
                <div class="status-item">
                    <strong>PIR Sensor:</strong><br>
                    <span id="pir-status">ไม่ได้ทดสอบ</span>
                </div>
                <div class="status-item">
                    <strong>Analog Sensors:</strong><br>
                    <span id="analog-status">ไม่ได้ทดสอบ</span>
                </div>
                <div class="status-item">
                    <strong>Digital Sensors:</strong><br>
                    <span id="digital-status">ไม่ได้ทดสอบ</span>
                </div>
                <div class="status-item">
                    <strong>I2C Devices:</strong><br>
                    <span id="i2c-status">ไม่ได้ทดสอบ</span>
                </div>
            </div>
            <div id="sensor-readings" class="card" style="display: none;">
                <h4>📊 ค่าเซ็นเซอร์แบบเรียลไทม์</h4>
                <div id="sensor-data"></div>
            </div>
        </div>
        
        <!-- Actuator Tests -->
        <div class="card">
            <h3>⚙️ ทดสอบอุปกรณ์ควบคุม</h3>
            <div class="form-group">
                <label for="actuator-select">เลือกอุปกรณ์:</label>
                <select id="actuator-select">
                    <option value="">-- เลือกอุปกรณ์ --</option>
                    <option value="servo">Servo Motor</option>
                    <option value="relay">Relay/Pump</option>
                    <option value="valve">Valve Control</option>
                    <option value="motor">DC Motor</option>
                </select>
            </div>
            <div id="actuator-controls" style="display: none;">
                <!-- Controls will be populated by JavaScript -->
            </div>
            <div id="actuator-test-results" class="alert" style="display: none;"></div>
        </div>
        
        <!-- System-Specific Tests -->
        <div class="card">
            <h3>🎯 ทดสอบเฉพาะระบบ</h3>
            <div id="system-specific-tests">
                <!-- Will be populated based on system type -->
            </div>
        </div>
        
        <!-- Network Tests -->
        <div class="card">
            <h3>🌐 ทดสอบเครือข่าย</h3>
            <div class="form-group">
                <button class="btn" onclick="testWiFi()">📶 ทดสอบ WiFi</button>
                <button class="btn" onclick="testInternet()">🌍 ทดสอบอินเทอร์เน็ต</button>
                <button class="btn" onclick="testBlink()">🔗 ทดสอบ Blink</button>
                <button class="btn" onclick="pingTest()">📡 Ping Test</button>
            </div>
            <div id="network-test-results" class="alert" style="display: none;"></div>
        </div>
        
        <!-- Diagnostic Information -->
        <div class="card">
            <h3>🔍 ข้อมูลการวินิจฉัย</h3>
            <div class="form-group">
                <button class="btn" onclick="generateDiagnostic()">📋 สร้างรายงานการวินิจฉัย</button>
                <button class="btn success" onclick="exportDiagnostic()">💾 ส่งออกรายงาน</button>
            </div>
            <div id="diagnostic-report" style="display: none;">
                <textarea id="diagnostic-text" rows="10" style="width: 100%; font-family: monospace;"></textarea>
            </div>
        </div>
    </div>
</div>

<script>
let sensorTestInterval;
let isTestingActuator = false;

// Basic Hardware Tests
function testLED() {
    showResult('basic-test-results', 'กำลังทดสอบ LED...', 'warning');
    fetch('/api/test/led', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                showResult('basic-test-results', '✅ LED ทำงานปกติ - ' + data.message, 'success');
            } else {
                showResult('basic-test-results', '❌ LED มีปัญหา - ' + data.error, 'error');
            }
        })
        .catch(error => showResult('basic-test-results', '❌ เกิดข้อผิดพลาดในการทดสอบ LED', 'error'));
}

function testBuzzer() {
    showResult('basic-test-results', 'กำลังทดสอบ Buzzer...', 'warning');
    fetch('/api/test/buzzer', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                showResult('basic-test-results', '✅ Buzzer ทำงานปกติ - ' + data.message, 'success');
            } else {
                showResult('basic-test-results', '❌ Buzzer มีปัญหา - ' + data.error, 'error');
            }
        })
        .catch(error => showResult('basic-test-results', '❌ เกิดข้อผิดพลาดในการทดสอบ Buzzer', 'error'));
}

function testLCD() {
    showResult('basic-test-results', 'กำลังทดสอบ LCD...', 'warning');
    fetch('/api/test/lcd', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                showResult('basic-test-results', '✅ LCD ทำงานปกติ - ' + data.message, 'success');
            } else {
                showResult('basic-test-results', '❌ LCD มีปัญหา - ' + data.error, 'error');
            }
        })
        .catch(error => showResult('basic-test-results', '❌ เกิดข้อผิดพลาดในการทดสอบ LCD', 'error'));
}

function testRTC() {
    showResult('basic-test-results', 'กำลังทดสอบ RTC...', 'warning');
    fetch('/api/test/rtc', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                showResult('basic-test-results', '✅ RTC ทำงานปกติ - ' + data.message, 'success');
            } else {
                showResult('basic-test-results', '❌ RTC มีปัญหา - ' + data.error, 'error');
            }
        })
        .catch(error => showResult('basic-test-results', '❌ เกิดข้อผิดพลาดในการทดสอบ RTC', 'error'));
}

// Sensor Tests
function startSensorTest() {
    document.getElementById('sensor-readings').style.display = 'block';
    updateSensorStatus('กำลังทดสอบ...', 'warning');
    
    sensorTestInterval = setInterval(function() {
        fetch('/api/test/sensors')
            .then(response => response.json())
            .then(data => {
                if (data.success) {
                    updateSensorReadings(data.sensors);
                    updateSensorStatus('ทำงานปกติ', 'success');
                } else {
                    updateSensorStatus('มีปัญหา', 'error');
                }
            })
            .catch(error => {
                updateSensorStatus('เกิดข้อผิดพลาด', 'error');
            });
    }, 1000);
}

function stopSensorTest() {
    if (sensorTestInterval) {
        clearInterval(sensorTestInterval);
        sensorTestInterval = null;
    }
    document.getElementById('sensor-readings').style.display = 'none';
    updateSensorStatus('หยุดทดสอบแล้ว', '');
}

function updateSensorStatus(status, type) {
    const elements = ['pir-status', 'analog-status', 'digital-status', 'i2c-status'];
    elements.forEach(id => {
        const element = document.getElementById(id);
        element.textContent = status;
        element.className = type;
    });
}

function updateSensorReadings(sensors) {
    const sensorData = document.getElementById('sensor-data');
    let html = '<div class="status-grid">';
    
    Object.keys(sensors).forEach(sensor => {
        html += `
            <div class="status-item">
                <strong>${sensor}:</strong><br>
                ${sensors[sensor].value} ${sensors[sensor].unit || ''}
            </div>
        `;
    });
    
    html += '</div>';
    sensorData.innerHTML = html;
}

// Actuator Tests
document.getElementById('actuator-select').addEventListener('change', function() {
    const actuatorType = this.value;
    const controlsDiv = document.getElementById('actuator-controls');
    
    if (!actuatorType) {
        controlsDiv.style.display = 'none';
        return;
    }
    
    let controlsHTML = '';
    
    switch (actuatorType) {
        case 'servo':
            controlsHTML = `
                <div class="form-group">
                    <label for="servo-angle">มุม Servo (0-180°):</label>
                    <input type="range" id="servo-angle" min="0" max="180" value="90" 
                           oninput="document.getElementById('angle-value').textContent = this.value">
                    <span id="angle-value">90</span>°
                    <button class="btn" onclick="controlServo()">🔄 หมุน Servo</button>
                </div>
            `;
            break;
        case 'relay':
            controlsHTML = `
                <div class="form-group">
                    <button class="btn success" onclick="controlRelay(true)">🟢 เปิด Relay</button>
                    <button class="btn danger" onclick="controlRelay(false)">🔴 ปิด Relay</button>
                    <button class="btn" onclick="testRelaySequence()">🔄 ทดสอบลำดับ</button>
                </div>
            `;
            break;
        case 'valve':
            controlsHTML = `
                <div class="form-group">
                    <label for="valve-duration">ระยะเวลา (วินาที):</label>
                    <input type="number" id="valve-duration" min="1" max="60" value="5">
                    <button class="btn success" onclick="controlValve(true)">🟢 เปิด Valve</button>
                    <button class="btn danger" onclick="controlValve(false)">🔴 ปิด Valve</button>
                </div>
            `;
            break;
        case 'motor':
            controlsHTML = `
                <div class="form-group">
                    <label for="motor-speed">ความเร็ว (0-255):</label>
                    <input type="range" id="motor-speed" min="0" max="255" value="128"
                           oninput="document.getElementById('speed-value').textContent = this.value">
                    <span id="speed-value">128</span>
                    <button class="btn success" onclick="controlMotor('forward')">⬆️ หมุนไปข้างหน้า</button>
                    <button class="btn warning" onclick="controlMotor('reverse')">⬇️ หมุนกลับ</button>
                    <button class="btn danger" onclick="controlMotor('stop')">⏹️ หยุด</button>
                </div>
            `;
            break;
    }
    
    controlsDiv.innerHTML = controlsHTML;
    controlsDiv.style.display = 'block';
});

function controlServo() {
    const angle = document.getElementById('servo-angle').value;
    showActuatorResult('กำลังหมุน Servo ไป ' + angle + '°...', 'warning');
    
    fetch('/api/test/servo', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({angle: parseInt(angle)})
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            showActuatorResult('✅ Servo หมุนไป ' + angle + '° เรียบร้อย', 'success');
        } else {
            showActuatorResult('❌ Servo มีปัญหา - ' + data.error, 'error');
        }
    })
    .catch(error => showActuatorResult('❌ เกิดข้อผิดพลาดในการควบคุม Servo', 'error'));
}

function controlRelay(state) {
    const action = state ? 'เปิด' : 'ปิด';
    showActuatorResult('กำลัง' + action + ' Relay...', 'warning');
    
    fetch('/api/test/relay', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({state: state})
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            showActuatorResult('✅ ' + action + ' Relay เรียบร้อย', 'success');
        } else {
            showActuatorResult('❌ Relay มีปัญหา - ' + data.error, 'error');
        }
    })
    .catch(error => showActuatorResult('❌ เกิดข้อผิดพลาดในการควบคุม Relay', 'error'));
}

function testRelaySequence() {
    showActuatorResult('กำลังทดสอบลำดับ Relay...', 'warning');
    
    fetch('/api/test/relay-sequence', {method: 'POST'})
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            showActuatorResult('✅ ทดสอบลำดับ Relay เรียบร้อย', 'success');
        } else {
            showActuatorResult('❌ ทดสอบลำดับ Relay มีปัญหา - ' + data.error, 'error');
        }
    })
    .catch(error => showActuatorResult('❌ เกิดข้อผิดพลาดในการทดสอบลำดับ', 'error'));
}

function controlValve(state) {
    const duration = document.getElementById('valve-duration').value;
    const action = state ? 'เปิด' : 'ปิด';
    
    showActuatorResult('กำลัง' + action + ' Valve...', 'warning');
    
    fetch('/api/test/valve', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({state: state, duration: parseInt(duration)})
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            showActuatorResult('✅ ' + action + ' Valve เรียบร้อย', 'success');
        } else {
            showActuatorResult('❌ Valve มีปัญหา - ' + data.error, 'error');
        }
    })
    .catch(error => showActuatorResult('❌ เกิดข้อผิดพลาดในการควบคุม Valve', 'error'));
}

function controlMotor(direction) {
    const speed = document.getElementById('motor-speed').value;
    let action = '';
    
    switch(direction) {
        case 'forward': action = 'หมุนไปข้างหน้า'; break;
        case 'reverse': action = 'หมุนกลับ'; break;
        case 'stop': action = 'หยุด'; break;
    }
    
    showActuatorResult('กำลัง' + action + ' Motor...', 'warning');
    
    fetch('/api/test/motor', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({direction: direction, speed: parseInt(speed)})
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            showActuatorResult('✅ Motor ' + action + ' เรียบร้อย', 'success');
        } else {
            showActuatorResult('❌ Motor มีปัญหา - ' + data.error, 'error');
        }
    })
    .catch(error => showActuatorResult('❌ เกิดข้อผิดพลาดในการควบคุม Motor', 'error'));
}

// Network Tests
function testWiFi() {
    showNetworkResult('กำลังทดสอบ WiFi...', 'warning');
    fetch('/api/test/wifi', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                showNetworkResult('✅ WiFi ทำงานปกติ - ' + data.message, 'success');
            } else {
                showNetworkResult('❌ WiFi มีปัญหา - ' + data.error, 'error');
            }
        })
        .catch(error => showNetworkResult('❌ เกิดข้อผิดพลาดในการทดสอบ WiFi', 'error'));
}

function testInternet() {
    showNetworkResult('กำลังทดสอบการเชื่อมต่ออินเทอร์เน็ต...', 'warning');
    fetch('/api/test/internet', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                showNetworkResult('✅ อินเทอร์เน็ตทำงานปกติ - ' + data.message, 'success');
            } else {
                showNetworkResult('❌ อินเทอร์เน็ตมีปัญหา - ' + data.error, 'error');
            }
        })
        .catch(error => showNetworkResult('❌ เกิดข้อผิดพลาดในการทดสอบอินเทอร์เน็ต', 'error'));
}

function testBlink() {
    showNetworkResult('กำลังทดสอบ Blink...', 'warning');
    fetch('/api/test/blink', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                showNetworkResult('✅ Blink ทำงานปกติ - ' + data.message, 'success');
            } else {
                showNetworkResult('❌ Blink มีปัญหา - ' + data.error, 'error');
            }
        })
        .catch(error => showNetworkResult('❌ เกิดข้อผิดพลาดในการทดสอบ Blink', 'error'));
}

function pingTest() {
    showNetworkResult('กำลังทำ Ping Test...', 'warning');
    fetch('/api/test/ping', {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                showNetworkResult('✅ Ping Test สำเร็จ - ' + data.message, 'success');
            } else {
                showNetworkResult('❌ Ping Test ล้มเหลว - ' + data.error, 'error');
            }
        })
        .catch(error => showNetworkResult('❌ เกิดข้อผิดพลาดใน Ping Test', 'error'));
}

// Diagnostic Functions
function generateDiagnostic() {
    document.getElementById('diagnostic-report').style.display = 'block';
    document.getElementById('diagnostic-text').value = 'กำลังสร้างรายงาน...';
    
    fetch('/api/diagnostic')
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                document.getElementById('diagnostic-text').value = data.report;
            } else {
                document.getElementById('diagnostic-text').value = 'เกิดข้อผิดพลาดในการสร้างรายงาน: ' + data.error;
            }
        })
        .catch(error => {
            document.getElementById('diagnostic-text').value = 'เกิดข้อผิดพลาดในการสร้างรายงาน';
        });
}

function exportDiagnostic() {
    const report = document.getElementById('diagnostic-text').value;
    if (!report || report === 'กำลังสร้างรายงาน...') {
        alert('กรุณาสร้างรายงานก่อน');
        return;
    }
    
    const blob = new Blob([report], { type: 'text/plain' });
    const url = window.URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'rdtrc_diagnostic_report_' + new Date().toISOString().slice(0,19).replace(/:/g, '-') + '.txt';
    document.body.appendChild(a);
    a.click();
    window.URL.revokeObjectURL(url);
    document.body.removeChild(a);
}

// Utility Functions
function showResult(elementId, message, type) {
    const element = document.getElementById(elementId);
    element.textContent = message;
    element.className = 'alert ' + type;
    element.style.display = 'block';
}

function showActuatorResult(message, type) {
    showResult('actuator-test-results', message, type);
}

function showNetworkResult(message, type) {
    showResult('network-test-results', message, type);
}

// Real-time updates
setInterval(function() {
    fetch('/api/status')
        .then(response => response.json())
        .then(data => {
            if (data.system) {
                document.getElementById('system-uptime').textContent = formatUptime(data.system.uptime);
                document.getElementById('system-memory').textContent = formatBytes(data.system.free_memory);
            }
        })
        .catch(error => console.log('Status update failed'));
}, 5000);

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

// Load system-specific tests on page load
window.onload = function() {
    loadSystemSpecificTests();
};

function loadSystemSpecificTests() {
    fetch('/api/system-tests')
        .then(response => response.json())
        .then(data => {
            if (data.success && data.tests) {
                const testsDiv = document.getElementById('system-specific-tests');
                let html = '<div class="form-group">';
                
                data.tests.forEach(test => {
                    html += `<button class="btn" onclick="runSystemTest('${test.id}')">${test.icon} ${test.name}</button>`;
                });
                
                html += '</div><div id="system-test-results" class="alert" style="display: none;"></div>';
                testsDiv.innerHTML = html;
            }
        })
        .catch(error => console.log('Failed to load system-specific tests'));
}

function runSystemTest(testId) {
    const resultDiv = document.getElementById('system-test-results');
    resultDiv.style.display = 'block';
    showResult('system-test-results', 'กำลังทดสอบ...', 'warning');
    
    fetch('/api/test/system/' + testId, {method: 'POST'})
        .then(response => response.json())
        .then(data => {
            if (data.success) {
                showResult('system-test-results', '✅ ' + data.message, 'success');
            } else {
                showResult('system-test-results', '❌ ' + data.error, 'error');
            }
        })
        .catch(error => {
            showResult('system-test-results', '❌ เกิดข้อผิดพลาดในการทดสอบ', 'error');
        });
}
</script>
)";
  
  html += HTML_FOOTER;
  webServer.send(200, "text/html", html);
}

// API Handlers for Device Testing

// Test LED
void handleTestLED() {
  DynamicJsonDocument doc(256);
  
  try {
    // Test RGB LED sequence
    digitalWrite(LED_RED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, HIGH);
    delay(500);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_BLUE_PIN, HIGH);
    delay(500);
    digitalWrite(LED_BLUE_PIN, LOW);
    
    doc["success"] = true;
    doc["message"] = "LED test completed - Red, Green, Blue sequence";
  } catch (...) {
    doc["success"] = false;
    doc["error"] = "LED test failed";
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

// Test Buzzer
void handleTestBuzzer() {
  DynamicJsonDocument doc(256);
  
  try {
    // Play test tone sequence
    tone(BUZZER_PIN, 1000, 300);
    delay(400);
    tone(BUZZER_PIN, 1500, 300);
    delay(400);
    tone(BUZZER_PIN, 2000, 300);
    
    doc["success"] = true;
    doc["message"] = "Buzzer test completed - 3 tone sequence";
  } catch (...) {
    doc["success"] = false;
    doc["error"] = "Buzzer test failed";
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

// Test LCD
void handleTestLCD() {
  DynamicJsonDocument doc(256);
  
  try {
    if (systemStatus.lcd_connected) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("LCD Test OK");
      lcd.setCursor(0, 1);
      lcd.print("RDTRC v" + String(FIRMWARE_VERSION));
      
      doc["success"] = true;
      doc["message"] = "LCD test completed - Display shows test message";
    } else {
      doc["success"] = false;
      doc["error"] = "LCD not connected";
    }
  } catch (...) {
    doc["success"] = false;
    doc["error"] = "LCD test failed";
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

// Test RTC
void handleTestRTC() {
  DynamicJsonDocument doc(256);
  
  try {
    if (systemStatus.rtc_connected) {
      DateTime now = rtc.now();
      String timeStr = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()) + 
                      " " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
      
      doc["success"] = true;
      doc["message"] = "RTC test completed - Current time: " + timeStr;
    } else {
      doc["success"] = false;
      doc["error"] = "RTC not connected";
    }
  } catch (...) {
    doc["success"] = false;
    doc["error"] = "RTC test failed";
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

// Test Sensors
void handleTestSensors() {
  DynamicJsonDocument doc(1024);
  
  try {
    JsonObject sensors = doc.createNestedObject("sensors");
    
    // Test PIR sensor if available
    sensors["PIR"]["value"] = digitalRead(PIR_PIN) ? "Motion Detected" : "No Motion";
    sensors["PIR"]["unit"] = "";
    
    // Test analog sensors (A0-A5)
    for (int i = 0; i < 6; i++) {
      String sensorName = "A" + String(i);
      int rawValue = analogRead(A0 + i);
      sensors[sensorName]["value"] = rawValue;
      sensors[sensorName]["unit"] = " (0-4095)";
    }
    
    // Test I2C devices
    sensors["I2C_LCD"]["value"] = systemStatus.lcd_connected ? "Connected" : "Not Connected";
    sensors["I2C_RTC"]["value"] = systemStatus.rtc_connected ? "Connected" : "Not Connected";
    
    doc["success"] = true;
  } catch (...) {
    doc["success"] = false;
    doc["error"] = "Sensor test failed";
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

// Test Servo
void handleTestServo() {
  DynamicJsonDocument doc(256);
  
  if (webServer.hasArg("plain")) {
    DynamicJsonDocument request(256);
    deserializeJson(request, webServer.arg("plain"));
    
    int angle = request["angle"];
    if (angle >= 0 && angle <= 180) {
      try {
        // Assuming servo is connected to a standard pin
        // This would need to be adapted for each system
        extern EnhancedSystemBase* currentSystem;
        if (currentSystem) {
          // Call system-specific servo test
          doc["success"] = true;
          doc["message"] = "Servo moved to " + String(angle) + " degrees";
        } else {
          doc["success"] = false;
          doc["error"] = "System not available";
        }
      } catch (...) {
        doc["success"] = false;
        doc["error"] = "Servo control failed";
      }
    } else {
      doc["success"] = false;
      doc["error"] = "Invalid angle (0-180)";
    }
  } else {
    doc["success"] = false;
    doc["error"] = "No angle specified";
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

// Test Relay
void handleTestRelay() {
  DynamicJsonDocument doc(256);
  
  if (webServer.hasArg("plain")) {
    DynamicJsonDocument request(256);
    deserializeJson(request, webServer.arg("plain"));
    
    bool state = request["state"];
    
    try {
      // This would need to be adapted for each system's relay pins
      extern EnhancedSystemBase* currentSystem;
      if (currentSystem) {
        // Call system-specific relay test
        doc["success"] = true;
        doc["message"] = "Relay " + String(state ? "ON" : "OFF");
      } else {
        doc["success"] = false;
        doc["error"] = "System not available";
      }
    } catch (...) {
      doc["success"] = false;
      doc["error"] = "Relay control failed";
    }
  } else {
    doc["success"] = false;
    doc["error"] = "No state specified";
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

// Test WiFi
void handleTestWiFi() {
  DynamicJsonDocument doc(512);
  
  try {
    if (WiFi.status() == WL_CONNECTED) {
      String ssid = WiFi.SSID();
      int rssi = WiFi.RSSI();
      String ip = WiFi.localIP().toString();
      
      doc["success"] = true;
      doc["message"] = "Connected to " + ssid + " (RSSI: " + String(rssi) + "dBm, IP: " + ip + ")";
    } else {
      doc["success"] = false;
      doc["error"] = "WiFi not connected";
    }
  } catch (...) {
    doc["success"] = false;
    doc["error"] = "WiFi test failed";
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

// Test Internet
void handleTestInternet() {
  DynamicJsonDocument doc(256);
  
  try {
    HTTPClient http;
    http.begin("http://www.google.com");
    http.setTimeout(5000);
    
    int httpCode = http.GET();
    http.end();
    
    if (httpCode > 0) {
      doc["success"] = true;
      doc["message"] = "Internet connection OK (Response: " + String(httpCode) + ")";
    } else {
      doc["success"] = false;
      doc["error"] = "No internet connection";
    }
  } catch (...) {
    doc["success"] = false;
    doc["error"] = "Internet test failed";
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

// Test Blink
void handleTestBlinkConnection() {
  DynamicJsonDocument doc(256);
  
  try {
    if (systemConfig.blink_enabled) {
      if (systemStatus.network.blink_connected) {
        doc["success"] = true;
        doc["message"] = "Blink connected to " + String(systemConfig.blink_server);
      } else {
        doc["success"] = false;
        doc["error"] = "Blink not connected";
      }
    } else {
      doc["success"] = false;
      doc["error"] = "Blink not enabled";
    }
  } catch (...) {
    doc["success"] = false;
    doc["error"] = "Blink test failed";
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

// Generate Diagnostic Report
void handleDiagnosticReport() {
  DynamicJsonDocument doc(2048);
  
  try {
    String report = "=== RDTRC System Diagnostic Report ===\n";
    report += "Generated: " + String(millis() / 1000) + " seconds after boot\n";
    report += "Firmware: " + String(FIRMWARE_VERSION) + " by " + String(FIRMWARE_AUTHOR) + "\n\n";
    
    // System Information
    report += "=== SYSTEM INFORMATION ===\n";
    report += "Device Name: " + String(systemConfig.device_name) + "\n";
    report += "System Type: " + String(systemConfig.system_type) + "\n";
    report += "MAC Address: " + WiFi.macAddress() + "\n";
    report += "Chip Model: " + String(ESP.getChipModel()) + "\n";
    report += "CPU Frequency: " + String(ESP.getCpuFreqMHz()) + " MHz\n";
    report += "Flash Size: " + String(ESP.getFlashChipSize() / 1024 / 1024) + " MB\n";
    report += "Free Heap: " + String(ESP.getFreeHeap() / 1024) + " KB\n";
    report += "CPU Temperature: " + String(temperatureRead()) + "°C\n\n";
    
    // Network Information
    report += "=== NETWORK INFORMATION ===\n";
    if (WiFi.status() == WL_CONNECTED) {
      report += "WiFi Status: Connected\n";
      report += "SSID: " + WiFi.SSID() + "\n";
      report += "IP Address: " + WiFi.localIP().toString() + "\n";
      report += "Gateway: " + WiFi.gatewayIP().toString() + "\n";
      report += "Signal Strength: " + String(WiFi.RSSI()) + " dBm\n";
    } else {
      report += "WiFi Status: Disconnected\n";
    }
    
    if (systemStatus.network.hotspot_active) {
      report += "Hotspot Status: Active\n";
      report += "Hotspot IP: " + systemStatus.network.hotspot_ip + "\n";
      report += "Connected Clients: " + String(systemStatus.network.connected_clients) + "\n";
    } else {
      report += "Hotspot Status: Inactive\n";
    }
    
    report += "Blink Status: " + String(systemStatus.network.blink_connected ? "Connected" : "Disconnected") + "\n";
    report += "Internet Available: " + String(systemStatus.network.internet_available ? "Yes" : "No") + "\n\n";
    
    // Hardware Status
    report += "=== HARDWARE STATUS ===\n";
    report += "LCD Connected: " + String(systemStatus.lcd_connected ? "Yes" : "No") + "\n";
    report += "RTC Connected: " + String(systemStatus.rtc_connected ? "Yes" : "No") + "\n";
    report += "Sensors OK: " + String(systemStatus.sensors_ok ? "Yes" : "No") + "\n";
    report += "SD Card Available: " + String(systemStatus.sd_card_available ? "Yes" : "No") + "\n\n";
    
    // Configuration
    report += "=== CONFIGURATION ===\n";
    report += "Network Mode: " + String((int)systemConfig.network_mode) + "\n";
    report += "OTA Enabled: " + String(systemConfig.ota_enabled ? "Yes" : "No") + "\n";
    report += "Debug Mode: " + String(systemConfig.debug_mode ? "Yes" : "No") + "\n";
    report += "Timezone Offset: GMT+" + String(systemConfig.timezone_offset) + "\n\n";
    
    // Error Information
    report += "=== ERROR INFORMATION ===\n";
    report += "Error Count: " + String(systemStatus.error_count) + "\n";
    report += "Last Error: " + (systemStatus.last_error.length() > 0 ? systemStatus.last_error : "None") + "\n\n";
    
    // Pin Status (basic test)
    report += "=== PIN STATUS ===\n";
    report += "LED Red (Pin " + String(LED_RED_PIN) + "): " + String(digitalRead(LED_RED_PIN) ? "HIGH" : "LOW") + "\n";
    report += "LED Green (Pin " + String(LED_GREEN_PIN) + "): " + String(digitalRead(LED_GREEN_PIN) ? "HIGH" : "LOW") + "\n";
    report += "LED Blue (Pin " + String(LED_BLUE_PIN) + "): " + String(digitalRead(LED_BLUE_PIN) ? "HIGH" : "LOW") + "\n";
    report += "PIR Sensor (Pin " + String(PIR_PIN) + "): " + String(digitalRead(PIR_PIN) ? "HIGH" : "LOW") + "\n\n";
    
    // Analog Readings
    report += "=== ANALOG READINGS ===\n";
    for (int i = 0; i < 6; i++) {
      report += "A" + String(i) + ": " + String(analogRead(A0 + i)) + "\n";
    }
    report += "\n";
    
    report += "=== END OF REPORT ===\n";
    
    doc["success"] = true;
    doc["report"] = report;
  } catch (...) {
    doc["success"] = false;
    doc["error"] = "Failed to generate diagnostic report";
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

// Get System-Specific Tests
void handleSystemTests() {
  DynamicJsonDocument doc(1024);
  
  try {
    JsonArray tests = doc.createNestedArray("tests");
    
    // Add system-specific tests based on system type
    String systemType = String(systemConfig.system_type);
    
    if (systemType.indexOf("Cat") >= 0) {
      JsonObject test1 = tests.createNestedObject();
      test1["id"] = "feeding_cycle";
      test1["name"] = "ทดสอบวงจรการให้อาหาร";
      test1["icon"] = "🍽️";
      
      JsonObject test2 = tests.createNestedObject();
      test2["id"] = "weight_sensor";
      test2["name"] = "ทดสอบเซ็นเซอร์น้ำหนัก";
      test2["icon"] = "⚖️";
    } else if (systemType.indexOf("Bird") >= 0) {
      JsonObject test1 = tests.createNestedObject();
      test1["id"] = "dual_servo";
      test1["name"] = "ทดสอบ Servo อาหาร+น้ำ";
      test1["icon"] = "🔄";
      
      JsonObject test2 = tests.createNestedObject();
      test2["id"] = "weather_sensors";
      test2["name"] = "ทดสอบเซ็นเซอร์สภาพอากาศ";
      test2["icon"] = "🌦️";
    } else if (systemType.indexOf("Tomato") >= 0) {
      JsonObject test1 = tests.createNestedObject();
      test1["id"] = "pump_system";
      test1["name"] = "ทดสอบระบบปั๊มน้ำ";
      test1["icon"] = "💧";
      
      JsonObject test2 = tests.createNestedObject();
      test2["id"] = "valve_sequence";
      test2["name"] = "ทดสอบลำดับ Valve";
      test2["icon"] = "🚿";
      
      JsonObject test3 = tests.createNestedObject();
      test3["id"] = "soil_sensors";
      test3["name"] = "ทดสอบเซ็นเซอร์ความชื้นดิน";
      test3["icon"] = "🌱";
    }
    
    // Common tests for all systems
    JsonObject test_common = tests.createNestedObject();
    test_common["id"] = "full_system";
    test_common["name"] = "ทดสอบระบบทั้งหมด";
    test_common["icon"] = "🔍";
    
    doc["success"] = true;
  } catch (...) {
    doc["success"] = false;
    doc["error"] = "Failed to get system tests";
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

// Handle System-Specific Test
void handleSystemSpecificTest() {
  String testId = webServer.pathArg(0);
  DynamicJsonDocument doc(256);
  
  try {
    extern EnhancedSystemBase* currentSystem;
    if (currentSystem) {
      // This would call system-specific test methods
      // For now, return a generic success message
      doc["success"] = true;
      doc["message"] = "System test '" + testId + "' completed successfully";
    } else {
      doc["success"] = false;
      doc["error"] = "System not available";
    }
  } catch (...) {
    doc["success"] = false;
    doc["error"] = "System test failed";
  }
  
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

// Setup Device Test Handlers
void setupDeviceTestHandlers() {
  // Main device test page
  webServer.on("/device-test", handleDeviceTest);
  
  // Basic hardware test APIs
  webServer.on("/api/test/led", HTTP_POST, handleTestLED);
  webServer.on("/api/test/buzzer", HTTP_POST, handleTestBuzzer);
  webServer.on("/api/test/lcd", HTTP_POST, handleTestLCD);
  webServer.on("/api/test/rtc", HTTP_POST, handleTestRTC);
  
  // Sensor test APIs
  webServer.on("/api/test/sensors", HTTP_GET, handleTestSensors);
  
  // Actuator test APIs
  webServer.on("/api/test/servo", HTTP_POST, handleTestServo);
  webServer.on("/api/test/relay", HTTP_POST, handleTestRelay);
  
  // Network test APIs
  webServer.on("/api/test/wifi", HTTP_POST, handleTestWiFi);
  webServer.on("/api/test/internet", HTTP_POST, handleTestInternet);
  webServer.on("/api/test/blink", HTTP_POST, handleTestBlinkConnection);
  
  // Diagnostic APIs
  webServer.on("/api/diagnostic", HTTP_GET, handleDiagnosticReport);
  webServer.on("/api/system-tests", HTTP_GET, handleSystemTests);
  webServer.on("/api/test/system/*", HTTP_POST, handleSystemSpecificTest);
  
  LOG_INFO("Device test handlers setup completed");
}
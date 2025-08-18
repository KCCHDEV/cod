# 📚 ฐานความรู้ครบถ้วน: ระบบรดน้ำเห็ดนางฟ้าและผักชีฟลั่ง

> **เอกสารความรู้เชิงลึก - หลักการทำงาน ทฤษฎี และเหตุผลของทุกส่วนประกอบ**  
> **จัดทำโดย: RDTRC Team**  
> **วันที่: 2024**

---

## 📖 สารบัญ

1. [🍄 ชีววิทยาของเห็ดนางฟ้า](#mushroom-biology)
2. [🌿 พฤกษศาสตร์ของผักชีฟลั่ง](#cilantro-botany)
3. [🔬 หลักการทำงานของเซนเซอร์](#sensor-principles)
4. [⚙️ ทฤษฎีการควบคุมอัตโนมัติ](#control-theory)
5. [🌐 สถาปัตยกรรม IoT](#iot-architecture)
6. [💧 ระบบจัดการน้ำ](#water-management)
7. [🌡️ การควบคุมสภาพแวดล้อม](#environmental-control)
8. [💡 ระบบแสงสว่าง](#lighting-systems)
9. [📊 การประมวลผลข้อมูล](#data-processing)
10. [🔋 การจัดการพลังงาน](#power-management)

---

## 🍄 ชีววิทยาของเห็ดนางฟ้า {#mushroom-biology}

### 🧬 โครงสร้างและวงจรชีวิต

**เห็ดนางฟ้า (Pleurotus ostreatus)** เป็นเชื้อราที่มีวงจรชีวิตซับซ้อน:

```
วงจรชีวิตเห็ดนางฟ้า:
├── Spore (สปอร์) → เริ่มต้นจากสปอร์เดี่ยว
├── Germination (การงอก) → สปอร์งอกเป็นใยเชื้อรา
├── Mycelium (ใยเชื้อรา) → เจริญเติบโตในวัสดุเพาะ
├── Primordium (หัวเห็ดเล็ก) → การก่อตัวของหัวเห็ด
├── Pin Stage (ระยะหัวเห็ด) → หัวเห็ดเริ่มมีรูปร่าง
├── Fruiting (ระยะออกผล) → เห็ดเจริญเติบโตเต็มที่
└── Maturation (ระยะสุก) → เห็ดสุกและปล่อยสปอร์
```

### 🌡️ ความต้องการทางสิ่งแวดล้อม

**1. ความชื้น (Humidity): 80-95%**

*เหตุผล:*
- เห็ดไม่มีผิวหนังป้องกันการสูญเสียน้ำ
- ใยเชื้อราต้องการความชื้นสูงในการเจริญเติบโต
- การก่อตัวของหัวเห็ด (Pinning) ต้องการความชื้นคงที่

*หลักการทำงาน:*
```
ความชื้น < 80%: 
├── ใยเชื้อราแห้ง → หยุดการเจริญเติบโต
├── หัวเห็ดร่วง → ไม่เกิดการออกผล
└── เห็ดแกร่ง/แข็ง → คุณภาพต่ำ

ความชื้น > 95%:
├── เกิดแบคทีเรีย → การเน่าเสีย
├── เกิดเชื้อราอื่น → การปนเปื้อน
└── หยดน้ำบนเห็ด → รูปร่างผิดปกติ
```

**2. อุณหภูมิ (Temperature): 20-25°C**

*เหตุผล:*
- เอนไซม์ในเชื้อราทำงานได้ดีที่สุดในช่วงนี้
- การสังเคราะห์โปรตีนและคาร์โบไฮเดรตเหมาะสม
- การแบ่งเซลล์เกิดขึ้นอย่างสม่ำเสมอ

*หลักการทำงาน:*
```
อุณหภูมิ < 20°C:
├── เมแทบอลิซึมช้า → การเจริญเติบโตล่าช้า
├── การก่อตัวหัวเห็ดลดลง → ผลผลิตต่ำ
└── ความต้านทานโรคลดลง

อุณหภูมิ > 25°C:
├── การหายใจเพิ่มขึ้น → สูญเสียพลังงาน
├── ความเสี่ยงต่อแบคทีเรีย → การปนเปื้อน
└── เห็ดเจริญเร็วเกินไป → คุณภาพต่ำ
```

**3. แสง (Light): 500-1000 Lux, 12 ชั่วโมง**

*เหตุผล:*
- แสงกระตุ้นการก่อตัวของหัวเห็ด (Photomorphogenesis)
- ช่วยในการสร้างเม็ดสี (Pigmentation)
- ควบคุมทิศทางการเจริญเติบโต (Phototropism)

### 🔬 กระบวนการทางชีวเคมี

**การหายใจของเชื้อรา:**
```
C₆H₁₂O₆ + 6O₂ → 6CO₂ + 6H₂O + ATP
(กลูโคส + ออกซิเจน → คาร์บอนไดออกไซด์ + น้ำ + พลังงาน)
```

*ความสำคัญ:*
- ต้องการออกซิเจนอย่างต่อเนื่อง
- ผลิต CO₂ ที่ต้องระบายออก
- สร้างความร้อนที่ต้องควบคุม

**การสร้างโปรตีน:**
- เห็ดนางฟ้ามีโปรตีน 15-25% ของน้ำหนักแห้ง
- มีกรดอะมิโนครบ 9 ชนิดที่จำเป็น
- การสังเคราะห์ต้องการไนโตรเจนจากวัสดุเพาะ

---

## 🌿 พฤกษศาสตร์ของผักชีฟลั่ง {#cilantro-botany}

### 🌱 โครงสร้างและการเจริญเติบโต

**ผักชีฟลั่ง (Coriandrum sativum)** เป็นพืชตระกูลร่ม (Apiaceae):

```
โครงสร้างผักชีฟลั่ง:
├── ระบบราก (Root System)
│   ├── รากแก้ว (Taproot) → ดูดซับน้ำและธาตุอาหาร
│   └── รากแขนง (Lateral roots) → เพิ่มพื้นที่ดูดซับ
├── ลำต้น (Stem)
│   ├── ข้อและปล้อง (Nodes & Internodes)
│   └── จุดแตกใหม่ (Growing points)
├── ใบ (Leaves)
│   ├── ใบเด็ด (Compound leaves)
│   └── แผ่นใบ (Leaflets)
└── ดอกและผล (Flowers & Seeds)
    ├── ช่อดอก (Umbels)
    └── เมล็ด (Coriander seeds)
```

### 💧 ความต้องการน้ำ

**ความชื้นดิน: 60-75%**

*เหตุผล:*
- รากผักชีฟลั่งไม่ลึก (15-20 cm) ต้องการน้ำในชั้นบน
- ใบมีพื้นผิวมาก ทำให้สูญเสียน้ำผ่านการคายน้ำ
- การสร้างใบใหม่ต้องการน้ำอย่างต่อเนื่อง

*หลักการทำงาน:*
```
ความชื้นดิน < 60%:
├── Wilting (การเหี่ยวแห้ง) → ใบเหี่ยว
├── Stomatal closure → หยุดการสังเคราะห์แสง
├── Growth cessation → หยุดการเจริญเติบโต
└── Early bolting → ออกดอกก่อนกำหนด

ความชื้นดิน > 75%:
├── Root rot (รากเน่า) → เชื้อราในดิน
├── Nutrient leaching → สูญเสียธาตุอาหาร
├── Poor aeration → ออกซิเจนไม่พอ
└── Fungal diseases → โรคใบไหม้
```

### 🌡️ ความต้องการอุณหภูมิ

**อุณหภูมิ: 17-27°C**

*เหตุผล:*
- ผักชีฟลั่งเป็นพืชอากาศเย็น (Cool-season crop)
- เอนไซม์ Rubisco ทำงานได้ดีในอุณหภูมิปานกลาง
- อุณหภูมิสูงกระตุ้นการออกดอก (Bolting)

*กระบวนการสังเคราะห์แสง:*
```
6CO₂ + 6H₂O + แสง → C₆H₁₂O₆ + 6O₂
(คาร์บอนไดออกไซด์ + น้ำ + แสง → กลูโคส + ออกซิเจน)
```

### 🌞 ความต้องการแสง

**ความเข้มแสง: 25,000-35,000 Lux, 6-8 ชั่วโมง**

*เหตุผล:*
- ผักชีฟลั่งเป็นพืช C₃ ต้องการแสงปานกลาง
- แสงมากเกินไปทำให้ใบแกร่งและขม
- แสงน้อยเกินไปทำให้ใบเหลืองและเจริญเติบโตช้า

### 🧪 กระบวนการทางสรีรวิทยา

**การคายน้ำ (Transpiration):**
- ผักชีฟลั่งคายน้ำ 200-300 กรัม ต่อ กรัมน้ำหนักแห้ง
- ช่วยดูดธาตุอาหารขึ้นจากราก
- ช่วยระบายความร้อนจากใบ

**การดูดซับธาตุอาหาร:**
```
ธาตุอาหารหลัก (NPK):
├── N (ไนโตรเจน) → สร้างโปรตีนและคลอโรฟิลล์
├── P (ฟอสฟอรัส) → การถ่ายทอดพลังงาน (ATP)
└── K (โพแทสเซียม) → การควบคุมปากใบและเอนไซม์

ธาตุอาหารรอง:
├── Ca (แคลเซียม) → โครงสร้างผนังเซลล์
├── Mg (แมกนีเซียม) → ศูนย์กลางคลอโรฟิลล์
└── S (กำมะถัน) → สร้างกรดอะมิโนที่มีกำมะถัน
```

---

## 🔬 หลักการทำงานของเซนเซอร์ {#sensor-principles}

### 🌡️ DHT22 Temperature & Humidity Sensor

**หลักการทำงาน:**

*การวัดความชื้น (Capacitive Humidity Sensing):*
```
วัสดุ Polymer → ดูดซับไอน้ำ → เปลี่ยนค่า Dielectric Constant
→ เปลี่ยน Capacitance → แปลงเป็นสัญญาณดิจิทัล
```

*การวัดอุณหภูมิ (Thermistor):*
```
NTC Thermistor → ความต้านทานลดลงเมื่ออุณหภูมิสูงขึ้น
→ วัดกระแสไฟฟ้า → คำนวณอุณหภูมิ
```

**สมการความสัมพันธ์:**
```
RT = R₀ × exp(B × (1/T - 1/T₀))
โดย:
RT = ความต้านทานที่อุณหภูมิ T
R₀ = ความต้านทานที่อุณหภูมิอ้างอิง T₀
B = ค่าคงที่ของวัสดุ
T = อุณหภูมิสัมบูรณ์ (Kelvin)
```

**ความแม่นยำและข้อจำกัด:**
- ความแม่นยำ: ±0.5°C สำหรับอุณหภูมิ, ±2-5% สำหรับความชื้น
- เวลาตอบสนอง: 2 วินาที
- ช่วงการทำงาน: -40°C ถึง 80°C, 0-100% RH

### 💧 Soil Moisture Sensor (Capacitive)

**หลักการทำงาน:**
```
Capacitive Sensing:
C = ε₀ × εᵣ × A / d

โดย:
C = Capacitance
ε₀ = Permittivity of free space
εᵣ = Relative permittivity of soil
A = Area of sensor plates  
d = Distance between plates
```

*การทำงาน:*
1. **ดินแห้ง:** εᵣ ≈ 3-5 → Capacitance ต่ำ
2. **ดินเปียก:** εᵣ ≈ 80 (ของน้ำ) → Capacitance สูง
3. **วงจรออสซิลเลเตอร์:** แปลง Capacitance เป็นความถี่
4. **ADC:** แปลงความถี่เป็นค่าดิจิทัล

**ข้อดีของ Capacitive เทียบกับ Resistive:**
- ไม่มีการกัดกร่อนของขั้วไฟฟ้า
- ไม่ได้รับผลกระทบจากเกลือในดิน
- อายุการใช้งานยาวนาน

### 📏 Water Level Sensor

**หลักการทำงาน (Pressure-based):**
```
Hydrostatic Pressure:
P = ρ × g × h

โดย:
P = Pressure at depth h
ρ = Density of water (1000 kg/m³)
g = Gravitational acceleration (9.81 m/s²)
h = Height of water column
```

*การแปลงสัญญาณ:*
1. **Pressure → Voltage:** ผ่าน Piezoresistive element
2. **Signal Conditioning:** ขยายและกรองสัญญาณ
3. **ADC Conversion:** แปลงเป็นค่าดิจิทัล
4. **Calibration:** แปลงเป็นเปอร์เซ็นต์ระดับน้ำ

### 🌡️ DS18B20 Temperature Probe

**หลักการทำงาน (Digital Temperature Sensor):**

*โครงสร้างภายใน:*
```
DS18B20 Internal Structure:
├── Temperature Sensor Core (Silicon bandgap)
├── ADC (12-bit resolution)
├── Digital Processing Unit
├── Memory (EEPROM for calibration)
└── 1-Wire Interface
```

*การวัดอุณหภูมิ:*
```
Bandgap Reference:
VBG = (kT/q) × ln(N) + Vgo

โดย:
k = Boltzmann constant
T = Absolute temperature
q = Electron charge
N = Current density ratio
Vgo = Extrapolated bandgap voltage
```

**ข้อดี:**
- ความแม่นยำสูง: ±0.5°C
- ไม่ต้องใช้ ADC ภายนอก
- สื่อสารผ่านสาย 1 เส้น
- แต่ละตัวมี Unique ID

### ☀️ Light Sensor (LDR - Light Dependent Resistor)

**หลักการทำงาน (Photoconductivity):**

*การทำงานของ Cadmium Sulfide (CdS):*
```
เมื่อได้รับแสง:
Photon Energy (hν) → Electron excitation
→ เพิ่ม Free electrons → ลดความต้านทาน

ในความมืด:
ไม่มี Photon excitation → น้อย Free electrons
→ ความต้านทานสูง
```

*Voltage Divider Circuit:*
```
Vout = Vin × RLDR / (R + RLDR)

โดย:
Vout = Output voltage to ADC
Vin = Supply voltage (3.3V)
RLDR = LDR resistance (แปรผันตามแสง)
R = Fixed resistor (10kΩ)
```

**ความสัมพันธ์แสงกับความต้านทาน:**
```
RLDR = A × Lux^(-γ)

โดย:
A = ค่าคงที่ขึ้นกับวัสดุ
γ = ค่าคงที่ (ประมาณ 0.7-0.9)
Lux = ความเข้มแสง
```

---

## ⚙️ ทฤษฎีการควบคุมอัตโนมัติ {#control-theory}

### 🎛️ PID Control Theory

**สมการ PID Controller:**
```
u(t) = Kp×e(t) + Ki×∫e(t)dt + Kd×de(t)/dt

โดย:
u(t) = Control output
e(t) = Error (Setpoint - Process Variable)
Kp = Proportional gain
Ki = Integral gain  
Kd = Derivative gain
```

**การประยุกต์ในระบบ:**

*1. Humidity Control (ควบคุมความชื้นเห็ด):*
```
Setpoint = 85% RH
Process Variable = Current humidity from DHT22
Error = 85% - Current humidity

If Error > 0: เปิด Misting pump
If Error < 0: เปิด Exhaust fan
```

*2. Soil Moisture Control (ควบคุมความชื้นดิน):*
```
Setpoint = 65% moisture
Process Variable = Soil moisture reading
Error = 65% - Current moisture

If Error > 5%: เปิด Water pump (3 minutes)
If Error < -5%: หยุดรดน้ำ (ป้องกันน้ำท่วม)
```

### 🔄 Feedback Control System

**Block Diagram:**
```
Reference → [+] → Controller → Plant → Output
Input        [-]                      ↓
             ↑                        ↓
             ← Sensor ← ← ← ← ← ← ← ← ←
             (Feedback Loop)
```

**ในระบบเห็ดนางฟ้า:**
```
Humidity Setpoint → PID Controller → Misting System → Chamber Humidity
    (85%)              (ESP32)         (Pump+Valve)        ↓
                         ↑                                  ↓
                         ← DHT22 Sensor ← ← ← ← ← ← ← ← ←
```

### 📊 System Response Analysis

**Transfer Function ของระบบความชื้น:**
```
G(s) = K / (τs + 1)

โดย:
K = Static gain (เปอร์เซ็นต์ความชื้นต่อวินาทีการพ่น)
τ = Time constant (เวลาที่ใช้ในการเพิ่มความชื้น 63%)
s = Laplace variable
```

**Time Response Characteristics:**
- **Rise Time:** เวลาที่ใช้เพื่อเข้าใกล้ค่าเป้าหมาย
- **Settling Time:** เวลาที่ใช้เพื่อเข้าสู่ค่าคงที่
- **Overshoot:** การเกินค่าเป้าหมายชั่วคราว
- **Steady-state Error:** ความผิดพลาดในสภาวะคงตัว

### 🧠 Fuzzy Logic Control

**สำหรับการควบคุมที่ซับซ้อน:**

*Fuzzy Sets สำหรับความชื้น:*
```
Linguistic Variables:
├── Very Dry (0-20%)
├── Dry (15-40%)  
├── Optimal (35-65%)
├── Wet (60-85%)
└── Very Wet (80-100%)
```

*Fuzzy Rules:*
```
IF humidity IS Very Dry AND temperature IS Normal 
THEN misting IS Long

IF humidity IS Optimal AND temperature IS High
THEN misting IS Short AND fan IS On
```

### ⏰ Scheduling Algorithm

**Cron-like Scheduling:**
```
Schedule Structure:
minute hour dayOfMonth month dayOfWeek command

Examples:
0 6 * * * START_MISTING  // ทุกวัน 6:00 น.
0 */4 * * * CHECK_HUMIDITY  // ทุก 4 ชั่วโมง
*/30 * * * * READ_SENSORS  // ทุก 30 วินาที
```

**Priority-based Scheduling:**
```
Priority Levels:
1. Emergency (น้ำหมด, อุณหภูมิสูงเกินไป)
2. Critical (ความชื้นต่ำมาก, เซนเซอร์เสีย)
3. Normal (ตารางเวลาปกติ)
4. Low (การบันทึกข้อมูล, การอัพเดท)
```

---

## 🌐 สถาปัตยกรรม IoT {#iot-architecture}

### 🏗️ System Architecture

**3-Tier Architecture:**
```
Presentation Tier (การนำเสนอ):
├── Web Interface (HTML/CSS/JavaScript)
├── Mobile App (Blink Integration)
└── REST API (JSON responses)

Logic Tier (การประมวลผล):
├── ESP32 Microcontroller
├── Control Algorithms (PID, Scheduling)
├── Data Processing (Filtering, Averaging)
└── Communication Protocols (WiFi, HTTP)

Data Tier (ข้อมูล):
├── EEPROM (Settings, Calibration)
├── SPIFFS (Web files, Logs)
└── External Storage (SD Card - optional)
```

### 📡 Communication Protocols

**1. HTTP/HTTPS (Web Interface):**
```
HTTP Request/Response Cycle:

Client Request:
GET /api/status HTTP/1.1
Host: 192.168.1.100
Accept: application/json

Server Response:
HTTP/1.1 200 OK
Content-Type: application/json
{
  "mushroom": {"humidity": 85.5, "temperature": 22.3},
  "cilantro": {"moisture": 68.1, "temperature": 24.1}
}
```

**2. WebSocket (Real-time Updates):**
```
WebSocket Connection:
Client → Server: WebSocket handshake
Server → Client: Upgrade to WebSocket
Client ↔ Server: Real-time data exchange

Data Format:
{
  "type": "sensor_update",
  "timestamp": 1640995200,
  "data": {
    "humidity": 85.5,
    "temperature": 22.3
  }
}
```

**3. MQTT (Optional - for Cloud Integration):**
```
MQTT Topics:
├── mushroom/humidity (publish sensor data)
├── mushroom/control/misting (subscribe to commands)
├── cilantro/moisture (publish sensor data)
├── cilantro/control/watering (subscribe to commands)
└── system/status (publish system health)
```

### 🔒 Security Mechanisms

**1. Authentication:**
```
JWT (JSON Web Token) Structure:
Header.Payload.Signature

Example:
eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.
eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiaWF0IjoxNTE2MjM5MDIyfQ.
SflKxwRJSMeKKF2QT4fwpMeJf36POk6yJV_adQssw5c
```

**2. Encryption (TLS/SSL):**
```
Handshake Process:
1. Client Hello (supported ciphers)
2. Server Hello (selected cipher)
3. Certificate exchange
4. Key exchange (Diffie-Hellman)
5. Encrypted communication
```

**3. Input Validation:**
```cpp
bool validateSensorData(float value, float min, float max) {
  if (isnan(value) || isinf(value)) return false;
  if (value < min || value > max) return false;
  return true;
}

// Example usage:
if (!validateSensorData(humidity, 0, 100)) {
  logError("Invalid humidity reading: " + String(humidity));
  return;
}
```

### 🌊 Data Flow Architecture

**Sensor Data Pipeline:**
```
Raw Sensor → ADC → Digital → Filtering → Calibration → Storage
Reading      Conversion  Value    Algorithm   Correction   Database

Example:
Soil Sensor → 12-bit ADC → 2048 → Moving → 65.3% → EEPROM
(Analog)      (0-4095)     value   Average   Moisture   Storage
```

**Control Command Pipeline:**
```
User Input → Validation → Authorization → Command → Hardware → Feedback
(Web/App)    (Range)      (Permission)     Queue      Control    (Sensor)

Example:  
"Start" → Check user → Verify → Add to → GPIO → Monitor
Misting   permissions   water   queue    HIGH    humidity
          level OK              
```

---

## 💧 ระบบจัดการน้ำ {#water-management}

### 🚰 Hydraulic Principles

**การไหลของของไหล (Fluid Flow):**

*Continuity Equation:*
```
A₁V₁ = A₂V₂ = Q

โดย:
A = Cross-sectional area
V = Velocity
Q = Flow rate (constant)
```

*Bernoulli's Equation:*
```
P₁/ρg + V₁²/2g + h₁ = P₂/ρg + V₂²/2g + h₂

โดย:
P = Pressure
ρ = Density
g = Gravitational acceleration
V = Velocity
h = Height
```

**การประยุกต์ในระบบ:**

*1. Misting System (ระบบพ่นฝอย):*
```
Design Parameters:
├── Pressure: 2-4 bar (เพื่อสร้างละอองฝอย)
├── Nozzle size: 0.2-0.5 mm (ควบคุมขนาดหยดน้ำ)
├── Flow rate: 50-100 ml/min (ป้องกันน้ำท่วม)
└── Droplet size: 10-50 μm (เพื่อการแพร่กระจาย)
```

*2. Drip Irrigation (ระบบรดน้ำหยด):*
```
Design Parameters:
├── Pressure: 0.5-1.5 bar (แรงดันต่ำ)
├── Emitter flow: 2-4 L/hr (ควบคุมการไหล)
├── Spacing: 10-15 cm (ระยะห่างหัวหยด)
└── Wetted area: 20-30 cm diameter (พื้นที่เปียก)
```

### 💧 Pump Selection and Control

**Centrifugal Pump Characteristics:**

*Pump Curve Equation:*
```
H = H₀ - aQ²

โดย:
H = Head (pressure)
H₀ = Shutoff head
a = Pump constant
Q = Flow rate
```

*Power Calculation:*
```
P = ρgQH/η

โดย:
P = Power required
ρ = Water density (1000 kg/m³)
g = 9.81 m/s²
Q = Flow rate (m³/s)
H = Head (m)
η = Pump efficiency (0.6-0.8)
```

**Pump Control Strategies:**

*1. On/Off Control:*
```cpp
void controlPump(bool enable, int pumpPin) {
  if (enable) {
    digitalWrite(pumpPin, HIGH);
    delay(100); // Soft start
  } else {
    digitalWrite(pumpPin, LOW);
  }
}
```

*2. PWM Control (Variable Speed):*
```cpp
void setPumpSpeed(int speed, int pumpPin) {
  // speed: 0-255 (PWM duty cycle)
  analogWrite(pumpPin, speed);
}
```

### 🔄 Water Quality Management

**Chemical Balance:**

*pH Control:*
```
Henderson-Hasselbalch Equation:
pH = pKa + log([A⁻]/[HA])

Optimal ranges:
├── Mushrooms: pH 6.0-7.5
├── Cilantro: pH 6.0-7.0
└── Hydroponic solution: pH 5.5-6.5
```

*Electrical Conductivity (EC):*
```
EC = Σ(Ci × λi)

โดย:
Ci = Concentration of ion i
λi = Molar conductivity of ion i

Optimal EC:
├── Mushrooms: 0.5-1.0 mS/cm
└── Cilantro: 1.2-1.8 mS/cm
```

**Water Treatment:**

*Filtration System:*
```
Water Treatment Chain:
Raw Water → Sediment → Carbon → UV → RO → Storage
            Filter     Filter  Sterilization  Membrane  Tank
```

*Disinfection Methods:*
1. **UV Sterilization:** 254 nm wavelength, 30-40 mJ/cm²
2. **Ozonation:** O₃ concentration 0.1-0.3 ppm
3. **Chlorination:** Free chlorine 0.5-1.0 ppm (ต้องกำจัดก่อนใช้)

### 📊 Flow Monitoring

**Flow Sensor Principles:**

*Turbine Flow Meter:*
```
Frequency ∝ Flow Rate
f = K × Q

โดย:
f = Pulse frequency (Hz)
K = K-factor (pulses/liter)
Q = Flow rate (L/min)
```

*Implementation:*
```cpp
volatile unsigned long pulseCount = 0;
float flowRate = 0.0;
const float K_FACTOR = 7.5; // pulses per liter

void IRAM_ATTR flowPulseCounter() {
  pulseCount++;
}

void calculateFlowRate() {
  flowRate = (pulseCount / K_FACTOR) / (timeInterval / 60.0);
  pulseCount = 0; // Reset counter
}
```

---

## 🌡️ การควบคุมสภาพแวดล้อม {#environmental-control}

### 🌬️ HVAC Principles

**Heat Transfer Mechanisms:**

*1. Conduction:*
```
q = -kA(dT/dx)

โดย:
q = Heat transfer rate
k = Thermal conductivity
A = Area
dT/dx = Temperature gradient
```

*2. Convection:*
```
q = hA(Ts - T∞)

โดย:
h = Heat transfer coefficient
A = Surface area
Ts = Surface temperature
T∞ = Fluid temperature
```

*3. Radiation:*
```
q = εσA(T₁⁴ - T₂⁴)

โดย:
ε = Emissivity
σ = Stefan-Boltzmann constant (5.67×10⁻⁸ W/m²K⁴)
A = Area
T = Absolute temperature
```

### 💨 Air Circulation Design

**Fan Selection and Sizing:**

*Airflow Requirements:*
```
Growing Chamber Volume = L × W × H
Air Changes per Hour (ACH) = 4-6 for mushrooms
Required CFM = (Volume × ACH) / 60

Example:
Chamber: 1m × 0.6m × 0.4m = 0.24 m³
ACH = 5
Required airflow = (0.24 × 5 × 60) / 60 = 1.2 m³/min
```

*Fan Curve Analysis:*
```
Static Pressure vs Airflow:
P = P₀ - aQ²

โดย:
P = Static pressure
P₀ = Maximum pressure (zero flow)
a = Fan constant
Q = Airflow rate
```

**Ventilation Strategies:**

*1. Exhaust Ventilation:*
- Remove excess humidity and CO₂
- Create negative pressure
- Prevent contamination

*2. Supply Ventilation:*
- Provide fresh air and oxygen
- Create positive pressure
- Filter incoming air

*3. Balanced Ventilation:*
- Equal supply and exhaust
- Maintain pressure balance
- Energy efficient

### 🌡️ Temperature Control Systems

**Heating System Design:**

*Electric Heater Sizing:*
```
Heat Load Calculation:
Q = UA(Ti - To) + Ventilation Load + Equipment Load

โดย:
Q = Total heat load (W)
U = Overall heat transfer coefficient
A = Surface area
Ti = Indoor temperature
To = Outdoor temperature
```

*PTC Heater Characteristics:*
```
PTC (Positive Temperature Coefficient) Heater:
R = R₀ × exp(α(T - T₀))

โดย:
R = Resistance at temperature T
R₀ = Reference resistance
α = Temperature coefficient
T = Operating temperature
T₀ = Reference temperature
```

**Cooling System Design:**

*Evaporative Cooling:*
```
Cooling Effect = ṁ × hfg

โดย:
ṁ = Mass flow rate of evaporated water
hfg = Latent heat of vaporization (2257 kJ/kg at 100°C)
```

*Efficiency Calculation:*
```
Efficiency = (Tdb1 - Tdb2) / (Tdb1 - Twb1) × 100%

โดย:
Tdb1 = Dry bulb temperature (inlet)
Tdb2 = Dry bulb temperature (outlet)  
Twb1 = Wet bulb temperature (inlet)
```

### 💧 Humidity Control

**Psychrometric Relationships:**

*Relative Humidity:*
```
RH = (Pv / Psat) × 100%

โดย:
Pv = Partial pressure of water vapor
Psat = Saturation pressure at given temperature
```

*Absolute Humidity:*
```
AH = 0.622 × Pv / (P - Pv)

โดย:
P = Total atmospheric pressure
```

*Dew Point Calculation:*
```
Tdp = (b × γ) / (a - γ)

โดย:
γ = ln(RH/100) + aT/(b+T)
a = 17.27, b = 237.7 (constants)
T = Temperature (°C)
```

**Humidification Methods:**

*1. Steam Humidification:*
- ประสิทธิภาพสูง (99%)
- ไม่มีแบคทีเรีย
- ใช้พลังงานสูง

*2. Ultrasonic Humidification:*
- ประหยัดพลังงาน
- ละอองฝอยขนาดเล็ก
- ต้องการน้ำสะอาด

*3. Evaporative Humidification:*
- ประหยัดพลังงาน
- ช่วยระบายความร้อน
- ความชื้นจำกัด

### 📊 Environmental Monitoring

**Data Acquisition System:**

*Sensor Fusion Algorithm:*
```cpp
struct SensorReading {
  float value;
  float confidence;
  unsigned long timestamp;
};

float fuseSensorData(SensorReading sensors[], int count) {
  float weightedSum = 0;
  float totalWeight = 0;
  
  for (int i = 0; i < count; i++) {
    if (isValidReading(sensors[i])) {
      weightedSum += sensors[i].value * sensors[i].confidence;
      totalWeight += sensors[i].confidence;
    }
  }
  
  return (totalWeight > 0) ? weightedSum / totalWeight : NAN;
}
```

*Kalman Filter Implementation:*
```cpp
class KalmanFilter {
private:
  float Q; // Process noise covariance
  float R; // Measurement noise covariance
  float P; // Estimation error covariance
  float K; // Kalman gain
  float X; // State estimate
  
public:
  float update(float measurement) {
    // Prediction step
    P = P + Q;
    
    // Update step
    K = P / (P + R);
    X = X + K * (measurement - X);
    P = (1 - K) * P;
    
    return X;
  }
};
```

---

## 💡 ระบบแสงสว่าง {#lighting-systems}

### 🌈 Light Physics and Plant Response

**Electromagnetic Spectrum:**
```
Light Spectrum for Plant Growth:
├── UV-B (280-315 nm): Stress response, compact growth
├── UV-A (315-400 nm): Flavonoid production
├── Blue (400-500 nm): Chlorophyll synthesis, stomatal opening
├── Green (500-600 nm): Penetration to lower leaves
├── Red (600-700 nm): Photosynthesis, flowering
├── Far-Red (700-800 nm): Stem elongation, shade response
└── IR (>800 nm): Heat generation
```

**Photosynthetic Photon Flux Density (PPFD):**
```
PPFD = ∫[400-700nm] φ(λ) dλ

โดย:
φ(λ) = Photon flux density at wavelength λ
Units: μmol/m²/s (micromoles per square meter per second)
```

### 🔬 LED Technology

**LED Working Principle:**
```
Energy Band Gap Theory:
E = hc/λ = hf

โดย:
E = Photon energy (eV)
h = Planck's constant (6.626×10⁻³⁴ J·s)
c = Speed of light (3×10⁸ m/s)
λ = Wavelength (m)
f = Frequency (Hz)
```

**LED Efficiency Metrics:**

*1. Luminous Efficacy:*
```
Efficacy = Luminous Flux / Electrical Power
Units: lm/W (lumens per watt)

High-efficiency LEDs: 150-200 lm/W
```

*2. Photosynthetic Photon Efficacy (PPE):*
```
PPE = PPFD / Electrical Power
Units: μmol/J (micromoles per joule)

Good LEDs: 2.0-2.7 μmol/J
```

### 🌱 Plant-Specific Lighting Design

**Mushroom Lighting Requirements:**

*Photomorphogenesis in Fungi:*
- เห็ดไม่มีคลอโรฟิลล์ → ไม่ทำสังเคราะห์แสง
- แสงกระตุ้นการก่อตัวของหัวเห็ด (Photoinduction)
- Blue light (400-500 nm) มีประสิทธิภาพสูงสุด

*Optimal Parameters:*
```
Light Requirements for Oyster Mushrooms:
├── Intensity: 500-1000 Lux (5-10 μmol/m²/s)
├── Duration: 12 hours light / 12 hours dark
├── Spectrum: Blue-dominant (450 nm peak)
└── Photoperiod: Consistent daily cycle
```

**Cilantro Lighting Requirements:**

*Photosynthesis Optimization:*
```
Light Saturation Curve:
Pn = Pmax × (I / (I + Km))

โดย:
Pn = Net photosynthesis rate
Pmax = Maximum photosynthesis rate
I = Light intensity (PPFD)
Km = Half-saturation constant
```

*Optimal Parameters:*
```
Light Requirements for Cilantro:
├── PPFD: 200-400 μmol/m²/s
├── DLI: 12-18 mol/m²/d (Daily Light Integral)
├── Spectrum: Full spectrum with red/blue ratio 1:1
├── Duration: 14-16 hours (long-day plant)
└── Photoperiod: Consistent to prevent bolting
```

### 💡 LED Driver and Control

**Constant Current LED Driver:**

*Basic Circuit:*
```
LED Driver Circuit:
Vin → Buck Converter → Current Sense → LED String
      ↑                    ↓
      PWM Controller ← Feedback
```

*Current Regulation:*
```
ILED = Vref / Rsense

โดย:
ILED = LED current
Vref = Reference voltage (typically 1.25V)
Rsense = Current sensing resistor
```

**PWM Dimming Control:**

*Duty Cycle Calculation:*
```
Duty Cycle = Ton / (Ton + Toff) × 100%
Average Current = Peak Current × Duty Cycle
```

*Implementation:*
```cpp
void setLightIntensity(int channel, int intensity) {
  // intensity: 0-100 (percentage)
  int pwmValue = map(intensity, 0, 100, 0, 255);
  analogWrite(channel, pwmValue);
}

void setPhotoperiod(int onHour, int offHour) {
  int currentHour = getCurrentHour();
  
  if (currentHour >= onHour && currentHour < offHour) {
    setLightIntensity(MUSHROOM_LIGHT_PIN, 30); // 30% for mushrooms
    setLightIntensity(CILANTRO_LIGHT_PIN, 80); // 80% for cilantro
  } else {
    setLightIntensity(MUSHROOM_LIGHT_PIN, 0);
    setLightIntensity(CILANTRO_LIGHT_PIN, 0);
  }
}
```

### 🌡️ Thermal Management

**LED Heat Generation:**

*Thermal Resistance Model:*
```
Tj = Ta + (Rjc + Rcs + Rsa) × P

โดย:
Tj = Junction temperature
Ta = Ambient temperature
Rjc = Junction-to-case thermal resistance
Rcs = Case-to-sink thermal resistance  
Rsa = Sink-to-ambient thermal resistance
P = Power dissipated as heat
```

*Heat Sink Design:*
```
Required Thermal Resistance:
Rsa = (Tj,max - Ta,max) / P - Rjc - Rcs

Typical values:
├── Rjc = 2-5 °C/W (LED package)
├── Rcs = 0.1-0.5 °C/W (thermal interface)
└── Tj,max = 85-125 °C (LED limit)
```

### 📊 Light Measurement and Control

**Light Sensor Calibration:**

*LDR Response Curve:*
```
RLDR = A × (Lux)^(-γ)

Calibration points:
├── Dark: 10MΩ (0 Lux)
├── Room light: 10kΩ (100 Lux)  
├── Bright light: 1kΩ (1000 Lux)
└── Direct sun: 100Ω (10000 Lux)
```

*Conversion to PPFD:*
```
PPFD ≈ Lux × 0.015 (approximate for white LEDs)

More accurate conversion requires spectral data:
PPFD = ∫[400-700nm] (Lux × S(λ) × V(λ)⁻¹) dλ
```

**Automated Light Control:**

*Daily Light Integral (DLI) Control:*
```cpp
class DLIController {
private:
  float targetDLI;
  float currentDLI;
  int lightIntensity;
  
public:
  void updateDLI(float ppfd, float hours) {
    currentDLI += (ppfd * hours * 3600) / 1000000; // mol/m²/d
  }
  
  void adjustLighting() {
    float dliError = targetDLI - currentDLI;
    float hoursRemaining = getHoursUntilDark();
    
    if (hoursRemaining > 0) {
      float requiredPPFD = (dliError * 1000000) / (hoursRemaining * 3600);
      lightIntensity = map(requiredPPFD, 0, 400, 0, 100);
      setLightIntensity(CILANTRO_LIGHT_PIN, lightIntensity);
    }
  }
};
```

---

## 📊 การประมวลผลข้อมูล {#data-processing}

### 🔢 Signal Processing

**Digital Signal Processing Fundamentals:**

*Sampling Theory (Nyquist Theorem):*
```
fs ≥ 2 × fmax

โดย:
fs = Sampling frequency
fmax = Maximum frequency component in signal
```

*For sensor data:*
- Temperature: fmax ≈ 0.01 Hz → fs ≥ 0.02 Hz (sample every 50s)
- Humidity: fmax ≈ 0.1 Hz → fs ≥ 0.2 Hz (sample every 5s)  
- Soil moisture: fmax ≈ 0.001 Hz → fs ≥ 0.002 Hz (sample every 8 minutes)

**Filtering Algorithms:**

*1. Moving Average Filter:*
```cpp
class MovingAverageFilter {
private:
  float buffer[WINDOW_SIZE];
  int index;
  float sum;
  
public:
  float filter(float input) {
    sum -= buffer[index];
    buffer[index] = input;
    sum += input;
    index = (index + 1) % WINDOW_SIZE;
    return sum / WINDOW_SIZE;
  }
};
```

*2. Exponential Smoothing:*
```cpp
float exponentialSmooth(float current, float previous, float alpha) {
  return alpha * current + (1 - alpha) * previous;
}

// Usage: alpha = 0.1 for slow response, 0.9 for fast response
```

*3. Median Filter (Noise Removal):*
```cpp
float medianFilter(float data[], int size) {
  // Sort array
  for (int i = 0; i < size - 1; i++) {
    for (int j = i + 1; j < size; j++) {
      if (data[i] > data[j]) {
        float temp = data[i];
        data[i] = data[j];
        data[j] = temp;
      }
    }
  }
  return data[size / 2]; // Return middle value
}
```

### 📈 Statistical Analysis

**Descriptive Statistics:**

*Mean and Variance:*
```cpp
struct Statistics {
  float mean;
  float variance;
  float stddev;
  int count;
};

Statistics calculateStats(float data[], int size) {
  Statistics stats = {0};
  
  // Calculate mean
  for (int i = 0; i < size; i++) {
    stats.mean += data[i];
  }
  stats.mean /= size;
  
  // Calculate variance
  for (int i = 0; i < size; i++) {
    float diff = data[i] - stats.mean;
    stats.variance += diff * diff;
  }
  stats.variance /= size;
  stats.stddev = sqrt(stats.variance);
  stats.count = size;
  
  return stats;
}
```

**Outlier Detection:**

*Z-Score Method:*
```cpp
bool isOutlier(float value, float mean, float stddev, float threshold = 3.0) {
  float zscore = abs(value - mean) / stddev;
  return zscore > threshold;
}
```

*Interquartile Range (IQR) Method:*
```cpp
bool isOutlierIQR(float value, float q1, float q3) {
  float iqr = q3 - q1;
  float lowerBound = q1 - 1.5 * iqr;
  float upperBound = q3 + 1.5 * iqr;
  return (value < lowerBound || value > upperBound);
}
```

### 🧠 Machine Learning Concepts

**Linear Regression for Trend Analysis:**

*Simple Linear Regression:*
```
y = mx + b

Where:
m = Σ((xi - x̄)(yi - ȳ)) / Σ((xi - x̄)²)
b = ȳ - m × x̄
```

*Implementation:*
```cpp
struct RegressionResult {
  float slope;
  float intercept;
  float correlation;
};

RegressionResult linearRegression(float x[], float y[], int size) {
  float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
  
  for (int i = 0; i < size; i++) {
    sumX += x[i];
    sumY += y[i];
    sumXY += x[i] * y[i];
    sumX2 += x[i] * x[i];
  }
  
  float meanX = sumX / size;
  float meanY = sumY / size;
  
  RegressionResult result;
  result.slope = (sumXY - size * meanX * meanY) / (sumX2 - size * meanX * meanX);
  result.intercept = meanY - result.slope * meanX;
  
  // Calculate correlation coefficient
  float sumY2 = 0;
  for (int i = 0; i < size; i++) {
    sumY2 += y[i] * y[i];
  }
  
  float numerator = sumXY - size * meanX * meanY;
  float denominator = sqrt((sumX2 - size * meanX * meanX) * (sumY2 - size * meanY * meanY));
  result.correlation = numerator / denominator;
  
  return result;
}
```

**Anomaly Detection:**

*Control Charts (Statistical Process Control):*
```cpp
class ControlChart {
private:
  float centerLine;
  float upperControlLimit;
  float lowerControlLimit;
  
public:
  void calculateLimits(float data[], int size) {
    Statistics stats = calculateStats(data, size);
    centerLine = stats.mean;
    upperControlLimit = stats.mean + 3 * stats.stddev;
    lowerControlLimit = stats.mean - 3 * stats.stddev;
  }
  
  bool isInControl(float value) {
    return (value >= lowerControlLimit && value <= upperControlLimit);
  }
};
```

### 📊 Data Visualization

**Real-time Charting:**

*Time Series Data Structure:*
```cpp
struct DataPoint {
  unsigned long timestamp;
  float value;
  String label;
};

class TimeSeriesBuffer {
private:
  DataPoint buffer[MAX_POINTS];
  int head, tail, count;
  
public:
  void addPoint(float value, String label = "") {
    buffer[head] = {millis(), value, label};
    head = (head + 1) % MAX_POINTS;
    if (count < MAX_POINTS) count++;
    else tail = (tail + 1) % MAX_POINTS;
  }
  
  String toJSON() {
    String json = "[";
    for (int i = 0; i < count; i++) {
      int index = (tail + i) % MAX_POINTS;
      json += "{\"timestamp\":" + String(buffer[index].timestamp);
      json += ",\"value\":" + String(buffer[index].value) + "}";
      if (i < count - 1) json += ",";
    }
    json += "]";
    return json;
  }
};
```

**Chart.js Integration:**
```javascript
function updateChart(sensorData) {
  const ctx = document.getElementById('sensorChart').getContext('2d');
  const chart = new Chart(ctx, {
    type: 'line',
    data: {
      datasets: [{
        label: 'Humidity',
        data: sensorData.humidity,
        borderColor: 'blue',
        fill: false
      }, {
        label: 'Temperature', 
        data: sensorData.temperature,
        borderColor: 'red',
        fill: false
      }]
    },
    options: {
      responsive: true,
      scales: {
        x: {
          type: 'time',
          time: {
            displayFormats: {
              minute: 'HH:mm'
            }
          }
        },
        y: {
          beginAtZero: true
        }
      }
    }
  });
}
```

### 🎯 Predictive Analytics

**Simple Prediction Models:**

*Linear Trend Extrapolation:*
```cpp
float predictNextValue(float data[], int size, int stepsAhead) {
  RegressionResult regression = linearRegression(
    generateTimeArray(size), data, size
  );
  
  float nextTime = size + stepsAhead;
  return regression.slope * nextTime + regression.intercept;
}
```

*Exponential Smoothing Prediction:*
```cpp
float exponentialSmoothingPredict(float data[], int size, float alpha) {
  float smoothed = data[0];
  
  for (int i = 1; i < size; i++) {
    smoothed = alpha * data[i] + (1 - alpha) * smoothed;
  }
  
  return smoothed; // Next predicted value
}
```

---

## 🔋 การจัดการพลังงาน {#power-management}

### ⚡ Power System Design

**Power Budget Analysis:**

*Total System Power Calculation:*
```
Component Power Analysis:
├── ESP32 (Active): 160-260 mA @ 3.3V = 0.5-0.9W
├── ESP32 (Sleep): 10-150 μA @ 3.3V = 0.03-0.5mW
├── DHT22 (2x): 2.5 mA @ 5V = 0.025W
├── Water Pumps (2x): 2A @ 12V = 48W (when active)
├── LED Lights (2x): 1A @ 12V = 24W (when active)
├── Fans (2x): 0.5A @ 12V = 12W (when active)
├── Heater: 3A @ 12V = 36W (when active)
├── Relays (9x): 70mA @ 5V = 3.15W
└── Sensors (misc): ~0.1W

Peak Power: ~125W
Average Power: ~15-25W (depending on duty cycles)
```

**Power Supply Selection:**

*Linear vs Switching Regulators:*

Linear Regulator:
```
Efficiency = Vout / Vin
Power Loss = (Vin - Vout) × Iout
Heat Generated = Power Loss

Example: 12V → 5V @ 2A
Efficiency = 5/12 = 41.7%
Power Loss = (12-5) × 2 = 14W (as heat!)
```

Switching Regulator (Buck Converter):
```
Efficiency = 85-95% (typical)
Power Loss = Pin × (1 - Efficiency)

Example: 12V → 5V @ 2A
Pin = 5V × 2A / 0.9 = 11.1W
Power Loss = 11.1 × 0.1 = 1.1W
```

### 🔋 Battery Backup System

**Battery Capacity Calculation:**

*Lead-Acid Battery:*
```
Required Capacity = (Load Power × Backup Time) / (Battery Voltage × Efficiency)

Example:
Load: 15W average
Backup time: 8 hours
Battery: 12V
Efficiency: 80%

Capacity = (15W × 8h) / (12V × 0.8) = 12.5 Ah
Select: 20Ah battery (with safety margin)
```

*Lithium-ion Battery:*
```
Advantages:
├── Higher energy density (150-250 Wh/kg vs 30-50 Wh/kg)
├── Lower self-discharge (<5% vs 15% per month)
├── Longer cycle life (2000+ vs 500 cycles)
└── Better efficiency (95% vs 85%)

Disadvantages:
├── Higher cost (3-5x)
├── Requires BMS (Battery Management System)
├── Temperature sensitive
└── Safety concerns (thermal runaway)
```

### 🌞 Solar Power Integration

**Solar Panel Sizing:**

*Daily Energy Calculation:*
```
Daily Energy Consumption = Average Power × 24 hours
Daily Energy = 20W × 24h = 480 Wh

Solar Panel Size = Daily Energy / (Peak Sun Hours × System Efficiency)
Solar Panel = 480Wh / (5h × 0.8) = 120W

Select: 150W panel (with weather margin)
```

*MPPT Controller:*
```
Maximum Power Point Tracking:
P = V × I
dP/dV = 0 (at maximum power point)

MPPT Algorithm:
1. Measure Vpv, Ipv
2. Calculate Ppv = Vpv × Ipv  
3. Compare with previous power
4. Adjust duty cycle to increase power
5. Repeat
```

### 🔌 Power Management Unit (PMU)

**Intelligent Power Control:**

*Load Prioritization:*
```cpp
enum PowerPriority {
  CRITICAL = 1,    // Sensors, ESP32
  HIGH = 2,        // Pumps, critical systems
  MEDIUM = 3,      // Lights, fans
  LOW = 4          // Non-essential features
};

class PowerManager {
private:
  float batteryVoltage;
  float batteryCapacity;
  bool acPowerAvailable;
  
public:
  void managePower() {
    updateBatteryStatus();
    
    if (batteryVoltage < LOW_BATTERY_THRESHOLD) {
      disableNonCriticalLoads();
    } else if (batteryVoltage < MEDIUM_BATTERY_THRESHOLD) {
      reducePowerConsumption();
    }
    
    if (!acPowerAvailable && batteryCapacity < EMERGENCY_THRESHOLD) {
      enterEmergencyMode();
    }
  }
  
private:
  void disableNonCriticalLoads() {
    digitalWrite(LED_LIGHTS_RELAY, LOW);
    digitalWrite(HEATER_RELAY, LOW);
    // Keep only pumps and sensors active
  }
  
  void enterEmergencyMode() {
    // Minimal operation: sensors only
    // Send alert notifications
    // Prepare for safe shutdown
  }
};
```

### 💤 Sleep Mode Implementation

**ESP32 Sleep Modes:**

*Deep Sleep Mode:*
```cpp
void enterDeepSleep(uint64_t sleepTimeUS) {
  // Configure wake-up sources
  esp_sleep_enable_timer_wakeup(sleepTimeUS);
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH);
  
  // Save critical data to RTC memory
  RTC_DATA_ATTR int bootCount = 0;
  RTC_DATA_ATTR float lastSensorValue = 0;
  
  // Enter deep sleep
  esp_deep_sleep_start();
}

void setup() {
  // Check wake-up reason
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  
  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Wakeup by timer");
      break;
    case ESP_SLEEP_WAKEUP_EXT1:
      Serial.println("Wakeup by external signal");
      break;
    default:
      Serial.println("Cold boot");
      break;
  }
}
```

*Light Sleep Mode:*
```cpp
void enterLightSleep(uint64_t sleepTimeUS) {
  // Light sleep maintains WiFi connection
  esp_sleep_enable_timer_wakeup(sleepTimeUS);
  esp_light_sleep_start();
  
  // Execution continues here after wake-up
  Serial.println("Woke up from light sleep");
}
```

### 📊 Power Monitoring

**Current and Voltage Measurement:**

*INA219 Current Sensor:*
```cpp
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

void measurePower() {
  float shuntvoltage = ina219.getShuntVoltage_mV();
  float busvoltage = ina219.getBusVoltage_V();
  float current_mA = ina219.getCurrent_mA();
  float power_mW = ina219.getPower_mW();
  float loadvoltage = busvoltage + (shuntvoltage / 1000);
  
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
}
```

*Battery State of Charge (SoC) Estimation:*
```cpp
class BatteryMonitor {
private:
  float nominalVoltage;
  float capacity_Ah;
  float coulombCount;
  
public:
  float estimateSoC() {
    // Voltage-based estimation (simple)
    float voltage = readBatteryVoltage();
    float socVoltage = mapVoltageToSoC(voltage);
    
    // Coulomb counting (more accurate)
    float current = readCurrent();
    coulombCount += current * (millis() - lastUpdate) / 3600000.0; // Ah
    float socCoulomb = (capacity_Ah - coulombCount) / capacity_Ah * 100;
    
    // Weighted average
    return 0.3 * socVoltage + 0.7 * socCoulomb;
  }
  
private:
  float mapVoltageToSoC(float voltage) {
    // Lead-acid discharge curve
    if (voltage > 12.6) return 100;
    if (voltage > 12.4) return 75;
    if (voltage > 12.2) return 50;
    if (voltage > 12.0) return 25;
    return 0;
  }
};
```

---

## 🎓 สรุปและการประยุกต์ใช้

### 🔬 หลักการทางวิทยาศาสตร์ที่ใช้

1. **ชีววิทยา:** เข้าใจวงจรชีวิตและความต้องการของพืชและเชื้อรา
2. **เคมี:** การควบคุม pH, EC, และสารอาหาร
3. **ฟิสิกส์:** การถ่ายเทความร้อน, การไหลของของไหล, แสง
4. **วิศวกรรม:** การควบคุมอัตโนมัติ, ระบบสื่อสาร, การจัดการพลังงาน
5. **คอมพิวเตอร์:** การประมวลผลข้อมูล, อัลกอริทึม, IoT

### 🎯 ข้อดีของระบบอัตโนมัติ

**1. ความแม่นยำ:**
- ควบคุมสภาพแวดล้อมได้แม่นยำ
- ลดความผิดพลาดจากมนุษย์
- การตอบสนองที่รวดเร็ว

**2. ประสิทธิภาพ:**
- ใช้ทรัพยากรอย่างเหมาะสม
- ลดการสูญเสียน้ำและพลังงาน
- เพิ่มผลผลิตและคุณภาพ

**3. สะดวกสบาย:**
- ทำงานตลอด 24 ชั่วโมง
- ควบคุมจากระยะไกล
- แจ้งเตือนเมื่อมีปัญหา

### 🔮 แนวโน้มอนาคต

**1. AI และ Machine Learning:**
- การเรียนรู้รูปแบบการเจริญเติบโต
- การปรับปรุงการควบคุมอัตโนมัติ
- การทำนายผลผลิต

**2. Precision Agriculture:**
- เซนเซอร์ที่แม่นยำยิ่งขึ้น
- การควบคุมระดับจุลภาค
- การปรับแต่งแต่ละต้น

**3. Sustainability:**
- ใช้พลังงานทดแทน
- ลดการใช้เคมีภัณฑ์
- เศรษฐกิจหมุนเวียน

---

*📚 เอกสารนี้รวบรวมความรู้ทางวิทยาศาสตร์และเทคโนโลยีที่ใช้ในระบบรดน้ำอัตโนมัติ เพื่อให้ผู้ใช้เข้าใจหลักการทำงานและสามารถปรับปรุงระบบได้อย่างมีประสิทธิภาพ*
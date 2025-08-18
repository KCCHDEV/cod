# 🔌 Wiring Diagram - Mushroom & Cilantro System

## 📋 Complete Wiring Guide

### 🎛️ ESP32 Pin Assignments

```
ESP32 Development Board
├── Power Pins
│   ├── VIN  → 5V DC Power Input
│   ├── GND  → Ground (Multiple connections)
│   └── 3V3  → 3.3V Output (for sensors)
│
├── Digital Output Pins (Relays)
│   ├── GPIO 18 → Relay 1 (Mushroom Misting Pump)
│   ├── GPIO 19 → Relay 2 (Cilantro Water Pump)
│   ├── GPIO 20 → Relay 3 (Mushroom Valve)
│   ├── GPIO 21 → Relay 4 (Cilantro Valve)
│   ├── GPIO 22 → Relay 5 (Humidity/Exhaust Fan)
│   ├── GPIO 23 → Relay 6 (Circulation Fan)
│   ├── GPIO 25 → Relay 7 (Mushroom LED Lights)
│   ├── GPIO 26 → Relay 8 (Cilantro LED Lights)
│   └── GPIO 27 → Relay 9 (Heater Element)
│
├── Sensor Input Pins
│   ├── GPIO 4  → DHT22 #1 (Mushroom Area)
│   ├── GPIO 5  → DHT22 #2 (Cilantro Area)
│   ├── GPIO 2  → DS18B20 (Temperature Probe)
│   ├── A0/GPIO36 → Soil Moisture Sensor
│   ├── A1/GPIO39 → Water Level Sensor (Mushroom Tank)
│   ├── A2/GPIO34 → Water Level Sensor (Cilantro Tank)
│   └── A3/GPIO35 → Light Sensor (LDR)
│
├── Control Pins
│   ├── GPIO 32 → Manual Control Button (INPUT_PULLUP)
│   ├── GPIO 33 → System Reset Button (INPUT_PULLUP)
│   ├── GPIO 14 → Buzzer/Alert Speaker
│   └── GPIO 13 → Status LED
│
└── Communication Pins
    ├── GPIO 1  → TX (Serial/Debug)
    └── GPIO 3  → RX (Serial/Debug)
```

---

## 🔧 Component Wiring Details

### 1. 🏠 Power Distribution

```
12V DC Power Supply (5A minimum)
├── 12V Rail
│   ├── → Water Pumps (2x)
│   ├── → LED Grow Lights (2x)
│   ├── → Fans (2x)
│   └── → Heater Element
│
├── 5V Rail (via Buck Converter)
│   ├── → ESP32 VIN
│   ├── → Relay Module VCC
│   └── → DHT22 Sensors VCC
│
└── 3.3V Rail (from ESP32)
    ├── → DS18B20 VDD
    ├── → Analog Sensors VCC
    └── → Pull-up Resistors
```

### 2. 🔄 Relay Module Connections

```
8-Channel Relay Module
├── VCC → 5V
├── GND → Ground
├── IN1 → GPIO 18 (Mushroom Misting Pump)
├── IN2 → GPIO 19 (Cilantro Water Pump)
├── IN3 → GPIO 20 (Mushroom Valve)
├── IN4 → GPIO 21 (Cilantro Valve)
├── IN5 → GPIO 22 (Humidity Fan)
├── IN6 → GPIO 23 (Circulation Fan)
├── IN7 → GPIO 25 (Mushroom Lights)
└── IN8 → GPIO 26 (Cilantro Lights)

Additional Relay for Heater:
├── VCC → 5V
├── GND → Ground
└── IN  → GPIO 27 (Heater)
```

### 3. 🌡️ DHT22 Temperature & Humidity Sensors

```
DHT22 #1 (Mushroom Area):
├── VCC → 5V
├── GND → Ground
├── DATA → GPIO 4
└── Pull-up → 10kΩ resistor between VCC and DATA

DHT22 #2 (Cilantro Area):
├── VCC → 5V
├── GND → Ground
├── DATA → GPIO 5
└── Pull-up → 10kΩ resistor between VCC and DATA
```

### 4. 🌡️ DS18B20 Temperature Probe

```
DS18B20 Waterproof Probe:
├── Red (VDD) → 3.3V
├── Black (GND) → Ground
├── Yellow (DATA) → GPIO 2
└── Pull-up → 4.7kΩ resistor between VDD and DATA
```

### 5. 💧 Water Level Sensors

```
Water Level Sensor #1 (Mushroom Tank):
├── VCC → 3.3V
├── GND → Ground
└── AOUT → A1 (GPIO 39)

Water Level Sensor #2 (Cilantro Tank):
├── VCC → 3.3V
├── GND → Ground
└── AOUT → A2 (GPIO 34)
```

### 6. 🌱 Soil Moisture Sensor

```
Capacitive Soil Moisture Sensor:
├── VCC → 3.3V
├── GND → Ground
└── AOUT → A0 (GPIO 36)
```

### 7. ☀️ Light Sensor

```
LDR (Light Dependent Resistor):
├── One leg → 3.3V
├── Other leg → A3 (GPIO 35) + 10kΩ to Ground
└── Forms voltage divider circuit
```

### 8. 🎮 Control Buttons

```
Manual Control Button:
├── One terminal → GPIO 32
├── Other terminal → Ground
└── Internal pull-up enabled in code

Reset Button:
├── One terminal → GPIO 33
├── Other terminal → Ground
└── Internal pull-up enabled in code
```

### 9. 🔊 Alert System

```
Buzzer:
├── Positive → GPIO 14
├── Negative → Ground
└── Optional: Add 100Ω resistor in series

Status LED:
├── Anode → GPIO 13
├── Cathode → 220Ω resistor → Ground
└── Use different colors for different states
```

---

## 🔌 Physical Layout Recommendations

### 📦 Enclosure Setup

```
Weatherproof Enclosure (IP65):
├── Main Compartment
│   ├── ESP32 Development Board
│   ├── Relay Modules
│   ├── Buck Converter (12V → 5V)
│   ├── Terminal Blocks
│   └── Cooling Fan (optional)
│
├── Sensor Compartment
│   ├── DHT22 Sensors (with ventilation)
│   ├── Light Sensor (clear window)
│   └── Status LED (visible outside)
│
└── External Connections
    ├── Power Input (12V DC)
    ├── Water Pump Outputs
    ├── Sensor Cables
    └── Network Cable (optional)
```

### 🌿 Field Installation

```
Mushroom Growing Area:
├── DHT22 Sensor #1 → Inside growing chamber
├── Misting Nozzles → Distributed throughout
├── LED Grow Lights → Above mushroom beds
├── Circulation Fan → For air movement
└── Water Tank #1 → Elevated for gravity feed

Cilantro Growing Area:
├── DHT22 Sensor #2 → Above plant canopy
├── Soil Moisture Sensor → In growing medium
├── Drip Irrigation Lines → At plant bases
├── LED Grow Lights → Above plants
└── Water Tank #2 → Separate reservoir
```

---

## ⚡ Power Calculations

### 🔋 Power Requirements

| Component | Voltage | Current | Power | Quantity | Total Power |
|-----------|---------|---------|-------|----------|-------------|
| ESP32 | 5V | 0.5A | 2.5W | 1 | 2.5W |
| DHT22 | 5V | 0.002A | 0.01W | 2 | 0.02W |
| DS18B20 | 3.3V | 0.001A | 0.003W | 1 | 0.003W |
| Water Pumps | 12V | 2A | 24W | 2 | 48W |
| LED Lights | 12V | 1A | 12W | 2 | 24W |
| Fans | 12V | 0.5A | 6W | 2 | 12W |
| Heater | 12V | 3A | 36W | 1 | 36W |
| Relays | 5V | 0.07A | 0.35W | 9 | 3.15W |
| **TOTAL** | | | | | **125.7W** |

**Recommended Power Supply:** 12V 15A (180W) with safety margin

---

## 🛡️ Safety Considerations

### ⚠️ Electrical Safety

1. **Ground Fault Protection**
   - Install GFCI breaker for water pump circuits
   - Ensure all metal components are grounded

2. **Water Protection**
   - Use IP65+ rated enclosures
   - Keep electrical components away from water
   - Use waterproof connectors

3. **Overcurrent Protection**
   - Install appropriate fuses for each circuit
   - Use thermal protection for motors

### 🔥 Fire Safety

1. **Heat Management**
   - Install cooling fans in enclosure
   - Monitor heater element temperature
   - Use thermal cutoff switches

2. **Component Quality**
   - Use certified components only
   - Check wire gauge ratings
   - Inspect connections regularly

---

## 🔧 Assembly Steps

### Step 1: Prepare Enclosure
1. Mount ESP32 on DIN rail or standoffs
2. Install relay modules with proper spacing
3. Mount buck converter for 5V supply
4. Install terminal blocks for connections

### Step 2: Power Wiring
1. Connect 12V input to main terminal
2. Wire buck converter for 5V rail
3. Distribute power to components
4. Install fuses and protection

### Step 3: Control Wiring
1. Connect ESP32 to relay modules
2. Wire sensor inputs with proper pull-ups
3. Install control buttons and indicators
4. Test all connections with multimeter

### Step 4: Sensor Installation
1. Mount DHT22 sensors in growing areas
2. Install soil moisture sensor in medium
3. Place water level sensors in tanks
4. Position light sensor for ambient reading

### Step 5: System Testing
1. Upload firmware to ESP32
2. Test each relay operation
3. Verify sensor readings
4. Check web interface connectivity
5. Test emergency stops and alarms

---

## 🔍 Troubleshooting Guide

### Common Issues:

**No Sensor Readings:**
- Check 3.3V power supply
- Verify pull-up resistors
- Test sensor with multimeter

**Relays Not Switching:**
- Check 5V supply to relay module
- Verify GPIO pin assignments
- Test relay coil resistance

**WiFi Connection Issues:**
- Check antenna connection
- Verify power supply stability
- Test in hotspot mode first

**Pumps Not Working:**
- Check 12V supply voltage
- Verify relay contacts
- Test pump directly with 12V

---

*⚡ Always disconnect power before making wiring changes ⚡*
# 🚀 Quick Setup Guide - Mushroom & Cilantro System

## 📦 What You'll Need

### 🛒 Shopping List

**Electronic Components:**
- [ ] ESP32 Development Board
- [ ] 8-Channel Relay Module  
- [ ] 2x DHT22 Temperature/Humidity Sensors
- [ ] 1x DS18B20 Waterproof Temperature Probe
- [ ] 1x Capacitive Soil Moisture Sensor
- [ ] 2x Water Level Sensors
- [ ] 1x Light Sensor (LDR)
- [ ] 12V 15A Power Supply
- [ ] Buck Converter (12V to 5V)
- [ ] Waterproof Enclosure (IP65)
- [ ] Breadboard/PCB for connections
- [ ] Jumper wires and connectors

**Physical Components:**
- [ ] 2x Water Pumps (12V)
- [ ] 2x Solenoid Valves (12V) 
- [ ] 2x LED Grow Light Strips (12V)
- [ ] 2x 12V Fans (Exhaust & Circulation)
- [ ] 1x Heating Element (12V)
- [ ] Misting nozzles for mushrooms
- [ ] Drip irrigation tubing for cilantro
- [ ] 2x Water reservoirs/tanks
- [ ] Growing containers/beds

---

## ⚡ Quick Start (30 Minutes)

### Step 1: Hardware Assembly (15 mins)
1. **Mount ESP32** in waterproof enclosure
2. **Connect power supply** (12V input, 5V for ESP32)
3. **Wire relay module** to GPIO pins 18-27
4. **Connect sensors** to analog pins A0-A3 and digital pins 2,4,5

### Step 2: Software Upload (10 mins)
1. **Download Arduino IDE** and install ESP32 support
2. **Install required libraries** (WiFi, DHT, OneWire, ArduinoJson)
3. **Upload firmware** `enhanced_mushroom_cilantro_system.ino`
4. **Open Serial Monitor** to see boot messages

### Step 3: Initial Setup (5 mins)
1. **Connect to WiFi hotspot** "MushroomCilantro_System"
2. **Open browser** to 192.168.4.1
3. **Configure your WiFi** credentials
4. **Test basic controls** through web interface

---

## 🔧 Detailed Setup

### 📱 WiFi Configuration

**Method 1: Hotspot Mode (Recommended)**
```
1. Power on the system
2. Look for WiFi network: "MushroomCilantro_System"
3. Password: "RDTRC2024"
4. Open browser → http://192.168.4.1
5. Enter your home WiFi credentials
6. System will restart and connect to your WiFi
```

**Method 2: Serial Configuration**
```
1. Connect ESP32 to computer via USB
2. Open Serial Monitor (115200 baud)
3. Follow on-screen prompts to enter WiFi details
4. Note the IP address shown after connection
```

### 🌐 Web Interface Access

After WiFi connection:
- **Local Network:** http://[ESP32_IP_ADDRESS]
- **Hotspot Mode:** http://192.168.4.1
- **Features Available:**
  - Real-time sensor monitoring
  - Manual pump/light controls  
  - Schedule management
  - System alerts and status

### 📊 Initial Sensor Calibration

**Soil Moisture Sensor:**
```
1. Place sensor in dry soil → Note reading (should be ~0-10%)
2. Place sensor in saturated soil → Note reading (should be ~90-100%)
3. Adjust code if needed for your specific sensor
```

**Water Level Sensors:**
```
1. Place in empty tank → Should read 0%
2. Fill tank completely → Should read 100%
3. Mark tank levels for easy monitoring
```

**Temperature Sensors:**
```
1. DHT22 sensors should read room temperature
2. DS18B20 probe should match ambient if not in water
3. Allow 2-3 minutes for accurate readings
```

---

## 🍄 Mushroom Growing Setup

### 🏠 Growing Environment

**Container Setup:**
```
Growing Chamber (Plastic Storage Container):
├── Size: 60cm x 40cm x 30cm minimum
├── Ventilation holes with filters
├── Misting nozzles at top corners
├── Drainage holes at bottom
└── Mushroom substrate bags/blocks inside
```

**Environmental Controls:**
- **Humidity:** 80-95% (controlled by misting + exhaust fan)
- **Temperature:** 20-25°C (controlled by heater)
- **Light:** Indirect LED, 12 hours on/off cycle
- **Air Circulation:** Gentle fan for air exchange

### 📅 Mushroom Schedule

**Default Misting Times:**
- 06:00 - Morning mist (30 sec) + lights ON
- 10:00 - Mid-morning mist (30 sec)
- 14:00 - Afternoon mist (30 sec)  
- 18:00 - Evening mist (30 sec) + lights OFF

**Growth Stages:**
1. **Week 1:** Incubation (high humidity, no light)
2. **Week 2:** Pinning (maintain humidity, add light)
3. **Week 3:** Fruiting (continue misting, monitor growth)
4. **Week 4+:** Harvest (reduce misting frequency)

---

## 🌿 Cilantro Growing Setup

### 🌱 Growing Environment

**Container Setup:**
```
Growing Bed (Planter Box or Hydroponic System):
├── Size: 50cm x 30cm x 20cm minimum
├── Drainage holes at bottom
├── Growing medium: Potting soil or coco coir
├── Drip irrigation lines spaced 10cm apart
└── Seeds or seedlings planted 5cm spacing
```

**Environmental Controls:**
- **Soil Moisture:** 60-75% (controlled by drip irrigation)
- **Temperature:** 17-27°C (shared heater with mushrooms)
- **Light:** Direct LED, 8 hours daily
- **pH:** 6.0-7.0 (test and adjust growing medium)

### 📅 Cilantro Schedule

**Default Watering Times:**
- 07:00 - Morning water (3 min) + lights ON
- 19:00 - Evening lights OFF
- **Auto-watering:** When soil moisture < 60%

**Growth Stages:**
1. **Week 1:** Germination (keep soil moist)
2. **Week 2:** Seedling (light watering daily)
3. **Week 3:** Vegetative growth (normal schedule)
4. **Week 4+:** Mature (harvest outer leaves)

---

## 🎛️ System Controls

### 🖥️ Web Interface Functions

**Dashboard Sections:**
```
🍄 Mushroom Status:
├── Current humidity & temperature
├── Water level in reservoir
├── Growth stage indicator
├── Next misting countdown
└── Manual controls (Start/Stop misting, Toggle lights)

🌿 Cilantro Status:
├── Soil moisture & temperature  
├── Water level in reservoir
├── Growth stage indicator
├── Next watering countdown
└── Manual controls (Start/Stop watering, Toggle lights)

📊 System Info:
├── Uptime and connection status
├── Environmental controls status
├── Alert notifications
└── System controls (Restart, Reset)
```

**Manual Override:**
- All automatic functions can be manually controlled
- Emergency stop buttons for pumps
- Light controls independent of schedule
- Alert acknowledgment and clearing

### 📱 Mobile App Integration

**Blink App Setup:**
1. Download Blink app from app store
2. Create account and log in
3. Add new device using QR code or manual setup
4. Control system remotely from anywhere

**Available Mobile Controls:**
- Start/stop watering and misting
- Toggle lights on/off
- View real-time sensor data
- Receive push notifications for alerts
- Adjust schedules and settings

---

## ⚠️ Important Safety Notes

### 🔒 Electrical Safety
- **Always disconnect power** before making wiring changes
- **Use GFCI protection** for all water pump circuits
- **Keep electronics dry** - use proper enclosures
- **Check connections regularly** for corrosion or loosening

### 💧 Water Safety
- **Monitor water levels** daily to prevent dry running pumps
- **Use food-safe materials** for water contact surfaces
- **Clean reservoirs weekly** to prevent algae/bacteria
- **Check for leaks** regularly around connections

### 🌡️ Environmental Safety
- **Monitor temperatures** to prevent overheating
- **Ensure adequate ventilation** to prevent CO2 buildup
- **Use appropriate heaters** with thermal protection
- **Keep growing areas clean** to prevent contamination

---

## 🔧 Maintenance Schedule

### 📅 Daily (5 minutes)
- [ ] Check water levels in both reservoirs
- [ ] Verify system status via web interface
- [ ] Observe plant/mushroom health visually

### 📅 Weekly (30 minutes)
- [ ] Clean misting nozzles and drip emitters
- [ ] Test all sensor readings for accuracy
- [ ] Refill water reservoirs with fresh water
- [ ] Clean growing containers as needed
- [ ] Check for pests or diseases

### 📅 Monthly (2 hours)
- [ ] Deep clean all water systems
- [ ] Calibrate sensors if needed
- [ ] Update firmware if available
- [ ] Inspect all electrical connections
- [ ] Replace growing medium for cilantro
- [ ] Harvest mature mushrooms and cilantro

---

## 🆘 Emergency Procedures

### 🚨 System Alerts

**Low Water Alert:**
1. Immediately refill affected reservoir
2. Check for leaks in system
3. Clear alert via web interface

**High Temperature Alert:**
1. Check heater operation (may be stuck ON)
2. Ensure fans are working properly
3. Add temporary cooling if needed

**Sensor Failure Alert:**
1. Check sensor connections
2. Clean sensor contacts
3. Replace sensor if readings still incorrect

### 🔌 Power Failure Recovery

**When Power Returns:**
1. System will automatically restart
2. Check all sensors are reading correctly
3. Verify pumps and lights resume normal operation
4. Check for any error messages in Serial Monitor

---

## 📞 Support & Troubleshooting

### 🔍 Common Issues

**Problem: Can't connect to web interface**
- Check WiFi connection status
- Try accessing via hotspot mode
- Restart system if needed

**Problem: Sensors showing 0 or incorrect values**
- Check 3.3V power supply to sensors
- Verify wiring connections
- Clean sensor contacts

**Problem: Pumps not starting**
- Check 12V power supply
- Verify relay operation (listen for clicking)
- Test pump directly with 12V source

### 📧 Getting Help

- **Documentation:** Full manual in `/docs/README_TH.md`
- **Wiring Guide:** Detailed diagrams in `/wiring/wiring_diagram.md`
- **GitHub Issues:** Report bugs and request features
- **Community Forum:** Share experiences with other users

---

## 🎉 Success Tips

### 🏆 For Best Results

**Mushroom Growing:**
- Maintain consistent humidity (most critical factor)
- Keep growing area clean and sterile
- Don't over-mist (can cause bacterial issues)
- Harvest at right time (before spores release)

**Cilantro Growing:**
- Start with good quality seeds
- Don't overwater (causes root rot)
- Harvest frequently to encourage new growth
- Succession plant every 2 weeks for continuous harvest

**System Management:**
- Monitor daily but don't over-adjust
- Keep detailed logs of changes and results
- Start with default settings and adjust gradually
- Learn your local environmental patterns

---

*🌱 Happy Growing! Your automated system will help you achieve consistent, healthy crops year-round! 🌱*
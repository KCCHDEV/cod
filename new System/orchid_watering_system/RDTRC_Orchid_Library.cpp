/*
 * RDTRC Orchid Library Implementation
 * Version: 4.0
 * Firmware made by: RDTRC
 * Updated: 2024
 */

#include "RDTRC_Orchid_Library.h"

// ============================================================================
// RDTRCOrchidSensors Implementation
// ============================================================================

RDTRCOrchidSensors::RDTRCOrchidSensors() {
  totalSensors = 0;
  
  // Initialize sensor status
  for (int i = 0; i < 16; i++) {
    sensors[i].isOnline = false;
    sensors[i].lastReading = 0;
    sensors[i].lastValue = 0;
    sensors[i].errorCount = 0;
    sensors[i].calibrationOffset = 0;
    sensors[i].calibrationMultiplier = 1.0;
  }
}

void RDTRCOrchidSensors::initializeSoilSensors(int pins[8]) {
  for (int i = 0; i < 8; i++) {
    soilSensorPins[i] = pins[i];
    pinMode(pins[i], INPUT);
    
    // Initialize sensor info
    sensors[totalSensors].sensorName = "Soil_" + String(i + 1);
    sensors[totalSensors].pin = pins[i];
    sensors[totalSensors].isOnline = true;
    totalSensors++;
  }
}

void RDTRCOrchidSensors::initializeValves(int pins[6]) {
  for (int i = 0; i < 6; i++) {
    valvePins[i] = pins[i];
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW); // Ensure valves are closed
  }
}

void RDTRCOrchidSensors::initializeDHT(int pin, int type) {
  // DHT initialization is done in main sketch
  sensors[totalSensors].sensorName = "DHT22";
  sensors[totalSensors].pin = pin;
  sensors[totalSensors].isOnline = true;
  totalSensors++;
}

void RDTRCOrchidSensors::initializeLightSensor(int pin) {
  pinMode(pin, INPUT);
  sensors[totalSensors].sensorName = "Light";
  sensors[totalSensors].pin = pin;
  sensors[totalSensors].isOnline = true;
  totalSensors++;
}

void RDTRCOrchidSensors::initializePHSensor(int pin) {
  pinMode(pin, INPUT);
  sensors[totalSensors].sensorName = "pH";
  sensors[totalSensors].pin = pin;
  sensors[totalSensors].isOnline = true;
  totalSensors++;
}

void RDTRCOrchidSensors::initializeECSensor(int pin) {
  pinMode(pin, INPUT);
  sensors[totalSensors].sensorName = "EC";
  sensors[totalSensors].pin = pin;
  sensors[totalSensors].isOnline = true;
  totalSensors++;
}

void RDTRCOrchidSensors::initializeWaterLevelSensor(int trigPin, int echoPin) {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  sensors[totalSensors].sensorName = "WaterLevel";
  sensors[totalSensors].pin = trigPin;
  sensors[totalSensors].isOnline = true;
  totalSensors++;
}

void RDTRCOrchidSensors::initializeFlowSensor(int pin) {
  pinMode(pin, INPUT);
  sensors[totalSensors].sensorName = "Flow";
  sensors[totalSensors].pin = pin;
  sensors[totalSensors].isOnline = true;
  totalSensors++;
}

void RDTRCOrchidSensors::initializeLCD(int sdaPin, int sclPin) {
  Wire.begin(sdaPin, sclPin);
  sensors[totalSensors].sensorName = "LCD";
  sensors[totalSensors].pin = sdaPin;
  sensors[totalSensors].isOnline = true;
  totalSensors++;
}

float RDTRCOrchidSensors::readSoilMoisture(int sensorIndex) {
  if (sensorIndex < 0 || sensorIndex >= 8) return -1;
  
  int rawValue = analogRead(soilSensorPins[sensorIndex]);
  float moisture = map(rawValue, 4095, 0, 0, 100); // ESP32 ADC
  
  // Apply calibration
  moisture = (moisture * sensors[sensorIndex].calibrationMultiplier) + sensors[sensorIndex].calibrationOffset;
  
  // Update sensor status
  updateSensorStatus(sensorIndex, true, moisture);
  
  return moisture;
}

float RDTRCOrchidSensors::readTemperature() {
  // This should be called from main sketch with DHT object
  // For now, return a placeholder
  return 25.0;
}

float RDTRCOrchidSensors::readHumidity() {
  // This should be called from main sketch with DHT object
  // For now, return a placeholder
  return 60.0;
}

int RDTRCOrchidSensors::readLightLevel() {
  int lightIndex = 8; // Light sensor index
  int rawValue = analogRead(sensors[lightIndex].pin);
  int lightLevel = map(rawValue, 0, 4095, 0, 100);
  
  updateSensorStatus(lightIndex, true, lightLevel);
  return lightLevel;
}

float RDTRCOrchidSensors::readPHLevel() {
  int phIndex = 9; // pH sensor index
  int rawValue = analogRead(sensors[phIndex].pin);
  float voltage = (rawValue / 4095.0) * 3.3;
  float ph = 3.5 * voltage; // Basic conversion, should be calibrated
  
  updateSensorStatus(phIndex, true, ph);
  return ph;
}

float RDTRCOrchidSensors::readECLevel() {
  int ecIndex = 10; // EC sensor index
  int rawValue = analogRead(sensors[ecIndex].pin);
  float voltage = (rawValue / 4095.0) * 3.3;
  float ec = voltage * 2.0; // Basic conversion, should be calibrated
  
  updateSensorStatus(ecIndex, true, ec);
  return ec;
}

float RDTRCOrchidSensors::readWaterLevel() {
  int waterIndex = 11; // Water level sensor index
  int trigPin = sensors[waterIndex].pin;
  int echoPin = sensors[waterIndex].pin + 1; // Assuming echo pin is next
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;
  float waterLevel = 50 - distance; // Assuming tank height is 50cm
  
  updateSensorStatus(waterIndex, true, waterLevel);
  return waterLevel;
}

float RDTRCOrchidSensors::readFlowRate() {
  int flowIndex = 12; // Flow sensor index
  int rawValue = analogRead(sensors[flowIndex].pin);
  float flowRate = rawValue / 10.0; // Basic conversion
  
  updateSensorStatus(flowIndex, true, flowRate);
  return flowRate;
}

bool RDTRCOrchidSensors::isSensorOnline(int sensorIndex) {
  if (sensorIndex < 0 || sensorIndex >= totalSensors) return false;
  return sensors[sensorIndex].isOnline;
}

void RDTRCOrchidSensors::updateSensorStatus(int sensorIndex, bool isOnline, float value) {
  if (sensorIndex < 0 || sensorIndex >= totalSensors) return;
  
  sensors[sensorIndex].isOnline = isOnline;
  sensors[sensorIndex].lastReading = millis();
  sensors[sensorIndex].lastValue = value;
  
  if (isOnline) {
    sensors[sensorIndex].errorCount = 0;
  }
}

void RDTRCOrchidSensors::handleSensorError(int sensorIndex) {
  if (sensorIndex < 0 || sensorIndex >= totalSensors) return;
  
  sensors[sensorIndex].errorCount++;
  if (sensors[sensorIndex].errorCount >= 3) {
    sensors[sensorIndex].isOnline = false;
  }
}

void RDTRCOrchidSensors::resetSensorError(int sensorIndex) {
  if (sensorIndex < 0 || sensorIndex >= totalSensors) return;
  
  sensors[sensorIndex].errorCount = 0;
  sensors[sensorIndex].isOnline = true;
}

int RDTRCOrchidSensors::getOnlineSensorCount() {
  int count = 0;
  for (int i = 0; i < totalSensors; i++) {
    if (sensors[i].isOnline) count++;
  }
  return count;
}

int RDTRCOrchidSensors::getTotalSensorCount() {
  return totalSensors;
}

String RDTRCOrchidSensors::getSensorStatusString() {
  String status = "";
  for (int i = 0; i < totalSensors; i++) {
    status += sensors[i].sensorName + ":";
    status += sensors[i].isOnline ? "ONLINE" : "OFFLINE";
    status += "(" + String(sensors[i].lastValue, 1) + ")";
    if (i < totalSensors - 1) status += ",";
  }
  return status;
}

void RDTRCOrchidSensors::calibrateSoilSensor(int sensorIndex, float dryValue, float wetValue) {
  if (sensorIndex < 0 || sensorIndex >= 8) return;
  
  // Calculate calibration parameters
  float range = wetValue - dryValue;
  sensors[sensorIndex].calibrationMultiplier = 100.0 / range;
  sensors[sensorIndex].calibrationOffset = -dryValue * sensors[sensorIndex].calibrationMultiplier;
}

void RDTRCOrchidSensors::calibratePHSensor(float ph4Value, float ph7Value, float ph10Value) {
  int phIndex = 9;
  // Store calibration values for pH sensor
  sensors[phIndex].calibrationOffset = ph7Value;
  sensors[phIndex].calibrationMultiplier = (ph10Value - ph4Value) / 6.0;
}

void RDTRCOrchidSensors::calibrateECSensor(float ec1Value, float ec2Value) {
  int ecIndex = 10;
  // Store calibration values for EC sensor
  sensors[ecIndex].calibrationOffset = ec1Value;
  sensors[ecIndex].calibrationMultiplier = (ec2Value - ec1Value) / 1.0;
}

void RDTRCOrchidSensors::gracefulDegradation() {
  // Check for offline sensors and adjust system behavior
  int offlineCount = totalSensors - getOnlineSensorCount();
  
  if (offlineCount > totalSensors / 2) {
    // More than half sensors offline - enter emergency mode
    Serial.println("WARNING: Multiple sensors offline - Emergency mode activated");
  }
}

bool RDTRCOrchidSensors::canOperateWithOfflineSensors() {
  int onlineCount = getOnlineSensorCount();
  return onlineCount >= 3; // Need at least 3 sensors to operate safely
}

String RDTRCOrchidSensors::getDegradationStatus() {
  int onlineCount = getOnlineSensorCount();
  int offlineCount = totalSensors - onlineCount;
  
  String status = "Online: " + String(onlineCount) + "/" + String(totalSensors);
  status += " | Offline: " + String(offlineCount);
  
  if (offlineCount == 0) {
    status += " | Status: OPTIMAL";
  } else if (offlineCount <= 2) {
    status += " | Status: DEGRADED";
  } else {
    status += " | Status: EMERGENCY";
  }
  
  return status;
}

// ============================================================================
// RDTRCOrchidWatering Implementation
// ============================================================================

RDTRCOrchidWatering::RDTRCOrchidWatering() {
  numZones = 0;
}

void RDTRCOrchidWatering::addZone(int zoneIndex, String orchidType, int interval, int duration, int threshold, int valvePin, int sensorIndex) {
  if (zoneIndex < 0 || zoneIndex >= 6) return;
  
  zones[zoneIndex].orchidType = orchidType;
  zones[zoneIndex].wateringInterval = interval;
  zones[zoneIndex].wateringDuration = duration;
  zones[zoneIndex].soilMoistureThreshold = threshold;
  zones[zoneIndex].isActive = true;
  zones[zoneIndex].lastWatering = 0;
  zones[zoneIndex].valvePin = valvePin;
  zones[zoneIndex].soilSensorIndex = sensorIndex;
  
  if (zoneIndex >= numZones) {
    numZones = zoneIndex + 1;
  }
}

void RDTRCOrchidWatering::setZoneActive(int zoneIndex, bool active) {
  if (zoneIndex < 0 || zoneIndex >= numZones) return;
  zones[zoneIndex].isActive = active;
}

void RDTRCOrchidWatering::updateZoneSchedule(int zoneIndex, int interval, int duration, int threshold) {
  if (zoneIndex < 0 || zoneIndex >= numZones) return;
  
  zones[zoneIndex].wateringInterval = interval;
  zones[zoneIndex].wateringDuration = duration;
  zones[zoneIndex].soilMoistureThreshold = threshold;
}

bool RDTRCOrchidWatering::shouldWaterZone(int zoneIndex, float soilMoisture, float temperature, float humidity) {
  if (zoneIndex < 0 || zoneIndex >= numZones) return false;
  if (!zones[zoneIndex].isActive) return false;
  
  // Check if enough time has passed since last watering
  unsigned long timeSinceLastWatering = millis() - zones[zoneIndex].lastWatering;
  unsigned long intervalMs = zones[zoneIndex].wateringInterval * 3600000UL; // Convert hours to ms
  
  if (timeSinceLastWatering < intervalMs) return false;
  
  // Check soil moisture threshold
  if (soilMoisture > zones[zoneIndex].soilMoistureThreshold) return false;
  
  // Check environmental conditions
  if (temperature < 15 || temperature > 35) return false;
  if (humidity < 30 || humidity > 90) return false;
  
  return true;
}

void RDTRCOrchidWatering::waterZone(int zoneIndex, int duration) {
  if (zoneIndex < 0 || zoneIndex >= numZones) return;
  
  // Open valve
  digitalWrite(zones[zoneIndex].valvePin, HIGH);
  
  // Wait for specified duration
  delay(duration);
  
  // Close valve
  digitalWrite(zones[zoneIndex].valvePin, LOW);
  
  // Update last watering time
  zones[zoneIndex].lastWatering = millis();
}

void RDTRCOrchidWatering::emergencyWatering(int zoneIndex, int duration) {
  if (zoneIndex < 0 || zoneIndex >= numZones) return;
  
  // Emergency watering bypasses all checks
  waterZone(zoneIndex, duration);
}

void RDTRCOrchidWatering::stopAllWatering() {
  for (int i = 0; i < numZones; i++) {
    digitalWrite(zones[i].valvePin, LOW);
  }
}

void RDTRCOrchidWatering::checkWateringSchedule() {
  // This function should be called from main loop
  // It will check each zone and water if needed
}

unsigned long RDTRCOrchidWatering::getTimeSinceLastWatering(int zoneIndex) {
  if (zoneIndex < 0 || zoneIndex >= numZones) return 0;
  return millis() - zones[zoneIndex].lastWatering;
}

String RDTRCOrchidWatering::getZoneStatus(int zoneIndex) {
  if (zoneIndex < 0 || zoneIndex >= numZones) return "INVALID_ZONE";
  
  String status = zones[zoneIndex].orchidType + ":";
  status += zones[zoneIndex].isActive ? "ACTIVE" : "INACTIVE";
  status += "|Last:" + String(getTimeSinceLastWatering(zoneIndex) / 3600000UL) + "h";
  
  return status;
}

String RDTRCOrchidWatering::getAllZonesStatus() {
  String status = "";
  for (int i = 0; i < numZones; i++) {
    status += getZoneStatus(i);
    if (i < numZones - 1) status += ",";
  }
  return status;
}

void RDTRCOrchidWatering::adjustForOrchidType(int zoneIndex, float temperature, float humidity, int lightLevel) {
  if (zoneIndex < 0 || zoneIndex >= numZones) return;
  
  String orchidType = zones[zoneIndex].orchidType;
  
  // Adjust watering based on orchid type and conditions
  if (orchidType == "Vanda") {
    // Vanda needs more water in high humidity
    if (humidity > 70) {
      zones[zoneIndex].wateringDuration = (int)(zones[zoneIndex].wateringDuration * 1.2);
    }
  } else if (orchidType == "Cattleya") {
    // Cattleya needs less water in low light
    if (lightLevel < 30) {
      zones[zoneIndex].wateringDuration = (int)(zones[zoneIndex].wateringDuration * 0.8);
    }
  }
}

bool RDTRCOrchidWatering::isOptimalConditions(float temperature, float humidity, int lightLevel) {
  return (temperature >= 18 && temperature <= 30) &&
         (humidity >= 50 && humidity <= 80) &&
         (lightLevel >= 20 && lightLevel <= 80);
}

String RDTRCOrchidWatering::getOrchidCareRecommendations(int zoneIndex) {
  if (zoneIndex < 0 || zoneIndex >= numZones) return "Invalid zone";
  
  String orchidType = zones[zoneIndex].orchidType;
  String recommendations = "";
  
  if (orchidType == "Phalaenopsis") {
    recommendations = "Keep moist, avoid direct sun, temp 20-30C";
  } else if (orchidType == "Cattleya") {
    recommendations = "Allow to dry between watering, bright light";
  } else if (orchidType == "Dendrobium") {
    recommendations = "Moderate water, good air circulation";
  } else if (orchidType == "Vanda") {
    recommendations = "High humidity, frequent misting";
  } else if (orchidType == "Oncidium") {
    recommendations = "Even moisture, moderate light";
  } else if (orchidType == "Cymbidium") {
    recommendations = "Cool temps, moderate water";
  }
  
  return recommendations;
}

// ============================================================================
// RDTRCOrchidEnvironment Implementation
// ============================================================================

RDTRCOrchidEnvironment::RDTRCOrchidEnvironment() {
  // Set default thresholds for orchids
  orchidThresholds.tempMin = 18.0;
  orchidThresholds.tempMax = 30.0;
  orchidThresholds.humidityMin = 50.0;
  orchidThresholds.humidityMax = 80.0;
  orchidThresholds.phMin = 5.5;
  orchidThresholds.phMax = 6.5;
  orchidThresholds.ecMin = 0.5;
  orchidThresholds.ecMax = 2.0;
  orchidThresholds.lightMin = 20;
  orchidThresholds.lightMax = 80;
}

bool RDTRCOrchidEnvironment::isTemperatureOptimal(float temperature) {
  return temperature >= orchidThresholds.tempMin && temperature <= orchidThresholds.tempMax;
}

bool RDTRCOrchidEnvironment::isHumidityOptimal(float humidity) {
  return humidity >= orchidThresholds.humidityMin && humidity <= orchidThresholds.humidityMax;
}

bool RDTRCOrchidEnvironment::isPHOptimal(float ph) {
  return ph >= orchidThresholds.phMin && ph <= orchidThresholds.phMax;
}

bool RDTRCOrchidEnvironment::isECOptimal(float ec) {
  return ec >= orchidThresholds.ecMin && ec <= orchidThresholds.ecMax;
}

bool RDTRCOrchidEnvironment::isLightOptimal(int lightLevel) {
  return lightLevel >= orchidThresholds.lightMin && lightLevel <= orchidThresholds.lightMax;
}

bool RDTRCOrchidEnvironment::isEnvironmentOptimal(float temp, float humidity, float ph, float ec, int light) {
  return isTemperatureOptimal(temp) && isHumidityOptimal(humidity) && 
         isPHOptimal(ph) && isECOptimal(ec) && isLightOptimal(light);
}

String RDTRCOrchidEnvironment::assessOrchidHealth(float temp, float humidity, float ph, float ec, int light, int soilMoisture) {
  int score = getEnvironmentalScore(temp, humidity, ph, ec, light);
  
  if (score >= 90) return "EXCELLENT";
  else if (score >= 75) return "GOOD";
  else if (score >= 60) return "FAIR";
  else if (score >= 40) return "POOR";
  else return "CRITICAL";
}

String RDTRCOrchidEnvironment::getEnvironmentalRecommendations(float temp, float humidity, float ph, float ec, int light) {
  String recommendations = "";
  
  if (!isTemperatureOptimal(temp)) {
    if (temp < orchidThresholds.tempMin) {
      recommendations += "Increase temperature, ";
    } else {
      recommendations += "Decrease temperature, ";
    }
  }
  
  if (!isHumidityOptimal(humidity)) {
    if (humidity < orchidThresholds.humidityMin) {
      recommendations += "Increase humidity, ";
    } else {
      recommendations += "Decrease humidity, ";
    }
  }
  
  if (!isPHOptimal(ph)) {
    if (ph < orchidThresholds.phMin) {
      recommendations += "Increase pH, ";
    } else {
      recommendations += "Decrease pH, ";
    }
  }
  
  if (!isECOptimal(ec)) {
    if (ec < orchidThresholds.ecMin) {
      recommendations += "Increase nutrients, ";
    } else {
      recommendations += "Decrease nutrients, ";
    }
  }
  
  if (!isLightOptimal(light)) {
    if (light < orchidThresholds.lightMin) {
      recommendations += "Increase light, ";
    } else {
      recommendations += "Decrease light, ";
    }
  }
  
  if (recommendations == "") {
    recommendations = "All conditions optimal";
  }
  
  return recommendations;
}

int RDTRCOrchidEnvironment::getEnvironmentalScore(float temp, float humidity, float ph, float ec, int light) {
  int score = 100;
  
  // Temperature scoring
  if (!isTemperatureOptimal(temp)) {
    float tempDiff = abs(temp - (orchidThresholds.tempMin + orchidThresholds.tempMax) / 2);
    score -= (int)(tempDiff * 5);
  }
  
  // Humidity scoring
  if (!isHumidityOptimal(humidity)) {
    float humidityDiff = abs(humidity - (orchidThresholds.humidityMin + orchidThresholds.humidityMax) / 2);
    score -= (int)(humidityDiff * 2);
  }
  
  // pH scoring
  if (!isPHOptimal(ph)) {
    float phDiff = abs(ph - (orchidThresholds.phMin + orchidThresholds.phMax) / 2);
    score -= (int)(phDiff * 10);
  }
  
  // EC scoring
  if (!isECOptimal(ec)) {
    float ecDiff = abs(ec - (orchidThresholds.ecMin + orchidThresholds.ecMax) / 2);
    score -= (int)(ecDiff * 15);
  }
  
  // Light scoring
  if (!isLightOptimal(light)) {
    int lightDiff = abs(light - (orchidThresholds.lightMin + orchidThresholds.lightMax) / 2);
    score -= lightDiff;
  }
  
  return max(0, score);
}

void RDTRCOrchidEnvironment::setTemperatureThresholds(float min, float max) {
  orchidThresholds.tempMin = min;
  orchidThresholds.tempMax = max;
}

void RDTRCOrchidEnvironment::setHumidityThresholds(float min, float max) {
  orchidThresholds.humidityMin = min;
  orchidThresholds.humidityMax = max;
}

void RDTRCOrchidEnvironment::setPHThresholds(float min, float max) {
  orchidThresholds.phMin = min;
  orchidThresholds.phMax = max;
}

void RDTRCOrchidEnvironment::setECThresholds(float min, float max) {
  orchidThresholds.ecMin = min;
  orchidThresholds.ecMax = max;
}

void RDTRCOrchidEnvironment::setLightThresholds(int min, int max) {
  orchidThresholds.lightMin = min;
  orchidThresholds.lightMax = max;
}

String RDTRCOrchidEnvironment::getEnvironmentalDataString(float temp, float humidity, float ph, float ec, int light, int soilMoisture) {
  String data = "T:" + String(temp, 1) + "C ";
  data += "H:" + String(humidity, 1) + "% ";
  data += "pH:" + String(ph, 1) + " ";
  data += "EC:" + String(ec, 1) + " ";
  data += "L:" + String(light) + "% ";
  data += "S:" + String(soilMoisture) + "%";
  
  return data;
}

void RDTRCOrchidEnvironment::logEnvironmentalData(float temp, float humidity, float ph, float ec, int light, int soilMoisture) {
  // This would typically write to SPIFFS or send to cloud
  String data = getEnvironmentalDataString(temp, humidity, ph, ec, light, soilMoisture);
  Serial.println("ENV_LOG: " + data);
}

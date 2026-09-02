#include <SoftwareSerial.h> 
 
// ---------------- PIN CONNECTIONS ---------------- 
#define TRIG_PIN 7 
#define ECHO_PIN 6 
#define BUZZER_PIN 8 
 
// SIM800L 
// Arduino D10 <- SIM800L TX 
// Arduino D11 -> SIM800L RX 
SoftwareSerial gsm(10, 11); 
 
// ---------------- TANK SETTINGS ---------------- 
#define TANK_HEIGHT 100.0   // Tank height in cm 
 
#define LOW_LEVEL 20        // Low water: below 20% 
#define FULL_LEVEL 90       // Full tank: 90% or above 
 
// ---------------- PHONE NUMBER ---------------- 
String phoneNumber = "+91XXXXXXXXXX"; 
 
// ---------------- ALERT FLAGS ---------------- 
bool lowAlert = false; 
bool fullAlert = false; 
 
 
// ================================================= 
// SETUP 
// ================================================= 
void setup() { 
 
  Serial.begin(9600); 
  gsm.begin(9600); 
 
  pinMode(TRIG_PIN, OUTPUT); 
  pinMode(ECHO_PIN, INPUT); 
  pinMode(BUZZER_PIN, OUTPUT); 
 
  digitalWrite(BUZZER_PIN, LOW); 
 
  Serial.println("================================"); 
  Serial.println(" SMART WATER TANK MONITORING"); 
  Serial.println("================================"); 
 
  delay(3000); 
 
  // Initialize GSM 
  gsm.println("AT"); 
  delay(1000); 
 
  gsm.println("AT+CMGF=1"); 
  delay(1000); 
 
  Serial.println("GSM Module Ready"); 
} 
 
 
// ================================================= 
// MAIN LOOP 
// ================================================= 
void loop() { 
 
  // Measure distance 
  float distance = getDistance(); 
 
  Serial.print("Distance: "); 
  Serial.print(distance); 
  Serial.println(" cm"); 
 
  // Calculate water level 
  float waterHeight = TANK_HEIGHT - distance; 
 
  if (waterHeight < 0) 
    waterHeight = 0; 
 
  if (waterHeight > TANK_HEIGHT) 
    waterHeight = TANK_HEIGHT; 
 
  int waterLevel = (waterHeight / TANK_HEIGHT) * 100; 
 
  Serial.print("Water Level: "); 
  Serial.print(waterLevel); 
  Serial.println("%"); 
 
 
  // ================================================= 
  // LOW WATER LEVEL 
  // ================================================= 
 
  if (waterLevel < LOW_LEVEL) { 
 
    Serial.println("WARNING: WATER LEVEL LOW!"); 
 
    digitalWrite(BUZZER_PIN, HIGH); 
    delay(500); 
    digitalWrite(BUZZER_PIN, LOW); 
 
    // Send SMS only once 
    if (lowAlert == false) { 
 
      sendSMS("ALERT: Water level is LOW. Please refill the tank."); 
 
      lowAlert = true; 
    } 
  } 
 
  else { 
    lowAlert = false; 
  } 
 
 
  // ================================================= 
  // FULL / OVERFLOW LEVEL 
  // ================================================= 
 
  if (waterLevel >= FULL_LEVEL) { 
 
    Serial.println("WARNING: TANK FULL!"); 
 
    digitalWrite(BUZZER_PIN, HIGH); 
    delay(500); 
    digitalWrite(BUZZER_PIN, LOW); 
 
    // Send SMS only once 
    if (fullAlert == false) { 
 
      sendSMS("ALERT: Water tank is FULL. Please switch OFF the motor."); 
 
      fullAlert = true; 
    } 
  } 
 
  else { 
    fullAlert = false; 
  } 
 
 
  // ================================================= 
  // NORMAL LEVEL 
  // ================================================= 
 
  if (waterLevel >= LOW_LEVEL && 
      waterLevel < FULL_LEVEL) { 
 
    digitalWrite(BUZZER_PIN, LOW); 
 
    Serial.println("Water Level: NORMAL"); 
  } 
 
  Serial.println("----------------------------"); 
 
  delay(2000); 
} 
 
 
// ================================================= 
// HC-SR04 DISTANCE FUNCTION 
// ================================================= 
 
float getDistance() { 
 
  long duration; 
  float distance; 
 
  // Send ultrasonic pulse 
  digitalWrite(TRIG_PIN, LOW); 
  delayMicroseconds(2); 
 
  digitalWrite(TRIG_PIN, HIGH); 
  delayMicroseconds(10); 
 
  digitalWrite(TRIG_PIN, LOW); 
 
  // Receive echo 
  duration = pulseIn(ECHO_PIN, HIGH, 30000); 
 
  // If no signal 
  if (duration == 0) { 
    return TANK_HEIGHT; 
  } 
 
  // Calculate distance 
  distance = duration * 0.0343 / 2; 
 
  return distance; 
} 
 
 
// ================================================= 
// SEND SMS USING SIM800L 
// ================================================= 
 
void sendSMS(String message) { 
 
  Serial.println("Sending SMS..."); 
 
  // SMS text mode 
  gsm.println("AT+CMGF=1"); 
  delay(1000); 
 
  // Phone number 
  gsm.print("AT+CMGS=\""); 
  gsm.print(phoneNumber); 
  gsm.println("\""); 
 
  delay(1000); 
 
  // Message 
  gsm.print(message); 
 
  // CTRL+Z 
  gsm.write(26); 
 
  delay(5000); 
 
  Serial.println("SMS Sent Successfully!"); 
} 

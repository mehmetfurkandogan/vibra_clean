/////////////////////////////////////////////////////////////////////////////
// VIBRA_CLEAN
// ME420 - Advanced Design for Sustainable Future - Group 4
// 19.11.2024
///////////////////////////////////////////////////////////////////////////// LIBRARIES
#include <WiFi.h>
#include <ESP32Servo.h>

///////////////////////////////////////////////////////////////////////////// CUSTOM DEFINES
// Motor Driver Pins
#define A3 14
#define A4 13
#define waterLevelSensor 19
#define servoPin 12
#define vibrationA 9
#define turbiditySensor 20

///////////////////////////////////////////////////////////////////////////// CUSTOM VARIABLES
// Servo
Servo myservo;

// Water level monitoring
volatile int waterLevel = 0;
const int threshold = 1600;

// Turbidity Monitoring
volatile int turbidityLevel;

// Motor Control
volatile int motorInput = 0;

// Servo control variables
int targetPosition = 90;
float rotationDuration = 0;
unsigned long rotationStartTime = 0;
bool isRotating = false;
unsigned long lastCWtime = 0;
unsigned long portioningInterval = 1000;
unsigned long unclogTime = 100;

// System Flags
bool washingFlag = false;

///////////////////////////////////////////////////////////////////////////// SETUP
void setup() {
  Serial.begin(115200);  // Initialize Serial communication
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(vibrationA, OUTPUT);

  pinMode(waterLevelSensor, INPUT);
  pinMode(turbiditySensor, INPUT);
  pinMode(servoPin, OUTPUT);

  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  digitalWrite(vibrationA, LOW);

  myservo.attach(servoPin);
}

///////////////////////////////////////////////////////////////////////////// LOOP
void loop() {

  myservo.detach();  // Detach the servo
  delay(10);
  myservo.attach(servoPin);

  // Read water level and turbidity data
  waterLevel = analogRead(waterLevelSensor);
  turbidityLevel = analogRead(turbiditySensor);

  // If washing process is active
  if (washingFlag) {
    analogWrite(vibrationA, 200);  // Vibrate for washing
    int error = threshold - waterLevel;
    motorInput = constrain(map(error, 0, threshold, 0, 255), 0, 220);

    if (waterLevel < threshold) {
      analogWrite(A3, motorInput);
      analogWrite(A4, motorInput);
    } else {
      analogWrite(A3, 0);
      analogWrite(A4, 0);
    }
  } else {
    analogWrite(vibrationA, 0);  // Stop vibration
    analogWrite(A3, 0);          // Stop motor
    analogWrite(A4, 0);
  }

  // Check if serial input is available
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');  // Read the command

    if (command.startsWith("SET_POSITION")) {
      int pos = command.substring(13).toInt();  // Extract position
      myservo.write(pos);
      Serial.println("Servo position set to: " + String(pos));
    } else if (command == "START_WASHING") {
      washingFlag = true;
      Serial.println("Washing started.");
    } else if (command == "STOP_WASHING") {
      washingFlag = false;
      Serial.println("Washing stopped.");
    } else if (command == "STOP_SERVO") {
      myservo.write(94);
      delay(10);
      Serial.println("Servo stopped and detached.");
    }
  }

  delay(10);  // Small delay for stability
}

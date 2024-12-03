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
#define green 11

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
unsigned long rotationStartTime = 0;
unsigned long rotationDuration = 0;  // Duration in milliseconds
bool isRotating = false;

// System Flags
bool washingFlag = false;
bool cycleFlag = true;


// PID Params
int error;
int prev_error = 0;
int delta_error;
float Kd = 0.2;

///////////////////////////////////////////////////////////////////////////// SETUP
void setup() {
  Serial.begin(115200);  // Initialize Serial communication
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(vibrationA, OUTPUT);

  pinMode(waterLevelSensor, INPUT);
  pinMode(turbiditySensor, INPUT);
  pinMode(servoPin, OUTPUT);
  pinMode(green, OUTPUT);


  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  digitalWrite(vibrationA, LOW);

  myservo.attach(servoPin);
}

///////////////////////////////////////////////////////////////////////////// LOOP
void loop() {
  myservo.detach();  // Detach the servo for a moment (optional, to prevent jitter)
  delay(10);
  myservo.attach(servoPin);

  // Read water level and turbidity data
  waterLevel = analogRead(waterLevelSensor);
  turbidityLevel = analogRead(turbiditySensor);


  if (waterLevel > 600 && cycleFlag) {
    cycleFlag = false;
  }

  if (waterLevel < 500) {
    cycleFlag = true;
  }

  // If washing process is active
  if (washingFlag && cycleFlag) {
    digitalWrite(green, HIGH);
    analogWrite(vibrationA, 4095);  // Vibrate for washing
    error = threshold - waterLevel;
    delta_error = error - prev_error;
    prev_error = error;
    motorInput = constrain(map(error + delta_error * Kd, 0, threshold, 0, 4095), 0, 3600);
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
    digitalWrite(green, LOW);
  }

  // Check if serial input is available
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');  // Read the command

    if (command.startsWith("SET_POSITION")) {
      // Extract the duration from the command
      String durationStr = command.substring(13);          // Get everything after "SET_POSITION "
      rotationDuration = durationStr.toInt() / 20 * 1000;  // Convert to integer (milliseconds)

      if (rotationDuration > 0) {
        Serial.println("Servo will rotate for: " + String(rotationDuration) + "ms");

        // Set the servo to the target position (e.g., 90 degrees) and start rotation
        targetPosition = 180;  // Set to 90 degrees (or any other desired position)
        myservo.write(targetPosition);
        delay(10);

        // Record the start time for the rotation duration
        rotationStartTime = millis();
        isRotating = true;  // Flag to indicate that the servo is rotating
      } else {
        Serial.println("Invalid rotation duration");
      }
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

  // If the servo is rotating, check if the duration has passed
  if (isRotating) {
    if (millis() - rotationStartTime >= rotationDuration) {
      myservo.write(94);  // Stop the servo rotation after the duration
      delay(10);
      isRotating = false;  // Reset the rotating flag
      Serial.println("Servo rotation complete.");
    }
  }

  delay(10);  // Small delay for stability
}

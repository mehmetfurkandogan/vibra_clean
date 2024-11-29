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
#define waterLevelSensor 19  // Change this to the correct analog pin
#define servoPin 12
#define yellow 10
#define green 11
#define vibrationA 9

///////////////////////////////////////////////////////////////////////////// CUSTOM VARIABLES
// Servo
Servo myservo;

// Wi-Fi credentials
const char* ssid = "mfd";
const char* password = "2QqepaUH28j3yYr";

// Water level monitoring
volatile int waterLevel = 0;
const int threshold = 1600;

// Motor Control
volatile int motorInput = 0;

// HTTP server
WiFiServer server(80);

// Servo control variables
int targetPosition = 90;
int rotationDuration = 0;
unsigned long rotationStartTime = 0;
bool isRotating = false;

// System Flags
bool washingFlag = false;

///////////////////////////////////////////////////////////////////////////// FUNCTION DEFINITIONS

///////////////////////////////////////////////////////////////////////////// SETUP
void setup() {
  Serial.begin(115200);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(vibrationA, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);

  pinMode(waterLevelSensor, INPUT);
  pinMode(servoPin, OUTPUT);

  digitalWrite(yellow, LOW);
  digitalWrite(green, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(vibrationA, LOW);
  delay(10);
  digitalWrite(A4, LOW);
  delay(10);

  // Create FreeRTOS tasks
  xTaskCreatePinnedToCore(webServerTask, "WebServerTask", 10000, NULL, 1, NULL, 1);  // Core 1
}

///////////////////////////////////////////////////////////////////////////// LOOP
void loop() {
  // Read the water level
  waterLevel = analogRead(waterLevelSensor);
  int error = threshold - waterLevel;
  motorInput = constrain(map(error, 0, threshold, 0, 255), 0, 220);
  // if (waterLevel < threshold) {
  //   analogWrite(A3, motorInput);
  //   delay(10);
  //   analogWrite(A4, motorInput);
  //   delay(10);
  // } else {
  //   analogWrite(A3, 0);
  //   delay(10);
  //   analogWrite(A4, 0);
  //   delay(10);
  // }
  delay(10);  // Small delay for stability
}

///////////////////////////////////////////////////////////////////////////// Task: Web Server and Servo Control
void webServerTask(void* parameter) {
  myservo.attach(servoPin);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  digitalWrite(yellow, HIGH);

  while (true) {
    WiFiClient client = server.available();
    if (client) {
      String currentLine = "";
      String header = "";

      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          header += c;

          if (c == '\n' && currentLine.length() == 0) {
            // HTTP response
            if (header.indexOf("GET /motorInput") >= 0) {
              // Respond with motor input value (plain text)
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/plain");
              client.println("Connection: close");
              client.println();
              client.println(motorInput);  // Send motor input value
            } else if (header.indexOf("GET /waterLevel") >= 0) {
              // Respond with water level value (plain text)
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/plain");
              client.println("Connection: close");
              client.println();
              client.println(waterLevel);  // Send water level value
            } else {
              // Full HTML response for other requests
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              client.println("<!DOCTYPE html>");
              client.println("<html>");
              client.println("<head>");
              client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
              client.println("<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>");
              client.println("</head>");
              client.println("<body>");
              client.println("<div class='header'>");
              client.println("<img src='https://upload.wikimedia.org/wikipedia/commons/9/95/Logo_EPFL_2019.svg' alt='EPFL Logo'>");
              client.println("<img src='https://images.squarespace-cdn.com/content/v1/66e0a1b90e50d05da21c996f/ef2ff8f5-7d70-4679-973c-abf29e29d44a/logo-black.png' alt='Other Logo'>");
              client.println("</div>");
              client.println("<h1>Motor Input Value: <span id='motorInputValue'>0</span></h1>");
              client.println("<canvas id='motorInputChart' width='200' height='100'></canvas>");
              client.println("<h1>Water Level: <span id='waterLevelValue'>0</span></h1>");
              client.println("<canvas id='waterLevelChart' width='200' height='100'></canvas>");
              client.println("<div class='container'>");
              client.println("<h1>Portioning System</h1>");
              client.println("<label for='timeInput'>Rotation Time (seconds): </label>");
              client.println("<input type='number' id='timeInput' value='1' min='0'><br><br>");
              client.println("<button class='btn-green' onclick='setServo(45)'>CW</button>");
              client.println("<button class='btn-gray' onclick='setServo(94)'>STOP</button>");
              client.println("<button class='btn-blue' onclick='setServo(130)'>CCW</button>");
              client.println("</div>");
              client.println("<div class='container'>");
              client.println("<h1>Cleaning Process</h1>");
              client.println("<button class='btn-green' onclick='setWashingFlag(1)'>START</button>");
              client.println("<button class='btn-red' onclick='setWashingFlag(0)'>STOP</button>");
              client.println("</div>");
              client.println("<script>");
              client.println("var motorInput = 0;");
              client.println("var waterLevel = 0;");
              client.println("var motorInputData = [];");
              client.println("var waterLevelData = [];");
              client.println("var chartMotor = new Chart(document.getElementById('motorInputChart'), {");
              client.println("    type: 'line',");
              client.println("    data: {");
              client.println("        labels: [],");
              client.println("        datasets: [{");
              client.println("            label: 'Motor Input Value',");
              client.println("            borderColor: 'rgba(0, 123, 255, 1)',");
              client.println("            fill: false,");
              client.println("            data: []");
              client.println("        }]");
              client.println("    },");
              client.println("    options: {");
              client.println("        scales: {");
              client.println("            x: {");
              client.println("                type: 'linear',");
              client.println("                position: 'bottom'");
              client.println("            }");
              client.println("        }");
              client.println("    }");
              client.println("});");

              client.println("var chartWater = new Chart(document.getElementById('waterLevelChart'), {");
              client.println("    type: 'line',");
              client.println("    data: {");
              client.println("        labels: [],");
              client.println("        datasets: [{");
              client.println("            label: 'Water Level',");
              client.println("            borderColor: 'rgba(0, 255, 0, 1)',");
              client.println("            fill: false,");
              client.println("            data: []");
              client.println("        }]");
              client.println("    },");
              client.println("    options: {");
              client.println("        scales: {");
              client.println("            x: {");
              client.println("                type: 'linear',");
              client.println("                position: 'bottom'");
              client.println("            }");
              client.println("        }");
              client.println("    }");
              client.println("});");

              client.println("function updateMotorInput() {");
              client.println("    fetch('/motorInput')  // Request the motor input from ESP32");
              client.println("    .then(response => response.text())");
              client.println("    .then(data => {");
              client.println("        motorInput = parseInt(data);");
              client.println("        if (isNaN(motorInput)) {");
              client.println("            console.error('Invalid motor input: ', data);");
              client.println("        } else {");
              client.println("            motorInputData.push(motorInput);");
              client.println("            if (motorInputData.length > 500) motorInputData.shift();");
              client.println("            chartMotor.data.labels = Array.from({ length: motorInputData.length }, (_, i) => i);");
              client.println("            chartMotor.data.datasets[0].data = motorInputData;");
              client.println("            chartMotor.update();");
              client.println("            document.getElementById('motorInputValue').textContent = motorInput;");
              client.println("        }");
              client.println("    });");
              client.println("}");

              client.println("function updateWaterLevel() {");
              client.println("    fetch('/waterLevel')  // Request the water level from ESP32");
              client.println("    .then(response => response.text())");
              client.println("    .then(data => {");
              client.println("        waterLevel = parseInt(data);");
              client.println("        if (isNaN(waterLevel)) {");
              client.println("            console.error('Invalid water level: ', data);");
              client.println("        } else {");
              client.println("            waterLevelData.push(waterLevel);");
              client.println("            if (waterLevelData.length > 500) waterLevelData.shift();");
              client.println("            chartWater.data.labels = Array.from({ length: waterLevelData.length }, (_, i) => i);");
              client.println("            chartWater.data.datasets[0].data = waterLevelData;");
              client.println("            chartWater.update();");
              client.println("            document.getElementById('waterLevelValue').textContent = waterLevel;");
              client.println("        }");
              client.println("    });");
              client.println("}");

              client.println("setInterval(updateMotorInput, 50);  // Update motor input every second");
              client.println("setInterval(updateWaterLevel, 50);  // Update water level every second");
              client.println("</script>");
              client.println("</body>");
              client.println("</html>");
            }
            break;
          }

          if (c == '\n') currentLine = "";
          else if (c != '\r') currentLine += c;
        }
      }

      client.stop();
    }

    delay(10);
  }
}

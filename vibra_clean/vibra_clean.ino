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


  pinMode(waterLevelSensor, INPUT);
  pinMode(servoPin, OUTPUT);
  digitalWrite(A3, LOW);
  delay(10);
  digitalWrite(A4, LOW);
  delay(10);


  // Create FreeRTOS tasks
  xTaskCreatePinnedToCore(webServerTask, "WebServerTask", 10000, NULL, 1, NULL, 1);  // Core 1
}

///////////////////////////////////////////////////////////////////////////// LOOP
void loop() {
  if (washingFlag == 1) {
    waterLevel = analogRead(waterLevelSensor);
    int error = threshold - waterLevel;
    motorInput = constrain(map(error, 0, threshold, 0, 255), 0, 255);
    // Serial.println(motorInput);
    if (waterLevel < threshold) {
      analogWrite(A3, motorInput);
      delay(10);
      analogWrite(A4, motorInput);
      delay(10);
    } else {
      analogWrite(A3, 0);
      delay(10);
      analogWrite(A4, 0);
      delay(10);
    }

    delay(10);  // Small delay for stability
  } else {
    analogWrite(A3, 0);
    delay(10);
    analogWrite(A4, 0);
    delay(10);
  }
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

  while (true) {
    WiFiClient client = server.available();
    if (client) {
      Serial.println("New Client.");
      String currentLine = "";
      String header = "";

      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          header += c;

          if (c == '\n' && currentLine.length() == 0) {
            // HTTP response
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // HTML content
            client.println("<!DOCTYPE html>");
            client.println("<html>");
            client.println("<head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>");
            client.println("body {");
            client.println("  font-family: Helvetica, sans-serif;");
            client.println("  text-align: center;");
            client.println("  margin: 0;");
            client.println("  background: url('https://plus.unsplash.com/premium_photo-1661917179706-33e305a4ee45?q=80&w=1470&auto=format&fit=crop&ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D') no-repeat center center fixed;");
            client.println("  color: white;");
            client.println("}");
            client.println(".header {");
            client.println("  display: flex;");
            client.println("  justify-content: space-around;");
            client.println("  align-items: center;");
            client.println("  max-width: 600px;");  // Match the width of the boxes below
            client.println("  margin: 0 auto;");    // Center align
            client.println("  padding: 20px;");
            client.println("  background-color: rgba(255, 255, 255, 0.1);");
            client.println("}");
            client.println(".header img {");
            client.println("  max-height: 80px;");
            client.println("  object-fit: contain;");  // Ensure images scale proportionally without distortion
            client.println("  width: auto;");          // Maintain aspect ratio by adjusting width automatically
            client.println("  max-width: 45%;");       // Scale images proportionally within the header
            client.println("}");
            client.println(".container {");
            client.println("  margin: 50px auto;");
            client.println("  max-width: 600px;");
            client.println("  background-color: rgba(255, 255, 255, 0.1);");
            client.println("  padding: 20px;");
            client.println("  border-radius: 15px;");
            client.println("  box-shadow: 0 4px 10px rgba(0, 0, 0, 0.3);");
            client.println("}");
            client.println("button {");
            client.println("  font-size: 18px;");
            client.println("  padding: 15px 30px;");
            client.println("  margin: 10px;");
            client.println("  border: none;");
            client.println("  border-radius: 5px;");
            client.println("  cursor: pointer;");
            client.println("  transition: transform 0.2s, background-color 0.3s;");
            client.println("}");
            client.println("button:hover {");
            client.println("  transform: scale(1.1);");
            client.println("}");
            client.println(".btn-green { background-color: #28a745; color: white; }");
            client.println(".btn-green:hover { background-color: #218838; }");
            client.println(".btn-red { background-color: #dc3545; color: white; }");
            client.println(".btn-red:hover { background-color: #c82333; }");
            client.println(".btn-blue { background-color: #007bff; color: white; }");
            client.println(".btn-blue:hover { background-color: #0056b3; }");
            client.println(".btn-gray { background-color: #6c757d; color: white; }");
            client.println(".btn-gray:hover { background-color: #5a6268; }");
            client.println("input[type='number'] {");
            client.println("  padding: 10px;");
            client.println("  font-size: 18px;");
            client.println("  margin-top: 10px;");
            client.println("  border-radius: 5px;");
            client.println("  border: 2px solid #ccc;");
            client.println("}");
            client.println("h2 {");
            client.println("  font-size: 24px;");
            client.println("  margin-top: 30px;");
            client.println("}");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");

            // Add images in a header with max-width
            client.println("<div class='header'>");
            client.println("<img src='https://upload.wikimedia.org/wikipedia/commons/9/95/Logo_EPFL_2019.svg' alt='EPFL Logo'>");
            client.println("<img src='https://images.squarespace-cdn.com/content/v1/66e0a1b90e50d05da21c996f/ef2ff8f5-7d70-4679-973c-abf29e29d44a/logo-black.png' alt='Other Logo'>");
            client.println("</div>");


            client.println("<div class='container'>");
            client.println("<h1>Portioning System</h1>");
            client.println("<label for='timeInput'>Rotation Time (seconds): </label>");
            client.println("<input type='number' id='timeInput' value='1' min='0'><br><br>");
            client.println("<button class='btn-green' onclick='setServo(0)'>CW</button>");
            client.println("<button class='btn-gray' onclick='setServo(94)'>STOP</button>");
            client.println("<button class='btn-blue' onclick='setServo(180)'>CCW</button>");
            client.println("</div>");
            client.println("<div class='container'>");
            client.println("<h1>Cleaning Process</h1>");
            client.println("<button class='btn-green' onclick='setWashingFlag(1)'>START</button>");
            client.println("<button class='btn-red' onclick='setWashingFlag(0)'>STOP</button>");
            client.println("</div>");
            client.println("<p id='errorMessage' style='color:red;'></p>");
            client.println("<script>");
            client.println("function setServo(pos) {");
            client.println("    const timeInput = document.getElementById('timeInput').value;");
            client.println("    const errorMessage = document.getElementById('errorMessage');");
            client.println("    errorMessage.textContent = '';");
            client.println("    if (isNaN(timeInput) || timeInput <= 0) {");
            client.println("        errorMessage.textContent = 'Please enter a valid positive number for the time.';");
            client.println("        return;");
            client.println("    }");
            client.println("    const timeInMilliseconds = timeInput * 1000;");
            client.println("    fetch('/?value=' + pos + '&time=' + timeInMilliseconds)");
            client.println("        .catch((error) => { console.error('Error:', error); });");
            client.println("}");
            client.println("function setWashingFlag(flag) {");
            client.println("    fetch('/?washingFlag=' + flag)");
            client.println("        .catch((error) => { console.error('Error:', error); });");
            client.println("}");
            client.println("</script>");
            client.println("</body>");
            client.println("</html>");


            break;
          }
          if (c == '\n') currentLine = "";
          else if (c != '\r') currentLine += c;
        }
      }

      // Parse HTTP GET request
      if (header.indexOf("GET /?value=") >= 0) {
        int pos1 = header.indexOf('=') + 1;
        int pos2 = header.indexOf('&');
        String valueString = header.substring(pos1, pos2);
        targetPosition = valueString.toInt();

        int timeStart = header.indexOf("time=") + 5;
        int timeEnd = header.indexOf(' ', timeStart);
        String timeString = header.substring(timeStart, timeEnd);
        rotationDuration = timeString.toInt();

        if (targetPosition == 94) {
          ;
          myservo.write(94);  // Stop
          isRotating = false;
          Serial.println("Servo Stopped.");
        } else {
          myservo.write(targetPosition);
          rotationStartTime = millis();
          isRotating = true;
          Serial.println("Servo moving to position: " + valueString);
        }
      } else if (header.indexOf("GET /?washingFlag=") >= 0) {
        int pos1 = header.indexOf('=') + 1;
        int pos2 = header.indexOf(' ', pos1);
        String flagString = header.substring(pos1, pos2);
        washingFlag = flagString.toInt();
        Serial.println("Washing flag updated: " + flagString);
      }

      client.stop();
      Serial.println("Client disconnected.");
    }

    // Check if rotation duration has elapsed
    if (isRotating && (millis() - rotationStartTime >= rotationDuration)) {
      myservo.write(94);  // Stop the servo
      isRotating = false;
      Serial.println("Servo rotation completed.");
    }

    delay(10);
  }
}
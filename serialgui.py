import sys
import serial
import time
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QLineEdit, QLabel
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QColor, QPalette
import requests



# Set up the serial connection to the ESP32 (Make sure to use the correct COM port)
ser = serial.Serial('COM7', 115200, timeout=1)  # Replace 'COM7' with your actual serial port

def send_command(command):
    """Send command to ESP32 via serial."""
    try:
        ser.write((command + '\n').encode())  # Send command with newline
        time.sleep(0.5)  # Wait a bit for the response
        response = ser.readline().decode('utf-8').strip()  # Read response from ESP32
        print(response)  # For debugging
        return response  # Return response to show in GUI
    except Exception as e:
        print(f"Error communicating with ESP32: {e}")
        return f"Error: {e}"

def start_washing():
    return send_command("START_WASHING")

def stop_washing():
    return send_command("STOP_WASHING")

def set_position(position):
    """Set the servo position if valid."""
    if 0 <= position <= 180:
        return send_command(f"SET_POSITION {position}")
    else:
        return "Position must be between 0 and 180."

def stop_servo():
    return send_command("STOP_SERVO")

# PyQt GUI Application
class VibraCleanApp(QWidget):
    def __init__(self):
        super().__init__()

        # Set up the window
        self.setWindowTitle("Vibra Clean Control")
        self.setGeometry(100, 100, 350, 500)

        # Create layout
        self.layout = QVBoxLayout()

        # Servo Position input
        self.position_label = QLabel("Servo Position (0-180):")
        self.position_label.setStyleSheet("font-size: 18px; color: black;")  # Label style
        self.layout.addWidget(self.position_label)

        self.position_input = QLineEdit()
        self.position_input.setStyleSheet("""
            font-size: 18px;
            padding: 10px;
            border-radius: 5px;
            border: 2px solid #ccc;
        """)
        self.layout.addWidget(self.position_input)

        # Buttons Layout
        self.buttons_layout = QVBoxLayout()

        # Set Position Button
        self.set_position_button = QPushButton("Set Position")
        self.set_position_button.setStyleSheet("""
            background-color: #007bff;
            color: white;
            font-size: 18px;
            padding: 15px 30px;
            border-radius: 5px;
            margin: 10px;
        """)
        self.set_position_button.clicked.connect(self.on_set_position)
        self.buttons_layout.addWidget(self.set_position_button)

        # Start Washing Button
        self.start_button = QPushButton("Start Washing")
        self.start_button.setStyleSheet("""
            background-color: #28a745;
            color: white;
            font-size: 18px;
            padding: 15px 30px;
            border-radius: 5px;
            margin: 10px;
        """)
        self.start_button.clicked.connect(self.on_start_washing)
        self.buttons_layout.addWidget(self.start_button)

        # Stop Washing Button
        self.stop_button = QPushButton("Stop Washing")
        self.stop_button.setStyleSheet("""
            background-color: #dc3545;
            color: white;
            font-size: 18px;
            padding: 15px 30px;
            border-radius: 5px;
            margin: 10px;
        """)
        self.stop_button.clicked.connect(self.on_stop_washing)
        self.buttons_layout.addWidget(self.stop_button)

        # Stop Servo Button
        self.stop_servo_button = QPushButton("Stop Servo")
        self.stop_servo_button.setStyleSheet("""
            background-color: #6c757d;
            color: white;
            font-size: 18px;
            padding: 15px 30px;
            border-radius: 5px;
            margin: 10px;
        """)
        self.stop_servo_button.clicked.connect(self.on_stop_servo)
        self.buttons_layout.addWidget(self.stop_servo_button)

        self.layout.addLayout(self.buttons_layout)

        # Response Label
        self.response_label = QLabel("")
        self.response_label.setStyleSheet("font-size: 18px; color: white; margin-top: 20px;")
        self.layout.addWidget(self.response_label)

        # Set main layout
        self.setLayout(self.layout)

    def on_set_position(self):
        """Handle Set Position Button click"""
        try:
            position = int(self.position_input.text())
            response = set_position(position)
            self.response_label.setText(response)
        except ValueError:
            self.response_label.setText("Invalid input, please enter a valid number.")

    def on_start_washing(self):
        """Handle Start Washing Button click"""
        response = start_washing()
        self.response_label.setText(response)

    def on_stop_washing(self):
        """Handle Stop Washing Button click"""
        response = stop_washing()
        self.response_label.setText(response)

    def on_stop_servo(self):
        """Handle Stop Servo Button click"""
        response = stop_servo()
        self.response_label.setText(response)

# Main function to run the PyQt application
def main():
    app = QApplication(sys.argv)

    # Apply a global stylesheet for better font and UI appearance
    app.setStyleSheet("""
        QWidget {
            font-family: Helvetica, sans-serif;
        }
    """)

    window = VibraCleanApp()
    window.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()

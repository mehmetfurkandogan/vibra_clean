import sys
import serial
import time
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QLineEdit, QLabel
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QColor, QPalette, QFont, QPixmap


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
    send_command(f"SET_POSITION {position}")

def stop_servo():
    return send_command("STOP_SERVO")


# PyQt GUI Application
class VibraCleanApp(QWidget):
    def __init__(self):
        super().__init__()

        # Set up the window
        self.setWindowTitle("Vibra Clean Control")
        self.setGeometry(100, 100, 400, 600)
        self.setStyleSheet("""
            background-image: url('https://wallpapercave.com/wp/wp3592385.jpg');
            background-size: cover;
            background-position: center;
            font-family: Helvetica, sans-serif;
        """)

        # Create layout
        self.layout = QVBoxLayout()

        # Header Section
        self.header_layout = QHBoxLayout()

        self.logo1 = QLabel()
        self.logo1.setPixmap(QPixmap("https://upload.wikimedia.org/wikipedia/commons/9/95/Logo_EPFL_2019.svg").scaledToHeight(80, Qt.SmoothTransformation))
        self.header_layout.addWidget(self.logo1)

        self.logo2 = QLabel()
        self.logo2.setPixmap(QPixmap("https://images.squarespace-cdn.com/content/v1/66e0a1b90e50d05da21c996f/ef2ff8f5-7d70-4679-973c-abf29e29d44a/logo-black.png").scaledToHeight(80, Qt.SmoothTransformation))
        self.header_layout.addWidget(self.logo2)

        self.layout.addLayout(self.header_layout)

        # Portioning System
        self.portioning_container = QWidget()
        self.portioning_layout = QVBoxLayout()

        self.portioning_title = QLabel("Portioning System")
        self.portioning_title.setStyleSheet("font-size: 24px; font-weight: bold; color: white;")
        self.portioning_layout.addWidget(self.portioning_title)

        self.time_input_label = QLabel("Rotation Time (seconds):")
        self.time_input_label.setStyleSheet("color: white; font-size: 18px;")
        self.portioning_layout.addWidget(self.time_input_label)

        self.time_input = QLineEdit()
        self.time_input.setStyleSheet("""
            font-size: 18px;
            padding: 10px;
            margin-top: 10px;
            border-radius: 5px;
            border: 2px solid #ccc;
        """)
        self.portioning_layout.addWidget(self.time_input)

        self.cw_button = QPushButton("CW")
        self.cw_button.setStyleSheet("""
            background-color: #28a745;
            color: white;
            font-size: 18px;
            padding: 15px 30px;
            border-radius: 5px;
            margin: 10px;
        """)
        self.cw_button.clicked.connect(self.on_cw_click)
        self.portioning_layout.addWidget(self.cw_button)

        self.stop_button = QPushButton("STOP")
        self.stop_button.setStyleSheet("""
            background-color: #6c757d;
            color: white;
            font-size: 18px;
            padding: 15px 30px;
            border-radius: 5px;
            margin: 10px;
        """)
        self.stop_button.clicked.connect(self.on_stop_click)
        self.portioning_layout.addWidget(self.stop_button)

        self.ccw_button = QPushButton("CCW")
        self.ccw_button.setStyleSheet("""
            background-color: #007bff;
            color: white;
            font-size: 18px;
            padding: 15px 30px;
            border-radius: 5px;
            margin: 10px;
        """)
        self.ccw_button.clicked.connect(self.on_ccw_click)
        self.portioning_layout.addWidget(self.ccw_button)

        self.portioning_container.setLayout(self.portioning_layout)
        self.layout.addWidget(self.portioning_container)

        # Cleaning Process
        self.cleaning_container = QWidget()
        self.cleaning_layout = QVBoxLayout()

        self.cleaning_title = QLabel("Cleaning Process")
        self.cleaning_title.setStyleSheet("font-size: 24px; font-weight: bold; color: white;")
        self.cleaning_layout.addWidget(self.cleaning_title)

        self.start_washing_button = QPushButton("START")
        self.start_washing_button.setStyleSheet("""
            background-color: #28a745;
            color: white;
            font-size: 18px;
            padding: 15px 30px;
            border-radius: 5px;
            margin: 10px;
        """)
        self.start_washing_button.clicked.connect(self.on_start_washing)
        self.cleaning_layout.addWidget(self.start_washing_button)

        self.stop_washing_button = QPushButton("STOP")
        self.stop_washing_button.setStyleSheet("""
            background-color: #dc3545;
            color: white;
            font-size: 18px;
            padding: 15px 30px;
            border-radius: 5px;
            margin: 10px;
        """)
        self.stop_washing_button.clicked.connect(self.on_stop_washing)
        self.cleaning_layout.addWidget(self.stop_washing_button)

        self.cleaning_container.setLayout(self.cleaning_layout)
        self.layout.addWidget(self.cleaning_container)

        # Error Message
        self.error_message = QLabel("")
        self.error_message.setStyleSheet("color: red; font-size: 16px;")
        self.layout.addWidget(self.error_message)

        self.setLayout(self.layout)

    def on_cw_click(self):
        """Handle CW button click"""
        self.set_servo_position(0)

    def on_stop_click(self):
        """Handle STOP button click"""
        self.set_servo_position(94)

    def on_ccw_click(self):
        """Handle CCW button click"""
        self.set_servo_position(180)

    def set_servo_position(self, pos):
        """Set servo position with time"""
        time_input = self.time_input.text()
        if not time_input.isdigit() or int(time_input) <= 0:
            self.error_message.setText("Please enter a valid positive number for the time.")
            return
        time_in_seconds = int(time_input)
        self.error_message.setText('')
        response = set_position(pos)
        print(f"Servo set to position {pos} for {time_in_seconds} seconds.")
        self.error_message.setText(response)

    def on_start_washing(self):
        """Handle Start Washing button click"""
        response = start_washing()
        self.error_message.setText(response)

    def on_stop_washing(self):
        """Handle Stop Washing button click"""
        response = stop_washing()
        self.error_message.setText(response)

# Main function to run the PyQt application
def main():
    app = QApplication(sys.argv)

    window = VibraCleanApp()
    window.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()

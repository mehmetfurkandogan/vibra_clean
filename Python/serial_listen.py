import serial
import time
import matplotlib.pyplot as plt
import csv

# Set up the serial connection (adjust 'COM7' and baud rate to match your setup)
ser = serial.Serial('COM7', 115200)  # Replace with the correct port and baud rate

# Give some time for the serial connection to establish
time.sleep(2)

# Define the CSV file path
csv_file = 'sensor_data.csv'

start_time = time.time()  # Start time for elapsed time calculation

# Open the CSV file in write mode to overwrite content initially and add the header
with open(csv_file, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["Time (seconds)", "Turbidity Level", "Water Level"])

try:
    print("Listening to serial data...")

    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()  # Read the serial data

            # Assuming the Arduino sends data in the format "turbidityLevel, waterLevel"
            data = line.split("\t")
            print(data)

            if len(data) == 2:  # Now expecting 2 values: turbidityLevel, and waterLevel
                try:
                    turbidityLevel = int(data[0])  # Convert turbidityLevel to integer
                    waterLevel = int(data[1])  # Convert waterLevel to integer

                    # Calculate the elapsed time
                    elapsed_time = time.time() - start_time

                    # Write the data to the CSV file immediately after receiving new data
                    with open(csv_file, mode='a', newline='') as file:  # Open in append mode
                        writer = csv.writer(file)
                        writer.writerow([elapsed_time, turbidityLevel, waterLevel])

                except ValueError:
                    # Handle case where the data is not as expected
                    print(f"Invalid data format: {line}")
            else:
                print(f"Data is not in expected format: {line}")

        # Optional: Add a small delay to avoid overwhelming the serial buffer
        time.sleep(0.1)

except KeyboardInterrupt:
    # Handle the Ctrl+C interruption to stop the process
    print("\nData collection stopped. Exiting...")

except Exception as e:
    print(f"Unexpected error: {e}")

finally:
    # Read the collected data from the CSV file and plot it
    timeData = []
    turbidityData = []
    waterLevelData = []

    try:
        with open(csv_file, mode='r') as file:
            reader = csv.reader(file)
            next(reader)  # Skip the header row

            # Read each row and append to the data lists
            for row in reader:
                if len(row) == 3:
                    timeData.append(float(row[0]))  # Time (seconds)
                    turbidityData.append(int(row[1]))  # Turbidity Level
                    waterLevelData.append(int(row[2]))  # Water Level

        # Now plot the data if it exists
        if turbidityData and waterLevelData:
            # Create a figure with 2 subplots (2 rows, 1 column)
            fig, axs = plt.subplots(2, 1, figsize=(10, 8))

            # Plot turbidity data with time
            axs[0].plot(timeData, turbidityData, label="Turbidity Level", color='b')
            axs[0].set_xlabel("Time (seconds)")
            axs[0].set_ylabel("Turbidity Level")
            axs[0].set_title("Turbidity Data over Time")
            axs[0].legend()

            # Plot water level data with time
            axs[1].plot(timeData, waterLevelData, label="Water Level", color='g')
            axs[1].set_xlabel("Time (seconds)")
            axs[1].set_ylabel("Water Level")
            axs[1].set_title("Water Level Data over Time")
            axs[1].legend()

            # Adjust layout for better spacing
            plt.tight_layout()

            # Display the plot
            plt.show()

        else:
            print("No data was collected.")
    except FileNotFoundError:
        print(f"Error: The file '{csv_file}' was not found.")
    except Exception as e:
        print(f"Unexpected error while reading the CSV: {e}")

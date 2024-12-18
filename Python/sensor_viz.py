import matplotlib.pyplot as plt
import csv
import numpy as np
from matplotlib.ticker import MaxNLocator
import matplotlib.patches as mpatches

# Define the CSV file path
csv_file = 'sensor_data_1.csv'

# Read the collected data from the CSV file
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

except FileNotFoundError:
    print(f"Error: The file '{csv_file}' was not found.")
except Exception as e:
    print(f"Unexpected error while reading the CSV: {e}")

# Smooth the data using a simple moving average
def smooth_data(data, window_size=1):
    """Smooth the data using a simple moving average."""
    return np.convolve(data, np.ones(window_size)/window_size, mode='valid')

# Apply smoothing to the data
smoothed_turbidity = turbidityData
smoothed_water_level = waterLevelData

idx = np.where((np.array(smoothed_turbidity) > 1800)&(np.array(smoothed_turbidity) < 2120))

# Use the indices to filter the data
smoothed_turbidity_filtered = np.array(smoothed_turbidity)[idx]  # Convert to array and apply the indices
time_turb = np.array(timeData)[idx]  # Similarly filter timeData using the same indices

# Convert back to lists if necessary
smoothed_turbidity_filtered = smoothed_turbidity_filtered.tolist()
time_turb = time_turb.tolist()

smoothed_turbidity_filtered = smooth_data(smoothed_turbidity_filtered, window_size=5)
time_turb = time_turb[2:-2]

# Adjust time data to match the length of smoothed data
smoothed_time = timeData[len(timeData) - len(smoothed_water_level):] 
time_thres = 382

smoothed_turbidity = 2400*np.ones_like(smoothed_turbidity) - np.array(smoothed_turbidity)
smoother_turbidity_filtered = 2400*np.ones_like(smoothed_turbidity_filtered) - np.array(smoothed_turbidity_filtered)

# Create a figure with 2 subplots (2 rows, 1 column)
fig, axs = plt.subplots(2, 1, figsize=(10, 8))

# Plot smoothed turbidity data with time
axs[0].plot(smoothed_time, smoothed_turbidity, label="Raw Turbidity", color='b', alpha=0.2)
# plot a line at y = 1700
axs[0].axhline(y=2400, color='g', linestyle='--', label='Clean Water')
axs[0].axhline(y=1700, color='r', linestyle='--', label='Air')
axs[0].plot(time_turb, smoothed_turbidity_filtered, label="Used Water", color='b')

axs[0].axvspan(0, 183, color='lightblue', alpha=0.3, label = "Pumps Running")  # First region: time 0 to 400
axs[0].axvspan(225, time_thres, color='lightblue', alpha=0.3)  # First region: time 0 to 400

axs[0].set_xlabel("Time (minutes)")
axs[0].set_ylabel("Turbidity Level")
axs[0].set_ylim(1500, 2600)
axs[0].set_title("Turbidity Data over Time")
axs[0].legend(loc='upper right')

# Plot smoothed water level data with time
axs[1].plot(smoothed_time, smoothed_water_level, label="Water Level", color='g')

axs[1].axvspan(0, 183, color='lightblue', alpha=0.3, label = "Pumps Running")  # First region: time 0 to 400
axs[1].axvspan(225, time_thres, color='lightblue', alpha=0.3)  # First region: time 0 to 400

# axs[1].fill_between([0, 400], 0, smoothed_water_level, color='g', alpha=0.5)
axs[1].set_xlabel("Time (minutes)")
axs[1].set_ylabel("Water Level")
axs[1].set_title("Water Level Data over Time")
axs[1].legend()

axs[0].xaxis.set_major_locator(MaxNLocator(integer=True, prune='both'))
axs[0].xaxis.set_major_formatter(plt.FuncFormatter(lambda x, _: f"{int(x // 60):02}:00"))
axs[1].xaxis.set_major_locator(MaxNLocator(integer=True, prune='both'))
axs[1].xaxis.set_major_formatter(plt.FuncFormatter(lambda x, _: f"{int(x // 60):02}:00"))

# Adjust layout for better spacing
plt.tight_layout()

# Display the plot
plt.show()


import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

# Given data (excluding the "S" value)
time = [0, 1, 3, 5]  # Time in seconds
weights = [
    [0],  # Time 0s: CCW (just an initial point)
    [19.2, 18.8, 7.4, 18.5, 24.5, 18.5, 19.3, 24.1, 20.2, 19.0, 18.4],  # Time 1s: CCW
    [61.4, 64.7, 56.4, 62.8, 61.9],  # Time 3s: CCW
    [104.2, 105.2, 105.4, 104.5, 106.8]  # Time 5s: CCW
]

# Flatten the weights and associate each value with its time
flattened_time = []
flattened_weights = []
for t, weight_set in zip(time, weights):
    flattened_time.extend([t] * len(weight_set))  # Duplicate time for each weight value
    flattened_weights.extend(weight_set)  # Append the weights

# Ensure that the data point at time 0 is always 0g (just for consistency)
flattened_time = [t if t != 0 else 0 for t in flattened_time]
flattened_weights = [w if t != 0 else 0 for t, w in zip(flattened_time, flattened_weights)]

# Fit function with intercept fixed to 0 (i.e., y = a * x)
def linear_fit_fixed(x, a):
    return a * x

# Time for fitting
x_data = np.array(flattened_time)
y_data = np.array(flattened_weights)

# Fit the data to the linear function with intercept fixed at 0
params, covariance = curve_fit(linear_fit_fixed, x_data, y_data)

# Extract the slope parameter 'a'
a = params[0]
print(f"Linear fit parameter: a = {a}")

# Generate fitted values
x_fit = np.linspace(0, 5, 100)
y_fit = linear_fit_fixed(x_fit, a)

# Print all data points
for t, w in zip(flattened_time, flattened_weights):
    print(f"Time: {t}s, Weight: {w}g")

# Plot the data and the fitted curve
plt.scatter(flattened_time, flattened_weights, color='blue', label='Data Points')
plt.plot(x_fit, y_fit, color='red', label=f'Linear Fit: y = {a:.2f}x')
plt.xlabel('Time (seconds)')
plt.ylabel('Weight (g)')
plt.title('Weight vs Time (Linear Fit with b = 0)')
plt.legend()
plt.grid(True)
plt.show()

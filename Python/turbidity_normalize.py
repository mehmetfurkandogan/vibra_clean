# Define the original data and the max value (4095) and the range we want to map to (0 to 3.3)
original_values = [
    1800, 2380, 2100, 2150, 2200, 800, 2000, 2380, 1920, 800, 2100, 2000, 2100
]
max_value = 4095
min_value = 0
new_min = 0
new_max = 3.3  # since you're using 3.3V

# Function to normalize a value
def normalize(value, min_value, max_value, new_min, new_max):
    return ((value - min_value) / (max_value - min_value)) * (new_max - new_min) + new_min

# Normalize all values
normalized_values = [normalize(value, min_value, max_value, new_min, new_max) for value in original_values]

# Function for turbidity conversion using the original polynomial
def turb_conversion(x):
    # Shift x for 3.3V input range
    x_shifted = x + 2
    # Apply the turbidity conversion formula with the shifted x value
    return -1120.4 * (x_shifted**2) + 5742.3 * x_shifted - 4353.9

# Apply the turbidity conversion to the normalized values
turb_values = [turb_conversion(value) for value in normalized_values]

# Output the normalized values and turbidity values
print("Original Values:", original_values)
print("Normalized Values:", normalized_values)
print("Turbidity Values (Shifted for 3.3V):", turb_values)

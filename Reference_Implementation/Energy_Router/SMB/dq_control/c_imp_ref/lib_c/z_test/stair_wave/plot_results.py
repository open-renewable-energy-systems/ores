import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Read the CSV file
data = pd.read_csv('stair_wave_angles.csv')

print("Raw data:")
print(data)

# Create the plot
plt.figure(figsize=(12, 6))

# Get the data for plotting
angles = data['Angle (rad)'].values
sum_indices = data['Sum_Index'].values

# Plot with zero-order hold (step plot)
plt.step(angles, sum_indices, 'b-', where='post', linewidth=2, label='Output Level')
plt.plot(angles, sum_indices, 'ro', label='Sample Points')

plt.xlabel('Angle (rad)')
plt.ylabel('Sum Index')
plt.title('Stair Wave Output vs Angle')
plt.grid(True)
plt.legend()

# Set y-axis limits
plt.ylim(min(sum_indices) - 0.5, max(sum_indices) + 0.5)

# Add pi markers on x-axis
plt.xticks([0, np.pi/2, np.pi, 3*np.pi/2, 2*np.pi],
           ['0', 'π/2', 'π', '3π/2', '2π'])

# Show the plot and keep it open until window is closed
plt.show()




import matplotlib.pyplot as plt
import numpy as np

# Load the data from the file
data = np.loadtxt('data.txt')

# Extract the x and y columns
x = data[:, 0]
y = data[:, 1]

# Create the plot
plt.plot(x, y)

# Add title and labels
plt.title('Plot of Data')
plt.xlabel('X')
plt.ylabel('Y')

# Show the plot
plt.show()
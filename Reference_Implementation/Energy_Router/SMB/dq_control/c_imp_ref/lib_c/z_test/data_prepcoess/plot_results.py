import pandas as pd
import matplotlib.pyplot as plt
import sys

def plot_data(csv_file):
    # Read the CSV file
    df = pd.read_csv(csv_file)
    
    # Debug print
    print("Time range (us):", df['time_us'].min(), "to", df['time_us'].max())
    
    # Convert time_us to milliseconds and adjust to start from 0
    start_time = df['time_us'].min()
    df['time_ms'] = (df['time_us'] - start_time) / 1000.0
    
    print("Time range (ms):", df['time_ms'].min(), "to", df['time_ms'].max())
    
    # Filter data for first 200ms
    df = df[df['time_ms'] <= 200]
    print("Points in 200ms window:", len(df))
    
    # Create figure and subplots
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))
    fig.suptitle('DQ Control Analysis (First 200ms)')
    
    # Plot 1: DQ values (raw and filtered)
    ax1.plot(df['time_ms'], df['d'], 'b-', label='d (raw)', alpha=0.5)
    ax1.plot(df['time_ms'], df['q'], 'r-', label='q (raw)', alpha=0.5)
    ax1.plot(df['time_ms'], df['filtered_d'], 'b--', label='filtered d')
    ax1.plot(df['time_ms'], df['filtered_q'], 'r--', label='filtered q')
    ax1.set_xlabel('Time (ms)')
    ax1.set_ylabel('Current (A)')
    ax1.legend()
    ax1.grid(True)
    ax1.set_title('D-Q Components')
    
    # Plot 2: Current magnitude and angle
    ax2.plot(df['time_ms'], df['curr_val'], 'g-', label='Magnitude')
    ax2.set_xlabel('Time (ms)')
    ax2.set_ylabel('Current Magnitude (A)', color='g')
    ax2.tick_params(axis='y', labelcolor='g')
    ax2.grid(True)
    
    ax2_twin = ax2.twinx()
    ax2_twin.plot(df['time_ms'], df['curr_angle'], 'b-', label='Angle')
    ax2_twin.set_ylabel('Current Angle (rad)', color='b')
    ax2_twin.tick_params(axis='y', labelcolor='b')
    
    # Add legends
    lines1, labels1 = ax2.get_legend_handles_labels()
    lines2, labels2 = ax2_twin.get_legend_handles_labels()
    ax2.legend(lines1 + lines2, labels1 + labels2, loc='upper right')
    
    # Adjust layout
    plt.tight_layout()
    
    # Save the plot
    output_file = csv_file.rsplit('.', 1)[0] + '_plot.png'
    plt.savefig(output_file)
    print(f"Plot saved as {output_file}")
    
    # Show the plot
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <csv_file>")
        sys.exit(1)
    
    plot_data(sys.argv[1])
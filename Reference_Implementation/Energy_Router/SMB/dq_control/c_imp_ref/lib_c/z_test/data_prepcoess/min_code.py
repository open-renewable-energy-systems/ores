from dataclasses import dataclass
from typing import List, Dict, Optional
import re
import os
import csv
import matplotlib.pyplot as plt
import numpy as np

@dataclass
class TransformData:
    timestamp: int
    time_us: int
    curr_val: float
    curr_angle: float
    beta: float
    d: float
    q: float
    filtered_d: float
    filtered_q: float
    mod_index: float
    phase_shift: float

def parse_timestamp(ts_str: str) -> int:
    return int(ts_str)

def parse_transform_line(line: str) -> Optional[Dict]:
    # Match all float values in the line
    pattern = r'time_us:(\d+),\s*curr_val:\s*([-\d.]+),\s*curr_angle:\s*([-\d.]+),\s*beta:\s*([-\d.]+),\s*d:\s*([-\d.]+),\s*q:\s*([-\d.]+),\s*filtered_d:\s*([-\d.]+),\s*filtered_q:\s*([-\d.]+),\s*mod_index:\s*([-\d.]+),\s*phase_shift:\s*([-\d.]+)'
    match = re.search(pattern, line)
    
    if match:
        return {
            'time_us': int(match.group(1)),
            'curr_val': float(match.group(2)),
            'curr_angle': float(match.group(3)),
            'beta': float(match.group(4)),
            'd': float(match.group(5)),
            'q': float(match.group(6)),
            'filtered_d': float(match.group(7)),
            'filtered_q': float(match.group(8)),
            'mod_index': float(match.group(9)),
            'phase_shift': float(match.group(10))
        }
    return None

def read_log_file(file_path: str) -> List[TransformData]:
    transform_data_list = []
    current_timestamp = None
    
    with open(file_path, 'r') as file:
        for line in file:
            line = line.strip()
            if 'T:' in line and '(' in line:
                timestamp_match = re.search(r'T:\s*(\d+)ms', line)
                if timestamp_match:
                    current_timestamp = parse_timestamp(timestamp_match.group(1))
            elif '<st_ufl_transform>' in line:
                transform_data = parse_transform_line(line)
                if transform_data and current_timestamp:
                    transform_data_list.append(TransformData(
                        timestamp=current_timestamp,
                        **transform_data
                    ))
    return transform_data_list

def save_to_csv(data_list: List[TransformData], output_path: str):
    # Define CSV headers based on TransformData fields
    headers = [
        'timestamp', 'time_us', 'curr_val', 'curr_angle', 'beta',
        'd', 'q', 'filtered_d', 'filtered_q', 'mod_index', 'phase_shift'
    ]
    
    try:
        with open(output_path, 'w', newline='') as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=headers)
            writer.writeheader()
            
            # Write each data entry
            for data in data_list:
                writer.writerow({
                    'timestamp': data.timestamp,
                    'time_us': data.time_us,
                    'curr_val': data.curr_val,
                    'curr_angle': data.curr_angle,
                    'beta': data.beta,
                    'd': data.d,
                    'q': data.q,
                    'filtered_d': data.filtered_d,
                    'filtered_q': data.filtered_q,
                    'mod_index': data.mod_index,
                    'phase_shift': data.phase_shift
                })
        print(f"Data saved to {output_path}")
    except Exception as e:
        print(f"Error saving CSV file: {e}")

def plot_data(data_list: List[TransformData]):
    import matplotlib.pyplot as plt
    import numpy as np
    
    # Extract time values and convert to milliseconds
    time_ms = np.array([data.time_us / 1000.0 for data in data_list])
    # Normalize time to start from 0
    time_ms = time_ms - time_ms[0]
    
    # Extract other values
    curr_val = [data.curr_val for data in data_list]
    curr_angle = [data.curr_angle for data in data_list]
    beta = [data.beta for data in data_list]
    alpha = [b + np.pi/2 for b in beta]  # Calculate alpha from beta
    d = [data.d for data in data_list]
    q = [data.q for data in data_list]
    filtered_d = [data.filtered_d for data in data_list]
    filtered_q = [data.filtered_q for data in data_list]
    
    # Create subplots
    fig, axs = plt.subplots(8, 1, figsize=(12, 24))
    fig.suptitle('Transform Data Analysis')
    
    # Plot current value
    axs[0].plot(time_ms, curr_val, 'b-')
    axs[0].set_ylabel('Current Value')
    axs[0].grid(True)
    
    # Plot current angle
    axs[1].plot(time_ms, curr_angle, 'r-')
    axs[1].set_ylabel('Current Angle')
    axs[1].grid(True)
    
    # Plot beta
    axs[2].plot(time_ms, beta, 'g-')
    axs[2].set_ylabel('Beta')
    axs[2].grid(True)
    
    # Plot d
    axs[3].plot(time_ms, d, 'm-')
    axs[3].set_ylabel('D')
    axs[3].grid(True)
    
    # Plot q
    axs[4].plot(time_ms, q, 'c-')
    axs[4].set_ylabel('Q')
    axs[4].set_xlabel('Time (ms)')
    axs[4].grid(True)
    
    # Adjust layout to prevent overlap
    plt.tight_layout()
    
    # Save plot
    plot_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'transform_plots.png')
    plt.savefig(plot_path)
    print(f"Plots saved to {plot_path}")
    
    # Show plot
    plt.show()

def main():
    try:
        # Get the absolute path to the log file
        script_dir = os.path.dirname(os.path.abspath(__file__))
        log_file_path = os.path.join(script_dir, 'testdata_charge_20241212_6A.log')
        
        print(f"Reading log file from: {log_file_path}")
        transform_data_list = read_log_file(log_file_path)
        
        if not transform_data_list:
            print("No data was parsed from the file!")
            return
            
        # Print first few entries as example
        print("\nFirst 5 entries:")
        for i, data in enumerate(transform_data_list[:5]):
            print(f"\nEntry {i + 1}:")
            print(f"Timestamp: {data.timestamp}")
            print(f"Time (us): {data.time_us}")
            print(f"Current Value: {data.curr_val:.3f}")
            print(f"Current Angle: {data.curr_angle:.3f}")
            print(f"Beta: {data.beta:.3f}")
            print(f"D: {data.d:.3f}")
            print(f"Q: {data.q:.3f}")
            print(f"Filtered D: {data.filtered_d:.3f}")
            print(f"Filtered Q: {data.filtered_q:.3f}")
            print(f"Mod Index: {data.mod_index:.3f}")
            print(f"Phase Shift: {data.phase_shift:.6f}")
        
        # Save to CSV
        csv_path = os.path.join(script_dir, 'transform_data.csv')
        save_to_csv(transform_data_list, csv_path)
        
        # Create plots
        plot_data(transform_data_list)
            
    except Exception as e:
        print(f"Error in main: {e}")
        print(f"Current working directory: {os.getcwd()}")
        print(f"Script directory: {os.path.dirname(os.path.abspath(__file__))}")
        print(f"Directory contents: {os.listdir(os.path.dirname(os.path.abspath(__file__)))}")
        raise

if __name__ == "__main__":
    main()
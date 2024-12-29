import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def plot_results(filename):
    # Read data
    data = pd.read_csv(filename)
    
    # Create subplots for different measurements
    plt.figure(figsize=(15, 20))
    
    # 1. Current measurements
    plt.subplot(5, 2, 1)
    plt.plot(data['time_us']/1000, data['i_meas'], label='Measured')
    plt.plot(data['time_us']/1000, data['i_alpha'], label='Alpha')
    plt.plot(data['time_us']/1000, data['i_beta'], label='Beta')
    plt.grid(True)
    plt.legend()
    plt.title('Current Measurements')
    plt.xlabel('Time (s)')
    plt.ylabel('Current (A)')

    # 2. DQ Currents
    plt.subplot(5, 2, 2)
    plt.plot(data['time_us']/1000, data['i_raw_d'], label='D-axis Raw')
    plt.plot(data['time_us']/1000, data['i_raw_q'], label='Q-axis Raw')
    plt.plot(data['time_us']/1000, data['i_filtered_d'], label='D-axis Filtered')
    plt.plot(data['time_us']/1000, data['i_filtered_q'], label='Q-axis Filtered')
    plt.grid(True)
    plt.legend()
    plt.title('DQ Currents')
    plt.xlabel('Time (s)')
    plt.ylabel('Current (A)')

    # 3. Grid Voltage
    plt.subplot(5, 2, 3)
    plt.plot(data['time_us']/1000, data['v_grid_meas'], label='Measured')
    plt.plot(data['time_us']/1000, data['v_grid_alpha'], label='Alpha')
    plt.plot(data['time_us']/1000, data['v_grid_beta'], label='Beta')
    plt.grid(True)
    plt.legend()
    plt.title('Grid Voltage')
    plt.xlabel('Time (s)')
    plt.ylabel('Voltage (V)')

    # 4. Grid Voltage DQ
    plt.subplot(5, 2, 4)
    plt.plot(data['time_us']/1000, data['v_grid_d'], label='D-axis')
    plt.plot(data['time_us']/1000, data['v_grid_q'], label='Q-axis')
    plt.grid(True)
    plt.legend()
    plt.title('Grid Voltage DQ')
    plt.xlabel('Time (s)')
    plt.ylabel('Voltage (V)')

    # 5. Control Voltage
    plt.subplot(5, 2, 5)
    plt.plot(data['time_us']/1000, data['v_cntl_alpha'], label='Alpha')
    plt.plot(data['time_us']/1000, data['v_cntl_beta'], label='Beta')
    plt.grid(True)
    plt.legend()
    plt.title('Control Voltage')
    plt.xlabel('Time (s)')
    plt.ylabel('Voltage (V)')

    # 6. Control Voltage DQ
    plt.subplot(5, 2, 6)
    plt.plot(data['time_us']/1000, data['v_cntl_d'], label='D-axis')
    plt.plot(data['time_us']/1000, data['v_cntl_q'], label='Q-axis')
    plt.grid(True)
    plt.legend()
    plt.title('Control Voltage DQ')
    plt.xlabel('Time (s)')
    plt.ylabel('Voltage (V)')

    # 7. Control Feedforward/Feedback
    plt.subplot(5, 2, 7)
    plt.plot(data['time_us']/1000, data['v_cntl_d_ff'], label='D-axis FF')
    plt.plot(data['time_us']/1000, data['v_cntl_d_fd'], label='D-axis FB')
    plt.plot(data['time_us']/1000, data['v_cntl_q_ff'], label='Q-axis FF')
    plt.plot(data['time_us']/1000, data['v_cntl_q_fd'], label='Q-axis FB')
    plt.grid(True)
    plt.legend()
    plt.title('Control FF/FB Components')
    plt.xlabel('Time (s)')
    plt.ylabel('Voltage (V)')

    # 8. Phase Measurements
    plt.subplot(5, 2, 8)
    plt.plot(data['time_us']/1000, data['i_phase_est'], label='Current Phase')
    plt.plot(data['time_us']/1000, data['v_cntl_tgt_phase'], label='Control Target Phase')
    plt.grid(True)
    plt.legend()
    plt.title('Phase Measurements')
    plt.xlabel('Time (s)')
    plt.ylabel('Phase (rad)')

    # 9. Modulation Parameters
    plt.subplot(5, 2, 9)
    plt.plot(data['time_us']/1000, data['v_cntl_mod_index'], label='Modulation Index')
    plt.plot(data['time_us']/1000, data['v_cntl_phase_shift'], label='Phase Shift')
    plt.plot(data['time_us']/1000, data['v_cntl_valid'], label='Valid Flag')
    plt.grid(True)
    plt.legend()
    plt.title('Modulation Parameters')
    plt.xlabel('Time (s)')
    plt.ylabel('Value')

    # 10. Voltage Magnitudes
    plt.subplot(5, 2, 10)
    plt.plot(data['time_us']/1000, data['v_cntl_peak'], label='Control Peak')
    plt.plot(data['time_us']/1000, data['v_dc'], label='DC Voltage')
    plt.grid(True)
    plt.legend()
    plt.title('Voltage Magnitudes')
    plt.xlabel('Time (s)')
    plt.ylabel('Voltage (V)')

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    plot_results("simulation_results.csv")  # Replace with your CSV filename
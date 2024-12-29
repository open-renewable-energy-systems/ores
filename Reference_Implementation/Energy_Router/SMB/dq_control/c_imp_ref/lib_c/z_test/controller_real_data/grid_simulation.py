import numpy as np
import matplotlib.pyplot as plt


import numpy as np

class LowPassFilter1st:
    def __init__(self, fs, fc):
        """
        Initialize the filter with sampling frequency (fs) and cutoff frequency (fc).
        """
        pi = np.pi
        
        # Pre-warp the cutoff frequency
        wc = 2.0 * fs * np.tan(pi * fc / fs)
        
        # Bilinear transform coefficients
        k = 2.0 * fs
        self.b0 = wc / (k + wc)
        self.b1 = self.b0
        self.a1 = (wc - k) / (k + wc)
        
        # Initialize state variables
        self.prev_input = 0.0
        self.prev_output = 0.0

    def apply(self, input_signal):
        """
        Apply the low-pass filter to the input signal.
        """
        # Calculate the output
        output = self.b0 * input_signal + self.b1 * self.prev_input - self.a1 * self.prev_output
        
        # Update state variables
        self.prev_input = input_signal
        self.prev_output = output
        
        return output
    
class BetaTransformation:
    def __init__(self, fs, fc1, fc2):
        """
        Initialize the beta transformation with two cascaded low-pass filters.
        """
        self.lpf1 = LowPassFilter1st(fs, fc1)
        self.lpf2 = LowPassFilter1st(fs, fc2)

    def transform(self, alpha_value):
        """
        Transform the alpha value to a beta value using two cascaded low-pass filters.
        """
        # Pass the alpha value through the first low-pass filter
        intermediate_value = self.lpf1.apply(alpha_value)
        
        # Pass the result through the second low-pass filter
        beta_value = 2.0 * self.lpf2.apply(intermediate_value)
        
        return beta_value
    
class SystemParams:
    def __init__(self):
        self.signal_freq = 50          # Signal frequency in Hz
        self.process_freq = 10000       # Process frequency in Hz (1 ms)
        self.current_update_freq = 10000  # Current update frequency in Hz (10 µs)
        self.Ts_process = 1 / self.process_freq     # Process time step (1 ms)
        self.Ts_current = 1 / self.current_update_freq  # Current time step (10 µs)
        self.omega = 2 * np.pi * self.signal_freq    # Angular frequency in rad/s
        self.Vg_rms = 230             # Grid voltage RMS
        self.R = 0.1                   # Resistance in Ohms
        self.L = 0.005                 # Inductance in Henrys
        self.sim_time = 0.5            # Changed to 1 second simulation time

class PIController:
    def __init__(self, kp, ki):
        self.kp = kp
        self.ki = ki
        self.integral = 0.0

    def update(self, error, Ts):
        self.integral += error * Ts
        return self.kp * error + self.ki * self.integral

class DQController:
    def __init__(self, params):
        self.params = params
        self.pi_d = PIController(kp=80.0, ki=1000.0)  # Tuned PI controller for d-axis
        self.pi_q = PIController(kp=80.0, ki=1000.0)  # Tuned PI controller for q-axis

    def park_transform(self, alpha, beta, theta):
        """
        Converts alpha-beta to d-q frame.
        """
        cos_theta = np.cos(theta)
        sin_theta = np.sin(theta)
        i_d = alpha * cos_theta + beta * sin_theta
        i_q = -alpha * sin_theta + beta * cos_theta
        return i_d, i_q

    def inverse_park_transform(self, v_d, v_q, theta):
        """
        Converts d-q frame to alpha-beta.
        """
        cos_theta = np.cos(theta)
        sin_theta = np.sin(theta)
        v_alpha = v_d * cos_theta - v_q * sin_theta
        v_beta = v_d * sin_theta + v_q * cos_theta
        return v_alpha, v_beta

    def update(self, i_ref_d, i_ref_q, 
               i_d, i_q, 
               v_grid_d, v_grid_q, 
               Ts, theta):
        """
        Updates the controller and calculates desired voltages.
        """
        error_d = i_ref_d - i_d
        error_q = i_ref_q - i_q

        v_d_desired =  self.pi_d.update(error_d, Ts) + v_grid_d
        v_q_desired =  self.pi_q.update(error_q, Ts) + v_grid_q

        v_alpha, v_beta = self.inverse_park_transform(v_d_desired, v_q_desired, theta)

        return v_alpha, v_beta, v_d_desired, v_q_desired

def simulate_system():
    params = SystemParams()
    controller = DQController(params)

    # Total number of current steps
    N_current = int(params.sim_time / params.Ts_current)

    # Initialize arrays
    t = np.arange(0, params.sim_time, params.Ts_current)
    v_grid = np.zeros(N_current)
    i_load = np.zeros(N_current)
    v_inv_alpha = np.zeros(N_current)
    v_inv_beta = np.zeros(N_current)
    v_d_desired = np.zeros(N_current)
    v_q_desired = np.zeros(N_current)
    i_alpha = np.zeros(N_current)
    i_beta = np.zeros(N_current)
    i_d = np.zeros(N_current)
    i_q = np.zeros(N_current)
    v_grid_alpha = np.zeros(N_current)
    v_grid_beta = np.zeros(N_current)
    v_grid_d = np.zeros(N_current)
    v_grid_q = np.zeros(N_current)

    i_ref_d = 20.0  # Desired d-axis current in Amperes
    i_ref_q = 0.0   # Desired q-axis current in Amperes (added reactive component)

    control_step_interval = int(params.current_update_freq / params.process_freq)  # 100 steps
    
    beta_transform = BetaTransformation(params.process_freq, params.signal_freq, params.signal_freq)

    for n in range(N_current-1):
        current_time = n * params.Ts_current
        theta = params.omega * current_time  # Theta at current time step

        # Generate grid voltage (single-phase)
        v_grid[n] = params.Vg_rms * np.sqrt(2) * np.sin(theta)
        v_grid_alpha[n] = v_grid[n]
        v_grid_beta[n]  = -params.Vg_rms * np.sqrt(2) * np.cos(theta)
        v_grid_d[n]     =  params.Vg_rms * np.sqrt(2)
        v_grid_q[n]     =  0.0
        # Convert single-phase current to alpha-beta frame
        i_alpha[n] = i_load[n]
        i_beta[n] = beta_transform.transform(i_alpha[n])

        # Perform Park transformation to get d-q currents
        i_d[n], i_q[n] = controller.park_transform(i_alpha[n], i_beta[n], theta)

        # if n % control_step_interval == 0:
            # Update controller and get desired voltages
        v_inv_alpha[n+1], v_inv_beta[n+1], \
            v_d_desired[n+1], v_q_desired[n+1] = controller.update(
            i_ref_d, i_ref_q,
            i_d[n], i_q[n], 
            v_grid_d[n], v_grid_q[n], 
            params.Ts_process,
            theta
        )
        # else:
        #     v_inv_alpha[n+1] = v_inv_alpha[n]
        #     v_inv_beta[n+1] = v_inv_beta[n]
        #     v_d_desired[n+1] = v_d_desired[n]
        #     v_q_desired[n+1] = v_q_desired[n]

        # System dynamics: Update current based on voltages
            # Calculate derivative
        di_dt1 = (v_inv_alpha[n+1] - v_grid[n] - params.R * i_load[n]) / params.L
        temp  = i_load[n] + di_dt1 * params.Ts_current
        di_dt2 = (v_inv_alpha[n+1] - v_grid[n] - params.R * temp) / params.L
    
        i_load[n+1] = i_load[n] +(di_dt1 + di_dt2) * params.Ts_current/2.0
        # i_load[n+1] = i_load[n] + temp  * params.Ts_current

    return t, v_grid, i_load, v_d_desired, \
           v_q_desired, v_inv_alpha, v_inv_beta, \
           i_alpha, i_beta, i_d, i_q, params

def plot_results(t, v_grid, i_load, 
                 v_d_desired, v_q_desired, 
                 v_inv_alpha, v_inv_beta, 
                 i_alpha, i_beta, 
                 i_d, i_q, params,
                 t_start=None, t_end=None):
    """
    Plot results between t_start and t_end (in seconds).
    If t_start/t_end is None, plot from beginning/to end respectively.
    """
    # Calculate start and end indices
    start_idx = 0 if t_start is None else int(t_start / params.Ts_current)
    end_idx = len(t) if t_end is None else int(t_end / params.Ts_current)
    
    plt.figure(figsize=(15, 25))

    plt.subplot(711)
    plt.plot(t[start_idx:end_idx]*1000, v_grid[start_idx:end_idx], label='Grid Voltage')
    plt.xlabel('Time (ms)')
    plt.ylabel('Voltage (V)')
    plt.ylim([-400, 400])
    plt.legend()
    plt.grid(True)

    plt.subplot(712)
    plt.plot(t[start_idx:end_idx]*1000, i_alpha[start_idx:end_idx], label='I Alpha')
    plt.plot(t[start_idx:end_idx]*1000, i_beta[start_idx:end_idx], label='I Beta')
    plt.xlabel('Time (ms)')
    plt.ylabel('Current (A)')
    plt.legend()
    plt.grid(True)

    plt.subplot(713)
    plt.plot(t[start_idx:end_idx]*1000, i_d[start_idx:end_idx], label='Id')
    plt.plot(t[start_idx:end_idx]*1000, i_q[start_idx:end_idx], label='Iq')
    plt.xlabel('Time (ms)')
    plt.ylabel('Current (A)')
    plt.legend()
    plt.grid(True)

    plt.subplot(714)
    plt.plot(t[start_idx:end_idx]*1000, v_d_desired[start_idx:end_idx], label='Desired Voltage D')
    plt.plot(t[start_idx:end_idx]*1000, v_q_desired[start_idx:end_idx], label='Desired Voltage Q')
    plt.xlabel('Time (ms)')
    plt.ylabel('Voltage (V)')
    plt.ylim([-400, 400])
    plt.legend()
    plt.grid(True)

    plt.subplot(715)
    plt.plot(t[start_idx:end_idx]*1000, v_inv_alpha[start_idx:end_idx], label='Inverter Voltage Alpha')
    plt.plot(t[start_idx:end_idx]*1000, v_inv_beta[start_idx:end_idx], label='Inverter Voltage Beta')
    plt.xlabel('Time (ms)')
    plt.ylabel('Voltage (V)')
    plt.ylim([-400, 400])
    plt.legend()
    plt.grid(True)

    plt.subplot(716)
    plt.plot(t[start_idx:end_idx]*1000, i_load[start_idx:end_idx] * params.R, label='Voltage across R')
    plt.xlabel('Time (ms)')
    plt.ylabel('Voltage (V)')
    plt.legend()
    plt.grid(True)

    plt.subplot(717)
    voltage_L = (np.gradient(i_load, params.Ts_current)) * params.L
    plt.plot(t[start_idx:end_idx]*1000, voltage_L[start_idx:end_idx], label='Voltage across L')
    plt.xlabel('Time (ms)')
    plt.ylabel('Voltage (V)')
    plt.ylim([-50, 50])
    plt.legend()
    plt.grid(True)

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    t, v_grid, i_load, v_d_desired, v_q_desired, \
          v_inv_alpha, v_inv_beta, i_alpha, \
          i_beta, i_d, i_q, params = simulate_system()
    
    # Plot last 200ms
    t_end =params.sim_time
    t_start = 0 #t_end - 0.2
    plot_results(t, v_grid, i_load, v_d_desired, v_q_desired, 
                v_inv_alpha, v_inv_beta, i_alpha, i_beta, 
                i_d, i_q, params, t_start, t_end)
#ifndef GRID_SIMULATION_H
#define GRID_SIMULATION_H

#define _DEFAULT_SOURCE
#include <stdbool.h>
#include <stdint.h>
#include "../../dq_to_modulation/dq_to_modulation.h"
#include "../../beta_transform/beta_transform_1p.h"
#include "../../dq_transform/dq_transform_1phase.h"
#include "../../dq_controller_pid/dq_controller_pid volt_feedback.h"
#include "../../misc/wrap_angle/wrap_angle.h"
#include "plant_simulator.h"

typedef struct {
    float signal_freq;          // Signal frequency in Hz
    float plant_sim_freq;         // Process frequency in Hz
    float control_update_freq;  // Current update frequency in Hz
    float Ts_plant_sim;           // Process time step
    float Ts_control;          // Control time step
    float omega;               // Angular frequency in rad/s
    float Vg_rms;             // Grid voltage RMS
    float I_desired_rms;      // Desired current RMS
    float V_desired_rms;      // Desired voltage RMS
    float R;                   // Resistance in Ohms
    float L;                   // Inductance in Henrys
    float sim_time;           // Simulation time in seconds
    int ratio_cntlFreqReduction; // Ratio of control update frequency to sensing simulation frequency
    float load_R;
    float load_L;
    
} SystemParams;


typedef struct LogData SimulationData;

// Function declarations
void init_system_params(SystemParams* params);
SimulationData* allocate_simulation_data(int length);
void free_simulation_data(SimulationData* data);
void simulate_system(SystemParams* params, SimulationData* data);
void save_results_to_file(const char* filename, SimulationData* data);

#endif /* GRID_SIMULATION_H */
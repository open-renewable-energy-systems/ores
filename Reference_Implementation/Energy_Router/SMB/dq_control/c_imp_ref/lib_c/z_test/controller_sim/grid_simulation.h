#ifndef GRID_SIMULATION_H
#define GRID_SIMULATION_H

#define _DEFAULT_SOURCE
#include <stdbool.h>
#include <stdint.h>
#include "../../dq_to_modulation/dq_to_modulation.h"
#include "beta_transform/beta_transform_1p.h"
#include "dq_transform/dq_transform_1phase.h"
#include "dq_controller_pid/dq_controller_pid.h"
#include "./plant_simulator.h"

typedef struct {
    float signal_freq;          // Signal frequency in Hz
    float plant_sim_freq;         // Process frequency in Hz
    float control_update_freq;  // Current update frequency in Hz
    float Ts_plant_sim;           // Process time step
    float Ts_control;          // Control time step
    float omega;               // Angular frequency in rad/s
    float Vg_rms;             // Grid voltage RMS
    float I_desired_rms;      // Desired current RMS
    float R;                   // Resistance in Ohms
    float L;                   // Inductance in Henrys
    float sim_time;           // Simulation time in seconds
    int ratio_cntlFreqReduction; // Ratio of control update frequency to sensing simulation frequency
} SystemParams;

typedef struct {
    int length;
    float* t;              // Time array
    float* v_grid;         // Grid voltage
    float* i_load;         // Load current
    float* v_inv_alpha;    // Inverter voltage alpha
    float* v_inv_beta;     // Inverter voltage beta
    float* v_d_desired;    // Desired d-axis voltage
    float* v_q_desired;    // Desired q-axis voltage
    float* v_d_actual;     // Actual d-axis voltage
    float* v_q_actual;     // Actual q-axis voltage
    float* i_alpha;        // Alpha current
    float* i_beta;         // Beta current
    float* i_d;           // D-axis current
    float* i_q;           // Q-axis current
    float* i_d_desired;    // Desired d-axis current
    float* i_q_desired;    // Desired q-axis current
    float *i_desired;
    float *i_alpha_desired;
    float *i_beta_desired;

    float* v_grid_alpha;  // Grid voltage alpha
    float* v_grid_beta;   // Grid voltage beta
    float* v_grid_d;      // Grid voltage d-axis
    float* v_grid_q;      // Grid voltage q-axis
    float* mod_index;     // Add these new members
    float* phase_shift;
    int* mod_valid;
    float vdc;  // DC bus voltage
} SimulationData;

// Function declarations
void init_system_params(SystemParams* params);
SimulationData* allocate_simulation_data(int length);
void free_simulation_data(SimulationData* data);
void simulate_system(SystemParams* params, SimulationData* data);
void save_results_to_file(const char* filename, SimulationData* data);

#endif /* GRID_SIMULATION_H */
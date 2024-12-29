#ifndef PLANT_SIMULATOR_H
#define PLANT_SIMULATOR_H

#include <math.h>
#include <stdbool.h>

typedef struct {
    float R;      // Resistance in Ohms
    float L;      // Inductance in Henries
    float Ts;     // Sampling time
    float omega;  // Angular frequency
    float Vg_rms; // Grid voltage RMS
    float Vg_mag; // Grid voltage magnitude
    float Vg_phase; // Grid voltage phase
    float control_update_freq; // Control update frequency
    float plant_sim_freq; // Plant simulation frequency
    float ratio_cntlFreqReduction; // Ratio of sensing frequency to control frequency
} PlantParams;

typedef struct {
    float id;  // d-axis current
    float iq;  // q-axis current
    float i_alpha;  // alpha-axis current
    float i_beta;  // beta-axis current
} PlantState;


// typedef struct {
//     float current;    // Current state
//     float current_q;  // Current q state
//     float current_d;  // Current d state
//     float voltage_q;  // Current q state
//     float voltage_d;  // Current d state
// } PlantState;

void PlantSimulator_Init(PlantState* state, PlantParams* params);
float PlantSimulator_Update(PlantState* state, PlantParams* params, 
                          float v_inverter_d, float v_inverter_q, float v_grid_d, float v_grid_q);

#endif /* PLANT_SIMULATOR_H */
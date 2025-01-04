#ifndef PLANT_SIMULATOR2_H
#define PLANT_SIMULATOR2_H

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
    float load_R;
    float load_L;
    float R_total;
    float L_total;
} PlantParams2;

typedef struct {
    float current;    // Current state
} PlantState2;

void PlantSimulator_Init2(PlantState2* state, PlantParams2* params);
float PlantSimulator_Update2(PlantState2* state, PlantParams2* params, float v_inverter);

#endif /* PLANT_SIMULATOR_H */

#include "plant_simulator.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void PlantSimulator_Init(PlantState* state, PlantParams* params) {
    state->current = 0.0f;
}

float PlantSimulator_Update(PlantState* state, PlantParams* params, float v_inverter) {
    float plant_freq = params->plant_sim_freq;
    float return_freq = params->control_update_freq;

    int N = (int)(plant_freq / return_freq);
    float mod = plant_freq  - (float)(N) * return_freq; 

    if(abs(mod) > 0.0001f) {
        printf("PlantSimulator_Update: plant_req must be a multiple of return_frq\n");
        exit(1);
    }
    float v_load = 0;
    for (int i = 0; i < N; i++) {

        float R_total = params->R + params->load_R;
        float L_total = params->L + params->load_L;

        // Current direction: positive = charging battery
        float di_dt = (-v_inverter - R_total * state->current) / L_total;
        state->current += di_dt * params->Ts;
        
        // Negative signs needed because current direction is reversed
        v_load = -state->current * params->load_R;
        v_load += -state->current * params->load_L * di_dt;
    }
    
    return v_load;
}

#include "plant_simulator.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void PlantSimulator_Init(PlantState* state, PlantParams* params) {
    state->current = 0.0f;
}

float PlantSimulator_Update(PlantState* state, PlantParams* params, 
                          float v_inverter, float v_grid, 
                          bool cos_flag) {
    // RK2 (Heun's method) integration
    float plant_freq = params->plant_sim_freq;
    float return_freq = params->control_update_freq;
    float Vg_mag = params->Vg_mag;
    float Vg_phase = params->Vg_phase;
    float theta = Vg_phase;

    int N = (int)(plant_freq / return_freq);
    float mod = plant_freq  - (float)(N) * return_freq; 

    if(abs(mod) > 0.0001f) {
        printf("PlantSimulator_Update: plant_req must be a multiple of return_frq\n");
        exit(1);
    }
    for (int i = 0; i < N; i++) {
        float v_grid_t = 0;
        if(cos_flag)
        {
            v_grid_t = Vg_mag *cosf(theta + 2*M_PI * plant_freq * params->Ts * i) ;
        }
        else
        {
            v_grid_t = Vg_mag *sinf(theta + 2*M_PI * plant_freq * params->Ts * i) ;
        }

        float di_dt1 = (v_grid_t - v_inverter - params->R * state->current) / params->L;
        // float di_dt1 = (v_inverter - v_grid_t - params->R * state->current) / params->L;

        state->current += di_dt1 * params->Ts;
    }
    return state->current;
}

// float temp = state->current + di_dt1 * params->Ts;
// float di_dt2 = (v_inverter - v_grid_t - params->R * temp) / params->L;

// state->current += (di_dt1 + di_dt2) * params->Ts / 2.0f;
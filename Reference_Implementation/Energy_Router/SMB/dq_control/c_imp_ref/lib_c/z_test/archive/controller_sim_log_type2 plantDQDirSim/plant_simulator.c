#include "plant_simulator.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Update state structure to include d-q currents

void PlantSimulator_Init(PlantState* state, PlantParams* params) {
    state->id = 0.0f;
    state->iq = 0.0f;
}

float PlantSimulator_Update(PlantState* state, PlantParams* params, 
                          float vd_inv, float vq_inv,  // d-q inverter voltages
                          float vd_grid, float vq_grid) {  // d-q grid voltages
    float plant_freq = params->plant_sim_freq;
    float return_freq = params->control_update_freq;
    float omega = 2 * M_PI * params->plant_sim_freq;  // angular frequency

    int N = (int)(plant_freq / return_freq);
    float mod = plant_freq - (float)(N) * return_freq;

    if(fabsf(mod) > 0.0001f) {
        printf("PlantSimulator_Update: plant_freq must be a multiple of return_freq\n");
        exit(1);
    }

    // Simulate using d-q dynamics
    for (int i = 0; i < N; i++) {
        // d-axis current derivative (grid to inverter direction)
        float did_dt = (vd_grid - vd_inv - params->R * state->id + omega * params->L * state->iq) / params->L;
        
        // q-axis current derivative (grid to inverter direction)
        float diq_dt = (vq_grid - vq_inv - params->R * state->iq - omega * params->L * state->id) / params->L;

        // Euler integration
        state->id += did_dt * params->Ts;
        state->iq += diq_dt * params->Ts;
    }

    // Return magnitude of current vector
    return sqrtf(state->id * state->id + state->iq * state->iq);
}
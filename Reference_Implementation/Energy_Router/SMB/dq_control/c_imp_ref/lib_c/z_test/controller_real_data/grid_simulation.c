#include "grid_simulation.h"
#include "./plant_simulator.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "../../dq_to_modulation/dq_to_modulation.h"
#include "../../beta_transform/beta_transform_1p.h"
#include "../../log_data_rw/log_data_rw.h"
#include "../../lowpass_filter_1storder/lowpass_filter_1storder.h"
#include "../../misc/power_2dq_ref/power_2dq_ref.h"

#define SET_D_AXIS_AS_COS 0

void init_system_params(SystemParams* params) {
    params->nominal_signal_freq = 50.0f;
    params->plant_sim_freq = 1000.0f; //
    params->control_update_freq = 1000.0f; // 这是控制理想频率，跟sensing频率一致， 实际按照ratio_sens2control计算
    params->ratio_cntlFreqReduction = 1; // control 频率是 sensing 频率的 1/ratio_cntlFreqReduction
    params->Ts_plant_sim = 1.0f / params->plant_sim_freq;
    params->Ts_control = 1.0f / params->control_update_freq;
    params->omega = 2.0f * M_PI * params->nominal_signal_freq;
    params->Vg_rms = 230.0f;
    params->I_desired_rms =2.6f;
    params->R = 0.1f;
    params->L = 0.005f;
    params->sim_time = 0.4f;
    params->apparent_power = params->Vg_rms * params->I_desired_rms;  // S = V * I
    params->power_factor = -1.0f;  // Unity power factor by default
    params->reactive_power_is_leading = false;  // Default to lagging power factor
    params->vdc = 400.0f;
    params->length = (int)(params->sim_time / params->control_update_freq);
}

SysRunStates* allocate_sys_run_states(int length) {
    SysRunStates* states = (SysRunStates*)malloc(sizeof(SysRunStates));
    if (states == NULL) {
        return NULL;
    }
    
    states->length = length;
    states->states = (SingleState*)calloc(length, sizeof(SingleState));
    if (states->states == NULL) {
        free(states);
        return NULL;
    }
    
    return states;
}

void simulate_system(SystemParams* params, SysRunStates* states) {
    LogData* log_data = load_log_data("../dq_decomp_real_data/log_data.csv");
    if (!log_data) {
        printf("Failed to read log data\n");
        return;
    }

    // Set lengths based on available log data
    int sim_length = log_data->length;
    if (sim_length <= 0) {
        printf("Error: No data in log file\n");
        free(log_data);
        return;
    }

    // Ensure we have enough allocated space
    if (sim_length > states->length) {
        printf("Warning: Truncating simulation to %d points\n", states->length);
        sim_length = states->length;
    }

    params->length = sim_length;
    states->length = sim_length;

    DQController_Params controller_params = {
        .kp_d = 10.0f,        // Match Python kp value
        .ki_d = 200.0f,      // Match Python ki value
        .kp_q = 10.0f,
        .ki_q = 200.0f,
        .omega = params->omega,
        .Ts = params->Ts_control,
        .integral_max =  10.0f,
        .integral_min =  -10.0f,
        .R = params->R,
        .L = params->L
    };

    PlantParams plant_params = {
        .L = params->L,
        .R = params->R,
        .Ts = params->Ts_plant_sim,
        .omega = params->omega,
        .Vg_rms = params->Vg_rms,
        .Vg_mag = params->Vg_rms * sqrtf(2.0f),
        .Vg_phase = 0.0f
    };

    PlantState plant_state = {0};
    BetaTransform_1p beta_transform_1p;
    DQController_State controller_state = {0};

    PlantSimulator_Init(&plant_state, &plant_params);
    BetaTransform_1p_Init(&beta_transform_1p,
                         params->nominal_signal_freq,
                         params->control_update_freq);
    DQController_Init(&controller_state, &controller_params);

    float Vg_peak = params->Vg_rms * sqrtf(2.0f);
    float I_desired_peak = params->I_desired_rms * sqrtf(2.0f);

    LowPassFilter1st lpf_id = {0};
    LowPassFilter1st lpf_iq = {0};
    float cutoff_freq = 10.0f;
    lpf_init(&lpf_id, params->control_update_freq, cutoff_freq);
    lpf_init(&lpf_iq, params->control_update_freq, cutoff_freq);

    dq_voltage_t dq_voltage_last = {0};
    int log_data_index = 0;

    for (int n = 0; n < sim_length - 1; n++) {
        SingleState* state = &states->states[n];
        state->t = n * params->Ts_control;
        float theta = log_data->current_angle[n];
        float theta_dq = 0.0f;
        
        if (SET_D_AXIS_AS_COS) {
            state->v_grid = Vg_peak * cosf(theta);
            state->v_grid_alpha = state->v_grid;
            state->v_grid_beta = Vg_peak * sinf(theta);
            theta_dq = theta;
        } else {
            state->v_grid = Vg_peak * sinf(theta);
            state->v_grid_alpha = state->v_grid;
            state->v_grid_beta = -Vg_peak * cosf(theta);
            theta = theta - M_PI/2.0;
            theta_dq = theta;
        }

        dq_transform_1phase(state->v_grid_alpha, state->v_grid_beta, theta_dq,
                           &state->v_grid_d, &state->v_grid_q);

        float id_ref, iq_ref;
        power_to_dq_current_ref(
            params->apparent_power,
            params->power_factor,
            state->v_grid_d,
            state->v_grid_q,
            &id_ref,
            &iq_ref,
            params->reactive_power_is_leading
        );

        state->i_d_desired = id_ref;
        state->i_q_desired = iq_ref;

        inverse_dq_transform_1phase(
            id_ref,
            iq_ref,
            theta_dq,
            &state->i_alpha_desired,
            &state->i_beta_desired
        );

        if (log_data_index < log_data->length) {
            state->i_alpha = log_data->current_value[log_data_index];
            state->i_beta = BetaTransform_1p_Update(&beta_transform_1p, state->i_alpha);
            log_data_index++;
        } else {
            printf("Warning: Ran out of log data at index %d\n", n);
            break;
        }

        dq_transform_1phase(state->i_alpha, state->i_beta, theta_dq,
                           &state->i_d, &state->i_q);

        state->i_d = lpf_process(&lpf_id, state->i_d);
        state->i_q = lpf_process(&lpf_iq, state->i_q);
        state->current_phase_shift = atan2f(state->i_q, state->i_d);

        printf("v_grid_d: %.6f\n", state->v_grid_d);
        printf("v_grid_q: %.6f\n", state->v_grid_q);
        printf("i_d_desired: %.6f\n", state->i_d_desired);
        printf("i_q_desired: %.6f\n", state->i_q_desired);
        printf("i_d: %.6f\n", state->i_d);
        printf("i_q: %.6f\n", state->i_q);
        printf("current_phase_shift: %.6f\n", state->current_phase_shift);
        printf("Goal d current is: %.6f,  + means discharging, - means charging\n", state->i_d_desired);
        printf("Actual d current is: %.6f,  + means discharging, - means charging\n", state->i_d);

        DQController_UpdateMeasurements(&controller_state, 
                                      state->i_d, state->i_q,
                                      state->v_grid_d, state->v_grid_q);
        DQController_SetReference(&controller_state, state->i_d_desired, state->i_q_desired);
        DQController_Update(&controller_state, &controller_params);

        dq_voltage_t dq_voltage = {
            .vd = DQController_GetVoltageD(&controller_state),
            .vq = DQController_GetVoltageQ(&controller_state),
            .vdc = params->vdc
        };

        // Debug prints for voltages
        printf("\nVoltage Debug at t=%.3f:\n", state->t);
        printf("DQ Voltages from controller:\n");
        printf("  vd: %.3f V\n", dq_voltage.vd);
        printf("  vq: %.3f V\n", dq_voltage.vq);
        printf("  vdc: %.3f V\n", dq_voltage.vdc);

        modulation_result_t mod_result = dq_to_modulation_calculate(dq_voltage);
        dq_voltage.vd = mod_result.vd_adjust;
        dq_voltage.vq = mod_result.vq_adjust;

        printf("Adjusted DQ Voltages after modulation:\n");
        printf("  vd_adjust: %.3f V\n", mod_result.vd_adjust);
        printf("  vq_adjust: %.3f V\n", mod_result.vq_adjust);
        
        // rewrite as needed in real-time to call the inverse dq transform based on interruption of control
        if (n % params->ratio_cntlFreqReduction == 0) {
            inverse_dq_transform_1phase(dq_voltage.vd,
                                      dq_voltage.vq,
                                      theta_dq,
                                      &state->v_inv_alpha,
                                      &state->v_inv_beta);
        } 

        else 

        {
            inverse_dq_transform_1phase(dq_voltage_last.vd,
                                      dq_voltage_last.vq,
                                      theta_dq,
                                      &state->v_inv_alpha,
                                      &state->v_inv_beta);
        }
        
        printf("Alpha-Beta Voltages:\n");
        printf("  v_inv_alpha: %.3f V\n", state->v_inv_alpha);
        printf("  v_inv_beta: %.3f V\n", state->v_inv_beta);
        printf("  v_grid_alpha: %.3f V\n", state->v_grid_alpha);
        printf("  v_grid_beta: %.3f V\n", state->v_grid_beta);
        
        dq_voltage_last = dq_voltage;

        state->mod_index = mod_result.index;
        state->phase_shift = mod_result.phase_shift;
        state->mod_valid = mod_result.valid;
        state->i_load = state->i_alpha;

        if (n < sim_length - 2) {
            states->states[n + 1].t = state->t + params->Ts_control;
        }
    }

    // Clean up
    if (log_data->current_value) free(log_data->current_value);
    if (log_data->current_angle) free(log_data->current_angle);
    if (log_data->time_s) free(log_data->time_s);
    free(log_data);
    
    printf("Simulation completed successfully with %d points\n", sim_length);
}

void free_sys_run_states(SysRunStates* states) {
    free(states);
}

void save_results_to_file(const char* filename, SysRunStates* states) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Error opening file %s\n", filename);
        return;
    }

    // Write header
    fprintf(fp, "t,v_grid,i_load,v_d_desired,v_q_desired,v_inv_alpha,v_inv_beta,"
            "i_alpha,i_beta,i_d,i_q,mod_index,phase_shift,mod_valid,current_phase_shift\n");

    // Write data only up to the actual simulation length
    for (int n = 0; n < states->length; n++) {
        SingleState* state = &states->states[n];
        fprintf(fp, "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%d,%.6f\n",
                state->t,
                state->v_grid,
                state->i_load,
                state->v_d_desired,
                state->v_q_desired,
                state->v_inv_alpha,  // This should now contain the actual voltage values
                state->v_inv_beta,   // This should now contain the actual voltage values
                state->i_alpha,
                state->i_beta,
                state->i_d,
                state->i_q,
                state->mod_index,
                state->phase_shift,
                state->mod_valid,
                state->current_phase_shift);

        // Debug print for verification
        if (n % 50 == 0) {  // Print every 50th sample to avoid too much output
            printf("Sample %d: v_inv_alpha = %.3f V, v_inv_beta = %.3f V\n", 
                   n, state->v_inv_alpha, state->v_inv_beta);
        }
    }

    fclose(fp);
    printf("Results saved to %s with %d points\n", filename, states->length);
}


#include "grid_simulation.h"
#include "./plant_simulator.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "../../dq_to_modulation/dq_to_modulation.h"
#include "../../beta_transform/beta_transform_1p.h"
#include "../../log_data_rw/log_data_rw2.h"
#include "../../lowpass_filter_1storder/lowpass_filter_1storder.h"
#include "../../notch_filter/notch_filter.h"
#include "../../dq_controller_pid/dq_controller_pid volt_feedback.h"

#define SET_D_AXIS_AS_COS 0
#define USE_NOTCH_FILTER 1
#define GRID_OMEGA (2.0f * M_PI * 50.0f)  // 50Hz grid frequency
#define VOLT_FB_MODE 0 // 0: feedback based on PEKA, 1: alpha_volt

void init_system_params2(SystemParams2* params) {
    params->signal_freq = 50.0f;
    params->plant_sim_freq = 100000.0f;
    params->control_update_freq = 1000.0f;
    params->ratio_cntlFreqReduction = 20;
    params->Ts_plant_sim = 1.0f / params->plant_sim_freq;
    params->Ts_control = 1.0f / params->control_update_freq;
    params->omega = 2.0f * M_PI * params->signal_freq;
    params->I_desired_rms = 6.0f;
    params->V_desired_rms = 70.0f;                                                                                                  // MCU调试设置的给定值
    params->R = 0.43f;
    params->L = 0.009f;
    params->sim_time = 2.4f;
    params->load_R = 50.0f;
    params->load_L = 0.00f;
}

// Replace SimulationData2 with LogData struct

SimulationData2* allocate_simulation_data2(int length) {
    SimulationData2* data = init_log_data(length);
    return data;
}

float k = 1.0f;

void simulate_system2(SystemParams2* params, SimulationData2* data) {
    // Add plant simulation steps
    const int PLANT_STEPS_PER_CONTROL = (int)(params->Ts_control / params->Ts_plant_sim);

    // Modify controller parameters - increase proportional gain and reduce integral gain
    DQControllerVoltFeedback_Params v_controller_params = {
        .kp_d = 1.0f,     // Increased from 0.0001                                                                                  // MCU需要传控制参数
        .ki_d = 1.0f / params->ratio_cntlFreqReduction, // Reduced from 0.00005
        .kp_q = 1.0f,     // Increased from 0.0001
        .ki_q = 1.0f / params->ratio_cntlFreqReduction, // Reduced from 0.00005
        .Ts = params->Ts_control,
        .integral_max = 100.0f,    // Reduced from 50.0
        .integral_min = -100.0f    // Reduced from -50.0
    };

    // Initialize components
    PlantParams2 plant_params = {
        .L = params->L,
        .R = params->R,
        .load_L = params->load_L,
        .load_R = params->load_R,
        .Ts = params->Ts_plant_sim,
        .omega = params->omega,
        .plant_sim_freq = params->plant_sim_freq,
        .control_update_freq = params->control_update_freq
    };

    PlantState2 plant_state = {0};
    DQControllerVoltFeedback_State v_controller_state;

    PlantSimulator_Init2(&plant_state, &plant_params);
    DQControllerVoltFeedback_Init(&v_controller_state, &v_controller_params);

    BetaTransform_1p volt_beta_transform_1p;
    BetaTransform_1p curr_beta_transform_1p = {0};

    BetaTransform_1p_Init(&volt_beta_transform_1p,
                          params->signal_freq,
                          params->control_update_freq);

    float V_ref_peak = params->V_desired_rms * sqrtf(2.0f); // Set desired output voltage                                           // MCU调试设置的给定值(直接这里给就可以)
    int LEN = data->length - 1;
    float vd_last = 0.0f;
    float vq_last = 0.0f;

    for (int n = 0; n < LEN; n++) {
        (data + n)->time_us = (uint64_t)(n * params->Ts_control * 1000000.0f);
        float t = (data + n)->time_us / 1000000.0f;
        float theta = params->omega * t;                                                                                            // MCU给相位
        float theta_dq = theta - M_PI_2;

        (data + n)->i_phase_est = theta;
        (data + n)->v_cntl_tgt_phase = theta;

        float v_meas_peak = sqrtf((data + n)->v_grid_alpha * (data + n)->v_grid_alpha + (data + n)->v_grid_beta * (data + n)->v_grid_beta); // MCU：v_meas_peak=电网电压的幅值(V)


        ////////////////////////////控制/////////////////////////////////////
        float vd_meas, vq_meas;
        if(VOLT_FB_MODE == 0) { // 如果电压只能获得峰值                                                                             // MCU当前使用
            vd_meas = v_meas_peak;
            vq_meas = 0.0f;
        } else { // 如果电压能实时测量
            dq_transform_1phase((data + n)->v_grid_alpha, (data + n)->v_grid_beta, theta_dq, &vd_meas, &vq_meas);
        }

        (data + n)->v_grid_d = vd_meas;
        (data + n)->v_grid_q = vq_meas;

        float v_d_ref = V_ref_peak;
        float v_q_ref = 0.0f;

        (data + n)->v_ref_d = v_d_ref;
        (data + n)->v_ref_q = v_q_ref;
        (data + n)->v_ref   = V_ref_peak;

        DQControllerVoltFeedback_SetReference(&v_controller_state, v_d_ref, v_q_ref);
        DQControllerVoltFeedback_UpdateMeasurements(&v_controller_state, vd_meas, vq_meas);
        DQControllerVoltFeedback_Update(&v_controller_state, &v_controller_params);

        // Get voltage outputs
        float v_d = DQControllerVoltFeedback_GetVoltageD(&v_controller_state);
        float v_q = DQControllerVoltFeedback_GetVoltageQ(&v_controller_state);


        dq_voltage_t dq_voltage = {
            .vd = v_d,
            .vq = v_q,
            .vdc = 192.0f // 给直流电压                                                                                             // MCU给电池电压
        };

        ///调制参数跟新：输入为电压dq， 输出为调制系数相位偏移
        modulation_result_t mod_result = dq_to_modulation_calculate(dq_voltage);
        dq_voltage.vd = mod_result.vd_adjust;
        dq_voltage.vq = mod_result.vq_adjust;

        // Add assignments to data structure
        (data + n)->v_dc = dq_voltage.vdc;
        (data + n)->v_cntl_mod_index = mod_result.index;                                                                            // 输出给MCU调制系数
        printf("mod_index: %f\n", (data + n)->v_cntl_mod_index);
        (data + n)->v_cntl_phase_shift = mod_result.phase_shift;                                                                    // 输出给MCU相位差
        /////////////////////电压反馈控制结束////////////////////////////////////////////

        //////////////////////////PWM控制仿真///////////////////////////////////////////
        // Inverse DQ transform
        if (n % params->ratio_cntlFreqReduction == 0)
        {
            inverse_dq_transform_1phase(v_d, v_q, theta_dq,
                                        &(data + n + 1)->v_smb_alpha,
                                        &(data + n + 1)->v_smb_beta);
            vd_last = v_d;
            vq_last = v_q;
        }

        else

        {
            inverse_dq_transform_1phase(vd_last, vq_last, theta_dq,
                                        &(data + n + 1)->v_smb_alpha,
                                        &(data + n + 1)->v_smb_beta);
        }

        // Store controller outputs
        (data + n + 1)->v_smb_d = v_d;                                                                                              // data输出给MCU_Log
        (data + n + 1)->v_smb_q = v_q;

        (data + n)->v_cntl_d = v_d;
        (data + n)->v_cntl_q = v_q;

        // Transform controller outputs back to alpha-beta
        float v_alpha_input, v_beta_input;
        inverse_dq_transform_1phase(v_d, v_q, theta_dq,
                                    &v_alpha_input,
                                    &v_beta_input);

        // Use the transformed voltage as input

        (data + n + 1)->v_grid_alpha = PlantSimulator_Update2(&plant_state,                                                         // MCU_Log不需要算
                                       &plant_params,
                                       v_alpha_input);

        (data + n + 1)->v_grid_beta  = BetaTransform_1p_Update(&volt_beta_transform_1p, (data + n + 1)->v_grid_alpha);              // MCU_Log不需要算

        // Store the plant state
        (data + n + 1)->i_alpha = plant_state.current;                                                                              // MCU给电流记录
        (data + n + 1)->i_beta = BetaTransform_1p_Update(&curr_beta_transform_1p, (data + n + 1)->i_alpha);

        (data + n + 1)->v_smb_alpha = v_alpha_input;
        // Update data storage - if you need to store these values
        (data + n)->v_grid_d = vd_meas;
        (data + n)->v_grid_q = vq_meas;

        // Store phase information
        (data + n)->i_phase_est = theta;
        (data + n)->v_cntl_tgt_phase = theta_dq;

        // Calculate and store alpha-beta components
        float v_cntl_alpha, v_cntl_beta;
        inverse_dq_transform_1phase(v_d, v_q, theta_dq, &v_cntl_alpha, &v_cntl_beta);
        (data + n)->v_cntl_alpha = v_cntl_alpha;
        (data + n)->v_cntl_beta = v_cntl_beta;

        // Calculate and store modulation metrics
        (data + n)->v_cntl_peak = sqrtf(v_cntl_alpha * v_cntl_alpha + v_cntl_beta * v_cntl_beta);
        (data + n)->v_cntl_valid = ((data + n)->v_cntl_peak <= (data + n)->v_dc) ? 1.0f : 0.0f;
        /////////////////////////PWM控制结束//////////////////////////////////////

    }

    printf("Simulation completed.\n");
    save_results_to_file2("simulation_results.csv", data);
}

void free_simulation_data2(SimulationData2* data) {

    cleanup_data(data);
}

// Update save_results_to_file2 function to use new structure
void save_results_to_file2(const char* filename, SimulationData2* data) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Error opening file %s\n", filename);
        return;
    }

    // Write header as a single string without line breaks
    fprintf(fp, "id,time_us,i_meas,i_alpha,i_beta,i_raw_d,i_raw_q,i_notch_d,i_notch_q,"
            "i_filtered_d,i_filtered_q,i_phase_est,v_cntl_tgt_phase,v_grid_meas,"
            "v_grid_alpha,v_grid_beta,v_grid_d,v_grid_q,v_smb_alpha,v_smb_beta,"
            "v_smb_d,v_smb_q,v_cntl_d,v_cntl_q,v_cntl_d_ff,v_cntl_d_fd,"
            "v_cntl_q_ff,v_cntl_q_fd,v_cntl_alpha,v_cntl_beta,v_cntl_mod_index,"
            "v_cntl_phase_shift,v_cntl_valid,v_cntl_peak,v_dc\n");

    // Write data rows
    for (int n = 0; n < data->length; n++) {
        float v_cntl_alpha, v_cntl_beta;
        float phase_est = (data + n)->i_phase_est;
        float target_phase = phase_est - M_PI_2;

        inverse_dq_transform_1phase(
            (data + n)->v_cntl_d,
            (data + n)->v_cntl_q,
            target_phase,
            &v_cntl_alpha,
            &v_cntl_beta
        );

        // Calculate modulation index (magnitude of control voltage vector)
        float mod_index = sqrtf(v_cntl_alpha * v_cntl_alpha + v_cntl_beta * v_cntl_beta);

        // Calculate phase shift (angle of control voltage vector)
        float phase_shift = atan2f((data + n)->v_cntl_q, (data + n)->v_cntl_d);

        // Calculate peak value (maximum absolute value of alpha/beta components)
        float v_cntl_peak = sqrtf(v_cntl_alpha * v_cntl_alpha + v_cntl_beta * v_cntl_beta);

        // Determine control validity (example condition: mod_index <= 1.0)
        int valid = (mod_index <= 1.0f) ? 1 : 0;

        // Write data in the same order as the header
        fprintf(fp, "%d,%lu,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,"
                "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,"
                "%.6f,%.6f,%.6f,%.6f,%d,%.6f,%.6f\n",
                n, (unsigned long)(data + n)->time_us,
                (double)(data + n)->i_alpha, (double)(data + n)->i_alpha, (double)(data + n)->i_beta,
                (double)(data + n)->i_alpha, (double)(data + n)->i_beta,
                (double)(data + n)->i_alpha, (double)(data + n)->i_beta,
                (double)(data + n)->i_alpha, (double)(data + n)->i_beta,
                (double)phase_est,
                (double)target_phase,
                (double)(data + n)->v_grid_alpha,
                (double)(data + n)->v_grid_alpha, (double)(data + n)->v_grid_beta,
                (double)(data + n)->v_grid_d, (double)(data + n)->v_grid_q,
                (double)(data + n)->v_smb_alpha, (double)(data + n)->v_smb_beta,
                (double)(data + n)->v_smb_d, (double)(data + n)->v_smb_q,
                (double)(data + n)->v_cntl_d, (double)(data + n)->v_cntl_q,
                (double)(data + n)->v_cntl_d, 0.0,
                (double)(data + n)->v_cntl_q, 0.0,
                (double)v_cntl_alpha, (double)v_cntl_beta,
                (double)(data + n)->v_cntl_mod_index,
                (double)phase_shift,
                valid,
                (double)v_cntl_peak,
                (double)(data + n)->v_dc);
    }

    fclose(fp);
    printf("Results saved to %s\n", filename);
}


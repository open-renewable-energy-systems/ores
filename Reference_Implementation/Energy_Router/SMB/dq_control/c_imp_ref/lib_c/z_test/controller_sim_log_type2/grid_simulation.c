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

#define SET_D_AXIS_AS_COS 0
#define USE_NOTCH_FILTER 1

void init_system_params(SystemParams* params) {
    params->signal_freq = 50.0f;
    params->plant_sim_freq = 100000.0f; //
    params->control_update_freq = 1000.0f; // 这是控制理想频率，跟sensing频率一致， 实际按照ratio_sens2control计算
    params->ratio_cntlFreqReduction = 20; // control 频率是 sensing 频率的 1/ratio_cntlFreqReduction
    params->Ts_plant_sim = 1.0f / params->plant_sim_freq;
    params->Ts_control = 1.0f / params->control_update_freq;
    params->omega = 2.0f * M_PI * params->signal_freq;
    params->Vg_rms = 77.0f;
    params->I_desired_rms = 6.0f;
    params->R = 0.43f;
    params->L = 0.009f;
    params->sim_time = 1.0f;
    printf("Debug Ts values:\n");
    printf("Ts_plant_sim: %.6f\n", params->Ts_plant_sim);
    printf("Ts_control: %.6f\n", params->Ts_control);
    printf("omega: %.6f\n", params->omega);
}

// Replace SimulationData with LogData struct

SimulationData* allocate_simulation_data(int length) {
    // Use the init_log_data function from log_data_rw2.c
    return init_log_data(length);
}

float k =1.0f;

void simulate_system(SystemParams* params, SimulationData* data) {

    // Scale integral gains for high frequency sampling
    DQController_Params controller_params = {
        .kp_d = 1.0f * k,
        .ki_d = 5.0f * k, // params->ratio_cntlFreqReduction,
        .kp_q = 1.0f * k,
        .ki_q = 5.0f * k,  // params->ratio_cntlFreqReduction,
        .omega = params->omega,
        .Ts = params->Ts_control,
        .integral_max = 100.0f,    // Increased from 300.0
        .integral_min = -100.0f,   // Increased from -300.0
        .R = 0.00f,
        .L = 0.009f
    };

    // Rest of initialization remains the same
    PlantParams plant_params = {
        .L = params->L,
        .R = params->R,
        .Ts = params->Ts_plant_sim,
        .omega = params->omega,
        .Vg_rms = params->Vg_rms,
        .Vg_mag = params->Vg_rms * sqrtf(2.0f),
        .Vg_phase = 0.0f
    };

    // Initialize all components
    PlantState plant_state = {0};
    BetaTransform_1p beta_transform_1p;
    DQController_State controller_state = {0};

    PlantSimulator_Init(&plant_state, &plant_params);
    BetaTransform_1p_Init(&beta_transform_1p,
                         params->signal_freq,
                         params->control_update_freq);
    DQController_Init(&controller_state, &controller_params);

    // Declare reference current variables
    // float i_ref_peak = 6.0f;
    // float i_ref_d = 6.0f;  // Target d-axis current
    // float i_ref_q = 0.0f;   // Target q-axis current for unity power factor

    float Vg_peak = params->Vg_rms * sqrtf(2.0f);
    float I_desired_peak = params->I_desired_rms * sqrtf(2.0f);
    float i_ref_peak = I_desired_peak;
    float i_ref_d    = I_desired_peak;
    float i_ref_q    = 0.0f;

    float current_t = 0.0f;
    float power_factor_target = 1.0;

    // Initialize filters for d and q currents
    LowPassFilter1st lpf_d, lpf_q;
    NotchFilter notch_d, notch_q;
    
    // Initialize low pass filters (e.g., 500Hz cutoff frequency)
    float lpf_cutoff_freq = 50.0f;  // Reduced from 500Hz to provide better filtering
    lpf_init(&lpf_d, params->control_update_freq, lpf_cutoff_freq);
    lpf_init(&lpf_q, params->control_update_freq, lpf_cutoff_freq);
    
    // Initialize notch filters for 50Hz
    notch_filter_init(&notch_d, params->control_update_freq, params->signal_freq, 0.98f);  // Changed from 0.98
    notch_filter_init(&notch_q, params->control_update_freq, params->signal_freq, 0.98f);  // Changed from 0.98

    // Add debug prints for filter coefficients
    // printf("Notch Filter Coefficients:\n");
    // printf("b0: %.6f, b1: %.6f, b2: %.6f\n", notch_d.b0, notch_d.b1, notch_d.b2);
    // printf("a1: %.6f, a2: %.6f\n", notch_d.a1, notch_d.a2);

    // Set reference once before the loop

    // Main simulation loop
    dq_voltage_t dq_voltage_last = {0};
    float theta_dq = 0.0f;



    /***
     * Loop for sensing and filter, 1k-20kHz
     ***/
    for (int n = 0; n < data->length - 1; n++) {
        data->time_us[n] = (uint64_t)(n * params->Ts_control * 1000000.0f);
        data->id[n] = n;
        
        float t = data->time_us[n] / 1000000.0f;
        float theta = params->omega * t;
        
        if (SET_D_AXIS_AS_COS)
        {
            data->v_grid_meas[n]  = Vg_peak * cosf(theta);
            data->v_grid_alpha[n] = data->v_grid_meas[n];    // α = cos(θ)
            data->v_grid_beta[n]  = Vg_peak * sinf(theta);   // β = sin(θ)
            theta_dq = theta;                                 // No rotation

            data->i_ref_alpha[n] = i_ref_peak * cosf(theta);
            data->i_ref_beta[n]  = i_ref_peak * sinf(theta);
        }
        else 
        {
            data->v_grid_meas[n]  = Vg_peak * sinf(theta);
            data->v_grid_alpha[n] = data->v_grid_meas[n];    // α = sin(θ)
            data->v_grid_beta[n]  = -Vg_peak * cosf(theta);  // β = -cos(θ)
            theta_dq = theta - M_PI/2.0;                     // -90° rotation
            data->i_ref_alpha[n] = i_ref_peak * sinf(theta);
            data->i_ref_beta[n]  = -i_ref_peak * cosf(theta);
        }

        data->i_alpha[n] = current_t;
        data->i_beta[n] = BetaTransform_1p_Update(&beta_transform_1p, data->i_alpha[n]);

        // Transform to dq using adjusted angle
        dq_transform_1phase(data->v_grid_alpha[n], data->v_grid_beta[n], theta_dq,
                           &data->v_grid_d[n], &data->v_grid_q[n]);


        dq_transform_1phase(data->i_alpha[n], data->i_beta[n], theta_dq,
                           &data->i_raw_d[n], &data->i_raw_q[n]);
        
        // dq_transform_1phase(data->i_ref_alpha[n], data->i_ref_beta[n], theta_dq,
        //                    &data->i_ref_d[n], &data->i_ref_q[n]);
        data->i_ref_d[n] = i_ref_peak;
        data->i_ref_q[n] = 0.0f;
        
        if (USE_NOTCH_FILTER)
        {
            data->i_notch_d[n] = notch_filter_apply(&notch_d, data->i_raw_d[n]);
            data->i_notch_q[n] = notch_filter_apply(&notch_q, data->i_raw_q[n]);
        }
        else
        {
            data->i_notch_d[n] = data->i_raw_d[n];
            data->i_notch_q[n] = data->i_raw_q[n];
        }
        // data->i_notch_d[n] = data->i_raw_d[n];
        // data->i_notch_q[n] = data->i_raw_q[n];
        
        // Then apply low pass filter
        data->i_filtered_d[n] = lpf_process(&lpf_d, data->i_notch_d[n]);
        data->i_filtered_q[n] = lpf_process(&lpf_q, data->i_notch_q[n]);

        
        printf("=========time: %.6f\n", t);
        printf("v_grid_d: %.6f\n", data->v_grid_d[n]);
        printf("v_grid_q: %.6f\n", data->v_grid_q[n]);
        printf("i_d_raw: %.6f\n", data->i_raw_d[n]);
        printf("i_q_raw: %.6f\n", data->i_raw_q[n]);
        printf("i_d_filtered: %.6f\n", data->i_filtered_d[n]);
        printf("i_q_filtered: %.6f\n", data->i_filtered_q[n]);
        printf("i_ref_d: %.6f\n", data->i_ref_d[n]);
        printf("i_ref_q: %.6f\n", data->i_ref_q[n]);
        // Controller update

        ///控制器的跟新：输入为 电流dq， 输出为电压dq
        ///注意：为了计算法方便， 这里是跟sensing 的频率一致，实际按照控制频率
        DQController_SetReference(&controller_state, data->i_ref_d[n], data->i_ref_q[n]);

        DQController_UpdateMeasurements(&controller_state, 
                                      data->i_filtered_d[n], data->i_filtered_q[n],
                                      data->v_grid_d[n], data->v_grid_q[n]);
        // DQController_SetReference(&controller_state, i_ref_d, i_ref_q);

        DQController_Update(&controller_state, &controller_params);

        // Log feedforward and feedback components
        data->v_cntl_d_ff[n] = DQController_GetVoltageD_FF(&controller_state);
        data->v_cntl_d_fd[n] = DQController_GetVoltageD_FB(&controller_state);
        data->v_cntl_q_ff[n] = DQController_GetVoltageQ_FF(&controller_state);
        data->v_cntl_q_fd[n] = DQController_GetVoltageQ_FB(&controller_state);
        data->v_cntl_d[n] = DQController_GetVoltageD(&controller_state);
        data->v_cntl_q[n] = DQController_GetVoltageQ(&controller_state);

        printf("v_cntl_d_ff, fd, total: %.6f, %.6f, %.6f\n", data->v_cntl_d_ff[n], data->v_cntl_d_fd[n], data->v_cntl_d[n]);
        printf("v_cntl_q_ff, fd, total: %.6f, %.6f, %.6f\n", data->v_cntl_q_ff[n], data->v_cntl_q_fd[n], data->v_cntl_q[n]);



        // Calculate modulation parameters
        dq_voltage_t dq_voltage = {
            .vd = DQController_GetVoltageD(&controller_state),
            .vq = DQController_GetVoltageQ(&controller_state),
            .vdc = 192.0f
        };
       
        ///调制参数跟新：输入为电压dq， 输出为调制系数相位偏移
        modulation_result_t mod_result = dq_to_modulation_calculate(dq_voltage);
       /// 上层控制部分结束， 下面开始仿真 


       /// 调制参数更新
       dq_voltage.vd = mod_result.vd_adjust;
       dq_voltage.vq = mod_result.vq_adjust;
        data->v_cntl_d[n]  = dq_voltage.vd;
        data->v_cntl_q[n]  = dq_voltage.vq;
        /// /////////////////////////////////////////////////////////////
        /// 此处仿真逆变器电压波形生成，实际系统的 由PWM + 逆变器的物理反应为这部分
        if (n%params->ratio_cntlFreqReduction==0)
        {
          inverse_dq_transform_1phase(dq_voltage.vd,
                                      dq_voltage.vq,
                                      theta_dq,
                                      &data->v_cntl_alpha[n],
                                      &data->v_cntl_beta[n]);

         data->v_smb_alpha[n] = data->v_cntl_alpha[n];
         data->v_smb_beta[n] = data->v_cntl_beta[n];
        }
        else
        {
            inverse_dq_transform_1phase(dq_voltage_last.vd,
                dq_voltage_last.vq,
                theta_dq,
                &data->v_cntl_alpha[n],
                &data->v_cntl_beta[n]);
            data->v_smb_alpha[n] = data->v_cntl_alpha[n];
            data->v_smb_beta[n] = data->v_cntl_beta[n];
        }
        
        dq_voltage_last = dq_voltage;
       ///////////////////////////////////////////////////////////////////////


        ///此处仿真逆变器的电流波形生成，内部采用高频仿真，输出为低频
        // Store modulation results
        data->v_cntl_mod_index[n] = mod_result.index;
        data->v_cntl_phase_shift[n] = mod_result.phase_shift;
        data->v_cntl_valid[n] = mod_result.valid;

        plant_params.Vg_phase = theta;
        plant_params.control_update_freq = params->control_update_freq;
        plant_params.plant_sim_freq = params->plant_sim_freq;
        current_t = PlantSimulator_Update(&plant_state, &plant_params,
                                                   data->v_smb_alpha[n],
                                                   data->v_grid_alpha[n], 
                                                   SET_D_AXIS_AS_COS);

        data->i_alpha[n+1] = current_t;
        data->i_meas[n+1] = current_t;
        ///////////////////// 模拟结束/////////////////////////////////////

        // Add these lines before the fprintf section
        data->v_cntl_peak[n] = sqrtf(dq_voltage.vd * dq_voltage.vd + dq_voltage.vq * dq_voltage.vq);
        data->v_dc[n] = dq_voltage.vdc;  // This is 400.0f as defined earlier
        data->v_cntl_tgt_phase[n] = atan2f(dq_voltage.vq, dq_voltage.vd) + theta;
        // Current phase estimation (phase from dq + reference frame angle)
        data->i_phase_est[n] = atan2f(data->i_raw_q[n], data->i_raw_d[n]) + theta;

        // SMB measurements (equal to control values)
        data->v_smb_meas[n] = data->v_cntl_alpha[n];
        data->v_smb_d[n] = data->v_cntl_d[n];
        data->v_smb_q[n] = data->v_cntl_q[n];
        data->v_smb_phase[n] = data->v_cntl_tgt_phase[n];

        // Add after controller update
        if (n % 10 == 0) {  // Print every 1000 samples to avoid flooding
            printf("Debug - Control outputs:\n");
            printf("v_d: %.2f, v_q: %.2f\n", 
                   DQController_GetVoltageD(&controller_state),
                   DQController_GetVoltageQ(&controller_state));
            printf("Current error d: %.2f, q: %.2f\n",
                   data->i_raw_d[n] - data->i_filtered_d[n],
                   data->i_raw_q[n] - data->i_filtered_q[n]);
        }

        // Update time for next iteration (in microseconds)
        data->time_us[n + 1] = data->time_us[n] + (params->Ts_control * 1000000.0f);
        data->id[n + 1] = n + 1;  // Update ID for next iteration

        data->v_cntl_tgt_phase[n] = wrap_angle(data->v_cntl_tgt_phase[n]);
        data->v_grid_phase[n] = wrap_angle(data->v_grid_phase[n]);
        data->i_phase_est[n] = wrap_angle(data->i_phase_est[n]);
    }

    // Handle the last element
    data->time_us[data->length-1] = (uint64_t)((data->length-1) * params->Ts_control * 1000000.0f);
    data->id[data->length-1] = data->length-1;
}

void free_simulation_data(SimulationData* data) {
    // Use the cleanup_data function from log_data_rw2.c
    cleanup_data(data);
}

// Update save_results_to_file function to use new structure
void save_results_to_file(const char* filename, SimulationData* data) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Error opening file %s\n", filename);
        return;
    }

    // Add i_ref_d and i_ref_q to header
    fprintf(fp, "id,time_us,"
           "i_meas,i_alpha,i_beta,i_raw_d,i_raw_q,i_notch_d,i_notch_q,i_filtered_d,i_filtered_q,i_phase_est,"
           "i_ref_d,i_ref_q,"  // Add these fields
           "v_grid_meas,v_grid_alpha,v_grid_beta,v_grid_d,v_grid_q,v_grid_phase,"
           "v_smb_meas,v_smb_alpha,v_smb_beta,v_smb_d,v_smb_q,v_smb_phase,"
           "v_cntl_d_ff,v_cntl_d_fd,v_cntl_q_ff,v_cntl_q_fd,"
           "v_cntl_d,v_cntl_q,v_cntl_peak,v_dc,"
           "v_cntl_mod_index,v_cntl_phase_shift,v_cntl_tgt_phase,v_cntl_valid,"
           "v_cntl_alpha,v_cntl_beta\n");

    // Write data rows with i_ref_d and i_ref_q
    for (int n = 0; n < data->length; n++) {
        fprintf(fp, "%lu,%lu,"  // id and time_us
               "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,"  // current measurements
               "%.6f,%.6f,"  // Add i_ref_d and i_ref_q
               "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,"  // grid voltage
               "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,"  // SMB voltage
               "%.6f,%.6f,%.6f,%.6f,"  // control feedforward/feedback
               "%.6f,%.6f,%.6f,%.6f,"  // control outputs
               "%.6f,%.6f,%.6f,%.6f,"  // modulation parameters
               "%.6f,%.6f\n",  // control alpha/beta
               data->id[n], data->time_us[n],
               data->i_meas[n], data->i_alpha[n], data->i_beta[n],
               data->i_raw_d[n], data->i_raw_q[n],
               data->i_notch_d[n], data->i_notch_q[n],
               data->i_filtered_d[n], data->i_filtered_q[n],
               data->i_phase_est[n],
               data->i_ref_d[n], data->i_ref_q[n],  // Add these values
               data->v_grid_meas[n], data->v_grid_alpha[n], data->v_grid_beta[n],
               data->v_grid_d[n], data->v_grid_q[n], data->v_grid_phase[n],
               data->v_smb_meas[n], data->v_smb_alpha[n], data->v_smb_beta[n],
               data->v_smb_d[n], data->v_smb_q[n], data->v_smb_phase[n],
               data->v_cntl_d_ff[n], data->v_cntl_d_fd[n],
               data->v_cntl_q_ff[n], data->v_cntl_q_fd[n],
               data->v_cntl_d[n], data->v_cntl_q[n],
               data->v_cntl_peak[n], data->v_dc[n],
               data->v_cntl_mod_index[n], data->v_cntl_phase_shift[n],
               data->v_cntl_tgt_phase[n], data->v_cntl_valid[n],
               data->v_cntl_alpha[n], data->v_cntl_beta[n]);
    }

    fclose(fp);
    printf("Results saved to %s\n", filename);
}


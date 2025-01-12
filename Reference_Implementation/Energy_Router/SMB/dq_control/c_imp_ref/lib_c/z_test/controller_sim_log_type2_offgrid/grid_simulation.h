#ifndef GRID_SIMULATION2_H
#define GRID_SIMULATION2_H

#define _DEFAULT_SOURCE
#include <stdbool.h>
#include <stdint.h>
#include "../../dq_to_modulation/dq_to_modulation.h"
#include "../../beta_transform/beta_transform_1p.h"
#include "../../dq_transform/dq_transform_1phase.h"
#include "../../dq_controller_pid/dq_controller_pid volt_feedback.h"
#include "../../misc/wrap_angle/wrap_angle.h"
#include "plant_simulator.h"
#include "../../log_data_rw/log_data_rw2.h"

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

} SystemParams2;



















typedef struct
{
    SystemParams2                               params;
    LogData                                     record_data[2];
    DQControllerVoltFeedback_Params             v_controller_params;
    PlantParams2                                plant_params;
    PlantState2                                 plant_state;
    DQControllerVoltFeedback_State              v_controller_state;
    BetaTransform_1p                            volt_beta_transform_1p;
    BetaTransform_1p                            curr_beta_transform_1p;
    int                                         n;                              // 仿真传递的n；MCU运行时过零传0，没有过零传1即可
    uint64_t                                    time_us;                        // MCU传递的时间戳，单位：μs
    float                                       curr_val;                       // MCU传递的瞬时电流值，单位：A
    float                                       theta;                          // MCU传递的theta，0~2PI
    float                                       vdc;                            // MCU传递的电池直流总压，单位：V
    float                                       V_ref_peak;                     // MCU传递的电网电压峰值，单位：V
    float                                       I_desired_rms;                  // MCU传递的期望反馈控制电流有效值，单位：A
} SMB_Calculate__;


// Function declarations

extern void Init_SMB_Calculate_type2_offgrid(SMB_Calculate__ *p_smb_calculate);
extern modulation_result_t SMB_Cal_Feedback_Fun_type2_offgrid(SMB_Calculate__ *p_smb_calculate, bool en_simulation);

#if !(defined COMPILE_APP_Program) && !(defined COMPILE_BL_Program)
void init_system_params2(SystemParams2* params);
LogData* allocate_simulation_data2(int length);
void free_simulation_data2(LogData* data);
void simulate_system2(SystemParams2* params, LogData* data);
void save_results_to_file2(const char* filename, LogData* data);
#endif

#endif /* GRID_SIMULATION2_H */

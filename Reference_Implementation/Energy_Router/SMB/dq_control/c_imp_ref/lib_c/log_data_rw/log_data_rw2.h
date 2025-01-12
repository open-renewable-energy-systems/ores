#ifndef LOG_DATA_RW2_H
#define LOG_DATA_RW2_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

// Define the structure
typedef struct  {
    int length;
    uint64_t id;
    uint64_t time_us;

    float i_meas;
    float i_alpha;
    float i_beta;
    float i_raw_d;
    float i_raw_q;
    float i_notch_d;
    float i_notch_q;
    float i_filtered_d;
    float i_filtered_q;
    float i_phase_est;

    float i_ref;
    float i_ref_d;
    float i_ref_q;
    float i_ref_alpha;
    float i_ref_beta;

    float v_grid_meas;
    float v_grid_alpha;
    float v_grid_beta;
    float v_grid_d;
    float v_grid_q;
    float v_grid_phase;

    float v_smb_meas;
    float v_smb_alpha;
    float v_smb_beta;
    float v_smb_d;
    float v_smb_q;
    float v_smb_phase;

    float v_cntl_d_ff;
    float v_cntl_d_fd;
    float v_cntl_q_ff;
    float v_cntl_q_fd;

    float v_cntl_d;
    float v_cntl_q;
    float v_cntl_peak;
    float v_dc;
    float v_cntl_mod_index;
    float v_cntl_phase_shift;
    float v_cntl_tgt_phase;
    float v_cntl_valid;
    float v_cntl_alpha;
    float v_cntl_beta;


    float v_ref;
    float v_ref_d;
    float v_ref_q;
    float v_ref_alpha;
    float v_ref_beta;


} LogData;

// Function prototypes
LogData* load_log_data(const char* filename);
LogData* init_log_data(int length);
void cleanup_data(LogData* data);

#endif /* LOG_DATA_RW_H */

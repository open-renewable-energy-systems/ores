#include "log_data_rw2.h"
#include <string.h>

LogData* load_log_data(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return NULL;
    }

    unsigned int data_len = 0;

    // First pass: count number of lines
    data_len = 0;
    char line[4096];  // Assuming max line length of 4096 characters
    while (fgets(line, sizeof(line), file)) {
        if (line[0] != '#')  // Skip comment lines
            data_len++;
    }
    data_len--;  // Subtract header line

    // Allocate memory for the structure
    LogData* data = (LogData*)malloc(sizeof(LogData) * data_len);
    if (!data) {
        fclose(file);
        return NULL;
    }
    memset(data, 0, sizeof(LogData) * data_len);         // 清空
    data->length = data_len;

    // Reset file position to beginning
    rewind(file);

    // Skip comment lines and header
    while (fgets(line, sizeof(line), file)) {
        if (line[0] != '#')
            break;
    }

    // Read data
    for (int i = 0; i < data_len; i++) {
        if (fscanf(file, "%lu,%lu,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                   &(data + i)->id, &(data + i)->time_us,
                   &(data + i)->i_meas, &(data + i)->i_alpha, &(data + i)->i_beta,
                   &(data + i)->i_raw_d, &(data + i)->i_raw_q,
                   &(data + i)->i_notch_d, &(data + i)->i_notch_q,
                   &(data + i)->i_filtered_d, &(data + i)->i_filtered_q, &(data + i)->i_phase_est,
                   &(data + i)->i_ref, &(data + i)->i_ref_d, &(data + i)->i_ref_q,
                   &(data + i)->i_ref_alpha, &(data + i)->i_ref_beta,
                   &(data + i)->v_grid_meas, &(data + i)->v_grid_alpha, &(data + i)->v_grid_beta,
                   &(data + i)->v_grid_d, &(data + i)->v_grid_q, &(data + i)->v_grid_phase,
                   &(data + i)->v_smb_meas, &(data + i)->v_smb_alpha, &(data + i)->v_smb_beta,
                   &(data + i)->v_smb_d, &(data + i)->v_smb_q, &(data + i)->v_smb_phase,
                   &(data + i)->v_cntl_d_ff, &(data + i)->v_cntl_d_fd,
                   &(data + i)->v_cntl_q_ff, &(data + i)->v_cntl_q_fd,
                   &(data + i)->v_cntl_d, &(data + i)->v_cntl_q,
                   &(data + i)->v_cntl_peak, &(data + i)->v_dc,
                   &(data + i)->v_cntl_mod_index, &(data + i)->v_cntl_phase_shift,
                   &(data + i)->v_cntl_tgt_phase, &(data + i)->v_cntl_valid,
                   &(data + i)->v_cntl_alpha, &(data + i)->v_cntl_beta,
                   &(data + i)->v_ref, &(data + i)->v_ref_d, &(data + i)->v_ref_q,
                   &(data + i)->v_ref_alpha, &(data + i)->v_ref_beta) != 48) {

            fprintf(stderr, "Error reading data at line %d\n", i + 1);
            cleanup_data(data);
            fclose(file);
            return NULL;
        }
    }

    fclose(file);
    return data;
}

void cleanup_data(LogData* data) {
    if (data == NULL) return;

    // Free the structure itself
    free(data);
}

LogData* init_log_data(int length) {
    if (length <= 0) {
        fprintf(stderr, "Error: Invalid length specified\n");
        return NULL;
    }

    LogData* data = (LogData*)malloc(sizeof(LogData) * length);
    if (!data) {
        return NULL;
    }

    memset(data, 0, sizeof(LogData) * length);           // 清空
    // Initialize length
    data->length = length;

    return data;
}
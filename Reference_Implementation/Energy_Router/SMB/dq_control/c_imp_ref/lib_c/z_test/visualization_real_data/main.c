#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../../beta_transform/beta_transform_1p.h"
#include "../../dq_transform/dq_transform_1phase.h"
#include "../../lowpass_filter_1storder/lowpass_filter_1storder.h"
#include "../../dq_to_modulation/dq_to_modulation.h"
#include "../../log_data_rw/log_data_rw.h"
#include "../../notch_filter/notch_filter.h"

// Forward declarations
void process_signals(struct LogData* data, struct LogData* data2);
void save_comparison_data(const struct LogData* data, const struct LogData* data2, const char* filename);

void cleanup_data_local(struct LogData* data) {
    if (data) {
        free(data->time_stamp);
        free(data->time_us);
        free(data->curr_val);
        free(data->curr_angle);
        free(data->beta);
        free(data->d);
        free(data->q);
        free(data->filtered_d);
        free(data->filtered_q);
        free(data->notch_d);
        free(data->notch_q);
        free(data->computed_angle);
    }
}

void debug_print_data(const struct LogData* data, const char* label) {
    printf("\nDebug info for %s:\n", label);
    printf("Length: %d\n", data->length);
    printf("Pointers:\n");
    printf("time_stamp: %p\n", (void*)data->time_stamp);
    printf("time_us: %p\n", (void*)data->time_us);
    printf("curr_val: %p\n", (void*)data->curr_val);
    printf("curr_angle: %p\n", (void*)data->curr_angle);
    printf("beta: %p\n", (void*)data->beta);
    printf("d: %p\n", (void*)data->d);
    printf("q: %p\n", (void*)data->q);
    printf("filtered_d: %p\n", (void*)data->filtered_d);
    printf("filtered_q: %p\n", (void*)data->filtered_q);
    printf("notch_d: %p\n", (void*)data->notch_d);
    printf("notch_q: %p\n", (void*)data->notch_q);
    printf("computed_angle: %p\n", (void*)data->computed_angle);
    if (data->curr_val) {
        printf("First curr_val: %f\n", data->curr_val[0]);
    }
}

void process_signals(struct LogData* data, struct LogData* data2) {
    if (!data || !data2) {
        printf("Null data pointer received\n");
        return;
    }
    
    debug_print_data(data, "Original data before processing");
    
    // Initialize data2
    data2->length = data->length;
    data2->time_stamp = calloc(data2->length, sizeof(float));
    data2->time_us = calloc(data2->length, sizeof(float));
    data2->curr_val = calloc(data2->length, sizeof(float));
    data2->curr_angle = calloc(data2->length, sizeof(float));
    data2->beta = calloc(data2->length, sizeof(float));
    data2->d = calloc(data2->length, sizeof(float));
    data2->q = calloc(data2->length, sizeof(float));
    data2->filtered_d = calloc(data2->length, sizeof(float));
    data2->filtered_q = calloc(data2->length, sizeof(float));
    data2->computed_angle = calloc(data2->length, sizeof(float));

    // For data1 (offline computation), allocate notch arrays and copy filtered data to them
    data->notch_d = calloc(data->length, sizeof(float));
    data->notch_q = calloc(data->length, sizeof(float));
    
    // Copy filtered data as notch data (since original data doesn't have notch)
    if (data->filtered_d && data->notch_d) {
        memcpy(data->notch_d, data->filtered_d, data->length * sizeof(float));
    }
    if (data->filtered_q && data->notch_q) {
        memcpy(data->notch_q, data->filtered_q, data->length * sizeof(float));
    }

    // Copy original data to data2
    if (data->time_stamp) memcpy(data2->time_stamp, data->time_stamp, data->length * sizeof(float));
    if (data->time_us) memcpy(data2->time_us, data->time_us, data->length * sizeof(float));
    if (data->curr_val) memcpy(data2->curr_val, data->curr_val, data->length * sizeof(float));
    if (data->curr_angle) memcpy(data2->curr_angle, data->curr_angle, data->length * sizeof(float));
    if (data->beta) memcpy(data2->beta, data->beta, data->length * sizeof(float));
    if (data->d) memcpy(data2->d, data->d, data->length * sizeof(float));
    if (data->q) memcpy(data2->q, data->q, data->length * sizeof(float));
    if (data->filtered_d) memcpy(data2->filtered_d, data->filtered_d, data->length * sizeof(float));
    if (data->filtered_q) memcpy(data2->filtered_q, data->filtered_q, data->length * sizeof(float));

    debug_print_data(data2, "data2 after allocation");

    // Initialize transforms and filters
    BetaTransform_1p beta_transform;
    LowPassFilter1st filter_d, filter_q;
    NotchFilter notch_d, notch_q;
    
    float sample_freq = 1000.0f;
    float target_freq = 50.0f;
    float fc = 10.0f;
    float notch_freq = 50.0f;
    float notch_ratio = 0.95f;
    
    // Initialize components
    BetaTransform_1p_Init(&beta_transform, target_freq, sample_freq);
    lpf_init(&filter_d, sample_freq, fc);
    lpf_init(&filter_q, sample_freq, fc);
    notch_filter_init(&notch_d, sample_freq, notch_freq, notch_ratio);
    notch_filter_init(&notch_q, sample_freq, notch_freq, notch_ratio);
    
    printf("Starting sample processing...\n");
    // Process each sample for offline computation (data)
    for (int i = 0; i < data->length; i++) {
        if (!data->curr_val) {
            printf("curr_val is NULL, skipping processing\n");
            return;
        }
        float alpha = data->curr_val[i];
        
        if (data->beta) data->beta[i] = BetaTransform_1p_Update(&beta_transform, alpha);
        float theta = data->curr_angle ? data->curr_angle[i] : 0.0f;
        float theta_dq = theta - M_PI/2.0;
        
        if (data->d && data->q && data->beta) {
            dq_transform_1phase(alpha, data->beta[i], theta_dq, &data->d[i], &data->q[i]);
        }
        
        if (data->filtered_d && data->d) data->filtered_d[i] = lpf_process(&filter_d, data->d[i]);
        if (data->filtered_q && data->q) data->filtered_q[i] = lpf_process(&filter_q, data->q[i]);
        if (data->notch_d && data->d) data->notch_d[i] = notch_filter_apply(&notch_d, data->d[i]);
        if (data->notch_q && data->q) data->notch_q[i] = notch_filter_apply(&notch_q, data->q[i]);
    }
    
    debug_print_data(data, "Original data after processing");
    debug_print_data(data2, "Data2 after processing");
}

void save_comparison_data(const struct LogData* data, const struct LogData* data2, const char* filename) {
    if (!data || !data2 || !filename) return;
    
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening output file\n");
        return;
    }
    
    fprintf(file, "timestamp,time_us,curr_value,curr_angle,"
           "beta_offline,beta_realtime,"
           "d_offline,d_realtime,"
           "q_offline,q_realtime,"
           "filtered_d_offline,filtered_d_realtime,"
           "filtered_q_offline,filtered_q_realtime,"
           "notch_d,notch_q,computed_angle,"
           "mod_index,phase_shift\n");

    for (int i = 0; i < data->length; i++) {
        fprintf(file, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
                data->time_stamp[i],
                data->time_us[i],
                data->curr_val[i],
                data->curr_angle[i],
                data->beta[i],
                data2->beta[i],
                data->d[i],
                data2->d[i],
                data->q[i],
                data2->q[i],
                data->filtered_d[i],
                data2->filtered_d[i],
                data->filtered_q[i],
                data2->filtered_q[i],
                data->notch_d[i],
                data->notch_q[i],
                data->computed_angle[i],
                data->mod_index[i],
                data->phase_shift[i]);
    }
    
    fclose(file);
}

int main() {
    const char* input_file = "(相位差反向)充电0A.log";
    const char* output_file = "comparison_data.csv";
    
    printf("Loading data from %s...\n", input_file);
    struct LogData* data = load_log_data(input_file);
    if (!data) {
        printf("Error loading data\n");
        return 1;
    }
    
    struct LogData* data2 = calloc(1, sizeof(struct LogData));
    if (!data2) {
        printf("Error allocating data2\n");
        cleanup_data(data);
        return 1;
    }
    
    printf("Loaded %d data points\n", data->length);
    printf("Processing signals...\n");
    process_signals(data, data2);
    
    printf("Saving results to %s...\n", output_file);
    save_comparison_data(data, data2, output_file);
    
    cleanup_data_local(data2);
    free(data2);
    cleanup_data(data);
    
    return 0;
}
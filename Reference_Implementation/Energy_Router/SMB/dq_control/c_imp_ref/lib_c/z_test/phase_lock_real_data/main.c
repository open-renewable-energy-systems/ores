#include <stdio.h>
#include <math.h>
#include "../../phase_lock/pll.h"
#include <errno.h>
#include <unistd.h>  // for getcwd
#include <string.h>  // for strerror
#include <stdlib.h>  // for malloc and realloc

#define TEST_DURATION_SEC 1.5f
#define SAMPLING_FREQ 1000.0f  // 10kHz sampling
#define NOMINAL_FREQ 50.0f      // 50Hz system
#define GRID_VOLTAGE_PEAK 325.0f   // 230Vrms * sqrt(2)

// New struct to store log data
typedef struct {
    float time;
    float current_val;
    float current_angle;
} LogData;

// Function to read log data
int read_log_data(const char* filename, LogData** data, int* num_samples) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening log file: %s\n", strerror(errno));
        return -1;
    }

    // Allocate initial memory
    int capacity = 2000;
    *data = (LogData*)malloc(capacity * sizeof(LogData));
    *num_samples = 0;
    char line[512];
    float time_ms = 0;

    // Debug: Print first few lines of file
    printf("Reading log file...\n");

    while (fgets(line, sizeof(line), fp)) {
        // Look for ID line with timestamp
        if (strstr(line, "ID:") && strstr(line, ",T:")) {
            // Extract time value
            char* time_start = strstr(line, "T:");
            if (time_start && sscanf(time_start, "T: %f", &time_ms) == 1) {
                // Get next line which should contain transform data
                if (fgets(line, sizeof(line), fp)) {
                    float curr_val, curr_angle;
                    // Match exact format from log: spaces and all
                    if (strstr(line, "<st_ufl_transform>curr_val:") &&
                        sscanf(line, "%*[^:]:%f%*[^:]:%f", &curr_val, &curr_angle) == 2) {
                        
                        // Debug first few samples
                        if (*num_samples < 5) {
                            printf("Found sample: time=%f, val=%f, angle=%f\n", 
                                   time_ms, curr_val, curr_angle);
                        }

                        if (*num_samples >= capacity) {
                            capacity *= 2;
                            LogData* new_data = (LogData*)realloc(*data, capacity * sizeof(LogData));
                            if (new_data == NULL) {
                                free(*data);
                                fclose(fp);
                                return -1;
                            }
                            *data = new_data;
                        }
                        
                        (*data)[*num_samples].time = time_ms / 1000.0f; // Convert to seconds
                        (*data)[*num_samples].current_val = curr_val;
                        (*data)[*num_samples].current_angle = curr_angle;
                        (*num_samples)++;
                    }
                }
            }
        }
    }

    printf("Total samples read: %d\n", *num_samples);
    if (*num_samples == 0) {
        printf("No valid data found in log file\n");
        free(*data);
        *data = NULL;
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int test_pll(void) {
    PLL pll;
    int result;
    
    // PLL control parameters
    float kd = 1.0f/3.0f;     // Phase detector gain
    float kp = 2.0f;            // Match Python's gain
    float ki = 10.0f;            // Match Python's gain
    float lpf_cutoff = 5.0f;    // Match Python's VCO LPF
    float freq_max = 55.0f;     
    float freq_min = 45.0f;     
    float k0 = 1.0f;           // Match Python's VCO gain
    float phase_0 = 0.0f;
    
    // Initialize notch filter configuration
    int notch_config = NOTCH_2ND_ORDER | NOTCH_3RD_ORDER;
    float notch_ratios[5] = {
        0.90f, 0.90f, 0.90f, 0.90f, 0.90f
    };
    

    // Read log data
    LogData* log_data;
    int num_log_samples = 0;
    printf("Reading log file: Log_20241123.log\n");
    if (read_log_data("Log_20241123.log", &log_data, &num_log_samples) != 0) {
        printf("Failed to read log data\n");
        return -1;
    }

    const char* filename = "pll_test_results.txt";
    printf("Writing results to: %s\n", filename);
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: Could not open %s for writing: %s\n", filename, strerror(errno));
        free(log_data);
        return -1;
    }
    
    // Write header
    fprintf(fp, "Time(s)\tGrid_V\tPD_Err\tReal_Freq\tEst_Freq\tEst_Phase\tTrue_Phase_Err\tNotch_Filter_Out\tLPF_Out\tControl_Signal\tcorrection_freq\n");
    

    // Initialize PLL with all required parameters
    // phase_0 =  log_data[0].current_angle;

    result = pll_init(&pll, 
                     SAMPLING_FREQ,
                     NOMINAL_FREQ,
                     notch_ratios,
                     notch_config,
                     lpf_cutoff,
                     kd,
                     kp,
                     ki,
                     freq_max,
                     freq_min,
                     k0,
                     phase_0);
                     
    if (result != PLL_SUCCESS) {
        printf("PLL initialization failed with error code: %d\n", result);
        return -1;
    }


    // Main test loop
    float est_phase = 0.0f;
    float true_phase_error = 0.0f, pd_error = 0.0f;
    
    // printf("Estimated phase: %f\n",  pll.output_vco_phase);
    // exit(0);
   int total_samples = 20;
   total_samples = num_log_samples;
    for (int i = 0; i < total_samples; i++) {
        // Use measured values instead of simulated ones
        float signal_t = log_data[i].current_val;
        float gt_phase = log_data[i].current_angle;
        
        // est_phase = pll.output_vco_phase;
        
        // Update PLL
        pll_update(&pll, signal_t);
        // 
        est_phase = pll.output_vco_phase;

        true_phase_error = fmod(est_phase - gt_phase, 2.0f * M_PI);
        if (true_phase_error > M_PI) {
            true_phase_error -= 2.0f * M_PI;
        } else if (true_phase_error < -M_PI) {
            true_phase_error += 2.0f * M_PI;
        }

        true_phase_error /=  M_PI;
        printf("True phase error: %f\n", true_phase_error);
        printf("Grid phase: %f\n", gt_phase);
        printf("Est phase: %f\n", est_phase);

        pd_error = pll.output_phase_detector;

        // Write data to file
        fprintf(fp, "%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\n",
               log_data[i].time,            // Time
               signal_t,                // Grid voltage
               pd_error,                    // Phase detector error
               NOMINAL_FREQ,                // Real frequency
               pll.output_vco_applied_freq, // Estimated frequency
               pll.output_vco_phase,        // Estimated phase
               true_phase_error,            // True phase error
               pll.output_notch_filter,     // Notch filter output
               pll.output_lpf,              // LPF output
               pll.output_pi,               // Control signal
               pll.output_vco_correction_freq);
    }
    
    // Flush and close file
    fflush(fp);
    fclose(fp);
    
    // Cleanup
    free(log_data);
    pll_cleanup(&pll);
    
    printf("Test completed. Processed %d samples.\n", num_log_samples);
    return 0;
}

int main(void) {
    printf("Starting PLL test...\n");
    int result = test_pll();
    printf("PLL test %s\n", result == 0 ? "PASSED" : "FAILED");
    return result;
}
#include <stdio.h>
#include <math.h>
#include "../../phase_lock/pll.h"
#include <errno.h>
#include <unistd.h>  // for getcwd
#include <string.h>  // for strerror

#define TEST_DURATION_SEC 1.5f
#define SAMPLING_FREQ 1000.0f  // 10kHz sampling
#define NOMINAL_FREQ 50.0f      // 50Hz system
#define GRID_VOLTAGE_PEAK 325.0f   // 230Vrms * sqrt(2)

int test_pll(void) {
    PLL pll;
    int result;
    float t = 0.0f;
    float grid_freq = 50.0f;
    float dt = 1.0f / SAMPLING_FREQ;
    int num_samples = (int)(TEST_DURATION_SEC * SAMPLING_FREQ);
    
    // Add initial phase offset
    float initial_phase = 0.0f* M_PI / 6.0f;  // 60 degrees initial phase offset
    
    // PLL control parameters
    float kd = 1.0f/325.0f;     // Phase detector gain
    float kp = 3.0f;            // Match Python's gain
    float ki = 50.0f;            // Match Python's gain
    float lpf_cutoff = 25.0f;   // Match Python's VCO LPF
    float freq_max = 55.0f;     
    float freq_min = 45.0f;     
    float k0 = 1.0f;            // Match Python's VCO gain
    
    // Initialize notch filter configuration
    int notch_config = NOTCH_2ND_ORDER | NOTCH_3RD_ORDER;
    float notch_ratios[5] = {
        0.90f, 0.90f, 0.90f, 0.90f, 0.90f
    };
    
    // Initialize PLL with all required parameters
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
                     0.0);
                     
    if (result != PLL_SUCCESS) {
        printf("PLL initialization failed with error code: %d\n", result);
        return -1;
    }

    const char* filename = "pll_test_results.txt";
    printf("Writing results to: %s\n", filename);
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: Could not open %s for writing: %s\n", filename, strerror(errno));
        pll_cleanup(&pll);
        return -1;
    }
    
    // Write header
    fprintf(fp, "Time(s)\tGrid_V\tPD_Err\tReal_Freq\tEst_Freq\tEst_Phase\tTrue_Phase_Err\tNotch_Filter_Out\tLPF_Out\tControl_Signal\tcorrection_freq\n");
    
    // Main test loop
    float est_phase = 0.0f;
    float true_phase_error = 0.0f, pd_error = 0.0f;

    for (int i = 0; i < num_samples; i++) {
        // Calculate grid signal for current time
        float time_freq = t * grid_freq;
        float grid_phase = 2.0f * M_PI * time_freq + initial_phase;
        float grid_voltage = GRID_VOLTAGE_PEAK * sinf(grid_phase);
        // Update PLL

        pll_update(&pll, grid_voltage);
        est_phase = pll.output_vco_phase;
        true_phase_error = grid_phase - est_phase;
        pd_error = pll.output_phase_detector;

        // Write data to file
        fprintf(fp, "%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\n",
               t,                          // Time
               grid_voltage,               // Grid voltage
               pd_error,                   // Phase detector error
               grid_freq,                  // Real frequency
               pll.output_vco_applied_freq,// Estimated frequency
               pll.output_vco_phase,       // Estimated phase
               true_phase_error,           // True phase error
               pll.output_notch_filter,    // Notch filter output
               pll.output_lpf,             // LPF output
               pll.output_pi,              // Control signal
               pll.output_vco_correction_freq); // VCO correction frequency
        
        t += dt;
    }
    
    // Make sure to flush and close the file
    fflush(fp);
    fclose(fp);
    
    printf("Test completed. Results written to %s\n", filename);
    
    // Cleanup
    pll_cleanup(&pll);
    return 0;
}

int main(void) {
    printf("Starting PLL test...\n");
    int result = test_pll();
    printf("PLL test %s\n", result == 0 ? "PASSED" : "FAILED");
    return result;
}
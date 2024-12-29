#include <stdio.h>
#include <math.h>
#include "../../phase_lock/pll.h"
#include "../../notch_filter/notch_filter.h"
#include <errno.h>
#include <string.h>
#include "../../log_data_rw/log_data_rw.h"

#define SAMPLING_FREQ 1000.0f  // 1kHz sampling frequency
#define NOMINAL_FREQ  50.0f    // 50Hz nominal grid frequency
#define US_TO_S 1e-6f         // Conversion factor from microseconds to seconds

int test_pll(void) {
    PLL pll;
    int result;
    
    // PLL control parameters
    float kd = 1.0f/3.0f;     
    float kp = 2.0f;           
    float ki = 10.0f;          
    float lpf_cutoff = 5.0f;   
    float freq_max = 55.0f;     
    float freq_min = 45.0f;     
    float k0 = 1.0f;           
    float phase_0 = 0.0f;

    // Load test data with correct filename
    struct LogData* log_data = load_log_data("testdata_discharge_20241212_6A.log");
    if (!log_data) {
        printf("Failed to load test data: testdata_discharge_20241212_6A.log\n");
        return -1;
    }

    // Calculate sampling frequency from time_us data
    float actual_ts = (log_data->time_us[1] - log_data->time_us[0]) * US_TO_S;
    float actual_fs = 1.0f / actual_ts;
    printf("Actual sampling frequency: %.2f Hz\n", actual_fs);

    // Initialize PLL with actual sampling frequency
    float notch_ratios[] = {0.1f, 0.1f, 0.1f, 0.1f, 0.1f};  // Q factors for notch filters
    result = pll_init(&pll, 
                      actual_fs,  // Use calculated sampling frequency
                      NOMINAL_FREQ,
                      notch_ratios,
                      NOTCH_2ND | NOTCH_3RD | NOTCH_4TH | NOTCH_5TH | NOTCH_6TH,  // Enable all notch filters
                      lpf_cutoff,
                      kd,
                      kp,
                      ki,
                      freq_max,
                      freq_min,
                      k0,
                      phase_0);

    if (result != PLL_SUCCESS) {
        printf("PLL initialization failed\n");
        cleanup_data(log_data);
        return -1;
    }

    // Open output file for plotting
    FILE* fp = fopen("pll_results.csv", "w");
    if (!fp) {
        printf("Failed to open output file\n");
        cleanup_data(log_data);
        return -1;
    }

    // Write CSV header with additional column for real angle and phase error
    fprintf(fp, "time,input_signal,phase_error,notch_filtered,lpf_filtered,pi_output,vco_freq_correction,vco_freq,vco_phase,real_angle,phase_diff\n");

    // Process each sample
    for (int i = 0; i < log_data->length; i++) {
        // Update PLL with current sample
        pll_update(&pll, log_data->curr_val[i]);

        // Calculate phase difference and normalize to [-π, π]
        float phase_diff = pll.output_vco_phase - log_data->curr_angle[i];
        while (phase_diff > M_PI) phase_diff -= 2.0f * M_PI;
        while (phase_diff < -M_PI) phase_diff += 2.0f * M_PI;

        // Save results for plotting
        fprintf(fp, "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n",
                log_data->time_us[i] * US_TO_S,
                log_data->curr_val[i],
                pll.output_phase_detector,
                pll.output_notch_filter,
                pll.output_lpf,
                pll.output_pi,
                pll.output_vco_correction_freq,
                pll.output_vco_applied_freq,
                pll.output_vco_phase,
                log_data->curr_angle[i],
                phase_diff);
    }
    
    // Cleanup
    fflush(fp);
    fclose(fp);
    cleanup_data(log_data);
    pll_cleanup(&pll);
    
    printf("Test completed. Processed %d samples.\n", log_data->length);
    return 0;
}

int main(void) {
    printf("Starting PLL test...\n");
    int result = test_pll();
    printf("PLL test %s\n", result == 0 ? "PASSED" : "FAILED");
    return result;
}
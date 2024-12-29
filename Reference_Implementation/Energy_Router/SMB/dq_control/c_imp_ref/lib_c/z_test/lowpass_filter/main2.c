#include "../../lowpass_filter_1storder/lowpass_filter_1storder.h"
#include <stdio.h>
#include <stdlib.h>

#define SAMPLE_RATE 1000.0f
#define CUTOFF_FREQ 50.0f
#define FUND_FREQ 50.0f
#define DURATION 0.2f  // 0.2 second of data
#define NUM_SAMPLES (int)(DURATION * SAMPLE_RATE)

int main() {
    // Initialize two identical filters
    LowPassFilter1st filter1, filter2;
    lpf_init(&filter1, SAMPLE_RATE, CUTOFF_FREQ);
    lpf_init(&filter2, SAMPLE_RATE, CUTOFF_FREQ);

    // Open file for writing
    FILE* fp = fopen("lpf_results.txt", "w");
    if (!fp) {
        printf("Error opening file\n");
        return 1;
    }

    fprintf(fp, "Time,Input,SingleFilter,DoubleFilter,Phase90\n");

    // Generate and process signals
    for (int i = 0; i < NUM_SAMPLES; i++) {
        float t = i / SAMPLE_RATE;
        
        // Generate input signal (50 Hz sine wave)
        float input = sinf(2.0f * M_PI * FUND_FREQ * t);
        
        // True 90-degree phase shifted signal
        float phase90 = -cosf(2.0f * M_PI * FUND_FREQ * t);
        
        // Process through cascaded filters
        float output1 = lpf_process(&filter1, input);
        float output2 = 2.0 * lpf_process(&filter2, output1); // 手动加2倍

        // Save to file
        fprintf(fp, "%.3f,%.3f,%.3f,%.3f,%.3f\n", 
                t, input, output1, output2, phase90);
    }

    fclose(fp);
    printf("\nResults saved to lpf_results.txt\n");
    return 0;
}
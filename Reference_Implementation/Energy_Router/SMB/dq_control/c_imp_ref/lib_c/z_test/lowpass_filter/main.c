#include "../../lowpass_filter_1storder/lowpass_filter_1storder.h"
#include <stdio.h>
#include <stdlib.h>

#define SAMPLE_RATE 1000.0f
#define CUTOFF_FREQ 50.0f
#define FUND_FREQ 50.0f
#define DURATION 0.2f  // 1 second of data
#define NUM_SAMPLES (int)(DURATION * SAMPLE_RATE)

int main() {
    // Initialize filter
    LowPassFilter1st filter;
    lpf_init(&filter, SAMPLE_RATE, CUTOFF_FREQ);

    // Open file for writing
    FILE* fp = fopen("lpf_results.txt", "w");
    if (!fp) {
        printf("Error opening file\n");
        return 1;
    }

    fprintf(fp, "Time,Input,Output,Original\n");  // Added Original to CSV header

    // Generate and process signals
    for (int i = 0; i < NUM_SAMPLES; i++) {
        float t = i / SAMPLE_RATE;
        
        // Generate original signal (only fundamental)
        float original = sinf(2.0f * M_PI * FUND_FREQ * t);          // 50 Hz
        
        // Generate input signal with harmonics
        float input = original +          // 50 Hz fundamental
                     0.1f * sinf(2.0f * M_PI * (3 * FUND_FREQ) * t) + // 150 Hz
                     0.0f * sinf(2.0f * M_PI * (5 * FUND_FREQ) * t);  // 250 Hz

        // Process through filter
        float output = lpf_process(&filter, input);

        // Print to terminal
        printf("t=%.3f, input=%.3f, output=%.3f, original=%.3f\n", t, input, output, original);

        // Save to file
        fprintf(fp, "%.3f,%.3f,%.3f,%.3f\n", t, input, output, original);
    }

    fclose(fp);
    printf("\nResults saved to lpf_results.txt\n");
    printf("Use plotting software (e.g., Python with matplotlib) to visualize the results\n");

    return 0;
}
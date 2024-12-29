#include "notch_filter.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FS 1000            // Sampling frequency in Hz
#define DURATION 2.0f      // Duration in seconds
#define N_SAMPLES (FS * DURATION)

void generate_test_signal(float* signal, int length) {
    float dt = 1.0f / FS;
    
    for (int i = 0; i < length; i++) {
        float t = i * dt;
        signal[i] = 1.0f * sinf(2.0f * M_PI * 50.0f * t)    // 50 Hz fundamental
                  + 0.3f * sinf(2.0f * M_PI * 100.0f * t)   // 2nd harmonic (100 Hz)
                  + 0.3f * sinf(2.0f * M_PI * 150.0f * t)   // 3rd harmonic (150 Hz)
                  + 0.2f * sinf(2.0f * M_PI * 250.0f * t);  // 5th harmonic (250 Hz)
    }
}


void generate_test_signal_pure(float* signal, int length) {
    float dt = 1.0f / FS;
    
    for (int i = 0; i < length; i++) {
        float t = i * dt;
        signal[i] = 1.0f * sinf(2.0f * M_PI * 50.0f * t);    // 50 Hz fundamental
    }
}

int main() {
    float* pure_signal  = (float*)malloc(N_SAMPLES * sizeof(float));
    float* input_signal = (float*)malloc(N_SAMPLES * sizeof(float));
    float* temp_signal1 = (float*)malloc(N_SAMPLES * sizeof(float));
    float* temp_signal2 = (float*)malloc(N_SAMPLES * sizeof(float));
    float* output_signal = (float*)malloc(N_SAMPLES * sizeof(float));
    
    // Generate test signal
    generate_test_signal(input_signal, N_SAMPLES);
    generate_test_signal_pure(pure_signal, N_SAMPLES);
    
    // Create notch filters for 100Hz, 150Hz, and 250Hz
    NotchFilter filter_100hz, filter_150hz, filter_250hz;
    float r = 0.95f;  // Notch width parameter
    
    // Initialize filters
    notch_filter_init(&filter_100hz, FS, 100.0f, r);  // 2nd harmonic
    notch_filter_init(&filter_150hz, FS, 150.0f, r);  // 3rd harmonic
    notch_filter_init(&filter_250hz, FS, 250.0f, r);  // 5th harmonic
    
    // Apply filters in series
    notch_filter_apply(&filter_100hz, input_signal, N_SAMPLES, temp_signal1);
    notch_filter_apply(&filter_150hz, temp_signal1, N_SAMPLES, temp_signal2);
    notch_filter_apply(&filter_250hz, temp_signal2, N_SAMPLES, output_signal);
    
    // Save results for plotting
    FILE* fp = fopen("filter_results.txt", "w");
    if (fp) {
        for (int i = 0; i < N_SAMPLES; i++) {
            fprintf(fp, "%f,%f,%f\n", pure_signal[i], input_signal[i], output_signal[i]);
        }
        fclose(fp);
    }
    
    // Clean up
    free(pure_signal);
    free(input_signal);
    free(temp_signal1);
    free(temp_signal2);
    free(output_signal);
    
    return 0;
}
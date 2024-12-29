#include <stdio.h>
#include <math.h>
#include "../../beta_transform/beta_transform_1p.h"

#define PI 3.14159265359f
#define TEST_DURATION_SEC 5.0f    // Changed from 0.1s to 1.0s
#define SAMPLING_FREQ 1000.0f     // 1kHz as specified
#define TARGET_FREQ 50.0f         // 50Hz as specified
#define NOISE_FREQ 150.0f         // 150Hz noise
#define NOISE_AMPLITUDE 0.0f      // 50% of main signal amplitude
#define NUM_SAMPLES (int)(TEST_DURATION_SEC * SAMPLING_FREQ)  // Now 1000 samples instead of 100

int main() {
    // Initialize beta transform
    BetaTransform_1p beta_transform;
    BetaTransform_1p_Init(&beta_transform, TARGET_FREQ, SAMPLING_FREQ);
    
    // Create output file
    FILE* fp = fopen("beta_transform_data.csv", "w");
    fprintf(fp, "Time,Alpha_Clean,Alpha_Noisy,Beta\n");
    
    float dt = 1.0f / SAMPLING_FREQ;
    float t, alpha_clean, alpha_noisy, beta;
    
    printf("Time(s)\tAlpha_Clean\tAlpha_Noisy\tBeta\n");
    
    for (int i = 0; i < NUM_SAMPLES; i++) {
        t = i * dt;
        
        // Generate signals
        alpha_clean = sinf(2.0f * PI * TARGET_FREQ * t);
        alpha_noisy = alpha_clean + 
                     NOISE_AMPLITUDE * sinf(2.0f * PI * NOISE_FREQ * t);
        
        // Process through beta transform
        beta = BetaTransform_1p_Update(&beta_transform, alpha_noisy);
        
        // Print to terminal (every 100th sample to avoid flooding)
        if (i % 100 == 0) {  // Changed from 20 to 100 due to more samples
            printf("%.3f\t%.3f\t%.3f\t%.3f\n", t, alpha_clean, alpha_noisy, beta);
        }
        
        // Save to file
        fprintf(fp, "%.6f,%.6f,%.6f,%.6f\n", t, alpha_clean, alpha_noisy, beta);
    }
    
    fclose(fp);
    printf("\nData saved to beta_transform_data.csv\n");
    
    return 0;
}
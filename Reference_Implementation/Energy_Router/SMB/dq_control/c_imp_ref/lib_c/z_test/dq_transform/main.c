#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dq_transform/dq_transform_1phase.h"

// Constants
#define FREQ_HZ 50
#define SAMPLE_RATE_HZ 1000
#define TEST_DURATION_SEC 2.0f
#define NUM_SAMPLES (int)(TEST_DURATION_SEC * SAMPLE_RATE_HZ)
#define SIGNAL_AMPLITUDE 1.0f

int main() {
    // Test with specific phase offsets
    float phase_offsets[] = {0.0f, M_PI_2, M_PI};
    int num_offsets = sizeof(phase_offsets) / sizeof(phase_offsets[0]);

    // Create build directory if it doesn't exist
    system("mkdir -p build");

    for (int i = 0; i < num_offsets; i++) {
        float phase_offset = phase_offsets[i];
        char filename[100];
        sprintf(filename, "build/dq_results_phase_%.2f.csv", phase_offset);
        
        FILE *fp = fopen(filename, "w");
        if (!fp) {
            printf("Error opening file %s\n", filename);
            return 1;
        }
        
        printf("Generating data for phase offset %.2f rad\n", phase_offset);
        
        // Write CSV header
        fprintf(fp, "t,alpha,beta,theta,d,q\n");
        
        // Initialize DQ transform variables
        float d = 0.0f;
        float q = 0.0f;
        
        // Generate test data and process
        for (int n = 0; n < NUM_SAMPLES; n++) {
            float t = n * (1.0f / SAMPLE_RATE_HZ);
            
            // Input signal frequency
            float omega_t = 2 * M_PI * FREQ_HZ * t;
            
            // Generate alpha-beta signals with phase offset
            // Alpha = cos(ωt + φ)
            // Beta = sin(ωt + φ)
            float alpha = SIGNAL_AMPLITUDE * sinf(omega_t + phase_offset);
            float beta = - SIGNAL_AMPLITUDE * cosf(omega_t + phase_offset);
            
            // Theta is the angle used for the Park transform
            // For synchronous operation, it should match omega_t
            float theta = omega_t;
            
            // Process through Park transform
            dq_transform_1phase(alpha, beta, theta, &d, &q);
            
            // Write results to CSV
            fprintf(fp, "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n", 
                    t, alpha, beta, theta, d, q);
        }
        
        fclose(fp);
        printf("Generated %s\n", filename);
    }
    
    printf("\nAll files generated successfully. Now run: python3 plot_results.py\n");
    return 0;
}
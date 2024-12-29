#include "lookup_table_5d.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Helper function to generate random float in range [min, max]
float random_float(float min, float max) {
    return min + (max - min) * (float)rand() / RAND_MAX;
}

int main() {
    // Initialize random seed
    srand(time(NULL));

    SwitchingAnglesTable* table = load_switching_angles_table("switching_angles_table.bin");
    if (!table) {
        printf("Failed to load switching angles table\n");
        return 1;
    }

    // Generate random test cases
    for(int test = 0; test < 5; test++) {  // Test 5 random cases
        // Random capacitor voltages in [0.95, 1.05]
        float ci1 = random_float(0.95f, 1.05f);
        float ci2 = random_float(0.95f, 1.05f);
        float ci3 = random_float(0.95f, 1.05f);
        float ci4 = random_float(0.95f, 1.05f);
        
        // Random modulation index in [0.75, 1.0]
        float m = random_float(0.75f, 1.0f);

        // Test case results
        SwitchingAnglesResult result_before;
        SwitchingAnglesResult result_after;

        // Run lookups
        lookup_switching_angles_5d(table, ci1, ci2, ci3, ci4, m, &result_before);
        lookup_switching_angles_5d_with_correction(table, ci1, ci2, ci3, ci4, m, &result_after);

        // Print results
        printf("\nTest case %d:\n", test + 1);
        printf("Input parameters:\n");
        printf("ci = [%.3f, %.3f, %.3f, %.3f], m = %.3f\n", ci1, ci2, ci3, ci4, m);
        
        printf("\nBefore correction:\n");
        printf("θ1 = %.6f rad\n", result_before.theta1);
        printf("θ2 = %.6f rad\n", result_before.theta2);
        printf("θ3 = %.6f rad\n", result_before.theta3);
        printf("θ4 = %.6f rad\n", result_before.theta4);
        printf("THD = %.4f%%\n", result_before.thd);
        printf("V1 error = %.4f%%\n", result_before.v1_error);

        printf("\nAfter correction:\n");
        printf("θ1 = %.6f rad\n", result_after.theta1);
        printf("θ2 = %.6f rad\n", result_after.theta2);
        printf("θ3 = %.6f rad\n", result_after.theta3);
        printf("θ4 = %.6f rad\n", result_after.theta4);
        printf("THD = %.4f%%\n", result_after.thd);
        printf("V1 error = %.4f%%\n", result_after.v1_error);

        // Print angle changes
        printf("\nAngle changes after correction:\n");
        printf("Δθ1 = %.6f rad\n", result_after.theta1 - result_before.theta1);
        printf("Δθ2 = %.6f rad\n", result_after.theta2 - result_before.theta2);
        printf("Δθ3 = %.6f rad\n", result_after.theta3 - result_before.theta3);
        printf("Δθ4 = %.6f rad\n", result_after.theta4 - result_before.theta4);
    }

    free_switching_angles_table(table);
    return 0;
}

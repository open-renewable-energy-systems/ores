#include "dyn_stair_wave_table.h"
#include "../interp_table_5d/interp_table_5d.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

void test_stair_wave_interpolation() {
    const int num_modules = 4;
    float modulation_index = 0.92f;
    
    single_dc_source_t dc_sources[4] = {
        {.vdc = 1.0f, .c = 1.0f, .m_common = modulation_index},
        {.vdc = 1.0f,  .c = 1.0f, .m_common = modulation_index},
        {.vdc = 1.0f, .c = 1.0f, .m_common = modulation_index},
        {.vdc = 1.0f,  .c = 1.0f, .m_common = modulation_index}
    };

    SwitchingAnglesTable* angles_table = (SwitchingAnglesTable*)init_switching_angles_lookup_table();
    assert(angles_table != NULL);
    
    StairWaveTable* table = init_stair_wave_table(num_modules);
    assert(table != NULL);

    update_stair_wave_table(table, angles_table, dc_sources, num_modules);

    printf("\nAnalyzing Harmonics using Switching Angles:\n");
    printf("Number of modules: %d\n", num_modules);

    // Get the switching angles from the table (first quadrant)
    float switching_angles[4];
    for (int i = 0; i < num_modules; i++) {
        switching_angles[i] = table->angles[i + 1];  // Skip angle 0
        printf("Switching angle %d: %.3f rad (%.1f deg)\n", 
               i + 1, 
               switching_angles[i], 
               switching_angles[i] * 180.0f / M_PI);
    }

    // Calculate harmonics using the formula: sum(ci * cos(k*theta_i))
    printf("\nHarmonic Components:\n");
    float fundamental = 0.0f;  // Store H1 for percentage calculations
    for (int k = 1; k <= 15; k += 2) {  // Only odd harmonics
        float harmonic = 0.0f;
        
        // Sum the contribution from each switching angle
        for (int i = 0; i < num_modules; i++) {
            harmonic += dc_sources[i].c * cosf(k * switching_angles[i])/k;
        }
        
        if (k == 1) {
            fundamental = harmonic;  // Store fundamental for percentage calculations
            // The fundamental should equal π * modulation_index
            printf("H%d: %.2f V (should be close to %.2f V)\n", 
                   k, harmonic, M_PI * modulation_index);
        } else {
            printf("H%d: %.2f V (%.1f%%)\n", 
                   k, harmonic, fabsf(harmonic/fundamental) * 100.0f);
        }
    }

    free_stair_wave_table(table);
    printf("\nStair wave interpolation test completed.\n");
}

int main() {
    test_stair_wave_interpolation();
    return 0;
}
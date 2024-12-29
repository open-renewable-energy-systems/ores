#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "../../pwm/stair_wave/four_modules/lookup_table_1d/dyn_stair_wave_table.h"
#include "../../pwm/stair_wave/four_modules/load_table_5d/load_switching_angles_table_5d.h"
#include "../../pwm/stair_wave/four_modules/stair_wave_pwm/stair_wave_pwm.h"
// Function implementations
void print_table_contents(const StairWaveTable* table) {
    printf("\nStair Wave Table Contents:\n");
    printf("Index | Angle (rad) | Angle (deg) | State\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < table->num_points; i++) {
        printf("%5d | %10.4f | %10.2f | %5d\n", 
               i, 
               table->angles[i], 
               table->angles[i] * 180.0f / M_PI,
               table->sum_index[i]);
    }
    printf("----------------------------------------\n");
}

void export_angles_to_csv(const StairWaveTable* table, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return;
    }

    fprintf(file, "Index,Angle (rad),Angle (deg),Sum_Index\n");
    for (int i = 0; i < table->num_points; i++) {
        fprintf(file, "%d,%f,%f,%d\n", 
                i, 
                table->angles[i], 
                table->angles[i] * 180.0f / M_PI,
                table->sum_index[i]);
    }

    fclose(file);
}

void print_switching_state_result(float angle, const SwitchingStateResult result) {
    printf("\nSwitching State for angle %.2f rad (%.2f deg):\n", 
           angle, angle * 180.0f / M_PI);
    printf("State: %d\n", result.state);
    printf("Index: %d\n\n", result.index);
}

void print_dc_switching_states(float angle, int state) {
    printf("\nDC Source Switching States at %.2f rad (%.2f deg):\n", 
           angle, angle * 180.0f / M_PI);
    printf("DC Source | State\n");
    printf("----------------\n");
    
    // Convert combined state to individual states
    for (int i = 0; i < 4; i++) {
        int dc_state = (state >= i + 1) ? 1 : 
                      (state <= -(i + 1)) ? -1 : 0;
        printf("   %d     |   %2d\n", i + 1, dc_state);
    }
    printf("----------------\n");
}

void print_multiple_switching_states(const StairWaveTable* table, int num_samples) {
    if (!table) {
        fprintf(stderr, "Null table pointer in print_multiple_switching_states\n");
        return;
    }

    float* test_angles = (float*)malloc(num_samples * sizeof(float));
    if (!test_angles) {
        fprintf(stderr, "Failed to allocate test angles\n");
        return;
    }

    float margin = 0.0001f;
    SwitchingStateResult last_result = {0, 0};
    
    // Generate and sort angles
    for (int i = 0; i < num_samples; i++) {
        test_angles[i] = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
    }
    
    // Sort angles (keep existing sort code)
    for (int i = 0; i < num_samples - 1; i++) {
        for (int j = 0; j < num_samples - i - 1; j++) {
            if (test_angles[j] > test_angles[j + 1]) {
                float temp = test_angles[j];
                test_angles[j] = test_angles[j + 1];
                test_angles[j + 1] = temp;
            }
        }
    }

    printf("\nSwitching States at Random Angles (sorted):\n");
    printf("Angle (rad) | Angle (deg) | State | DC1 | DC2 | DC3 | DC4\n");
    printf("--------------------------------------------------------\n");
    
    for (int i = 0; i < num_samples; i++) {
        get_switching_state(table, test_angles[i], margin, i == 0, &last_result);
        
        int dc_states[4];
        for (int j = 0; j < 4; j++) {
            dc_states[j] = (last_result.state >= j + 1) ? 1 : 
                          (last_result.state <= -(j + 1)) ? -1 : 0;
        }
        
        printf("%10.4f | %10.2f | %5d | %3d | %3d | %3d | %3d\n",
               test_angles[i],
               test_angles[i] * 180.0f / M_PI,
               last_result.state,
               dc_states[0], dc_states[1], dc_states[2], dc_states[3]);
    }
    printf("--------------------------------------------------------\n");
    
    free(test_angles);
}

void compute_harmonics(const float* switching_angles, const single_dc_source_t* dc_sources, 
                      int num_modules, float modulation_index) {
    const int num_harmonics = 5;  // Will compute up to 9th harmonic (odd harmonics)
    float harmonics[num_harmonics];  // Store magnitude of each harmonic
    float desired_v1 = M_PI * modulation_index;  // Should be less than π since 0 ≤ M ≤ 1
    
    // Initialize harmonics array
    for (int h = 0; h < num_harmonics; h++) {
        int k = 2*h + 1;  // harmonic order: 1, 3, 5, 7, 9
        float harmonic_k = 0.0f;

        // Sum up contributions from each DC source
        for (int i = 0; i < num_modules; i++) {
            float ci = dc_sources[i].c;
            float theta_i = switching_angles[i+1];
            
            // Debug print for fundamental component calculation
            if (h == 0) {
                printf("DC%d: ci=%.4f, theta_i=%.4f, cos(k*theta_i)=%.4f, contribution=%.4f\n",
                       i+1, ci, theta_i, cosf(k * theta_i), ci * cosf(k * theta_i));
            }
            
            // Compute k-th harmonic component: ci * cos(k*theta_i)
            harmonic_k += ci * cosf(k * theta_i)/k;
        }

        // Divide by k after summing all contributions
        harmonics[h] = harmonic_k / k;
    }

    // Print results
    printf("\nHarmonic Analysis:\n");
    printf("Desired V1 (π*M): %9.4f\n", desired_v1);
    printf("Actual V1:       %9.4f\n", harmonics[0]);
    printf("V1 Error:        %9.4f%%\n", 100.0f * (harmonics[0] - desired_v1) / desired_v1);
    printf("\nHarmonic Components:\n");
    printf("Order | Magnitude | Normalized (%%)\n");
    printf("--------------------------------\n");
    for (int h = 0; h < num_harmonics; h++) {
        int harmonic_order = 2*h + 1;
        float normalized = 100.0f * harmonics[h] / harmonics[0];  // Normalize to fundamental in percentage
        printf("%5d | %9.4f | %9.2f%%\n", 
               harmonic_order, 
               harmonics[h],
               normalized);
    }
    printf("--------------------------------\n");
}

int main(void) {
    printf("Starting PWM control test...\n");  // Debug print
   
   // 初级输入
    pwm_mini_input_t pwm_mini_input = {
        .vdc1 = 90.0f,
        .vdc2 = 90.0f,
        .vdc3 = 90.0f,
        .vdc4 = 90.0f,
        .io_index1 = 0,
        .io_index2 = 1,
        .io_index3 = 2,
        .io_index4 = 3,
        .phase_shift = 0.0f,
        .modulation_index = 0.8f,
        .sort_strategy = 0,
        .interruption_mode = INTERRUPTION_MODE_TIMMER_REQ,
        .num_modules = 4,
        .pwm_state_phase_safe_margine = 0.0f
    };
    
    printf("PWM mini input initialized\n");  // Debug print

    //中间输入
    single_dc_source_t dc_sources[4] = {
        {
            .vdc = pwm_mini_input.vdc1, 
            .io_index = pwm_mini_input.io_index1, 
            .m_common = pwm_mini_input.modulation_index, 
            .phase_shift = 0.0f  // 0 degrees
        },
        {
            .vdc = pwm_mini_input.vdc2, 
            .io_index = pwm_mini_input.io_index2, 
            .m_common = pwm_mini_input.modulation_index, 
            .phase_shift = 0.0f  // 90 degrees
        },
        {
            .vdc = pwm_mini_input.vdc3, 
            .io_index = pwm_mini_input.io_index3, 
            .m_common = pwm_mini_input.modulation_index, 
            .phase_shift = 0.0f  // 180 degrees
        },
        {
            .vdc = pwm_mini_input.vdc4, 
            .io_index = pwm_mini_input.io_index4, 
            .m_common = pwm_mini_input.modulation_index, 
            .phase_shift = 0.0f  // 270 degrees
        }
    };

    printf("DC sources initialized\n");  // Debug print

    float v_avg = (dc_sources[0].vdc + dc_sources[1].vdc + dc_sources[2].vdc + dc_sources[3].vdc) / pwm_mini_input.num_modules;
    printf("v_avg: %f\n", v_avg);

    dc_sources[0].c = dc_sources[0].vdc/v_avg;
    dc_sources[1].c = dc_sources[1].vdc/v_avg;
    dc_sources[2].c = dc_sources[2].vdc/v_avg;
    dc_sources[3].c = dc_sources[3].vdc/v_avg;

    printf("DC sources normalized. v_avg = %f\n", v_avg);  // Debug print

    // After normalizing voltages, also initialize other necessary fields
    printf("DC sources normalized. v_avg = %f\n", v_avg);
    for (int i = 0; i < pwm_mini_input.num_modules; i++) {
        dc_sources[i].c = dc_sources[i].vdc / v_avg;
        dc_sources[i].m_self = 0.0f;
        dc_sources[i].vdc_ref = v_avg;
        dc_sources[i].soc = 1.0f;
        dc_sources[i].pwm_state = STATE_ALL_OFF;
        dc_sources[i].valid = true;
    }

    pwm_mini_input.sort_strategy = optimize_sort_strategy_based_on_interruption_mode(pwm_mini_input.interruption_mode);

    // Initialize PWM control
    PWMControlState pwm_state = {
        .num_modules = pwm_mini_input.num_modules,
        .current_angle = 0.0f,  // Start from 0
        .update_table = true,
        .reset_dc_sources = false,
        .dc_sources_sort_mode = pwm_mini_input.sort_strategy,
        .is_initialized = true,
        .vdc_sort_mode = true,
        .update_1d_table = true,
        .phase_shift = 0.0f,  // Base phase shift
        .dc_sources = dc_sources,
        .table = NULL,
        .angles_table = NULL
    };


    printf("Initializing PWM control...\n");  // Debug print
    bool init_success = init_pwm_control(&pwm_state);
    if (!init_success) {
        printf("Failed to initialize PWM control!\n");
        return 1;
    }
    printf("PWM control initialized successfully\n");  // Debug print


    printf("Updating PWM control...\n");  // Debug print
    bool update_success = update_pwm_control(&pwm_state, dc_sources);


    if (!update_success) {
        printf("Failed to update PWM control!\n");
        cleanup_pwm_control(&pwm_state);
        return 1;
    }
    printf("PWM control updated successfully\n");  // Debug print

    printf("\nPrinting switching states:\n");  // Debug print
    print_multiple_switching_states(pwm_state.table, 10);

    printf("\nComputing harmonics:\n");  // Debug print
    compute_harmonics(pwm_state.table->angles, dc_sources, pwm_mini_input.num_modules, pwm_mini_input.modulation_index);

    printf("\nExporting angles to CSV...\n");  // Debug print
    export_angles_to_csv(pwm_state.table, "stair_wave_angles.csv");
    
    printf("\nCleaning up...\n");  // Debug print
    cleanup_pwm_control(&pwm_state);

    printf("Test completed successfully!\n");  // Debug print
    return 0;
}

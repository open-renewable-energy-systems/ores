#include "dq_to_modulation.h"
#include <math.h>

modulation_result_t dq_to_modulation_calculate(dq_voltage_t dq_voltage) {
    modulation_result_t result = {0};    
    // Calculate magnitude of the voltage vector
    float v_magnitude = sqrtf(dq_voltage.vd * dq_voltage.vd + dq_voltage.vq * dq_voltage.vq);

    if (dq_voltage.vdc < 1.0f) {
        dq_voltage.vdc = 1.0f;
    }
    
    // Calculate initial modulation index
    float mod_index = v_magnitude / dq_voltage.vdc;
    
    // Initialize adjusted values
    result.vd_adjust = dq_voltage.vd;
    result.vq_adjust = dq_voltage.vq;
    
    // Limit the modulation index and scale the voltage vector if necessary
    if (mod_index > MAX_MOD_INDEX) {
        float scale_factor = MAX_MOD_INDEX / mod_index;
        result.vd_adjust = dq_voltage.vd * scale_factor;
        result.vq_adjust = dq_voltage.vq * scale_factor;
        result.index = MAX_MOD_INDEX;
    } else {
        result.index = mod_index;
    }

    result.phase_shift = atan2f(result.vq_adjust, result.vd_adjust);
    while (result.phase_shift > M_PI) {
        result.phase_shift -= 2.0f * M_PI;
    }
    while (result.phase_shift < -M_PI) {
        result.phase_shift += 2.0f * M_PI;
    }

    if (result.phase_shift > M_PI) {
        result.phase_shift -= 2.0f * M_PI;
    }
    if (result.phase_shift < -M_PI) {
        result.phase_shift += 2.0f * M_PI;
    }
        
    // Check if modulation index is within valid range
    result.valid = (result.index >= 0.0f && result.index <= MAX_MOD_INDEX);
    
    return result;
}
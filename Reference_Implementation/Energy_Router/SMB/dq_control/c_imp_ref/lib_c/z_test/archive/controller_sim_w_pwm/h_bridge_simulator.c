#include "h_bridge_simulator.h"

float get_module_output_voltage(uint32_t bridge_state, float vdc) {
    // Check high-side switches (bits 7-6: S1H S1L)
    uint8_t high_side_state = (bridge_state & 0b11000000) >> 6;
    
    switch (high_side_state) {
        case 0b10:  // S1H on, S1L off
            return vdc;     // Positive voltage
        case 0b01:  // S1H off, S1L on
            return -vdc;    // Negative voltage
        case 0b11:  // Both on (high-side bypass)
            return 0.0f;    // Current can flow but no voltage contribution
        case 0b00:  // Both off (all switches off)
            return 0.0f;    // Simplified: assume no current
        default:
            return 0.0f;
    }
}

float simulate_mmc_output(single_dc_source_t * dc_sources, int num_of_modules) {
    float output_voltage = 0.0f;
    
    for (int i = 0; i < num_of_modules; i++) {
        if (!dc_sources[i].valid) {
            continue;
        }
        
        uint32_t bridge_state = convert_HBridgeState_to_binary(dc_sources[i].pwm_state);
        output_voltage += get_module_output_voltage(bridge_state, dc_sources[i].vdc);
    }
    
    return output_voltage;
}

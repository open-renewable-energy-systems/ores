#ifndef H_BRIDGE_SIMULATOR_H
#define H_BRIDGE_SIMULATOR_H

#include "../../pwm/stair_wave/common/modulation_2pwm/modulation_2pwm.h"
#include <stdint.h>

// Simulates the MMC output voltage based on H-bridge states and DC voltages
float simulate_mmc_output(single_dc_source_t * dc_sources, int num_of_modules);

// Helper function to get individual module contribution
float get_module_output_voltage(uint32_t bridge_state, float vdc);

#endif // H_BRIDGE_SIMULATOR_H

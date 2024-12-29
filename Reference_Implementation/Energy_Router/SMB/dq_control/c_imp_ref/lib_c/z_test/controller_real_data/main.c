#include "grid_simulation.h"
#include <stdio.h>

int main() {
    SystemParams params;
    init_system_params(&params);
    
    int n_control_steps = (int)(params.sim_time / params.Ts_control);
    SysRunStates* states = allocate_sys_run_states(n_control_steps);
    
    // Run simulation
    simulate_system(&params, states);
    
    // Save results
    save_results_to_file("simulation_results.csv", states);
    
    // Cleanup
    free_sys_run_states(states);
    
    return 0;
}
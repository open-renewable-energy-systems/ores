#include "grid_simulation.h"
#include <stdio.h>

int main() {
    SystemParams2 params;
    init_system_params2(&params);
    
    int n_control_steps = (int)(params.sim_time / params.Ts_control);
    SimulationData* sim_data = allocate_simulation_data2(n_control_steps);
    
    // Run simulation
    simulate_system2(&params, sim_data);
    
    // Save results
    save_results_to_file2("simulation_results.csv", sim_data);
    
    // Cleanup
    free_simulation_data2(sim_data);
    
    return 0;
}
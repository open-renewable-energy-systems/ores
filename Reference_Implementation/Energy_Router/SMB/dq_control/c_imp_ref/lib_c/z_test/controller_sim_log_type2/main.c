#include "grid_simulation.h"
#include <stdio.h>

int main() {
    SystemParams params;
    init_system_params(&params);
    
    int n_control_steps = (int)(params.sim_time / params.Ts_control);
    SimulationData* sim_data = allocate_simulation_data(n_control_steps);
    
    // Run simulation
    simulate_system(&params, sim_data);
    
    // Save results
    save_results_to_file("simulation_results.csv", sim_data);
    
    // Cleanup
    free_simulation_data(sim_data);
    
    return 0;
}
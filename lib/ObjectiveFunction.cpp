
#include "Cell.h"
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>

double objective_function(int time_steps, int initial_cells, int n_simulations, double p_div, double func_decline, double p_diff, double p_die, std::vector<int> observed_counts) {
    //const int TIME_STEPS = 100;
    //const int INITIAL_CELLS = 10;
    //const int N_SIMULATIONS = 10;

    std::vector<int> final_cell_counts(n_simulations);
    
    for(int i = 0; i < n_simulations; ++i) {
        auto cell_counts = simulate(time_steps, initial_cells, p_die, p_diff, p_div, func_decline);
        int final_cell_count = cell_counts.back();
        final_cell_counts[i] = final_cell_count;
    }

    std::sort(final_cell_counts.begin(), final_cell_counts.end(), std::greater<int>());

    
    //std::cout << "simulated:" <<final_cell_counts[0] << std::endl;
    //std::cout << "observed:" << observed_counts[0] << std::endl;
    double total_sum_of_squares = 0;
    for(int i = 0; i < n_simulations; ++i) {
        double log_simulated_count = std::log(final_cell_counts[i]+1);
        double log_observed_count = std::log(observed_counts[i]+1);  // avoid log 0 by adding pseudo count 1
        double difference = log_simulated_count - log_observed_count;
        total_sum_of_squares += std::abs(difference);
    }

    double fitness =  total_sum_of_squares / n_simulations;
    return fitness;
}
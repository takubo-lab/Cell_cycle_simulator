// Cell.h
#ifndef CELL_H
#define CELL_H

#include <vector>
#include <string>

struct Cell {
    bool differentiated = false;
    int time_since_differentiation = 0; 
};

std::vector<int> simulate(int time_steps, int initial_cells, double p_die, double p_diff, double p_div, double func_decline);
double objective_function(int time_steps, int initial_cells, int n_simulations, double p_div, double func_decline, double p_diff, double p_die, std::vector<int> observed_counts);
std::vector<int> read_observed_counts_from_csv(const std::string& filename);

#endif
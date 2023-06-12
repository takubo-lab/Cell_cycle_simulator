// Cell.h
#ifndef CELL_H
#define CELL_H

#include <vector>

struct Cell {
    bool differentiated = false;
    int time_since_differentiation = 0; 
};

std::vector<int> simulate(int time_steps, int initial_cells, double p_die, double p_diff, double p_div, double func_decline);

#endif
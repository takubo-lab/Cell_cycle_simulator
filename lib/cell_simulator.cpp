#include "Cell.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <map>



int main(int argc, char* argv[]) {
        if(argc < 8) {
        std::cerr << "Usage: " << argv[0] << " DIVIDE_PROB DEATH_PROB" << std::endl;
        return 1;
    }

    const double DIVIDE_PROB = std::atof(argv[1]);
    const double DEATH_PROB = std::atof(argv[2]);
    const double DIFF_PROB = std::atof(argv[3]);
    const double FUNC_DECLINE = std::atof(argv[4]);
    const int TIME_STEPS = std::atof(argv[5]);
    const int INITIAL_CELLS = std::atof(argv[6]);
    const int N_SIMULATIONS = std::atof(argv[7]);



    int n_simulations = N_SIMULATIONS;
    // std::map<int, int> histogram;

    for(int i = 0; i < n_simulations; ++i) {
        auto cell_counts = simulate(TIME_STEPS, INITIAL_CELLS, DEATH_PROB, DIFF_PROB, DIVIDE_PROB, FUNC_DECLINE);
        int final_cell_count = cell_counts.back();

        std::cout << final_cell_count << " " << i << "\n";
       // histogram[final_cell_count]++;
    }

    //for(const auto& p : histogram) {
    //    std::cout << p.first << " " << p.second << "\n";
    //}

    return 0;
}

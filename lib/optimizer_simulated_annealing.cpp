#include "Cell.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>



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

    std::sort(final_cell_counts.begin(), final_cell_counts.end(), std::greater<int>());
    //std::cout << "simulated:" <<final_cell_counts[0] << std::endl;
    //std::cout << "observed:" << observed_counts[0] << std::endl;
    double total_sum_of_squares = 0;
    for(int i = 0; i < n_simulations; ++i) {
        double log_simulated_count = log(final_cell_counts[i]+1);
        double log_observed_count = log(observed_counts[i]+1);  // avoid log 0 by adding pseudo count 1
        double difference = log_simulated_count - log_observed_count;
        total_sum_of_squares += abs(difference);
    }

    double fitness = - total_sum_of_squares / n_simulations;
    return fitness;
}

double acceptance_probability(double energy, double new_energy, double temperature) {
    if(new_energy < energy) {
        return 1.0;
    }
    return exp((energy - new_energy) / temperature);
}

std::tuple<double, double, double, double> simulated_annealing(int time_steps, int initial_cells, int n_simulations, double p_div, double func_decline, double p_diff, double p_die, std::vector<int> observed_counts) {
    const double INITIAL_TEMPERATURE = 100.0;
    const double FINAL_TEMPERATURE = 0.1;
    const double COOLING_RATE = 0.99;
    const int MAX_ITERATIONS = 1000;

    double temperature = INITIAL_TEMPERATURE;
    double energy = objective_function(time_steps, initial_cells, n_simulations, p_div, func_decline, p_diff, p_die,observed_counts);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);


    for(int i = 0; i < MAX_ITERATIONS; ++i) {
        double new_p_div = p_div + (dis(gen) * 2 - 1) * 0.1;
        double new_func_decline = func_decline + (dis(gen) * 2 - 1) * 0.1;
        double new_p_diff = p_diff + (dis(gen) * 2 - 1) * 0.1;
        double new_p_die = p_die + (dis(gen) * 2 - 1) * 0.1;

        double new_energy = objective_function(time_steps, initial_cells, n_simulations, new_p_div, new_func_decline, new_p_diff, new_p_die, observed_counts);
        double ap = acceptance_probability(energy, new_energy, temperature);

        if(ap > dis(gen)) {
            p_div = new_p_div;
            func_decline = new_func_decline;
            p_diff = new_p_diff;
            p_die = new_p_die;
            energy = new_energy;
        }

        temperature *= COOLING_RATE;
        if(temperature < FINAL_TEMPERATURE) {
            break;
        }
    }
    return std::make_tuple(p_div, p_diff, p_die, func_decline);
}

std::vector<int> read_observed_counts_from_csv(const std::string& filename) {
    std::vector<int> observed_counts;
    std::cout << filename <<std::endl;
    std::ifstream file(filename);
    if(file.is_open()) {
        std::string line;
        while(std::getline(file, line)) {
            std::stringstream ss(line);
            std::string cell_count_str;
             
            while(std::getline(ss, cell_count_str, ',')) { // ここを変更
             std::cout << cell_count_str <<std::endl;
                int cell_count = static_cast<int>(std::round(std::stod(cell_count_str)));
                std::cout << cell_count <<std::endl;
                observed_counts.push_back(cell_count);
                
            }
        }
        file.close();
    }


    // Sort observed_counts in descending order
    std::sort(observed_counts.begin(), observed_counts.end(), std::greater<int>());
    std::cout << observed_counts[0];
    return observed_counts;
}


int main(int argc, char* argv[]) {
    std::string filename = argv[1];
    int time_steps = std::stoi(argv[2]);
    int initial_cells = std::stoi(argv[3]);
    //int n_simulations = std::stoi(argv[4]);

    std::vector<int> observed_counts = read_observed_counts_from_csv(filename);
    int n_simulations = observed_counts.size(); // set n_simulations to number of lines in the file


    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::uniform_real_distribution<> dis(0, 1);

    double p_div = dis(gen)* 0.7;
    double func_decline = dis(gen)*30;
    double p_diff = dis(gen);
    double p_die = dis(gen);
    double sum = p_div + p_diff + p_die;
    if (sum >= 1.0) {
        p_div /= sum;
        p_diff /= sum;
        p_die /= sum;
    }

    auto result = simulated_annealing(time_steps, initial_cells, n_simulations, p_div, func_decline, p_diff, p_die, observed_counts);
    p_div = std::get<0>(result);
    p_diff = std::get<1>(result);
    p_die = std::get<2>(result);
    func_decline = std::get<3>(result);
    
    std::cout << "p_div: " << p_div << std::endl;
    std::cout << "p_diff: " << p_diff << std::endl;
    std::cout << "p_die: " << p_die << std::endl;
    std::cout << "func_decline: " << func_decline << std::endl;

    return 0;
}
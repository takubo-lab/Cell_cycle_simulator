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



double acceptance_probability(double energy, double new_energy, double temperature) {
    if(new_energy < energy) {
        return 1.0;
    }
    return exp((energy - new_energy) / temperature);
}

std::tuple<double, double, double, double, double> simulated_annealing(int time_steps, int initial_cells, int n_simulations, double p_div, double func_decline, double p_diff, double p_die, double initial_temperature, double final_temperature, double cooling_rate, int max_iterations, std::vector<int> observed_counts) {
    //const double INITIAL_TEMPERATURE = 100.0;
    //const double FINAL_TEMPERATURE = 0.1;
    //const double COOLING_RATE = 0.99;
    //const int MAX_ITERATIONS = 1000;

    double temperature = initial_temperature;
    double energy = objective_function(time_steps, initial_cells, n_simulations, p_div, func_decline, p_diff, p_die,observed_counts);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);


    for(int i = 0; i < max_iterations; ++i) {
        double new_p_div = std::clamp(p_div + (dis(gen) * 2 - 1) * 0.1, 0.0, 0.7);
        double new_func_decline = std::clamp(func_decline + (dis(gen) * 2 - 1) * 3, 0.0, 50.0);
        double new_p_diff = std::clamp(p_diff + (dis(gen) * 2 - 1) * 0.1, 0.0, 0.5);
        double new_p_die = std::clamp(p_die + (dis(gen) * 2 - 1) * 0.1, 0.0, 0.5);

        double sum = new_p_div + new_p_diff + new_p_die;
    if (sum >= 1.0) {
        new_p_div /= sum;
        new_p_diff /= sum;
        new_p_die /= sum;
    }


        double new_energy = objective_function(time_steps, initial_cells, n_simulations, new_p_div, new_func_decline, new_p_diff, new_p_die, observed_counts);
        double ap = acceptance_probability(energy, new_energy, temperature);

        if(ap > dis(gen)) {
            p_div = new_p_div;
            func_decline = new_func_decline;
            p_diff = new_p_diff;
            p_die = new_p_die;
            energy = new_energy;
        }
        std::cout <<"iteration: " << i << ", p_div: " << p_div << ", func_decline: " << func_decline << ", p_diff: " << p_diff << ", p_die: " << p_die << ", energy: " << energy << std::endl;

        temperature *= cooling_rate;
        if(temperature < final_temperature) {
            break;
        }
    }
    return std::make_tuple(p_div, p_diff, p_die, func_decline, energy);
}


void write_fitness_to_file(std::tuple<double, double, double, double, double>& result, std::string& output_filename) {
    
    double p_div = std::get<0>(result);
    double p_diff = std::get<1>(result);
    double p_die = std::get<2>(result);
    double func_decline = std::get<3>(result);
    double energy = std::get<4>(result);

    std::ofstream fitness_file(output_filename);
    fitness_file << "p_div: " << p_div << ", func_decline: " << func_decline << ", p_diff: " << p_diff << ", p_die: " << p_die << ", energy: " << energy << std::endl;

    fitness_file.close();    
}



int main(int argc, char* argv[]) {
    std::string filename = argv[1];
    int time_steps = std::stoi(argv[2]);
    int initial_cells = std::stoi(argv[3]);
    //int n_simulations = std::stoi(argv[4]);
    double initial_temperature = std::stod(argv[4]);
    double final_temperature = std::stod(argv[5]);
    double cooling_rate = std::stod(argv[6]);
    int max_iterations = std::stod(argv[7]);
    std::string output_filename = argv[8];


    std::vector<int> observed_counts = read_observed_counts_from_csv(filename);
    int n_simulations = observed_counts.size(); // set n_simulations to number of lines in the file


    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::uniform_real_distribution<> dis(0, 1);

    double p_div = dis(gen)* 0.7;
    double func_decline = dis(gen)*30;
    double p_diff = dis(gen);
    double p_die = dis(gen);
    double energy;
    double sum = p_div + p_diff + p_die;
    if (sum >= 1.0) {
        p_div /= sum;
        p_diff /= sum;
        p_die /= sum;
    }

    auto result = simulated_annealing(time_steps, initial_cells, n_simulations, p_div, func_decline, p_diff, p_die, initial_temperature, final_temperature, cooling_rate, max_iterations, observed_counts);

    write_fitness_to_file(result, output_filename);
    return 0;
}
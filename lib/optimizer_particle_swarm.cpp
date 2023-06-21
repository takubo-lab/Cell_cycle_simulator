#include "Cell.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>


std::vector<double> generate_particle(std::mt19937& gen, std::uniform_real_distribution<>& dis) {
    std::vector<double> particle(4);
    
    
    particle[0] = dis(gen) * 0.7; // p_div
    particle[1] = dis(gen) * 30;  //func_decline
    particle[2] = dis(gen) ;   //p_diff
    particle[3] = dis(gen) * 0.5; //p_die

    return particle;
}

void evaluate_particle(int time_steps, int initial_cells, int n_simulations, std::vector<double>& particle, double& fitness_value, std::vector<int> observed_counts) {
    // Assume that the objective_function is defined elsewhere in your code
    fitness_value = objective_function(time_steps, initial_cells, n_simulations, particle[0], particle[1], particle[2], particle[3], observed_counts);
}

void update_velocity(std::vector<double>& velocity, std::vector<double>& position, std::vector<double>& best_position, std::vector<double>& swarm_best_position, double inertia_weight, double cognitive_weight, double social_weight, std::mt19937& gen, std::uniform_real_distribution<>& dis) {
    for(size_t i = 0; i < velocity.size(); ++i) {
        double r1 = dis(gen);
        double r2 = dis(gen);
        velocity[i] = inertia_weight * velocity[i] + cognitive_weight * r1 * (best_position[i] - position[i]) + social_weight * r2 * (swarm_best_position[i] - position[i]);
    }
}

void update_position(std::vector<double>& position, std::vector<double>& velocity) {
    for(size_t i = 0; i < position.size(); ++i) {
        position[i] += velocity[i];
        // Ensure that position values remain in the range [0, 1]
        if (position[i] < 0) position[i] = 0;
        if (position[i] > 1) position[i] = 1;
    }
}

std::pair<std::vector<double>, double>  optimize_parameters(int time_steps, int initial_cells, int n_simulations,  int num_particles, int num_iterations, double inertia_weight, double cognitive_weight, double social_weight, std::vector<int>& observed_counts) {
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::uniform_real_distribution<> dis(0, 1);

    std::vector<std::vector<double>> particles(num_particles);
    std::vector<double> fitness_values(num_particles);
    std::vector<std::vector<double>> best_positions(num_particles);
    std::vector<double> best_fitness_values(num_particles);
    std::vector<double> swarm_best_position;
    double swarm_best_fitness_value = std::numeric_limits<double>::max();

    for(int i = 0; i < num_particles; i++) {
        particles[i] = generate_particle(gen, dis);
        evaluate_particle(time_steps, n_simulations, initial_cells, particles[i], fitness_values[i],observed_counts);
        best_positions[i] = particles[i];
        best_fitness_values[i] = fitness_values[i];

        if(fitness_values[i] < swarm_best_fitness_value) {
            swarm_best_position = particles[i];
            swarm_best_fitness_value = fitness_values[i];
        }
    }

    for(int i = 0; i < num_iterations; i++) {
        for(int j = 0; j < num_particles; j++) {
            std::vector<double> velocity(particles[j].size());
            update_velocity(velocity, particles[j], best_positions[j], swarm_best_position, inertia_weight, cognitive_weight, social_weight, gen, dis);
            update_position(particles[j], velocity);
            evaluate_particle(time_steps,initial_cells,n_simulations, particles[j], fitness_values[j],observed_counts);

            if(fitness_values[j] < best_fitness_values[j]) {
                best_positions[j] = particles[j];
                best_fitness_values[j] = fitness_values[j];
            }

            if(fitness_values[j] < swarm_best_fitness_value) {
                swarm_best_position = particles[j];
                swarm_best_fitness_value = fitness_values[j];
            }
        }
        std::cout <<"Iteration: " << i << ". p_div: " << swarm_best_position[0] << ", func_decline: " << swarm_best_position[1] << ", p_diff: " << swarm_best_position[2] << ", p_die: " << swarm_best_position[3] <<  ", fitness: " << swarm_best_fitness_value << std::endl;
    }

    return std::make_pair(swarm_best_position, swarm_best_fitness_value);
}




void write_fitness_to_file(std::vector<double>& parameters, double fitness, std::string& output_filename) {
    
    double p_div = parameters[0];
    double func_decline = parameters[1];
    double p_diff = parameters[2];
    double p_die = parameters[3];
    
    std::ofstream fitness_file(output_filename);
    fitness_file << "p_div: " << p_div << ", func_decline: " << func_decline << ", p_diff: " << p_diff << ", p_die: " << p_die <<  ", fitness: " << fitness << std::endl;

    fitness_file.close();    
}


int main(int argc, char* argv[]) {

    std::string filename = argv[1];
    int time_steps = std::stoi(argv[2]);
    int initial_cells = std::stoi(argv[3]);
    //int n_simulations = std::stoi(argv[4]);
    int num_particles = std::stoi(argv[4]); // default 100
    int num_iterations = std::stoi(argv[5]);  //default 100
    double inertia_weight = std::stod(argv[6]);  //default 0.5
    double cognitive_weight = std::stod(argv[7]);  // default 1.0
    double social_weight = std::stod(argv[8]);  //default 1.0
    std::string output_filename = argv[9];
    
    std::vector<int> observed_counts = read_observed_counts_from_csv(filename);
    int n_simulations = observed_counts.size(); // set n_simulations to number of lines in the file


    auto result = optimize_parameters(time_steps, initial_cells, n_simulations, num_particles, num_iterations, inertia_weight, cognitive_weight, social_weight, observed_counts);
    std::vector<double> best_parameters = result.first;
    double best_fitness_value = result.second;

    write_fitness_to_file(best_parameters, best_fitness_value, output_filename);
    
    std::cout << "Best parameters: ";
    for(double parameter : best_parameters) {
        std::cout << parameter << " ";
    }
    std::cout << std::endl;
    std::cout << "Best fitness value: " << best_fitness_value << std::endl;
    std::cout << "Optimization was successfully completed." << std::endl;

    return 0;
}








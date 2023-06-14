#include "Cell.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

double objective_function(double p_die, double p_diff, double p_div, double func_decline) {
    // Run the simulation with the given parameters and calculate the sum of squared errors between the simulated and experimental cell counts
    // Return the objective function value
}

std::vector<double> generate_particle(std::mt19937& gen, std::uniform_real_distribution<>& dis) {
    // Generate a random particle by sampling from the parameter space
    // Return a vector of parameter values
}

void evaluate_particle(std::vector<double>& particle, double& fitness_value) {
    // Evaluate the fitness of the particle using the objective function
    // Store the fitness value in the fitness_value variable
}

void update_velocity(std::vector<double>& velocity, std::vector<double>& position, std::vector<double>& best_position, std::vector<double>& swarm_best_position, double inertia_weight, double cognitive_weight, double social_weight, std::mt19937& gen, std::uniform_real_distribution<>& dis) {
    // Update the velocity of the particle based on its current position, its best position so far, and the best position of the swarm
    // Use the inertia_weight, cognitive_weight, and social_weight parameters to control the influence of each component
    // Use the gen and dis parameters to generate random numbers for the update
}

void update_position(std::vector<double>& position, std::vector<double>& velocity) {
    // Update the position of the particle based on its current velocity
}

std::vector<double> optimize_parameters(int num_particles, int num_iterations, double inertia_weight, double cognitive_weight, double social_weight, std::string& output_filename) {
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
        evaluate_particle(particles[i], fitness_values[i]);
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
            evaluate_particle(particles[j], fitness_values[j]);

            if(fitness_values[j] < best_fitness_values[j]) {
                best_positions[j] = particles[j];
                best_fitness_values[j] = fitness_values[j];
            }

            if(fitness_values[j] < swarm_best_fitness_value) {
                swarm_best_position = particles[j];
                swarm_best_fitness_value = fitness_values[j];
            }
        }
    }

    return swarm_best_position;
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
    
    
    std::vector<double> best_parameters = optimize_parameters(num_particles, num_iterations, inertia_weight, cognitive_weight,social_weight, output_filename);

    std::cout << "Best parameters: ";
    for(double parameter : best_parameters) {
        std::cout << parameter << " ";
    }
    std::cout << std::endl;
    std::cout << "Optimization completed was successfully completed." << std::endl;

    return 0;
}
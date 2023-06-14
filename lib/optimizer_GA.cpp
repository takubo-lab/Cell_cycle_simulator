#include "Cell.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <filesystem>



struct Chromosome {
    double p_div;
    double func_decline;
    double p_diff;
    double p_die;
    double fitness;
};


std::vector<Chromosome> generate_initial_population(int population_size) {
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::uniform_real_distribution<> dis(0, 1);

    std::vector<Chromosome> population(population_size);
    for(auto& chromosome : population) {
        chromosome.p_div = dis(gen) * 0.7;
        chromosome.func_decline = dis(gen) * 20;
        chromosome.p_diff = dis(gen) * (1 - chromosome.p_div);
        chromosome.p_die = dis(gen) * (1 - chromosome.p_div - chromosome.p_diff);
        while(chromosome.p_div + chromosome.p_diff + chromosome.p_die >= 1) {
            chromosome.p_div = dis(gen) * 0.7;
            chromosome.p_diff = dis(gen) * (1 - chromosome.p_div);
            chromosome.p_die = dis(gen) * (1 - chromosome.p_div - chromosome.p_diff);
        }
    }

    return population;
}

double calculate_fitness(const Chromosome& chromosome, int time_steps, int initial_cells, int n_simulations, const std::vector<int>& observed_counts) {
    // simulated cell count vector
    std::vector<int> final_cell_counts(n_simulations);
    


    for(int i = 0; i < n_simulations; ++i) {
        auto cell_counts = simulate(time_steps, initial_cells, chromosome.p_die, chromosome.p_diff, chromosome.p_div, chromosome.func_decline);
        int final_cell_count = cell_counts.back();
        final_cell_counts[i] = final_cell_count;
    }

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



std::vector<Chromosome> select_parents(const std::vector<Chromosome>& population, int tournament_size) {
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<> dis(0, population.size() - 1);

    std::vector<Chromosome> parents;
    for(int i = 0; i < population.size(); ++i) {
        std::vector<Chromosome> tournament(tournament_size);
        for(auto& chromosome : tournament) {
            int index = dis(gen);
            chromosome = population[index];
        }

        auto best_chromosome = *std::max_element(tournament.begin(), tournament.end(), [](const Chromosome& a, const Chromosome& b) {
            return a.fitness < b.fitness;
        });

        parents.push_back(best_chromosome);
    }

    return parents;
}

std::vector<Chromosome> reproduce(const std::vector<Chromosome>& parents, double crossover_rate, double mutation_rate) {
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::uniform_real_distribution<> dis(0, 1);

    std::vector<Chromosome> offspring(parents.size());
    for(int i = 0; i < parents.size(); i += 2) {
        const auto& parent1 = parents[i];
        const auto& parent2 = parents[i + 1];

        auto child1 = parent1;
        auto child2 = parent2;

        if(dis(gen) < crossover_rate) {
            std::uniform_int_distribution<> dis(0, 3);
            int crossover_point = dis(gen);

            switch(crossover_point) {
                case 0:
                    child1.p_div = parent2.p_div;
                    child2.p_div = parent1.p_div;
                    break;
                case 1:
                    child1.func_decline = parent2.func_decline;
                    child2.func_decline = parent1.func_decline;
                    break;
                case 2:
                    child1.p_diff = parent2.p_diff;
                    child2.p_diff = parent1.p_diff;
                    break;
                case 3:
                    child1.p_die = parent2.p_die;
                    child2.p_die = parent1.p_die;
                    break;
            }
        }

        if(dis(gen) < mutation_rate) {
            std::normal_distribution<> dis(0, 0.1);

            child1.p_div += dis(gen);
            child1.func_decline += dis(gen);
            child1.p_diff += dis(gen);
            child1.p_die += dis(gen);

            child2.p_div += dis(gen);
            child2.func_decline += dis(gen);
            child2.p_diff += dis(gen);
            child2.p_die += dis(gen);
        }

        offspring[i] = child1;
        offspring[i + 1] = child2;
    }

    return offspring;
}

std::vector<Chromosome> select_next_generation(const std::vector<Chromosome>& population, const std::vector<Chromosome>& offspring, int elitism_count) {
    std::vector<Chromosome> combined_population = population;
    combined_population.insert(combined_population.end(), offspring.begin(), offspring.end());

    std::sort(combined_population.begin(), combined_population.end(), [](const Chromosome& a, const Chromosome& b) {
        return a.fitness > b.fitness;
    });

    std::vector<Chromosome> next_generation(elitism_count);
    std::copy(combined_population.begin(), combined_population.begin() + elitism_count, next_generation.begin());

    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<> dis(elitism_count, combined_population.size() - 1);

    for(int i = elitism_count; i < population.size(); ++i) {
        int index = dis(gen);
        next_generation.push_back(combined_population[index]);
    }

    return next_generation;
}


std::vector<Chromosome> optimize_parameters(int time_steps, int initial_cells, int n_simulations, int population_size, int tournament_size, double crossover_rate, double mutation_rate, int elitism_count, int max_generations, std::vector<int> observed_counts) {
    std::vector<Chromosome> population = generate_initial_population(population_size);
    //std::ofstream fitness_file("output\\fitness_tmp.csv");

    for(int generation = 0; generation < max_generations; ++generation) {
        for(auto& chromosome : population) {
            chromosome.fitness = calculate_fitness(chromosome, time_steps, initial_cells, n_simulations, observed_counts);
        }

        std::vector<Chromosome> parents = select_parents(population, tournament_size);
        std::vector<Chromosome> offspring = reproduce(parents, crossover_rate, mutation_rate);
        population = select_next_generation(population, offspring, elitism_count);
        std::cout << "generation: " << generation << ", p_div: " << population[0].p_div << ", func_decline: " << population[0].func_decline << ", p_diff: " << population[0].p_diff << ", p_die: " << population[0].p_die << ", fitness: " << population[0].fitness << std::endl;
       // write_fitness_to_file(population, generation,fitness_file);
    }
    //fitness_file.close();
    return population;
}

std::vector<int> read_observed_counts_from_csv(const std::string& filename) {
    std::vector<int> observed_counts;

    std::ifstream file(filename);
    if(file.is_open()) {
        std::string line;
        while(std::getline(file, line)) {
            std::stringstream ss(line);
            std::string cell_count_str;
             
            while(std::getline(ss, cell_count_str, ',')) { 
                int cell_count = static_cast<int>(std::round(std::stod(cell_count_str)));
                observed_counts.push_back(cell_count);
                
            }
        }
        file.close();
    }


    // Sort observed_counts in descending order
    std::sort(observed_counts.begin(), observed_counts.end(), std::greater<int>());
    //std::cout << observed_counts[0];
    return observed_counts;
}

void write_fitness_to_file(const std::vector<Chromosome>& population, std::string& output_filename, int max_generations) {
    

    std::ofstream fitness_file(output_filename);
    for(const auto& chromosome : population) {
        fitness_file << "generation:" << max_generations << ", p_div: " << chromosome.p_div << ", func_decline: " << chromosome.func_decline << ", p_diff: " << chromosome.p_diff << ", p_die: " << chromosome.p_die << ", fitness: " << chromosome.fitness << std::endl;
         
    }
    fitness_file.close();    
}


int main(int argc, char* argv[]) {
    std::string filename = argv[1];
    int time_steps = std::stoi(argv[2]);
    int initial_cells = std::stoi(argv[3]);
    //int n_simulations = std::stoi(argv[4]);
    int population_size = std::stoi(argv[4]); // default 100
    int tournament_size = std::stoi(argv[5]);  //default 5
    double crossover_rate = std::stod(argv[6]);  //default 0.8
    double mutation_rate = std::stod(argv[7]);  // default 0.1
    int elitism_count = std::stoi(argv[8]);  //default 10
    int max_generations = std::stoi(argv[9]);  //default 100
    std::string output_filename = argv[10];

    std::vector<int> observed_counts = read_observed_counts_from_csv(filename);
    int n_simulations = observed_counts.size(); // set n_simulations to number of lines in the file


    std::vector<Chromosome> best_chromosomes = optimize_parameters(time_steps, initial_cells, n_simulations, population_size, tournament_size, crossover_rate, mutation_rate, elitism_count, max_generations,observed_counts);
    write_fitness_to_file(best_chromosomes,output_filename, max_generations); 

    return 0;
}
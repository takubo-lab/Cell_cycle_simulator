#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <sstream>
#include <string>

struct Cell {
    bool differentiated = false;
    int time_since_differentiation = 0; // 新たに追加
};


std::vector<int> simulate(int time_steps, int initial_cells, double p_die, double p_diff, double p_div, double func_decline) {
    
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::uniform_real_distribution<> dis(0, 1);

    std::vector<Cell> cells(initial_cells);
    std::vector<int> cell_counts(time_steps, 0);

    for(int t = 0; t < time_steps; ++t) {
        cell_counts[t] = cells.size();
        std::vector<Cell> new_cells;

        for(auto& cell : cells) {
            if(cell.differentiated) {
                cell.time_since_differentiation++; // 分化している細胞は時間を一つ進める
            }

            double p_div_adjusted = cell.differentiated 
                ? p_div / (1 + func_decline * cell.time_since_differentiation) // 分化した細胞の分裂確率は時間に応じて減少する
                : p_div;

            double rand = dis(gen);
            //std::cout << rand;
            if(rand < p_die) {
               //std::cout << "Dead \n";
               //std::cout << rand << std::endl;
                continue;
            }
            else if(rand <= p_die + p_diff) {
                //std::cout << "Diff \n";
                //std::cout << rand << std::endl;
                cell.differentiated = true;
                new_cells.push_back(cell);
            }

            else if(rand <= p_die + p_diff + p_div_adjusted) {
                //std::cout << "Diff \n";
                //std::cout << rand << std::endl;
                new_cells.push_back(Cell());
                new_cells.push_back(cell);
            }
            else if(rand <= 1) {
               //  std::cout << "Stay \n";
                // std::cout << rand << std::endl;
                new_cells.push_back(cell);
                
            }

          
        }

        cells = new_cells;
    }

    return cell_counts;
}

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
        chromosome.p_div = dis(gen);
        chromosome.func_decline = dis(gen);
        chromosome.p_diff = dis(gen);
        chromosome.p_die = dis(gen);
    }

    return population;
}

double calculate_fitness(const Chromosome& chromosome, int time_steps, int initial_cells, int n_simulations, const std::vector<int>& observed_counts) {
    std::vector<int> final_cell_counts(n_simulations);
    for(int i = 0; i < n_simulations; ++i) {
        auto cell_counts = simulate(time_steps, initial_cells, chromosome.p_die, chromosome.p_diff, chromosome.p_div, chromosome.func_decline);
        int final_cell_count = cell_counts.back();
        final_cell_counts[i] = final_cell_count;
    }

    std::sort(final_cell_counts.begin(), final_cell_counts.end(), std::greater<int>());
    
    double total_sum_of_squares = 0;
    for(int i = 0; i < n_simulations; ++i) {
        double log_simulated_count = log(final_cell_counts[i]+1);
        double log_observed_count = log(observed_counts[i]+1);  // avoid log 0 by adding pseudo count 1
        double difference = log_simulated_count - log_observed_count;
        total_sum_of_squares += difference * difference;
    }

    double fitness = total_sum_of_squares / n_simulations;
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

std::vector<Chromosome> optimize_parameters(int time_steps, int initial_cells, int n_simulations, int population_size, int tournament_size, double crossover_rate, double mutation_rate, int elitism_count, int max_generations) {
    std::vector<Chromosome> population = generate_initial_population(population_size);
    std::vector<int> observed_counts = read_observed_counts_from_csv("observed_counts.csv");
    
    for(int generation = 0; generation < max_generations; ++generation) {
        for(auto& chromosome : population) {
            chromosome.fitness = calculate_fitness(chromosome, time_steps, initial_cells, n_simulations, observed_counts);
        }

        std::vector<Chromosome> parents = select_parents(population, tournament_size);
        std::vector<Chromosome> offspring = reproduce(parents, crossover_rate, mutation_rate);
        population = select_next_generation(population, offspring, elitism_count);
    }

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
            if(std::getline(ss, cell_count_str, ',')) {
                int cell_count = std::stoi(cell_count_str);
                observed_counts.push_back(cell_count);
            }
        }
        file.close();
    }

    return observed_counts;
}

int main() {
    int time_steps = 100;
    int initial_cells = 10;
    int n_simulations = 10;
    int population_size = 100;
    int tournament_size = 5;
    double crossover_rate = 0.8;
    double mutation_rate = 0.1;
    int elitism_count = 10;
    int max_generations = 100;

    std::vector<Chromosome> best_chromosomes = optimize_parameters(time_steps, initial_cells, n_simulations, population_size, tournament_size, crossover_rate, mutation_rate, elitism_count, max_generations);

    for(const auto& chromosome : best_chromosomes) {
        std::cout << "p_div: " << chromosome.p_div << ", func_decline: " << chromosome.func_decline << ", p_diff: " << chromosome.p_diff << ", p_die: " << chromosome.p_die << ", fitness: " << chromosome.fitness << std::endl;
    }

    return 0;
}
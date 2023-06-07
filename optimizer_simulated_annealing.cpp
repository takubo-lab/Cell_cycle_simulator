#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <cmath>

struct Cell {
    bool differentiated = false;
    int time_since_differentiation = 0;
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
                if(cell.differentiated) {
                // If the cell is differentiated, make both daughter cells differentiated
                Cell newCell;
                newCell.differentiated = true;
                new_cells.push_back(newCell);
                new_cells.push_back(cell);
                } else {
                // If the cell is not differentiated, proceed as before
                new_cells.push_back(Cell());
                new_cells.push_back(cell);
                }
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

double objective_function(double p_div, double func_decline, double p_diff, double p_die) {
    const int TIME_STEPS = 100;
    const int INITIAL_CELLS = 10;
    const int N_SIMULATIONS = 10;

    double total_final_cell_count = 0;
    for(int i = 0; i < N_SIMULATIONS; ++i) {
        auto cell_counts = simulate(TIME_STEPS, INITIAL_CELLS, p_die, p_diff, p_div, func_decline);
        int final_cell_count = cell_counts.back();
        total_final_cell_count += final_cell_count;
    }

    double average_final_cell_count = total_final_cell_count / N_SIMULATIONS;
    return -average_final_cell_count; // maximize final cell count
}

double acceptance_probability(double energy, double new_energy, double temperature) {
    if(new_energy < energy) {
        return 1.0;
    }
    return exp((energy - new_energy) / temperature);
}

void simulated_annealing(double& p_div, double& func_decline, double& p_diff, double& p_die) {
    const double INITIAL_TEMPERATURE = 100.0;
    const double FINAL_TEMPERATURE = 0.1;
    const double COOLING_RATE = 0.99;
    const int MAX_ITERATIONS = 1000;

    double temperature = INITIAL_TEMPERATURE;
    double energy = objective_function(p_div, func_decline, p_diff, p_die);

    for(int i = 0; i < MAX_ITERATIONS; ++i) {
        double new_p_div = p_div + (dis(gen) * 2 - 1) * 0.1;
        double new_func_decline = func_decline + (dis(gen) * 2 - 1) * 0.1;
        double new_p_diff = p_diff + (dis(gen) * 2 - 1) * 0.1;
        double new_p_die = p_die + (dis(gen) * 2 - 1) * 0.1;

        double new_energy = objective_function(new_p_div, new_func_decline, new_p_diff, new_p_die);
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
}

int main() {
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::uniform_real_distribution<> dis(0, 1);

    double p_div = dis(gen);
    double func_decline = dis(gen);
    double p_diff = dis(gen);
    double p_die = dis(gen);

    simulated_annealing(p_div, func_decline, p_diff, p_die);

    std::cout << "p_div: " << p_div << std::endl;
    std::cout << "func_decline: " << func_decline << std::endl;
    std::cout << "p_diff: " << p_diff << std::endl;
    std::cout << "p_die: " << p_die << std::endl;

    return 0;
}
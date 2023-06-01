#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <map>
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
            else if(rand <= p_die + p_diff + (p_div - p_div_adjusted)) {
                //std::cout << "Diff \n";
                //std::cout << rand << std::endl;
                cell.differentiated = true;
                new_cells.push_back(cell);
            }
            else if(rand <= 1) {
               //  std::cout << "Div \n";
                // std::cout << rand << std::endl;
                new_cells.push_back(cell);
                new_cells.push_back(Cell());
            }
          
        }

        cells = new_cells;
    }

    return cell_counts;
}

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
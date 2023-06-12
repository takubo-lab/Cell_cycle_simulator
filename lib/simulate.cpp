// Cell.cpp
#include "Cell.h"
#include <random>

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
    

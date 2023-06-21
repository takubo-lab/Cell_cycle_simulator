#include "Cell.h"
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cmath>

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
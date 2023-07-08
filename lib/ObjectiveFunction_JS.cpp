
#include "Cell.h"
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>
#include <iostream>
#include <map>

int get_bin(double x, double min, double max, int n_bins) {
    return floor((x - min) / (max - min) * n_bins);
}

void make_histogram(std::map<int, double>& histogram, const std::vector<int>& data, int n_bins) {
    // Apply log10 transformation and find the range
    std::vector<double> transformed_data(data.size());
    std::transform(data.begin(), data.end(), transformed_data.begin(), [](int x) { return log10(x+1); }); // x+1 to avoid log10(0)
    double min = *std::min_element(transformed_data.begin(), transformed_data.end());
    double max = *std::max_element(transformed_data.begin(), transformed_data.end());

    for(auto x: transformed_data) {
        int bin = get_bin(x, min, max, n_bins);
        histogram[bin] += 1.0;
    }
    for(auto& p: histogram) {
        p.second /= transformed_data.size();
    }
}



double KL_divergence(std::map<int, double> P, std::map<int, double> Q) {
    double kl_divergence = 0;
    for(auto p: P) {
        kl_divergence += p.second * log(p.second / Q[p.first]);
    }
    return kl_divergence;
}

double JS_divergence(std::map<int, double> P, std::map<int, double> Q) {
    std::map<int, double> M;
    for(auto p: P) {
        M[p.first] = 0.5 * (p.second + Q[p.first]);
    }
    return 0.5 * KL_divergence(P, M) + 0.5 * KL_divergence(Q, M);
}

double objective_function(int time_steps, int initial_cells, int n_simulations, double p_div, double func_decline, double p_diff, double p_die, std::vector<int> observed_counts) {
    std::vector<int> final_cell_counts(n_simulations);
    std::map<int, double> P, Q;
    double eps = 1e-10; // to avoid division by zero
    int n_bins = 1 + log2(n_simulations);



    make_histogram(P, observed_counts, n_bins);
    make_histogram(Q, final_cell_counts, n_bins);


    for(int i = 0; i < n_simulations; ++i) {
        auto cell_counts = simulate(time_steps, initial_cells, p_die, p_diff, p_div, func_decline);
        int final_cell_count = cell_counts.back();
        final_cell_counts[i] = final_cell_count;
        Q[final_cell_count] += 1.0;
    }

    for(auto observed_count: observed_counts) {
        P[observed_count] += 1.0;
    }

    for(auto& p: P) {
        p.second = (p.second + eps) / observed_counts.size();
    }

    for(auto& q: Q) {
        q.second = (q.second + eps) / n_simulations;
    }

    double fitness = JS_divergence(P, Q);
    return fitness;
}

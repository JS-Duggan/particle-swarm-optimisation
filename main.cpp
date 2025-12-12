#include "swarm.cpp"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

void saveParetoFront(const std::vector<solution>& pareto_front, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file " << filename << "\n";
        return;
    }

    for (const auto& sol : pareto_front) {
        // Save position
        for (size_t i = 0; i < sol.position.size(); i++) {
            file << sol.position[i];
            if (i != sol.position.size() - 1) file << ",";
        }

        file << ",";

        // Save fitness
        for (size_t i = 0; i < sol.fitness.size(); i++) {
            file << sol.fitness[i];
            if (i != sol.fitness.size() - 1) file << ",";
        }

        file << "\n";
    }

    file.close();
}

int main(int argc, char* argv[]) {

    vector<vector<double>> cov_matrix;
    vector<double> ret_matrix;
    ifstream file1(argv[1]);
    ifstream file2(argv[2]);
    string line;
    // Skip the first row
    getline(file1, line);
    while (getline(file1, line)) {
        stringstream ss(line);
        string cell;

        // Skip the first column
        getline(ss, cell, ',');
        getline(ss, cell, ',');
        ret_matrix.push_back(stod(cell));
    }
    // Skip the first row
    getline(file2, line);
    while (getline(file2, line)) {
        stringstream ss(line);
        string cell;
        vector<double> row;

        // Skip the first column
        getline(ss, cell, ',');

        // Read the numeric values
        while (getline(ss, cell, ',')) {
            row.push_back(stod(cell)); // convert string to double
        }

        cov_matrix.push_back(row);
    }

    auto variance = [& cov_matrix](const vector<double>& position) -> double {
        double variance = 0;
        for (int i = 0; i < position.size(); i++) {
            for (int j = i; j < position.size(); j++) {
                variance += position[i] * position[j] * cov_matrix[i][j];
            }
        }

        return variance;
    };

    auto returns = [& ret_matrix](const vector<double>& position) -> double {
        double r = 0;
        for (int i = 0; i < position.size(); i++) {
            r += position[i] * ret_matrix[i];
        }

        return -r;
    };
	/* 2d f(x, y), 40 particles, 0.7 innertial weight, local and global cognition arre same */
	Swarm swarm(505, 1000, 0.7, 1.4, 1.4, {returns, variance});
	vector<vector<double>> results;

	/* 100 steps */
	for (int i = 0; i < 1000; i++) {
        cout << "Step: " << i << endl;
		swarm.timeStep();
	}

    saveParetoFront(swarm.pareto_front, "results.csv");

}
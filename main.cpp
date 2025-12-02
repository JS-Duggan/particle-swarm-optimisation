#include "swarm.cpp"
#include <iostream>
#include <vector>

int main() {
	/* Himmelblau's functions */
	auto f1 = [](const vector<double>& x) {
		double X = x[0], Y = x[1];
		return X*X + Y - 11.0;
	};
	auto f2 = [](const vector<double>& x) {
		double X = x[0], Y = x[1];
		return X + Y*Y - 7.0;
	};


	Swarm swarm(2, 40, 0.7, 1.4, 1.4, {f1, f2});
	vector<vector<double>> results;
	for (int i = 0; i < 100; i++) {
		results.push_back(swarm.timeStep());
	}

	for (const auto& step : results) {
		for (double x : step) {
			cout << x << " ";
		}
		cout << endl;
	}
}
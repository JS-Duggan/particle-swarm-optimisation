#include "swarm.cpp"
#include <iostream>
#include <vector>

double T2(double z) {
    return 2.0 * z * z - 1.0;
}

double T3(double z) {
    return 4.0 * z * z * z - 3.0 * z;
}

double T4(double z) {
    return 8.0 * z * z * z * z - 8.0 * z * z + 1.0;
}

int main() {
    auto g1 = [](const vector<double>& x) -> double {
        double t3x = T3(x[0]);
        double t2y = T2(x[1]);
        return t3x + t2y;      // g1(x,y) = T3(x) + T2(y)
    };

    auto g2 = [](const vector<double>& x) -> double {
        double t4x = T4(x[0]);
        double t3y = T3(x[1]);
        return t4x - t3y;      // g2(x,y) = T4(x) - T3(y)
    };

    auto g3 = [](const vector<double>& x) -> double {
        return x[0] + x[1] - 1.2;    // g3(x,y) = x + y - 1.2
    };


	/* 2d f(x, y), 40 particles, 0.7 innertial weight, local and global cognition arre same */
	Swarm swarm(2, 40, 0.7, 1.4, 1.4, {g1, g2, g3});
	vector<vector<double>> results;

	/* 100 steps */
	for (int i = 0; i < 1000; i++) {
		swarm.timeStep();
	}
	for (const auto& step : results) {
		for (double x : step) {
			cout << x << " ";
		}
		cout << endl;
	}
}
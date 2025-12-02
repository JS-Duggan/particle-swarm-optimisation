#include "particle.cpp"
#include <ctime>
#include <fstream>

class Swarm {
private:
  /* initial conditions */
  double best_fitness;
  vector<double> g_best;

  vector<vector<vector<double>>> history;

  /* parameters */
  double w;
  double c1;
  double c2;
  mt19937 seed;

  /* objectives to optimise for */
  vector<Particle> swarm;
  vector< function<double(const vector<double>&)> > objectives;
  double fitness(const vector<double>& position);
  void updateBest();

public:
  vector<double> timeStep();
  void saveHistory(const std::string &filename);
  Swarm(int d, int p, double w, double c1, double c2, vector< function<double(const vector<double>&)> > objectives);
};

Swarm::Swarm(int d, int p, double w, double c1, double c2, vector< function<double(const vector<double>&)> > objectives): w(w), c1(c1), c2(c2), objectives(objectives) {
  seed = mt19937(time(nullptr));
  swarm.reserve(p);
  best_fitness = numeric_limits<double>::infinity();
  history.resize(p);
  for (int i = 0; i < p; i++) {
    swarm.emplace_back(d, -10.0, 10.0, seed, [this](const vector<double>& pos) { return fitness(pos); });

    if (swarm[i].best_fitness < best_fitness) {
      best_fitness = swarm[i].best_fitness;
      g_best = swarm[i].p_best;
    }
    history[i].push_back(swarm[i].position);
  }

}

double Swarm::fitness(const vector<double>& position) {
  double total = 0.0;

  for (const auto& f : objectives) {
    total += pow(f(position), 2);
  }

  return total;
}

void Swarm::updateBest() {
  for (const Particle& p : swarm) {
    if (best_fitness > p.best_fitness) {
      best_fitness = p.best_fitness;
      g_best = p.p_best;
    }
  }
}

vector<double> Swarm::timeStep() {
  for (int i = 0; i < swarm.size(); i++) {
    swarm[i].updateVelocity(w, c1, c2, g_best, seed);
    swarm[i].updatePosition([this](const vector<double>& pos) { return fitness(pos); });
    history[i].push_back(swarm[i].position);
  }

  updateBest();

  return g_best;
}

void Swarm::saveHistory(const std::string &filename) {
  ofstream file(filename);

  int num_particles = swarm.size();
  int num_steps     = history[0].size();

  for (int t = 0; t < num_steps; ++t) {
    for (int i = 0; i < num_particles; ++i) {
      file << t << "," << i;
      for (double v : history[i][t]) {
        file << "," << v;
      }
      file << "\n";
    }
  }
}

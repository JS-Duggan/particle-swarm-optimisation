#include "particle.cpp"
#include <ctime>

class Swarm {
private:
  /* initial conditions */
  double best_fitness;
  vector<double> g_best;

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
  Swarm(int d, int p, double w, double c1, double c2, vector< function<double(const vector<double>&)> > objectives);
};

Swarm::Swarm(int d, int p, double w, double c1, double c2, vector< function<double(const vector<double>&)> > objectives): w(w), c1(c1), c2(c2), objectives(objectives) {
  swarm.reserve(p);
  best_fitness = INT_MAX;
  for (int i = 0; i < p; i++) {
    swarm.emplace_back(d);
    swarm[i].best_fitness = fitness(swarm[i].position);
    if (swarm[i].best_fitness < best_fitness) {
      best_fitness = swarm[i].best_fitness;
      g_best = swarm[i].p_best;
    }
  }

  seed = mt19937(time(nullptr));
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
  
  for (Particle& p : swarm) {
    p.updateVelocity(w, c1, c2, g_best, seed);
    p.updatePosition([this](const vector<double>& pos) { return fitness(pos); });
  }

  updateBest();

  return g_best;
}
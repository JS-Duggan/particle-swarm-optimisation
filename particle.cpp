#include <vector>
#include <functional>
#include <random>

using namespace std;

#ifndef PARTICLE_CPP
#define PARTICLE_CPP

struct Particle {
  int dimension;
  double best_fitness;
  vector<double> p_best;
  vector<double> velocity;
  vector<double> position;

  Particle(int d) : dimension(d), best_fitness(INT_MAX), p_best(d), velocity(d), position(d) {};
  void updateVelocity(double w, double c1, double c2, const vector<double>& g_best, mt19937& gen);
  void updatePosition(const std::function<double(const vector<double>&)>& fitness);
};

void Particle::updatePosition(const function<double(const vector<double>&)>& fitness) {
  for (int i = 0; i < dimension; i++) {
    position[i] += velocity[i];
  }
  double current_fitness = fitness(position);

  if (current_fitness < best_fitness) {
    best_fitness = current_fitness;
    p_best = position;
  }
}

void Particle::updateVelocity(double w, double c1, double c2, const vector<double>& g_best, mt19937& gen) {
  uniform_real_distribution<double> dist(0.0, 1.0);

  for (int i = 0; i < dimension; i++) {
    double local = c1 * dist(gen) * (p_best[i] - position[i]);    //local
    double global = c2 * dist(gen) * (g_best[i] - position[i]);   //global 
    velocity[i] = velocity[i] * w + local + global;
  }
}

#endif
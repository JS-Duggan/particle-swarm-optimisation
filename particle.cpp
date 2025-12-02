#include <vector>
#include <functional>
#include <random>
#include <limits>

using namespace std;

#ifndef PARTICLE_CPP
#define PARTICLE_CPP

struct Particle {
  int dimension;
  double best_fitness;
  double lower;
  double upper;
  double vmax;

  vector<double> p_best;
  vector<double> velocity;
  vector<double> position;

  Particle(int d, double lower, double upper, mt19937& gen, const function<double(const vector<double>&)>& fitness);
  void updateVelocity(double w, double c1, double c2, const vector<double>& g_best, mt19937& gen);
  void updatePosition(const std::function<double(const vector<double>&)>& fitness);
};

Particle::Particle(int d, double lower, double upper, mt19937& gen, const function<double(const vector<double>&)>& fitness) : 
  dimension(d), 
  p_best(d), 
  velocity(d), 
  position(d),
  upper(upper),
  lower(lower) {
  uniform_real_distribution<double> dist(lower, upper);

  for (double& x : position) {
    x = dist(gen);
  }
  best_fitness = fitness(position);
  p_best = position;
  vmax = (upper - lower) * 0.2;
}

void Particle::updatePosition(const function<double(const vector<double>&)>& fitness) {
  for (int i = 0; i < dimension; i++) {
    position[i] += velocity[i];
    if (position[i] < lower) position[i] = lower;
    else if (position[i] > upper) position[i] = upper;
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
    if (velocity[i] < -vmax) velocity[i] = vmax;
    else if (velocity[i] > vmax) velocity[i] = vmax;
  }
}

#endif
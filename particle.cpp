#include <vector>
#include <functional>
#include <random>
#include <limits>

using namespace std;

#ifndef PARTICLE_CPP
#define PARTICLE_CPP

struct solution {
  vector<double> position;
  vector<double> fitness;

  solution(int d): position(d) {};
};

struct Particle {
  int dimension;
  double lower;
  double upper;
  double vmax;
  
  solution p_best;
  vector<double> velocity;
  vector<double> position;

  Particle(int d, double lower, double upper, mt19937& gen, const function<vector<double>(const vector<double>&)>& fitness);
  void updateVelocity(double w, double c1, double c2, const vector<double>& g_best, mt19937& gen);
  void updatePosition();
  void updateBestPosition(vector<double> position, vector<double> fitness);
  void normaliseWeight();
};

/**
 * particle constructor sets the dimension of the solution space, generates a random position based on a provided seed and
 * sets the upper and lower limites of the search space
 */
Particle::Particle(int d, double lower, double upper, mt19937& gen, const function<vector<double>(const vector<double>&)>& fitness) : 
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
  p_best.fitness = fitness(position);
  p_best.position = position;
  vmax = (upper - lower) * 0.1;
}

/** 
* position is updated by adding the velocity to the current position
* to prevent solutions exiting the search space, velocity is inverted at the boundary
*/
void Particle::updatePosition() {
  for (int i = 0; i < dimension; i++) {
    position[i] += velocity[i];
    if (position[i] < lower) {
      position[i] = lower + (lower - velocity[i]);
      velocity[i] = -velocity[i];
    }
    else if (position[i] > upper) {
      position[i] = upper - (position[i] - upper);
      velocity[i] = -velocity[i];
    }
  }
}

/**
 * selecting the best position will be done by first checking if either the new position or previous best are dominated
 * If there is a dominated solution, that will be discarded. Else, select the solution that has the lowest crowding factor
 */
void Particle::updateBestPosition(vector<double> position, vector<double> fitness) {
  p_best.position = position;
  p_best.fitness = fitness;
}

/** 
 * update velocity based on the function V(t+1) = w * V(t) + c_1 * r_1 * (p_best - position) +  c_1 * r_1 * (p_best - g_best)
 * using a pareto front, the closest non-dominated solution will be g_best
*/
void Particle::updateVelocity(double w, double c1, double c2, const vector<double>& g_best, mt19937& gen) {
  
  uniform_real_distribution<double> dist(0.0, 1.0);

  for (int i = 0; i < dimension; i++) {
    double local = c1 * dist(gen) * (p_best.position[i] - position[i]);    //local
    double global = c2 * dist(gen) * (g_best[i] - position[i]);   //global 
    velocity[i] = velocity[i] * w + local + global;
    if (velocity[i] < -vmax) velocity[i] = -vmax;
    else if (velocity[i] > vmax) velocity[i] = vmax;
  }
}


void Particle::normaliseWeight() {
  double sum = accumulate(position.begin(), position.end(), 0.0);

  if (sum == 0) {
    double val = 1.0 / position.size();
    fill(position.begin(), position.end(), val);
  }

  for (double& p : position) {
    p /= sum;
  }
}
#endif
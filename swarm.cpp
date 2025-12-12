#include "particle.cpp"
#include <ctime>
#include <fstream>
#include <algorithm>

class Swarm {
private:
  /* initial conditions */
  
  /* parameters */
  int dim;
  double w;
  double c1;
  double c2;
  mt19937 seed;
  
  /* objectives to optimise for */
  vector<Particle> swarm;
  vector< function<double(const vector<double>&)> > objectives;
  vector<double> fitness(const vector<double>& position);
  vector<double> closestFrontier(const vector<double>& position);
  void updateFront();
  void updateParticleBest(Particle& p);
  bool lessCrowded(const vector<double>& a, const vector<double>& b);
  double crowdingDistance(const vector<double>& candidate);
  
  public:
  vector<solution> pareto_front;
  vector<vector<solution>> history;
  void timeStep();
  Swarm(int d, int p, double w, double c1, double c2, vector< function<double(const vector<double>&)> > objectives);
};

Swarm::Swarm(int d, int p, double w, double c1, double c2, vector< function<double(const vector<double>&)> > objectives): dim(d), w(w), c1(c1), c2(c2), objectives(objectives) {
  seed = mt19937(time(nullptr));
  swarm.reserve(p);
  for (int i = 0; i < p; i++) {
    /* search space is 0 to 1 */
    swarm.emplace_back(d, 0.0, 1.0, seed, [this](const vector<double>& pos) { return fitness(pos); });
  }
  updateFront();
}

vector<double> Swarm::fitness(const vector<double>& position) {
  vector<double> fit;
  fit.reserve(objectives.size());
  for (const auto& f : objectives) {
    fit.emplace_back(f(position));
  }

  return fit;
}

void Swarm::timeStep() {
  for (int i = 0; i < swarm.size(); i++) {
    vector<double> g_best = closestFrontier(swarm[i].position);
    swarm[i].updateVelocity(w, c1, c2, g_best, seed);
    swarm[i].updatePosition();
    swarm[i].normaliseWeight();
    updateParticleBest(swarm[i]);
  }

  updateFront();
  history.push_back(pareto_front);
}

void Swarm::updateParticleBest(Particle& p) {

  vector<double> current_fitness = fitness(p.position);
  bool dominates = true;
  bool strictly_better = false;
  bool pbest_dominates = true;
  bool strictly_better_pbest = false;

  for (int i = 0; i < current_fitness.size(); i++) {
    /* worse in at least one objective */
    if (current_fitness[i] > p.p_best.fitness[i]) dominates = false;
    if (p.p_best.fitness[i] > current_fitness[i]) pbest_dominates = false;
    /* better in at least one */
    if (current_fitness[i] < p.p_best.fitness[i]) strictly_better = true;
    if (p.p_best.fitness[i] < current_fitness[i]) strictly_better_pbest = true;
  }

  if (dominates && strictly_better) {
    p.p_best.position = p.position;
    p.p_best.fitness = current_fitness;
    return;
  }

  if (pbest_dominates && strictly_better_pbest) return;

  if (lessCrowded(current_fitness, p.p_best.fitness)) {
    p.p_best.position = p.position;
    p.p_best.fitness = current_fitness;
  }
}

bool Swarm::lessCrowded(const vector<double>& a, const vector<double>& b) {
  double a_crowding = crowdingDistance(a);
  double b_crowding = crowdingDistance(b);
  return a_crowding > b_crowding;
}

double Swarm::crowdingDistance(const vector<double>& candidate) {
    double totalCrowding = 0.0;

    for (int m = 0; m < pareto_front[0].fitness.size(); ++m) {
        // Extract objective m values from the Pareto front
        vector<double> objValues;
        for (const auto& s : pareto_front) objValues.push_back(s.fitness[m]);

        // Sort objective values
        sort(objValues.begin(), objValues.end());

        // Find neighbors
        auto it = std::lower_bound(objValues.begin(), objValues.end(), candidate[m]);
        double prev, next;

        if (it == objValues.begin()) {
            prev = *it;  // boundary
            next = *(it + 1);
        } else if (it == objValues.end() - 1) {
            prev = *(it - 1);
            next = *it;  // boundary
        } else {
            prev = *(it - 1);
            next = *(it + 1);
        }

        double f_min = objValues.front();
        double f_max = objValues.back();
        if (f_max == f_min) f_max += 1e-12; // avoid divide by zero

        double cd = (next - prev) / (f_max - f_min);
        totalCrowding += cd;
    }

    return totalCrowding;
}

bool dominates(const solution& a, const solution& b) {
    bool strictlyBetter = false;
    int dim = a.fitness.size();
    for (int i = 0; i < dim; ++i) {
        if (a.fitness[i] > b.fitness[i]) return false; // worse in at least one
        if (a.fitness[i] < b.fitness[i]) strictlyBetter = true;
    }
    return strictlyBetter;
}

void Swarm::updateFront() {
  for (const auto& candidate : swarm) {
    solution cand = candidate.p_best;
    bool dominated = false;
    std::vector<int> to_remove;

    // Check against existing front
    for (int i = 0; i < pareto_front.size(); ++i) {
      if (dominates(pareto_front[i], cand)) {
        dominated = true;   // candidate is dominated
        break;
      }
      if (dominates(cand, pareto_front[i])) {
        to_remove.push_back(i);  // mark dominated solution to remove
      }
    }

    if (dominated) continue;  // candidate is dominated; skip

    // Remove dominated solutions
    // Remove from back to front to avoid invalidating indices
    for (int i = to_remove.size() - 1; i >= 0; --i) {
      pareto_front.erase(pareto_front.begin() + to_remove[i]);
    }

    // Add the non-dominated candidate
    pareto_front.push_back(cand);
  }
}

vector<double> Swarm::closestFrontier(const vector<double>& position) {
  double distance = HUGE_VAL;
  vector<double> point(dim);

  for (const auto& p : pareto_front) {
    double running = 0.0;

    for (int i = 0; i < dim; i++) {
      running += pow((position[i] - p.position[i]), 2);
    }

    if (running < distance) {
      distance = running;
      point = p.position;
    }
  }

  return point;
}

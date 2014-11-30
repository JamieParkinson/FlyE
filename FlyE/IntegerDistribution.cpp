#include "IntegerDistribution.h"

typedef std::uniform_int_distribution<> u_int_dist;
typedef std::piecewise_linear_distribution<> piecewise_dist;

IntegerDistribution::~IntegerDistribution() {}

UniformDistribution::UniformDistribution(int kLower, int kUpper) {
  u_int_dist::param_type params { kLower, kUpper };
  dist_.param(params);
}

int UniformDistribution::operator ()(mersenne_twister &generator) {
  return dist_(generator);
}

SingleDistribution::SingleDistribution(int k)
    : k_(k) {
}

int SingleDistribution::operator ()(mersenne_twister &generator) {
  return k_;
}

TriangleDistribution::TriangleDistribution(std::vector<float> &kpieces,
                                           std::vector<float> &kweights) {
  piecewise_dist::param_type params { kpieces.begin(), kpieces.end(), kweights
      .begin() };
  dist_.param(params);
}

int TriangleDistribution::operator ()(mersenne_twister &generator) {
  return static_cast<int>(dist_(generator));
}

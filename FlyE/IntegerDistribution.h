#pragma once

#include <random> // C++11 RNGs

// Using the Curiously Recurring Template Pattern

/** @brief A wrapper class for some C++11 RNGs
 *
 * The integer RNGs in C++11 are curiously not all derived from some bass class.
 * This means they can be a pain to use - this class offers a very limited level of abstraction.
 * Note that the distributions can only use a Mersenne Twister generator.
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class IntegerDistribution {
 public:
  /** @brief Virtual operator to get a number from a generator
   *
   * @param generator An instance of a std::mt19937
   * @return A random integer
   */
  virtual int operator()(std::mt19937 &generator) = 0;

  virtual ~IntegerDistribution();
};

/** @brief Uniform distribution of integers */
class UniformDistribution : public IntegerDistribution {
 private:
  std::uniform_int_distribution<> dist_; //!< The underlying RNG

 public:
  /** @brief Constructor for the uniform distribution
   *
   * @param kLower Lower bound for distribution
   * @param kUpper Upper bound for distribution
   */
  UniformDistribution(int kLower, int kUpper) {
    std::uniform_int_distribution<>::param_type params { kLower, kUpper };
    dist_.param(params);
  }

  /** @brief Returns uniformly distributed integers
   *
   * @param generator A std::mt19937 instance
   * @return A uniformly distributed integer
   */
  int operator()(std::mt19937 &generator) {
    return dist_(generator);
  }
};

/** @brief A single integer. Dirac delta distribution */
class SingleDistribution : public IntegerDistribution {
 private:
  int k_; //!< The number to always return

 public:
  /** @brief Sets the number to return */
  SingleDistribution(int k)
      : k_(k) {
  }

  /** @brief Always returns the value of k
   *
   * @param generator A std:mt19937 instance
   * @return k
   */
  int operator()(std::mt19937 &generator) {
    return k_;
  }
};

/** @brief A triangular integer distribution */
class TriangleDistribution : public IntegerDistribution {
 private:
  std::piecewise_linear_distribution<> dist_; //!< The underlying RNG

 public:
  /** @brief Initialises the distribution
   *
   * @param kpieces pieces
   * @param kweights weights
   */
  TriangleDistribution(std::vector<float> &kpieces,
                       std::vector<float> &kweights) {
    std::piecewise_linear_distribution<>::param_type params { kpieces.begin(),
        kpieces.end(), kweights.begin() };
    dist_.param(params);
  }

  /** @brief Returns triangularly distributed integers
   *
   * @param generator A std::mt19937 instance
   * @return A triangularly distributed integer
   */
  int operator()(std::mt19937 &generator) {
    return (int) dist_(generator);
  }
};

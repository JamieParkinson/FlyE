#pragma once

#include <random> // C++11 RNGs

typedef std::mt19937 mersenne_twister;

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
   * @param generator An instance of a mersenne_twister
   * @return A random integer
   */
  virtual int operator()(mersenne_twister &generator) = 0;

  /** @brief Blank virtual destructor */
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
  UniformDistribution(int kLower, int kUpper);

  /** @brief Returns uniformly distributed integers
   *
   * @param generator A mersenne_twister instance
   * @return A uniformly distributed integer
   */
  int operator()(mersenne_twister &generator);
};

/** @brief A single integer. Dirac delta distribution */
class SingleDistribution : public IntegerDistribution {
 private:
  int k_; //!< The number to always return

 public:
  /** @brief Sets the number to return */
  SingleDistribution(int k);

  /** @brief Always returns the value of k
   *
   * @param generator A std:mt19937 instance
   * @return k
   */
  int operator()(mersenne_twister &generator);
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
                       std::vector<float> &kweights);

  /** @brief Returns triangularly distributed integers
   *
   * @param generator A mersenne_twister instance
   * @return A triangularly distributed integer
   */
  int operator()(mersenne_twister &generator);
};

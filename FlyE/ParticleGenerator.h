#pragma once

#include <memory>
#include <vector>

#include "AntiHydrogen.h"
#include "SubConfig.h"
#include "IntegerDistribution.h"

typedef std::mt19937 mersenne_twister;

template<typename PType>
/** @brief A class to generate a vector of particles for the simulation
 *
 * Particle parameters are held in the shared_ptr<ParticlesConfig> object. Type of particles is the template parameter.
 * (Currently only AntiHydrogen)
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class ParticleGenerator {
 protected:
  std::shared_ptr<ParticlesConfig> particlesConfig_;  //!< Holds configuration data for particles
  std::shared_ptr<AcceleratorConfig> acceleratorConfig_;  //!< Holds configuration data for the geometry; necessary for putting particles in the right places
  std::vector<PType> particles_; //!< The vector of particles which will be created

  /** @brief Generates normally distributed particles
   *
   * @param generator An instance of std::mt19937
   * @param sigmaV Standard deviation of particle velocity (from temperature)
   * @param sectionWidth The width of one section of the accelerator
   * @param kDist A pointer to an IntegerDistribution
   */
  void generateNormDist(mersenne_twister generator, float sigmaV, int sectionWidth,
                        IntegerDistribution *kDist);

  /** @brief Generates uniformly distributed particles
   *
   * @param generator An instance of std::mt19937
   * @param sigmaV Standard deviation of particle velocity (from temperature)
   * @param sectionWidth The width of one section of the accelerator
   * @param kDist A pointer to an IntegerDistribution
   */
  void generateUniformDist(mersenne_twister generator, float sigmaV,
                           int sectionWidth, IntegerDistribution *kDist);

 public:
  /** @brief Constructs a ParticleGenerator from the given configuration objects
   *
   * @param particlesConfig Config data for particles
   * @param acceleratorConfig Config data for the accelerator
   */
  ParticleGenerator(std::shared_ptr<ParticlesConfig> particlesConfig,
                    std::shared_ptr<AcceleratorConfig> acceleratorConfig);

  /** @brief Generates a single particle
   *
   * To be used as the synchronous particle
   *
   * @param x x-coordinate of particle
   * @param y y-coordinate of particle
   * @param z z-coordinate of particle
   * @param vx x-component of particle velocity
   * @param vy y-component of particle velocity
   * @param vz z-component of particle velocity
   */
  void generateSynchronousParticle(int x, int y, int z, int vx, int vy, int vz);

  /** @brief Generates particles according to the particles configuration object
   *
   * Calls either generateNormDist() or generateUniformDist()
   */
  void generateParticles();

  /** @brief Returns the generated particles
   *
   * @return All the particles this generator has created
   */
  std::vector<PType>& getParticles();
};

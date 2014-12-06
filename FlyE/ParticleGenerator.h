/**@file ParticleGenerator.h
 * @brief This file contains the ParticleGenerator class
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
#pragma once

#include <memory>
#include <vector>
#include <random>

#include "ezETAProgressBar.hpp"

#include "AntiHydrogen.h"
#include "SubConfig.h"
#include "IntegerDistribution.h"
#include "PhysicalConstants.h"

/** @brief The standard (MT19937) mersenne twister RNG */
typedef std::mt19937 mersenne_twister;

/** @brief Normal distribution of floats */
typedef std::normal_distribution<float> float_n_dist;

/** @brief Uniform distribution of floats */
typedef std::uniform_real_distribution<float> float_u_dist;

template<class PType>
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
  std::vector<PType> particles_;  //!< The vector of particles which will be created

  /** @brief Generates normally distributed particles
   *
   * @param generator An instance of std::mt19937
   * @param sigmaV Standard deviation of particle velocity (from temperature)
   * @param sectionWidth The width of one section of the accelerator
   * @param kDist A pointer to an IntegerDistribution
   */
  void generateNormDist(mersenne_twister generator, float sigmaV,
                        int sectionWidth, IntegerDistribution *kDist);

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
                    std::shared_ptr<AcceleratorConfig> acceleratorConfig)
      : particlesConfig_(particlesConfig),
        acceleratorConfig_(acceleratorConfig) {
  }

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

template<class PType>
std::vector<PType>& ParticleGenerator<PType>::getParticles() {
  return particles_;
}

/** @brief Template specialisation for the AntiHydrogen generator
 * @copydoc ParticleGenerator<PType>::generateSynchronousParticle()
 */
template<> void
ParticleGenerator<AntiHydrogen>::generateSynchronousParticle(
    int x, int y, int z, int vx, int vy, int vz) {
  particles_.emplace_back(x, y, z, vx, vy, vz, particlesConfig_->n(),
                          particlesConfig_->k());
}

/** @brief Template specialisation for the AntiHydrogen generator
 * @copydoc ParticleGenerator<PType>::generateNormDist()
 */
template<> void
ParticleGenerator<AntiHydrogen>::generateNormDist(
    mersenne_twister generator, float sigmaV, int sectionWidth,
    IntegerDistribution *kDist) {

  float_n_dist x_dist(acceleratorConfig_->x() / 2, particlesConfig_->sigmaX());  // Center at start of cylinder
  float_n_dist y_dist(acceleratorConfig_->y() / 2, particlesConfig_->sigmaY());
  float_n_dist z_dist(2.25 * sectionWidth, particlesConfig_->sigmaZ());  // Should mean most particles are in the cylinder (~0.27% are outside)
  float_n_dist v_dist(0, sigmaV);

  ez::ezETAProgressBar particlesBar(particlesConfig_->nParticles() - 1);
  particlesBar.start();

  for (int i = 0; i < particlesConfig_->nParticles() - 1; ++i, ++particlesBar) {  // Generate particles
    particles_.emplace_back(x_dist(generator), y_dist(generator),
                            z_dist(generator), v_dist(generator),
                            v_dist(generator), v_dist(generator),
                            particlesConfig_->n(), (*kDist)(generator));
  }

}

/** @brief Template specialisation for the AntiHydrogen generator
 * @copydoc ParticleGenerator<PType>::generateUniformDist()
 */
template<> void
ParticleGenerator<AntiHydrogen>::generateUniformDist(
    mersenne_twister generator, float sigmaV, int sectionWidth,
    IntegerDistribution *kDist) {

  float_u_dist uniform_dist(0, 1);
  float_n_dist muller_dist(0, 1);  // Using (a modified version of) the method described by Muller (1959) to generate spherical velocity dist.

  muller_dist(generator);

  ez::ezETAProgressBar particlesBar(particlesConfig_->nParticles() - 1);
  particlesBar.start();

  for (int i = 0; i < particlesConfig_->nParticles() - 1; ++i, ++particlesBar) {  // Generate particles
    float p_theta = 2 * M_PI * uniform_dist(generator);
    float p_r = (0.5 * acceleratorConfig_->y() - 5)
        * sqrt(uniform_dist(generator));
    float v_r = 2 * sigmaV * pow(uniform_dist(generator), 1 / 3.);
    float vx = muller_dist(generator);
    float vy = muller_dist(generator);
    float vz = muller_dist(generator);

    float muller_factor = pow(pow(vx, 2) + pow(vy, 2) + pow(vz, 2), -0.5);

    particles_.emplace_back(p_r * cos(p_theta) + 0.5 * acceleratorConfig_->x(),
                            p_r * sin(p_theta) + 0.5 * acceleratorConfig_->y(),
                            sectionWidth * (3 * uniform_dist(generator) + 1),
                            v_r * muller_factor * vx, v_r * muller_factor * vy,
                            v_r * muller_factor * vz, particlesConfig_->n(),
                            (*kDist)(generator));
  }
}

/** @brief Template specialisation for the AntiHydrogen generator
 * @copydoc ParticleGenerator<PType>::generateParticles()
 */
template<> void
ParticleGenerator<AntiHydrogen>::generateParticles() {
  float sigmaV = sqrt(
      2 * (particlesConfig_->temperature() * Physics::kb / Physics::FWHMfactor)
          / Physics::mH);  // Std dev of velocity
  int sectionWidth = Physics::N_IN_SECTION * acceleratorConfig_->z()
      / acceleratorConfig_->nElectrodes();  // Width of each section of 4 electrodes

  mersenne_twister generator;

  IntegerDistribution* kDist;
  if (particlesConfig_->kDist() == "single") {
    kDist = new SingleDistribution(particlesConfig_->k());
  } else if (particlesConfig_->kDist() == "uniform") {
    kDist = new UniformDistribution(1, particlesConfig_->n() - 1);
  } else {  // kDist == Triangle
    std::vector<float> kpieces { 0.0, (float) particlesConfig_->n() };
    std::vector<float> kweights { 1, 0 };
    kDist = new TriangleDistribution(kpieces, kweights);
  }

  generateSynchronousParticle(0.5 * acceleratorConfig_->x(),
                              0.5 * acceleratorConfig_->y(), 0.5 * sectionWidth,
                              0, 0, 0);

  std::cout << "Generating " << particlesConfig_->nParticles() << " particles..." << std::endl;

  if (particlesConfig_->normDist())
    generateNormDist(generator, sigmaV, sectionWidth, kDist);
  else
    generateUniformDist(generator, sigmaV, sectionWidth, kDist);

  std::cout << std::endl;
}

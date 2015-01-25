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

  /** @brief Generates a radially uniform distribution of velocities with radius v_r
   *
   * @param generator An instance of std::mt19937
   * @param v_r Standard deviation of particle velocity (from temperature)
   * @return A tuple of 3 floats - (vx, vy, vz)
   */
  tuple3Dfloat generateUniformVels(float sigmaV, mersenne_twister generator);

  /** @brief Generates normally distributed particles
   *
   * @param generator An instance of std::mt19937
   * @param sigmaV Standard deviation of particle velocity (from temperature)
   * @param normVels Whether the velocities are normally distributed
   * @param sectionWidth The width of one section of the accelerator
   * @param offset The distance to offset the particle distribution from the (start) end of the accelerator
   * @param kDist A pointer to an IntegerDistribution
   */
  void generateNormDist(mersenne_twister generator, float sigmaV,
                        bool normVels, int sectionWidth, float offset,
                        IntegerDistribution *kDist);

  /** @brief Generates uniformly distributed particles
   *
   * @param generator An instance of std::mt19937
   * @param sigmaV Standard deviation of particle velocity (from temperature)
   * @param normVels Whether the velocities are normally distributed
   * @param sectionWidth The width of one section of the accelerator
   * @param offset The distance to offset the particle distribution from the (start) end of the accelerator
   * @param kDist A pointer to an IntegerDistribution
   * @param full Whether to fill the complete phase space (default false)
   */
  void generateUniformDist(mersenne_twister generator, float sigmaV,
                           bool normVels, int sectionWidth, float offset,
                           IntegerDistribution *kDist, bool full = false);

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
  void generateSynchronousParticle(float x, float y, float z, float vx, float vy, float vz);

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

template<class PType>
tuple3Dfloat ParticleGenerator<PType>::generateUniformVels(
    float sigmaV, mersenne_twister generator) {
  float_u_dist uniform_dist(0, 1);
  float v_r = 2 * sigmaV * pow(uniform_dist(generator), 1 / 3.);

  // Using (a modified version of) the method described by Muller (1959) to generate spherical velocity dist.
  float_n_dist muller_dist(0, 1);

  float x = muller_dist(generator);
  float y = muller_dist(generator);
  float z = muller_dist(generator);

  float muller_factor = pow(pow(x, 2) + pow(y, 2) + pow(z, 2), -0.5);

  return std::make_tuple(x * v_r * muller_factor, y * v_r * muller_factor,
                         z * v_r * muller_factor);
}

/** @brief Template specialisation for the AntiHydrogen generator
 * @copydoc ParticleGenerator<PType>::generateSynchronousParticle()
 */
template<> void ParticleGenerator<AntiHydrogen>::generateSynchronousParticle(
    float x, float y, float z, float vx, float vy, float vz) {
  particles_.emplace_back(x, y, z, vx, vy, vz, particlesConfig_->n(),
                          particlesConfig_->k());
}

/** @brief Template specialisation for the AntiHydrogen generator
 * @copydoc ParticleGenerator<PType>::generateNormDist()
 */
template<> void ParticleGenerator<AntiHydrogen>::generateNormDist(
    mersenne_twister generator, float sigmaV, bool normVels,
    int sectionWidth, float offset, IntegerDistribution *kDist) {

  float_n_dist x_dist(acceleratorConfig_->x() / 2,
                      particlesConfig_->distRadius());  // Center at start of cylinder
  float_n_dist y_dist(acceleratorConfig_->y() / 2,
                      particlesConfig_->distRadius());
  float_n_dist z_dist(2.25*offset, particlesConfig_->distLength());  // Should mean most particles are in the cylinder (~0.27% are outside)
  float_n_dist v_dist(0, sigmaV);

  ez::ezETAProgressBar particlesBar(particlesConfig_->nParticles() - 1);
  particlesBar.start();

  for (int i = 0; i < particlesConfig_->nParticles() - 1; ++i, ++particlesBar) {  // Generate particles
    tuple3Dfloat velocities =
        (!normVels) ?
            generateUniformVels(sigmaV, generator) :
            std::make_tuple(v_dist(generator), v_dist(generator),
                            v_dist(generator));

    particles_.emplace_back(x_dist(generator), y_dist(generator),
                            z_dist(generator), std::get<0>(velocities),
                            std::get<1>(velocities), std::get<2>(velocities),
                            particlesConfig_->n(), (*kDist)(generator));
  }

}

/** @brief Template specialisation for the AntiHydrogen generator
 * @copydoc ParticleGenerator<PType>::generateUniformDist()
 */
template<> void ParticleGenerator<AntiHydrogen>::generateUniformDist(
    mersenne_twister generator, float sigmaV, bool normVels,
    int sectionWidth, float offset, IntegerDistribution *kDist, bool full) {

  float_u_dist uniform_dist(0, 1);
  float_n_dist v_dist(0, sigmaV);

  float radius = (!full) ? particlesConfig_->distRadius() : 0.5*acceleratorConfig_->x() - 5;
  float length = (!full) ? particlesConfig_->distLength() : 3*sectionWidth;

  ez::ezETAProgressBar particlesBar(particlesConfig_->nParticles() - 1);
  particlesBar.start();

  for (int i = 0; i < particlesConfig_->nParticles() - 1; ++i, ++particlesBar) {  // Generate particles
    float p_theta = 2 * M_PI * uniform_dist(generator);
    float p_r = radius * sqrt(uniform_dist(generator));

    tuple3Dfloat velocities =
        (!normVels) ?
            generateUniformVels(sigmaV, generator) :
            std::make_tuple(v_dist(generator), v_dist(generator),
                            v_dist(generator));

    particles_.emplace_back(
        p_r * cos(p_theta) + 0.5 * acceleratorConfig_->x(),
        p_r * sin(p_theta) + 0.5 * acceleratorConfig_->y(),
        offset + length*uniform_dist(generator),
        std::get<0>(velocities), std::get<1>(velocities),
        std::get<2>(velocities), particlesConfig_->n(), (*kDist)(generator));
  }
}

/** @brief Template specialisation for the AntiHydrogen generator
 * @copydoc ParticleGenerator<PType>::generateParticles()
 */
template<> void ParticleGenerator<AntiHydrogen>::generateParticles() {
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
  } else {
    std::vector<float> kpieces { 1.0, (float) particlesConfig_->n() };
    std::vector<float> kweights { 1, 0 };
    kDist = new TriangleDistribution(kpieces, kweights);
  }

  generateSynchronousParticle(0.5 * acceleratorConfig_->x(),
                              0.5 * acceleratorConfig_->y(), 2.5*sectionWidth,
                              0, 0, 0);

  std::cout << "Generating " << particlesConfig_->nParticles()
            << " particles..." <<  std::endl;

  if (particlesConfig_->positionDist() == "normal") {
    generateNormDist(generator, sigmaV, particlesConfig_->vNormDist(), sectionWidth, particlesConfig_->distOffset(), kDist);
  } else if (particlesConfig_->positionDist() == "uniform") {
    generateUniformDist(generator, sigmaV, particlesConfig_->vNormDist(), sectionWidth, particlesConfig_->distOffset(), kDist);
  } else if (particlesConfig_->positionDist() == "full") {
    generateUniformDist(generator, sigmaV, false, sectionWidth, sectionWidth, kDist, true);
  }

  delete kDist;
  std::cout << std::endl;
}

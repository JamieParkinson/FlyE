#include "ParticleGenerator.h"

#include <random>

#include "PhysicalConstants.h"

template<typename PType>
ParticleGenerator<PType>::ParticleGenerator(
    std::shared_ptr<ParticlesConfig> particlesConfig,
    std::shared_ptr<AcceleratorConfig> acceleratorConfig)
    : particlesConfig_(particlesConfig),
      acceleratorConfig_(acceleratorConfig) {
}

template<typename PType>
template<class T_DistType>
int ParticleGenerator<PType>::generateK(IntegerDistribution<T_DistType> &dist, std::mt19937 generator) {
  return dist(generator);
}

template<> void ParticleGenerator<AntiHydrogen>::generateSynchronousParticle(int x, int y,
                                                                  int z, int vx,
                                                                  int vy,
                                                                  int vz) {
  particles_.emplace_back(x, y, z, vx, vy, vz, particlesConfig_->n(),
                          particlesConfig_->k());
}

template<> void ParticleGenerator<AntiHydrogen>::generateParticles() {
  float sigmaV = sqrt(
      2 * (particlesConfig_->temperature() * Physics::kb / Physics::FWHMfactor) / Physics::mH);  // Std dev of velocity
  float sectionWidth = Physics::N_IN_SECTION * acceleratorConfig_->z() / acceleratorConfig_->nElectrodes();  // Width of each section of 4 electrodes

  std::mt19937 generator(std::random_device());




}

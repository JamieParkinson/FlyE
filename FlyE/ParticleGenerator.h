#pragma once

#include <memory>
#include <vector>

#include "AntiHydrogen.h"
#include "SubConfig.h"
#include "IntegerDistribution.h"

template<typename PType>
class ParticleGenerator {
 protected:
  std::shared_ptr<ParticlesConfig> particlesConfig_;
  std::shared_ptr<AcceleratorConfig> acceleratorConfig_;
  std::vector<PType> particles_;

  void generateNormDist(std::mt19937 generator, float sigmaV, int sectionWidth, IntegerDistribution *kDist);

  void generateUniformDist(std::mt19937 generator, float sigmaV, int sectionWidth, IntegerDistribution *kDist);

 public:
  ParticleGenerator(std::shared_ptr<ParticlesConfig> particlesConfig, std::shared_ptr<AcceleratorConfig> acceleratorConfig);

  void generateSynchronousParticle(int x, int y, int z, int vx, int vy, int vz);

  void generateParticles();

  std::vector<PType>& getParticles();
};

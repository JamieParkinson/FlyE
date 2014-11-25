#include "ParticleGenerator.h"

#include <random>

#include "PhysicalConstants.h"

typedef std::normal_distribution<float> float_n_dist;
typedef std::uniform_real_distribution<float> float_u_dist;

template<typename PType>
ParticleGenerator<PType>::ParticleGenerator(
    std::shared_ptr<ParticlesConfig> particlesConfig,
    std::shared_ptr<AcceleratorConfig> acceleratorConfig)
    : particlesConfig_(particlesConfig),
      acceleratorConfig_(acceleratorConfig) {
}

template<> void ParticleGenerator<AntiHydrogen>::generateSynchronousParticle(
    int x, int y, int z, int vx, int vy, int vz) {
  particles_.emplace_back(x, y, z, vx, vy, vz, particlesConfig_->n(),
                          particlesConfig_->k());
}

template<> void ParticleGenerator<AntiHydrogen>::generateNormDist(
    mersenne_twister generator, float sigmaV, int sectionWidth,
    IntegerDistribution *kDist) {

  float_n_dist x_dist(acceleratorConfig_->x() / 2, particlesConfig_->sigmaX());  // Center at start of cylinder
  float_n_dist y_dist(acceleratorConfig_->y() / 2, particlesConfig_->sigmaY());
  float_n_dist z_dist(2.25 * sectionWidth, particlesConfig_->sigmaZ());  // Should mean most particles are in the cylinder (~0.27% are outside)
  float_n_dist v_dist(0, sigmaV);

  for (int i = 0; i < particlesConfig_->nParticles() - 1; ++i) {  // Generate particles
    particles_.emplace_back(x_dist(generator), y_dist(generator),
                            z_dist(generator), v_dist(generator),
                            v_dist(generator), v_dist(generator),
                            particlesConfig_->n(), (*kDist)(generator));
  }

}

template<> void ParticleGenerator<AntiHydrogen>::generateUniformDist(
    mersenne_twister generator, float sigmaV, int sectionWidth,
    IntegerDistribution *kDist) {
  float_u_dist uniform_dist(0, 1);
  float_n_dist muller_dist(0, 1);  // Using (a modified version of) the method described by Muller (1959) to generate spherical velocity dist.

  for (int i = 0; i < particlesConfig_->nParticles() - 1; ++i) {  // Generate particles
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
                            v_r * muller_factor * vz, particlesConfig_->n(), (*kDist)(generator));
  }
}

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
  } else {  // kDist == Triangle
    std::vector<float> kpieces { 0.0, (float) particlesConfig_->n() };
    std::vector<float> kweights { 1, 0 };
    kDist = new TriangleDistribution(kpieces, kweights);
  }

  if (particlesConfig_->normDist())
    generateNormDist(generator, sigmaV, sectionWidth, kDist);
  else
    generateUniformDist(generator, sigmaV, sectionWidth, kDist);
}

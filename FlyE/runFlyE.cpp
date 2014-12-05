/*
 * At this stage a sort of pseudocode for how I'd like things to look
 */

#include <iostream>
#include "FlyE.h"

int main(int argc, char* argv[]) {
  ConfigLoader myConfig("/home/jamie/git/FlyE/FlyE/flyE.conf");

  AcceleratorGeometry myAccelerator(myConfig.getAcceleratorConfig());
  myAccelerator.importElectrodes();

  ParticleGenerator<AntiHydrogen> myGenerator(myConfig.getParticlesConfig(),
                                              myConfig.getAcceleratorConfig());
  myGenerator.generateParticles();
  std::vector<AntiHydrogen> particles = myGenerator.getParticles();

  Simulator *mySimulator = new Simulator(myAccelerator, particles,
                                         myConfig.getSimulationConfig(),
                                         myConfig.getStorageConfig());

  mySimulator->run();

  SimulationNumbers stats = mySimulator->getBasicStats();
  std::cout << stats << std::endl;

  mySimulator->write("/home/jamie/FlyEfiles/out1.h5");

  ConfigLoader my2ndConfig("/home/jamie/git/FlyE/FlyE/flyE2.conf");

  std::cout << *my2ndConfig.getParticlesConfig() << std::endl;

  ParticleGenerator<AntiHydrogen> my2ndGenerator(
      my2ndConfig.getParticlesConfig(), myConfig.getAcceleratorConfig());
  my2ndGenerator.generateParticles();
  particles = my2ndGenerator.getParticles();

  delete mySimulator;
  mySimulator = new Simulator(myAccelerator, particles,
                              my2ndConfig.getSimulationConfig(),
                              my2ndConfig.getStorageConfig());

  mySimulator->run();

  SimulationNumbers stats2 = mySimulator->getBasicStats();
  std::cout << stats2 << std::endl;

  mySimulator->write("/home/jamie/FlyEfiles/out2.h5");

  delete mySimulator;
}

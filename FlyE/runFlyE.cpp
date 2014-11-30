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

  Simulator mySimulator(myAccelerator, particles,
                        myConfig.getSimulationConfig(),
                        myConfig.getStorageConfig());

  mySimulator.run();

  SimulationNumbers stats = mySimulator.getBasicStats();
  std::cout << stats << std::endl;

  mySimulator.write("/home/jamie/FlyEfiles/out.h5");
}

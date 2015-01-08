#include <iostream>
#include "FlyE.h"

int main(int argc, char* argv[]) {
  ConfigLoader myConfig1K("/home/ubuntu/config-files/flyE1K.conf");
  ConfigLoader myConfig100mK("/home/ubuntu/config-files/flyE100mK.conf");

  AcceleratorGeometry myAccelerator(myConfig1K.getAcceleratorConfig());  // Same geometry for both
  myAccelerator.importElectrodes();

  Simulator *mySimulator;

  ParticleGenerator<AntiHydrogen> generator1K(
      myConfig1K.getParticlesConfig(), myConfig1K.getAcceleratorConfig());
  ParticleGenerator<AntiHydrogen> generator100mK(
      myConfig100mK.getParticlesConfig(), myConfig100mK.getAcceleratorConfig());

  // 1K run
  mySimulator = new Simulator(myAccelerator, generator1K.getParticles(),
                              myConfig1K.getSimulationConfig(),
                              myConfig1K.getStorageConfig());
  mySimulator->run();
  SimulationNumbers stats1K = mySimulator->getBasicStats();
  std::cout << stats1K << std::endl;
  mySimulator->write("/home/ubuntu/1Kdata.h5");

  // 100mK run
  mySimulator = new Simulator(myAccelerator, generator100mK.getParticles(),
                              myConfig100mK.getSimulationConfig(),
                              myConfig100mK.getStorageConfig());
  mySimulator->run();
  SimulationNumbers stats100mK = mySimulator->getBasicStats();
  std::cout << stats100mK << std::endl;
  mySimulator->write("/home/ubuntu/100mKdata.h5");
}

#include <iostream>
#include "FlyE.h"

int main(int argc, char* argv[]) {
  ConfigLoader myConfig("/home/jamie/git/FlyE/FlyE/flyE.conf");

  AcceleratorGeometry myAccelerator(myConfig.getAcceleratorConfig());
  myAccelerator.importElectrodes();

  Simulator *mySimulator;

  for (float v = 145.0; v < 145.1 ; v += 1.0) {
    ParticleGenerator<AntiHydrogen> myGenerator(
        myConfig.getParticlesConfig(), myConfig.getAcceleratorConfig());
    myGenerator.generateParticles();
    std::vector<AntiHydrogen> particles = myGenerator.getParticles();

    std::shared_ptr<SimulationConfig> simConf = myConfig.getSimulationConfig();
    simConf->setMaxVoltage(v);

    mySimulator = new Simulator(myAccelerator, particles,
                                simConf,
                                myConfig.getStorageConfig());


    mySimulator->run();

    SimulationNumbers stats = mySimulator->getBasicStats();
    std::cout << stats << std::endl;

    mySimulator->write(
        "/home/jamie/FlyEfiles/out" + std::to_string(static_cast<int>(v))
            + ".h5");
  }
}

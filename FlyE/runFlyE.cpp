#include <iostream>
#include <algorithm>
#include "FlyE.h"

int main(int argc, char* argv[]) {
  std::string confDirectory = "/home/jamie/FlyEfiles/config-files/";
  std::string outDirectory = "/home/jamie/FlyEfiles/";

  // List of config file names
  std::vector<std::string> confNames =
      { "exp1K", "exp100mK", "trap1K", "trap100mK", "fullDist1K",
          "uniformStark", "triangleStark", "inglisTeller" };

  std::reverse(confNames.begin(), confNames.end());
  // Only load the geometry once
  ConfigLoader geometryLoader(confDirectory + confNames[0] + ".conf");
  AcceleratorGeometry accelerator(geometryLoader.getAcceleratorConfig());
  accelerator.importElectrodes();

  // Loop config files
  for (auto confName : confNames) {
    std::cout << "Running from config file: " << confName << std::endl;
    ConfigLoader loader(confDirectory + confName + ".conf");

    ParticleGenerator<AntiHydrogen> generator(loader.getParticlesConfig(),
                                              loader.getAcceleratorConfig());
    generator.generateParticles();

    Simulator simulator(accelerator, generator.getParticles(),
                        loader.getSimulationConfig(),
                        loader.getStorageConfig());

    simulator.run();
    SimulationNumbers stats = simulator.getBasicStats();
    std::cout << stats << std::endl;

    simulator.write(outDirectory + confName + ".h5"); // Enforce naming convention automatically

    std::cout << "*************************************************************************\n" << std::endl;
  }
}

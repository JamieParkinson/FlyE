/*
 * A class to read in and then contain the contents of a config file
 */
#include <string>
#include <memory>

#include "SubConfig.h"

class ConfigLoader {
 private:
  std::string configFilePath_;
  std::unique_ptr<AcceleratorConfig> accelConf_;
  std::unique_ptr<SimulationConfig> simConf_;
  std::unique_ptr<ParticlesConfig> particlesConf_;
  std::unique_ptr<StorageConfig> storageConf_;

 public:
  // Constructs the SimConfig object from the file located at configFilePath
  ConfigLoader(std::string configFilePath);

  std::unique_ptr<AcceleratorConfig> getAcceleratorConfig();
  std::unique_ptr<ParticlesConfig> getParticlesConfig();
  std::unique_ptr<SimulationConfig> getSimulationConfig();
  std::unique_ptr<StorageConfig> getStorageConfig();
};

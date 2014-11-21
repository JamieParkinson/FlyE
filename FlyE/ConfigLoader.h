/*
 * A class to read in and then contain the contents of a config file
 */
#include <string>
#include <libconfig.h++>

#include "SubConfig.h"

class ConfigLoader {
 private:
  std::string configFilePath_ = "";
  libconfig::Config config_;
  AcceleratorConfig *accelConf_;
  SimulationConfig *simConf_;
  ParticlesConfig *particlesConf_;
  StorageConfig *storageConf_;

 public:
  // Constructs the SimConfig object from the file located at configFilePath
  ConfigLoader(std::string configFilePath);

  AcceleratorConfig* getAcceleratorConfig();
  ParticlesConfig* getParticlesConfig();
  SimulationConfig* getSimulationConfig();
  StorageConfig* getStorageConfig();
};

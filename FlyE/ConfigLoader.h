/*
 * A class to read in and then contain the contents of a config file
 */
#include <string>
#include "inih/cpp/INIReader.h"

#include "SubConfig.h"

class ConfigLoader {
 private:
  std::string configFilePath_;
  INIReader *myReader_;
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

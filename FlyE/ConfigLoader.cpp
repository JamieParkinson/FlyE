
#include "ConfigLoader.h"

#include "inih/cpp/INIReader.h"

#include "PhysicalConstants.h"

ConfigLoader::ConfigLoader(std::string configFilePath)
    : configFilePath_(configFilePath) {
  INIReader myReader_(configFilePath);

  accelConf_ = std::make_shared<AcceleratorConfig>(myReader_);
  particlesConf_ = std::make_shared<ParticlesConfig>(myReader_);
  simConf_ = std::make_shared<SimulationConfig>(myReader_);
  storageConf_ = std::make_shared<StorageConfig>(myReader_);
}

std::shared_ptr<AcceleratorConfig> ConfigLoader::getAcceleratorConfig() {
  return accelConf_;
}

std::shared_ptr<ParticlesConfig> ConfigLoader::getParticlesConfig() {
  return particlesConf_;
}

std::shared_ptr<SimulationConfig> ConfigLoader::getSimulationConfig() {
  return simConf_;
}

std::shared_ptr<StorageConfig> ConfigLoader::getStorageConfig() {
  return storageConf_;
}

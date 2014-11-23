
#include "ConfigLoader.h"

#include "inih/cpp/INIReader.h"

#include "PhysicalConstants.h"
#include "Containers.h"

ConfigLoader::ConfigLoader(std::string configFilePath)
    : configFilePath_(configFilePath) {
  INIReader myReader_(configFilePath);

  accelConf_ = std::shared_ptr<AcceleratorConfig>(new AcceleratorConfig(myReader_));
  particlesConf_ = std::shared_ptr<ParticlesConfig>(new ParticlesConfig(myReader_));
  simConf_ = std::shared_ptr<SimulationConfig>(new SimulationConfig(myReader_));
  storageConf_ = std::shared_ptr<StorageConfig>(new StorageConfig(myReader_));
}

std::shared_ptr<AcceleratorConfig> ConfigLoader::getAcceleratorConfig() {
  return std::move(accelConf_);
}

std::shared_ptr<ParticlesConfig> ConfigLoader::getParticlesConfig() {
  return std::move(particlesConf_);
}

std::shared_ptr<SimulationConfig> ConfigLoader::getSimulationConfig() {
  return std::move(simConf_);
}

std::shared_ptr<StorageConfig> ConfigLoader::getStorageConfig() {
  return std::move(storageConf_);
}

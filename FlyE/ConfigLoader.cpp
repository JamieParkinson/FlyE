#include "ConfigLoader.h"

#include "inih/cpp/INIReader.h"

#include "PhysicalConstants.h"
#include "Containers.h"

ConfigLoader::ConfigLoader(std::string configFilePath)
    : configFilePath_(configFilePath) {
  INIReader myReader_(configFilePath);

  accelConf_ = std::unique_ptr<AcceleratorConfig>(new AcceleratorConfig(myReader_));
  particlesConf_ = std::unique_ptr<ParticlesConfig>(new ParticlesConfig(myReader_));
  simConf_ = std::unique_ptr<SimulationConfig>(new SimulationConfig(myReader_));
  storageConf_ = std::unique_ptr<StorageConfig>(new StorageConfig(myReader_));
}

std::unique_ptr<AcceleratorConfig> ConfigLoader::getAcceleratorConfig() {
  return std::move(accelConf_);
}

std::unique_ptr<ParticlesConfig> ConfigLoader::getParticlesConfig() {
  return std::move(particlesConf_);
}

std::unique_ptr<SimulationConfig> ConfigLoader::getSimulationConfig() {
  return std::move(simConf_);
}

std::unique_ptr<StorageConfig> ConfigLoader::getStorageConfig() {
  return std::move(storageConf_);
}

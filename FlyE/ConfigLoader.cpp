#include "ConfigLoader.h"

#include "PhysicalConstants.h"
#include "Containers.h"

ConfigLoader::ConfigLoader(std::string configFilePath)
    : configFilePath_(configFilePath) {
  myReader_ = new INIReader(configFilePath);

  accelConf_ = new AcceleratorConfig(myReader_);
  particlesConf_ = new ParticlesConfig(myReader_);
  simConf_ = new SimulationConfig(myReader_);
  storageConf_ = new StorageConfig(myReader_);

  delete myReader_;
}

AcceleratorConfig* ConfigLoader::getAcceleratorConfig() {
  return accelConf_;
}

ParticlesConfig* ConfigLoader::getParticlesConfig() {
  return particlesConf_;
}

SimulationConfig* ConfigLoader::getSimulationConfig() {
  return simConf_;
}

StorageConfig* ConfigLoader::getStorageConfig() {
  return storageConf_;
}

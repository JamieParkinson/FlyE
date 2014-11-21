#include "ConfigLoader.h"

#include "PhysicalConstants.h"
#include "Containers.h"

ConfigLoader::ConfigLoader(std::string configFilePath)
    : configFilePath_(configFilePath) {
  config_.readFile(configFilePath.c_str());
  libconfig::Setting &settings = config_.getRoot();

  for (int i = 0; i < settings.getLength(); ++i) {
    libconfig::Setting &setting = settings[i];
    std::string name(setting.getName(), 11);

    SubConfig *subConf = new SubConfig();
    if (name == "accelerator") {
      subConf = new AcceleratorConfig();
      accelConf_ = static_cast<AcceleratorConfig*>(subConf);
    } else if (name == "simulation") {
      subConf = new SimulationConfig();
      simConf_ = static_cast<SimulationConfig*>(subConf);
    } else if (name == "particles") {
      subConf = new ParticlesConfig();
      particlesConf_ = static_cast<ParticlesConfig*>(subConf);
    } else if (name == "storage") {
      subConf = new StorageConfig();
      storageConf_ = static_cast<StorageConfig*>(subConf);
    }

    subConf->setRoot(setting);
    subConf->populate();
  }
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

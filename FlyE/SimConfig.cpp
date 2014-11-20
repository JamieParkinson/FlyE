#include "SimConfig.h"
#include "PhysicalConstants.h"
#include "Containers.h"

SimConfig::SimConfig(std::string configFilePath) : configFilePath_(configFilePath) {
  configFromPath(configFilePath);
  calculateAdditionalVars();
}

void SimConfig::configFromPath(std::string path) {
  config_.readFile(path.c_str());
}

void SimConfig::calculateAdditionalVars() {
  libconfig::Setting &dimensions = config_.lookup("espace.dimensions");
  dimensions[0] -= 2;
  dimensions[1] -= 2;
  dimensions[2] -= 2;

  libconfig::Setting &espace = config_.lookup("espace");
  espace.add("space_size", libconfig::Setting::TypeInt) =  dimensions[0]*dimensions[1]*dimensions[2]*Physics::N_DIMENSIONS;
}

template<typename T>
T SimConfig::getConfigVariable(std::string variableName) {
  return *config_.lookup(variableName);
}


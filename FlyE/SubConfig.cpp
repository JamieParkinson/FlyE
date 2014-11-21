#include <libconfig.h++>

#include "SubConfig.h"

void SubConfig::setRoot(libconfig::Setting &root) {
  myRoot_ = &root;
}

std::string SubConfig::toString() {
  std::string str = "Config information for " + myRoot_.getName();

  for (int i = 0; i < myRoot_.getLength(); ++i) {
    str.append(myRoot_[i].getName());
    str.append(": ");
    str.append(static_cast<std::string>(myRoot_[i]));
    str.append("\n");
  }

  return str;
}

void AcceleratorConfig::populate() {
  datDirectory_ = myRoot_["dat_directory"];
  nElectrodes_ = myRoot_["n_electrodes"];
  PAname_ = myRoot_["pa_name"];
  x_ = myRoot_["dimensions"]["x"];
  y_ = myRoot_["dimensions"]["y"];
  z_ = myRoot_["dimensions"]["z"];
}

const std::string& AcceleratorConfig::datDirectory() const {
  return datDirectory_;
}

int AcceleratorConfig::nElectrodes() const {
  return nElectrodes_;
}

const std::string& AcceleratorConfig::PAname() const {
  return PAname_;
}

int AcceleratorConfig::x() const {
  return x_;
}

int AcceleratorConfig::y() const {
  return y_;
}

int AcceleratorConfig::z() const {
  return z_;
}

void SimulationConfig::populate() {
  accelerationScheme_ = myRoot_["accel_scheme"];
  duration_ = myRoot_["duration"];
  inglisTeller_ = myRoot_["inglis_teller"];
  maxVoltage_ = myRoot_["max_voltage"];
  nParticles_ = myRoot_["n_particles"];
  targetVel_ = myRoot_["target_vel"];
  timeStep_ = myRoot_["time_step"];
}

const std::string& SimulationConfig::accelerationScheme() const {
  return accelerationScheme_;
}

float SimulationConfig::duration() const {
  return duration_;
}

bool SimulationConfig::inglisTeller() const {
  return inglisTeller_;
}

float SimulationConfig::maxVoltage() const {
  return maxVoltage_;
}

int SimulationConfig::nParticles() const {
  return nParticles_;
}

float SimulationConfig::targetVel() const {
  return targetVel_;
}

float SimulationConfig::timeStep() const {
  return timeStep_;
}

void ParticlesConfig::populate() {
  k_ = myRoot_["k"];
  kDist_ = myRoot_["k_dist"];
  n_ = myRoot_["n"];
  normDist_ = myRoot_["norm_dist"];
  sigmaX_ = myRoot_["sigma"]["x"];
  sigmaY_ = myRoot_["sigma"]["y"];
  sigmaZ_ = myRoot_["sigma"]["z"];
  temperature_ = myRoot_["temperature"];
}

int ParticlesConfig::k() const {
  return k_;
}

const std::string& ParticlesConfig::kDist() const {
  return kDist_;
}

int ParticlesConfig::n() const {
  return n_;
}

bool ParticlesConfig::normDist() const {
  return normDist_;
}

float ParticlesConfig::sigmaX() const {
  return sigmaX_;
}

float ParticlesConfig::sigmaY() const {
  return sigmaY_;
}

float ParticlesConfig::sigmaZ() const {
  return sigmaZ_;
}

float ParticlesConfig::temperature() const {
  return temperature_;
}

void StorageConfig::populate() {
  outDir_ = myRoot_["output_dir"];
  storeCollisions_ = myRoot_["store_collisions"];
  storeTrajectories_ = myRoot_["store_trajectories"];
}

const std::string& StorageConfig::outDir() const {
  return outDir_;
}

bool StorageConfig::storeTrajectories() const {
  return storeTrajectories_;
}

bool StorageConfig::storeCollisions() const {
  return storeCollisions_;
}

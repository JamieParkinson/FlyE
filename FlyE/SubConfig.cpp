#include <libconfig.h++>

#include "SubConfig.h"

SubConfig::SubConfig(INIReader *reader) {
  populate(reader);
}

AcceleratorConfig::AcceleratorConfig(INIReader *reader) : SubConfig(reader) {}

void AcceleratorConfig::populate(INIReader *reader) {
  datDirectory_ = reader->Get("accelerator", "dat_directory", "~");
  nElectrodes_ = reader->GetInteger("accelerator", "n_electrodes", 36);
  PAname_ = reader->Get("accelerator", "pa_name", "cylinder");
  x_ = reader->GetInteger("accelerator", "x", 54) - 2;
  y_ = reader->GetInteger("accelerator", "y", 54) - 2;
  z_ = reader->GetInteger("accelerator", "z", 200) - 2;
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

void SimulationConfig::populate(INIReader *reader) {
  accelerationScheme_ = reader->Get("simulation", "accel_scheme", "trap");
  duration_ = (float) reader->GetReal("simulation", "duration", 6e-4);
  inglisTeller_ = reader->GetBoolean("simulation", "inglis_teller", false);
  maxVoltage_ = (float) reader->GetReal("simulation", "max_voltage", 100);
  nParticles_ = reader->GetInteger("simulation", "n_particles", 50000);
  targetVel_ = (float) reader->GetReal("simulation", "target_vel", 500);
  timeStep_ = (float) reader->GetReal("simulation", "time_step", 1e-6);
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

void ParticlesConfig::populate(INIReader *reader) {
  k_ = reader->GetInteger("particles", "k", 20);
  kDist_ = reader->Get("particles", "k_dist", "single");
  n_ = reader->GetInteger("particles", "n", 25);
  normDist_ = reader->GetBoolean("particles", "norm_dist", false);
  sigmaX_ = (float) reader->GetReal("particles", "sigma_x", 0.5);
  sigmaY_ = (float) reader->GetReal("particles", "sigma_y", 0.5);
  sigmaZ_ = (float) reader->GetReal("particles", "sigma_z", 5);
  temperature_ = (float) reader->GetReal("particles", "temperature", 1.0);
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

void StorageConfig::populate(INIReader *reader) {
  outDir_ = reader->Get("storage", "output_dir", "~");
  storeCollisions_ = reader->GetBoolean("storage", "store_collisions", true);
  storeTrajectories_ = reader->GetBoolean("storage", "store_trajectories", true);
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

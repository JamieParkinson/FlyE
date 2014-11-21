#include <sstream>

#include "SubConfig.h"

SubConfig::~SubConfig() {}

AcceleratorConfig::AcceleratorConfig(INIReader &reader) {
  populate(reader);
}

void AcceleratorConfig::populate(INIReader &reader) {
  datDirectory_ = reader.Get("accelerator", "dat_directory", "~");
  nElectrodes_ = reader.GetInteger("accelerator", "n_electrodes", 36);
  PAname_ = reader.Get("accelerator", "pa_name", "cylinder");
  x_ = reader.GetInteger("accelerator", "x", 54) - 2;
  y_ = reader.GetInteger("accelerator", "y", 54) - 2;
  z_ = reader.GetInteger("accelerator", "z", 200) - 2;
}

std::string AcceleratorConfig::toString() {
  std::stringstream str;

  str << "Accelerator Config: \n";
  str << ".dat file directory: " << datDirectory_ << "\n";
  str << "PA file prefix: " << PAname_ << "\n";
  str << "Number of electrodes: " << nElectrodes_ << "\n";
  str << "Dimensions (x, y, z): (" << x_ << ", " << y_ << ", " << z_ << ")";

  return str.str();
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

SimulationConfig::SimulationConfig(INIReader &reader) {
  populate(reader);
}

void SimulationConfig::populate(INIReader &reader) {
  accelerationScheme_ = reader.Get("simulation", "accel_scheme", "trap");
  duration_ = (float) reader.GetReal("simulation", "duration", 6e-4);
  inglisTeller_ = reader.GetBoolean("simulation", "inglis_teller", false);
  maxVoltage_ = (float) reader.GetReal("simulation", "max_voltage", 100);
  targetVel_ = (float) reader.GetReal("simulation", "target_vel", 500);
  timeStep_ = (float) reader.GetReal("simulation", "time_step", 1e-6);
}

std::string SimulationConfig::toString() {
  std::stringstream str;

  str << "Simulation Config: \n";
  str << "Time step: " << timeStep_ << "\n";
  str << "Duration: " << duration_ << "\n";
  str << "Acceleration scheme: " << accelerationScheme_ << "\n";
  str << "Max voltage: " << maxVoltage_ << "\n";
  str << "Target velocity: " << targetVel_ << "\n";
  str << (inglisTeller_) ? "Using Inglis-Teller limit" : "Not using Inglis-Teller limit";

  return str.str();
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

float SimulationConfig::targetVel() const {
  return targetVel_;
}

float SimulationConfig::timeStep() const {
  return timeStep_;
}

ParticlesConfig::ParticlesConfig(INIReader &reader) {
  populate(reader);
}

void ParticlesConfig::populate(INIReader &reader) {
  nParticles_ = reader.GetInteger("simulation", "n_particles", 50000);
  k_ = reader.GetInteger("particles", "k", 20);
  kDist_ = reader.Get("particles", "k_dist", "single");
  n_ = reader.GetInteger("particles", "n", 25);
  normDist_ = reader.GetBoolean("particles", "norm_dist", false);
  sigmaX_ = (float) reader.GetReal("particles", "sigma_x", 0.5);
  sigmaY_ = (float) reader.GetReal("particles", "sigma_y", 0.5);
  sigmaZ_ = (float) reader.GetReal("particles", "sigma_z", 5);
  temperature_ = (float) reader.GetReal("particles", "temperature", 1.0);
}

std::string ParticlesConfig::toString() {
  std::stringstream str;

  str << "Particles Config: \n";
  str << "Number of particles: " << nParticles_ << "\n";
  str << "Temperature: " << temperature_ << "\n";

  if (normDist_) {
    str << "Using normal distribution of particles\n";
    str << "\t Sigma(x): " << sigmaX_ << "\n";
    str << "\t Sigma(y): " << sigmaY_ << "\n";
    str << "\t Sigma(z): " << sigmaZ_ << "\n";
  }

  str << "Distribution of k: " << kDist_ << "\n";

  if (kDist_ == "single") {
    str << "\tk: " << k_ << "\n";
  }

  str << "n: " << n_;

  return str.str();
}

int ParticlesConfig::nParticles() const {
  return nParticles_;
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

StorageConfig::StorageConfig(INIReader &reader) {
  populate(reader);
}

void StorageConfig::populate(INIReader &reader) {
  outDir_ = reader.Get("storage", "output_dir", "~");
  storeCollisions_ = reader.GetBoolean("storage", "store_collisions", true);
  storeTrajectories_ = reader.GetBoolean("storage", "store_trajectories", true);
}

std::string StorageConfig::toString() {
  std::stringstream str;

  str << "Storage Config: \n";
  str << "Output directory: " << outDir_ << "\n";
  str << (storeCollisions_) ? "Storing collisions\n" : "Not storing collisions\n";
  str << (storeTrajectories_) ? "Storing trajectories" : "Not storing trajectories";

  return str.str();
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

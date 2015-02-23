#include <sstream>
#include <iostream>

#include "SubConfig.h"

SubConfig::~SubConfig() {
}

std::ostream& operator <<(std::ostream &out, SubConfig &subConf) {
  subConf.printOn(out);
  return out;
}

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

void AcceleratorConfig::printOn(std::ostream &out) {
  std::stringstream str;

  str << "Accelerator Config: \n";
  str << ".dat file directory: " << datDirectory_ << "\n";
  str << "PA file prefix: " << PAname_ << "\n";
  str << "Number of electrodes: " << nElectrodes_ << "\n";
  str << "Dimensions (x, y, z): (" << x_ << ", " << y_ << ", " << z_ << ")";

  out << str.str();
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
  if (accelerationScheme_ != "exponential" && accelerationScheme_ != "trap"
      && accelerationScheme_ != "instantaneous") {
    try {
      throw "Invalid value for acceleration scheme!";
    } catch (const char* e) {
      std::cout << e << std::endl;
      std::terminate();
    }
  }
  trapShakeTime_ = (float) reader.GetReal("simulation", "trap_shake_time", 1e-6);

  duration_ = (float) reader.GetReal("simulation", "duration", 6e-4);
  inglisTeller_ = reader.GetBoolean("simulation", "inglis_teller", false);
  maxVoltage_ = (float) reader.GetReal("simulation", "max_voltage", 100);
  targetVel_ = (float) reader.GetReal("simulation", "target_vel", 500);
  timeStep_ = (float) reader.GetReal("simulation", "time_step", 1e-6);
}

void SimulationConfig::printOn(std::ostream &out) {
  std::stringstream str;

  str << "Simulation Config: \n";
  str << "Time step: " << timeStep_ << "\n";
  str << "Duration: " << duration_ << "\n";
  str << "Acceleration scheme: " << accelerationScheme_ << "\n";
  str << "Trap shake time: " << trapShakeTime_ << "\n";
  str << "Max voltage: " << maxVoltage_ << "\n";
  str << "Target velocity: " << targetVel_ << "\n";

#pragma GCC diagnostic push // Makes g++ shut up about these ternary operators supposedly having no effect
#pragma GCC diagnostic ignored "-Wunused-value"
  str << (inglisTeller_) ?
      "Using Inglis-Teller limit" : "Not using Inglis-Teller limit";
#pragma GCC diagnostic pop

  out << str.str();
}

const std::string& SimulationConfig::accelerationScheme() const {
  return accelerationScheme_;
}

float SimulationConfig::trapShakeTime() const {
  return trapShakeTime_;
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

void SimulationConfig::setAccelerationScheme(const std::string &scheme) {
  accelerationScheme_ = scheme;
}

void SimulationConfig::setTrapShakeTime(float shakeTime) {
  trapShakeTime_ = shakeTime;
}

void SimulationConfig::setDuration(float duration) {
  duration_ = duration;
}

void SimulationConfig::setInglisTeller(bool inglisTeller) {
  inglisTeller_ = inglisTeller;
}

void SimulationConfig::setMaxVoltage(float maxVoltage) {
  maxVoltage_ = maxVoltage;
}

void SimulationConfig::setTargetVel(float targetVel) {
  targetVel_ = targetVel;
}

void SimulationConfig::setTimeStep(float timeStep) {
  timeStep_ = timeStep;
}

ParticlesConfig::ParticlesConfig(INIReader &reader) {
  populate(reader);
}

void ParticlesConfig::populate(INIReader &reader) {
  nParticles_ = static_cast<int>(reader.GetReal("particles", "n_particles",
                                                50000));
  k_ = reader.GetInteger("particles", "k", 20);
  kDist_ = reader.Get("particles", "k_dist", "single");
  if (kDist_ != "single" && kDist_ != "uniform" && kDist_ != "triangle") {
    try {
      throw "Invalid k distribution value!";
    } catch (const char* e) {
      std::cout << e << std::endl;
      std::terminate();
    }
  }

  n_ = reader.GetInteger("particles", "n", 25);
  positionDist_ = reader.Get("particles", "position_dist", "full");
  distRadius_ = (float) reader.GetReal("particles", "dist_radius", 0.5);
  distLength_ = (float) reader.GetReal("particles", "dist_length", 5);
  distOffset_ = (float) reader.GetReal("particles", "dist_offset", 22);
  temperature_ = (float) reader.GetReal("particles", "temperature", 1.0);
}

void ParticlesConfig::printOn(std::ostream &out) {
  std::stringstream str;

  str << "Particles Config: \n";
  str << "Number of particles: " << nParticles_ << "\n";
  str << "Temperature: " << temperature_ << "\n";

  str << "Type of particle distribution: " << positionDist_ << "\n";

  std::string dimensionString = (positionDist_ != "normal") ? "standard deviation" : "size";
  str << "Radial " << dimensionString << "of particle distribution: " << distRadius_ << "\n";
  str << "Axial " << dimensionString << "of particle distribution: " << distLength_ << "\n";
  str << "Offset from start of accelerator: " << distOffset_ << "\n";

  str << "Distribution of k: " << kDist_ << "\n";

  if (kDist_ == "single") {
    str << "\tk: " << k_ << "\n";
  }

  str << "n: " << n_;

  out << str.str();
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

std::string ParticlesConfig::positionDist() const {
  return positionDist_;
}

float ParticlesConfig::distRadius() const {
  return distRadius_;
}

float ParticlesConfig::distLength() const {
  return distLength_;
}

float ParticlesConfig::distOffset() const {
  return distOffset_;
}

bool ParticlesConfig::vNormDist() const {
  return vNormDist_;
}

float ParticlesConfig::temperature() const {
  return temperature_;
}

StorageConfig::StorageConfig(INIReader &reader) {
  populate(reader);
}

void StorageConfig::populate(INIReader &reader) {
  storeCollisions_ = reader.GetBoolean("storage", "store_collisions", true);
  storeTrajectories_ = reader.GetBoolean("storage", "store_trajectories", true);
  compression_ = reader.GetInteger("storage", "compression", 0);
}

void StorageConfig::printOn(std::ostream &out) {
  std::stringstream str;

  str << "Storage Config: \n";

#pragma GCC diagnostic push // Makes g++ shut up about these ternary operators supposedly having no effect
#pragma GCC diagnostic ignored "-Wunused-value"
  str << (storeCollisions_) ?
      "Storing collisions\n" : "Not storing collisions\n";
  str << (storeTrajectories_) ?
      "Storing trajectories" : "Not storing trajectories";
#pragma GCC diagnostic pop

  out << str.str();
}

bool StorageConfig::storeTrajectories() const {
  return storeTrajectories_;
}

bool StorageConfig::storeCollisions() const {
  return storeCollisions_;
}

int StorageConfig::compression() const {
  return compression_;
}

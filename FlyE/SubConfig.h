#pragma once

#include "inih/cpp/INIReader.h"

class SubConfig {
 public:
  virtual ~SubConfig();
  virtual void populate(INIReader &reader) = 0;
  virtual std::string toString() = 0;
};

class AcceleratorConfig : public SubConfig {
 protected:
  int nElectrodes_;
  int x_, y_, z_;
  std::string datDirectory_;
  std::string PAname_;

 public:
  AcceleratorConfig(INIReader &reader);
  void populate(INIReader &reader);
  std::string toString();
  // Getters
  const std::string& datDirectory() const;
  int nElectrodes() const;
  const std::string& PAname() const;
  int x() const;
  int y() const;
  int z() const;
};

class SimulationConfig : public SubConfig {
 protected:
  float timeStep_, duration_;
  int nParticles_;
  float maxVoltage_;
  float targetVel_;
  std::string accelerationScheme_;
  bool inglisTeller_;

 public:
  SimulationConfig(INIReader &reader);
  void populate(INIReader &reader);
  std::string toString();
  // Getters
  const std::string& accelerationScheme() const;
  float duration() const;
  bool inglisTeller() const;
  float maxVoltage() const;
  int nParticles() const;
  float targetVel() const;
  float timeStep() const;
};

class ParticlesConfig : public SubConfig {
 protected:
  bool normDist_;
  float sigmaX_, sigmaY_, sigmaZ_;
  float temperature_;
  std::string kDist_;
  int n_, k_;

 public:
  ParticlesConfig(INIReader &reader);
  void populate(INIReader &reader);
  std::string toString();
  // Getters
  int k() const;
  const std::string& kDist() const;
  int n() const;
  bool normDist() const;
  float sigmaX() const;
  float sigmaY() const;
  float sigmaZ() const;
  float temperature() const;
};

class StorageConfig : public SubConfig {
 protected:
  std::string outDir_;
  bool storeTrajectories_, storeCollisions_;

 public:
  StorageConfig(INIReader &reader);
  void populate(INIReader &reader);
  std::string toString();
  const std::string& outDir() const;
  bool storeTrajectories() const;
  bool storeCollisions() const;
};

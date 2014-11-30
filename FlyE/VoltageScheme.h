#pragma once

#include <vector>

class Particle;

class VoltageScheme {
 protected:
  float maxVoltage_;
  int nElectrodes_;
  int sectionWidth_;
  std::vector<float> voltages_;
  float timeStep_;

 public:
  VoltageScheme(float maxVoltage, int nElectrodes, int sectionWidth, float timeStep);

  virtual std::vector<float> getVoltages(int t) = 0;

  virtual std::vector<float> getInitialVoltages() = 0;

  virtual bool isActive(int t) = 0;

  virtual ~VoltageScheme();
};

class SynchronousParticleScheme : public VoltageScheme {
 protected:
  int section_ = 2;
  Particle &synchronousParticle_;

 public:
  SynchronousParticleScheme(Particle &synchronousParticle, float maxVoltage, int nElectrodes, int sectionWidth, float timeStep);

  virtual std::vector<float> getVoltages(int t) = 0;

  virtual bool isActive(int t) = 0;

  std::vector<float> getInitialVoltages();
};

class InstantaneousScheme : public SynchronousParticleScheme {
 public:
  InstantaneousScheme(Particle &synchronousParticle, float maxVoltage, int nElectrodes, int sectionWidth, float timeStep);

  std::vector<float> getVoltages(int t);

  bool isActive(int t);
};

class ExponentialScheme : public SynchronousParticleScheme {
 protected:
  static constexpr int timeConstant_ = 1000.0;
  float deltaT_ = 0.0;
  float startRampTime_ = 0.0;

 public:
  ExponentialScheme(Particle &synchronousParticle, float maxVoltage, int nElectrodes, int sectionWidth, float timeStep);

  std::vector<float> getVoltages(int t);

  bool isActive(int t);
};

class MovingTrapScheme : public VoltageScheme {
 protected:
  static constexpr int trapWidth_ = 6;
  float targetVel_;
  float offTime_;
  static constexpr int nOscillations_ = 3; // Experimental

  static float frequency(int k, float V);

 public:
  // TODO remember to deal with OffTime
  MovingTrapScheme(float maxVoltage, int nElectrodes, int sectionWidth, float timeStep, float targetVel, int k);

  std::vector<float> getInitialVoltages();

  std::vector<float> getVoltages(int t);

  bool isActive(int t);
};

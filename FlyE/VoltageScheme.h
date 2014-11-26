#pragma once

#include <vector>

class Particle;

class VoltageScheme {
 protected:
  float maxVoltage_;
  bool isActive_ = true;
  int nElectrodes_;
  int sectionWidth_;
  std::vector<float> voltages_;
  float timeStep_;

 public:
  VoltageScheme(float maxVoltage, int nElectrodes, int sectionWidth, float timeStep);

  virtual std::vector<float> getVoltages(int t) = 0;

  virtual bool isActive(int t) = 0;

  virtual ~VoltageScheme();
};

class SynchronousParticleScheme : public VoltageScheme {
 protected:
  int section_ = 2;
  Particle &synchronousParticle_;

 public:
  SynchronousParticleScheme(Particle &synchronousParticle, float maxVoltage, int nElectrodes, int sectionWidth, float timeStep);
};

class InstantaneousScheme : public SynchronousParticleScheme {
 public:
  InstantaneousScheme(Particle &synchronousParticle, float maxVoltage, int nElectrodes, int sectionWidth, float timeStep);

  std::vector<float> getVoltages(int t);

  bool isActive(int t);
};

class ExponentialScheme : public SynchronousParticleScheme {
 protected:
  int timeConstant_ = 1000.0;
  float deltaT_ = 0.0;
  float startRampTime_ = 0.0;

 public:
  ExponentialScheme(Particle &synchronousParticle, float maxVoltage, int nElectrodes, int sectionWidth, float timeStep);

  std::vector<float> getVoltages(int t);

  bool isActive(int t);
};

class MovingTrapScheme : public VoltageScheme {

};

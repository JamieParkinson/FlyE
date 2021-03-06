#include "VoltageScheme.h"

#include <algorithm>
#include <iostream>

#include "Particle.h"
#include "PhysicalConstants.h"

VoltageScheme::~VoltageScheme() {
}

VoltageScheme::VoltageScheme(float maxVoltage, int nElectrodes,
                             int sectionWidth, float timeStep)
    : maxVoltage_(maxVoltage),
      nElectrodes_(nElectrodes),
      sectionWidth_(sectionWidth),
      voltages_(nElectrodes, 0.0),
      timeStep_(timeStep) {
}

//Synchronous
SynchronousParticleScheme::SynchronousParticleScheme(
    Particle &synchronousParticle, float maxVoltage, int nElectrodes,
    int sectionWidth, float timeStep)
    : VoltageScheme(maxVoltage, nElectrodes, sectionWidth, timeStep),
      synchronousParticle_(synchronousParticle) {
}

std::vector<float> SynchronousParticleScheme::getInitialVoltages() {
  std::fill(voltages_.begin(), voltages_.begin() + Physics::N_IN_SECTION,
            maxVoltage_);

  return voltages_;
}

// Instantaneous
InstantaneousScheme::InstantaneousScheme(Particle &synchronousParticle,
                                         float maxVoltage, int nElectrodes,
                                         int sectionWidth, float timeStep)
    : SynchronousParticleScheme(synchronousParticle, maxVoltage, nElectrodes,
                                sectionWidth, timeStep) {
}

bool InstantaneousScheme::isActive(int t) {
  return (synchronousParticle_.getLocDim<2>() >= section_ * sectionWidth_
      && section_ < nElectrodes_ / Physics::N_IN_SECTION);
}

std::vector<float> InstantaneousScheme::getVoltages(int t) {
  std::fill(voltages_.begin() + Physics::N_IN_SECTION * (section_ - 1),
            voltages_.begin() + Physics::N_IN_SECTION * section_, maxVoltage_);

  ++section_;

  return voltages_;
}

// Exponential
ExponentialScheme::ExponentialScheme(Particle &synchronousParticle,
                                     float maxVoltage, int nElectrodes,
                                     int sectionWidth, float timeStep)
    : SynchronousParticleScheme(synchronousParticle, maxVoltage, nElectrodes,
                                sectionWidth, timeStep) {
}

bool ExponentialScheme::isActive(int t) {
  return (t * timeStep_ <= (startRampTime_ + deltaT_));
}

std::vector<float> ExponentialScheme::getVoltages(int t) {
  float tSeconds = timeStep_ * t;

  if (synchronousParticle_.getLocDim<2>() >= section_ * sectionWidth_
      && section_ < nElectrodes_ / Physics::N_IN_SECTION) {  // Use synchronous particle to switch electrodes, if it's past this section but not past the end

    float syncAccel = (synchronousParticle_.getVelDim<2>()
        - synchronousParticle_.recallVel(startRampTime_ / timeStep_, 2))
        / (tSeconds - startRampTime_);  // Extrapolate particle accel.
    startRampTime_ = tSeconds;  // Update the last crossing time

    // Increase v until roughly when the next electrode is switched on: deltaT is the time the voltage increases for. Just a solution of const. accel. equations.
    deltaT_ = (-synchronousParticle_.getVelDim<2>()
        + sqrt(
            pow(synchronousParticle_.getVelDim<2>(), 2)
                + 2 * syncAccel * sectionWidth_ / Physics::MM_M_FACTOR))
        / syncAccel;

    section_++;
  }

  float voltage = maxVoltage_
      * (exp(timeConstant_ * (tSeconds - startRampTime_)) - 1)
      / (exp(timeConstant_ * deltaT_) - 1);

  std::fill(voltages_.begin() + Physics::N_IN_SECTION * (section_ - 2),
            voltages_.begin() + Physics::N_IN_SECTION * (section_ - 1),
            voltage);

  return voltages_;
}

MovingTrapScheme::MovingTrapScheme(float maxVoltage, int nElectrodes,
                                   int sectionWidth, float timeStep,
                                   float shakeTime, float endTime,
                                   float targetVel, int k)
    : VoltageScheme(maxVoltage, nElectrodes, sectionWidth, timeStep),
      targetVel_(targetVel),
      shakeTime_(shakeTime) {

  float constantVOffTime = 2
      * (sectionWidth * (nElectrodes / Physics::N_IN_SECTION - 3.5))
      / (targetVel_ * Physics::MM_M_FACTOR);
  if (constantVOffTime > endTime)
    std::cout
        << "WARNING: You should run this simulation for longer to reach your target velocity!"
        << std::endl;

  // Tune for transverse oscillation
  std::vector<int> nOscillations(10);
  std::iota(nOscillations.begin(), nOscillations.end(), 1);
  float diff = std::numeric_limits<float>::max();
  for (int n : nOscillations) {
    float thisOffTime = 0.5 * n / frequency(k);
    float thisDiff = std::abs(thisOffTime - constantVOffTime);

    if (thisDiff < diff) {
      if (thisOffTime - endTime > 0.1 * endTime)
        continue;

      thisOffTime = (thisOffTime > endTime) ? endTime : thisOffTime;
      diff = thisDiff;
      offTime_ = thisOffTime;
    }
  }

  // The trap won't be moving at the start
  offTime_ += shakeTime_;
}

float MovingTrapScheme::frequency(int k) {
  // Copied out of Mathematica, in turn copied from MATLAB. See ManifoldIntersect.nb and freqScript.m
  return
      (1.999753439e9 < 3.033655172413793e6 * k * maxVoltage_) ?
          0.0454669
              * (59321.0
                  + sqrt(-1.999753439e9 + 3.033655172413793e6 * k * maxVoltage_)) :
          2700;
}

bool MovingTrapScheme::isActive(int t) {
  return (t * timeStep_ < offTime_);
}

std::vector<float> MovingTrapScheme::getInitialVoltages() {
  return getVoltages(0);
}

std::vector<float> MovingTrapScheme::getVoltages(int t) {
  float tSeconds = t * timeStep_;
  float voltage = maxVoltage_;

  // If the next step is the point where we turn it off, do that now
  if (tSeconds + timeStep_ >= offTime_) {
    std::fill(voltages_.begin(), voltages_.end(), 0.0);
    return voltages_;
  }

  // Deal with ramp-up stage
  float phase;
  if (tSeconds <= shakeTime_) {
    voltage = maxVoltage_ * (0.3 + tSeconds * 0.7 / shakeTime_);
    phase = 0.8 * sin(3 * M_PI * tSeconds / shakeTime_);
  } else {
    phase = pow(tSeconds - shakeTime_, 2) * M_PI * targetVel_
        * Physics::MM_M_FACTOR / (offTime_ * sectionWidth_ * trapWidth_);
  }

  std::vector<int> listOfEs(nElectrodes_);
  std::iota(listOfEs.begin(), listOfEs.end(), 4);  // Create range 4:1:nElectrodes_
  // Transform into something like 1 1 1 1 2 2 2 2 3 3 3 3 etc
  std::transform(
      listOfEs.begin(), listOfEs.end(), listOfEs.begin(),
      [](const int &e) -> int {return floor(e/Physics::N_IN_SECTION);});

  // Periodic potential
  std::transform(
      listOfEs.begin(),
      listOfEs.end(),
      voltages_.begin(),
      [&](const int &e) -> float {return voltage * cos((2 * M_PI * (e % trapWidth_) / trapWidth_) - phase);});

  return voltages_;
}

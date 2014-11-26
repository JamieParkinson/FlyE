#include "VoltageScheme.h"

#include "Particle.h"
#include "PhysicalConstants.h";

VoltageScheme::VoltageScheme(float maxVoltage, int nElectrodes,
                             int sectionWidth, float timeStep)
    : maxVoltage_(maxVoltage),
      nElectrodes_(nElectrodes),
      sectionWidth_(sectionWidth),
      timeStep_(timeStep) {
}

SynchronousParticleScheme::SynchronousParticleScheme(
    Particle &synchronousParticle, float maxVoltage, int nElectrodes,
    int sectionWidth, float timeStep)
    : VoltageScheme(maxVoltage, nElectrodes, sectionWidth, timeStep),
      synchronousParticle_(synchronousParticle) {
}

// Instantaneous
InstantaneousScheme::InstantaneousScheme(Particle &synchronousParticle,
                                         float maxVoltage, int nElectrodes,
                                         int sectionWidth, float timeStep)
    : SynchronousParticleScheme(synchronousParticle, maxVoltage, nElectrodes,
                                sectionWidth, timeStep) {
}

bool InstantaneousScheme::isActive(int t) {
  return (synchronousParticle_.getLoc(2) >= section_ * sectionWidth_
      && section_ < nElectrodes_ / Physics::N_IN_SECTION);
}

std::vector<float> InstantaneousScheme::getVoltages(int t) {
  for (int e = Physics::N_IN_SECTION * (section_ - 1);
      e < Physics::N_IN_SECTION * section_; ++e) {
    voltages_[e] = maxVoltage_;  // Instantaneous switch
  }
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
  return (deltaT_ != 0.0 && t*timeStep_ <= (startRampTime_ + deltaT_));
}

std::vector<float> ExponentialScheme::getVoltages(int t) {
  float tSeconds = timeStep_*t;

  if (synchronousParticle_.getLoc(2) >= section_ * sectionWidth_
      && section_ < nElectrodes_ / 4) {  // Use synchronous particle to switch electrodes, if it's past this section but not past the end

    float syncAccel = (synchronousParticle_.getVel(2)
        - synchronousParticle_.recallVel(startRampTime_/timeStep_, 2))
        / (tSeconds - startRampTime_);  // Extrapolate particle accel.
    startRampTime_ = tSeconds;  // Update the last crossing time

    // Increase v until roughly when the next electrode is switched on: deltaT is the time the voltage increases for. Just a solution of const. accel. equations.
    deltaT_ = (-synchronousParticle_.getVel(2)
        + sqrt(
            pow(synchronousParticle_.getVel(2),
                2) + 2 * syncAccel * sectionWidth_ / Physics::MM_M_CORRECTION))
        / syncAccel;

    section_++;
  }

  // TODO check the status of this isActive() method (ie compatibility with these if statements)
  // TODO better methods for filling vectors with same number

  if (deltaT_ && tSeconds <= startRampTime_ + deltaT_) {  // Increase V until the appropriate time
    for (int e = Physics::N_IN_SECTION * (section_ - 2);
        e < Physics::N_IN_SECTION * (section_ - 1); ++e) {  // Ugly! We have to subtract 1 from section because it was incremented in the above branch.

      voltages_[e] = maxVoltage_ * (exp(timeConstant_* (tSeconds - startRampTime_)) - 1) / (exp(timeConstant_ * deltaT_) - 1);  // 1000 is just a constant that works well. I have no justification for it.
    }
  }

  return voltages_;
}

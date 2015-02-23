/**@file VoltageScheme.h
 * @brief This file contains the VoltageScheme abstract class, and its subclasses
 *
 * The subclasses of VoltageScheme are: SynchronousParticleScheme and MovingTrapScheme
 * The subclasses of SynchronousParticleScheme are: ExponentialScheme and InstantaneousScheme
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
#pragma once

#include <vector>

class Particle;

/** @brief An abstract class for voltage schemes for the accelerator
 *
 * The derived classes return vectors of the voltages to be applied respectively to each electrode;
 */
class VoltageScheme {
 protected:
  float maxVoltage_; //!< The maximum voltage that can be applied
  int nElectrodes_; //!< The number of electrodes to generate voltages for
  int sectionWidth_; //!< The width of a "section" of electrodes in the cylindrical accelerator
  std::vector<float> voltages_; //!< A vector of voltages
  float timeStep_; //!< The time step being used in the simulation

 public:
  /** @brief Base constructor initialises all of the member variables
   *
   * @param maxVoltage The maximum voltage that can be applied
   * @param nElectrodes The number of electrodes to generate voltages for
   * @param sectionWidth The width of a "section" of electrodes in the cylindrical accelerator
   * @param timeStep The time step being used in the simulation
   */
  VoltageScheme(float maxVoltage, int nElectrodes, int sectionWidth, float timeStep);

  /** @brief Returns a vector of the electrode voltages at time t
   *
   * @param t Time to get the voltage for
   * @return A vector of electrode voltages
   */
  virtual std::vector<float> getVoltages(int t) = 0;

  /** @brief Returns a vector of the initial voltages to apply before the simulation starts
   *
   * @return A vector of the initial voltages to apply before the simulation starts
   */
  virtual std::vector<float> getInitialVoltages() = 0;

  /** @brief Returns true if the voltages will be different from the previous access
   *
   * @param t The time to check activity
   * @return True if the voltages will be different from the previous access
   */
  virtual bool isActive(int t) = 0;

  /** @brief Virtual destructor, does nothing */
  virtual ~VoltageScheme();
};

/** @brief An abstract class for VoltageSchemes that rely on a synchronous particle
 *
 * Adds a member to store the current "section" that the synchronous particle is in,
 * and stores a reference to the synchronous particle
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class SynchronousParticleScheme : public VoltageScheme {
 protected:
  int section_ = 2; //!< The "section" that the synchronous particle is currently in
  Particle &synchronousParticle_; //!< The synchronous particle

 public:
  /** @brief Constructs the SynchronousParticleScheme with all the necessary member variables
   *
   * @see VoltageScheme
   *
   * @param synchronousParticle The synchronous particle (usually the first Particle in the vector of Particles)
   * @param maxVoltage The maximum voltage that can be applied
   * @param nElectrodes The number of electrodes to generate voltages for
   * @param sectionWidth The width of a "section" of electrodes in the cylindrical accelerator
   * @param timeStep The time step being used in the simulation
   */
  SynchronousParticleScheme(Particle &synchronousParticle, float maxVoltage, int nElectrodes, int sectionWidth, float timeStep);

  virtual std::vector<float> getVoltages(int t) = 0;

  virtual bool isActive(int t) = 0;

  std::vector<float> getInitialVoltages();
};

/** @brief A SynchronousParticleScheme that switches on sections instantaneously as the synchronous particle enters them
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class InstantaneousScheme : public SynchronousParticleScheme {
 public:
  /** @copydoc SynchronousParticleScheme::SynchronousParticleScheme() */
  InstantaneousScheme(Particle &synchronousParticle, float maxVoltage, int nElectrodes, int sectionWidth, float timeStep);

  std::vector<float> getVoltages(int t);

  bool isActive(int t);
};

/** @brief A SynchronousParticleScheme which exponentially increases the voltage on sections as the synchronous particle enters them
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class ExponentialScheme : public SynchronousParticleScheme {
 protected:
  static constexpr int timeConstant_ = 1000.0; //!< The time constant of the exponential increase
  float deltaT_ = std::numeric_limits<float>::max(); //!< The estimated time until the particle will enter the next section
  float startRampTime_ = 0.0; //!< The time at which the current section was activated

 public:
  /** @copydoc SynchronousParticleScheme::SynchronousParticleScheme() */
  ExponentialScheme(Particle &synchronousParticle, float maxVoltage, int nElectrodes, int sectionWidth, float timeStep);

  std::vector<float> getVoltages(int t);

  bool isActive(int t);
};

/** @brief A VoltageScheme which appplies a periodic oscillating voltage to create a moving trap
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class MovingTrapScheme : public VoltageScheme {
 protected:
  static constexpr int trapWidth_ = 6; //!< The number of sections that a trap should span
  float targetVel_; //!< The target velocity for the particles (oscillation frequency depends on this)
  float offTime_; //!< The time at which the E-Field should be switched off for the particles to fly freely
  float endTime_; //!< The simulation end time
  float shakeTime_; //!< The amount of time to ramp up the trap voltage for before it moves

  /** @brief Gets the frequency of transverse oscillation of particles with Stark quantum number k
   *
   * This assumes that the particles are transversely oscillating in a nearly harmonic potential. The equation used in the source was
   * found in MATLAB by computing a regression from data at various k-values. Because k and V both have a linear effect on the dipole moment,
   * we can generalise that regression to work for different k and V values. This function accesses the member variable maxVoltage_ to do this.
   *
   * @param k Any stark quantum number of the particles
   * @return An estimate of the frequency of transverse particle oscillation (Hz)
   */
  float frequency(int k);

 public:
  /** @brief Constructs the MovingTrapScheme with the appropriate parameters
   *
   * @param maxVoltage The maximum voltage that can be applied
   * @param nElectrodes The number of electrodes to generate voltages for
   * @param sectionWidth The width of a "section" of electrodes in the cylindrical accelerator
   * @param timeStep The time step being used in the simulation
   * @param endTime The simulation end time
   * @param targetVel The target velocity for the particles to reach
   * @param k The k-value to estimate frequency from. Ideally this will be the average k-value.
   */
  MovingTrapScheme(float maxVoltage, int nElectrodes, int sectionWidth, float timeStep, float shakeTime, float endTime, float targetVel, int k);

  std::vector<float> getInitialVoltages();

  std::vector<float> getVoltages(int t);

  bool isActive(int t);
};

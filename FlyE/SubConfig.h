/**@file SubConfig.h
 * @brief This file contains the SubConfig class, and its subclasses
 *
 * The subclasses of SubConfig are: AcceleratorConfig, ParticlesConfig, SimulationConfig and StorageConfig
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
#pragma once

#include <ostream>
#include <memory>
#include "inih/cpp/INIReader.h"

/**@brief Abstract class which the config data containers inherit from
 *
 * This class is mostly just here for semantic reasons. All the different domains of configuration data
 * (eg Accelerator, Particles etc) are stored in classes which inherit from this.
 *
 * @see ConfigLoader
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class SubConfig {
 protected:
  /** @brief Acts on an ostream to give formatted contents of member variables */
  virtual void printOn(std::ostream &out) = 0;  //!< Java-inspired method to print out the data in the object.

 public:
  /** @brief For pretty printing the config data
   *
   * @param out Any output-stream object (ie cout)
   * @param subConf Any SubConfig object (or derived classes)
   * @return An ostream containing the pretty-printed data
   */
  friend std::ostream& operator <<(std::ostream &out, SubConfig &subConf);

  virtual ~SubConfig();

  /** @brief Populates the member variables of the object using the data in the INIReader object
   *
   * @param reader An INIReader object, usually supplied by constructor
   */
  virtual void populate(INIReader &reader) = 0;
};

/** @brief For storing configuration data pertaining to the accelerator geometry */
class AcceleratorConfig : public SubConfig {
 protected:
  int nElectrodes_;  //!< Number of electrodes in the accelerator
  ///@{ @brief x, y, z dimensions of the accelerator
  int x_, y_, z_;  ///@}
  std::string datDirectory_;  //!< Directory in which the E-Field .dat files are stored
  std::string PAname_;  //!< prefix for EXSIMECK-named files

  //!< @copydoc SubConfig::printOn()
  void printOn(std::ostream &out);

 public:
  /** @brief Generates the object using the INIReader
   *
   * @param reader An INIReader object, usually supplied by ConfigLoader
   * @return AcceleratorConfig object
   */
  AcceleratorConfig(INIReader &reader);

  //!< @copydoc SubConfig::populate()
  void populate(INIReader &reader);

  // Getters
  /** @brief Directory in which the E-Field .dat files are stored */
  const std::string& datDirectory() const;

  /** @brief Number of electrodes in the accelerator */
  int nElectrodes() const;

  /** @brief Prefix for EXSIMECK-named files */
  const std::string& PAname() const;

  /** @brief Dimension in x of accelerator geometry */
  int x() const;

  /** @brief Dimension in y of accelerator geometry */
  int y() const;

  /** @brief Dimension in z of accelerator geometry */
  int z() const;
};

/** @brief For storing configuration data pertaining to the nature of the simulation
 *
 * Setters are provided because these variables can be changed without having to regenerate particles etc
 */
class SimulationConfig : public SubConfig {
 protected:
  float timeStep_;  //!< Timestep to use in simulation (s)
  float duration_;  //!< Time to run simulation for (s)
  float maxVoltage_;  //!< Maximum voltage that can be applied to electrodes (V)
  float targetVel_;  //!< Velocity to try to accelerate the particles to (m/s)
  std::string accelerationScheme_;  //!< Scheme for acceleration: trap, inst or exp
  bool inglisTeller_;  //!< Whether to neutralise the dipole moment of particles past the I-T limit

  //!< @copydoc SubConfig::printOn()
  void printOn(std::ostream &out);

 public:
  /** @brief Generates the object using the INIReader
   *
   * @param reader An INIReader object, usually supplied by ConfigLoader
   * @return SimulationConfig object
   */
  SimulationConfig(INIReader &reader);

  //!< @copydoc SubConfig::populate()
  void populate(INIReader &reader);

  // Getters

  /** @brief Scheme for acceleration: trap, inst or exp */
  const std::string& accelerationScheme() const;

  /** @brief Time to run simulation for (s) */
  float duration() const;

  /** @brief Whether to neutralise the dipole moment of particles past the I-T limit */
  bool inglisTeller() const;

  /** @brief Maximum voltage that can be applied to electrodes (V) */
  float maxVoltage() const;

  /** @brief Velocity to try to accelerate the particles to (m/s) */
  float targetVel() const;

  /** @brief Timestep to use in simulation (s) */
  float timeStep() const;

  /** @brief Setter for acceleration scheme
   *
   * @param scheme String representing scheme: "trap", "exp" or "inst"
   */
  void setAccelerationScheme(const std::string &scheme);

  /** @brief Setter for simulation duration
   *
   * @param duration Time to simulate for
   */
  void setDuration(float duration);

  /** @brief Setter for whether to neutralise particles past the I-T limit
   *
   * @param inglisTeller True if neutralising particles
   */
  void setInglisTeller(bool inglisTeller);

  /** @brief Setter for maximum voltage to apply to electrodes
   *
   * @param maxVoltage Maximum voltage to apply to electrodes
   */
  void setMaxVoltage(float maxVoltage);

  /** @brief Setter for the target velocity of particles
   *
   * @param targetVel Target velocity of particles
   */
  void setTargetVel(float targetVel);

  /** @brief Setter for the simulation time step
   *
   * @param timeStep Time step to simulate with
   */
  void setTimeStep(float timeStep);
};

/** @brief For storing configuration data pertaining to the particles in the simulation */
class ParticlesConfig : public SubConfig {
 protected:
  int nParticles_;  //!< Number of particles in the simulation
  bool normDist_;  //!< Whether the particles are to be normally distributed
  ///@{ @brief The size of the distribution of particles (mm). If normally distributed, then the standard deviations.
  float distRadius_, distLength_;  ///@}
  float temperature_;  //!< Temperature of initial particle cloud
  std::string kDist_;  //!< Distribution of k-values: single, uniform or triangle
  int n_;  //!< Principal quantum number of all particles
  int k_;  //!< If kDist is single, then use this value

  //!< @copydoc SubConfig::printOn()
  void printOn(std::ostream &out);

 public:
  /** @brief Generates the object using the INIReader
   *
   * @param reader An INIReader object, usually supplied by ConfigLoader
   * @return ParticlesConfig object
   */
  ParticlesConfig(INIReader &reader);

  //!< @copydoc SubConfig::populate()
  void populate(INIReader &reader);

  // Getters

  /** @brief Number of particles in the simulation */
  int nParticles() const;

  /** @brief If kDist is single, then use this value */
  int k() const;

  /** @brief Distribution of k-values: single, uniform or triangle */
  const std::string& kDist() const;

  /** @brief Principal quantum number of all particles */
  int n() const;

  /** @brief Whether the particles are to be normally distributed */
  bool normDist() const;

  ///@{ @return The dimensions of the distribution of particles
  float distRadius() const;
  float distLength() const;
  ///@}

  /** @brief Temperature of initial particle cloud */
  float temperature() const;
};

/** @brief For storing configuration data pertaining to the storage of simulation results */
class StorageConfig : public SubConfig {
 protected:
  std::string outDir_;  //!< The directory to write output files to
  bool storeTrajectories_;  //!< Whether to store the full particle trajectories or just endpoints
  bool storeCollisions_;  //!< Whether to store any data of particles that collide
  int compression_; //!< Integer from 0-9 sets compression level. 0 is no compression.

  //!< @copydoc SubConfig::printOn()
  void printOn(std::ostream &out);

 public:
  /** @brief Generates the object using the INIReader
   *
   * @param reader An INIReader object, usually supplied by ConfigLoader
   * @return ParticlesConfig object
   */
  StorageConfig(INIReader &reader);

  //!< @copydoc SubConfig::populate()
  void populate(INIReader &reader);

  /** @brief The directory to write output files to */
  const std::string& outDir() const;

  /** @brief Whether to store the full particle trajectories or just endpoints */
  bool storeTrajectories() const;

  /** @brief Whether to store any data of particles that collide */
  bool storeCollisions() const;

  /** @brief Integer from 0-9 sets compression level. 0 is no compression. */
  int compression() const;
};

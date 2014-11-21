#pragma once

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
 public:
  virtual ~SubConfig();

  /** @brief Populates the member variables of the object using the data in the INIReader object
   *
   * @param reader An INIReader object, usually supplied by constructor
   */
  virtual void populate(INIReader &reader) = 0;

  /** @brief Returns a string with formatted contents of member variables
   *
   * @return A string with formatted contents of member variables
   */
  virtual std::string toString() = 0;  //!< Java-inspired method to print out the data in the object.
};

/** @brief For storing configuration data pertaining to the accelerator geometry */
class AcceleratorConfig : public SubConfig {
 protected:
  int nElectrodes_;  //!< Number of electrodes in the accelerator
  ///@{ @brief x, y, z dimensions of the accelerator
  int x_, y_, z_;  ///@}
  std::string datDirectory_;  //!< Directory in which the E-Field .dat files are stored
  std::string PAname_;  //!< prefix for EXSIMECK-named files

 public:
  /** @brief Generates the object using the INIReader
   *
   * @param reader An INIReader object, usually supplied by ConfigLoader
   * @return AcceleratorConfig object
   */
  AcceleratorConfig(INIReader &reader);

  //!< @copydoc SubConfig::populate()
  void populate(INIReader &reader);

  //!< @copydoc SubConfig::toString()
  std::string toString();

  // Getters

  /** @return Directory in which the E-Field .dat files are stored */
  const std::string& datDirectory() const;

  /** @return Number of electrodes in the accelerator */
  int nElectrodes() const;

  /** @return Prefix for EXSIMECK-named files */
  const std::string& PAname() const;

  /** @return Dimension in x of accelerator geometry */
  int x() const;

  /** @return Dimension in y of accelerator geometry */
  int y() const;

  /** @return Dimension in z of accelerator geometry */
  int z() const;
};

/** @brief For storing configuration data pertaining to the nature of the simulation */
class SimulationConfig : public SubConfig {
 protected:
  float timeStep_;  //!< Timestep to use in simulation (s)
  float duration_;  //!< Time to run simulation for (s)
  float maxVoltage_;  //!< Maximum voltage that can be applied to electrodes (V)
  float targetVel_;  //!< Velocity to try to accelerate the particles to (m/s)
  std::string accelerationScheme_;  //!< Scheme for acceleration: trap, inst or exp
  bool inglisTeller_;  //!< Whether to neutralise the dipole moment of particles past the I-T limit

 public:
  /** @brief Generates the object using the INIReader
   *
   * @param reader An INIReader object, usually supplied by ConfigLoader
   * @return SimulationConfig object
   */
  SimulationConfig(INIReader &reader);

  //!< @copydoc SubConfig::populate()
  void populate(INIReader &reader);

  //!< @copydoc SubConfig::toString()
  std::string toString();

  // Getters

  /** @return Scheme for acceleration: trap, inst or exp */
  const std::string& accelerationScheme() const;

  /** @return Time to run simulation for (s) */
  float duration() const;

  /** @return Whether to neutralise the dipole moment of particles past the I-T limit */
  bool inglisTeller() const;

  /** @return Maximum voltage that can be applied to electrodes (V) */
  float maxVoltage() const;

  /** @return Velocity to try to accelerate the particles to (m/s) */
  float targetVel() const;

  /** @return Timestep to use in simulation (s) */
  float timeStep() const;
};

/** @brief For storing configuration data pertaining to the particles in the simulation */
class ParticlesConfig : public SubConfig {
 protected:
  int nParticles_;  //!< Number of particles in the simulation
  bool normDist_;  //!< Whether the particles are to be normally distributed
  ///@{ @brief If normDist is true, the parameters of the normal distributions
  float sigmaX_, sigmaY_, sigmaZ_;  ///@}
  float temperature_;  //!< Temperature of initial particle cloud
  std::string kDist_;  //!< Distribution of k-values: single, uniform or triangle
  int n_;  //!< Principal quantum number of all particles
  int k_;  //!< If kDist is single, then use this value

 public:
  /** @brief Generates the object using the INIReader
   *
   * @param reader An INIReader object, usually supplied by ConfigLoader
   * @return ParticlesConfig object
   */
  ParticlesConfig(INIReader &reader);

  //!< @copydoc SubConfig::populate()
  void populate(INIReader &reader);

  //!< @copydoc SubConfig::toString()
  std::string toString();

  // Getters

  /** @return Number of particles in the simulation */
  int nParticles() const;

  /** @return If kDist is single, then use this value */
  int k() const;

  /** @return Distribution of k-values: single, uniform or triangle */
  const std::string& kDist() const;

  /** @return Principal quantum number of all particles */
  int n() const;

  /** @return Whether the particles are to be normally distributed */
  bool normDist() const;

  ///@{ @return If normDist is true, the parameters of the normal distributions
  float sigmaX() const;
  float sigmaY() const;
  float sigmaZ() const;
  ///@}

  /** @return Temperature of initial particle cloud */
  float temperature() const;
};

/** @brief For storing configuration data pertaining to the storage of simulation results */
class StorageConfig : public SubConfig {
 protected:
  std::string outDir_; //!< The directory to write output files to
  bool storeTrajectories_; //!< Whether to store the full particle trajectories or just endpoints
  bool storeCollisions_; //!< Whether to store any data of particles that collide

 public:
  /** @brief Generates the object using the INIReader
    *
    * @param reader An INIReader object, usually supplied by ConfigLoader
    * @return ParticlesConfig object
    */
  StorageConfig(INIReader &reader);

  //!< @copydoc SubConfig::populate()
  void populate(INIReader &reader);

  //!< @copydoc SubConfig::toString()
  std::string toString();

  /** @return The directory to write output files to */
  const std::string& outDir() const;

  /** @return Whether to store the full particle trajectories or just endpoints */
  bool storeTrajectories() const;

  /** @return Whether to store any data of particles that collide */
  bool storeCollisions() const;
};

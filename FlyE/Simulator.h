/**@file Simulator.h
 * @brief This file contains the Simulator class, and the SimuationNumbers struct
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
#pragma once

#include <vector>
#include <memory>
#include <ostream>

#include "AcceleratorGeometry.h"
#include "AntiHydrogen.h"
#include "SmartField.h"
#include "SubConfig.h"
#include "VoltageScheme.h"

/** @brief A struct which stores the numbers of each particle type
 *
 * Also provides pretty-printed output
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
struct SimulationNumbers {
  int nCollided; //!< Number of particles which have collided with the accelerator geometry
  int nIonised; //!< Number of ionised particles
  int nSucceeded; //!< Number of particles which have reached the end of the accelerator
  int nNeutralised; //!< Number of particles which have had their dipole moments neutralised
  int nParticles; //!< Total number of particles

  /** @brief Pretty prints the values of the member variables
   *
   * @param out Any out-stream (usually std::cout)
   * @param simNums An instance of this struct
   * @return An out-stream including the pretty-printed info
   */
  friend std::ostream& operator <<(std::ostream &out, SimulationNumbers &simNums);
};

/** @brief A class to actually run the simulation we want
 *
 * Takes appropriate config variables as well as an AcceleratorGeometry and a vector of particles,
 * then runs the simulation as configured. Responsible for moving, colliding etc particles.
 *
 * Also provides access to Writer, which writes the simulation data to an HDF5 file
 *
 * @see Writer
 * @see AcceleratorGeometry
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class Simulator {
  friend class Writer;
 protected:
  AcceleratorGeometry geometry_; //!< The geometry to run the simulation with
  std::vector<AntiHydrogen> particles_; //!< A vector of Particles (or, in this case, AntiHydrogen) to run the simulation with

  std::shared_ptr<SimulationConfig> simulationConfig_; //!< Configuration pertaining to the nature of the simulation
  std::shared_ptr<AcceleratorConfig> acceleratorConfig_; //!< Configuration pertaining to the accelerator
  std::shared_ptr<StorageConfig> storageConfig_; //!< Configuration pertaining to how the data is stored

  SmartField field_; //!< A SmartField for accessing the E-Field in the accelerator
  VoltageScheme *voltageScheme_; //!< The scheme for applying voltages: exponential, instantaneous or trap

  SimulationNumbers statsStorage_; //!< Storage for basic statistics from the simulation

 public:
  /** Construct from a geometry and vector of particles, and appropriate storage structs
   *
   * @param geometry An AcceleratorGeometry instance
   * @param particles A vector of AntiHydrogens
   * @param simulationConfig Configuration data pertaining to the simulation
   * @param storageConfig Configuration data pertaining to the storage of data
   */
  Simulator(AcceleratorGeometry &geometry, std::vector<AntiHydrogen> &particles,
            std::shared_ptr<SimulationConfig> simulationConfig,
            std::shared_ptr<StorageConfig> storageConfig);

  /** @brief Run the simulation! */
  void run();

  /** @brief Returns a SimulationNumbers struct containing basic simulation stats
   *
   * @return A SimulationNumbers struct containing basic simulation stats
   */
  SimulationNumbers getBasicStats();

  /** @brief Write the simulation data to the given path
   *
   * @param fileName The path to write simulation data to
   */
  void write(std::string fileName);
};

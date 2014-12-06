/** @file AcceleratorGeometry.h
 *  @brief This file contains the AcceleratorGeometry class
 *
 *  @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
#pragma once

#include <memory>
#include <vector>

#include "Electrode.h"
#include "SmartField.h"
#include "ElectrodeLocator.h"

class AcceleratorConfig;

/** @brief A class for handling the complete geometry of the accelerator
 *
 * Predominantly a container for Electrodes.
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class AcceleratorGeometry {
 protected:
  std::vector< std::shared_ptr<Electrode> > electrodes_; //!< For storing (pointers) to all the Electrodes in the accelerator
  std::shared_ptr<AcceleratorConfig> config_; //!< The configuration data for the geometry

 public:
  /** @brief Constructs from a shared_ptr to an AcceleratorConfig instance
   *
   * @param config Contains all of the configuration/geometry data
   */
  AcceleratorGeometry(std::shared_ptr<AcceleratorConfig> config);

  /** @brief Tells all electrodes to import their E-Field files */
  void importElectrodes();

  /** @brief Applies electrode voltages in order from the vector
   *
   * @param voltages A vector of voltages to apply, respectively, to the electrodes
   */
  void applyElectrodeVoltages(std::vector<float> voltages);

  /** @brief Returns a SmartField for the current state of the geometry
   *
   * @return Effectively the current field (given voltages) in the accelerator
   */
  SmartField makeSmartField();

  /** @brief Sums all the Electrodes and returns the superposed field
   *
   * @return The total field in the accelerator
   */
  VectorField makeVectorField();

  /** @brief Returns an ElectrodeLocator for everything in the accelerator
   *
   * @return ElectrodeLocator for *all* electrodes
   */
  ElectrodeLocator electrodeLocations();

  /** @brief Returns a shared_ptr to the AcceleratorConfig instance associated with this geometry
   *
   * @return A shared_ptr to the AcceleratorConfig instance associated with this geometry
   */
  std::shared_ptr<AcceleratorConfig> getAcceleratorConfig();
};

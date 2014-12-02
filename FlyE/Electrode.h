#pragma once

#include <blitz/tinyvec2.h>
#include <memory>

#include "VectorField.h"
#include "SubConfig.h"

/** @brief Represents a single electrode in the accelerator geometry
 *
 * Derived from VectorField. Adds methods to import E-Field files, change its voltage and get its location.
 * ElectrodeLocator is a friend class. Also remembers its number within the geometry.
 *
 * @see ElectrodeLocator
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class Electrode : public VectorField {
 protected:
  int electrodeNumber_; //!< The number/index of the electrode within the accelerator geometry.
  float currentVoltage_ = 1.0; //!< The voltage that is currently being applied to the electrode
 public:
  /** @brief Blank constructor */
  Electrode();
  /** @brief Constructs an "empty" Electrode with given index
   *
   * @param electrodeNumber The number/index of the electrode within the accelerator geometry.
   */
  Electrode(int electrodeNumber);

  /** @brief Copy constructor
   *
   * @param elec Electrode to copy
   */
  Electrode(const Electrode &elec);

  /** @brief Imports the E-Field files associated with this electrode
   *
   * Stores them as the base field of the Electrode. By convention, this is the field when the Electrode has
   * a PD of 1V applied to it. As such, any other voltage can be obtained by multiplying the field by a scalar.
   *
   * @param config Relevant AcceleratorConfig, usually provided by ConfigLoader
   *
   * @see ConfigLoader
   * @see AcceleratorConfig
   */
  void import(std::shared_ptr<AcceleratorConfig> config);

  /** @brief Applies a voltage to the Electrode
   *
   * Trying a new approach for this: the scalar multiplication will only be done when we get values from the Electrode.
   *
   * @param voltage The voltage to apply
   */
  void applyVoltage(float voltage);

  float getVoltage();
};


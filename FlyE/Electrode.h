#pragma once

#include <memory>

#include "VectorField.h"

class AcceleratorConfig;
class ElectrodeLocator;

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
  friend class ElectrodeLocator;
 protected:
  int electrodeNumber_; //!< The number/index of the electrode within the accelerator geometry.
  float currentVoltage_; //!< The voltage that is currently being applied to the electrode
 public:
  /** @brief Constructs an "empty" Electrode with given index
   *
   * @param electrodeNumber The number/index of the electrode within the accelerator geometry.
   */
  Electrode(int electrodeNumber);

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
  void import(std::unique_ptr<AcceleratorConfig> config);

  /** @brief Applies a voltage to the Electrode
   *
   * Trying a new approach for this: the scalar multiplication will only be done when we get values from the Electrode.
   *
   * @param voltage The voltage to apply
   */
  void applyVoltage(float voltage);

  /** @brief Shadows blitz's access operator, but applies the voltage first
   *
   * @param x x-coordinate of point
   * @param y y-coordinate of point
   * @param z z-coordinate of point
   * @return The TinyVector of the field at the point, multiplied by currentVoltage_
   */
  blitz::TinyVector<float, 3> operator ()(int x, int y, int z);

  /** @brief An ElectrodeLocator for this electrode
   *
   * The ElectrodeLocator stores the location of the Electrode
   *
   * @see ElectrodeLocator
   *
   * @return An ElectrodeLocator for this electrode
   */
  ElectrodeLocator getLocator();
};


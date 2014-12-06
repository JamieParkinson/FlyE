/**@file ElectrodeLocator.h
 * @brief This file contains the ElectrodeLocator class
 *
 * ElectrodeLocator is derived from a 3D Blitz++ array of bools
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
#pragma once

#include <memory>
#include <blitz/array-impl.h>

#include "tupleDefs.h"

class Electrode;

/** @brief A class for finding the physical location of electrodes in an E-Field
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class ElectrodeLocator : public blitz::Array<bool, 3> {
 public:
  /** @brief Constructs an ElectrodeLocator from an Electrode
   *
   * @param electrode An Electrode, the location of which will be determined
   */
  ElectrodeLocator(Electrode &electrode);

  /** @brief For construction from a shared_ptr to an Electrode
   *
   * Useful because shared_ptr<Electrode>s are quite common
   *
   * @param electrode A shared_ptr to an Electrode to be located
   */
  ElectrodeLocator(std::shared_ptr<Electrode> electrode);

  /**
   * @brief Checks to see if the electrode exists at a given point
   * @param x x-coordinate to check
   * @param y y-coordinate to check
   * @param z z-coordinate to check
   * @return true if it does exist at the point, false otherwise
   */
  bool existsAt(int x, int y, int z);

  /**
   * @brief Checks to see if the electrode exists at a given point
   * @param r A tuple of 3 integers (x, y, z)
   * @return true if it does exist at the point, false otherwise
   */
  bool existsAt(const tuple3Dint &r);
};

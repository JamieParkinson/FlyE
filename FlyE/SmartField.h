#pragma once

#include "Electrode.h"

/** @brief A clever way to access the superposed fields of electrodes
 *
 * Only sums when accessing a specific point
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class SmartField : public VectorField {
 protected:
  std::vector< std::shared_ptr<Electrode> > electrodes_; //!< All of the electrodes in an AcceleratorGeometry

 public:
  /** @brief Blank constructor, does nothing */
  SmartField();

  /** @brief Constructs a SmartField from a vector of electrodes
   *
   * @param electrodes A vector of shared_ptr<Electrode>s
   */
  SmartField(std::vector< std::shared_ptr<Electrode> > electrodes);

  /** @brief The vector of the field at a point
   *
   * @param x x-coordinate of the point
   * @param y y-coordinate of the point
   * @param z z-coordinate of the point
   * @return A TinyVector of the field at the given point
   */
  ///@{
  blitz::TinyVector<float, 3> at(int x, int y, int z);
  blitz::TinyVector<float, 3> operator ()(int x, int y, int z);
  ///@}

  /** @brief The magnitude of the field at a point
   *
   * @param x x-coordinate of the point
   * @param y y-coordinate of the point
   * @param z z-coordinate of the point
   * @return The magnitude of the field at (x, y, z)
   */
  float magnitudeAt(int x, int y, int z);

  /** @brief The gradient of the magnitude of the field at a point
   *
   * @param x x-coordinate of the point
   * @param y y-coordinate of the point
   * @param z z-coordinate of the point
   * @return The gradient of the magnitude of the field at (x, y, z)
   */
  ///@{
  float gradientXat(int x, int y, int z);
  float gradientYat(int x, int y, int z);
  float gradientZat(int x, int y, int z);
  ///@}
};

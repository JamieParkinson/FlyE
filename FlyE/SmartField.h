/**@file SmartField.h
 * @brief This file contains the SmartField class and the std::hash<tuple3D> specialization
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
#pragma once

#include "Electrode.h"

#include <unordered_map>
#include <tuple>

//!< 3D tuple of integers
typedef std::tuple<int, int, int> tuple3D;
/** @brief 8-digit primes for hash generation */
///@{
#define P1 71234567
#define P2 25935017
#define P3 53781811
///@}
namespace std {
/** @brief Template specialisation for std::hash<tuple> */
template<> struct hash<tuple3D> {
  /** @brief For accessing the hashing algorithm
   *
   * @param t A tuple3D (aka std::tuple<int, int, int>) of integer coords
   * @return A size_t (aka unsigned int) hash code
   */
  size_t operator()(const tuple3D &t) const {
    // See Consistent Collision and Self-Collision Handling for Deformable Objects (Heidelburger, 2007)
    return std::hash<int>()( ((get<0>(t)*P1) ^ (get<1>(t)*P2) ^ (get<2>(t)*P3)) );
  }
};
}

/** @brief A clever way to access the superposed fields of electrodes
 *
 * Only sums when accessing a specific point, but remembers magnitude values of points which have been accessed previously
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class SmartField {
 protected:
  std::vector< std::shared_ptr<Electrode> > electrodes_; //!< All of the electrodes in an AcceleratorGeometry
  std::unordered_map<tuple3D, float> magnitudeMemory_; //!< Remembers magnitudes that have already been accessed

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

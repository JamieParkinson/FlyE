/**@file SmartField.h
 * @brief This file contains the SmartField class and the std::hash<tuple3D> specialization
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
#pragma once

#include "Electrode.h"

#include <unordered_map>
#include <tuple>

/** @brief 8-digit primes for hash generation */
///@{
#define P1 71234567
#define P2 25935017
#define P3 53781811
///@}
namespace std {
/** @brief Template specialisation for std::hash<tuple> */
template<> struct hash<tuple3Dint> {
  /** @brief For accessing the hashing algorithm
   *
   * @param t A tuple3D (aka std::tuple<int, int, int>) of integer coords
   * @return A size_t (aka unsigned int) hash code
   */
  size_t operator()(const tuple3Dint &t) const {
    // See Consistent Collision and Self-Collision Handling for Deformable Objects (Heidelburger, 2007)
    return std::hash<int>()( ((get<0>(t)*P1) ^ (get<1>(t)*P2) ^ (get<2>(t)*P3)) );
  }
};
}

/** @brief A clever way to access the superposed fields of electrodes
 *
 * Only sums when accessing a specific point, but remembers magnitude values of points which have been accessed previously.
 *
 * This code is very much not DRY, but I don't want it to inherit from VectorField because that would bring the overhead of
 * the Blitz++ array with it. Maybe I'll change this in future but I think this slightly ugly repetition is faster.
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class SmartField {
 protected:
  std::vector< std::shared_ptr<Electrode> > electrodes_; //!< All of the electrodes in an AcceleratorGeometry
  std::unordered_map<tuple3Dint, float> magnitudeMemory_; //!< Remembers magnitudes that have already been accessed

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

  /** @brief The vector of the field at a point
   *
   * @param r A tuple of 3 integers (x, y, z)
   * @return A TinyVector of the field at the given point
   */
  ///@{
  blitz::TinyVector<float, 3> at(tuple3Dint r);
  blitz::TinyVector<float, 3> operator ()(tuple3Dint r);
  ///@}

  /** @brief The magnitude of the field at a point
   *
   * @param x x-coordinate of the point
   * @param y y-coordinate of the point
   * @param z z-coordinate of the point
   * @return The magnitude of the field at (x, y, z)
   */
  float magnitudeAt(int x, int y, int z);

  /** @brief The magnitude of the field at a point
   *
   * @param r A 3D integer tuple (x, y, z)
   * @return The magnitude of the field at the given point
   */
  float magnitudeAt(tuple3Dint r);

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

  ///@{
  /** @brief Gradients in each direction
   * @name Gradients
   * @param r A tuple of 3 integers (x, y, z)
   * @return The gradient of the magnitude of the field at the given point
   */
  float gradientXat(tuple3Dint r);
  float gradientYat(tuple3Dint r);
  float gradientZat(tuple3Dint r);
  ///@}
};

#pragma once

#include <blitz/array.h>

/**
 * @brief A general 3D vector field class, with basic operators
 *
 * Based on Blitz++ Arrays - specifically, Array<TinyVector<float, 3>, 3>
 *
 * Contains methods for adding/subtracting fields and multiplication by a scalar. Also offers methods for
 * field magnitude at a point, and x/y/z gradient at a point. There is a static method for summing a vector of
 * shared_ptr<VectorField>s.
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class VectorField {
 protected:
  /** @brief A 3D array of vectors. @see blitz::Array<blitz::TinyVector<float, 3>, 3> */
  blitz::Array<blitz::TinyVector<float, 3>, 3> fieldArray_;

  /** @brief To be used internally; for calculating the magnitude of the vector field at a point
   *
   * sqrt(x^2 + y^2 + z^2)
   *
   * @param vec A Blitz++ 3D TinyVector of floats (cartesian)
   * @return The magnitude of the given vector
   */
  static float vectorMagnitude(blitz::TinyVector<float, 3> vec);

 public:
  /**
   * @brief Constructs a VectorField of dimensions sizeX * sizeY * sizeZ
   *
   * @param sizeX The size of the dimension along x
   * @param sizeY The size of the dimension along y
   * @param sizeZ The size of the dimension along z
   */
  VectorField(int sizeX, int sizeY, int sizeZ);

  /**
   * @brief Constructs a VectorField from a Blitz++ array of the correct dimensions
   *
   * @param blitzArray An array of the same type as fieldArray_
   */
  VectorField(blitz::Array<blitz::TinyVector<float, 3>, 3> blitzArray);

  /**
   * @brief Basic addition/subtraction operators for combining fields by superposition
   * @name Addition/Subtraction
   * @param vec Another VectorField object
   */
  ///@{
  VectorField operator +(VectorField vec);
  VectorField operator -(VectorField vec);
  void operator +=(VectorField vec);
  void operator -=(VectorField vec);
  ///@}

  /**@brief For multiplying every value in the vector field by a scalar
   * @name Multiplication
   * @param scalar The number to multiply field by
   */
  ///@{
  VectorField operator *(float scalar);
  void operator *=(float scalar);
  ///@}

  /** @brief Magnitude of the vector field at a point
   *
   * @see vectorMagnitude()
   *
   * @param x x-coordinate of point
   * @param y y-coordinate of point
   * @param z z-coordinate of point
   * @return The magnitude of the field at the point (x,y,z)
   */
  float magnitudeAt(int x, int y, int z);

  ///@{
  /** @brief Gradients in each direction
   * @name Gradients
   * @param x x-coordinate of point
   * @param y y-coordinate of point
   * @param z z-coordinate of point
   * @return The gradient of the magnitude of the field at the given point
   */
  float gradientXat(int x, int y, int z);
  float gradientYat(int x, int y, int z);
  float gradientZat(int x, int y, int z);
  ///@}

  /** @brief For adding together std::vectors of shared_ptr<VectorField>s
   *
   * nToSum is optional. If -1 (default value), all in fields are summed.
   *
   * @param fields Contains all of the shared_ptr<VectorFields> to be summed
   * @param nToSum Optional parameter: if set, only the first nToSum elements of fields will be summed
   * @return A new VectorField which is the superposition of all in fields
   */
  static VectorField sumVectorFields(
      std::vector<std::shared_ptr<VectorField> > &fields, int nToSum = -1);
};

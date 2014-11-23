#pragma once

#include <blitz/array-impl.h>
#include <blitz/tinyvec2.h>
#include <memory>
#include <vector>

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
class VectorField : public blitz::Array<blitz::TinyVector<float, 3>, 3> {
 protected:
  /** @brief To be used internally; for calculating the magnitude of the vector field at a point
   *
   * sqrt(x^2 + y^2 + z^2)
   *
   * @param vec A Blitz++ 3D TinyVector of floats (cartesian)
   * @return The magnitude of the given vector
   */
  static float vectorMagnitude(const blitz::TinyVector<float, 3> &vec);

 public:
  /** @brief Empty void constructor
   *
   * Just here to be used by subclasses
   */
  VectorField();
  /**
   * @brief Constructs a VectorField of dimensions sizeX * sizeY * sizeZ
   *
   * @param sizeX The size of the dimension along x
   * @param sizeY The size of the dimension along y
   * @param sizeZ The size of the dimension along z
   */
  VectorField(int sizeX, int sizeY, int sizeZ);

  /**
   * @brief Copy constructor; uses blitz's version
   * @param vec VectorField we're copying from
   */
  VectorField(const VectorField &vec);

  /**
   * @brief Assignment operator overload
   * @param rhs The right hand side VectorField of the assignment
   * @return The VectorField with contents replaced by those of rhs
   */
  VectorField operator =(const VectorField &rhs);

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

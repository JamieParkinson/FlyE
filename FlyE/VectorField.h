/*
 * Stores the geometry of the accelerator. Based on the Blitz++ library, with some helper methods
 */
#pragma once

#include <blitz/array.h>

class VectorField {
 protected:
  // A 3D array of vectors
  blitz::Array<blitz::TinyVector<float,3>, 3> fieldArray_;

  // To be used internally; for calculating the magnitude of the vector field at a point
  static float vectorMagnitude(blitz::TinyVector<float, 3>);

 public:
  // Constructs a VectorField of dimensions sizeX * sizeY * sizeZ
  VectorField(int sizeX, int sizeY, int sizeZ);

  // Constructs a VectorField from a Blitz++ array of the correct dimensions
  VectorField(blitz::Array<blitz::TinyVector<float,3>, 3> blitzArray);

  // Just wrappers for Blitz++ functions
  VectorField operator +(VectorField vec);
  VectorField operator -(VectorField vec);
  void operator +=(VectorField vec);
  void operator -=(VectorField vec);

  // Scalar multiplication
  VectorField operator *(float scalar);
  void operator *=(float scalar);

  // Magnitude of the vector field at a point
  float magnitudeAt(int x, int y, int z);

  // Gradients in each direction
  float gradientXat(int x, int y, int z);
  float gradientYat(int x, int y, int z);
  float gradientZat(int x, int y, int z);

  // For adding together std::vectors of shared_ptr<VectorField>s
  // -1 means sum everything
  static VectorField sumVectorFields(std::vector< std::shared_ptr<VectorField> > &fields, int nToSum = -1);
};

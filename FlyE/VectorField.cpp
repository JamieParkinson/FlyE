#include "VectorField.h"
#include "PhysicalConstants.h"

// Constructors
VectorField::VectorField() {
}

VectorField::VectorField(int sizeX, int sizeY, int sizeZ) {
  this->resize(sizeX, sizeY, sizeZ);
}

VectorField::VectorField(const VectorField &vec)
    : blitz::Array<blitz::TinyVector<float, 3>, 3>(vec) {
}

// Operator overload
VectorField VectorField::operator =(const VectorField &rhs) {
  *this = rhs;
  return *this;
}

// Other useful methods
float VectorField::vectorMagnitude(blitz::TinyVector<float, 3> vec) {
  return static_cast<float>(sqrt(
      vec(0) * vec(0) + vec(1) * vec(1) + vec(2) * vec(2)));
}

float VectorField::magnitudeAt(int x, int y, int z) {
  return vectorMagnitude(this->operator()(x, y, z));
}

float VectorField::gradientXat(int x, int y, int z) {
  return 0.5 * Physics::MM_M_CORRECTION
      * (magnitudeAt(x + 1, y, z) - magnitudeAt(x - 1, y, z));
}

float VectorField::gradientYat(int x, int y, int z) {
  return 0.5 * Physics::MM_M_CORRECTION
      * (magnitudeAt(x, y + 1, z) - magnitudeAt(x, y - 1, z));
}

float VectorField::gradientZat(int x, int y, int z) {
  return 0.5 * Physics::MM_M_CORRECTION
      * (magnitudeAt(x, y, z + 1) - magnitudeAt(x, y, z - 1));
}

#include "VectorField.h"
#include "PhysicalConstants.h"

// Constructors
VectorField::VectorField() {}

VectorField::VectorField(int sizeX, int sizeY, int sizeZ) {
  this->resize(sizeX, sizeY, sizeZ);
}

VectorField::VectorField(const VectorField &vec) {
  *this = vec;
}

// Operator overload
VectorField VectorField::operator =(const VectorField &rhs) {
  *this = rhs;
  return *this;
}

// Other useful methods
static float VectorField::vectorMagnitude(const blitz::TinyVector<float, 3> &vec) {
  return static_cast<float>( sqrt(vec(0)*vec(0) + vec(1)*vec(1) + vec(2)*vec(2)) );
}

float VectorField::magnitudeAt(int x, int y, int z) {
  return vectorMagnitude(this->operator()(x, y, z));
}

float VectorField::gradientXat(int x, int y, int z) {
  return 0.5*Physics::MM_M_CORRECTION*(magnitudeAt(x+1, y, z) - magnitudeAt(x-1, y, z));
}

float VectorField::gradientYat(int x, int y, int z) {
  return 0.5*Physics::MM_M_CORRECTION*(magnitudeAt(x, y+1, z) - magnitudeAt(x, y-1, z));
}

float VectorField::gradientZat(int x, int y, int z) {
  return 0.5*Physics::MM_M_CORRECTION*(magnitudeAt(x, y, z+1) - magnitudeAt(x, y, z-1));
}

// TODO improve this? Template metaprogramming etc
static VectorField VectorField::sumVectorFields(std::vector< std::shared_ptr<VectorField> > &fields, int nToSum) {
  blitz::Array<blitz::TinyVector<float, 3>, 3> sumArray;
  sumArray.resize(fields[0]->shape()); // Make sure this VectorField is the right shape

  nToSum = (nToSum == -1) ? fields.size() : nToSum; // Sum all if no value of nToSum is given

#pragma omp parallel for
  for (VectorField field = fields.begin(); field < fields.begin() + nToSum; ++field) {
    sumArray += field;
  }

  return VectorField(sumArray);
}

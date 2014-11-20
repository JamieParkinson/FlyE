#include "VectorField.h"
#include "PhysicalConstants.h"

VectorField::VectorField(int sizeX, int sizeY, int sizeZ) {
  fieldArray_.resize(sizeX, sizeY, sizeZ);
  fieldArray_ = 0.0;
}

VectorField::VectorField(blitz::Array<blitz::TinyVector<float,3>, 3> blitzArray) :
    fieldArray_(blitzArray) {}

VectorField VectorField::operator +(VectorField vec) {
  return VectorField(fieldArray_ + vec.fieldArray_);
}

VectorField VectorField::operator -(VectorField vec) {
  return VectorField(fieldArray_ - vec.fieldArray_);
}

void VectorField::operator +=(VectorField vec) {
  fieldArray_ += vec.fieldArray_;
}

void VectorField::operator -=(VectorField vec) {
  fieldArray_ -= vec.fieldArray_;
}

VectorField VectorField::operator *(float scalar) {
  return VectorField(fieldArray_*scalar);
}

void VectorField::operator *=(float scalar) {
  fieldArray_ *= scalar;
}

static float VectorField::vectorMagnitude(blitz::TinyVector<float, 3> vec) {
  return static_cast<float>( sqrt(vec(0)*vec(0) + vec(1)*vec(1) + vec(2)*vec(2)) );
}

float VectorField::magnitude(int x, int y, int z) {
  return vectorMagnitude(fieldArray_(x, y, z));
}

float VectorField::gradientX(int x, int y, int z) {
  return 0.5*Physics::MM_M_CORRECTION*(magnitude(x+1, y, z) - magnitude(x-1, y, z));
}

float VectorField::gradientY(int x, int y, int z) {
  return 0.5*Physics::MM_M_CORRECTION*(magnitude(x, y+1, z) - magnitude(x, y-1, z));
}

float VectorField::gradientZ(int x, int y, int z) {
  return 0.5*Physics::MM_M_CORRECTION*(magnitude(x, y, z+1) - magnitude(x, y, z-1));
}

static VectorField VectorField::sumVectorFields(std::vector< std::shared_ptr<VectorField> > &fields, int nToSum) {
  blitz::Array<blitz::TinyVector<float, 3>, 3> sumArray;
  sumArray.resize(fields[0]->fieldArray_.shape()); // Make sure this VectorField is the right shape

  nToSum = (nToSum == -1) ? fields.size() : nToSum; // Sum all if no value of nToSum is given

#pragma omp parallel for
  for (VectorField field = fields.begin(); field < fields.begin() + nToSum; ++field) {
    sumArray += field.fieldArray_;
  }

  return VectorField(sumArray);
}

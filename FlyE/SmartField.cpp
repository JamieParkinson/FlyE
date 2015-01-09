#include "SmartField.h"

#include "PhysicalConstants.h"

SmartField::SmartField() {
}

SmartField::SmartField(std::vector<std::shared_ptr<Electrode> > electrodes)
    : electrodes_(electrodes) {
  magnitudeMemory_.reserve(electrodes_[0]->numElements());
}

blitz::TinyVector<float, 3> SmartField::at(int x, int y, int z) {
  blitz::TinyVector<float, 3> point(0.0);

  for (auto &electrode : electrodes_) {
    point += electrode->getVoltage() * (*electrode)(x, y, z);
  }

  return point;
}

blitz::TinyVector<float, 3> SmartField::operator ()(int x, int y, int z) {
  return this->at(x, y, z);
}

blitz::TinyVector<float, 3> SmartField::operator ()(tuple3Dint r) {
  return this->at(std::get<0>(r), std::get<1>(r), std::get<2>(r));
}

blitz::TinyVector<float, 3> SmartField::at(tuple3Dint r) {
  return this->at(std::get<0>(r), std::get<1>(r), std::get<2>(r));
}

float SmartField::magnitudeAt(int x, int y, int z) {
 tuple3Dint t = std::make_tuple(x, y, z);

 if (magnitudeMemory_.count(t) == 0) {
#pragma omp critical
   {
     magnitudeMemory_.insert({{t, VectorField::vectorMagnitude(this->at(x, y, z))}});
   }
 }
 return magnitudeMemory_[t];
}

float SmartField::magnitudeAt(tuple3Dint t) {
 if (magnitudeMemory_.count(t) == 0) {
#pragma omp critical
   {
     magnitudeMemory_.insert({{t, VectorField::vectorMagnitude(this->at(t))}});
   }
 }
 return magnitudeMemory_[t];
}

float SmartField::gradientXat(int x, int y, int z) {
  return 0.5 * Physics::MM_M_FACTOR
      * (magnitudeAt(x + 1, y, z) - magnitudeAt(x - 1, y, z));
}

float SmartField::gradientXat(tuple3Dint r) {
  return gradientXat(std::get<0>(r), std::get<1>(r), std::get<2>(r));
}

float SmartField::gradientYat(int x, int y, int z) {
  return 0.5 * Physics::MM_M_FACTOR
      * (magnitudeAt(x, y + 1, z) - magnitudeAt(x, y - 1, z));
}

float SmartField::gradientYat(tuple3Dint r) {
  return gradientYat(std::get<0>(r), std::get<1>(r), std::get<2>(r));
}

float SmartField::gradientZat(int x, int y, int z) {
  return 0.5 * Physics::MM_M_FACTOR
      * (magnitudeAt(x, y, z + 1) - magnitudeAt(x, y, z - 1));
}

float SmartField::gradientZat(tuple3Dint r) {
  return gradientZat(std::get<0>(r), std::get<1>(r), std::get<2>(r));
}

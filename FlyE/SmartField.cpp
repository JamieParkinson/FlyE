#include "SmartField.h"

#include "PhysicalConstants.h"

SmartField::SmartField(std::vector<std::shared_ptr<Electrode> > electrodes)
    : electrodes_(electrodes) {
}

blitz::TinyVector<float, 3> SmartField::at(int x, int y, int z) {
  blitz::TinyVector<float, 3> point;

  for (auto &electrode : electrodes_) {
    point += electrode->getVoltage() * electrode->operator()(x, y, z);
  }

  return point;
}

blitz::TinyVector<float, 3> SmartField::operator ()(int x, int y, int z) {
  return this->at(x, y, z);
}

float SmartField::magnitudeAt(int x, int y, int z) {
  return VectorField::vectorMagnitude(this->at(x, y, z));
}

float SmartField::gradientXat(int x, int y, int z) {
  return 0.5 * Physics::MM_M_CORRECTION
      * (magnitudeAt(x + 1, y, z) - magnitudeAt(x - 1, y, z));
}

float SmartField::gradientYat(int x, int y, int z) {
  return 0.5 * Physics::MM_M_CORRECTION
      * (magnitudeAt(x, y + 1, z) - magnitudeAt(x, y - 1, z));
}

float SmartField::gradientZat(int x, int y, int z) {
  return 0.5 * Physics::MM_M_CORRECTION
      * (magnitudeAt(x, y, z + 1) - magnitudeAt(x, y, z - 1));
}

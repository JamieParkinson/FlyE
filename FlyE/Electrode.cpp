#include "Electrode.h"

Electrode::Electrode(int electrodeNumber) : electrodeNumber_(electrodeNumber), currentVoltage_(1.0) {}

void Electrode::applyVoltage(float voltage) {
  currentVoltage_ = voltage;
}

blitz::TinyVector<float, 3> Electrode::operator ()(int x, int y, int z) {
  return currentVoltage_ * this->operator()(x, y, z);
}

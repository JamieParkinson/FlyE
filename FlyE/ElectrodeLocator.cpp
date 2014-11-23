#include "ElectrodeLocator.h"
#include "Electrode.h"

ElectrodeLocator::ElectrodeLocator() {
}

// Deeply suspicious code. Exploits built-in implicit boolean type conversion.
// is TRUE where the y-component of the Electrode's field is === zero
ElectrodeLocator::ElectrodeLocator(Electrode &electrode)
    : blitz::Array<bool, 3>(!electrode.extractComponent(float(), 1, 3)) {
}

ElectrodeLocator::ElectrodeLocator(std::shared_ptr<Electrode> electrode)
    : blitz::Array<bool, 3>(!electrode->extractComponent(float(), 1, 3)) {
}

bool ElectrodeLocator::existsAt(int x, int y, int z) {
  return this->operator()(x, y, z);
}

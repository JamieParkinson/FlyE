#pragma once

#include <blitz/array-impl.h>

#include "Electrode.h"

class ElectrodeLocator : public blitz::Array<bool, 3>{
 public:
  ElectrodeLocator(Electrode &electrode);
  bool existsAt(int x, int y, int z);
};

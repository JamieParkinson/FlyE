#pragma once

#include "Electrode.h"

class SmartField {
 protected:
  std::vector< std::shared_ptr<Electrode> > electrodes_;

 public:
  SmartField(std::vector< std::shared_ptr<Electrode> > electrodes);

  blitz::TinyVector<float, 3> at(int x, int y, int z);
  blitz::TinyVector<float, 3> operator ()(int x, int y, int z);

  float magnitudeAt(int x, int y, int z);

  float gradientXat(int x, int y, int z);
  float gradientYat(int x, int y, int z);
  float gradientZat(int x, int y, int z);
};

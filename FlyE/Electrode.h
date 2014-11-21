/*
 * A subclass of VectorField - gains some electrode-specific capabilities
 */
#pragma once

#include "VectorField.h"

class Electrode : public VectorField {
 public:
  bool import();

  void setVoltage(float voltage);
};


#pragma once

#include <memory>
#include <vector>

#include "Electrode.h"

class AcceleratorConfig;

class AcceleratorGeometry {
 protected:
  std::vector< std::shared_ptr<Electrode> > electrodes_;
  std::shared_ptr<AcceleratorConfig> config_;

 public:
  AcceleratorGeometry(std::shared_ptr<AcceleratorConfig> config);

  void importElectrodes();

  void applyElectrodeVoltages(std::vector<float> voltages);
};

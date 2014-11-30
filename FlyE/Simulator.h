#pragma once

#include <vector>
#include <memory>
#include <ostream>

#include "AcceleratorGeometry.h"
#include "AntiHydrogen.h"
#include "SmartField.h"
#include "SubConfig.h"
#include "VoltageScheme.h"

struct SimulationNumbers {
  int nCollided, nIonised, nSucceeded, nNeutralised;

  friend std::ostream& operator <<(std::ostream &out, SimulationNumbers &simNums);
};

class Simulator {
  friend class Writer;
 protected:
  AcceleratorGeometry geometry_;
  std::vector<AntiHydrogen> particles_;

  std::shared_ptr<SimulationConfig> simulationConfig_;
  std::shared_ptr<AcceleratorConfig> acceleratorConfig_;
  std::shared_ptr<StorageConfig> storageConfig_;

  SmartField field_;
  VoltageScheme *voltageScheme_;

  SimulationNumbers statsStorage_;

 public:
  Simulator(AcceleratorGeometry &geometry, std::vector<AntiHydrogen> &particles,
            std::shared_ptr<SimulationConfig> simulationConfig,
            std::shared_ptr<StorageConfig> storageConfig);

  void run();

  SimulationNumbers getBasicStats();

  void write(std::string fileName);
};

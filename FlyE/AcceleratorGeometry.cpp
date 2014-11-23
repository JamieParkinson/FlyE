#include "AcceleratorGeometry.h"

#include "SubConfig.h"

AcceleratorGeometry::AcceleratorGeometry(std::shared_ptr<AcceleratorConfig> config) : config_(config){
  electrodes_.reserve(config->nElectrodes());
}

void AcceleratorGeometry::importElectrodes() {
  for (int e = 0; e < config_->nElectrodes(); ++e) {
    electrodes_.emplace_back(std::make_shared<Electrode>(e));
    electrodes_[e]->import(config_);
  }
}

void AcceleratorGeometry::applyElectrodeVoltages(std::vector<float> voltages) {
  for (unsigned int e = 0; e < voltages.size(); ++e) {
    electrodes_[e]->applyVoltage(voltages[e]);
  }
}

SmartField AcceleratorGeometry::makeSmartField() {
  return SmartField(electrodes_);
}

ElectrodeLocator AcceleratorGeometry::electrodeLocations() {
  ElectrodeLocator locator;

  for (auto &electrode : electrodes_) {
    locator += ElectrodeLocator(electrode);
  }

  return locator;
}

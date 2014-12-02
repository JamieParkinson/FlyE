#include "AcceleratorGeometry.h"

#include "SubConfig.h"
#include "ezETAProgressBar.hpp"

AcceleratorGeometry::AcceleratorGeometry(
    std::shared_ptr<AcceleratorConfig> config)
    : config_(config) {
  electrodes_.reserve(config->nElectrodes());
}

void AcceleratorGeometry::importElectrodes() {
  std::cout << "Importing " << config_->nElectrodes() << " electrodes..." << std::endl;

  ez::ezETAProgressBar importBar(config_->nElectrodes());
  importBar.start();

  for (int e = 0; e < config_->nElectrodes(); ++e, ++importBar) {
    electrodes_.emplace_back(std::make_shared<Electrode>(e + 1));
    electrodes_[e]->import(config_);
  }

  std::cout << std::endl;
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
  ElectrodeLocator locator(electrodes_[0]);

  for (auto electrode = electrodes_.begin() + 1; electrode < electrodes_.end(); ++electrode) {
    locator += ElectrodeLocator(*electrode);
  }

  return locator;
}

std::shared_ptr<AcceleratorConfig> AcceleratorGeometry::getAcceleratorConfig() {
  return config_;
}

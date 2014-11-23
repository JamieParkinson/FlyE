#include "Electrode.h"

#include <blitz/array.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "PhysicalConstants.h"

Electrode::Electrode() : electrodeNumber_(0), currentVoltage_(0) {}

Electrode::Electrode(int electrodeNumber)
    : electrodeNumber_(electrodeNumber),
      currentVoltage_(1.0) {
}

Electrode::Electrode(const Electrode &elec)
    : VectorField(elec),
      electrodeNumber_(elec.currentVoltage_),
      currentVoltage_(elec.currentVoltage_){
}

Electrode Electrode::operator =(const Electrode &rhs) {
  *this = rhs;
  return *this;
}

void Electrode::applyVoltage(float voltage) {
  currentVoltage_ = voltage;
}

void Electrode::import(std::shared_ptr<AcceleratorConfig> config) {
  for (int x = 0; x < config->x(); ++x) {  // Layers (excluding nx-th and first)

    std::string piece;
    std::string line;
    std::stringstream basePath;
    std::stringstream path;

    // width+2 to correct zero-indexing, and (nElectrodes - electrodeNumber_ + 1) to correct weird backwards numbering
    basePath << config->datDirectory() << config->PAname() << "_E"
             << config->nElectrodes() - electrodeNumber_ + 1 << "_L" << x + 2
             << "_";

    for (int d = 0; d < Physics::N_DIMENSIONS; ++d) {
      path << basePath << Physics::axes[d] << ".dat";
      blitz::Array<float, 3> thisDimension = this->extractComponent(float(), d,
                                                                    3);  // Get this dimension

      std::ifstream datFile(path.str().c_str());

      if (datFile.fail()) {
        std::cout << "Error reading .dat file: " << path << std::endl;
        continue;
      }

      int y = 0;
      int z = -8;  // Skip header text

      while (getline(datFile, line)) {  // Lines ('\n' delimiter)
        if (z >= 0) {  // Ignore header text
          y = 0;
          std::stringstream linestream(line);
          while (getline(linestream, piece, '\t')) {  // Tabs (ie cells)
            thisDimension(x, y, z) = Physics::SIMION_CORRECTION * stod(piece);  // Ugly access to overloaded parentheses
            ++y;
          }
        }
        ++z;
      }
      datFile.close();

      this->extractComponent(float(), d, 3) = thisDimension;  // Put this dimension back
    }
  }
}

float Electrode::getVoltage() {
  return currentVoltage_;
}

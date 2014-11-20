/* Source for Espace class */

#include "Espace.h"
#include "PhysicalConstants.h"

Espace::Espace(const params &config)
    : config_(config) {
  field_.resize(config_.spaceSize);
  normField_.resize(config_.spaceSize);
}
;

float Espace::gradientX(const int x, const int y, const int z) {
  return Physics::MM_M_CORRECTION * (magAt(x + 1, y, z) - magAt(x - 1, y, z)) / 2;
}

float Espace::gradientY(const int x, const int y, const int z) {
  return Physics::MM_M_CORRECTION * (magAt(x, y + 1, z) - magAt(x, y - 1, z)) / 2;
}

float Espace::gradientZ(const int x, const int y, const int z) {
  return Physics::MM_M_CORRECTION * (magAt(x, y, z + 1) - magAt(x, y, z - 1)) / 2;
}

float& Espace::at(const int x, const int y, const int z, const int d) {
  return field_[d + Physics::N_DIMENSIONS * z + Physics::N_DIMENSIONS * config_.z * y
      + Physics::N_DIMENSIONS * config_.z * config_.y * x];
}

float& Espace::normAt(const int x, const int y, const int z, const int d) {
  return normField_[d + Physics::N_DIMENSIONS * z + Physics::N_DIMENSIONS * config_.z * y
      + Physics::N_DIMENSIONS * config_.z * config_.y * x];
}

float Espace::magAt(const int x, const int y, const int z) {
  int cPart = Physics::N_DIMENSIONS * z + Physics::N_DIMENSIONS * config_.z * y
      + Physics::N_DIMENSIONS * config_.z * config_.y * x;
  return sqrt(
      pow(field_[cPart], 2) + pow(field_[cPart + 1], 2)
          + pow(field_[cPart + 2], 2));
}

Espace& Espace::operator+=(const Espace& b) {  // Allows array addition
  for (auto &point : field_) {
    point += b.field_[&point - &field_[0]];
  }
  return *this;
}

Electrode::Electrode(params &config)
    : Espace(config),
      voltage_(0) {
}

Electrode::Electrode(int n, params &config)
    : Espace(config) {
  import(n);
}

void Electrode::import(int n) {
#pragma omp parallel for // Parallelisation
  for (int x = 0; x <= config_.x - 1; ++x) {  // Layers (excluding nx-th and first)
    std::string piece;
    std::string line;
    std::string datPath = std::string(config_.datDirectory) + config_.paName
        + "_E" + std::to_string(config_.nElectrodes - n + 1) + "_L"
        + std::to_string(x + 2) + "_";  // width+2 to correct zero-indexing, and (nElectrodes - n + 1) to correct weird backwards numbering

    for (int d = 0; d < Physics::N_DIMENSIONS; ++d) {
      std::string datFilePath = datPath + dimensions[d] + ".dat";
      std::ifstream datFile(datFilePath.c_str());
      if (datFile.fail()) {
        std::cout << "Error reading .dat file: " << datFilePath << std::endl;
        continue;
      }

      int y = 0;
      int z = -8;  // Skip header text
      while (getline(datFile, line)) {  // Lines ('\n' delimiter)
        if (z >= 0) {  // Ignore header text
          y = 0;
          std::stringstream linestream(line);
          while (getline(linestream, piece, '\t')) {  // Tabs (ie cells)
            normAt(x, y, z, d) = Physics::SIMION_CORRECTION * stod(piece);  // Ugly access to overloaded parentheses
            ++y;
          }
        }
        ++z;
      }
      datFile.close();
    }
  }
}

void Electrode::setVoltage(float v) {
  voltage_ = v;
  field_ = normField_;
  for (auto &point : field_) {
    point *= v;
  }
}

Espace Electrode::sumElectrodes(
    const std::vector<std::shared_ptr<Electrode> > &electrodes, params &config,
    int nToSum) {
  Espace sumSpace(config);

  if (nToSum == 0)
    nToSum = config.nElectrodes;  // Sum all by default

#pragma omp parallel for
  for (auto electrode = electrodes.begin();
      electrode < electrodes.begin() + nToSum; ++electrode) {
    sumSpace += *(*electrode);
  }
  return sumSpace;
}


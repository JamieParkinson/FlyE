#pragma once

#include <H5Cpp.h>
#include "Simulator.h"

class Writer {
 protected:
  Simulator *simulator_;

  H5::H5File *outFile_;
  H5::FloatType fType_;
  H5::IntType iType_;
  H5::DSetCreatPropList dPropList_;

  std::vector<std::string> typeNames_ { "Succeeded", "Collided", "Ionised",
      "Remaining" };
  std::vector<int> nParticlesOfType_;

  std::vector<std::shared_ptr<H5::DataSet> > dSets_;  // Storing datasets
  std::vector<std::shared_ptr<H5::DataSet> > ntimeDSets_;  // For neutralisation times
  std::vector<std::shared_ptr<H5::DataSet> > kDSets_;  // For k-values

  std::vector<H5::DataSpace> dSpaces_;  // Storing dataspaces
  std::vector<H5::DataSpace> ntimeDSpaces_;  // For neutralisation times
  std::vector<H5::DataSpace> kDSpaces_;  // For k-values

  std::vector< std::vector<int> > ntimeVecs_;  // For neutralisation time vectors
  std::vector< std::vector<int> > kVecs_;  // For k-value vectors

  std::vector<int> pTypeCounts_ = { 0, 0, 0, 0, 0 };  // Particle type counters

  static hsize_t *startParams(int dimension, int phaseCoord, int particleIndex);
  static hsize_t *countParams(Particle &particle);

 public:
  Writer(std::string &fileName, Simulator *simulator);

  void initializeSetsAndSpaces();

  void writeParticles();
};

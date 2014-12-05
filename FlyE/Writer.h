#pragma once

#include <H5Cpp.h>
#include "Simulator.h"

template<typename T>
struct HDF5Container1D {
  std::vector< std::shared_ptr<H5::DataSet> > dSets;
  std::vector<H5::DataSpace> dSpaces;
  std::vector< std::vector<T> > vecs;

  HDF5Container1D() : vecs(4) {
  }
};

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

  std::vector<std::shared_ptr<H5::DataSet> > trajectoryDSets_;  // Storing datasets
  std::vector<H5::DataSpace> trajectoryDSpaces_;  // Storing dataspaces

  HDF5Container1D<int> nTimes_;
  HDF5Container1D<int> ks_;
  HDF5Container1D<float> maxFields_;

  std::vector<int> pTypeCounts_ = { 0, 0, 0, 0 };  // Particle type counters

  static hsize_t *startParams(int dimension, int phaseCoord, int particleIndex);
  static hsize_t *countParams(Particle &particle);

 public:
  Writer(std::string &fileName, Simulator *simulator);

  void initializeSetsAndSpaces();

  void writeParticles();
};

/**@file Writer.h
 * @brief This file contains the Writer class and the HDF5Container1D struct
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
#pragma once

#include <H5Cpp.h>
#include "Simulator.h"

/** @brief A basic container for the HDF5 components for the scalar data that will be stored
 *
 * Hard to describe.
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
template<typename T>
struct HDF5Container1D {
  std::vector< std::shared_ptr<H5::DataSet> > dSets; //!< A vector of shared_ptrs to HDF5 DataSets
  std::vector<H5::DataSpace> dSpaces; //!< A vector of HDF5 DataSpaces
  std::vector< std::vector<T> > vecs; //!< A vector of vectors of values (of type T)

  //!< Construct the container with 4 vectors in vecs
  HDF5Container1D() : vecs(4) {}

  /** @brief Closes all HDF5 elements */
  void close() {
    for (int i = 0; i < 4; ++i) {
      dSpaces[i].close();
    }
    dSets.clear();
  }
};

/** @brief A class for writing the simulation data out to a file
 *
 * The HDF5 C++ library is really, really bad. The code for this is impossible to understand.
 * It's also extremely fast.
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class Writer {
 protected:
  Simulator *simulator_; //!< A pointer to a Simulator. Safe because the Simulator calls this and manages itself.

  H5::H5File *outFile_; //!< A pointer to an HDF5 file object
  H5::FloatType fType_; //!< The HDF5 native float type
  H5::IntType iType_; //!< The HDF5 native integer type
  H5::DSetCreatPropList dPropList_; //!< HDF5 properties list - for memory chunking

  std::vector<std::string> typeNames_ { "Succeeded", "Collided", "Ionised",
      "Remaining" }; //!< Vector of strings for the particle type names
  std::vector<int> nParticlesOfType_; //!< The number of particles of each type

  std::vector<std::shared_ptr<H5::DataSet> > trajectoryDSets_; //!< For storing each type of particle's DataSets
  std::vector<H5::DataSpace> trajectoryDSpaces_; //!< For storing each type of particle's DataSpaces

  HDF5Container1D<int> nTimes_; //!< For storing everything related to neutralisation times
  HDF5Container1D<int> ks_; //!< For storing everything related to k-values
  HDF5Container1D<float> maxFields_; //!< For storing everything related to the maximum field values each particle encounters

  std::vector<int> pTypeCounts_ = { 0, 0, 0, 0 };  //!< Counters for each particle type

  /** @brief Returns the appropriate hsize_t[] for selecting particle hyperslabs start values
   *
   * @param dimension The dimension we're working in (0 = x, 1 = y, 2 = z)
   * @param phaseCoord The coordinate of the dimension of phase space we're working in (0 = position, 1 = velocity)
   * @param particleIndex The index of this particle in the vector of Particles
   * @return Pointer to the first element of an hsize_t array of values
   */
  static hsize_t *startParams(int dimension, int phaseCoord, int particleIndex);

  /** @brief Returns the appropriate hsize_t[] for selecting particle hyperslab count values
   *
   * @param particle The Particle that we have selected.
   * @return Pointer to the first element of an hsize_t array of values
   */
  static hsize_t *countParams(Particle &particle);

 public:
  /** @brief Constructs a writer that will write the data of simulator to fileName
   *
   * @param fileName The path to write the data to
   * @param simulator The Simulator to take data from
   */
  Writer(std::string &fileName, Simulator *simulator);

  //!< Terminates all the messy HDF5 pieces nicely
  ~Writer();

  /** @brief Initializes the HDF5 DataSets and DataSpaces to be used in writing */
  void initializeSetsAndSpaces();

  /** @brief Writes the simulation data to file */
  void writeParticles();
};

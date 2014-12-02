#include "Writer.h"

#include "PhysicalConstants.h"
#include "ezETAProgressBar.hpp"

Writer::Writer(std::string &fileName, Simulator *simulator)
    : simulator_(simulator),
      outFile_(new H5::H5File(fileName.c_str(), H5F_ACC_TRUNC)),
      fType_(H5::PredType::NATIVE_FLOAT),
      iType_(H5::PredType::NATIVE_INT),
      nParticlesOfType_(  // Don't look at this, it's awful
          { simulator_->statsStorage_.nSucceeded, simulator_->statsStorage_
              .nCollided, simulator_->statsStorage_.nIonised, (int) simulator_
              ->particles_.size() - simulator_->statsStorage_.nSucceeded
              - simulator_->statsStorage_.nCollided
              - simulator_->statsStorage_.nIonised }),
      ntimeVecs_(4),
      kVecs_(4) {
  fType_.setOrder(H5T_ORDER_LE);  // Little endian
  iType_.setOrder(H5T_ORDER_LE);

  hsize_t dChunkDims[4];  // We want out datasets chunked by particle dimensions across time (like our memory vecs)
  dChunkDims[0] = 1;
  dChunkDims[1] = 1;
  dChunkDims[2] =  // Number of time steps if we're storing full trajectories, 2 if not
      (simulator_->storageConfig_->storeTrajectories()) ?
          (simulator_->simulationConfig_->duration()
              / simulator_->simulationConfig_->timeStep()) + 1 :
          2;
  dChunkDims[3] = 1;
  dPropList_.setChunk(4, dChunkDims);  // Set up chunking - MASSIVE performance increase
}

hsize_t *Writer::startParams(int dimension, int phaseCoord, int particleIndex) {
  static hsize_t start[4];

  start[0] = dimension;
  start[1] = phaseCoord;  // Position or velocity
  start[2] = 0;  // time
  start[3] = particleIndex;

  return start;
}

hsize_t *Writer::countParams(Particle &particle) {
  static hsize_t count[4];

  count[0] = 1;
  count[1] = 1;
  count[2] = (int) particle.recallLoc(0).size();
  count[3] = 1;

  return count;
}

void Writer::initializeSetsAndSpaces() {
  H5::Group* typeGroup;

  for (int type = 0; type < 4; ++type) {  // Initialise all my groups and datasets
    typeGroup = new H5::Group(outFile_->createGroup("/" + typeNames_[type]));

    hsize_t dataDims[4];  // I write these in reverse order because it seems that this is how things turn out in MATLAB
    dataDims[3] = nParticlesOfType_[type];  // Particle
    dataDims[2] =
        (simulator_->storageConfig_->storeTrajectories()) ?
            (simulator_->simulationConfig_->duration()
                / simulator_->simulationConfig_->timeStep()) + 1 :
            2;
    dataDims[1] = 2;  // Position and velocity
    dataDims[0] = Physics::N_DIMENSIONS;  // x, y, z

    hsize_t scalarDims[1];  // For neutralisation times and k-values
    scalarDims[0] = nParticlesOfType_[type];

    dSpaces_.emplace_back(4, dataDims);
    ntimeDSpaces_.emplace_back(1, scalarDims);
    kDSpaces_.emplace_back(1, scalarDims);

    ntimeVecs_[type].reserve(nParticlesOfType_[type]);
    kVecs_[type].reserve(nParticlesOfType_[type]);

    dSets_.emplace_back(
        std::make_shared<H5::DataSet>(
            outFile_->createDataSet("/" + typeNames_[type] + "/data", fType_,
                                    dSpaces_[type], dPropList_)));  // Initialise dataset
    ntimeDSets_.emplace_back(
        std::make_shared<H5::DataSet>(
            outFile_->createDataSet("/" + typeNames_[type] + "/neutralTimes",
                                    iType_, ntimeDSpaces_[type])));

    kDSets_.emplace_back(
        std::make_shared<H5::DataSet>(
            outFile_->createDataSet("/" + typeNames_[type] + "/ks", iType_,
                                    kDSpaces_[type])));
  }

  delete typeGroup;
}

void Writer::writeParticles() {
  int pType;

  hsize_t stride[4];
  stride[0] = 1;
  stride[1] = 1;
  stride[2] = 1;
  stride[3] = 1;

  std::cout << "Writing data file (" << outFile_->getFileName() << ")..." << std::endl;

  ez::ezETAProgressBar writerBar(static_cast<int>(simulator_->particles_.size()));
  writerBar.start();

  for (AntiHydrogen &particle : simulator_->particles_) {  // Look through particles
    ++writerBar;
    if (particle.succeeded()) {  // Determine particle type
      pType = 0;
    } else if (particle.isDead()) {
      pType = particle.isDead();
    } else {
      pType = 3;
    }

    if (pType == 1 && !simulator_->storageConfig_->storeCollisions()) {  // If we're discarding collisions
      continue;
    }

    H5::DataSpace fileSpace = dSpaces_[pType];

    hsize_t *count = countParams(particle);

    hsize_t memDims[1];
    memDims[0] = count[2];
    H5::DataSpace memSpace = H5::DataSpace(1, memDims);

    for (int d = 0; d < Physics::N_DIMENSIONS; ++d) {
      hsize_t *start = startParams(d, 0, pTypeCounts_[pType]);

      fileSpace.selectHyperslab(H5S_SELECT_SET, count, start, stride);
      dSets_[pType]->write(particle.recallLoc(d).data(), fType_, memSpace,
                           fileSpace);

      start[1] = 1;  // Velocity
      fileSpace.selectHyperslab(H5S_SELECT_SET, count, start, stride);
      dSets_[pType]->write(particle.recallVel(d).data(), fType_, memSpace,
                           fileSpace);
    }

    ++pTypeCounts_[pType];  // Increase out count of this particle type

    ntimeVecs_[pType].emplace_back(particle.neutralisationTime());  // Store ntime
    kVecs_[pType].emplace_back(particle.k());  // Store k

    particle.forget();  // Clear the memory of the particle we just dealt with
  }

  for (int type = 0; type < 4; ++type) {  // Write neutralisation times & k-values
    ntimeDSets_[type]->write(ntimeVecs_[type].data(), iType_);
    kDSets_[type]->write(kVecs_[type].data(), iType_);
  }

  std::cout << std::endl;

  delete outFile_;
}

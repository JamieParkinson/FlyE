#ifndef MYFUNC_INCLUDE_GUARD
#define MYFUNC_INCLUDE GUARD

#include <iostream>
#include <fstream>
#include <sys/sysinfo.h>
#include <libconfig.h++>

#ifndef N_DIMENSIONS
#define N_DIMENSIONS 3
#endif

#ifndef N_TYPES
#define N_TYPES 4
#endif

using namespace std;

#include "myStructs.h"

// Global variables
extern float timeStep, duration, maxVoltage, sigmaX, sigmaY, sigmaZ, temperature, targetVel, targetVelPrecision;
extern bool normDist, storeCollisions, storeTrajectories, inglisTeller;
extern int nParticles, n, k;
extern string outDir;
extern params config;
extern accelerationSchemes scheme;
extern kDists kDist;

// Uses libconfig++ to read in my config file
void readConfig(const string confFilePath) {
  libconfig::Config myConf;
  myConf.readFile(confFilePath.c_str());
  myConf.setAutoConvert(true);
  const libconfig::Setting& c = myConf.getRoot();

  config.nElectrodes = c["espace"]["n_electrodes"];
  config.nx = c["espace"]["dimensions"]["x"];
  config.ny = c["espace"]["dimensions"]["y"];
  config.nz = c["espace"]["dimensions"]["z"];
  config.x = config.nx - 2;
  config.y = config.ny - 2;
  config.z = config.nz - 2;
  config.datDirectory = c["espace"]["dat_directory"].c_str();
  config.paName = c["espace"]["pa_name"].c_str();
  config.spaceSize = config.x * config.y * config.z * N_DIMENSIONS;

  timeStep = c["simulation"]["time_step"];
  duration = c["simulation"]["duration"];
  nParticles = c["simulation"]["n_particles"];
  maxVoltage = c["simulation"]["max_voltage"];
  scheme = (c["simulation"]["accel_scheme"] == "exponential") ?
      Exponential : (c["simulation"]["accel_scheme"] == "instantaneous") ?
      Instantaneous :
      Trap;
  inglisTeller = c["simulation"]["inglis_teller"];

  normDist = c["norm_dist"];
  sigmaX = c["sigma"]["x"];
  sigmaY = c["sigma"]["y"];
  sigmaZ = c["sigma"]["z"];

  temperature = c["particles"]["temperature"];
  n = c["particles"]["n"];
  k = c["particles"]["k"];
  kDist = (c["particles"]["k_dist"] == "uniform") ?
      Uniform : (c["particles"]["k_dist"] == "single") ?
      Single :
      Triangle;

  targetVel = c["target_v"];
  targetVelPrecision = c["target_v_precision"];

  outDir = c["storage"]["output_dir"].c_str();
  storeCollisions = c["storage"]["store_collisions"];
  storeTrajectories = c["storage"]["store_trajectories"];
}

// Uses the very very bad HDF5 library to write to HDF5. Awful code, ugly, impossible to understand. Very fast.
void writeHDF5(vector<Particle> &particles, const string &outFilePath,
               const int &nTimeSteps,
               const int &nSucceeded, const int &nIonised, const int &nCollided,
               const int &nParticles) {
  H5::H5File* outFile = new H5::H5File(outFilePath.c_str(), H5F_ACC_TRUNC);  // Initialise data file

  H5::FloatType dType(H5::PredType::NATIVE_FLOAT);  // Type of data (native float)
  H5::IntType iType(H5::PredType::NATIVE_INT);
  dType.setOrder(H5T_ORDER_LE);  // Little endian
  iType.setOrder(H5T_ORDER_LE);
  H5::DSetCreatPropList dPropList;  // Properties list
  hsize_t dChunkDims[4];  // We want out datasets chunked by particle dimensions across time (like our memory vecs)
  dChunkDims[0] = 1;
  dChunkDims[1] = 1;
  dChunkDims[2] = nTimeSteps + 1;
  dChunkDims[3] = 1;
  dPropList.setChunk(4, dChunkDims);  // Set up chunking - MASSIVE performance increase

  vector<int> nParticlesOfType;
  vector<string> typeNames;

  nParticlesOfType = {nSucceeded, nCollided, nIonised,
    nParticles - nSucceeded - nCollided - nIonised};
  typeNames = {"Succeeded", "Collided", "Ionised",
    "Remaining"};

  vector<shared_ptr<H5::DataSet> > dSets;  // Storing datasets
  vector<shared_ptr<H5::DataSet> > ntimeDSets; // For neutralisation times
  vector<H5::DataSpace> dSpaces;  // Storing dataspaces
  vector<H5::DataSpace> ntimeDSpaces; // For neutralisation times
  vector< vector<int> > ntimeVecs(N_TYPES); // For neutralisation time vectors
  vector<int> pTypeCounts = { 0, 0, 0, 0, 0 };  // Particle type counters
  int pType;  // Will be used to select elements of above vectors

  H5::Group* typeGroup;
  for (int type = 0; type < N_TYPES; ++type) {  // Initialise all my groups and datasets
    typeGroup = new H5::Group( outFile->createGroup("/" + typeNames[type]));

    hsize_t dataDims[4];  // I write these in reverse order because it seems that this is how things turn out in MATLAB
    dataDims[3] = nParticlesOfType[type];  // Particle
    dataDims[2] = nTimeSteps + 1;  // Time
    dataDims[1] = 2;  // Position and velocity
    dataDims[0] = N_DIMENSIONS;  // x, y, z

    hsize_t ntimeDims[1]; // For neutralisation times
    ntimeDims[0] = nParticlesOfType[type];

    dSpaces.emplace_back(4, dataDims);
    ntimeDSpaces.emplace_back(1, ntimeDims);

    ntimeVecs[type].reserve(nParticlesOfType[type]);

    dSets.emplace_back(
        make_shared<H5::DataSet>(
            outFile->createDataSet("/" + typeNames[type] + "/data", dType, dSpaces[type],
                                   dPropList)));  // Initialise dataset
    ntimeDSets.emplace_back(
        make_shared<H5::DataSet>(
            outFile->createDataSet("/" + typeNames[type] + "/neutralTimes", iType, ntimeDSpaces[type])));
  }

  for (Particle &particle : particles) {  // Look through particles
    if (particle.succeeded()) {  // Determine particle type
      pType = 0;
    } else if (particle.isDead()) {
      pType = particle.isDead();
    } else {
      pType = N_TYPES - 1;
    }

    if (pType == 1 && !storeCollisions) {
      continue;
    }

    H5::DataSpace fileSpace = dSpaces[pType];

    // For the following we'll need to change start[1] for pos/vel and start[0] for dimension
    hyperslabParams thisParams = particle.makeParameters(pTypeCounts[pType]);

    hsize_t memDims[1];
    memDims[0] = thisParams.count[2];
    H5::DataSpace memSpace = H5::DataSpace(1, memDims);

    for (int d = 0; d < N_DIMENSIONS; ++d) {
      thisParams.start[0] = d;

      thisParams.start[1] = 0;  // Position
      fileSpace.selectHyperslab(H5S_SELECT_SET, thisParams.count,
                                thisParams.start, thisParams.stride);
      dSets[pType]->write(particle.recallLoc(d).data(), dType, memSpace,
                          fileSpace);

      thisParams.start[1] = 1;  // Velocity
      fileSpace.selectHyperslab(H5S_SELECT_SET, thisParams.count,
                                thisParams.start, thisParams.stride);
      dSets[pType]->write(particle.recallVel(d).data(), dType, memSpace,
                          fileSpace);
    }

    ++pTypeCounts[pType];  // Increase out count of this particle type

    ntimeVecs[pType].emplace_back(particle.neutralisationTime()); // Store ntime

    particle.forget();  // Clear the memory of the particle we just dealt with

    cout << &particle - &particles[0] << "/" << nParticles << " exported\r"
         << flush;
  }

  for (int type = 0; type < N_TYPES; ++type) { // Write neutralisation times
    ntimeDSets[type]->write(ntimeVecs[type].data(), iType);
  }

  delete typeGroup;
  delete outFile;
}

// Experimental for possibility of dumping data if RAM is too full. Only works on Linux.
bool checkMemFull() {
  struct sysinfo info;
  sysinfo(&info);
  return (1 - (info.freeram / static_cast<float>(info.totalram)) > 0.93) ? true : false;
}

#endif

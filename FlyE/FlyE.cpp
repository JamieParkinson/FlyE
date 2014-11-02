/* 
 * Program to fly Rydberg H atoms through a time-varying electric field
 * 
 * Various options are configured in a given .conf file (give the path as an argument to this)
 * Electrode configuration is a split hollow cylinder, specified in splitcylinder.gem
 * makefields.py generates the .dat files from SIMION .pa files
 * 
 * 07/2014 Jamie Parkinson
 * 
 */

/*
 * Config file guide:
 *
 * N_ELECTRODES: Number of individual electrodes in the arrangement
 * [X|Y|Z]_DIMENSION: The dimensions of the matrices in the .dat files
 * DAT_DIRECTORY: Path to the directory in which the .dat files are stored
 * PA_NAME: The prefix to the dat file names eg [PA_NAME]_E1_L2_X.dat
 * TIME_STEP: Time step to run simulation with (s)
 * DURATION: Number of seconds to run simulation for
 * N_PARTICLES: Number of particles to run simulation with
 * MAX_VOLTAGE: The maximum voltage that any electrode will reach
 * SIGMA_[X|Y|Z]: Standard deviation of inital particle positions in each axis
 * TEMPERATURE: Temperature of initial particle distribution (for generating velocities)
 * NORM_DIST: if TRUE then we get a cloud of particles at the beginning, if FALSE then we get a uniform phase space
 * TARGET_V: The particle velocity we're aiming for...
 * TARGET_V_PRECISION: +/- this precision
 *
 */

#include <random> // For random number generation
#include <chrono> // For (system clock) timing
#include <algorithm> // for_each

#define N_DIMENSIONS 3
#define N_IN_SECTION 4 // The number of electrodes in each section (for symmetry in x/y)
#define SIMION_CORRECTION 0.1 // Not sure why it's needed but gives us V/m
#define MM_M_CORRECTION 1000 // To convert from m to mm

/*
 * DISCARD_COLLISIONS knows whether to keep data from collisions. Also affects HDF5 writing.
 * LEAN affects whether any trajectory data apart from initial and final states is saved
 */

//#define DISCARD_COLLISIONS
//#define LEAN
//#define INGLIS_TELLER

#ifdef DISCARD_COLLISIONS
bool discardCollisions = true;
#else
bool discardCollisions = false;
#endif

#include "Espace.h" // My classes
#include "Particle.h"
#include "myFunctions.h" // Includes myStructs.h (for params and hyperslabParams) too

#define V_SCHEME 3 // [ 1 : exp | 2 : inst | 3 : trap ] for choosing voltage scheme

using namespace std;

const float e = -1.60217657e-19;  // Electron charge
const float a0 = 5.2917721092e-11;  // Bohr radius
const float m = 1.6737e-27;  // Hydrogen mass
const float kb = 1.3806488e-23;  // Boltzmann constant
const float Fion = 1.14222e11;  // Ionisation threshold excluding n^(-4) factor
const float FWHMfactor = 2.35482;  // FWHM to std dev = 2 sqrt( 2 ln 2 )

chrono::time_point<chrono::system_clock> chronoStart, chronoEnd;  // To use for timing
chrono::duration<float> chronoElapsed;

float timeStep, duration;

// Various global variables, mostly for storing config values
bool normDist;
int nParticles, n, k;
float maxVoltage, sigmaX, sigmaY, sigmaZ, temperature, targetVel,
    targetVelPrecision;  // For normal distribution of initial particle positions
string outDir;
params config;

int nIonised = 0, nCollided = 0, nSucceeded = 0, nNeutralised = 0;  // For counting the particles' fates

constexpr char types[4] = { 'S', 'C', 'I', 'T' };  // Successes, Collisions, Ionisations, Trapped
constexpr char lcDimensions[N_DIMENSIONS] = { 'x', 'y', 'z' };

int main(int argc, char *argv[]) {
  cout << "IMPORTING E-FIELD DATA:\n" << endl;

  cout << "Reading config..." << endl;
  readConfig(argv[1]); // argv[1] is the path to the config file

  vector<shared_ptr<Electrode> > allElectrodes;  // Array of electrodes - shared_ptr gives auto memory management
  allElectrodes.reserve(config.nElectrodes);  // Make sure the vector doesn't have to be resized too much at runtime

  chronoStart = chrono::system_clock::now();  // Start timing import
  for (int n = 0; n < config.nElectrodes; ++n) {  // Import every electrode
    allElectrodes.emplace_back(make_shared<Electrode>(n + 1, config));
    cout << "Importing electrodes: " << n + 1 << "/" << config.nElectrodes
         << "\r" << flush;  // Progress
  }
  cout << endl;
  chronoEnd = chrono::system_clock::now();  // Finish timing import
  chronoElapsed = chronoEnd - chronoStart;
  cout << "Time to import: " << chronoElapsed.count() << "s" << endl;

  cout << "Generating initial particle distribution..." << endl;

  vector<Particle> particles;  // Initialise vector of particles
  particles.reserve(nParticles);
  int nTimeSteps = duration / timeStep;  // Cast as an int in order to round

  // TODO establish the meaning of temperature for a uniform velocity distribution
  float sigmaV = sqrt(2 * (temperature * kb / FWHMfactor) / m);  // Std dev of velocity
  float sectionWidth = N_IN_SECTION * config.z / config.nElectrodes;  // Width of each section of 4 electrodes

  particles.emplace_back(config.x / 2, config.y / 2, 55, 0, 0, 0);  // Synchronous particle

  mt19937 generator(chronoElapsed.count());  // Set up normal distribution generators (Mersenne Twister). Use the time from a few seconds ago as seed.

  if (normDist) {  // Cloud or uniform phase space. Ugly code duplication but necessary due to variable scope.
    normal_distribution<float> x_dist(config.x / 2, sigmaX);  // Center at start of cylinder
    normal_distribution<float> y_dist(config.y / 2, sigmaY);
    normal_distribution<float> z_dist(55, sigmaZ);  // Should mean most particles are in the cylinder (~0.27% are outside)
    normal_distribution<float> v_dist(0, sigmaV);

    for (int i = 0; i < nParticles - 1; ++i) {  // Generate particles
      particles.emplace_back(x_dist(generator), y_dist(generator),
                             z_dist(generator), v_dist(generator),
                             v_dist(generator), v_dist(generator), n, k);
    }
  } else {  // Uniform particle distribution between electrodes 1 and 4
    uniform_real_distribution<float> uniform_dist(0, 1);
    normal_distribution<float> muller_dist(0, 1);  // Using (a modified version of) the method described by Muller (1959) to generate spherical velocity dist.

    for (int i = 0; i < nParticles - 1; ++i) {  // Generate particles
      float p_theta = 2 * M_PI * uniform_dist(generator);
      float p_r = (0.5 * config.y - 5) * sqrt(uniform_dist(generator));
      float v_r = 2 * sigmaV * pow(uniform_dist(generator), 1 / 3.);
      float vx = muller_dist(generator);
      float vy = muller_dist(generator);
      float vz = muller_dist(generator);

      float muller_factor = pow(pow(vx, 2) + pow(vy, 2) + pow(vz, 2), -0.5);

      particles.emplace_back(p_r * cos(p_theta) + 0.5 * config.x,
                             p_r * sin(p_theta) + 0.5 * config.y,
                             sectionWidth * (3 * uniform_dist(generator) + 1),
                             v_r * muller_factor * vx, v_r * muller_factor * vy,
                             v_r * muller_factor * vz, n, k);
    }
  }

  const float Fit = Particle::ITlim(n,k); // Inglis-Teller limit

#if V_SCHEME != 3
  int section = 2;  // For keeping track of the electrodes which are on
#endif

  // TODO tune turn-off time to get collimated exit beam

  // For getting electrode positions
  for (auto &electrode : allElectrodes) {
    electrode->setVoltage(&electrode - &allElectrodes[0]);
  }
  Espace normWorld = Electrode::sumElectrodes(allElectrodes, config);

#if V_SCHEME == 1
  int lastCrossingTime = 0;
  float deltaT = 0, ti = 0;
  cout << "-- Using the exponentially increasing voltage scheme --" << endl;
#elif V_SCHEME == 2
  cout << "-- Using the instantaneous switching of electrodes scheme --" << endl;
#elif V_SCHEME == 3
  cout << "-- Using the moving trap voltage scheme --" << endl;
#endif

  cout << "Setting initial voltages..." << endl;

#if V_SCHEME == 3 // Moving trap
  for (int e = 0; e < config.nElectrodes; e += N_IN_SECTION) {
    for (int s = 0; s < N_IN_SECTION; ++s) {
      if (((e / 4) + 1) % 6 != 0) {
        allElectrodes[e + s]->setVoltage(
            maxVoltage * cos( M_PI * (((e / 4) + 1) % 6) / 3));  // Do all 4 with periodic potential
      } else {
        allElectrodes[e + s]->setVoltage(maxVoltage);  // Do all 4 with periodic potential
      }
    }
  }
  Espace thisWorld = Electrode::sumElectrodes(allElectrodes, config);
  //float offTime = config.z / (MM_M_CORRECTION * targetVel);
  float offTime = 3.7037e-4;

  // TODO Different I-T limits for each k state

#else // Exponential or instantaneous
  for (int e = 0; e < N_IN_SECTION; ++e) {
    allElectrodes[e]->setVoltage(maxVoltage);
  }
  Espace thisWorld = Electrode::sumElectrodes(allElectrodes,config,N_IN_SECTION);
#endif

  cout << "Starting simulation..." << endl;
  chronoStart = chrono::system_clock::now();

  for (int t = 0; t < nTimeSteps; ++t) {  // Time loop
#pragma omp parallel for schedule( guided, 3 ) reduction( +:nCollided,nIonised,nSucceeded,nNeutralised ) // Particularly good parallelisation
    for (auto particle = particles.begin(); particle < particles.end();
        ++particle) {  // Particle loop (using Iterator)
      if (particle->isDead() || particle->succeeded())
        continue;  // Check to see if the particle is alive

      vector<int> rndLoc = { static_cast<int>(round(particle->getLoc(0))),  // Get closest gridpoints
          static_cast<int>(round(particle->getLoc(1))), static_cast<int>(round(
              particle->getLoc(2))) };

      float mag = thisWorld.magAt(rndLoc[0], rndLoc[1], rndLoc[2]);

      if ((rndLoc[0] <= 1 || rndLoc[1] <= 1 || rndLoc[2] <= 1)
          || (rndLoc[0] >= config.x - 1 || rndLoc[1] >= config.y - 1)
          || (normWorld.magAt(rndLoc[0], rndLoc[1], rndLoc[2]) == 0.0)) {
        particle->collide();
        nCollided++;
        continue;
      }  // Exploit the fact that |E| is 0 in electrodes to detect collisions

      if (mag >= (Fion / pow(particle->getN(), 4))) {
        particle->ionise();
        nIonised++;
        continue;
      }  // Ionise if field too strong

#ifdef INGLIS_TELLER
      if (mag >= (Fit / pow(particle->getN(), 5)) && !particle->isNeutralised()) {
        particle->neutralise(t);
        nNeutralised++;
      } // Neutralise is field is past the Inglis-Teller limit
#endif

      if (rndLoc[2] >= config.z) {
        particle->succeed();
        nSucceeded++;
        continue;
      }  // If particle makes it to the far end

      float dEx = thisWorld.gradientX(rndLoc[0], rndLoc[1], rndLoc[2]);  // Field gradients
      float dEy = thisWorld.gradientY(rndLoc[0], rndLoc[1], rndLoc[2]);
      float dEz = thisWorld.gradientZ(rndLoc[0], rndLoc[1], rndLoc[2]);

      float ax = dEx * particle->mu() / m;  // Accelerations
      float ay = dEy * particle->mu() / m;
      float az = dEz * particle->mu() / m;

      particle->setVel(particle->getVel(0) + ax * timeStep,  // Accelerate it
                       particle->getVel(1) + ay * timeStep,
                       particle->getVel(2) + az * timeStep);

      particle->setLoc(
          particle->getLoc(
              0) + (particle->getVel(0)*timeStep + 0.5 * ax * pow(timeStep,2)) * MM_M_CORRECTION,  // Move it
          particle->getLoc(
              1) + (particle->getVel(1)*timeStep + 0.5 * ay * pow(timeStep,2)) * MM_M_CORRECTION,
          particle->getLoc(
              2) + (particle->getVel(2)*timeStep + 0.5 * az * pow(timeStep,2)) * MM_M_CORRECTION);
#ifndef LEAN
      particle->memorise();  // Commit to memory
#endif
    }

#if V_SCHEME == 1 // Exponentially increasing voltages on electrodes

    if ( particles[0].getLoc(2) >= section * sectionWidth && section < config.nElectrodes/4) {  // Use synchronous particle to switch electrodes, if it's past this section but not past the end
      float syncAccel = ( particles[0].getVel(2) - particles[0].recallVel(lastCrossingTime,2) ) / ( timeStep * (t - lastCrossingTime) );// Extrapolate particle accel.
      lastCrossingTime = t;// Update the last crossing time

      // Increase v until roughly when the next electrode is switched on: deltaT is the time the voltage increases for. Just a solution of const. accel. equations.
      deltaT = ( -particles[0].getVel(2) + sqrt( pow( particles[0].getVel(2),2 ) + 2 * syncAccel * sectionWidth / MM_M_CORRECTION ) ) / syncAccel;
      ti = t * timeStep;// remember we have to work in seconds

      cout << "Section " << section << " switched on at " << t*timeStep << "s" << endl;
      section++;
    }

    if ( deltaT && t <= (ti + deltaT)/timeStep) {  // Increase V until the appropriate time
      for ( int e = N_IN_SECTION * (section - 2); e < N_IN_SECTION * (section - 1); ++e) {  // Ugly! We have to subtract 1 from section because it was incremented in the above branch.
#pragma omp single // Not convinced this is necessary but it emphasises the fact that affects all the particles
        allElectrodes[e]->setVoltage( maxVoltage * ( exp( 1000*(t*timeStep - ti) ) - 1) / ( exp( 1000*deltaT ) - 1) );  // 1000 is just a constant that works well. I have no justification for it.
      }
      thisWorld = Electrode::sumElectrodes(allElectrodes, config, N_IN_SECTION*(section-1));  // Sum electrode fields
    }

#elif V_SCHEME == 2 // Instantaneous switching of electrodes

    if ( particles[0].getLoc(2) >= section * sectionWidth && section < config.nElectrodes/4) {  // Use synchronous particle to switch electrodes as above
      for (int e = N_IN_SECTION * (section - 1); e < N_IN_SECTION * section; ++e) {
#pragma omp single
        allElectrodes[e]->setVoltage(maxVoltage);  // Instantaneous switch
      }
      cout << "Section " << section << " switched on at " << t*timeStep << endl;
      thisWorld = Electrode::sumElectrodes(allElectrodes, config, N_IN_SECTION*section);  // Sum electrode fields
      section++;
    }

#elif V_SCHEME == 3 // Moving trap
    if (t < static_cast<int>(offTime / timeStep)) {
#pragma omp parallel for collapse(2) schedule( guided )
      for (int e = 0; e < config.nElectrodes; e += N_IN_SECTION) {
        for (int s = 0; s < N_IN_SECTION; ++s) {
          if (((e / 4) + 1) % 6 != 0) {
            allElectrodes[e + s]->setVoltage(
                maxVoltage
                    * cos(
                        (M_PI * (((e / 4) + 1) % 6) / 3)
                            - (pow(t * timeStep, 2) * M_PI * targetVel
                                * MM_M_CORRECTION / (offTime * sectionWidth * 6))));  // Do all 4 with periodic potential
          } else {
            allElectrodes[e + s]->setVoltage(
                maxVoltage
                    * cos(
                        pow(t * timeStep, 2) * M_PI * targetVel
                            * MM_M_CORRECTION / (offTime * sectionWidth * 6)));  // Do all 4 with periodic potential
          }
        }
      }
      thisWorld = Electrode::sumElectrodes(allElectrodes, config);
    } else {
      allElectrodes[0]->setVoltage(0.0);
      allElectrodes[1]->setVoltage(0.0);
      allElectrodes[2]->setVoltage(0.0);
      thisWorld = Electrode::sumElectrodes(allElectrodes, config, 2);
    }
#endif
    cout << timeStep * t << "s simulated\r" << flush;
  }

  chronoEnd = chrono::system_clock::now();  // Finish timing import
  chronoElapsed = chronoEnd - chronoStart;
  cout << "Time to run simulation: " << chronoElapsed.count() << "s" << endl;

  cout << "No. of collisions: " << nCollided << endl;
  cout << "No. of ionisations: " << nIonised << endl;
  cout << "No. past the I-T limit: " << nNeutralised << endl;
  cout << "No. of successes: " << nSucceeded << endl;

  cout << "Exporting simulation data..." << endl;

  string outFilePath = string(outDir) + "outFile.h5";
#ifndef LEAN
  int nts = nTimeSteps + 1;
#else
  int nts = 2;
#endif
  writeHDF5(particles, outFilePath, nts, discardCollisions, nSucceeded,
            nIonised, nCollided, nParticles);

  return 0;
}

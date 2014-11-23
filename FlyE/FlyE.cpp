/* 
 * Program to fly Rydberg H atoms through a time-varying electric field
 * 
 * Various options are configured in a given .conf file (give the path as an argument to this)
 * Electrode configuration is a split hollow cylinder, specified in splitcylinder.gem
 * makefields.py generates the .dat files from SIMION .pa files
 * 
 * 07/2014 Jamie Parkinson
 * 
 *
 * Config file guide:
 *
 * espace:
 * {
 *  n_electrodes: Number of individual electrodes in the arrangement
 *  dimensions = {x; y; z}: The dimensions of the matrices in the .dat files
 *  dat_directory: Path to the directory in which the .dat files are stored
 *  pa_name: The prefix to the dat file names eg [PA_NAME]_E1_L2_X.dat
 * }
 *
 * simulation:
 * {
 *  time_step: Time step to run simulation with (s)
 *  duration: Number of seconds to run simulation for
 *  n_particles: Number of particles to run simulation with
 *  max_voltage: The maximum voltage that any electrode will reach
 *  accel_scheme: "instantaneous", "exponential" or "trap"
 *  inglis_teller: Whether to neutralise particles in a field above the I-T limit
 * }
 *
 * norm_dist: Whether to initialise particles uniformly or normally distributed
 * sigma = {x; y; z}: Standard deviation of initial particle positions in each axis
 *
 * particles:
 * {
 *  temperature: Temperature of initial particle distribution (for generating velocities)
 *  n: Principal quantum no. of all particles
 *  k_dist: Whether to distribute ks "uniform"ly, "single" or "triangle"
 *  k: Single k value
 * }
 *
 * target_v: The particle velocity we're aiming for...
 * target_v_precision: +/- this precision
 *
 * storage:
 * {
 *  output_dir: Directory to write the HDF5 output file
 *  store_trajectories: Whether to store full trajectories or just start and end points
 *  store_collisions: Whether to store the trajectories of particles that collide
 * }
 *
 */
#include <chrono> // For (system clock) timing
#include <algorithm> // for_each

#include "Espace.h" // Basically a 4D array with some useful methods
#include "Particle.h" // Quite an in-depth particle object
#include "myFunctions.h" // Includes myStructs.h (for params and hyperslabParams) too
#include "myKDist.h" // Wrapper/abstraction for RNGs
#include "PhysicalConstants.h" // Contains all my constants

using namespace std;

chrono::time_point<chrono::system_clock> chronoStart, chronoEnd;  // To use for timing
chrono::duration<float> chronoElapsed;

float timeStep, duration;

// Various global variables, mostly for storing config values
bool normDist, storeCollisions, storeTrajectories, inglisTeller;
int nParticles, n, k;
float maxVoltage, sigmaX, sigmaY, sigmaZ, temperature, targetVel,
    targetVelPrecision;  // For normal distribution of initial particle positions
string outDir;
params config;
accelerationSchemes scheme;
kDists kDist;

int nIonised = 0, nCollided = 0, nSucceeded = 0, nNeutralised = 0;  // For counting the particles' fates

int main(int argc, char *argv[]) {
  cout << "Reading config file..." << endl;
  readConfig(argv[1]);  // argv[1] is the path to the config file

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

  float sigmaV = sqrt(2 * (temperature * Physics::kb / Physics::FWHMfactor) / Physics::mH);  // Std dev of velocity
  float sectionWidth = Physics::N_IN_SECTION * config.z / config.nElectrodes;  // Width of each section of 4 electrodes

  particles.emplace_back(config.x / 2, config.y / 2, 55, 0, 0, 0);  // Synchronous particle

  mt19937 generator(chronoElapsed.count());  // Set up normal distribution generators (Mersenne Twister). Use the time from a few seconds ago as seed.

  // Using my wrapper class because C++11's RNGs don't have a common parent
  myDist* k_dist;
  if (kDist == Single) {
    k_dist = new mySingleDist(k);
  } else if (kDist == Uniform) {
    k_dist = new myUniformDist(1, n-1);
  } else { // kDist == Triangle
    vector<float> kpieces {0.0, (float) n};
    vector<float> kweights {1, 0};
    k_dist = new myTriangleDist(kpieces, kweights);
  }


  if (normDist) {  // Cloud or uniform phase space. Ugly code duplication but necessary due to variable scope.
    normal_distribution<float> x_dist(config.x / 2, sigmaX);  // Center at start of cylinder
    normal_distribution<float> y_dist(config.y / 2, sigmaY);
    normal_distribution<float> z_dist(55, sigmaZ);  // Should mean most particles are in the cylinder (~0.27% are outside)
    normal_distribution<float> v_dist(0, sigmaV);

    for (int i = 0; i < nParticles - 1; ++i) {  // Generate particles
      particles.emplace_back(x_dist(generator), y_dist(generator),
                             z_dist(generator), v_dist(generator),
                             v_dist(generator), v_dist(generator), n,
                             (*k_dist)(generator));
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
                             v_r * muller_factor * vz, n, (*k_dist)(generator));
    }
  }

  // TODO tune turn-off time to get collimated exit beam

  // For getting electrode positions (because |E| = 0 in electrodes)
  for (auto &electrode : allElectrodes) {
    electrode->setVoltage(&electrode - &allElectrodes[0]); // Arbitrary voltage
  }
  Espace normWorld = Electrode::sumElectrodes(allElectrodes, config);

  int section = 2;  // For the exponential and instantaneous schemes
  int lastCrossingTime = 0;  // For the exponential scheme
  float deltaT = 0, ti = 0;  // Also for the exponential scheme

  if (scheme == Exponential) {
    cout << "-- Using the exponentially increasing voltage scheme --" << endl;
  } else if (scheme == Instantaneous) {
    cout << "-- Using the instantaneous switching of electrodes scheme --"
         << endl;
  } else {
    cout << "-- Using the moving trap voltage scheme --" << endl;
  }

  cout << "Setting initial voltages..." << endl;

  Espace thisWorld = Espace(config);
  float offTime;
  if (scheme == Trap) {  // Moving trap
    for (int e = 0; e < config.nElectrodes; e += Physics::N_IN_SECTION) {
      for (int s = 0; s < Physics::N_IN_SECTION; ++s) {
        if (((e / 4) + 1) % 6 != 0) {
          allElectrodes[e + s]->setVoltage(
              maxVoltage * cos( M_PI * (((e / 4) + 1) % 6) / 3));  // Do all 4 with periodic potential
        } else {
          allElectrodes[e + s]->setVoltage(maxVoltage);  // Do all 4 with periodic potential
        }
      }
    }
    thisWorld = Electrode::sumElectrodes(allElectrodes, config);
    //float offTime = config.z / (MM_M_CORRECTION * targetVel);
    offTime = 3.7037e-4;
  } else {  // Exponential or instantaneous
    for (int e = 0; e < Physics::N_IN_SECTION; ++e) {
      allElectrodes[e]->setVoltage(maxVoltage);
    }
    thisWorld = Electrode::sumElectrodes(allElectrodes, config, Physics::N_IN_SECTION);
  }

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

      if (mag >= (Physics::Fion / pow(particle->getN(), 4))) {
        particle->ionise();
        nIonised++;
        continue;
      }  // Ionise if field too strong

      if (mag >= particle->ITlim(n, k) && !particle->isNeutralised()
          && inglisTeller) {
        particle->neutralise(t);
        nNeutralised++;
      }  // Neutralise is field is past the Inglis-Teller limit

      if (rndLoc[2] >= config.z) {
        particle->succeed();
        nSucceeded++;
        continue;
      }  // If particle makes it to the far end

      float dEx = thisWorld.gradientX(rndLoc[0], rndLoc[1], rndLoc[2]);  // Field gradients
      float dEy = thisWorld.gradientY(rndLoc[0], rndLoc[1], rndLoc[2]);
      float dEz = thisWorld.gradientZ(rndLoc[0], rndLoc[1], rndLoc[2]);

      float ax = dEx * particle->mu() / Physics::mH;  // Accelerations
      float ay = dEy * particle->mu() / Physics::mH;
      float az = dEz * particle->mu() / Physics::mH;

      particle->setVel(particle->getVel(0) + ax * timeStep,  // Accelerate it
                       particle->getVel(1) + ay * timeStep,
                       particle->getVel(2) + az * timeStep);

      particle->setLoc(
          particle->getLoc(0) + (particle->getVel(0)*timeStep + 0.5 * ax * pow(timeStep,2)) * Physics::MM_M_CORRECTION,  // Move it
          particle->getLoc(1) + (particle->getVel(1)*timeStep + 0.5 * ay * pow(timeStep,2)) * Physics::MM_M_CORRECTION,
          particle->getLoc(2) + (particle->getVel(2)*timeStep + 0.5 * az * pow(timeStep,2)) * Physics::MM_M_CORRECTION);

      if (storeTrajectories)
        particle->memorise();  // Commit to memory
    }

    if (scheme == Exponential) {  // Exponentially increasing voltages on electrodes

      if (particles[0].getLoc(2) >= section * sectionWidth
          && section < config.nElectrodes / 4) {  // Use synchronous particle to switch electrodes, if it's past this section but not past the end
        float syncAccel = (particles[0].getVel(2)
            - particles[0].recallVel(lastCrossingTime, 2))
            / (timeStep * (t - lastCrossingTime));  // Extrapolate particle accel.
        lastCrossingTime = t;  // Update the last crossing time

        // Increase v until roughly when the next electrode is switched on: deltaT is the time the voltage increases for. Just a solution of const. accel. equations.
        deltaT = (-particles[0].getVel(2)
            + sqrt(
                pow(particles[0].getVel(2),
                    2) + 2 * syncAccel * sectionWidth / Physics::MM_M_CORRECTION))
            / syncAccel;
        ti = t * timeStep;  // remember we have to work in seconds

        cout << "Section " << section << " switched on at " << t * timeStep
             << "s" << endl;
        section++;
      }

      if (deltaT && t <= (ti + deltaT) / timeStep) {  // Increase V until the appropriate time
        for (int e = Physics::N_IN_SECTION * (section - 2);
            e < Physics::N_IN_SECTION * (section - 1); ++e) {  // Ugly! We have to subtract 1 from section because it was incremented in the above branch.
#pragma omp single // Not convinced this is necessary but it emphasises the fact that affects all the particles
          allElectrodes[e]->setVoltage(
              maxVoltage * (exp(1000 * (t * timeStep - ti)) - 1)
                  / (exp(1000 * deltaT) - 1));  // 1000 is just a constant that works well. I have no justification for it.
        }
        thisWorld = Electrode::sumElectrodes(allElectrodes, config,
                                             Physics::N_IN_SECTION * (section - 1));  // Sum electrode fields
      }

    } else if (scheme == Instantaneous) {  // Instantaneous switching of electrodes

      if (particles[0].getLoc(2) >= section * sectionWidth
          && section < config.nElectrodes / 4) {  // Use synchronous particle to switch electrodes as above
        for (int e = Physics::N_IN_SECTION * (section - 1); e < Physics::N_IN_SECTION * section;
            ++e) {
#pragma omp single
          allElectrodes[e]->setVoltage(maxVoltage);  // Instantaneous switch
        }
        cout << "Section " << section << " switched on at " << t * timeStep
             << endl;
        thisWorld = Electrode::sumElectrodes(allElectrodes, config,
                                             Physics::N_IN_SECTION * section);  // Sum electrode fields
        section++;
      }

    } else if (scheme == Trap) {  // Moving trap
      if (t < static_cast<int>(offTime / timeStep)) {
#pragma omp parallel for collapse(2) schedule( guided )
        for (int e = 0; e < config.nElectrodes; e += Physics::N_IN_SECTION) {
          for (int s = 0; s < Physics::N_IN_SECTION; ++s) {
            if (((e / 4) + 1) % 6 != 0) {
              allElectrodes[e + s]->setVoltage(
                  maxVoltage
                      * cos(
                          (M_PI * (((e / 4) + 1) % 6) / 3)
                              - (pow(t * timeStep, 2) * M_PI * targetVel
                                  * Physics::MM_M_CORRECTION
                                  / (offTime * sectionWidth * 6))));  // Do all 4 with periodic potential
            } else {
              allElectrodes[e + s]->setVoltage(
                  maxVoltage
                      * cos(
                          pow(t * timeStep, 2) * M_PI * targetVel
                              * Physics::MM_M_CORRECTION
                              / (offTime * sectionWidth * 6)));  // Do all 4 with periodic potential
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
    }
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

  int nts = (storeTrajectories) ? (nTimeSteps + 1) : 2;  // Number of timesteps in HDF5 file

  writeHDF5(particles, outFilePath, nts, nSucceeded, nIonised, nCollided,
            nParticles);

  return 0;
}


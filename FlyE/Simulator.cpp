#include "Simulator.h"

#include <numeric>

#include "PhysicalConstants.h"
#include "Writer.h" // Agh, circular dependency
#include "ezETAProgressBar.hpp"

// Output operator for SimulationNumbers
std::ostream& operator <<(std::ostream& out, SimulationNumbers &simNums) {
  out << "Number of successful particles: " << simNums.nSucceeded
      << " (" << 100.0 * simNums.nSucceeded / static_cast<float>(simNums.nParticles) << "%)\n"

      << "Number of collided particles: " << simNums.nCollided
      << " (" << 100.0 * simNums.nCollided / static_cast<float>(simNums.nParticles) << "%)\n"

      << "Number of ionised particles: " << simNums.nIonised
      << " (" << 100.0 * simNums.nIonised / static_cast<float>(simNums.nParticles) << "%)\n"

      << "Number of neutralised particles: " << simNums.nNeutralised
      << " (" << 100.0 * simNums.nNeutralised / static_cast<float>(simNums.nParticles) << "%)\n";
  return out;
}

Simulator::Simulator(AcceleratorGeometry &geometry,
                     std::vector<AntiHydrogen> &particles,
                     std::shared_ptr<SimulationConfig> simulationConfig,
                     std::shared_ptr<StorageConfig> storageConfig)
    : geometry_(geometry),
      particles_(particles),
      simulationConfig_(simulationConfig),
      acceleratorConfig_(geometry.getAcceleratorConfig()),
      storageConfig_(storageConfig) {

  statsStorage_.nParticles = static_cast<int>(particles_.size());

  int sectionWidth = Physics::N_IN_SECTION * acceleratorConfig_->z() /  // Section width
      acceleratorConfig_->nElectrodes();

  if (simulationConfig_->accelerationScheme() == "trap") {
    // Ignore any error here; the last argument is an implementation-specific off switch for experimental STL parallelism
    int avgK = std::accumulate(
        particles_.begin(), particles_.end(), 0,
        [](int total, AntiHydrogen &particle) {return total + particle.k();}, __gnu_parallel::sequential_tag()) / particles_.size();

    voltageScheme_ = new MovingTrapScheme(simulationConfig_->maxVoltage(),  // Max voltage
        acceleratorConfig_->nElectrodes(),  // Number of electrodes
        sectionWidth,  // Section width,
        simulationConfig_->timeStep(),  // Time step
        simulationConfig_->duration(), // End time
        simulationConfig_->targetVel(),  // Target velocity
        avgK);  // Average k
  } else if (simulationConfig_->accelerationScheme() == "instantaneous") {
    voltageScheme_ = new InstantaneousScheme(particles_[0],  // Synchronous particle
        simulationConfig_->maxVoltage(),  // Max voltage
        acceleratorConfig_->nElectrodes(),  // Number of electrodes
        sectionWidth,  // Section width
        simulationConfig_->timeStep());  // Time step
  } else if (simulationConfig_->accelerationScheme() == "exponential") {
    voltageScheme_ = new ExponentialScheme(particles_[0],  // Synchronous particle
        simulationConfig_->maxVoltage(),  // Max voltage
        acceleratorConfig_->nElectrodes(),  // Number of electrodes
        sectionWidth,  // Section width
        simulationConfig_->timeStep());  // Time step
  }

  geometry_.applyElectrodeVoltages(voltageScheme_->getInitialVoltages());
  field_ = geometry_.makeSmartField();
}

Simulator::~Simulator() {
  delete voltageScheme_;
}

void Simulator::run() {
  int nTimeSteps = simulationConfig_->duration()
      / simulationConfig_->timeStep();

  int nCollided = 0;
  int nIonised = 0;
  int nSucceeded = 0;
  int nNeutralised = 0;

  ElectrodeLocator locator = geometry_.electrodeLocations();

  std::cout << "Running simulation..." << std::endl;
  ez::ezETAProgressBar timeBar(nTimeSteps);
  timeBar.start();

  for (int t = 0; t < nTimeSteps; ++t, ++timeBar) {
// Particularly good parallelisation
#pragma omp parallel for schedule( guided, 3 ) reduction( +:nCollided, nIonised, nSucceeded, nNeutralised )
    for (auto particle = particles_.begin(); particle < particles_.end();
        ++particle) {
      if (particle->isDead() || particle->succeeded()) {
        continue;  // Check to see if the particle is alive
      }

      tuple3Dint rndLoc = particle->getIntLoc();

      if ((std::get<0>(rndLoc) <= 1 || std::get<1>(rndLoc) <= 1 || std::get<2>(rndLoc) <= 1)
          || (std::get<0>(rndLoc) >= acceleratorConfig_->x() - 1
              || std::get<1>(rndLoc) >= acceleratorConfig_->y() - 1)
          || locator.existsAt(rndLoc)) {
        particle->collide();

        if (!storageConfig_->storeCollisions()) {
          particle->forget();
        }

        ++nCollided;
        continue;
      }

      float mag = field_.magnitudeAt(rndLoc);

      if (mag >= particle->ionisationLim()) {
        particle->ionise();
        ++nIonised;
        continue;
      }  // Ionise if field too strong

      if (simulationConfig_->inglisTeller() && mag >= particle->ITlim() && !particle->isNeutralised()) {
        particle->neutralise(t);
        ++nNeutralised;
      }  // Neutralise is field is past the Inglis-Teller limit

      if (std::get<2>(rndLoc) >= acceleratorConfig_->z()) {
        particle->succeed();
        ++nSucceeded;
        continue;
      }  // If particle makes it to the far end

      particle->checkMaxField(mag); // Storing max field encountered

      float dEx = field_.gradientXat(rndLoc);  // Field gradients
      float dEy = field_.gradientYat(rndLoc);
      float dEz = field_.gradientZat(rndLoc);

      float ax = dEx * particle->mu() / Physics::mH;  // Accelerations
      float ay = dEy * particle->mu() / Physics::mH;
      float az = dEz * particle->mu() / Physics::mH;

      particle->setVel(
          particle->getVelDim<0>() + ax * simulationConfig_->timeStep(),  // Accelerate it
          particle->getVelDim<1>() + ay * simulationConfig_->timeStep(),
          particle->getVelDim<2>() + az * simulationConfig_->timeStep());

      particle->setLoc(
          particle->getLocDim<0>()
              + (particle->getVelDim<0>() * simulationConfig_->timeStep()
                  + 0.5 * ax * pow(simulationConfig_->timeStep(), 2))
                  * Physics::MM_M_FACTOR,  // Move it
          particle->getLocDim<1>()
              + (particle->getVelDim<1>() * simulationConfig_->timeStep()
                  + 0.5 * ay * pow(simulationConfig_->timeStep(), 2))
                  * Physics::MM_M_FACTOR,
          particle->getLocDim<2>()
              + (particle->getVelDim<2>() * simulationConfig_->timeStep()
                  + 0.5 * az * pow(simulationConfig_->timeStep(), 2))
                  * Physics::MM_M_FACTOR);

      if (storageConfig_->storeTrajectories())
        particle->memorise();  // Commit to memory
    }
    // Update field
    if (voltageScheme_->isActive(t)) {
      geometry_.applyElectrodeVoltages(voltageScheme_->getVoltages(t+1));
      field_ = geometry_.makeSmartField();
    }
  }

  statsStorage_.nCollided = nCollided;
  statsStorage_.nSucceeded = nSucceeded;
  statsStorage_.nIonised = nIonised;
  statsStorage_.nNeutralised = nNeutralised;

  std::cout << std::endl;
}

SimulationNumbers Simulator::getBasicStats() {
  return statsStorage_;
}

void Simulator::write(std::string fileName) {
  Writer writer(fileName, this);
  writer.initializeSetsAndSpaces();
  writer.writeParticles();
}

void Simulator::setSimulatorConfig(std::shared_ptr<SimulationConfig> simulationConfig) {
  simulationConfig_ = simulationConfig;
}

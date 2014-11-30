#include "Simulator.h"

#include <numeric>

#include "PhysicalConstants.h"
#include "Writer.h" // Agh, circular dependency

std::ostream& operator <<(std::ostream& out, SimulationNumbers &simNums) {
  out << "Number of successful particles: " << simNums.nSucceeded
      << "\nNumber of collided particles: " << simNums.nCollided
      << "\nNumber of ionised particles: " << simNums.nIonised
      << "\nNumber of neutralised particles: " << simNums.nNeutralised;
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

  int sectionWidth = Physics::N_IN_SECTION * acceleratorConfig_->z() /  // Section width
      acceleratorConfig_->nElectrodes();

  if (simulationConfig_->accelerationScheme() == "trap") {

    int avgK = std::accumulate(
        particles_.begin(), particles_.end(), 0,
        [](int total, AntiHydrogen &particle) {return total + particle.k();}, __gnu_parallel::sequential_tag())
        / particles_.size();

    voltageScheme_ = new MovingTrapScheme(simulationConfig_->maxVoltage(),  // Max voltage
        acceleratorConfig_->nElectrodes(),  // Number of electrodes
        sectionWidth,  // Section width
        simulationConfig_->timeStep(),  // Time step
        simulationConfig_->targetVel(),  // Target velocity
        avgK);  // Average k
  } else if (simulationConfig_->accelerationScheme() == "instantaneous") {
    voltageScheme_ = new InstantaneousScheme(particles_[0],  // Synchronous particle
        simulationConfig_->maxVoltage(),  // Max voltage
        acceleratorConfig_->nElectrodes(),  // Number of electroes
        sectionWidth,  // Section width
        simulationConfig_->timeStep());  // Time step
  } else if (simulationConfig_->accelerationScheme() == "exponential") {
    voltageScheme_ = new ExponentialScheme(particles_[0],  // Synchronous particle
        simulationConfig_->maxVoltage(),  // Max voltage
        acceleratorConfig_->nElectrodes(),  // Number of electroes
        sectionWidth,  // Section width
        simulationConfig_->timeStep());  // Time step
  }

  geometry_.applyElectrodeVoltages(voltageScheme_->getInitialVoltages());
  field_ = geometry_.makeSmartField();
}

void Simulator::run() {
  int nTimeSteps = simulationConfig_->duration()
      / simulationConfig_->timeStep();

  int nCollided = 0;
  int nIonised = 0;
  int nSucceeded = 0;
  int nNeutralised = 0;

  ElectrodeLocator locator = geometry_.electrodeLocations();

  for (int t = 0; t < nTimeSteps; ++t) {
#pragma omp parallel for schedule( guided, 3 ) reduction( +:nCollided, nIonised, nSucceeded, nNeutralised ) // Particularly good parallelisation
    for (auto particle = particles_.begin(); particle < particles_.end();
        ++particle) {
      if (particle->isDead() || particle->succeeded())
        continue;  // Check to see if the particle is alive

      std::vector<int> rndLoc = { static_cast<int>(round(particle->getLoc(0))),  // Get closest gridpoints
          static_cast<int>(round(particle->getLoc(1))), static_cast<int>(round(
              particle->getLoc(2))) };

      float mag = field_.magnitudeAt(rndLoc[0], rndLoc[1], rndLoc[2]);

      if ((rndLoc[0] <= 1 || rndLoc[1] <= 1 || rndLoc[2] <= 1)
          || (rndLoc[0] >= acceleratorConfig_->x() - 1
              || rndLoc[1] >= acceleratorConfig_->y() - 1)
          || locator.existsAt(rndLoc[0], rndLoc[1], rndLoc[2])) {
        particle->collide();
        ++nCollided;
        continue;
      }  // Exploit the fact that |E| is 0 in electrodes to detect collisions

      if (mag >= (Physics::Fion / pow(particle->n(), 4))) {
        particle->ionise();
        ++nIonised;
        continue;
      }  // Ionise if field too strong

      if (mag >= particle->ITlim() && !particle->isNeutralised()
          && simulationConfig_->inglisTeller()) {
        particle->neutralise(t);
        ++nNeutralised;
      }  // Neutralise is field is past the Inglis-Teller limit

      if (rndLoc[2] >= acceleratorConfig_->z()) {
        particle->succeed();
        ++nSucceeded;
        continue;
      }  // If particle makes it to the far end

      float dEx = field_.gradientXat(rndLoc[0], rndLoc[1], rndLoc[2]);  // Field gradients
      float dEy = field_.gradientYat(rndLoc[0], rndLoc[1], rndLoc[2]);
      float dEz = field_.gradientZat(rndLoc[0], rndLoc[1], rndLoc[2]);

      float ax = dEx * particle->mu() / Physics::mH;  // Accelerations
      float ay = dEy * particle->mu() / Physics::mH;
      float az = dEz * particle->mu() / Physics::mH;

      particle->setVel(
          particle->getVel(0) + ax * simulationConfig_->timeStep(),  // Accelerate it
          particle->getVel(1) + ay * simulationConfig_->timeStep(),
          particle->getVel(2) + az * simulationConfig_->timeStep());

      particle->setLoc(
          particle->getLoc(0)
              + (particle->getVel(0) * simulationConfig_->timeStep()
                  + 0.5 * ax * pow(simulationConfig_->timeStep(), 2))
                  * Physics::MM_M_CORRECTION,  // Move it
          particle->getLoc(1)
              + (particle->getVel(1) * simulationConfig_->timeStep()
                  + 0.5 * ay * pow(simulationConfig_->timeStep(), 2))
                  * Physics::MM_M_CORRECTION,
          particle->getLoc(2)
              + (particle->getVel(2) * simulationConfig_->timeStep()
                  + 0.5 * az * pow(simulationConfig_->timeStep(), 2))
                  * Physics::MM_M_CORRECTION);

      if (storageConfig_->storeTrajectories())
        particle->memorise();  // Commit to memory
    }
    // Update field
    if (voltageScheme_->isActive(t)) {
      geometry_.applyElectrodeVoltages(voltageScheme_->getVoltages(t));
      field_ = geometry_.makeSmartField();
    }
  }

  statsStorage_.nCollided = nCollided;
  statsStorage_.nSucceeded = nSucceeded;
  statsStorage_.nIonised = nIonised;
  statsStorage_.nNeutralised = nNeutralised;
}

SimulationNumbers Simulator::getBasicStats() {
  return statsStorage_;
}

void Simulator::write(std::string fileName) {
  Writer writer(fileName, this);
  writer.initializeSetsAndSpaces();
  writer.writeParticles();
}

/*
 * At this stage a sort of pseudocode for how I'd like things to look
 */

#include <iostream>
#include "ConfigLoader.h"

int main(int argc, char* argv[]) {
  ConfigLoader *myConfig = new ConfigLoader("/home/jamie/git/FlyE/FlyE/flyE.conf");
  AcceleratorConfig *accelConf = myConfig->getAcceleratorConfig();
  std::cout << accelConf->toString() << std::endl;


 /* AcceleratorGeometry *myAccelerator(myConfig);
  myAccelerator->importEFields();

  ParticleGenerator *myGenerator(myConfig);
  myGenerator->makeSynchronousParticle();
  Particles myParticles = myGenerator->makeParticles();

  Simulator *mySimulator = new Simulator(ConfigLoader, myAccelerator, myParticles);

  mySimulator->run();
  mySimulator->printBasicStats();

  mySimulator->write("PATH TO WRITE"); */
}

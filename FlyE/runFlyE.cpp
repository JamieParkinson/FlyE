/*
 * At this stage a sort of pseudocode for how I'd like things to look
 */

int main(int argc, char* argv[]) {
  SimConfig *myConfig = new SimConfig("PATH TO CONFIG");

  AcceleratorGeometry *myAccelerator(myConfig);
  myAccelerator->importEFields();

  ParticleGenerator *myGenerator(myConfig);
  myGenerator->makeSynchronousParticle();
  Particles myParticles = myGenerator->makeParticles();

  Simulator *mySimulator = new Simulator(SimConfig, myAccelerator, myParticles);

  mySimulator->run();
  mySimulator->printBasicStats();

  mySimulator->write("PATH TO WRITE");
}

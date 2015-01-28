FlyE is a piece of software written in C++ for simulating the acceleration of particles in an electric field. This build is specifically for simulationthe acceleration of Hydrogen/Antihydrogen for the AEgIS experiment at CERN.

The software is fully object-oriented and is documented in these pages. An explanation of the basic usage of the software as well as a manual for the config file follows.

##Basic Usage

FlyE does not deal directly with the calculation of electric fields. It is designed to import electric field files that have been produced by EXSIMECK,
which in turn takes SIMION files as input. The naming convention for the electric field files is `[PA name]_E[electrode number]_L[layer number]_[X/Y/Z].dat`.
A mostly undocumented helper script, `makefields.py`, to run EXSIMECK for an entire geometry as a batch job is included in the `Non-FlyE` directory.
This script also generates the `[accelerator]` section of the FlyE configuration file.

Including "FlyE.h" is necessary to access any of FlyE's classes. The `HDF5` and `Blitz++` libraries must be installed and compilation requires a version of G++ with support
for C++11. Also, the config file parser `inih` should be present in the working directory, as should the `ezETAProgressBar.hpp` script. The following compiler/linker flags are also required:

* `-DBZ_THREADSAFE`
* `-D_GLIBCXX_PARALLEL`
* `-fopenmp`
* `-lblitz`
* `-lpthread`
* `-lconfig++`

The following optimisations are recommended:

* `-O3`
* `-march=native`
* `-ffast-math`
* `-pipe`
* `-flto`

Below is an example of a basic program to run one simulation from one config file, print out some basic statistics from the simulation and write
the results to an HDF5 file.

```
 #include <iostream>
 #include "FlyE.h"

 int main(int argc, char* argv[]) {
   ConfigLoader myConfig("/home/jamie/git/FlyE/FlyE/flyE.conf");

   AcceleratorGeometry myAccelerator(myConfig.getAcceleratorConfig());
   myAccelerator.importElectrodes();

   ParticleGenerator<AntiHydrogen> myGenerator(myConfig.getParticlesConfig(), myConfig.getAcceleratorConfig());
   myGenerator.generateParticles();
   std::vector<AntiHydrogen> particles = myGenerator.getParticles();

   Simulator mySimulator(myAccelerator, particles,
                         myConfig.getSimulationConfig(),
                         myConfig.getStorageConfig());

   mySimulator->run();

   SimulationNumbers stats = mySimulator->getBasicStats();
   std::cout << stats << std::endl;

   mySimulator->write("OUTPUT FILE");
 }
```

##Configuration Files

FlyE uses the `inih` parser for its configuration files. Therefore, the syntax used is the informal `.INI` standard. The options are separated into sections
according to where they will be used in the program. The options are documented below.

* `accelerator`
  * `n_electrodes` - Number of electrodes in the accelerator geometry (integer).
  * `x/y/z` - The size of the geometry (from the electric field files) in each direction (integer).
  * `dat_directory` - The directory in whih the electric field files are stored (string).
  * `pa_name` - The prefix for the naming convention of the electric field files (string).
* `simulation`
  * `time_step` - The time step to use in the simulation (in seconds, float).
  * `duration` - The amount of time to run the simulation for (in seconds, float).
  * `max_voltage` - The maximum voltage to apply to the electrodes (volts, float).
  * `target_vel` - The velocity to accelerate the particles to. Only used with the 'trap' acceleration scheme (m/s, float).
  * `accel_scheme` - The scheme for voltages to accelerate particles. Can be 'exponential', 'instantaneous' or 'trap' (string).
  * `inglis_teller` - Whether to neutralise the electric dipole moment of particles if the field is greater than their Inglis-Teller limit (boolean).
* `particles`
  * `n_particles` - Number of particle to generate for the simulation (integer).
  * `position_dist` - How to distribute the particles in space. Can be one of the following: (string)
    * 'full' - Particles are uniformly distributed throughout the entire initial phase space.
    * 'uniform' - Particles are distributed uniformly in a cylinder described by the variables below.
    * 'normal' - Particles are distributed normally in a cloud described by the variables below.
  * `v_normal_dist` - Whether to distribute the velocities as a gaussian or as a uniform sphere (bool).
  * `dist_radius` - The radius of the distribution of particles - if norm_dist is true, then the radial standard deviation.
  * `dist_length` - The length of the distribution of particles - if norm_dist is true, then the axial standard deviation.
  * `dist_offset` - The offset of the particle distribution from the z=0 plane, in mm. (float)
  * `temperature` - The temperature of the distribution of particles (kelvin, float).
  * `n` - The principal quantum number of the particles (integer).
  * `k_dist` - How to distribute the values of k, the Stark quantum number. Can be one of the following: (string)
    * 'single' - All particles take the given value of k.
    * 'uniform' - Values of k are uniformly distributed between 1 and (n-1).
    * 'triangle' - Values of k are distributed between 1 and (n-1) according to a triangle distribution which is maximum at (n-1).
  * `k` - If `k_dist` is 'single', then all particles have this value of k.
* `storage`
  * `store_trajectories` - Whether to store the complete trajectories of the particles, or just their start and end locations/velocities (boolean).
  * `store_collisions` - Whether to store any data at all for particles which collide with the accelerator geometry (boolean).
  * `compression` - The level of GZIP compression to apply in the HDF5 output files (1-9, where 9 is max compression. integer).

## Output file specification

Output files are written in the HDF5 scienfic data format. The hierarchy inside this files is as follows.

#### Groups
* 'Succeeded' - for particles which reach the end of the accelerator.
* 'Collided' - for particles which collide with the accelerator geometry.
* 'Ionised' - for particles which are ionised in the course of the simulation.
* 'Remaining' - for particles which were still active at the end of the simulation.

#### Datasets
* 'data' - A 4D array of the particle trajectories. This is arranged like so:
  * 1st dimension - Index of particle.
  * 2nd dimension - Timestep index. If trajectories are not being stored, then 1 is initial and 2 is final.
  * 3rd dimension - Phase axis: 1 for location, 2 for velocity.
  * 4th dimension - Cartesian axis - 1 for x, 2 for y, 3 for z.
* 'neutralTimes' - The times at which particles' dipole moments were neutralised. -1 if never neutralised.
* 'ks' - The k-values of the particles.
* 'maxFields' - The maximum |E| encountered by the particles.

So an example address in the file would be '/Succeeded/data' for the trajectories of successful particles.

## Use on Amazon Web Services

The easiest way to get FlyE running on an EC2 instance with an Ubuntu AMI is to run these commands:

* Update repositories with `sudo apt-get update`
* Install everything we need with `sudo apt-get install git gcc g++ make libhdf5-dev libblitz-dev`
* Clone the git remote repository from GitHub: `git clone [*.git remote url]`
* `cd` into the Release directory and run `make`
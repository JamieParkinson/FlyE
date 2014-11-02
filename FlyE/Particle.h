/*
 * Particle class for FlyE
 * 
 * Each instance of this class represents one particle. The type of the position/velocity variables is templated as T. Each particle has the following properties:
 * 	Position - r_ (3d vector)
 * 	Velocity - v_ (3d vector)
 * 	Trajectory storage - rTrajectory_ and vTrajectory_ (FLATTENED vectors; have to be accessed as pseudomultidimensional)
 * 	Principal quantum no. - n_ (int)
 * 	Stark quantum no. - k_ (int)
 * 	Status - collided_, ionised_ and succeeded_ are bools which are true if the particle has collided with an electrode,
 * 			 been ionised or reached the end of the accelerator, respectively.
 * 
 * We can construct the particle with no arguments at rest at the origin, or with (x, y, z, vx, vy, vz). Calling memorise() at any time commits the current position and velocity
 * trajectory vectors. Various overloads of setLoc and setVel, and getLoc and getVel, can change/get the location and velocity. Similarly recallLoc and recallVel can access the trajectory
 * vectors. collide(), ionise(), and succeed() change the status variables, and we can check the status with isDead().
 * 
 * IMPORTANT: Note that, in order to conserve memory, the trajectory vectors are deleted if the particle collides or ionises.
 * 
 * Jamie Parkinson, July 2014
 */

#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <H5Cpp.h>

#include "myStructs.h"

#ifndef N_DIMENSIONS
#define N_DIMENSIONS 3
#endif

extern const float e, a0; // Declared in FlyE.cpp

class Particle {
 protected:
  std::vector<float> r_, v_;  // Position and velocity
  //std::vector<float> rTrajectory_, vTrajectory_; // Memory
  std::vector<float> xTraj_, yTraj_, zTraj_;
  std::vector<float> xvTraj_, yvTraj_, zvTraj_;
  int n_ = 25;
  int k_ = 12;
  float mu_; // Dipole moment
  bool collided_ = false;
  bool ionised_ = false;
  bool succeeded_ = false;
  bool neutralised_ = false;
  int neutralisationTime_ = -1;

 public:
  // Constructor with non-zero v and r and free enough memory
  Particle(float x, float y, float z, float vx, float vy, float vz);

  // Overload constructor to set n and k values
  Particle(float x, float y, float z, float vx, float vy, float vz, int n, int k);

  // Commit current location and velocity to memory
  void memorise();

  // Delete memory vectors
  void forget();

  // Delete all but first part of memory vectors
  void cutDownMemory();

  // Return dipole moment;
  float mu();

  // Return vector of locations in d (x1,x2,x3,...)
  std::vector<float> recallLoc(int d);

  // Return the d-location at the i-th position
  float recallLoc(int i, int d);

  // Return vector of velocities in d (vx1,vx2,vx3,...)
  std::vector<float> recallVel(int d);

  // Return the d-location at the i-th position
  float recallVel(int i, int d);

  // Set the location
  void setLoc(float x, float y, float z);

  // Set the location with a location vector
  void setLoc(std::vector<float> &loc);

  // Set the velocity
  void setVel(float vx, float vy, float vz);

  // Set the velocity with a velocity vector
  void setVel(std::vector<float> &vel);

  // Return the vector {x,y,z}
  std::vector<float> getLoc();

  // Return component d of {x,y,z}
  float getLoc(int d);

  // Return the vector {vx,vy,vz}
  std::vector<float> getVel();

  // Return component d of {vx,vy,vz}
  float getVel(int d);

  // Return principle qu. no.
  int getN();

  // Return stark qu. no.
  int getK();

  // Has the particle collided with an electrode or ionised? Returns 1 for collision, 2 for ionisation
  int isDead();

  // Has the particle passed the I-T limit?
  bool isNeutralised();

  // Get neutralisation time
  int neutralisationTime();

  // Has it made it out of the max z end?
  bool succeeded();

  // The particle ionised
  void ionise();

  // The particle collided with an electrode
  void collide();

  // Makes it past max z
  void succeed();

  // When past the I-T limit
  void neutralise();

  // Overload for adding time as well
  void neutralise(int t);

  // For generating hyperslab parameters
  hyperslabParams makeParameters(int pi, int d = 0, char pv = 'p');

  // Gets the IT limit for a given state
  static float ITlim(int n, int k);

  // Calls static method above
  float ITlim();
};

#include <vector>
#include <fstream>
#include <string>

#include "Particle.h"
#include "PhysicalConstants.h"

Particle::Particle(float x, float y, float z, float vx, float vy, float vz)
    : r_( { x, y, z }),
      v_( { vx, vy, vz }),
      collided_(false),
      succeeded_(false) {
  memorise();
}

void Particle::memorise() {
  /*for ( int d = 0; d < N_DIMENSIONS; ++d ) {
   rTrajectory_.emplace_back( getLoc(d) );
   vTrajectory_.emplace_back( getVel(d) );
   }*/
  xTraj_.emplace_back(r_[0]);
  yTraj_.emplace_back(r_[1]);
  zTraj_.emplace_back(r_[2]);

  xvTraj_.emplace_back(v_[0]);
  yvTraj_.emplace_back(v_[1]);
  zvTraj_.emplace_back(v_[2]);
}

void Particle::forget() {
  /*
   rTrajectory_.clear(); rTrajectory_.shrink_to_fit();
   vTrajectory_.clear(); vTrajectory_.shrink_to_fit();
   */
  xTraj_.clear();
  xTraj_.shrink_to_fit();
  yTraj_.clear();
  yTraj_.shrink_to_fit();
  zTraj_.clear();
  zTraj_.shrink_to_fit();

  xvTraj_.clear();
  xvTraj_.shrink_to_fit();
  yvTraj_.clear();
  yvTraj_.shrink_to_fit();
  zvTraj_.clear();
  zvTraj_.shrink_to_fit();
}

std::vector<float> Particle::recallLoc(int d) {
  switch (d) {
    case 0:
      return xTraj_;
    case 1:
      return yTraj_;
    case 2:
      return zTraj_;
    default:
      return zTraj_;
  }
}

float Particle::recallLoc(int i, int d) {
  switch (d) {
    case 0:
      return xTraj_[i];
    case 1:
      return yTraj_[i];
    case 2:
      return zTraj_[i];
    default:
      return zTraj_[i];
  }
}

std::vector<float> Particle::recallVel(int d) {
  switch (d) {
    case 0:
      return xvTraj_;
    case 1:
      return yvTraj_;
    case 2:
      return zvTraj_;
    default:
      return zvTraj_;
  }
}

float Particle::recallVel(int i, int d) {
  switch (d) {
    case 0:
      return xvTraj_[i];
    case 1:
      return yvTraj_[i];
    case 2:
      return zvTraj_[i];
    default:
      return zvTraj_[i];
  }
}

void Particle::setLoc(float x, float y, float z) {
  r_ = {x,y,z};
}

void Particle::setLoc(std::vector<float> &loc) {
  r_ = loc;
}

void Particle::setVel(float vx, float vy, float vz) {
  v_ = {vx,vy,vz};
}

void Particle::setVel(std::vector<float> &vel) {
  v_ = vel;
}

std::vector<float> Particle::getLoc() {
  return r_;
}

float Particle::getLoc(int d) {
  return r_[d];
}

std::vector<float> Particle::getVel() {
  return v_;
}

float Particle::getVel(int d) {
  return v_[d];
}

int Particle::isDead() {
  return (collided_) ? 1 : 0;
}

bool Particle::succeeded() {
  return succeeded_;
}

void Particle::collide() {
  collided_ = true;
  if (storeCollisions)
    memorise();
}

void Particle::succeed() {
  memorise();
  succeeded_ = true;
}

void Particle::cutDownMemory() {
  float x = xTraj_[0];
  float y = yTraj_[0];
  float z = zTraj_[0];
  float xv = xvTraj_[0];
  float yv = yvTraj_[0];
  float zv = zvTraj_[0];
  forget();
  xTraj_.emplace_back(x);
  yTraj_.emplace_back(y);
  zTraj_.emplace_back(z);
  xvTraj_.emplace_back(xv);
  yvTraj_.emplace_back(yv);
  zvTraj_.emplace_back(zv);
}

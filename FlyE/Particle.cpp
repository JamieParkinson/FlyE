#include <vector>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>

#include "Particle.h"
#include "PhysicalConstants.h"

Particle::Particle(float x, float y, float z, float vx, float vy, float vz)
    : r_(std::make_tuple(x, y, z)),
      v_(std::make_tuple(vx, vy, vz)),
      collided_(false),
      succeeded_(false),
      maxField_(std::numeric_limits<float>::min()) {
  memorise();
}

void Particle::memorise() {
  /*for ( int d = 0; d < N_DIMENSIONS; ++d ) {
   rTrajectory_.emplace_back( getLoc(d) );
   vTrajectory_.emplace_back( getVel(d) );
   }*/
  xTraj_.emplace_back(std::get<0>(r_));
  yTraj_.emplace_back(std::get<1>(r_));
  zTraj_.emplace_back(std::get<2>(r_));

  xvTraj_.emplace_back(std::get<0>(v_));
  yvTraj_.emplace_back(std::get<1>(v_));
  zvTraj_.emplace_back(std::get<2>(v_));
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
  r_ = std::make_tuple(x, y, z);
}

void Particle::setLoc(tuple3Dfloat loc) {
  r_ = loc;
}

void Particle::setVel(float vx, float vy, float vz) {
  v_ = std::make_tuple(vx, vy, vz);
}

void Particle::setVel(tuple3Dfloat vel) {
  v_ = vel;
}

tuple3Dfloat Particle::getLoc() {
  return r_;
}

tuple3Dint Particle::getIntLoc() {
  return std::make_tuple(static_cast<int>(round(std::get<0>(r_))),
                         static_cast<int>(round(std::get<1>(r_))),
                         static_cast<int>(round(std::get<2>(r_))));
}

tuple3Dfloat Particle::getVel() {
  return v_;
}

int Particle::isDead() {
  return (collided_) ? 1 : 0;
}

bool Particle::succeeded() {
  return succeeded_;
}

void Particle::collide() {
  collided_ = true;
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

float Particle::maxField() {
  return maxField_;
}

void Particle::checkMaxField(float magnitude) {
  if (magnitude > maxField_) maxField_ = magnitude;
}

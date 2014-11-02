/* Source for Particle class */

#include "Particle.h"

Particle::Particle(float x, float y, float z, float vx, float vy,
                   float vz)
    : r_( { x, y, z }),
      v_( { vx, vy, vz }) {
  memorise();
}

Particle::Particle(float x, float y, float z, float vx, float vy,
                   float vz, int n, int k)
    : r_( { x, y, z }),
      v_( { vx, vy, vz }),
      n_(n),
      k_(k) {
  mu_ = 1.5 * n * k * e * a0;
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

float Particle::mu() {
  return mu_;
}

std::vector<float> Particle::recallLoc(int d) {
  /*std::vector<float> returnVec;
   for (auto it = rTrajectory_.begin(); it < rTrajectory_.end(); it += 3) {
   returnVec.emplace_back(*(d + it));
   }
   return returnVec;*/
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
  //return rTrajectory_[N_DIMENSIONS * i + d];
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
  /*
   std::vector<float> returnVec;
   for (auto it = vTrajectory_.begin(); it < vTrajectory_.end(); it += 3) {
   returnVec.emplace_back(*(d + it));
   }
   return returnVec;
   */
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
  //return vTrajectory_[N_DIMENSIONS * i + d];
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

int Particle::getN() {
  return n_;
}

int Particle::getK() {
  return k_;
}

int Particle::isDead() {
  return (collided_) ? 1 : (ionised_) ? 2 : 0;
}

bool Particle::isNeutralised() {
  return neutralised_;
}

bool Particle::succeeded() {
  return succeeded_;
}

void Particle::ionise() {
  ionised_ = true;
  memorise();
  // forget();
}

void Particle::collide() {
  collided_ = true;
  memorise();
#ifdef DISCARD_COLLISIONS
  forget();
#endif
}

void Particle::succeed() {
  memorise();
  succeeded_ = true;
}

void Particle::neutralise() {
  mu_ = 0.0;
  neutralised_ = true;
}

void Particle::neutralise(int t) {
  mu_ = 0.0;
  neutralised_ = true;
  neutralisationTime_ = t;
}

int Particle::neutralisationTime() {
  return neutralisationTime_;
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

hyperslabParams Particle::makeParameters(int pi, int d, char pv) {
  hyperslabParams myParams;

  myParams.start[0] = d;  // x,y,z
  myParams.start[1] = (pv == 'p') ? 0 : 1;  // position or velocity
  myParams.start[2] = 0;  // time
  myParams.start[3] = pi;  // particle index

  myParams.count[0] = 1;  // Just 1 dimension
  myParams.count[1] = 1;  // Just position or velocity
  myParams.count[2] = xTraj_.size();  // All of time
  myParams.count[3] = 1;  // Just 1 particle

  myParams.stride[0] = 1;
  myParams.stride[1] = 1;
  myParams.stride[2] = 1;
  myParams.stride[3] = 1;

  return myParams;
}

float Particle::ITlim(int n, int k) {
  // Solution of:
  // -\frac{1}{2} 3 a_0 e F n (n+1)-\frac{13.6057 e}{(n+1)^2}=\frac{3}{2} a_0 e F (n-1) n-\frac{13.6057 e}{n^2}

  return (4.53523 + 9.07046*n)/(n*n*n*n*a0*(1+n)*(1+n));
}

float Particle::ITlim() {
  return ITlim(n_,k_);
}

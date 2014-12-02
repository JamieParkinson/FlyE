#include "AntiHydrogen.h"

#include <cmath>

#include "PhysicalConstants.h"

AntiHydrogen::AntiHydrogen(float x, float y, float z, float vx, float vy,
                           float vz, int n, int k)
    : Particle(x, y, z, vx, vy, vz),
      n_(n),
      k_(k),
      mu_(1.5 * n * k * Physics::e * Physics::a0),

      /* Solution of:
       * \frac{1}{2} (-3) a_0 e F n (n+1)-\frac{c h R_{\infty }}{(n+1)^2}=\frac{3}{2} a_0 e F k n-\frac{c h R_{\infty }}{n^2}
       *
       * ITlim = \frac{2 c h (2 n+1) R_{\infty }}{3 a_0 e n^3 (n+1)^2 (k+n+1)}
       */
      ITlim_(
          Physics::Fit * (1 + 2 * n)
              / (n * n * n * (1 + n) * (1 + n) * (1 + k + n))),

      ionisationLim_(Physics::Fion * pow(n_, -4)),

      ionised_(false),
      neutralised_(-1) {
}

float AntiHydrogen::mu() {
  return mu_;
}

void AntiHydrogen::neutralise(int t) {
  neutralised_ = t;
  memorise();
}

bool AntiHydrogen::isNeutralised() {
  return (neutralised_ == -1);
}

int AntiHydrogen::neutralisationTime() {
  return neutralised_;
}

void AntiHydrogen::ionise() {
  ionised_ = true;
  memorise();
}

int AntiHydrogen::isDead() {
  return (collided_) ? 1 : (ionised_) ? 2 : 0;
}

float AntiHydrogen::ITlim() {
  return ITlim_;
}

float AntiHydrogen::ionisationLim() {
  return ionisationLim_;
}

int AntiHydrogen::k() {
  return k_;
}

int AntiHydrogen::n() {
  return n_;
}

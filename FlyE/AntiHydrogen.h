/** @file AntiHydrogen.h
 * @brief This file contains the AntiHydrogen class
 *
 * AntiHydrogen is a subclass of Particle
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
#pragma once
#include "Particle.h"

/** @brief An implementation of Particle with some specific properties
 *
 * Adds n, k, mu (which is a function of n and k), Inglis-Teller limit
 * and ionisation and neutralisation related methods and member variables
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class AntiHydrogen : public Particle {
 protected:
  int n_;  //!< Principal quantum number
  int k_;  //!< Stark quantum number
  float mu_;  //!< Dipole moment
  /** @brief Inglis-Teller limit
   * Solution of:
   *
   * \f[
   * -\frac{3}{2} a_0 e F n (n+1)-\frac{ R_{\infty} h c }{(n+1)^2}=\frac{3}{2} a_0 e F k n-\frac{R_{\infty } h c}{n^2}
   * \f]
   * \f[
   * lim_{IT} = \frac{2 c h (2 n+1) R_{\infty }}{3 a_0 e n^3 (n+1)^2 (k+n+1)}
   * \f]
   */
  float ITlim_;
  float ionisationLim_;  //!< Ionisation limit

  bool ionised_;  //!< Whether the particle has ionised
  int neutralised_;  //!< Neutralisation time. -1 if not neutralised.

 public:
  /** @brief Constructor for starting initial position/velocity and quantum numbers
   *
   * @param x x-coordinate
   * @param y y-coordinate
   * @param z z-coordinate
   * @param vx velocity component in x-direction
   * @param vy velocity component in y-direction
   * @param vz velocity component in z-direction
   * @param n Principal quantum number
   * @param k Stark quantum number
   */
  AntiHydrogen(float x, float y, float z, float vx, float vy, float vz, int n,
               int k);

  /** Getter for mu_ (dipole moment)
   *
   * @return Dipole moment
   */
  float mu();

  /** @brief Neutralises the particle
   *
   * Sets neutralised_ to t
   *
   * @param t The time at which neutralisation occurred
   */
  void neutralise(int t);

  /** @brief Whether the particle has been neutralised
   *
   * @return true if neutralised, false if not
   */
  bool isNeutralised();

  /** @brief Get time at which neutralisation occurred
   *
   * @return Time at which neutralisation occurred
   */
  int neutralisationTime();

  /** @brief Ionises the particle
   *
   * Sets ionised_ to true
   */
  void ionise();

  /** @brief Has the particle ionised or collided?
   *
   * Shadows method in Particle
   *
   * @return 1 if collided, 2 if ionised, 0 otherwise
   */
  int isDead();

  /** @brief Gets the Inglis-Teller limit
   *
   * @return The Inglis-Teller limit of this particle
   */
  float ITlim();

  /** @brief Gets the ionisation field limit
   *
   * @return The ionisation limit of this particle
   */
  float ionisationLim();

  /** @brief Gets the value of k
   *
   * @return value of k
   */
  int k();

  /** @brief Gets the value of n
   *
   * @return value of n
   */
  int n();
};

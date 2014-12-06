/**@file Particle.h
 * @brief This file contains the Particle class
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
#pragma once

#include <vector>
#include <tuple>

#include "tupleDefs.h"

/** @brief Basic/generic particle class
 *
 * This will work but will need to be extended to add any specific properties or methods
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class Particle {
 protected:
  tuple3Dfloat r_; //!< Position
  tuple3Dint v_; //!< Velocity

  ///@{ @brief Contain the trajectory coordinates
  std::vector<float> xTraj_, yTraj_, zTraj_;
  std::vector<float> xvTraj_, yvTraj_, zvTraj_;
  ///@}

  bool collided_; //!< Whether the particle has collided
  bool succeeded_; //!< Whether the particle has reached the end of the accelerator

  float maxField_; //!< The maximum E-Field magnitude that the particle encountered

 public:
  /** @brief Constructor to set initial position and velocity
   *
   * @param x x-coordinate
   * @param y y-coordinate
   * @param z z-coordinate
   * @param vx velocity component in x-direction
   * @param vy velocity component in y-direction
   * @param vz velocity component in z-direction
   */
  Particle(float x, float y, float z, float vx, float vy, float vz);

  void memorise(); //!< Commit current location and velocity to memory

  void forget(); //!< Delete memory vectors

  void cutDownMemory();  //!< Delete all but first part of memory vectors

  /** @brief Return vector of locations in d (x1,x2,x3,...)
   *
   * @param d The dimension of locations to get
   * @return A vector of all d-locations
   */
  std::vector<float> recallLoc(int d);

  /** @brief Return the d-location at the i-th position
   *
   * @param i The time/index in trajectory to get
   * @param d The dimension to get
   * @return The corresponding position
   */
  float recallLoc(int i, int d);

  /** @brief Return vector of velocities in d (vx1,vx2,vx3,...)
   *
   * @param d The dimension of velocities to get
   * @return A vector of all d-velocities
   */
  std::vector<float> recallVel(int d);

  /** @brief Return the d-location at the i-th position
   *
   * @param i The time/index in trajectory to get
   * @param d The dimension to get
   * @return The corresponding velocity
   */
  float recallVel(int i, int d);

  /** @brief Set the location
   *
   * @param x x-coordinate
   * @param y y-coordinate
   * @param z z-coordinate
   */
  void setLoc(float x, float y, float z);

  /** @brief Set the location with a location tuple
   *
   * @param loc (x,y,z) tuple
   */
  void setLoc(tuple3Dfloat &loc);

  /** @brief Set the velocity
     *
     * @param vx x-component
     * @param vy y-component
     * @param vz z-component
     */
  void setVel(float vx, float vy, float vz);

  /** @brief Set the velocity with a velocity tuple
   *
   * @param vel (vx, vy, vz) tuple
   */
  void setVel(tuple3Dfloat &vel);

  /** @brief Return the tuple {x,y,z}
   *
   * @return The tuple (x, y, z)
   */
  tuple3Dfloat getLoc();

  /** @brief Returns the location in the templated dimension d
   * Template parameter d = 0 for x, d = 1 for y, d = 2 for z
   * @return The current d-location of the Particle
   */
  template<int d>
  float getLocDim() {
    return std::get<d>(r_);
  }

  /** @brief Returns the tuple {x,y,z} rounded to an integer grid
   *
   * @return The tuple {x,y,z} rounded to an integer grid
   */
  tuple3Dint getIntLoc();

  /** @brief Return the tuple {vx,vy,vz}
   *
   * @return The tuple (vx, vy, vz)
   */
  tuple3Dfloat getVel();

  /** @brief Returns the velocity in the templated dimension d
   * Template parameter d = 0 for x, d = 1 for y, d = 2 for z
   * @return The current d-velocity of the Particle
   */
  template<int d>
  float getVelDim() {
    return std::get<d>(v_);
  }

  /** @brief Whether the particle has collided
   *
   * Expected to be overloaded in derived classes for ionisation etc
   *
   * @return 0 if not collided, 1 if collided
   */
  int isDead();

  /** @brief Whether the particle has reached the end of the accelerator
   *
   * @return true if successful, false if not
   */
  bool succeeded();

  /** @brief Call a collision event
   *
   * Sets the collided_ int to 1
   */
  void collide();

  /** @brief Call a success event
   *
   * Sets the succeeded_ bool to true
   */
  void succeed();


  /** @brief Gets the maximum |E| that the particle encountered
   *
   * @return The maximum |E| that the particle encountered
   */
  float maxField();

  /** @brief Checks to see if the field given is greater than the previous max
   *
   * @param magnitude An E-field magnitude
   */
  void checkMaxField(float magnitude);
};

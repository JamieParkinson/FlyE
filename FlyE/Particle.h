#pragma once

#include <vector>

/** @brief Basic/generic particle class
 *
 * This will work but will need to be extended to add any specific properties or methods
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class Particle {
 protected:
  std::vector<float> r_; //!< Position
  std::vector<float> v_; //!< Velocity

  ///@{ @brief Contain the trajectory coordinates
  std::vector<float> xTraj_, yTraj_, zTraj_;
  std::vector<float> xvTraj_, yvTraj_, zvTraj_;
  ///@}

  bool collided_; //!< Whether the particle has collided
  bool succeeded_; //!< Whether the particle has reached the end of the accelerator

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

  /** @brief Set the location with a location vector
   *
   * @param loc (x,y,z) vector
   */
  void setLoc(std::vector<float> &loc);

  /** @brief Set the velocity
     *
     * @param x x-component
     * @param y y-component
     * @param z z-component
     */
  void setVel(float vx, float vy, float vz);

  /** @brief Set the velocity with a velocity vector
   *
   * @param vel (vx, vy, vz vector)
   */
  void setVel(std::vector<float> &vel);

  /** @brief Return the vector {x,y,z}
   *
   * @return The vector (x, y, z)
   */
  std::vector<float> getLoc();

  /** Return component d of {x,y,z}
   *
   * @param d component to get (x = 0, y = 1, z = 2)
   * @return Component d of (x, y, z)
   */
  float getLoc(int d);

  /** @brief Return the vector {vx,vy,vz}
   *
   * @return The vector (vx, vy, vz)
   */
  std::vector<float> getVel();

  /** Return component d of {vx,vy,vz}
    *
    * @param d component to get (vx = 0, vy = 1, vz = 2)
    * @return Component d of (vx, vy, vz)
    */
  float getVel(int d);

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

  /** @brief Call a collision event and throw the track away
   *
   * Sets the collided_ int to 1
   */
  void collide();

  /** @brief Call a collision event and don't throw it away */
  void collideAndStore();

  /** @brief Call a success event
   *
   * Sets the succeeded_ bool to true
   */
  void succeed();
};

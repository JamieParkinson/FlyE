/** @file PhysicalConstants.h
 * @brief All the constants needed in the program are stored here in the namespace "Physics"
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
#pragma once

namespace Physics {

constexpr float e = -1.60217657e-19;  //!< Electron charge
constexpr float a0 = 5.2917721092e-11;  //!< Bohr radius
constexpr float mH = 1.6737e-27;  //!< Hydrogen mass
constexpr float kb = 1.3806488e-23;  //!< Boltzmann constant
constexpr float Fion = 1.14222e11;  //!< Ionisation threshold excluding n^(-4) factor
constexpr float Fit = 1.71407e11;  //!< Inglis-Teller limit excluding algebraic terms (see Particle.cpp)
constexpr float FWHMfactor = 2.35482;  //!< FWHM to std dev = 2 sqrt( 2 ln 2 )

constexpr int N_DIMENSIONS = 3; //!< Cartesian space
constexpr int N_IN_SECTION = 4; //!< The number of electrodes in each section (for symmetry in x/y)
constexpr float SIMION_CORRECTION = 0.1; //!< Not sure why it's needed but gives us V/m
constexpr int MM_M_CORRECTION = 1000; //!< To convert from m to mm

constexpr char axes[3] = {'X', 'Y', 'Z'}; //!< Characters for each Cartesian axis name

}


/*
 * myStructs.h
 *
 *  Created on: 6 Sep 2014
 *      Author: jamie
 */

#ifndef MYSTRUCTS_H_
#define MYSTRUCTS_H_

#include <H5Cpp.h>

struct params {  // Holds the variables for the setup of the environment
  int nElectrodes;
  int nx, ny, nz, x, y, z, spaceSize;
  std::string datDirectory, paName;
};

struct hyperslabParams {
  hsize_t start[4], stride[4], count[4], block[4];
};

enum accelerationSchemes {
  Exponential, Instantaneous, Trap
};

enum kDists {
  Uniform, Single, Triangle
};

#endif /* MYSTRUCTS_H_ */

/** @file ConfigLoader.h
 * @brief This file contains the ConfigLoader class
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
#include <string>
#include <memory>

#include "SubConfig.h"
/** @brief A class for loading a config file and storing its contents appropriately.
 *
 * The class uses the inih library - https://code.google.com/p/inih/ - to load and parse config files.
 * Configuration data is stored in appropriate structs for each domain of usagem, and getters for (pointers to) this data are provided.
 *
 * @author Jamie Parkinson <jamie.parkinson.12@ucl.ac.uk>
 */
class ConfigLoader {
 private:
  std::string configFilePath_; /** Path to config file. @see ConfigLoader() */
  std::shared_ptr<AcceleratorConfig> accelConf_; //!< Smart pointer to AcceleratorConfig object
  std::shared_ptr<SimulationConfig> simConf_; //!< Smart pointer to SimulationConfig object
  std::shared_ptr<ParticlesConfig> particlesConf_; //!< Smart pointer to ParticlesConfig object
  std::shared_ptr<StorageConfig> storageConf_; //!< Smart pointer to ParticlesConfig object

 public:
  /** @brief Constructs ConfigLoader from the file at the given path.
   *
   * @param configFilePath Path to the config file
   * @return The ConfigLoader object
   */
  ConfigLoader(std::string configFilePath);

  std::shared_ptr<AcceleratorConfig> getAcceleratorConfig(); //!< Getter for AcceleratorConfig object
  std::shared_ptr<ParticlesConfig> getParticlesConfig(); //!< Getter for ParticlesConfig object
  std::shared_ptr<SimulationConfig> getSimulationConfig(); //!< Getter for SimulationConfig object
  std::shared_ptr<StorageConfig> getStorageConfig(); //!< Getter for StorageConfig object
};

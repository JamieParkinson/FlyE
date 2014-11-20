/*
 * A class to read in and then contain the contents of a config file
 */
#include <string>
#include <libconfig.h++>

class SimConfig {
 private:
  std::string configFilePath_ = "";
  libconfig::Config config_;

  // Will generate any variables that depend on what we've imported
  void calculateAdditionalVars();

 public:
  // Constructs the SimConfig object from the file located at configFilePath
  SimConfig(std::string configFilePath);

  // Imports a config file using libconfig++
  void configFromPath(std::string path);

  // Returns the variable at the config path given in variableName
  template<typename T>
  T getConfigVariable(std::string variableName);
};

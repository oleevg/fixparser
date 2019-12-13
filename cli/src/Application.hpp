#pragma once

namespace fixparser {
namespace cli {

struct ApplicationParameters
{
  ApplicationParameters();

  std::string logPropertiesFile;
  std::string fixDataFile;

  static std::string GetDefaultLogPropertiesFile()
  {
    return "log4cxx.properties";
  }

  static std::string GetDefaultFixDataFile()
  {
    return "fix_data.txt"
  }
};

class Application
{
public:
  int run(int argc, const char **argv);

private:
  void parseArguments(int argc, const char **argv);

  private:
    ApplicationParameters applicationParameters;
};
}
}
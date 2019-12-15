#pragma once

#include <string>

namespace fixparser {
namespace cli {

  struct ApplicationParameters
  {
    ApplicationParameters();

    std::string logPropertiesFile;
    std::string fixDataFile;
    int orderBookOutputSize;

    static std::string GetDefaultLogPropertiesFile()
    {
      return "log4cxx.properties";
    }

    static std::string GetDefaultFixDataFile()
    {
      return "fix_data.txt";
    }

    static int GetDefaultOrderBookOutputSize()
    {
      return 5;
    }
  };

  class Application
  {
    public:
      int run(int argc, const char** argv);

    private:
      void parseArguments(int argc, const char** argv);

    private:
      ApplicationParameters applicationParameters;
  };

}
}


#include <string>
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <log4cxx/logger.hpp>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>

#include <core/BaseException.hpp>
#include <core/ProtocolParser.hpp>

#include "Application.hpp"

namespace fixparser {
namespace cli {

ApplicationParameters::ApplicationParameters() : logPropertiesFile(GetDefaultLogPropertiesFile()), fixDataFile(GetDefaultFixDataFile())
{

}

int Application::run(int argc, const char **argv)
{
  int rc = 0;

  log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("fixparser.cli");

  try
  {
    parseArguments(argc, argv);

    if (boost::filesystem::exists(applicationParameters.logPropertiesFile))
    {
      log4cxx::PropertyConfigurator::configure(applicationParameters.logPropertiesFile);
    }
    else
    {
      std::cout << "Log4cxx configuration file " << applicationParameters.logPropertiesFile << " is not found. Use default logging configuration." << tsd::endl;
      log4cxx::BasicConfigurator::configure();
    }

    core::ProtocolParser protocolParser;
    protocolParser.run(std::fstream::open(applicationParameters.fixDataFile, std::fstream::in));
  }
  catch (const core::BaseException &exc)
  {
    LOG4CXX_ERROR(logger, "Internal error occured. " << exc.what());
    rc = -1;
  }
  catch (const std::exception &exc)
  {
    LOG4CXX_ERROR(logger, "Unknown error occurred. " << exc.what());
    rc = -1;
  }

  return rc;
}

void Application::parseArguments(int argc, const char **argv)
{
  namespace options = boost::program_options;

  ApplicationParameters applicationParameters;
  options::options_description optionDescription((boost::format("Usage: %s [options]... \nOptions") % argv[0]).str());

  optionDescription.add_options()
  ("log-properties,l", options::value<std::string>(&applicationParameters.logPropertiesFile)->default_value(ApplicationParameters::GetDefaultLogPropertiesFile()), "File path with log4cxx settings.")
  ("data-file,d", options::value<std::string>(&applicationParameters.fixDataFile)->default_value(ApplicationParameters::GetDefaultFixDataFile()), "File path with fix protocol data.")
  ("help,h", "As it says.");

  options::variables_map variableMap;

  options::store(options::parse_command_line(argc, argv, optionDescription), variableMap);
  options::notify(variableMap);

  if (variableMap.count("help"))
  {
    std::cout << optionDescription << "\n";
    exit(0);
  }
}
}
}
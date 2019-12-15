/*
 * FixHelper.cpp
 *
 *  Created on: 12/8/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#include <string>

#include <model/FixHelper.hpp>

namespace fixparser {
namespace model {

  namespace {

    unsigned int getAsciiSymbolsSumInternal(const std::string& str, char delimiter)
    {
      unsigned int result = 0;

      for(const auto& c: str)
      {
        result += c;
      }

      result += delimiter;

      return result;
    }

  }

  bool FixHelper::testTag(unsigned int tag, fixparser::model::FixFieldTag tagValue)
  {
    return tag == static_cast<int>(tagValue);
  }

  unsigned int FixHelper::getAsciiSymbolsSum(model::FixFieldTag tag)
  {
    unsigned int result = 0;
    std::string str = std::to_string(static_cast<unsigned  int>(tag));

    return getAsciiSymbolsSumInternal(str, '=');
  }

  unsigned int FixHelper::getAsciiSymbolsSum(const model::ByteArray& tag)
  {
    unsigned int result = 0;

    for(const auto& c: tag)
    {
      result += c;
    }

    result += '^';

    return result;
  }

  unsigned int FixHelper::getAsciiSymbolsSum(const model::TagValue& tagValue)
  {
    unsigned int result = getAsciiSymbolsSum(static_cast<model::FixFieldTag>(tagValue.tag));
    result += getAsciiSymbolsSum(tagValue.value);

    return result;
  }

  unsigned int FixHelper::getAsciiSymbolsSumTag(const std::string& tag)
  {
    return getAsciiSymbolsSumInternal(tag, '=');
  }


  unsigned int FixHelper::getAsciiSymbolsSumValue(const std::string& tag)
  {
    return getAsciiSymbolsSumInternal(tag, '^');
  }

  unsigned int FixHelper::toInteger(const model::ByteArray& value)
  {
    return std::atoi(std::string(value.cbegin(), value.cend()).c_str());
  }

  float FixHelper::toFloat(const ByteArray& value)
  {
    return std::stof(std::string(value.cbegin(), value.cend()).c_str());
  }
}
}



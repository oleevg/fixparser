/*
 * FixHelper.hpp
 *
 *  Created on: 12/8/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#ifndef FIXPARSER_FIXHELPER_HPP
#define FIXPARSER_FIXHELPER_HPP

#include <string>

#include <model/FixFieldTag.hpp>
#include <model/TagValue.hpp>

namespace fixparser {
namespace model {

  class FixHelper
  {
    public:
      static bool testTag(unsigned int tag, model::FixFieldTag tagValue);

      static unsigned int toInteger(const model::ByteArray& value);

      static float toFloat(const model::ByteArray& value);

      static unsigned int getAsciiSymbolsSum(model::FixFieldTag tag);

      static unsigned int getAsciiSymbolsSum(const model::ByteArray& tag);

      static unsigned int getAsciiSymbolsSumTag(const std::string& tag);

      static unsigned int getAsciiSymbolsSumValue(const std::string& value);

      static unsigned int getAsciiSymbolsSum(const model::TagValue& tagValue);
  };

}
}


#endif //FIXPARSER_FIXHELPER_HPP

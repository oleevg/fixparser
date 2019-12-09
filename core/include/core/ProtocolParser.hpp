/*
 * ProtocolParser.hpp
 *
 *  Created on: 12/4/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#ifndef FIXPARSER_PROTOCOLPARSER_HPP
#define FIXPARSER_PROTOCOLPARSER_HPP

#include <iostream>
#include <memory>

namespace fixparser {
namespace core {

  struct ProtocolParserImpl;

  class ProtocolParser
  {
    public:
      ProtocolParser();
      ~ProtocolParser();

      void run(std::istream& inputStream, char tagValueDelimiter, char fieldsDelimiter);

    private:
      std::unique_ptr<ProtocolParserImpl> impl_;
  };

}
}


#endif //FIXPARSER_PROTOCOLPARSER_HPP

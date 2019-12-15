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

#include <core/FixMessageProcessor.hpp>

namespace fixparser {
namespace core {

  struct ProtocolParserImpl;

  class ProtocolParser
  {
    public:
      ProtocolParser(const FixMessageProcessor::Ptr& fixMessageProcessor);
      ~ProtocolParser();

      void run(std::istream& inputStream, char tagValueDelimiter = '=', char fieldsDelimiter = '^');

    private:
      std::unique_ptr<ProtocolParserImpl> impl_;
      FixMessageProcessor::Ptr fixMessageProcessor_;
  };

}
}


#endif //FIXPARSER_PROTOCOLPARSER_HPP

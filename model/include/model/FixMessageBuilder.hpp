/*
 * FixMessageBuilder.hpp
 *
 *  Created on: 12/14/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#ifndef FIXPARSER_FIXMESSAGEBUILDER_HPP
#define FIXPARSER_FIXMESSAGEBUILDER_HPP

#include <model/FixBaseMessage.hpp>

namespace fixparser {
namespace model {

  class FixMessageBuilder
  {
    public:
      FixBaseMessage::Ptr CreateMessage(const FixBaseHeader::Ptr& header);
  };

}
}


#endif //FIXPARSER_FIXMESSAGEBUILDER_HPP

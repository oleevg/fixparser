/*
 * FixMessageBuilder.cpp
 *
 *  Created on: 12/14/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#include <stdexcept>

#include <boost/format.hpp>

#include <model/FixMessageBuilder.hpp>

#include "MarketDataIncrementalMessage.hpp"
#include "MarketDataSnapshotMessage.hpp"

namespace fixparser {
namespace model {

  FixBaseMessage::Ptr FixMessageBuilder::CreateMessage(const fixparser::model::FixBaseHeader::Ptr& header)
  {
    if(header->getMessageType() == static_cast<char>(FixMessageType::MarketDataSnapshot))
    {
      return std::make_shared<MarketDataSnapshotMessage>(header);
    }
    else if(header->getMessageType() == static_cast<char>(FixMessageType::MarketDataIncrement))
    {
      return std::make_shared<MarketDataIncrementalMessage>(header);
    }
    else
    {
      return nullptr;
    }
  }

}
}

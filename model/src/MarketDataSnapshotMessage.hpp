/*
 * MarketDataSnapshotMessage.hpp
 *
 *  Created on: 12/14/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#ifndef FIXPARSER_MARKETDATASNAPSHOTMESSAGE_HPP
#define FIXPARSER_MARKETDATASNAPSHOTMESSAGE_HPP

#include <initializer_list>

#include <model/TagValue.hpp>
#include <model/FixBaseMessage.hpp>

namespace fixparser {
namespace model {

  class MarketDataSnapshotMessage : public FixBaseMessage
  {
    public:
      MarketDataSnapshotMessage(const FixBaseHeader::Ptr& header) : FixBaseMessage(header, GetGroupFields())
      {}

    private:
      static std::initializer_list<TagType>  GetGroupFields()
      {
        return {static_cast<TagType>(FixFieldTag::MDEntryType),
                static_cast<TagType>(FixFieldTag::MDEntryPx),
                static_cast<TagType>(FixFieldTag::MDEntrySize),
                static_cast<TagType>(FixFieldTag::MDEntryDate),
                static_cast<TagType>(FixFieldTag::DeribitTradeId),
                static_cast<TagType>(FixFieldTag::IndexPrice),
                static_cast<TagType>(FixFieldTag::OrderId),
                static_cast<TagType>(FixFieldTag::OrdStatus),
                static_cast<TagType>(FixFieldTag::SecondaryOrderId),
                static_cast<TagType>(FixFieldTag::Side),
                static_cast<TagType>(FixFieldTag::Text)};
      }
  };

}
}


#endif //FIXPARSER_MARKETDATASNAPSHOTMESSAGE_HPP

/*
 * MarketDataIncrementalMessage.hpp
 *
 *  Created on: 12/14/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#ifndef FIXPARSER_MARKETDATAINCREMENTALMESSAGE_HPP
#define FIXPARSER_MARKETDATAINCREMENTALMESSAGE_HPP

#include <initializer_list>

#include <model/FixBaseMessage.hpp>

namespace fixparser {
namespace model {

  class MarketDataIncrementalMessage : public FixBaseMessage
  {
    public:
      MarketDataIncrementalMessage(const FixBaseHeader::Ptr& header)
              : FixBaseMessage(header, GetGroupFields())
      {

      }

    private:
      static std::initializer_list<TagType> GetGroupFields()
      {
        return {static_cast<TagType>(FixFieldTag::MDEntryType),
                 static_cast<TagType>(FixFieldTag::MDEntryPx),
                 static_cast<TagType>(FixFieldTag::MDEntrySize),
                 static_cast<TagType>(FixFieldTag::MDEntryDate),
                 static_cast<TagType>(FixFieldTag::MDUpdateAction)};
      }
  };

}
}


#endif //FIXPARSER_MARKETDATAINCREMENTALMESSAGE_HPP

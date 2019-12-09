/*
 * FixFieldTag.hpp
 *
 *  Created on: 12/7/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#ifndef FIXPARSER_FIXFIELDTAG_HPP
#define FIXPARSER_FIXFIELDTAG_HPP

namespace fixparser {
namespace model {

  enum class FixFieldTag
  {
      BeginString = 8,
      BodyLength = 9,
      CheckSum  =10,
      MsgType = 35,
      SenderCompID = 49,
      Symbol = 55,
      TargetCompID = 56,
      ContractMultiplier  =231,
      MDReqID = 262,
      NoMDEntries = 268,
      MDEntryType = 269,
      MDEntryPx = 270,
      MDEntrySize = 271,
      MDEntryDate = 272,
      MDUpdateAction = 279,
      TradeVolume24h = 100087,
      MarkPrice = 100090
  };

}
}

#endif //FIXPARSER_FIXFIELDTAG_HPP

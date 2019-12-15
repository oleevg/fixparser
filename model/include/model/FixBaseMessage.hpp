/*
 * FixMessage.hpp
 *
 *  Created on: 12/7/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#ifndef FIXPARSER_FIXBASEMESSAGE_HPP
#define FIXPARSER_FIXBASEMESSAGE_HPP

#include <vector>
#include <memory>
#include <unordered_set>

#include <log4cxx/logger.h>

#include <model/FixBaseHeader.hpp>

namespace fixparser {
namespace model {

  typedef std::unordered_set<model::TagType> FieldsSet;

  enum class FixMessageType : char
  {
      MarketDataSnapshot = 'W',
      MarketDataIncrement = 'X'
  };

  enum class MarketDataUpdateAction
  {
      New = 0,
      Change = 1,
      Delete = 2
  };

  class FixBaseMessage
  {
    public:
      typedef std::shared_ptr<FixBaseMessage> Ptr;
      typedef std::vector<FieldsMap> FieldsGroupCollection;

    private:
      enum class ControlSumCheckState
      {
          NotChecked,
          Valid,
          Invalid
      };

    public:
      void addField(TagType tag, const ByteArray& value);

      void addGroupFields(const FieldsMap& fieldsMap);

      TagValue::Ptr getField(TagType tag) const;

      FixMessageType getMessageType() const;

      bool isGroupField(TagType tag) const;

      bool checkControlSum();

      size_t getGroupFieldsSize() const;

      const FieldsMap& getGroupFields(size_t index) const;

    protected:
      FixBaseMessage(const FixBaseHeader::Ptr& header, const FieldsSet& groupFields);

    private:
      FixBaseHeader::Ptr header_;
      FieldsMap fieldsMap_;
      FieldsGroupCollection groupFieldsCollection_;
      FieldsSet fieldsInGroup_;
      ControlSumCheckState controlSumCheckState_;

      log4cxx::LoggerPtr logger_;
  };

}
}

#endif //FIXPARSER_FIXBASEMESSAGE_HPP

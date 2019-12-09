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

#include <log4cxx/logger.h>

#include <model/FixBaseHeader.hpp>

namespace fixparser {
namespace model {

  class FixBaseMessage
  {
    public:
      typedef std::shared_ptr<FixBaseMessage> Ptr;
      typedef std::vector<FieldsMap> FieldsGroupCollection;

    public:
      FixBaseMessage(const FixBaseHeader::Ptr& header);

      void addField(TagType tag, const ByteArray& value);

      void addFieldsGroup(const FieldsMap& fieldsMap);

      bool checkControlSum();

    private:
      enum class ControlSumCheckState
      {
        NotChecked,
        Valid,
        Invalid
      };

    private:
      FixBaseHeader::Ptr header_;
      FieldsMap fieldsMap_;
      FieldsGroupCollection fieldsGroupCollection_;
      ControlSumCheckState controlSumCheckState_;

      log4cxx::LoggerPtr logger_;
  };

}
}

#endif //FIXPARSER_FIXBASEMESSAGE_HPP

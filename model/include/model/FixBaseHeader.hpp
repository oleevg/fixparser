/*
 * FixBaseHeader.hpp
 *
 *  Created on: 12/8/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#ifndef FIXPARSER_FIXBASEHEADER_HPP
#define FIXPARSER_FIXBASEHEADER_HPP

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include <log4cxx/logger.h>

#include <model/TagValue.hpp>
#include <model/FixFieldTag.hpp>

namespace fixparser {
namespace model {

  typedef std::unordered_map<TagType, TagValue::Ptr> FieldsMap;

  class FixBaseHeader
  {
    public:
      typedef std::shared_ptr<FixBaseHeader> Ptr;
      typedef std::vector<FixFieldTag> RequiredTags;

    public:
      FixBaseHeader(char messageType, size_t messageLength, const std::string& messageVersion, const RequiredTags& requiredTags);

      void addField(TagType tag, const ByteArray& value);

      std::string getMessageVersion() const;

      char getMessageType() const;

      size_t getMessageLength() const;

      bool isValid();

      size_t getFieldsSum() const;

    private:
      char messageType_;
      size_t messageLength_;
      std::string messageVersion_;
      RequiredTags messageRequiredTags_;

      FieldsMap fieldsMap_;
      log4cxx::LoggerPtr logger_;
  };

  FixBaseHeader::Ptr CreateFixHeader(char messageType, size_t messageLength, const std::string& messageVersion);

}
}

#endif //FIXPARSER_FIXBASEHEADER_HPP

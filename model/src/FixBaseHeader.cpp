/*
 * FixBaseHeader.cpp
 *
 *  Created on: 12/8/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#include <stdexcept>
#include <algorithm>

#include <boost/format.hpp>

#include <model/FixFieldTag.hpp>
#include <model/FixBaseHeader.hpp>
#include <model/FixHelper.hpp>

namespace fixparser {
namespace model {

  FixBaseHeader::FixBaseHeader(char messageType, size_t messageLength, const std::string& messageVersion, const RequiredTags& requiredTags):
  messageType_(messageType), messageLength_(messageLength), messageVersion_(messageVersion), messageRequiredTags_(requiredTags)
  {
    logger_ = log4cxx::Logger::getLogger("fixparser.model");
  }

  bool FixBaseHeader::addField(TagType tag, const ByteArray& value)
  {
    auto iter = std::find_if(messageRequiredTags_.cbegin(), messageRequiredTags_.cend(), [](const decltype (messageRequiredTags_)::value_type & value)
    {
      return static_cast<TagType>(value);
    });

    if(iter == messageRequiredTags_.cend())
    {
      LOG4CXX_WARN(logger_, "Attempt to add not header field with tag " << tag);
      return false;
    }

    auto tagValue = std::make_shared<model::TagValue>(tag, value);
    auto result = fieldsMap_.insert({tag, tagValue});
    if(!result.second)
    {
      LOG4CXX_WARN(logger_, "Attempt to add field with tag " << tag << " multiple time.");
      return false;
    }

    return true;
  }

  std::string FixBaseHeader::getMessageVersion() const
  {
    return messageVersion_;
  }

  char FixBaseHeader::getMessageType() const
  {
    return messageType_;
  }

  size_t FixBaseHeader::getMessageLength() const
  {
    return messageLength_;
  }

  bool FixBaseHeader::isValid()
  {
    return fieldsMap_.size() == messageRequiredTags_.size();
  }

  size_t FixBaseHeader::getFieldsSum() const
  {
    size_t result = 0;

    result += FixHelper::getAsciiSymbolsSum(FixFieldTag::BeginString);
    result += FixHelper::getAsciiSymbolsSumValue(messageVersion_);

    result += FixHelper::getAsciiSymbolsSum(FixFieldTag::BodyLength);
    result += FixHelper::getAsciiSymbolsSumValue(std::to_string(messageLength_));

    result += FixHelper::getAsciiSymbolsSum(FixFieldTag::MsgType);
    result += FixHelper::getAsciiSymbolsSumValue(std::string(messageType_, 1));

    for (const auto& field: fieldsMap_)
    {
      result += FixHelper::getAsciiSymbolsSum(*field.second);
    }

    return result;
  }

  FixBaseHeader::Ptr CreateFixHeader(char messageType, size_t messageLength, const std::string& messageVersion)
  {
    if(messageVersion == "FIX.4.4")
    {
      FixBaseHeader::RequiredTags requiredTags{FixFieldTag::SenderCompID, FixFieldTag::TargetCompID};
      return std::make_shared<FixBaseHeader>(messageType, messageLength, messageVersion, requiredTags);
    }
    else
    {
      throw std::logic_error((boost::format("Fix protocol %s is not supported.") % messageVersion).str());
    }
  }

}
}


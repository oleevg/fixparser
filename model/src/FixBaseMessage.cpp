/*
 * FixBaseMessage.cpp
 *
 *  Created on: 12/8/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#include <model/FixBaseMessage.hpp>

namespace fixparser {
namespace model {

  FixBaseMessage::FixBaseMessage(const FixBaseHeader::Ptr& header):
  header_(header)
  {
    logger_ = log4cxx::Logger::getLogger("fixparser.model.message");
  }

  void FixBaseMessage::addField(TagType tag, const ByteArray& value)
  {
    auto tagValue = std::make_shared<model::TagValue>(tag, value);
    auto result = fieldsMap_.insert({tag, tagValue});
    if(!result.second)
    {
      LOG4CXX_WARN(logger_, "Attempt to add field with tag " << tag << " multiple time");
    }
  }

  void FixBaseMessage::addFieldsGroup(const FieldsMap& fieldsMap)
  {
    fieldsGroupCollection_.push_back(fieldsMap);
  }

  bool FixBaseMessage::checkControlSum()
  {
    if(controlSumCheckState_ != ControlSumCheckState::NotChecked)
    {
      return controlSumCheckState_ == ControlSumCheckState::Valid;
    }

    size_t fieldsSum = 0;
    fieldsSum += header_->getFieldsSum();

  }
}
}


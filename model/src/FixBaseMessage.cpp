/*
 * FixBaseMessage.cpp
 *
 *  Created on: 12/8/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#include <model/FixBaseMessage.hpp>

namespace fixparser {
namespace model {

  FixBaseMessage::FixBaseMessage(const FixBaseHeader::Ptr& header, const FieldsSet& groupFields) :
  header_(header), fieldsInGroup_(groupFields)
  {
    logger_ = log4cxx::Logger::getLogger("fixparser.model.FixBaseMessage");
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

  void FixBaseMessage::addGroupFields(const FieldsMap& fieldsMap)
  {
    groupFieldsCollection_.push_back(fieldsMap);
  }

  bool FixBaseMessage::checkControlSum()
  {
//    if(controlSumCheckState_ != ControlSumCheckState::NotChecked)
//    {
//      return controlSumCheckState_ == ControlSumCheckState::Valid;
//    }
//
//    size_t fieldsSum = 0;
//    fieldsSum += header_->getFieldsSum();

    return true;

  }

  bool FixBaseMessage::isGroupField(TagType tag) const
  {
    return fieldsInGroup_.count(tag);
  }

  FixMessageType FixBaseMessage::getMessageType() const
  {
    return static_cast<FixMessageType>(header_->getMessageType());
  }

  TagValue::Ptr FixBaseMessage::getField(TagType tag) const
  {
    TagValue::Ptr result = nullptr;

    auto field = fieldsMap_.find(tag);
    if(field != fieldsMap_.end())
    {
      result = field->second;
    }

    return result;
  }

  size_t FixBaseMessage::getGroupFieldsSize() const
  {
    return groupFieldsCollection_.size();
  }

  const FieldsMap& FixBaseMessage::getGroupFields(size_t index) const
  {
    if(index >= getGroupFieldsSize())
    {
      throw std::out_of_range("Requested index is out of destination size");
    }

    return groupFieldsCollection_[index];
  }

}
}


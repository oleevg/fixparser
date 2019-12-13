/*
 * ProtocolParser.cpp
 *
 *  Created on: 12/4/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#include <iostream>

#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>

#include <log4cxx/logger.h>

#include <model/FixBaseHeader.hpp>
#include <model/FixBaseMessage.hpp>
#include <model/FixFieldTag.hpp>
#include <model/FixHelper.hpp>
#include <model/TagValue.hpp>

#include <core/ProtocolParser.hpp>

namespace fixparser {
namespace core {

  struct ProtocolParserImpl: boost::msm::front::state_machine_def<ProtocolParserImpl>
  {
    ProtocolParserImpl(std::istream& inputStream, char tagValueDelimiter, char fieldsDelimiter):
            inputStream_(inputStream), tagValueDelimiter_(tagValueDelimiter), fieldsDelimiter_(fieldsDelimiter)
    {
      logger_ = log4cxx::Logger::getLogger("fixparser.core.ProtocolParser");
    }

    void readField(char* buffer, size_t size, model::ByteArray& byteArray)
    {
      if(!inputStream_)
      {
        LOG4CXX_INFO(logger_, "End of the input data stream.");
      }

      inputStream_.getline(buffer, size, tagValueDelimiter_);
      LOG4CXX_DEBUG(logger_, "Found tag: " << std::string(buffer));

      char symbol = 0;
      byteArray.clear();
      inputStream_.get(symbol);
      while (inputStream_ && symbol != fieldsDelimiter_)
      {
        byteArray.push_back(symbol);
        inputStream_.get(symbol);
      }

      LOG4CXX_DEBUG(logger_, "Found value: " << std::string(byteArray.begin(), byteArray.end()));
    }

    // Events
    struct HeaderProcessed {};

    struct FieldsGroupProcessed {};

    struct FieldsGroupStarted {};

    struct ControlSumRead
    {
      model::TagValue value;
    };

    struct ValidMessageProcessed {};

    struct InvalidMessageProcessed {};

    // States
    struct HeaderProcessing : boost::msm::front::state<>
    {
      template<class Event, class Fsm>
      void on_entry(Event const&, Fsm& stateMachine) const
      {
        LOG4CXX_DEBUG(stateMachine.logger_, "enter in " << __PRETTY_FUNCTION__);

        char messageType = 0;
        size_t messageLength = 0;
        std::string messageVersion;

        char tagBuffer[24];
        model::ByteArray value;

        // model::FixFieldTag::BeginString processing
        stateMachine.readField(tagBuffer, sizeof(tagBuffer), value);
        if(model::FixHelper::testTag(tagBuffer, model::FixFieldTag::BeginString))
        {
            messageVersion.assign(value.cbegin(), value.cend());
        }
        else
        {
          LOG4CXX_ERROR(stateMachine.logger_, "FixFieldTag::BeginString is not the first statement.");
          return;
        }

        // model::FixFieldTag::BodyLength processing
        stateMachine.readField(tagBuffer, sizeof(tagBuffer), value);
        if (model::FixHelper::testTag(tagBuffer, model::FixFieldTag::BodyLength))
        {
          messageLength = std::atoi(std::string(value.cbegin(), value.cend()).c_str());
        }
        else
        {
          LOG4CXX_ERROR(stateMachine.logger_, "FixFieldTag::BodyLength is not found.");
          return;
        }

        // model::FixFieldTag::MsgType processing
        stateMachine.readField(tagBuffer, sizeof(tagBuffer), value);
        if (model::FixHelper::testTag(tagBuffer, model::FixFieldTag::MsgType))
        {
          if(value.size() > 1)
          {
            LOG4CXX_ERROR(stateMachine.logger_, "Wrong length for the field FixFieldTag::MsgType.");
          }

          messageType = value.front();
        }
        else
        {
          LOG4CXX_ERROR(stateMachine.logger_, "FixFieldTag::MsgType is not found.");
          return;
        }

        stateMachine.header_ = model::CreateFixHeader(messageType, messageLength, messageVersion);
        while (!stateMachine.header_->isValid())
        {
          stateMachine.readField(tagBuffer, sizeof(tagBuffer), value);
          stateMachine.header_->addField(std::atoi(tagBuffer), value);
        }

        stateMachine.message_ = std::make_shared<model::FixBaseMessage>(stateMachine.header_);
        stateMachine.process_event(HeaderProcessed());
      }
    };

    struct FieldProcessing : boost::msm::front::state<>
    {
      template<class Event, class Fsm>
      void on_entry(Event const&, Fsm& stateMachine) const
      {
        LOG4CXX_DEBUG(stateMachine.logger_, "enter in " << __PRETTY_FUNCTION__);

        char tagBuffer[24];
        model::ByteArray value;

        while(stateMachine.inputStream_)
        {
          stateMachine.readField(tagBuffer, sizeof(tagBuffer), value);
          if (model::FixHelper::testTag(tagBuffer, model::FixFieldTag::NoMDEntries))
          {
            stateMachine.message_->addField(std::atoi(tagBuffer), value);
            stateMachine.process_event(FieldsGroupStarted());
          }
          else if (model::FixHelper::testTag(tagBuffer, model::FixFieldTag::CheckSum))
          {
            ControlSumRead controlSumRead{model::TagValue{static_cast<unsigned >(std::atoi(tagBuffer)), value}};

            stateMachine.process_event(controlSumRead);
          }

          stateMachine.message_->addField(std::atoi(tagBuffer), value);
        }
      }
    };

    struct FieldsGroupProcessing : boost::msm::front::state<>
    {
      template<class Event, class Fsm>
      void on_entry(Event const&, Fsm& stateMachine) const
      {
        LOG4CXX_DEBUG(stateMachine.logger_, "enter in " << __PRETTY_FUNCTION__);
      }
    };

    struct ControlSumChecking : boost::msm::front::state<>
    {
      template<class Event, class Fsm>
      void on_entry(Event const&, Fsm& stateMachine) const
      {
        LOG4CXX_DEBUG(stateMachine.logger_, "enter in " << __PRETTY_FUNCTION__);
      }
    };

    struct MessageProcessing : boost::msm::front::state<>
    {
      template<class Event, class Fsm>
      void on_entry(Event const&, Fsm& stateMachine) const
      {
        LOG4CXX_DEBUG(stateMachine.logger_, "enter in " << __PRETTY_FUNCTION__);
      }
    };

    // Set initial state
    typedef HeaderProcessing initial_state;

    // Transition table
    struct transition_table : boost::mpl::vector<
            //          Start   Event   Next    Action      Guard
            boost::msm::front::Row < HeaderProcessing, HeaderProcessed, FieldProcessing, boost::msm::front::none, boost::msm::front::none>,
            boost::msm::front::Row < FieldProcessing, FieldsGroupStarted, FieldsGroupProcessing, boost::msm::front::none, boost::msm::front::none>,
            boost::msm::front::Row < FieldsGroupProcessing, FieldsGroupProcessed, FieldProcessing, boost::msm::front::none, boost::msm::front::none>,
            boost::msm::front::Row < FieldProcessing, ControlSumRead, ControlSumChecking, boost::msm::front::none, boost::msm::front::none>
    > {};

    std::istream& inputStream_;
    log4cxx::LoggerPtr logger_;

    model::FixBaseHeader::Ptr header_;
    model::FixBaseMessage::Ptr message_;

    char tagValueDelimiter_;
    char fieldsDelimiter_;
  };

  ProtocolParser::ProtocolParser()
  {

  }

  ProtocolParser::~ProtocolParser() = default;

  void ProtocolParser::run(std::istream& inputStream, char tagValueDelimiter, char fieldsDelimiter)
  {
    typedef boost::msm::back::state_machine<ProtocolParserImpl> StateMachine;

    StateMachine stateMachine(std::ref(inputStream), tagValueDelimiter, fieldsDelimiter);
    stateMachine.start();
  }

}
}


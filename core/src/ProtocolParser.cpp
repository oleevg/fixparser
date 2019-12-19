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
#include <model/FixMessageBuilder.hpp>

#include <core/ProtocolParser.hpp>
#include <core/FixMessageProcessor.hpp>
#include <core/exceptions/FixParserException.hpp>

#define READ_OR_RETURN_ON_ENDOF_STREAM(stateMachine, fieldData) \
{ bool endOfStream = !stateMachine.readField(fieldData, stateMachine); \
if(endOfStream) { stateMachine.process_event(EndOfStream()); return; } }

namespace fixparser {
namespace core {

  namespace {

    class FieldData
    {
      public:
        FieldData()
        {
          tagBuffer[0] = '\0';
        }

        void clear()
        {
          tagBuffer[0] = '\0';
          value.clear();
        }

        unsigned int getTag() const
        {
          return std::atoi(tagBuffer);
        }

        const model::ByteArray& getValue() const
        {
          return value;
        }

      public:
        char tagBuffer[24];
        model::ByteArray value;
    };

    class GroupFields
    {
      public:
        void clear()
        {
          fieldsMap_.clear();
        }

        void addField(model::TagType tag, const model::ByteArray& value)
        {
          auto tagValue = std::make_shared<model::TagValue>(tag, value);
          fieldsMap_.insert({tag, tagValue});
        }

        const model::FieldsMap& getFieldsMap() const
        {
          return fieldsMap_;
        }

      private:
        model::FieldsMap fieldsMap_;
    };

  }

  struct ProtocolParserImpl: boost::msm::front::state_machine_def<ProtocolParserImpl>
  {
    ProtocolParserImpl(std::istream& inputStream, char tagValueDelimiter, char fieldsDelimiter):
            inputStream_(inputStream), fieldsGroupCounter_(0), tagValueDelimiter_(tagValueDelimiter), fieldsDelimiter_(fieldsDelimiter)
    {
      logger_ = log4cxx::Logger::getLogger("fixparser.core.ProtocolParser");
    }

    bool isStreamValid() const
    {
      bool result = true;

      if(inputStream_.eof())
      {
        LOG4CXX_INFO(logger_, "End of the input data stream.");
        result = false;
      }
      else if(!inputStream_)
      {
        throw core::exceptions::BaseException("Input data stream is in an error state.");
      }

      return result;
    }

    template <class Fsm>
    bool readField(FieldData& fieldData, Fsm& stateMachine)
    {
      bool result = isStreamValid();

      if(!result)
      {
        return result;
      }
      else
      {
        fieldData.clear();

        inputStream_.getline(fieldData.tagBuffer, sizeof(fieldData.tagBuffer), tagValueDelimiter_);
        LOG4CXX_DEBUG(logger_, "Found tag: " << std::string(fieldData.tagBuffer));

        char symbol = 0;
        result = isStreamValid();
        if(result)
        {
          inputStream_.get(symbol);
          while ((result = isStreamValid()) && symbol != fieldsDelimiter_)
          {
            fieldData.value.push_back(symbol);
            inputStream_.get(symbol);
          }

          LOG4CXX_DEBUG(logger_, "Found value: " << std::string(fieldData.getValue().begin(), fieldData.getValue().end())
                                                 << ", size: " << fieldData.getValue().size());
        }
      }

      return result;
    }

    // Events
    struct InitialEvent {};

    struct HeaderProcessed {};

    struct GroupFieldsProcessed
    {
      FieldData fieldData;
    };

    struct GroupFieldsStarted {};

    struct ControlSumRead
    {
      FieldData fieldData;
    };

    struct ValidMessageProcessed {};

    struct InvalidMessageProcessed {};

    struct UnsupportedMessageFound {};

    struct MessageProcessed {};

    struct EndOfStream {};

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

        FieldData fieldData;

        // model::FixFieldTag::BeginString processing
        READ_OR_RETURN_ON_ENDOF_STREAM(stateMachine, fieldData);

        if(model::FixHelper::testTag(fieldData.getTag(), model::FixFieldTag::BeginString))
        {
            messageVersion.assign(fieldData.getValue().cbegin(), fieldData.getValue().cend());
        }
        else
        {
          LOG4CXX_ERROR(stateMachine.logger_, "FixFieldTag::BeginString is not the first statement.");
          return;
        }

        // model::FixFieldTag::BodyLength processing
        READ_OR_RETURN_ON_ENDOF_STREAM(stateMachine, fieldData);

        if (model::FixHelper::testTag(fieldData.getTag(), model::FixFieldTag::BodyLength))
        {
          messageLength = std::atoi(std::string(fieldData.getValue().cbegin(), fieldData.getValue().cend()).c_str());
        }
        else
        {
          LOG4CXX_ERROR(stateMachine.logger_, "FixFieldTag::BodyLength is not found.");
          return;
        }

        // model::FixFieldTag::MsgType processing
        READ_OR_RETURN_ON_ENDOF_STREAM(stateMachine, fieldData);

        if (model::FixHelper::testTag(fieldData.getTag(), model::FixFieldTag::MsgType))
        {
          if(fieldData.getValue().size() > 1)
          {
            LOG4CXX_ERROR(stateMachine.logger_, "Wrong length for the field FixFieldTag::MsgType.");
          }

          messageType = fieldData.getValue().front();
        }
        else
        {
          LOG4CXX_ERROR(stateMachine.logger_, "FixFieldTag::MsgType is not found.");
          return;
        }

        stateMachine.header_ = model::CreateFixHeader(messageType, messageLength, messageVersion);
        while (!stateMachine.header_->isValid())
        {
          READ_OR_RETURN_ON_ENDOF_STREAM(stateMachine, fieldData);
          stateMachine.header_->addField(fieldData.getTag(), fieldData.getValue());
        }

        auto message = model::FixMessageBuilder().CreateMessage(stateMachine.header_);
        if(message)
        {
          stateMachine.message_ = message;
          stateMachine.process_event(HeaderProcessed());
        }
        else
        {
          stateMachine.process_event(UnsupportedMessageFound());
        }
      }
    };

    struct FieldProcessing : boost::msm::front::state<>
    {
      template <class Fsm>
      bool processField(const FieldData& fieldData, Fsm& stateMachine) const
      {
        if (model::FixHelper::testTag(fieldData.getTag(), model::FixFieldTag::NoMDEntries))
        {
          stateMachine.fieldsGroupCounter_ = model::FixHelper::toInteger(fieldData.getValue());

          stateMachine.message_->addField(fieldData.getTag(), fieldData.getValue());
          stateMachine.process_event(GroupFieldsStarted());
          return false;
        }
        else if (model::FixHelper::testTag(fieldData.getTag(), model::FixFieldTag::CheckSum))
        {
          ControlSumRead controlSumRead{fieldData};

          stateMachine.process_event(controlSumRead);
          return false;
        }
        else
        {
          stateMachine.message_->addField(fieldData.getTag(), fieldData.getValue());
          return true;
        }
      }

      template<class Fsm>
      void on_entry(const HeaderProcessed&, Fsm& stateMachine) const
      {
        LOG4CXX_DEBUG(stateMachine.logger_, "enter in " << __PRETTY_FUNCTION__);

        while(stateMachine.isStreamValid())
        {
          FieldData fieldData;
          stateMachine.readField(fieldData, stateMachine);
          if(!processField(fieldData, stateMachine))
          {
            break;
          }
        }

        LOG4CXX_DEBUG(stateMachine.logger_, "exit from " << __PRETTY_FUNCTION__);
      }

      template<class Fsm>
      void on_entry(const GroupFieldsProcessed& event, Fsm& stateMachine) const
      {
        LOG4CXX_DEBUG(stateMachine.logger_, "enter in " << __PRETTY_FUNCTION__);
        processField(event.fieldData, stateMachine);

        while(stateMachine.isStreamValid())
        {
          FieldData fieldData;
          stateMachine.readField(fieldData, stateMachine);
          if(!processField(fieldData, stateMachine))
          {
            break;
          }
        }
        LOG4CXX_DEBUG(stateMachine.logger_, "exit from " << __PRETTY_FUNCTION__);
      }
    };

    struct GroupFieldsProcessing : boost::msm::front::state<>
    {
      template<class Event, class Fsm>
      void on_entry(Event const&, Fsm& stateMachine) const
      {
        LOG4CXX_DEBUG(stateMachine.logger_, "enter in " << __PRETTY_FUNCTION__);

        const auto& fieldsMap = stateMachine.groupFields_.getFieldsMap();
        const auto& message = stateMachine.message_;
        FieldData fieldData;

        for(size_t i = 0; i < stateMachine.fieldsGroupCounter_; )
        {
          READ_OR_RETURN_ON_ENDOF_STREAM(stateMachine, fieldData);

          if(message->isGroupField(fieldData.getTag()))
          {
            if (fieldsMap.count(fieldData.getTag()))
            {
              stateMachine.message_->addGroupFields(fieldsMap);

              ++i;
              stateMachine.groupFields_.clear();
            }

            stateMachine.groupFields_.addField(fieldData.getTag(), fieldData.getValue());
          }
          else
          {
            LOG4CXX_DEBUG(stateMachine.logger_, "Not group field found: " << fieldData.getTag());
            ++i;
          }
        }

        GroupFieldsProcessed groupFieldsProcessed{fieldData};
        stateMachine.process_event(groupFieldsProcessed);
      }
    };

    struct ControlSumChecking : boost::msm::front::state<>
    {
      template<class Event, class Fsm>
      void on_entry(Event const&, Fsm& stateMachine) const
      {
        LOG4CXX_DEBUG(stateMachine.logger_, "enter in " << __PRETTY_FUNCTION__);

        if(stateMachine.message_)
        {
          if(stateMachine.message_->checkControlSum())
          {
            stateMachine.process_event(ValidMessageProcessed());
            return;
          }
        }

        stateMachine.process_event(InvalidMessageProcessed());
      }
    };

    struct MessageProcessing : boost::msm::front::state<>
    {
      template<class Event, class Fsm>
      void on_entry(Event const&, Fsm& stateMachine) const
      {
        LOG4CXX_DEBUG(stateMachine.logger_, "enter in " << __PRETTY_FUNCTION__);

        stateMachine.fixMessageProcessor_->processMessageAsync(stateMachine.message_);

        stateMachine.message_.reset();
        stateMachine.header_.reset();

        stateMachine.process_event(MessageProcessed());
      }
    };

    struct UnsupportedMessageProcessing : boost::msm::front::state<>
    {
      template<class Event, class Fsm>
      void on_entry(Event const&, Fsm& stateMachine) const
      {
        LOG4CXX_DEBUG(stateMachine.logger_, "enter in " << __PRETTY_FUNCTION__);

        while(stateMachine.isStreamValid())
        {
          FieldData fieldData;
          stateMachine.readField(fieldData, stateMachine);

          if(model::FixHelper::testTag(fieldData.getTag(), model::FixFieldTag::CheckSum))
          {
            stateMachine.process_event(InitialEvent());
            return;
          }
        }
      }
    };

    struct EndOfStreamProcessing : boost::msm::front::state<>
    {
      template<class Event, class Fsm>
      void on_entry(Event const&, Fsm& stateMachine) const
      {
        LOG4CXX_INFO(stateMachine.logger_, "End of input data stream detected.");
        stateMachine.stop();
      }
    };

    // Set initial state
    typedef HeaderProcessing initial_state;
    typedef InitialEvent initial_event;

    // Transition table
    struct transition_table : boost::mpl::vector<
            //          Start   Event   Next    Action      Guard
            boost::msm::front::Row < HeaderProcessing, HeaderProcessed, FieldProcessing, boost::msm::front::none, boost::msm::front::none>,
            boost::msm::front::Row < FieldProcessing, GroupFieldsStarted, GroupFieldsProcessing, boost::msm::front::none, boost::msm::front::none>,
            boost::msm::front::Row < GroupFieldsProcessing, GroupFieldsProcessed, FieldProcessing, boost::msm::front::none, boost::msm::front::none>,
            boost::msm::front::Row < FieldProcessing, ControlSumRead, ControlSumChecking, boost::msm::front::none, boost::msm::front::none>,
            boost::msm::front::Row < ControlSumChecking, ValidMessageProcessed, MessageProcessing, boost::msm::front::none, boost::msm::front::none>,
            boost::msm::front::Row < ControlSumChecking, InvalidMessageProcessed, HeaderProcessing, boost::msm::front::none, boost::msm::front::none>,
            boost::msm::front::Row < MessageProcessing, MessageProcessed, HeaderProcessing, boost::msm::front::none, boost::msm::front::none>,
            boost::msm::front::Row < HeaderProcessing, UnsupportedMessageFound, UnsupportedMessageProcessing, boost::msm::front::none, boost::msm::front::none>,
            boost::msm::front::Row < UnsupportedMessageProcessing, InitialEvent, HeaderProcessing, boost::msm::front::none, boost::msm::front::none>,
            boost::msm::front::Row < HeaderProcessing, EndOfStream, EndOfStreamProcessing, boost::msm::front::none, boost::msm::front::none>,
            boost::msm::front::Row < FieldProcessing, EndOfStream, EndOfStreamProcessing, boost::msm::front::none, boost::msm::front::none>,
            boost::msm::front::Row < GroupFieldsProcessing, EndOfStream, EndOfStreamProcessing, boost::msm::front::none, boost::msm::front::none>
    > {};

    std::istream& inputStream_;
    log4cxx::LoggerPtr logger_;

    model::FixBaseHeader::Ptr header_;
    model::FixBaseMessage::Ptr message_;
    GroupFields groupFields_;
    size_t fieldsGroupCounter_;

    char tagValueDelimiter_;
    char fieldsDelimiter_;

    FixMessageProcessor::Ptr fixMessageProcessor_;
  };

  ProtocolParser::~ProtocolParser() = default;

  void ProtocolParser::run(std::istream& inputStream, char tagValueDelimiter, char fieldsDelimiter)
  {
    typedef boost::msm::back::state_machine<ProtocolParserImpl> StateMachine;

    StateMachine stateMachine(std::ref(inputStream), tagValueDelimiter, fieldsDelimiter);
    stateMachine.fixMessageProcessor_ = fixMessageProcessor_;
    stateMachine.start();
  }

  ProtocolParser::ProtocolParser(const FixMessageProcessor::Ptr& fixMessageProcessor):
  fixMessageProcessor_(fixMessageProcessor)
  {

  }

}
}


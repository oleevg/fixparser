/*
 * FixMessageProcessor.cpp
 *
 *  Created on: 12/15/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <queue>
#include <condition_variable>

#include <boost/format.hpp>

#include <log4cxx/logger.h>

#include <model/FixHelper.hpp>

#include <core/FixMessageProcessor.hpp>
#include <core/OrderBook.hpp>
#include <core/exceptions/BaseException.hpp>

namespace fixparser {
namespace core {

  class FixMessageProcessorImpl
  {
    public:
      typedef std::queue<model::FixBaseMessage::Ptr> Messages;

    public:
      FixMessageProcessorImpl(const OrderBook::Ptr& _orderBook) :
              orderBook(_orderBook), processingThreadExit(false)
      {
        logger = log4cxx::Logger::getLogger("fixparser.core.FixMessageProcessor");

        processingThread = std::thread([this]()
        {
          LOG4CXX_INFO(logger, "FIX messages processing thread started.");

          while(!processingThreadExit)
          {
            std::unique_lock<std::mutex> lck(messagesMtx);
            while (messages.empty() && !processingThreadExit)
            {
              messagesCondVar.wait_for(lck, std::chrono::seconds(1));
            }

            if(processingThreadExit)
            {
              return;
            }

            auto message = messages.front();
            messages.pop();
            lck.unlock();

            processMessage(message);
          }

          LOG4CXX_INFO(logger, "FIX messages processing thread finished.");
        });
      }

      ~FixMessageProcessorImpl()
      {
        processingThreadExit = true;
        processingThread.join();
      }

      void addMessageAsync(const model::FixBaseMessage::Ptr& message)
      {
        std::lock_guard<std::mutex> lck(messagesMtx);
        messages.push(message);
        messagesCondVar.notify_one();
      }

    private:
      model::TagValue::Ptr findField(const model::FieldsMap& fieldsMap, model::FixFieldTag tag)
      {
        auto iter = fieldsMap.find(static_cast<model::TagType>(tag));
        if(iter == fieldsMap.end())
        {
          throw core::exceptions::BaseException((boost::format("Couldn't find field with the tag: %d") % static_cast<model::TagType>(tag)).str());
        }

        return iter->second;
      }

      float getFloatValue(const model::FieldsMap& fieldsMap, model::FixFieldTag tag)
      {
        auto value = findField(fieldsMap, tag);

        return model::FixHelper::toFloat(value->value);
      }

      unsigned int getIntValue(const model::FieldsMap& fieldsMap, model::FixFieldTag tag)
      {
        auto value = findField(fieldsMap, tag);

        return model::FixHelper::toInteger(value->value);
      }

      OrderBookItemType convertToOrderBookItemType(unsigned int value)
      {
        static struct
        {
          int value;
          OrderBookItemType itemTypeValue;
        } conversions[]
        {
                {0, OrderBookItemType::Buy},
                {1, OrderBookItemType::Sell},
                {2, OrderBookItemType::Trade}
        };

        for (const auto& conv : conversions)
        {
          if(value == conv.value)
          {
            return conv.itemTypeValue;
          }
        }

        return OrderBookItemType::Unknown;
      }

      model::MarketDataUpdateAction convertToMarketDataUpdateAction(unsigned int value)
      {
        static struct
        {
          int value;
          model::MarketDataUpdateAction updateAction;
        } conversions []
        {
                {0, model::MarketDataUpdateAction::New},
                {1, model::MarketDataUpdateAction::Change},
                {2, model::MarketDataUpdateAction::Delete}
        };

        for (const auto& conv : conversions)
        {
          if(value == conv.value)
          {
            return conv.updateAction;
          }
        }

        throw core::exceptions::BaseException((boost::format("Unsupported update action type found: %d") % value).str());
      }

      void processMessage(const model::FixBaseMessage::Ptr& message)
      {
        auto messageType = message->getMessageType();
        LOG4CXX_DEBUG(logger, "Processing FIX message with type: " << static_cast<char>(messageType));

        if(messageType == model::FixMessageType::MarketDataSnapshot)
        {
          auto groupFieldsSize = message->getGroupFieldsSize();
          for (size_t i = 0; i < groupFieldsSize; ++i)
          {
            auto groupFields = message->getGroupFields(i);

            auto entryType = getIntValue(groupFields, model::FixFieldTag::MDEntryType);
            auto price = getFloatValue(groupFields, model::FixFieldTag::MDEntryPx);
            auto quantity = getIntValue(groupFields, model::FixFieldTag::MDEntrySize);

            orderBook->addItem(convertToOrderBookItemType(entryType), {price, quantity});
          }
        }
        else if(messageType == model::FixMessageType::MarketDataIncrement)
        {
          auto groupFieldsSize = message->getGroupFieldsSize();
          for (size_t i = 0; i < groupFieldsSize; ++i)
          {
            auto groupFields = message->getGroupFields(i);

            auto entryType = getIntValue(groupFields, model::FixFieldTag::MDEntryType);
            auto updateAction = getIntValue(groupFields, model::FixFieldTag::MDUpdateAction);
            auto price = getFloatValue(groupFields, model::FixFieldTag::MDEntryPx);
            auto quantity = getIntValue(groupFields, model::FixFieldTag::MDEntrySize);

            auto mdUpdateAction = convertToMarketDataUpdateAction(updateAction);
            auto mdEntryType = convertToOrderBookItemType(entryType);

            if(mdUpdateAction == model::MarketDataUpdateAction::New)
            {
              orderBook->addItem(mdEntryType, {price, quantity});
            }
            else if(mdUpdateAction == model::MarketDataUpdateAction::Delete)
            {
              orderBook->deleteItem(mdEntryType, {price, quantity});
            }
            else if(mdUpdateAction == model::MarketDataUpdateAction::Change)
            {
              orderBook->changeItem(mdEntryType, {price, quantity});
            }
            else
            {
              LOG4CXX_WARN(logger, "Not supported update action found: " << updateAction);
            }
          }
        }
        else
        {
          LOG4CXX_WARN(logger, "Couldn't process unsupported FIX message with type: " << static_cast<char>(messageType));
        }
      }

    private:
      log4cxx::LoggerPtr logger;
      OrderBook::Ptr orderBook;

      Messages messages;

      std::mutex messagesMtx;
      std::condition_variable messagesCondVar;

      std::thread processingThread;
      std::atomic<bool> processingThreadExit;
  };

  FixMessageProcessor::FixMessageProcessor(const OrderBook::Ptr& orderBook)
  {
    impl_ = std::make_unique<FixMessageProcessorImpl>(orderBook);
  }

  FixMessageProcessor::~FixMessageProcessor() = default;


  void FixMessageProcessor::processMessageAsync(const model::FixBaseMessage::Ptr& message)
  {
    impl_->addMessageAsync(message);
  }

}
}

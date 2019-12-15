/*
 * FixMessageProcessor.hpp
 *
 *  Created on: 12/15/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#ifndef FIXPARSER_FIXMESSAGEPROCESSOR_HPP
#define FIXPARSER_FIXMESSAGEPROCESSOR_HPP

#include <memory>

#include <model/FixBaseMessage.hpp>

#include <core/OrderBook.hpp>

namespace fixparser {
namespace core {

  class FixMessageProcessorImpl;

  class FixMessageProcessor
  {
    public:
      typedef std::shared_ptr<FixMessageProcessor> Ptr;

    public:
      FixMessageProcessor(const OrderBook::Ptr& orderBook);

      ~FixMessageProcessor();

      void processMessageAsync(const model::FixBaseMessage::Ptr& message);

    private:
      std::unique_ptr<FixMessageProcessorImpl> impl_;
  };

}
}


#endif //FIXPARSER_FIXMESSAGEPROCESSOR_HPP

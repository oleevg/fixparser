/*
 * ConsoleOrderBookObserver.cpp
 *
 *  Created on: 12/15/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#include <iostream>

#include <log4cxx/logger.h>

#include <core/OrderBook.hpp>
#include <core/OrderBookItem.hpp>
#include <core/IOrderBookObserver.hpp>

namespace fixparser {
namespace core {

  std::ostream& operator << (std::ostream& stream, const OrderBookItem& orderBookItem)
  {
    stream << "price: " << orderBookItem.price << " (" << orderBookItem.quantity << ")" << std::endl;
  }

  class ConsoleOrderBookObserver : public IOrderBookObserver
  {
    public:
      ConsoleOrderBookObserver(size_t orderBookItems, const OrderBook::Ptr& orderBook):
      orderBookItemsSize_(orderBookItems), orderBook_(orderBook)
      {
        logger_ = log4cxx::Logger::getLogger("fixparser.core.ConsoleOrderBookObserver");
      }

      void observe() override
      {
        LOG4CXX_DEBUG(logger_, "Notification received.");

        std::cout << "[Order book state]" << std::endl;

        std::cout << " Total SELL: " << orderBookItemsSize_ << std::endl;
        auto items = orderBook_->getNthLowestSelling(orderBookItemsSize_);

        auto counter = items.size();
        for (auto iter = items.crbegin(); iter != items.crend(); ++iter)
        {
          std::cout << "[" << counter-- << "]: " << *iter << std::endl;
        }

        items = orderBook_->getNthHighestBuying(orderBookItemsSize_);
        counter = items.size();
        for (const auto& item: items)
        {
          std::cout << "[" << counter-- << "]: " << item << std::endl;
        }
        std::cout << " Total BUY: " << orderBookItemsSize_ << std::endl;
      }

    private:
      OrderBook::Ptr orderBook_;
      size_t orderBookItemsSize_;

      log4cxx::LoggerPtr logger_;
  };

  IOrderBookObserver::Ptr CreateConsoleOrderBookObserver(size_t orderBookItemsSize, const OrderBook::Ptr& orderBook)
  {
    return std::make_shared<ConsoleOrderBookObserver>(orderBookItemsSize, orderBook);
  }

}
}


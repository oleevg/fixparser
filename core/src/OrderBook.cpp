/*
 * OrderBook.cpp
 *
 *  Created on: 12/15/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#include <algorithm>
#include <functional>

#include <log4cxx/logger.h>

#include <core/OrderBookItem.hpp>
#include <core/OrderBook.hpp>

namespace fixparser {
namespace core {

  class OrderBookImpl
  {
    public:
      typedef std::function<bool(const OrderBookItem& lhs, const OrderBookItem& rhs)> OrderBookItemsComparator;

    public:
      OrderBookImpl(size_t expectedSize)
      {
        logger = log4cxx::Logger::getLogger("fixparser.core.OrderBook");

        sellItemsComparator = [](const OrderBookItem& lhs, const OrderBookItem& rhs) -> bool
        {
          return lhs.price < rhs.price;
        };

        buyItemsComparator = [](const OrderBookItem& lhs, const OrderBookItem& rhs) -> bool
        {
          return lhs.price > rhs.price;
        };

        sellItems.reserve(expectedSize);
        buyItems.reserve(expectedSize);
      }

      bool addItem(OrderBookItemType type, const OrderBookItem& item)
      {
        OrderBookItems* items = &buyItems;
        OrderBookItemsComparator* comparator = &buyItemsComparator;

        if(type == OrderBookItemType::Sell)
        {
          items = &sellItems;
          comparator = &sellItemsComparator;
        }

        auto iter = std::upper_bound(items->cbegin(), items->cend(), item, *comparator);
        items->insert(iter, item);

        return true;
      }

      bool deleteItem(OrderBookItemType type, const OrderBookItem& item)
      {
        bool result = true;

        OrderBookItems* items = &buyItems;
        OrderBookItemsComparator* comparator = &buyItemsComparator;

        if (type == OrderBookItemType::Sell)
        {
          items = &sellItems;
          comparator = &sellItemsComparator;
        }

        auto iter = findItem(*items, *comparator, item);

        if(iter != items->end())
        {
          items->erase(iter);
        }
        else
        {
          LOG4CXX_WARN(logger, "Couldn't delete not present order book item with the price " << item.price);
          result = false;
        }

        return result;
      }

      bool changeItem(OrderBookItemType type, const OrderBookItem& item)
      {
        bool result = true;

        OrderBookItems* items = &buyItems;
        OrderBookItemsComparator* comparator = &buyItemsComparator;

        if (type == OrderBookItemType::Sell)
        {
          items = &sellItems;
          comparator = &sellItemsComparator;
        }

        auto iter = findItem(*items, *comparator, item);
        if(iter != items->end())
        {
          LOG4CXX_DEBUG(logger, "Updating order book item for the price " << item.price << ". Old quantity: " << iter->quantity << ", new quantity: " << item.quantity);
          iter->quantity = item.quantity;
        }
        else
        {
          LOG4CXX_WARN(logger, "Couldn't update not present order book item with the price " << item.price);
          result = false;
        }

        return true;
      }

      OrderBookItems getNthLowestSelling(size_t itemsCount) const
      {
        auto count = std::min(itemsCount, sellItems.size());

        return OrderBookItems(sellItems.cbegin(), sellItems.cbegin() + count);
      }

      OrderBookItems getNthHighestBuying(size_t itemsCount) const
      {
        auto count = std::min(itemsCount, buyItems.size());

        return OrderBookItems(buyItems.cbegin(), buyItems.cbegin() + count);
      }

    private:
      OrderBookItems::iterator findItem(OrderBookItems& items, OrderBookItemsComparator& comparator, const OrderBookItem& item)
      {
        auto iters = std::equal_range(items.begin(), items.end(), item, comparator);
        if(iters.first == items.end())
        {
          LOG4CXX_WARN(logger, "Order book item with the price " << item.price << " is not present.");
          return items.end();
        }

        return iters.first;
      }

    private:
      log4cxx::LoggerPtr logger;

      OrderBookItems sellItems; // Asks. Ascending order
      OrderBookItemsComparator sellItemsComparator;

      OrderBookItems buyItems;  // Bids. Descending order
      OrderBookItemsComparator buyItemsComparator;
  };

  OrderBook::OrderBook(size_t expectedSize)
  {
    impl_ = std::make_unique<OrderBookImpl>(expectedSize);
  }

  OrderBook::~OrderBook() = default;

  void OrderBook::addObserver(const IOrderBookObserver::Ptr& observer)
  {
    observers_.push_back(observer);
  }

  void OrderBook::addItem(OrderBookItemType type, const OrderBookItem& item)
  {
    if(impl_->addItem(type, item))
    {
      notify();
    }
  }

  void OrderBook::deleteItem(OrderBookItemType type, const OrderBookItem& item)
  {
    if(impl_->deleteItem(type, item))
    {
      notify();
    }
  }

  void OrderBook::changeItem(OrderBookItemType type, const OrderBookItem& item)
  {
    if(impl_->changeItem(type, item))
    {
      notify();
    }
  }

  OrderBookItems OrderBook::getNthLowestSelling(size_t itemsCount) const
  {
    return impl_->getNthLowestSelling(itemsCount);
  }

  OrderBookItems OrderBook::getNthHighestBuying(size_t itemsCount) const
  {
    return impl_->getNthHighestBuying(itemsCount);
  }

  void OrderBook::notify()
  {
    for(auto& observer: observers_)
    {
      observer->observe();
    }
  }

}
}

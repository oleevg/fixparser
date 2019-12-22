/*
 * OrderBook.hpp
 *
 *  Created on: 12/15/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#ifndef FIXPARSER_ORDERBOOK_HPP
#define FIXPARSER_ORDERBOOK_HPP

#include <memory>
#include <list>
#include <vector>

#include <core/OrderBookItem.hpp>
#include <core/IOrderBookObserver.hpp>

namespace fixparser {
namespace core {

  enum class OrderBookItemType
  {
      Buy = 0,
      Sell = 1,
      Trade = 2,
      Unknown = 0x0FFFFFFF
  };

  typedef std::vector<OrderBookItem> OrderBookItems;

  class OrderBookImpl;

  class OrderBook
  {
    public:
      typedef std::shared_ptr<OrderBook> Ptr;
      typedef std::list<IOrderBookObserver::Ptr> Observers;

    public:
      OrderBook(size_t expectedSize = 2000);

      ~OrderBook();

      void addObserver(const IOrderBookObserver::Ptr& observer);

      void addItem(OrderBookItemType type, const OrderBookItem& item);

      void deleteItem(OrderBookItemType type, const OrderBookItem& item);

      void changeItem(OrderBookItemType type, const OrderBookItem& item);

      OrderBookItems getNthLowestSelling(size_t itemsCount) const;

      OrderBookItems getNthHighestBuying(size_t itemsCount) const;

    private:
      void notify();

    private:
      Observers observers_;
      std::unique_ptr<OrderBookImpl> impl_;
  };

  IOrderBookObserver::Ptr CreateConsoleOrderBookObserver(size_t orderBookItemsSize, const OrderBook::Ptr& orderBook);

}
}


#endif //FIXPARSER_ORDERBOOK_HPP

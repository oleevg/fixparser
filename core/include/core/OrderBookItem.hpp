/*
 * OrderBookItem.hpp
 *
 *  Created on: 12/15/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#ifndef FIXPARSER_ORDERBOOKITEM_HPP
#define FIXPARSER_ORDERBOOKITEM_HPP

namespace fixparser {
namespace core {

  class OrderBookItem
  {
    public:
      float price;
      size_t quantity;
  };

}
}

#endif //FIXPARSER_ORDERBOOKITEM_HPP

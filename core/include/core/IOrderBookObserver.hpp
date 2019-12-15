/*
 * IOrderBookObserver.hpp
 *
 *  Created on: 12/15/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#ifndef FIXPARSER_IORDERBOOKOBSERVER_HPP
#define FIXPARSER_IORDERBOOKOBSERVER_HPP

#include <memory>

namespace fixparser {
namespace core {

  class IOrderBookObserver
  {
    public:
      typedef std::shared_ptr<IOrderBookObserver> Ptr;

    public:
      virtual ~IOrderBookObserver() = default;

      virtual void observe() = 0;
  };

}
}

#endif //FIXPARSER_IORDERBOOKOBSERVER_HPP

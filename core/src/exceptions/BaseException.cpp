/*
 * BaseException.cpp
 *
 *  Created on: 12/13/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#include <core/exceptions/BaseException.hpp>

namespace fixparser {
namespace core {
namespace exceptions {

  BaseException::BaseException(const std::string& errorMessage, const std::string& header):
  errorMessage_(header + errorMessage)
  {

  }

  const char* BaseException::what() const noexcept
  {
    return errorMessage_.c_str();
  }

}
}
}


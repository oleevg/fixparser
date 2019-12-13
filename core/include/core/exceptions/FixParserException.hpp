#include <core/exceptions/BaseException.hpp>

namespace fixparser {
namespace core {
namespace exceptions {

class FixParserException: public BaseException
{
  public:
  FixParserException(const std::string& message):
  BaseException("Error in parsing FIX messages. " + message)
  {

  }

};

} // namespace exceptions
}
}
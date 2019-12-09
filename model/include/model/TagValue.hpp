/*
 * TagValue.hpp
 *
 *  Created on: 12/7/19
 *      Author: Oleg F., fedorov.ftf@gmail.com
 */

#ifndef FIXPARSER_TAGVALUE_HPP
#define FIXPARSER_TAGVALUE_HPP

#include <vector>
#include <memory>

namespace fixparser {
namespace model {

  typedef std::vector<char> ByteArray;
  typedef unsigned int TagType;

  class TagValue
  {
    public:
      typedef std::shared_ptr<TagValue> Ptr;

    public:
      TagValue(TagType tagType, const ByteArray& byteArray):
      tag(tagType), value(byteArray)
      {}

      TagType tag;
      ByteArray value;
  };

}
}

#endif //FIXPARSER_TAGVALUE_HPP

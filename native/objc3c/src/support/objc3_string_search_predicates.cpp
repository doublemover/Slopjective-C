#include "support/objc3_string_search_predicates.h"

namespace objc3c::support {

bool Contains(std::string_view value, std::string_view needle) {
  return value.find(needle) != std::string_view::npos;
}

}  // namespace objc3c::support

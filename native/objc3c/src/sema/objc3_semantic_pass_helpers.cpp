#include "sema/objc3_semantic_pass_helpers.h"

#include <sstream>

std::string JoinStringVector(const std::vector<std::string> &items,
                             const std::string &separator) {
  std::ostringstream out;
  for (std::size_t index = 0; index < items.size(); ++index) {
    if (index != 0u) {
      out << separator;
    }
    out << items[index];
  }
  return out.str();
}

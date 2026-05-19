#include "support/objc3_string_join.h"

namespace objc3c::support {

std::string JoinStringVector(const std::vector<std::string> &items,
                             std::string_view separator) {
  std::size_t size = 0;
  for (const std::string &item : items) {
    size += item.size();
  }
  if (!items.empty()) {
    size += separator.size() * (items.size() - 1u);
  }

  std::string joined;
  joined.reserve(size);
  for (std::size_t index = 0; index < items.size(); ++index) {
    if (index != 0u) {
      joined.append(separator.data(), separator.size());
    }
    joined.append(items[index]);
  }
  return joined;
}

}  // namespace objc3c::support

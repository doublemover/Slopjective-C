#include "support/objc3_method_family.h"

namespace objc3c::support {

bool SelectorStartsWith(std::string_view selector, std::string_view prefix) {
  return selector.size() >= prefix.size() &&
         selector.compare(0u, prefix.size(), prefix) == 0;
}

std::string ClassifyMethodFamilyFromSelector(std::string_view selector) {
  if (SelectorStartsWith(selector, "mutableCopy")) {
    return "mutableCopy";
  }
  if (SelectorStartsWith(selector, "copy")) {
    return "copy";
  }
  if (SelectorStartsWith(selector, "init")) {
    return "init";
  }
  if (SelectorStartsWith(selector, "new")) {
    return "new";
  }
  return "none";
}

}  // namespace objc3c::support

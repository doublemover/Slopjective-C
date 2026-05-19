#include "support/objc3_method_family.h"

#include "support/objc3_string_predicates.h"

namespace objc3c::support {

std::string ClassifyMethodFamilyFromSelector(std::string_view selector) {
  if (StartsWith(selector, "mutableCopy")) {
    return "mutableCopy";
  }
  if (StartsWith(selector, "copy")) {
    return "copy";
  }
  if (StartsWith(selector, "init")) {
    return "init";
  }
  if (StartsWith(selector, "new")) {
    return "new";
  }
  return "none";
}

}  // namespace objc3c::support

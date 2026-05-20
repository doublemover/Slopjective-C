#include "support/objc3_method_family.h"

#include "support/objc3_string_predicates.h"

namespace objc3c::support {

namespace {

bool IsAsciiLowercase(char c) {
  return c >= 'a' && c <= 'z';
}

bool SelectorStartsMethodFamily(std::string_view selector,
                                std::string_view family) {
  if (!StartsWith(selector, family)) {
    return false;
  }
  return selector.size() == family.size() ||
         !IsAsciiLowercase(selector[family.size()]);
}

}  // namespace

std::string ClassifyMethodFamilyFromSelector(std::string_view selector) {
  if (SelectorStartsMethodFamily(selector, "alloc")) {
    return "alloc";
  }
  if (SelectorStartsMethodFamily(selector, "mutableCopy")) {
    return "mutableCopy";
  }
  if (SelectorStartsMethodFamily(selector, "copy")) {
    return "copy";
  }
  if (SelectorStartsMethodFamily(selector, "init")) {
    return "init";
  }
  if (SelectorStartsMethodFamily(selector, "new")) {
    return "new";
  }
  return "none";
}

}  // namespace objc3c::support

#include "runtime/selectors/selector_spelling.h"

#include <string_view>

namespace {

bool IsRuntimeSelectorSpellingCodeUnitAllowed(char c) {
  const unsigned char code_unit = static_cast<unsigned char>(c);
  return code_unit >= 0x20u && code_unit != 0x7fu;
}

bool IsValidRuntimeSelectorSpelling(std::string_view selector) {
  if (selector.empty()) {
    return false;
  }
  for (char c : selector) {
    if (!IsRuntimeSelectorSpellingCodeUnitAllowed(c)) {
      return false;
    }
  }
  return true;
}

}  // namespace

namespace objc3c::runtime {

const char *NormalizeRuntimeSelectorSpelling(const char *selector) {
  return selector;
}

bool RuntimeSelectorTableAcceptsDynamicSelector(const char *selector) {
  return NormalizeRuntimeSelectorSpelling(selector) != nullptr;
}

bool RuntimeSelectorTableAcceptsMetadataSelector(const char *selector) {
  const char *canonical_selector = NormalizeRuntimeSelectorSpelling(selector);
  return canonical_selector != nullptr &&
         IsValidRuntimeSelectorSpelling(std::string_view(canonical_selector));
}

}  // namespace objc3c::runtime

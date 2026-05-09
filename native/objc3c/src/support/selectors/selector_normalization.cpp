#include "support/selectors/selector_normalization.h"

#include <string_view>

#include "support/selectors/selector_spelling.h"

namespace objc3c::support::selectors {

const char *NormalizeSelectorSpelling(const char *selector) {
  return selector;
}

bool IsValidMetadataSelectorSpelling(const char *selector) {
  return selector != nullptr && IsValidSelectorSpelling(std::string_view(selector));
}

}  // namespace objc3c::support::selectors

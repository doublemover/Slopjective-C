#include "support/selectors/metadata_selector_validation.h"

#include <string_view>

#include "support/selectors/selector_validation.h"

namespace objc3c::support::selectors {

bool IsValidMetadataSelectorSpelling(const char *selector) {
  return selector != nullptr &&
         IsValidSelectorSpelling(std::string_view(selector));
}

}  // namespace objc3c::support::selectors

#include "support/selectors/selector_normalization.h"

namespace objc3c::support::selectors {

const char *NormalizeSelectorSpelling(const char *selector) {
  return selector;
}

bool IsValidMetadataSelectorSpelling(const char *selector) {
  return selector != nullptr && selector[0] != '\0';
}

}  // namespace objc3c::support::selectors

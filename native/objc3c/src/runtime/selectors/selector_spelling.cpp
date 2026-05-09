#include "runtime/selectors/selector_spelling.h"

#include "support/selectors/selector_normalization.h"

namespace objc3c::runtime {

const char *NormalizeRuntimeSelectorSpelling(const char *selector) {
  return objc3c::support::selectors::NormalizeSelectorSpelling(selector);
}

bool RuntimeSelectorTableAcceptsDynamicSelector(const char *selector) {
  return NormalizeRuntimeSelectorSpelling(selector) != nullptr;
}

bool RuntimeSelectorTableAcceptsMetadataSelector(const char *selector) {
  return objc3c::support::selectors::IsValidMetadataSelectorSpelling(
      NormalizeRuntimeSelectorSpelling(selector));
}

}  // namespace objc3c::runtime

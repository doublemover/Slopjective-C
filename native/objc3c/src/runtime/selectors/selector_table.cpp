#include "runtime/selectors/selector_table.h"

#include "support/selectors/selector_normalization.h"

namespace objc3c::runtime {

bool RuntimeSelectorTableAcceptsDynamicSelector(const char *selector) {
  return objc3c::support::selectors::NormalizeSelectorSpelling(selector) !=
         nullptr;
}

bool RuntimeSelectorTableAcceptsMetadataSelector(const char *selector) {
  return objc3c::support::selectors::IsValidMetadataSelectorSpelling(
      objc3c::support::selectors::NormalizeSelectorSpelling(selector));
}

}  // namespace objc3c::runtime

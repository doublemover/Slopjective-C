#include "runtime/storage/property_accessors.h"

#include "support/selectors/selector_normalization.h"

namespace objc3c::runtime {

bool RuntimePropertyAccessorSelectorIsMaterializable(const char *selector) {
  return objc3c::support::selectors::IsValidMetadataSelectorSpelling(
      objc3c::support::selectors::NormalizeSelectorSpelling(selector));
}

bool RuntimePropertySetterHasSupportedArity(unsigned long long parameter_count) {
  return parameter_count == 1;
}

}  // namespace objc3c::runtime

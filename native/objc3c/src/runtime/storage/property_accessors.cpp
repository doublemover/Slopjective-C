#include "runtime/storage/property_accessors.h"

#include "runtime/selectors/selector_spelling.h"

namespace objc3c::runtime {

bool RuntimePropertyAccessorSelectorIsMaterializable(const char *selector) {
  return RuntimeSelectorTableAcceptsMetadataSelector(selector);
}

bool RuntimePropertySetterHasSupportedArity(unsigned long long parameter_count) {
  return parameter_count == 1;
}

}  // namespace objc3c::runtime

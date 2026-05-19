#pragma once

namespace objc3c::runtime {

const char *NormalizeRuntimeSelectorSpelling(const char *selector);
bool RuntimeSelectorTableAcceptsDynamicSelector(const char *selector);
bool RuntimeSelectorTableAcceptsMetadataSelector(const char *selector);

}  // namespace objc3c::runtime

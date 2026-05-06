#pragma once

namespace objc3c::runtime {

bool RuntimeSelectorTableAcceptsDynamicSelector(const char *selector);
bool RuntimeSelectorTableAcceptsMetadataSelector(const char *selector);

}  // namespace objc3c::runtime

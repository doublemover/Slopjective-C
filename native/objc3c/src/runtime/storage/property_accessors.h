#pragma once

namespace objc3c::runtime {

bool RuntimePropertyAccessorSelectorIsMaterializable(const char *selector);
bool RuntimePropertySetterHasSupportedArity(unsigned long long parameter_count);

}  // namespace objc3c::runtime

#pragma once

#include <string_view>

namespace objc3c::support {

extern const char kObjc3PropertyWeakLifetimeProfile[];
extern const char kObjc3PropertyStrongOwnedLifetimeProfile[];
extern const char kObjc3PropertyWeakRuntimeHookProfile[];

bool PropertyAttributeProfileContains(std::string_view profile,
                                      std::string_view needle);

}  // namespace objc3c::support

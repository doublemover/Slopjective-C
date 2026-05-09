#pragma once

#include <string>

namespace objc3c::runtime {

enum class DispatchFamily;
struct SlowPathResolution;

bool TryResolveRuntimeBuiltinObjectMethod(
    const std::string &class_name,
    DispatchFamily family,
    const char *selector_spelling,
    SlowPathResolution &resolution);

}  // namespace objc3c::runtime

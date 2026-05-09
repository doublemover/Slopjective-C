#pragma once

#include <string>

namespace objc3c::support {

std::string BuildRuntimeBackedPropertyStorageModifier(
    bool is_copy,
    bool is_strong,
    bool is_retain,
    bool is_weak,
    bool is_unowned,
    bool is_assign,
    bool is_unsafe_unretained);

}  // namespace objc3c::support

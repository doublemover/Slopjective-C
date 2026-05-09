#pragma once

#include <string_view>

namespace objc3c::support {

bool IsObjectPointerTypeNameConsistent(
    bool object_pointer_type_spelling,
    std::string_view object_pointer_type_name);

}  // namespace objc3c::support

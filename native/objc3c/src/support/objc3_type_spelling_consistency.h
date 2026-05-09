#pragma once

#include <string_view>

namespace objc3c::support {

bool IsObjectPointerTypeNameConsistent(
    bool object_pointer_type_spelling,
    std::string_view object_pointer_type_name);
bool IsPointerDeclaratorDepthConsistent(
    bool has_pointer_declarator,
    unsigned pointer_declarator_depth);

}  // namespace objc3c::support

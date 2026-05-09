#include "support/objc3_type_spelling_consistency.h"

namespace objc3c::support {

bool IsObjectPointerTypeNameConsistent(
    bool object_pointer_type_spelling,
    std::string_view object_pointer_type_name) {
  return !object_pointer_type_spelling || !object_pointer_type_name.empty();
}

bool IsPointerDeclaratorDepthConsistent(bool has_pointer_declarator,
                                        unsigned pointer_declarator_depth) {
  return has_pointer_declarator ? pointer_declarator_depth > 0u
                                : pointer_declarator_depth == 0u;
}

}  // namespace objc3c::support

#include "support/objc3_object_pointer_type_name_consistency.h"

namespace objc3c::support {

bool IsObjectPointerTypeNameConsistent(
    bool object_pointer_type_spelling,
    std::string_view object_pointer_type_name) {
  return !object_pointer_type_spelling || !object_pointer_type_name.empty();
}

}  // namespace objc3c::support

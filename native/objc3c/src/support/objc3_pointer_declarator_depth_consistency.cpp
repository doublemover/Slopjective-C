#include "support/objc3_pointer_declarator_depth_consistency.h"

namespace objc3c::support {

bool IsPointerDeclaratorDepthConsistent(bool has_pointer_declarator,
                                        unsigned pointer_declarator_depth) {
  return has_pointer_declarator ? pointer_declarator_depth > 0u
                                : pointer_declarator_depth == 0u;
}

}  // namespace objc3c::support

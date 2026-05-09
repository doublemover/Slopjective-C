#include "runtime/errors/error_bridge_kind.h"

namespace objc3c::runtime {

const char *RuntimeErrorCatchKindName(int catch_kind) {
  switch (catch_kind) {
  case 1:
    return "nserror";
  case 2:
    return "id<error>";
  default:
    return "unknown";
  }
}

}  // namespace objc3c::runtime

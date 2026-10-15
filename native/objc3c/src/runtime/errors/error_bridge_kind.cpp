#include "runtime/errors/error_bridge_kind.h"

namespace objc3c::runtime {

const char *RuntimeErrorCatchKindName(int catch_kind) {
  switch (catch_kind) {
  case 1:
    return "nserror";
  case 2:
    return "id<error>";
  case 3:
    return "foreign-exception";
  case 4:
    return "typed-error";
  default:
    return "unknown";
  }
}

const char *RuntimeForeignExceptionKindName(int foreign_kind) {
  switch (foreign_kind) {
  case 1:
    return "objc-exception";
  case 2:
    return "cxx-exception";
  case 3:
    return "swift-error";
  default:
    return "unsupported-foreign-exception";
  }
}

bool RuntimeForeignExceptionKindIsSupported(int foreign_kind) {
  return foreign_kind == 1 || foreign_kind == 2 || foreign_kind == 3;
}

}  // namespace objc3c::runtime

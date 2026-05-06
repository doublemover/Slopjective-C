#include "runtime/dispatch/builtin_methods.h"

namespace objc3c::runtime {

const char *DescribeResolvedImplementationKind(
    RuntimeBuiltinKind builtin_kind, const void *implementation) {
  switch (builtin_kind) {
    case RuntimeBuiltinKind::Alloc:
      return "builtin-alloc";
    case RuntimeBuiltinKind::Init:
      return "builtin-init";
    case RuntimeBuiltinKind::New:
      return "builtin-new";
    case RuntimeBuiltinKind::PropertyGetter:
      return "builtin-property-getter";
    case RuntimeBuiltinKind::PropertySetter:
      return "builtin-property-setter";
    case RuntimeBuiltinKind::None:
      break;
  }
  if (implementation != nullptr) {
    return "emitted-method-body";
  }
  return "";
}

}  // namespace objc3c::runtime

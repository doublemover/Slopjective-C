#include "runtime/dispatch/runtime_method_shape.h"

#include "runtime/dispatch/runtime_method_return.h"

namespace objc3c::runtime {

objc3_runtime_dispatch_status_code RuntimeMethodShapeStatus(
    const char *return_type_name, std::uint64_t parameter_count) {
  if (parameter_count > 4) {
    return OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT;
  }
  const RuntimeMethodReturnKind return_kind =
      ClassifyRuntimeReturnType(return_type_name);
  if (!RuntimeMethodReturnKindIsDispatchResultSupported(return_kind)) {
    return OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE;
  }
  return OBJC3_RUNTIME_DISPATCH_STATUS_OK;
}

bool IsSupportedRuntimeMethodShape(
    const char *return_type_name, std::uint64_t parameter_count) {
  return RuntimeMethodShapeStatus(return_type_name, parameter_count) ==
         OBJC3_RUNTIME_DISPATCH_STATUS_OK;
}

}  // namespace objc3c::runtime

#include "runtime/dispatch/method_lookup.h"

#include <string>

namespace objc3c::runtime {

RuntimeMethodReturnKind ClassifyRuntimeReturnType(
    const char *return_type_name) {
  if (return_type_name == nullptr) {
    return RuntimeMethodReturnKind::Unsupported;
  }
  const std::string type_name = return_type_name;
  if (type_name == "void") {
    return RuntimeMethodReturnKind::Void;
  }
  if (type_name == "bool" || type_name == "BOOL") {
    return RuntimeMethodReturnKind::Bool;
  }
  if (type_name == "i32" || type_name == "int" ||
      type_name == "NSInteger" || type_name == "NSUInteger" ||
      type_name == "id" || type_name == "instancetype" ||
      type_name == "Class" || type_name == "SEL" ||
      type_name == "Protocol") {
    return RuntimeMethodReturnKind::I32Like;
  }
  if (type_name.empty()) {
    return RuntimeMethodReturnKind::Unsupported;
  }
  return RuntimeMethodReturnKind::Unsupported;
}

objc3_runtime_dispatch_status_code RuntimeMethodShapeStatus(
    const char *return_type_name, std::uint64_t parameter_count) {
  if (parameter_count > 4) {
    return OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT;
  }
  if (ClassifyRuntimeReturnType(return_type_name) ==
      RuntimeMethodReturnKind::Unsupported) {
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

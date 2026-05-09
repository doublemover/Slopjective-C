#include "runtime/dispatch/method_invocation.h"

#include "runtime/dispatch/method_invocation_signatures.h"

namespace objc3c::runtime {

RuntimeTypedDispatchResult InvokeRuntimeMethodImplementation(
    const void *implementation, RuntimeMethodReturnKind return_kind,
    std::uint64_t parameter_count, int a0, int a1, int a2, int a3) {
  if (implementation == nullptr) {
    return RuntimeTypedDispatchFailure(
        OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA, return_kind);
  }
  switch (return_kind) {
    case RuntimeMethodReturnKind::Int32:
    case RuntimeMethodReturnKind::ObjectReference:
    case RuntimeMethodReturnKind::ClassReference:
    case RuntimeMethodReturnKind::SelectorReference:
    case RuntimeMethodReturnKind::ProtocolReference:
      return InvokeIntCompatibleRuntimeMethodSignature(
          implementation, return_kind, parameter_count, a0, a1, a2, a3);
    case RuntimeMethodReturnKind::Bool:
      return InvokeBoolRuntimeMethodSignature(
          implementation, return_kind, parameter_count, a0, a1, a2, a3);
    case RuntimeMethodReturnKind::Void:
      return InvokeVoidRuntimeMethodSignature(
          implementation, return_kind, parameter_count, a0, a1, a2, a3);
    case RuntimeMethodReturnKind::Unsupported:
      return RuntimeTypedDispatchFailure(
          OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE, return_kind);
  }
  return RuntimeTypedDispatchFailure(
      OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE, return_kind);
}

}  // namespace objc3c::runtime

#include "runtime/dispatch/method_invocation.h"

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
      switch (parameter_count) {
        case 0:
          return RuntimeTypedDispatchSuccess(
              return_kind,
              reinterpret_cast<int (*)()>(const_cast<void *>(implementation))());
        case 1:
          return RuntimeTypedDispatchSuccess(
              return_kind,
              reinterpret_cast<int (*)(int)>(
                  const_cast<void *>(implementation))(a0));
        case 2:
          return RuntimeTypedDispatchSuccess(
              return_kind,
              reinterpret_cast<int (*)(int, int)>(
                  const_cast<void *>(implementation))(a0, a1));
        case 3:
          return RuntimeTypedDispatchSuccess(
              return_kind,
              reinterpret_cast<int (*)(int, int, int)>(
                  const_cast<void *>(implementation))(a0, a1, a2));
        case 4:
          return RuntimeTypedDispatchSuccess(
              return_kind,
              reinterpret_cast<int (*)(int, int, int, int)>(
                  const_cast<void *>(implementation))(a0, a1, a2, a3));
        default:
          return RuntimeTypedDispatchFailure(
              OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT,
              return_kind);
      }
    case RuntimeMethodReturnKind::Bool:
      switch (parameter_count) {
        case 0:
          return RuntimeTypedDispatchSuccess(
              return_kind,
              reinterpret_cast<bool (*)()>(const_cast<void *>(implementation))()
                  ? 1
                  : 0);
        case 1:
          return RuntimeTypedDispatchSuccess(
              return_kind,
              reinterpret_cast<bool (*)(int)>(
                  const_cast<void *>(implementation))(a0)
                  ? 1
                  : 0);
        case 2:
          return RuntimeTypedDispatchSuccess(
              return_kind,
              reinterpret_cast<bool (*)(int, int)>(
                  const_cast<void *>(implementation))(a0, a1)
                  ? 1
                  : 0);
        case 3:
          return RuntimeTypedDispatchSuccess(
              return_kind,
              reinterpret_cast<bool (*)(int, int, int)>(
                  const_cast<void *>(implementation))(a0, a1, a2)
                  ? 1
                  : 0);
        case 4:
          return RuntimeTypedDispatchSuccess(
              return_kind,
              reinterpret_cast<bool (*)(int, int, int, int)>(
                  const_cast<void *>(implementation))(a0, a1, a2, a3)
                  ? 1
                  : 0);
        default:
          return RuntimeTypedDispatchFailure(
              OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT,
              return_kind);
      }
    case RuntimeMethodReturnKind::Void:
      switch (parameter_count) {
        case 0:
          reinterpret_cast<void (*)()>(const_cast<void *>(implementation))();
          return RuntimeTypedDispatchSuccess(return_kind, 0);
        case 1:
          reinterpret_cast<void (*)(int)>(
              const_cast<void *>(implementation))(a0);
          return RuntimeTypedDispatchSuccess(return_kind, 0);
        case 2:
          reinterpret_cast<void (*)(int, int)>(
              const_cast<void *>(implementation))(a0, a1);
          return RuntimeTypedDispatchSuccess(return_kind, 0);
        case 3:
          reinterpret_cast<void (*)(int, int, int)>(
              const_cast<void *>(implementation))(a0, a1, a2);
          return RuntimeTypedDispatchSuccess(return_kind, 0);
        case 4:
          reinterpret_cast<void (*)(int, int, int, int)>(
              const_cast<void *>(implementation))(a0, a1, a2, a3);
          return RuntimeTypedDispatchSuccess(return_kind, 0);
        default:
          return RuntimeTypedDispatchFailure(
              OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT,
              return_kind);
      }
    case RuntimeMethodReturnKind::Unsupported:
      return RuntimeTypedDispatchFailure(
          OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE, return_kind);
  }
  return RuntimeTypedDispatchFailure(
      OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_RETURN_TYPE, return_kind);
}

}  // namespace objc3c::runtime

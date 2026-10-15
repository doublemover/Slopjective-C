#include "runtime/dispatch/method_invocation_signatures.h"

namespace objc3c::runtime {

RuntimeTypedDispatchResult InvokeI32RuntimeMethodSignature(
    const void *implementation, RuntimeMethodReturnKind return_kind,
    std::uint64_t parameter_count, int a0, int a1, int a2, int a3,
    int *throws_error_out) {
  if (throws_error_out != nullptr) {
    switch (parameter_count) {
      case 0:
        return RuntimeTypedDispatchSuccess(
            return_kind,
            reinterpret_cast<int (*)(int *)>(
                const_cast<void *>(implementation))(throws_error_out));
      case 1:
        return RuntimeTypedDispatchSuccess(
            return_kind,
            reinterpret_cast<int (*)(int, int *)>(
                const_cast<void *>(implementation))(a0, throws_error_out));
      case 2:
        return RuntimeTypedDispatchSuccess(
            return_kind,
            reinterpret_cast<int (*)(int, int, int *)>(
                const_cast<void *>(implementation))(a0, a1, throws_error_out));
      case 3:
        return RuntimeTypedDispatchSuccess(
            return_kind,
            reinterpret_cast<int (*)(int, int, int, int *)>(
                const_cast<void *>(implementation))(a0, a1, a2,
                                                    throws_error_out));
      case 4:
        return RuntimeTypedDispatchSuccess(
            return_kind,
            reinterpret_cast<int (*)(int, int, int, int, int *)>(
                const_cast<void *>(implementation))(a0, a1, a2, a3,
                                                    throws_error_out));
      default:
        return RuntimeTypedDispatchFailure(
            OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT,
            return_kind);
    }
  }
  switch (parameter_count) {
    case 0:
      return RuntimeTypedDispatchSuccess(
          return_kind,
          reinterpret_cast<int (*)()>(const_cast<void *>(implementation))());
    case 1:
      return RuntimeTypedDispatchSuccess(
          return_kind,
          reinterpret_cast<int (*)(int)>(const_cast<void *>(implementation))(
              a0));
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
}

RuntimeTypedDispatchResult InvokeBoolRuntimeMethodSignature(
    const void *implementation, RuntimeMethodReturnKind return_kind,
    std::uint64_t parameter_count, int a0, int a1, int a2, int a3,
    int *throws_error_out) {
  if (throws_error_out != nullptr) {
    switch (parameter_count) {
      case 0:
        return RuntimeTypedDispatchSuccess(
            return_kind,
            reinterpret_cast<bool (*)(int *)>(
                const_cast<void *>(implementation))(throws_error_out)
                ? 1
                : 0);
      case 1:
        return RuntimeTypedDispatchSuccess(
            return_kind,
            reinterpret_cast<bool (*)(int, int *)>(
                const_cast<void *>(implementation))(a0, throws_error_out)
                ? 1
                : 0);
      case 2:
        return RuntimeTypedDispatchSuccess(
            return_kind,
            reinterpret_cast<bool (*)(int, int, int *)>(
                const_cast<void *>(implementation))(a0, a1, throws_error_out)
                ? 1
                : 0);
      case 3:
        return RuntimeTypedDispatchSuccess(
            return_kind,
            reinterpret_cast<bool (*)(int, int, int, int *)>(
                const_cast<void *>(implementation))(a0, a1, a2,
                                                    throws_error_out)
                ? 1
                : 0);
      case 4:
        return RuntimeTypedDispatchSuccess(
            return_kind,
            reinterpret_cast<bool (*)(int, int, int, int, int *)>(
                const_cast<void *>(implementation))(a0, a1, a2, a3,
                                                    throws_error_out)
                ? 1
                : 0);
      default:
        return RuntimeTypedDispatchFailure(
            OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT,
            return_kind);
    }
  }
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
          reinterpret_cast<bool (*)(int)>(const_cast<void *>(implementation))(
              a0)
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
}

RuntimeTypedDispatchResult InvokeVoidRuntimeMethodSignature(
    const void *implementation, RuntimeMethodReturnKind return_kind,
    std::uint64_t parameter_count, int a0, int a1, int a2, int a3,
    int *throws_error_out) {
  if (throws_error_out != nullptr) {
    switch (parameter_count) {
      case 0:
        reinterpret_cast<void (*)(int *)>(
            const_cast<void *>(implementation))(throws_error_out);
        return RuntimeTypedDispatchSuccess(return_kind, 0);
      case 1:
        reinterpret_cast<void (*)(int, int *)>(
            const_cast<void *>(implementation))(a0, throws_error_out);
        return RuntimeTypedDispatchSuccess(return_kind, 0);
      case 2:
        reinterpret_cast<void (*)(int, int, int *)>(
            const_cast<void *>(implementation))(a0, a1, throws_error_out);
        return RuntimeTypedDispatchSuccess(return_kind, 0);
      case 3:
        reinterpret_cast<void (*)(int, int, int, int *)>(
            const_cast<void *>(implementation))(a0, a1, a2, throws_error_out);
        return RuntimeTypedDispatchSuccess(return_kind, 0);
      case 4:
        reinterpret_cast<void (*)(int, int, int, int, int *)>(
            const_cast<void *>(implementation))(a0, a1, a2, a3,
                                               throws_error_out);
        return RuntimeTypedDispatchSuccess(return_kind, 0);
      default:
        return RuntimeTypedDispatchFailure(
            OBJC3_RUNTIME_DISPATCH_STATUS_UNSUPPORTED_ARGUMENT_LAYOUT,
            return_kind);
    }
  }
  switch (parameter_count) {
    case 0:
      reinterpret_cast<void (*)()>(const_cast<void *>(implementation))();
      return RuntimeTypedDispatchSuccess(return_kind, 0);
    case 1:
      reinterpret_cast<void (*)(int)>(const_cast<void *>(implementation))(a0);
      return RuntimeTypedDispatchSuccess(return_kind, 0);
    case 2:
      reinterpret_cast<void (*)(int, int)>(const_cast<void *>(implementation))(
          a0, a1);
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
}

}  // namespace objc3c::runtime

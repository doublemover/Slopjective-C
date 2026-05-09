#pragma once

#include <cstdint>

#include "runtime/public/objc3_runtime_result.h"

namespace objc3c::runtime {

enum class DispatchFamily {
  Invalid = 0,
  Instance = 1,
  Class = 2,
};

enum class RuntimeMethodReturnKind {
  Unsupported = 0,
  Int32 = 1,
  Bool = 2,
  Void = 3,
  ObjectReference = 4,
  ClassReference = 5,
  SelectorReference = 6,
  ProtocolReference = 7,
};

struct RuntimeTypedDispatchResult {
  objc3_runtime_dispatch_status_code status_code =
      OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR;
  RuntimeMethodReturnKind return_kind = RuntimeMethodReturnKind::Unsupported;
  int value = 0;
};

RuntimeMethodReturnKind ClassifyRuntimeReturnType(
    const char *return_type_name);
const char *RuntimeMethodReturnKindName(RuntimeMethodReturnKind return_kind);
bool RuntimeMethodReturnKindIsDispatchResultSupported(
    RuntimeMethodReturnKind return_kind);
objc3_runtime_dispatch_status_code RuntimeMethodShapeStatus(
    const char *return_type_name, std::uint64_t parameter_count);
bool IsSupportedRuntimeMethodShape(
    const char *return_type_name, std::uint64_t parameter_count);
RuntimeTypedDispatchResult RuntimeTypedDispatchSuccess(
    RuntimeMethodReturnKind return_kind, int value);
RuntimeTypedDispatchResult RuntimeTypedDispatchFailure(
    objc3_runtime_dispatch_status_code status_code,
    RuntimeMethodReturnKind return_kind);

}  // namespace objc3c::runtime

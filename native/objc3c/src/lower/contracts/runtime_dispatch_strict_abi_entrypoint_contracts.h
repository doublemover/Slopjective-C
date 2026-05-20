#pragma once

#include <cstddef>

// Strict dispatch entrypoint ABI constants own the fixed-slot canonical
// runtime dispatch symbol and argument slot limits.
inline constexpr std::size_t kObjc3RuntimeDispatchDefaultArgs = 4;
inline constexpr std::size_t kObjc3RuntimeDispatchMaxArgs = 16;
inline constexpr const char *kObjc3RuntimeDispatchSymbol =
    "objc3_runtime_dispatch_i32";
inline constexpr const char *kObjc3RuntimeTypedDispatchValueSymbol =
    "objc3_runtime_dispatch_typed_value";
inline constexpr const char *kObjc3RuntimeTypedDispatchValueFromClassSymbol =
    "objc3_runtime_dispatch_typed_value_from_class";
inline constexpr int kObjc3RuntimeDispatchReturnKindUnsupported = 0;
inline constexpr int kObjc3RuntimeDispatchReturnKindI32 = 1;
inline constexpr int kObjc3RuntimeDispatchReturnKindBool = 2;
inline constexpr int kObjc3RuntimeDispatchReturnKindVoid = 3;
inline constexpr int kObjc3RuntimeDispatchReturnKindObjectReference = 4;
inline constexpr int kObjc3RuntimeDispatchReturnKindClassReference = 5;
inline constexpr int kObjc3RuntimeDispatchReturnKindSelectorReference = 6;
inline constexpr int kObjc3RuntimeDispatchReturnKindProtocolReference = 7;

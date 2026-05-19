#pragma once

#include "lower/contracts/runtime_dispatch_strict_abi_entrypoint_contracts.h"

// Strict runtime dispatch lowering ABI constants own the canonical runtime
// entrypoint, selector handle ABI, and fail-closed error model.
inline constexpr const char *kObjc3RuntimeDispatchLoweringAbiContractId =
    "objc3c.runtime.dispatch.lowering.abi.freeze.v1";
inline constexpr const char *kObjc3RuntimeDispatchLoweringAbiBoundaryModel =
    "canonical-runtime-dispatch-default-target";
inline constexpr const char
    *kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol =
        "objc3_runtime_dispatch_i32";
inline constexpr const char *kObjc3RuntimeDispatchLoweringSelectorLookupSymbol =
    "objc3_runtime_lookup_selector";
inline constexpr const char *kObjc3RuntimeDispatchLoweringSelectorHandleType =
    "objc3_runtime_selector_handle";
inline constexpr const char *kObjc3RuntimeDispatchLoweringReceiverAbiType =
    "i32";
inline constexpr const char *kObjc3RuntimeDispatchLoweringSelectorAbiType =
    "ptr";
inline constexpr const char *kObjc3RuntimeDispatchLoweringArgumentAbiType =
    "i32";
inline constexpr const char *kObjc3RuntimeDispatchLoweringResultAbiType =
    "i32";
inline constexpr const char *kObjc3RuntimeDispatchLoweringSelectorOperandModel =
    "selector-cstring-pointer-remains-lowered-operand-until-next-runtime-phase";
inline constexpr const char *kObjc3RuntimeDispatchLoweringSelectorHandleModel =
    "runtime-lookup-produces-selector-handle-before-live-dispatch";
inline constexpr const char *kObjc3RuntimeDispatchLoweringArgumentPaddingModel =
    "zero-pad-to-fixed-runtime-arg-slot-count";
inline constexpr const char *kObjc3RuntimeDispatchLoweringDefaultTargetModel =
    "default-lowering-target-is-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchLoweringStrictDispatchErrorModel =
        "resolved-runtime-call-or-hard-diagnostic-error-only";
inline constexpr const char *kObjc3RuntimeDispatchLoweringDeferredCasesModel =
    "direct-dispatch-remains-fail-closed-after-live-cutover";

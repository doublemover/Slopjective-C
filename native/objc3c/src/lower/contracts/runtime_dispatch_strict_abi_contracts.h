#pragma once

#include "lower/contracts/lowering_ownership_contracts.h"

#include <cstddef>
#include <string>

// Strict runtime dispatch ABI contracts own the canonical runtime entrypoint,
// fixed-slot call ABI, fail-closed direct dispatch, and live call-generation
// cutover constants.
inline constexpr std::size_t kObjc3RuntimeDispatchDefaultArgs = 4;
inline constexpr std::size_t kObjc3RuntimeDispatchMaxArgs = 16;
inline constexpr const char *kObjc3RuntimeDispatchSymbol =
    "objc3_runtime_dispatch_i32";

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
inline constexpr const char *kObjc3RuntimeDispatchLoweringOwnerModel =
    kObjc3LoweringNoFallbackOwnerModel;

inline bool Objc3RuntimeDispatchLoweringOwnerIsReady() {
  return Objc3LoweringStrictOwnerModelIsReady(
      kObjc3RuntimeDispatchLoweringOwner,
      kObjc3RuntimeDispatchLoweringOwnerModel,
      true,
      true);
}

inline std::string Objc3RuntimeDispatchLoweringOwnerReplayKey() {
  return Objc3LoweringOwnerReplayKey(
      kObjc3RuntimeDispatchLoweringOwner,
      kObjc3RuntimeDispatchLoweringOwnerModel,
      true,
      true);
}

inline constexpr const char *kObjc3RuntimeDispatchCallAbiGenerationContractId =
    "objc3c.runtime.call.abi.instance.class.dispatch.v1";
inline constexpr const char
    *kObjc3RuntimeDispatchCallAbiGenerationActiveLoweringModel =
        "instance-class-super-and-dynamic-sends-lower-directly-to-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchCallAbiGenerationDeferredLoweringModel =
        "direct-dispatch-remains-fail-closed-until-supported-surface-materializes";
inline constexpr const char *kObjc3RuntimeDispatchSuperNilContractId =
    "objc3c.runtime.call.abi.super.nil.direct.dispatch.v1";
inline constexpr const char
    *kObjc3RuntimeDispatchSuperNilActiveLoweringModel =
        "instance-class-super-and-nil-sends-lower-directly-to-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchSuperNilDeferredLoweringModel =
        "direct-dispatch-remains-fail-closed-until-supported-surface-materializes";
inline constexpr const char
    *kObjc3RuntimeDispatchSuperNilUnsupportedModel =
        "direct-dispatch-fails-closed-until-supported-surface-materializes";
inline constexpr const char *kObjc3RuntimeDispatchLiveCutoverContractId =
    "objc3c.runtime.call.abi.live.dispatch.cutover.v1";
inline constexpr const char
    *kObjc3RuntimeDispatchLiveCutoverActiveLoweringModel =
        "all-supported-sends-lower-directly-to-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchLiveCutoverStrictDispatchModel =
        "resolved-runtime-call-or-hard-diagnostic-error-only";
inline constexpr const char
    *kObjc3RuntimeDispatchLiveCutoverDefaultTargetModel =
        "default-lowering-target-is-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3RuntimeDispatchLiveCutoverDeferredCasesModel =
        "direct-dispatch-remains-fail-closed-after-live-cutover";

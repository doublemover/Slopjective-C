#pragma once

// Strict runtime dispatch call ABI constants own live call-generation,
// super/nil, and fail-closed live cutover models.
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

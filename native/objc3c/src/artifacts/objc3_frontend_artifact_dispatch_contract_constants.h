#pragma once

namespace objc3::artifacts::frontend {

inline constexpr const char *kObjc3ArtifactRuntimeDispatchSymbol =
    "objc3_runtime_dispatch_i32";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringContractId =
    "objc3c.dispatch.dispatch.control.lowering.contract.v1";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_dispatch_dispatch_control_lowering_contract";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringModel =
    "dispatch-direct-call-candidates-final-sealed-boundaries-and-dynamism-intent-metadata-now-feed-one-deterministic-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringDeferredModel =
    "live-direct-call-selector-bypass-runtime-dispatch-boundary-realization-and-runnable-metadata-consumption-remain-later-dispatch-control-runtime-work";
inline constexpr const char *kObjc3DispatchDispatchControlLoweringLaneContract =
    "objc3c.dispatch.dispatch.control.lowering.contract.v1";
inline constexpr const char
    *kObjc3ArtifactRuntimeDispatchLiveCutoverStrictDispatchModel =
        "resolved-runtime-call-or-hard-diagnostic-error-only";
inline constexpr const char
    *kObjc3ArtifactRuntimeDispatchLiveCutoverDefaultTargetModel =
        "default-lowering-target-is-canonical-runtime-entrypoint";
inline constexpr const char
    *kObjc3ArtifactRuntimeDispatchLiveCutoverDeferredCasesModel =
        "direct-dispatch-remains-fail-closed-after-live-cutover";

}  // namespace objc3::artifacts::frontend

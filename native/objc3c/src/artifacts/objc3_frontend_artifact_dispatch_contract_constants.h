#pragma once

namespace objc3::artifacts::frontend {

inline constexpr const char *kObjc3ArtifactRuntimeDispatchSymbol =
    "objc3_runtime_dispatch_i32";
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

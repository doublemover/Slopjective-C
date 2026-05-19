#pragma once

// Block ABI lowering lane constants own the stable replay identifiers for
// invoke trampoline layout, storage escape, copy-dispose helpers, deterministic
// baseline accounting, and executable block ABI helper boundaries.
inline constexpr const char *kObjc3BlockAbiInvokeTrampolineLoweringLaneContract =
    "objc3c.block.abi.invoke.trampoline.lowering.v1";
inline constexpr const char *kObjc3BlockStorageEscapeLoweringLaneContract =
    "objc3c.block.storage.escape.lowering.v1";
inline constexpr const char *kObjc3BlockCopyDisposeLoweringLaneContract =
    "objc3c.block.copy.dispose.lowering.v1";
inline constexpr const char *kObjc3BlockDeterminismPerfBaselineLoweringLaneContract =
    "objc3c.block.determinism.perf.baseline.lowering.v1";
inline constexpr const char
    *kObjc3ExecutableBlockLoweringAbiArtifactBoundaryLaneContract =
        "objc3c.block.lowering.abi.artifact.boundary.v1";
inline constexpr const char *kObjc3ExecutableBlockObjectInvokeThunkLoweringLaneContract =
    "objc3c.block.object.invoke.thunk.lowering.v1";
inline constexpr const char *kObjc3ExecutableBlockByrefHelperLoweringLaneContract =
    "objc3c.block.byref.helper.lowering.v1";
inline constexpr const char *kObjc3ExecutableBlockEscapeRuntimeHookLoweringLaneContract =
    "objc3c.block.escape.runtime.hook.lowering.v1";

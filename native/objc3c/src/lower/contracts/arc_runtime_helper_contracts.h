#pragma once

#include <string>

// ARC runtime helper contracts own the private helper ABI, runtime execution
// proof, debug instrumentation, and runnable ARC gate/closeout surfaces.
inline constexpr const char *kObjc3RuntimeArcHelperApiSurfaceContractId =
    "objc3c.runtime.arc.helper.api.surface.freeze.v1";
inline constexpr const char *kObjc3RuntimeArcHelperApiSurfaceReferenceModel =
    "public-runtime-abi-stays-register-lookup-dispatch-while-arc-helper-entrypoints-remain-private-bootstrap-internal-runtime-abi";
inline constexpr const char *kObjc3RuntimeArcHelperApiSurfaceWeakModel =
    "weak-storage-and-current-property-access-remain-served-through-private-runtime-helper-entrypoints-and-runtime-side-tables";
inline constexpr const char *kObjc3RuntimeArcHelperApiSurfaceAutoreleasepoolModel =
    "autorelease-return-and-autoreleasepool-support-remain-private-runtime-helper-behavior-without-public-abi-widening";
inline constexpr const char *kObjc3RuntimeArcHelperApiSurfaceFailClosedModel =
    "no-public-runtime-arc-helper-api-no-user-facing-arc-runtime-header-widening-yet";

inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportContractId =
    "objc3c.runtime.arc.helper.runtime.support.v1";
inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportDependencyModel =
    "runtime-baseline-plus-runnable-arc-lowering-plus-private-helper-surface";
inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportWeakModel =
    "arc-generated-weak-current-property-access-lowers-and-links-through-private-runtime-helper-entrypoints";
inline constexpr const char
    *kObjc3RuntimeArcHelperRuntimeSupportAutoreleaseReturnModel =
        "arc-generated-autorelease-return-paths-link-and-execute-through-private-runtime-helper-entrypoints";
inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportExecutionModel =
    "runtime-library-backed-helper-entrypoints-remain-private-but-executable-through-linked-native-arc-programs";
inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportFailClosedModel =
    "unsupported-arc-runtime-surfaces-stay-private-fixture-proven-and-fail-closed-outside-the-supported-slice";

inline constexpr const char *kObjc3RuntimeArcDebugInstrumentationContractId =
    "objc3c.runtime.arc.debug.instrumentation.v1";
inline constexpr const char *kObjc3RuntimeArcDebugInstrumentationDependencyModel =
    "live-helper-runtime-plus-private-bootstrap-internal-debug-snapshots";
inline constexpr const char *kObjc3RuntimeArcDebugInstrumentationCoverageModel =
    "retain-release-autorelease-weak-current-property-and-autoreleasepool-helper-traffic-publishes-deterministic-debug-counters-and-last-value-context";
inline constexpr const char *kObjc3RuntimeArcDebugInstrumentationValidationModel =
    "runtime-probes-and-targeted-arc-fixtures-consume-private-debug-snapshots-without-widening-the-public-runtime-abi";
inline constexpr const char *kObjc3RuntimeArcDebugInstrumentationFailClosedModel =
    "arc-debug-hooks-remain-private-testing-surface-only-and-must-not-claim-broader-runtime-completeness";

inline constexpr const char *kObjc3RunnableArcRuntimeGateContractId =
    "objc3c.runnable.arc.runtime.gate.v1";
inline constexpr const char *kObjc3RunnableArcRuntimeGateEvidenceModel =
    "source-sema-lowering-runtime-summary-chain";
inline constexpr const char *kObjc3RunnableArcRuntimeGateActiveModel =
    "runnable-arc-gate-consumes-arc-mode-semantics-lowering-and-runtime-proofs-rather-than-parser-only-or-metadata-only-claims";
inline constexpr const char *kObjc3RunnableArcRuntimeGateNonGoalModel =
    "no-runnable-arc-closeout-matrix-no-public-runtime-abi-widening-no-cross-module-arc-claims-before-the-runnable-arc-closeout";
inline constexpr const char *kObjc3RunnableArcRuntimeGateFailClosedModel =
    "fail-closed-on-runnable-arc-runtime-evidence-drift";

inline constexpr const char *kObjc3RunnableArcCloseoutContractId =
    "objc3c.runnable.arc.closeout.v1";
inline constexpr const char *kObjc3RunnableArcCloseoutMatrixModel =
    "closeout-matrix-consumes-lowering-runtime-and-integrated-evidence-without-widening-the-supported-runnable-arc-slice";
inline constexpr const char *kObjc3RunnableArcCloseoutSmokeModel =
    "integrated-arc-fixtures-and-private-property-runtime-probes-prove-supported-cleanup-block-and-property-behavior-through-native-toolchain-and-runtime";
inline constexpr const char *kObjc3RunnableArcCloseoutFailClosedModel =
    "fail-closed-on-runnable-arc-closeout-drift-or-runbook-mismatch";

std::string Objc3RuntimeArcHelperApiSurfaceSummary();
std::string Objc3RuntimeArcHelperRuntimeSupportSummary();
std::string Objc3RuntimeArcDebugInstrumentationSummary();
std::string Objc3RunnableArcRuntimeGateSummary();
std::string Objc3RunnableArcCloseoutSummary();

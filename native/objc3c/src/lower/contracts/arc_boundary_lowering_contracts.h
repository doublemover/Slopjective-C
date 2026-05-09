#pragma once

#include <string>

// ARC boundary lowering owns mode admission, semantic inference, helper-backed
// runtime lowering, and runnable ARC gate/closeout contract surfaces.
inline constexpr const char *kObjc3ArcSourceModeBoundaryContractId =
    "objc3c.arc.source.mode.boundary.freeze.v1";
inline constexpr const char *kObjc3ArcSourceModeBoundarySourceModel =
    "ownership-qualifier-weak-unowned-autoreleasepool-and-arc-fixit-source-surfaces-remain-live-without-enabling-runnable-arc-mode";
inline constexpr const char *kObjc3ArcSourceModeBoundaryModeModel =
    "native-driver-admits-fobjc-arc-and-fno-objc-arc-while-runnable-arc-stays-bounded-to-the-helper-backed-supported-slice";
inline constexpr const char *kObjc3ArcSourceModeBoundaryNonGoalModel =
    "no-generalized-arc-cleanup-insertion-no-public-arc-runtime-abi-mode-split-no-full-arc-automation-beyond-the-supported-helper-backed-slice";
inline constexpr const char *kObjc3ArcSourceModeBoundaryFailClosedModel =
    "fail-closed-on-arc-source-mode-boundary-drift-before-arc-automation";

inline constexpr const char *kObjc3ArcModeHandlingContractId =
    "objc3c.arc.mode.handling.v1";
inline constexpr const char *kObjc3ArcModeHandlingSourceModel =
    "ownership-qualified-method-property-return-and-block-capture-surfaces-are-runnable-under-explicit-arc-mode";
inline constexpr const char *kObjc3ArcModeHandlingModeModel =
    "driver-admits-fobjc-arc-and-fno-objc-arc-and-threads-arc-mode-through-frontend-sema-and-ir";
inline constexpr const char *kObjc3ArcModeHandlingFailClosedModel =
    "non-arc-mode-still-rejects-executable-ownership-qualified-method-and-function-signatures";
inline constexpr const char *kObjc3ArcModeHandlingNonGoalModel =
    "no-implicit-nonarc-promotion-no-cross-module-arc-mode-inference-no-full-arc-automation-beyond-the-supported-helper-backed-slice";

inline constexpr const char *kObjc3ArcSemanticRulesContractId =
    "objc3c.arc.semantic.rules.v1";
inline constexpr const char *kObjc3ArcSemanticRulesSourceModel =
    "explicit-arc-mode-admits-only-explicit-ownership-surfaces-while-forbidden-property-forms-and-broad-inference-remain-fail-closed";
inline constexpr const char *kObjc3ArcSemanticRulesSemanticModel =
    "conflicting-property-ownership-forms-and-atomic-ownership-aware-storage-still-fail-closed-while-general-arc-inference-remains-deferred";
inline constexpr const char *kObjc3ArcSemanticRulesFailClosedModel =
    "forbidden-arc-property-forms-and-non-inferred-lifetime-semantics-terminate-deterministically";
inline constexpr const char *kObjc3ArcSemanticRulesNonGoalModel =
    "no-implicit-retain-release-inference-no-lifetime-extension-no-method-family-based-arc-semantics-yet";

inline constexpr const char *kObjc3ArcInferenceLifetimeContractId =
    "objc3c.arc.inference.lifetime.v1";
inline constexpr const char *kObjc3ArcInferenceLifetimeSourceModel =
    "explicit-arc-mode-now-infers-strong-owned-executable-object-signatures-for-the-supported-runnable-slice";
inline constexpr const char *kObjc3ArcInferenceLifetimeSemanticModel =
    "arc-enabled-unqualified-object-signatures-now-produce-canonical-retain-release-lifetime-accounting-while-nonarc-remains-zero-inference";
inline constexpr const char *kObjc3ArcInferenceLifetimeFailClosedModel =
    "non-arc-mode-keeps-unqualified-object-signatures-non-inferred-and-zero-retain-release-lifetime-accounting";
inline constexpr const char *kObjc3ArcInferenceLifetimeNonGoalModel =
    "no-full-arc-cleanup-synthesis-no-weak-autorelease-return-property-synthesis-or-block-interaction-arc-semantics-yet";

inline constexpr const char *kObjc3ArcInteractionSemanticsContractId =
    "objc3c.arc.interaction.semantics.v1";
inline constexpr const char *kObjc3ArcInteractionSemanticsSourceModel =
    "explicit-arc-mode-now-covers-weak-autorelease-return-property-synthesis-and-block-ownership-interactions-for-the-supported-runnable-slice";
inline constexpr const char *kObjc3ArcInteractionSemanticsSemanticModel =
    "weak-properties-and-nonowning-captures-stay-nonretaining-autorelease-returns-stay-profiled-and-synthesized-property-accessors-publish-owned-lifetime-packets-under-arc";
inline constexpr const char *kObjc3ArcInteractionSemanticsFailClosedModel =
    "unsupported-arc-cleanup-and-broader-interactions-still-remain-explicitly-deferred";
inline constexpr const char *kObjc3ArcInteractionSemanticsNonGoalModel =
    "no-general-arc-cleanup-insertion-no-cross-module-arc-interop-no-full-method-family-automation-yet";

inline constexpr const char *kObjc3ArcLoweringAbiCleanupModelContractId =
    "objc3c.arc.lowering.abi.cleanup.model.v1";
inline constexpr const char *kObjc3ArcLoweringAbiCleanupModelSourceModel =
    "arc-inference-and-interaction-semantic-packets-feed-one-lane-c-helper-call-and-cleanup-boundary";
inline constexpr const char *kObjc3ArcLoweringAbiCleanupModelAbiModel =
    "private-runtime-helper-call-boundary-over-retain-release-autorelease-weak-property-and-block-helpers";
inline constexpr const char *kObjc3ArcLoweringAbiCleanupModelCleanupModel =
    "helper-call-plus-autoreleasepool-scope-lowering-without-general-cleanup-stack-or-return-slot-optimization";
inline constexpr const char *kObjc3ArcLoweringAbiCleanupModelFailClosedModel =
    "unsupported-ownership-qualified-signatures-and-generalized-arc-cleanups-remain-fail-closed";
inline constexpr const char *kObjc3ArcLoweringAbiCleanupModelNonGoalModel =
    "no-full-arc-automation-no-exception-cleanup-widening-no-objc-runtime-abi-parity-claim";

inline constexpr const char *kObjc3ArcAutomaticInsertionContractId =
    "objc3c.arc.automatic.insertion.v1";
inline constexpr const char *kObjc3ArcAutomaticInsertionSourceModel =
    "lane-c-consumes-arc-semantic-insertion-flags-for-supported-function-and-method-param-return-lowering";
inline constexpr const char *kObjc3ArcAutomaticInsertionLoweringModel =
    "owned-params-retain-on-entry-release-on-exit-and-autoreleasing-returns-lower-through-private-runtime-helpers";
inline constexpr const char *kObjc3ArcAutomaticInsertionFailureModel =
    "only-supported-runnable-arc-param-return-insertion-paths-materialize-automatic-helper-calls";
inline constexpr const char *kObjc3ArcAutomaticInsertionNonGoalModel =
    "no-general-local-lifetime-inference-no-full-cleanup-stack-no-cross-module-arc-optimization";

inline constexpr const char *kObjc3ArcCleanupWeakLifetimeHooksContractId =
    "objc3c.arc.cleanup.weak.lifetime.hooks.v1";
inline constexpr const char *kObjc3ArcCleanupWeakLifetimeHooksSourceModel =
    "lane-c-extends-the-supported-arc-lowering-slice-with-scope-exit-cleanups-weak-current-property-hooks-and-block-capture-lifetime-cleanup";
inline constexpr const char *kObjc3ArcCleanupWeakLifetimeHooksLoweringModel =
    "scope-exit-and-implicit-exit-cleanups-unwind-pending-block-dispose-and-arc-owned-storage-while-weak-current-property-access-stays-runtime-hooked";
inline constexpr const char *kObjc3ArcCleanupWeakLifetimeHooksFailureModel =
    "only-supported-scope-cleanup-weak-current-property-and-captured-lifetime-paths-materialize-runtime-lowering";
inline constexpr const char *kObjc3ArcCleanupWeakLifetimeHooksNonGoalModel =
    "no-general-weak-local-storage-lowering-no-exception-cleanup-stack-no-cross-module-arc-optimization";

inline constexpr const char *kObjc3ArcBlockAutoreleaseReturnLoweringContractId =
    "objc3c.arc.block.autorelease.return.lowering.v1";
inline constexpr const char *kObjc3ArcBlockAutoreleaseReturnLoweringSourceModel =
    "lane-c-extends-the-supported-arc-lowering-slice-with-escaping-block-owned-capture-cleanup-and-autorelease-return-conventions";
inline constexpr const char *kObjc3ArcBlockAutoreleaseReturnLoweringModel =
    "escaping-block-promotion-and-terminal-branch-cleanup-compose-with-autoreleasing-returns-without-dropping-live-owned-storage-cleanup";
inline constexpr const char *kObjc3ArcBlockAutoreleaseReturnLoweringFailureModel =
    "only-supported-escaping-block-owned-capture-and-autorelease-return-edge-cases-materialize-runtime-lowering";
inline constexpr const char *kObjc3ArcBlockAutoreleaseReturnLoweringNonGoalModel =
    "no-general-method-family-arc-automation-no-public-runtime-arc-abi-no-cross-module-arc-optimization";

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
inline constexpr const char *kObjc3RuntimeArcHelperRuntimeSupportAutoreleaseReturnModel =
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

std::string Objc3ArcSourceModeBoundarySummary();
std::string Objc3ArcModeHandlingSummary(bool arc_mode_enabled);
std::string Objc3ArcSemanticRulesSummary();
std::string Objc3ArcInferenceLifetimeSummary();
std::string Objc3ArcInteractionSemanticsSummary();
std::string Objc3ArcLoweringAbiCleanupModelSummary();
std::string Objc3ArcAutomaticInsertionSummary();
std::string Objc3ArcCleanupWeakLifetimeHooksSummary();
std::string Objc3ArcBlockAutoreleaseReturnLoweringSummary();
std::string Objc3RuntimeArcHelperApiSurfaceSummary();
std::string Objc3RuntimeArcHelperRuntimeSupportSummary();
std::string Objc3RuntimeArcDebugInstrumentationSummary();
std::string Objc3RunnableArcRuntimeGateSummary();
std::string Objc3RunnableArcCloseoutSummary();

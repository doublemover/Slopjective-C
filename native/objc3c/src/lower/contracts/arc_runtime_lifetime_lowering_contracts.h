#pragma once

#include <string>

// ARC runtime lifetime lowering owns the helper-call ABI cleanup model,
// retain/release/autorelease insertion, weak lifetime hooks, and block
// autorelease return handoff for the supported runnable ARC slice.
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
    "no-public-runtime-arc-abi-no-cross-module-arc-optimization-no-method-family-automation-beyond-retained-message-result-cleanup";

std::string Objc3ArcLoweringAbiCleanupModelSummary();
std::string Objc3ArcAutomaticInsertionSummary();
std::string Objc3ArcCleanupWeakLifetimeHooksSummary();
std::string Objc3ArcBlockAutoreleaseReturnLoweringSummary();

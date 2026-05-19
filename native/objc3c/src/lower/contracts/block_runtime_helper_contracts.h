#pragma once

#include <string>

// Block runtime helper contracts own private block helper symbols, runtime
// layout/copy-dispose/byref support, runnable block gates, and runtime-backed
// closure evidence.
inline constexpr const char *kObjc3RuntimePromoteBlockI32Symbol =
    "objc3_runtime_promote_block_i32";
inline constexpr const char *kObjc3RuntimeInvokeBlockI32Symbol =
    "objc3_runtime_invoke_block_i32";

inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureContractId =
    "objc3c.runtime.backed.semantics.closure.v1";
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureSurfacePath =
    "reports/claimability/runtime-backed-semantics-closure";
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureEvidenceModel =
    "durable-source-sema-lowering-ir-runtime-fixture-report-chain";
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureBlockModel =
    "escaping-block-promotion-byref-forwarding-copy-dispose-and-captured-lifetime-lowering-route-through-private-block-runtime-helpers";
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureArcModel =
    "arc-retain-release-autorelease-weak-autoreleasepool-and-cleanup-ordering-route-through-private-runtime-helpers";
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureErrorModel =
    "throws-error-out-status-nserror-bridge-catch-dispatch-and-cleanup-paths-route-through-private-error-runtime-helpers";
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureConcurrencyModel =
    "async-continuation-task-group-cancellation-executor-hop-actor-isolation-mailbox-replay-and-race-guard-paths-route-through-private-concurrency-runtime-helpers";
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureRuntimeHelperModel =
    "private-helper-symbol-set-is-the-claimed-runtime-backed-boundary-and-public-runtime-abi-remains-unwidened";
inline constexpr const char *kObjc3RuntimeBackedSemanticsClosureFailClosedModel =
    "missing-durable-evidence-drifted-helper-symbols-unsupported-escape-throw-task-or-actor-surfaces-stay-fail-closed";

inline constexpr const char *kObjc3RuntimeBlockApiObjectLayoutContractId =
    "objc3c.runtime.block.api.object.layout.freeze.v1";
inline constexpr const char
    *kObjc3RuntimeBlockAllocationCopyDisposeInvokeSupportContractId =
        "objc3c.runtime.block.allocation.copy.dispose.invoke.support.v1";
inline constexpr const char
    *kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId =
        "objc3c.runtime.block.byref.forwarding.heap.promotion.interop.v1";

inline constexpr const char *kObjc3RunnableBlockRuntimeGateContractId =
    "objc3c.runnable.block.runtime.gate.v1";
inline constexpr const char *kObjc3RunnableBlockRuntimeGateEvidenceModel =
    "source-sema-lowering-runtime-summary-chain";
inline constexpr const char *kObjc3RunnableBlockRuntimeGateActiveModel =
    "runnable-block-gate-consumes-source-sema-lowering-and-runtime-proofs-rather-than-metadata-only-summaries";
inline constexpr const char *kObjc3RunnableBlockRuntimeGateNonGoalModel =
    "no-public-block-object-abi-no-public-runtime-helper-header-no-generalized-foreign-block-interop-no-caller-frame-forwarding-bridge";
inline constexpr const char *kObjc3RunnableBlockRuntimeGateFailClosedModel =
    "fail-closed-on-runnable-block-runtime-evidence-drift";

inline constexpr const char *kObjc3RunnableBlockExecutionMatrixContractId =
    "objc3c.runnable.block.execution.matrix.v1";
inline constexpr const char *kObjc3RunnableBlockExecutionMatrixEvidenceModel =
    "source-sema-lowering-runtime-integrated-native-block-smoke-matrix";
inline constexpr const char *kObjc3RunnableBlockExecutionMatrixActiveModel =
    "closeout-matrix-runs-owned-nonowning-byref-and-escaping-block-fixtures-against-the-native-runtime";
inline constexpr const char *kObjc3RunnableBlockExecutionMatrixNonGoalModel =
    "no-public-block-object-abi-no-public-runtime-helper-header-no-generalized-foreign-block-interop-no-caller-frame-forwarding-bridge";
inline constexpr const char *kObjc3RunnableBlockExecutionMatrixFailClosedModel =
    "fail-closed-on-runnable-block-execution-matrix-drift-or-doc-mismatch";

std::string Objc3RuntimeBlockApiObjectLayoutSummary();
std::string Objc3RuntimeBlockAllocationCopyDisposeInvokeSupportSummary();
std::string Objc3RuntimeBlockByrefForwardingHeapPromotionInteropSummary();
std::string Objc3RunnableBlockRuntimeGateSummary();
std::string Objc3RunnableBlockExecutionMatrixSummary();
std::string Objc3RuntimeBackedSemanticsClosureSummary();

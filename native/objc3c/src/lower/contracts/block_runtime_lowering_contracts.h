#pragma once

#include <cstddef>
#include <string>

// Block runtime lowering owns executable block semantics from source capture
// records through private runtime helper integration and closure evidence.
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

inline constexpr const char *kObjc3BlockLiteralCaptureLoweringLaneContract =
    "objc3c.block.literal.capture.lowering.v1";
inline constexpr const char *kObjc3BlockAbiInvokeTrampolineLoweringLaneContract =
    "objc3c.block.abi.invoke.trampoline.lowering.v1";
inline constexpr const char *kObjc3BlockStorageEscapeLoweringLaneContract =
    "objc3c.block.storage.escape.lowering.v1";
inline constexpr const char *kObjc3BlockCopyDisposeLoweringLaneContract =
    "objc3c.block.copy.dispose.lowering.v1";
inline constexpr const char *kObjc3BlockDeterminismPerfBaselineLoweringLaneContract =
    "objc3c.block.determinism.perf.baseline.lowering.v1";
inline constexpr const char *kObjc3BlockSourceModelCompletionLaneContract =
    "objc3c.block.source.model.v1";
inline constexpr const char *kObjc3BlockSourceStorageAnnotationLaneContract =
    "objc3c.block.source.storage.annotations.v1";
inline constexpr const char *kObjc3BlockRuntimeSemanticRulesLaneContract =
    "objc3c.block.runtime.semantic.rules.v1";
inline constexpr const char
    *kObjc3ExecutableBlockLoweringAbiArtifactBoundaryLaneContract =
        "objc3c.block.lowering.abi.artifact.boundary.v1";
inline constexpr const char *kObjc3ExecutableBlockObjectInvokeThunkLoweringLaneContract =
    "objc3c.block.object.invoke.thunk.lowering.v1";
inline constexpr const char *kObjc3ExecutableBlockByrefHelperLoweringLaneContract =
    "objc3c.block.byref.helper.lowering.v1";
inline constexpr const char *kObjc3ExecutableBlockEscapeRuntimeHookLoweringLaneContract =
    "objc3c.block.escape.runtime.hook.lowering.v1";

struct Objc3BlockLiteralCaptureLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t block_parameter_entries = 0;
  std::size_t block_capture_entries = 0;
  std::size_t block_body_statement_entries = 0;
  std::size_t block_empty_capture_sites = 0;
  std::size_t block_nondeterministic_capture_sites = 0;
  std::size_t block_non_normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockSourceModelCompletionContract {
  std::size_t block_literal_sites = 0;
  std::size_t signature_entries_total = 0;
  std::size_t explicit_typed_parameter_entries_total = 0;
  std::size_t implicit_parameter_entries_total = 0;
  std::size_t capture_inventory_entries_total = 0;
  std::size_t byvalue_readonly_capture_entries_total = 0;
  std::size_t invoke_surface_entries_total = 0;
  std::size_t non_normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockSourceStorageAnnotationContract {
  std::size_t block_literal_sites = 0;
  std::size_t capture_entries_total = 0;
  std::size_t mutated_capture_entries_total = 0;
  std::size_t byref_capture_entries_total = 0;
  std::size_t copy_helper_intent_sites = 0;
  std::size_t dispose_helper_intent_sites = 0;
  std::size_t heap_candidate_sites = 0;
  std::size_t expression_sites = 0;
  std::size_t global_initializer_sites = 0;
  std::size_t binding_initializer_sites = 0;
  std::size_t assignment_value_sites = 0;
  std::size_t return_value_sites = 0;
  std::size_t call_argument_sites = 0;
  std::size_t message_argument_sites = 0;
  std::size_t non_normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockAbiInvokeTrampolineLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t invoke_argument_slots_total = 0;
  std::size_t capture_word_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t descriptor_symbolized_sites = 0;
  std::size_t invoke_trampoline_symbolized_sites = 0;
  std::size_t missing_invoke_trampoline_sites = 0;
  std::size_t non_normalized_layout_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockStorageEscapeLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t mutable_capture_count_total = 0;
  std::size_t byref_slot_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t requires_byref_cells_sites = 0;
  std::size_t escape_analysis_enabled_sites = 0;
  std::size_t escape_to_heap_sites = 0;
  std::size_t escape_profile_normalized_sites = 0;
  std::size_t byref_layout_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockCopyDisposeLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t mutable_capture_count_total = 0;
  std::size_t byref_slot_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t copy_helper_required_sites = 0;
  std::size_t dispose_helper_required_sites = 0;
  std::size_t profile_normalized_sites = 0;
  std::size_t copy_helper_symbolized_sites = 0;
  std::size_t dispose_helper_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockDeterminismPerfBaselineLoweringContract {
  std::size_t block_literal_sites = 0;
  std::size_t baseline_weight_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t deterministic_capture_sites = 0;
  std::size_t heavy_tier_sites = 0;
  std::size_t normalized_profile_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

std::string Objc3ExecutableBlockSourceClosureSummary();
std::string Objc3ExecutableBlockSourceModelCompletionSummary();
std::string Objc3ExecutableBlockSourceStorageAnnotationSummary();
std::string Objc3ExecutableBlockRuntimeSemanticRulesSummary();
std::string Objc3ExecutableBlockLoweringAbiArtifactBoundarySummary();
std::string Objc3ExecutableBlockObjectInvokeThunkLoweringSummary();
std::string Objc3ExecutableBlockByrefHelperLoweringSummary();
std::string Objc3ExecutableBlockEscapeRuntimeHookLoweringSummary();
std::string Objc3RuntimeBlockApiObjectLayoutSummary();
std::string Objc3RuntimeBlockAllocationCopyDisposeInvokeSupportSummary();
std::string Objc3RuntimeBlockByrefForwardingHeapPromotionInteropSummary();
std::string Objc3RunnableBlockRuntimeGateSummary();
std::string Objc3RunnableBlockExecutionMatrixSummary();
std::string Objc3RuntimeBackedSemanticsClosureSummary();

bool IsValidObjc3BlockSourceModelCompletionContract(
    const Objc3BlockSourceModelCompletionContract &contract);
std::string Objc3BlockSourceModelCompletionReplayKey(
    const Objc3BlockSourceModelCompletionContract &contract);
bool IsValidObjc3BlockSourceStorageAnnotationContract(
    const Objc3BlockSourceStorageAnnotationContract &contract);
std::string Objc3BlockSourceStorageAnnotationReplayKey(
    const Objc3BlockSourceStorageAnnotationContract &contract);
bool IsValidObjc3BlockLiteralCaptureLoweringContract(
    const Objc3BlockLiteralCaptureLoweringContract &contract);
std::string Objc3BlockLiteralCaptureLoweringReplayKey(
    const Objc3BlockLiteralCaptureLoweringContract &contract);
bool IsValidObjc3BlockAbiInvokeTrampolineLoweringContract(
    const Objc3BlockAbiInvokeTrampolineLoweringContract &contract);
std::string Objc3BlockAbiInvokeTrampolineLoweringReplayKey(
    const Objc3BlockAbiInvokeTrampolineLoweringContract &contract);
bool IsValidObjc3BlockStorageEscapeLoweringContract(
    const Objc3BlockStorageEscapeLoweringContract &contract);
std::string Objc3BlockStorageEscapeLoweringReplayKey(
    const Objc3BlockStorageEscapeLoweringContract &contract);
bool IsValidObjc3BlockCopyDisposeLoweringContract(
    const Objc3BlockCopyDisposeLoweringContract &contract);
std::string Objc3BlockCopyDisposeLoweringReplayKey(
    const Objc3BlockCopyDisposeLoweringContract &contract);
bool IsValidObjc3BlockDeterminismPerfBaselineLoweringContract(
    const Objc3BlockDeterminismPerfBaselineLoweringContract &contract);
std::string Objc3BlockDeterminismPerfBaselineLoweringReplayKey(
    const Objc3BlockDeterminismPerfBaselineLoweringContract &contract);

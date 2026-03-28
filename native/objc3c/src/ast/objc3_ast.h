#pragma once

#include <memory>
#include <string>
#include <vector>

#include "token/objc3_token_contract.h"

struct SymbolRow {
  std::string kind;
  std::string name;
  unsigned line;
  unsigned column;
};

struct SymbolContext {
  std::vector<SymbolRow> rows;
};

enum class ValueType {
  Unknown,
  I32,
  Bool,
  Void,
  Function,
  ObjCId,
  ObjCClass,
  ObjCSel,
  ObjCProtocol,
  ObjCInstancetype,
  ObjCObjectPtr
};

struct Stmt;

struct Expr {
  // executable-block-source-closure freeze constants: lane-A now
  // freezes the truthful parser/AST boundary for block literals, captures,
  // and replay-stable profiling before runnable block realization begins.
  static inline constexpr const char *kObjc3ExecutableBlockSourceClosureContractId =
      "objc3c.executable.block.source.closure.v1";
  static inline constexpr const char *kObjc3ExecutableBlockSourceSurfaceModel =
      "parser-owned-block-literal-source-closure-freezes-capture-abi-storage-copy-dispose-and-baseline-profiles-before-runnable-block-realization";
  static inline constexpr const char *kObjc3ExecutableBlockSourceEvidenceModel =
      "hello-ir-boundary-plus-block-literal-o3s221-fail-closed-native-probe";
  static inline constexpr const char *kObjc3ExecutableBlockSourceFailureModel =
      "fail-closed-on-block-source-surface-drift-before-block-runtime-realization";
  static inline constexpr const char *kObjc3ExecutableBlockSourceNonGoalModel =
      "no-block-pointer-declarator-spellings-no-explicit-byref-storage-spellings-no-block-runtime-lowering";
  static inline constexpr const char *kObjc3ExecutableBlockSourceModelCompletionContractId =
      "objc3c.executable.block.source.model.completion.v1";
  static inline constexpr const char *kObjc3ExecutableBlockSignatureModel =
      "block-source-model-publishes-deterministic-parameter-signature-entries-before-runnable-block-realization";
  static inline constexpr const char *kObjc3ExecutableBlockCaptureInventoryModel =
      "block-source-model-publishes-deterministic-capture-storage-inventory-before-byref-and-helper-lowering";
  static inline constexpr const char *kObjc3ExecutableBlockInvokeSurfaceModel =
      "block-source-model-publishes-deterministic-descriptor-and-invoke-surface-symbols-before-runnable-block-lowering";
  static inline constexpr const char *kObjc3ExecutableBlockSourceModelEvidenceModel =
      "source-only-runner-manifest-success-plus-native-fail-closed-block-probe";
  static inline constexpr const char *kObjc3ExecutableBlockSourceModelFailureModel =
      "fail-closed-on-block-signature-capture-or-invoke-source-model-drift-before-runnable-block-lowering";
  static inline constexpr const char *kObjc3ExecutableBlockSourceModelLaneContract =
      "objc3c.block.source.model.v1";
  // block-source-storage-annotation constants: lane-A now extends
  // the truthful source-only block model with parser-owned byref/helper/
  // escape-shape annotations while runnable block lowering remains out of
  // scope.
  static inline constexpr const char *kObjc3ExecutableBlockSourceStorageAnnotationContractId =
      "objc3c.executable.block.source.storage.annotation.v1";
  static inline constexpr const char *kObjc3ExecutableBlockByrefStorageModel =
      "block-source-model-publishes-deterministic-byref-capture-candidates-before-runnable-block-byref-lowering";
  static inline constexpr const char *kObjc3ExecutableBlockHelperIntentModel =
      "block-source-model-publishes-copy-dispose-helper-intent-before-runnable-block-helper-lowering";
  static inline constexpr const char *kObjc3ExecutableBlockEscapeShapeModel =
      "block-source-model-publishes-heap-promotion-relevant-escape-shape-categories-before-runnable-block-escape-analysis";
  static inline constexpr const char *kObjc3ExecutableBlockSourceStorageAnnotationLaneContract =
      "objc3c.block.source.storage.annotations.v1";
  // block-runtime-semantic-rules freeze constants: lane-B now
  // freezes the truthful semantic boundary that current source-only block
  // admission exposes while native emit paths still fail closed on runnable
  // block semantics.
  static inline constexpr const char *kObjc3ExecutableBlockRuntimeSemanticRulesContractId =
      "objc3c.executable.block.runtime.semantic.rules.v1";
  static inline constexpr const char *kObjc3ExecutableBlockRuntimeCaptureLegalityModel =
      "block-runtime-semantics-freeze-capture-legality-on-deterministic-source-owned-capture-inventory";
  static inline constexpr const char *kObjc3ExecutableBlockRuntimeStorageClassModel =
      "block-runtime-semantics-freeze-byref-storage-as-source-annotation-only-until-runnable-byref-lowering-lands";
  static inline constexpr const char *kObjc3ExecutableBlockRuntimeEscapeBehaviorModel =
      "block-runtime-semantics-freeze-escape-shape-as-source-annotation-only-until-runnable-heap-promotion-lands";
  static inline constexpr const char *kObjc3ExecutableBlockRuntimeHelperGenerationModel =
      "block-runtime-semantics-freeze-helper-intent-as-source-annotation-only-until-runnable-helper-lowering-lands";
  static inline constexpr const char *kObjc3ExecutableBlockRuntimeInvocationModel =
      "block-runtime-semantics-freeze-block-literals-as-source-only-function-shaped-values-while-runnable-invocation-fails-closed";
  static inline constexpr const char *kObjc3ExecutableBlockRuntimeFailClosedModel =
      "block-runtime-semantics-fail-closed-on-native-emit-before-runnable-block-semantics-land";
  // capture-legality/escape/invocation implementation constants:
  // lane-B now upgrades the source-only semantic path so live capture
  // resolution, truthful escape classification, and local block invocation
  // typing are enforced before runnable block-object lowering lands.
  static inline constexpr const char *kObjc3ExecutableBlockCaptureLegalityImplementationContractId =
      "objc3c.executable.block.capture.legality.escape.and.invocation.v1";
  static inline constexpr const char *kObjc3ExecutableBlockCaptureLegalityImplementationModel =
      "source-only-sema-enforces-live-capture-resolution-and-mutability-classification-before-runnable-block-object-lowering";
  static inline constexpr const char *kObjc3ExecutableBlockEscapeClassificationImplementationModel =
      "source-only-sema-classifies-byref-escape-and-copy-dispose-requirements-from-parser-owned-annotations-before-runnable-helper-lowering";
  static inline constexpr const char *kObjc3ExecutableBlockInvocationTypingImplementationModel =
      "source-only-sema-types-local-block-invocations-as-callable-values-while-native-block-execution-remains-fail-closed";
  // byref/copy-dispose/object-ownership constants: lane-B now
  // extends the live source-only block semantics with ownership-sensitive
  // helper eligibility and fail-closed mutation rules for non-owning object
  // captures.
  static inline constexpr const char *kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId =
      "objc3c.executable.block.byref.copy.dispose.and.object.capture.ownership.v1";
  static inline constexpr const char *kObjc3ExecutableBlockByrefMutationOwnershipModel =
      "source-only-sema-rejects-weak-or-unowned-byref-mutation-before-runnable-block-ownership-lowering";
  static inline constexpr const char *kObjc3ExecutableBlockCopyDisposeEligibilityModel =
      "owned-object-captures-promote-copy-dispose-helper-eligibility-even-without-byref-cells";
  static inline constexpr const char *kObjc3ExecutableBlockObjectCaptureOwnershipModel =
      "weak-and-unowned-object-captures-remain-non-owning-and-do-not-force-copy-dispose-helpers";
  // block-lowering-ABI/artifact-boundary constants: lane-C now
  // freezes the truthful lowering boundary required for runnable block objects
  // without claiming that emitted block records, invoke thunks, byref cells,
  // or helper bodies already exist on native paths.
  static inline constexpr const char *kObjc3ExecutableBlockLoweringAbiArtifactBoundaryContractId =
      "objc3c.executable.block.lowering.abi.artifact.boundary.v1";
  static inline constexpr const char *kObjc3ExecutableBlockLoweringAbiModel =
      "block-object-descriptor-invoke-byref-and-helper-abi-boundary-freezes-on-source-modeled-lowering-surfaces-before-runnable-emission";
  static inline constexpr const char *kObjc3ExecutableBlockHelperSymbolPolicyModel =
      "copy-dispose-and-byref-helper-symbols-remain-source-modeled-and-non-emitted-until-next-runtime-phase";
  static inline constexpr const char *kObjc3ExecutableBlockArtifactInventoryModel =
      "source-only-manifest-lowering-surfaces-plus-fail-closed-native-ir-boundary-before-runnable-block-object-artifacts";
  static inline constexpr const char *kObjc3ExecutableBlockLoweringFailClosedModel =
      "native-emit-fails-closed-on-block-literals-before-runnable-block-object-lowering";
  static inline constexpr const char *kObjc3ExecutableBlockLoweringNonGoalModel =
      "no-emitted-block-object-records-no-invoke-thunks-no-byref-cell-storage-no-copy-dispose-helper-bodies";
  // executable-block-object/invoke-thunk constants: lane-C now
  // upgrades the frozen C001 boundary into one runnable lowering slice for
  // stack-allocated block objects, readonly scalar captures, and direct local
  // invocation, while byref/helper/ownership-sensitive cases stay deferred to
  // C003.
  static inline constexpr const char *kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId =
      "objc3c.executable.block.object.and.invoke.thunk.lowering.v1";
  static inline constexpr const char *kObjc3ExecutableBlockObjectInvokeThunkActiveModel =
      "native-lowering-emits-stack-block-objects-and-direct-local-invoke-thunks-for-readonly-scalar-captures";
  static inline constexpr const char *kObjc3ExecutableBlockObjectInvokeThunkDeferredModel =
      "byref-cells-copy-dispose-helpers-owned-object-captures-and-heap-promotion-stay-fail-closed-until-next-runtime-phase";
  static inline constexpr const char *kObjc3ExecutableBlockObjectInvokeThunkExecutionEvidenceModel =
      "native-compile-link-run-proves-local-block-invocation-through-emitted-block-storage-and-invoke-thunk";
  // byref-cell/copy-helper/dispose-helper constants: lane-C now
  // widens runnable native block lowering to non-escaping byref and owned
  // capture cases by emitting stack byref cells plus copy/dispose helper
  // bodies, while heap-promotion and runtime-managed block copying remain
  // deferred to later work.
  static inline constexpr const char *kObjc3ExecutableBlockByrefHelperLoweringContractId =
      "objc3c.executable.block.byref.helper.lowering.v1";
  static inline constexpr const char *kObjc3ExecutableBlockByrefHelperLoweringActiveModel =
      "native-lowering-emits-stack-byref-cells-and-copy-dispose-helper-bodies-for-nonescaping-block-captures";
  static inline constexpr const char *kObjc3ExecutableBlockByrefHelperLoweringDeferredModel =
      "heap-promotion-and-runtime-managed-block-copy-dispose-lifecycle-remain-deferred-to-later-runtime-work";
  static inline constexpr const char *kObjc3ExecutableBlockByrefHelperLoweringExecutionEvidenceModel =
      "native-compile-link-run-proves-byref-mutation-and-owned-capture-helper-lowering-through-emitted-block-helper-bodies";
  // escaping-block runtime-hook constants: lane-C widens runnable
  // block lowering to readonly-scalar escaping cases by lowering heap
  // promotion and invoke hooks against the native runtime while ownership-
  // sensitive escaping captures stay deferred to lane-D runtime work.
  static inline constexpr const char *kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId =
      "objc3c.executable.block.escape.runtime.hook.lowering.v1";
  static inline constexpr const char *kObjc3ExecutableBlockEscapeRuntimeHookLoweringActiveModel =
      "native-lowering-emits-runtime-block-promotion-and-invoke-hooks-for-readonly-scalar-escaping-block-values";
  static inline constexpr const char *kObjc3ExecutableBlockEscapeRuntimeHookLoweringDeferredModel =
      "byref-forwarding-owned-capture-escape-lifetimes-and-runtime-managed-copy-dispose-remain-deferred-to-later-runtime-work";
  static inline constexpr const char *kObjc3ExecutableBlockEscapeRuntimeHookLoweringExecutionEvidenceModel =
      "native-compile-link-run-proves-returned-and-argument-passed-readonly-scalar-block-values-through-runtime-promotion-hooks";
  // runnable-block-runtime gate constants: lane-E now freezes the
  // truthful integrated block-runtime proof boundary above the retained source,
  // sema, lowering, and runtime evidence chain before broader block closeout
  // samples expand it.
  static inline constexpr const char *kObjc3RunnableBlockRuntimeGateContractId =
      "objc3c.runnable.block.runtime.gate.v1";
  static inline constexpr const char *kObjc3RunnableBlockRuntimeGateEvidenceModel =
      "a003-b003-c004-d003-summary-chain";
  static inline constexpr const char *kObjc3RunnableBlockRuntimeGateActiveModel =
      "runnable-block-gate-consumes-source-sema-lowering-and-runtime-proofs-rather-than-metadata-only-summaries";
  static inline constexpr const char *kObjc3RunnableBlockRuntimeGateNonGoalModel =
      "no-public-block-object-abi-no-public-runtime-helper-header-no-generalized-foreign-block-interop-no-caller-frame-forwarding-bridge";
  static inline constexpr const char *kObjc3RunnableBlockRuntimeGateFailClosedModel =
      "fail-closed-on-runnable-block-runtime-evidence-drift";
  // runnable-block execution-matrix constants: lane-E now closes
  // the current current slice with one truthful executable matrix over the
  // existing native runtime path rather than widening the supported surface.
  static inline constexpr const char *kObjc3RunnableBlockExecutionMatrixContractId =
      "objc3c.runnable.block.execution.matrix.v1";
  static inline constexpr const char *kObjc3RunnableBlockExecutionMatrixEvidenceModel =
      "a003-b003-c004-d003-e001-summary-plus-integrated-native-block-smoke-matrix";
  static inline constexpr const char *kObjc3RunnableBlockExecutionMatrixActiveModel =
      "closeout-matrix-runs-owned-nonowning-byref-and-escaping-block-fixtures-against-the-native-runtime";
  static inline constexpr const char *kObjc3RunnableBlockExecutionMatrixNonGoalModel =
      "no-public-block-object-abi-no-public-runtime-helper-header-no-generalized-foreign-block-interop-no-caller-frame-forwarding-bridge";
  static inline constexpr const char *kObjc3RunnableBlockExecutionMatrixFailClosedModel =
      "fail-closed-on-runnable-block-execution-matrix-drift-or-doc-mismatch";
  // ARC source-surface/mode-boundary constants: ownership
  // qualifiers, weak/unowned metadata, autoreleasepool profiling, and ARC
  // fix-it summaries are already parser/sema-visible, but the native driver
  // still rejects `-fobjc-arc` and executable ownership-qualified
  // functions/methods remain fail-closed until ARC automation lands.
  static inline constexpr const char *kObjc3ArcSourceModeBoundaryContractId =
      "objc3c.arc.source.mode.boundary.freeze.v1";
  static inline constexpr const char *kObjc3ArcSourceModeBoundarySourceModel =
      "ownership-qualifier-weak-unowned-autoreleasepool-and-arc-fixit-source-surfaces-remain-live-without-enabling-runnable-arc-mode";
  static inline constexpr const char *kObjc3ArcSourceModeBoundaryModeModel =
      "native-driver-rejects-fobjc-arc-while-executable-ownership-qualified-functions-and-methods-stay-fail-closed";
  static inline constexpr const char *kObjc3ArcSourceModeBoundaryNonGoalModel =
      "no-fobjc-arc-cli-mode-no-fno-objc-arc-cli-mode-no-automatic-arc-cleanup-insertion-no-user-visible-arc-runtime-mode-split";
  static inline constexpr const char *kObjc3ArcSourceModeBoundaryFailClosedModel =
      "fail-closed-on-arc-source-mode-boundary-drift-before-arc-automation";
  static inline constexpr const char *kObjc3ArcModeHandlingContractId =
      "objc3c.arc.mode.handling.v1";
  static inline constexpr const char *kObjc3ArcModeHandlingSourceModel =
      "ownership-qualified-method-property-return-and-block-capture-surfaces-are-runnable-under-explicit-arc-mode";
  static inline constexpr const char *kObjc3ArcModeHandlingModeModel =
      "driver-admits-fobjc-arc-and-fno-objc-arc-and-threads-arc-mode-through-frontend-sema-and-ir";
  static inline constexpr const char *kObjc3ArcModeHandlingFailClosedModel =
      "non-arc-mode-still-rejects-executable-ownership-qualified-method-and-function-signatures";
  static inline constexpr const char *kObjc3ArcModeHandlingNonGoalModel =
      "no-generalized-arc-cleanup-synthesis-no-implicit-nonarc-promotion-no-full-arc-automation-yet";
  static inline constexpr const char *kObjc3ArcSemanticRulesContractId =
      "objc3c.arc.semantic.rules.v1";
  static inline constexpr const char *kObjc3ArcSemanticRulesSourceModel =
      "explicit-arc-mode-admits-only-explicit-ownership-surfaces-while-forbidden-property-forms-and-broad-inference-remain-fail-closed";
  static inline constexpr const char *kObjc3ArcSemanticRulesSemanticModel =
      "conflicting-property-ownership-forms-and-atomic-ownership-aware-storage-still-fail-closed-while-general-arc-inference-remains-deferred";
  static inline constexpr const char *kObjc3ArcSemanticRulesFailClosedModel =
      "forbidden-arc-property-forms-and-non-inferred-lifetime-semantics-terminate-deterministically";
  static inline constexpr const char *kObjc3ArcSemanticRulesNonGoalModel =
      "no-implicit-retain-release-inference-no-lifetime-extension-no-method-family-based-arc-semantics-yet";
  static inline constexpr const char *kObjc3ArcInferenceLifetimeContractId =
      "objc3c.arc.inference.lifetime.v1";
  static inline constexpr const char *kObjc3ArcInferenceLifetimeSourceModel =
      "explicit-arc-mode-now-infers-strong-owned-executable-object-signatures-for-the-supported-runnable-slice";
  static inline constexpr const char *kObjc3ArcInferenceLifetimeSemanticModel =
      "arc-enabled-unqualified-object-signatures-now-produce-canonical-retain-release-lifetime-accounting-while-nonarc-remains-zero-inference";
  static inline constexpr const char *kObjc3ArcInferenceLifetimeFailClosedModel =
      "non-arc-mode-keeps-unqualified-object-signatures-non-inferred-and-zero-retain-release-lifetime-accounting";
  static inline constexpr const char *kObjc3ArcInferenceLifetimeNonGoalModel =
      "no-full-arc-cleanup-synthesis-no-weak-autorelease-return-property-synthesis-or-block-interaction-arc-semantics-yet";
  static inline constexpr const char *kObjc3ArcInteractionSemanticsContractId =
      "objc3c.arc.interaction.semantics.v1";
  static inline constexpr const char *kObjc3ArcInteractionSemanticsSourceModel =
      "explicit-arc-mode-now-covers-weak-autorelease-return-property-synthesis-and-block-ownership-interactions-for-the-supported-runnable-slice";
  static inline constexpr const char *kObjc3ArcInteractionSemanticsSemanticModel =
      "weak-properties-and-nonowning-captures-stay-nonretaining-autorelease-returns-stay-profiled-and-synthesized-property-accessors-publish-owned-lifetime-packets-under-arc";
  static inline constexpr const char *kObjc3ArcInteractionSemanticsFailClosedModel =
      "unsupported-arc-cleanup-and-broader-interactions-still-remain-explicitly-deferred";
  static inline constexpr const char *kObjc3ArcInteractionSemanticsNonGoalModel =
      "no-general-arc-cleanup-insertion-no-cross-module-arc-interop-no-full-method-family-automation-yet";
  // Legacy extraction anchor retained for contract tests:
  // enum class Kind { Number, BoolLiteral, NilLiteral, Identifier, Binary, Conditional, Call, MessageSend };
  enum class Kind {
    Number,
    BoolLiteral,
    NilLiteral,
    Identifier,
    Binary,
    Conditional,
    Call,
    MessageSend,
    BlockLiteral
  };
  enum class MessageSendForm { None, Unary, Keyword };
  enum class TryOperatorKind { None, Propagate, Optional, Forced };
  enum class DispatchSurfaceKind {
    Unclassified,
    Instance,
    Class,
    Super,
    Direct,
    Dynamic
  };
  struct MessageSendSelectorPiece {
    std::string keyword;
    bool has_argument = false;
    unsigned line = 1;
    unsigned column = 1;
  };
  struct BlockParameter {
    std::string name;
    ValueType type = ValueType::Unknown;
  };
  struct ExplicitBlockCaptureItem {
    std::string name;
    std::string mode;
  };
  Kind kind = Kind::Number;
  int number = 0;
  bool bool_value = false;
  std::string ident;
  std::string selector;
  MessageSendForm message_send_form = MessageSendForm::None;
  std::string message_send_form_symbol;
  bool optional_send_enabled = false;
  bool optional_member_access_enabled = false;
  std::string optional_send_symbol;
  bool optional_send_is_normalized = false;
  std::vector<MessageSendSelectorPiece> selector_lowering_pieces;
  std::string selector_lowering_symbol;
  bool selector_lowering_is_normalized = false;
  unsigned dispatch_abi_receiver_slots_marshaled = 0;
  unsigned dispatch_abi_selector_slots_marshaled = 0;
  unsigned dispatch_abi_argument_value_slots_marshaled = 0;
  unsigned dispatch_abi_argument_padding_slots_marshaled = 0;
  unsigned dispatch_abi_argument_total_slots_marshaled = 0;
  unsigned dispatch_abi_total_slots_marshaled = 0;
  unsigned dispatch_abi_runtime_arg_slots = 0;
  std::string dispatch_abi_marshalling_symbol;
  bool dispatch_abi_marshalling_is_normalized = false;
  bool nil_receiver_semantics_enabled = false;
  bool nil_receiver_foldable = false;
  bool nil_receiver_requires_runtime_dispatch = true;
  std::string nil_receiver_folding_symbol;
  bool nil_receiver_semantics_is_normalized = false;
  bool super_dispatch_enabled = false;
  bool super_dispatch_requires_class_context = false;
  std::string super_dispatch_symbol;
  bool super_dispatch_semantics_is_normalized = false;
  std::string method_family_name;
  bool method_family_returns_retained_result = false;
  bool method_family_returns_related_result = false;
  std::string method_family_semantics_symbol;
  bool method_family_semantics_is_normalized = false;
  bool runtime_shim_host_link_required = true;
  bool runtime_shim_host_link_elided = false;
  unsigned runtime_shim_host_link_declaration_parameter_count = 0;
  std::string runtime_dispatch_bridge_symbol;
  std::string runtime_shim_host_link_symbol;
  bool runtime_shim_host_link_is_normalized = false;
  DispatchSurfaceKind dispatch_surface_kind = DispatchSurfaceKind::Unclassified;
  std::string dispatch_surface_family_symbol;
  std::string dispatch_surface_entrypoint_family_symbol;
  bool dispatch_surface_is_normalized = false;
  std::vector<std::string> block_parameter_names_lexicographic;
  std::size_t block_parameter_count = 0;
  std::vector<std::string> block_parameter_signature_entries_lexicographic;
  std::vector<ValueType> block_parameter_types_source_order;
  std::vector<BlockParameter> block_parameters_source_order;
  std::size_t block_explicit_typed_parameter_count = 0;
  std::size_t block_implicit_parameter_count = 0;
  std::string block_signature_profile;
  std::vector<std::string> block_capture_names_lexicographic;
  std::size_t block_capture_count = 0;
  std::vector<std::string> block_capture_inventory_entries_lexicographic;
  std::size_t block_byvalue_readonly_capture_count = 0;
  std::string block_capture_inventory_profile;
  bool block_has_explicit_capture_list = false;
  std::vector<ExplicitBlockCaptureItem> block_explicit_capture_items_source_order;
  std::vector<std::string> block_explicit_capture_names_lexicographic;
  std::size_t block_explicit_capture_count = 0;
  std::size_t block_explicit_capture_move_count = 0;
  std::size_t block_explicit_capture_weak_count = 0;
  std::size_t block_explicit_capture_unowned_count = 0;
  std::size_t block_explicit_capture_plain_count = 0;
  std::string block_explicit_capture_profile;
  std::vector<std::string> block_mutated_capture_names_lexicographic;
  std::size_t block_mutated_capture_count = 0;
  std::vector<std::string> block_byref_capture_names_lexicographic;
  std::size_t block_byref_capture_count = 0;
  std::size_t block_body_statement_count = 0;
  std::string block_capture_profile;
  bool block_capture_set_deterministic = false;
  bool block_literal_is_normalized = false;
  std::size_t block_abi_invoke_argument_slots = 0;
  std::size_t block_abi_capture_word_count = 0;
  std::vector<std::string> block_invoke_surface_entries_lexicographic;
  std::string block_invoke_surface_profile;
  std::string block_source_model_replay_key;
  bool block_source_model_is_normalized = false;
  std::string block_abi_layout_profile;
  std::string block_abi_descriptor_symbol;
  std::string block_invoke_trampoline_symbol;
  bool block_abi_has_invoke_trampoline = false;
  bool block_abi_layout_is_normalized = false;
  std::size_t block_storage_mutable_capture_count = 0;
  std::size_t block_storage_byref_slot_count = 0;
  bool block_storage_requires_byref_cells = false;
  bool block_storage_escape_analysis_enabled = false;
  bool block_storage_escape_to_heap = false;
  bool block_storage_escape_profile_is_normalized = false;
  std::string block_storage_escape_profile;
  std::string block_storage_byref_layout_symbol;
  bool block_copy_helper_required = false;
  bool block_dispose_helper_required = false;
  bool block_copy_dispose_profile_is_normalized = false;
  std::string block_copy_dispose_profile;
  std::string block_copy_helper_symbol;
  std::string block_dispose_helper_symbol;
  bool block_copy_helper_intent_required = false;
  bool block_dispose_helper_intent_required = false;
  bool block_escape_shape_promotes_to_heap_candidate = false;
  bool block_source_storage_annotations_are_normalized = false;
  std::string block_helper_intent_profile;
  std::string block_escape_shape_symbol;
  std::string block_escape_shape_profile;
  std::size_t block_runtime_owned_object_capture_count = 0;
  std::size_t block_runtime_weak_object_capture_count = 0;
  std::size_t block_runtime_unowned_object_capture_count = 0;
  std::vector<std::string> block_runtime_owned_object_capture_names_lexicographic;
  std::vector<std::string> block_runtime_weak_object_capture_names_lexicographic;
  std::vector<std::string> block_runtime_unowned_object_capture_names_lexicographic;
  bool block_runtime_copy_helper_required = false;
  bool block_runtime_dispose_helper_required = false;
  bool block_runtime_capture_ownership_is_normalized = false;
  std::string block_runtime_capture_ownership_profile;
  std::size_t block_determinism_perf_baseline_weight = 0;
  bool block_determinism_perf_baseline_profile_is_normalized = false;
  std::string block_determinism_perf_baseline_profile;
  bool typed_keypath_literal_enabled = false;
  bool typed_keypath_root_is_self = false;
  std::string typed_keypath_root_name;
  std::vector<std::string> typed_keypath_components;
  std::string typed_keypath_literal_profile;
  bool typed_keypath_literal_is_normalized = false;
  bool try_expression_enabled = false;
  TryOperatorKind try_operator_kind = TryOperatorKind::None;
  bool try_expression_requires_throwing_context = false;
  bool try_expression_is_normalized = false;
  std::string try_expression_profile;
  bool await_expression_enabled = false;
  bool throw_statement_enabled = false;
  bool throw_statement_is_normalized = false;
  std::string throw_statement_profile;
  std::vector<std::unique_ptr<Stmt>> block_body;
  std::string op = "+";
  std::unique_ptr<Expr> receiver;
  std::unique_ptr<Expr> left;
  std::unique_ptr<Expr> right;
  std::unique_ptr<Expr> third;
  std::vector<std::unique_ptr<Expr>> args;
  unsigned line = 1;
  unsigned column = 1;
};

struct LetStmt;
struct AssignStmt;
struct ReturnStmt;
struct IfStmt;
struct DoWhileStmt;
struct ForStmt;
struct SwitchStmt;
struct WhileStmt;
struct BlockStmt;
struct ExprStmt;

struct Stmt {
  enum class Kind {
    Let,
    Assign,
    Return,
    If,
    DoWhile,
    For,
    Switch,
    While,
    Break,
    Continue,
    Empty,
    Block,
    Defer,
    Expr
  };
  Kind kind = Kind::Expr;
  std::unique_ptr<LetStmt> let_stmt;
  std::unique_ptr<AssignStmt> assign_stmt;
  std::unique_ptr<ReturnStmt> return_stmt;
  std::unique_ptr<IfStmt> if_stmt;
  std::unique_ptr<DoWhileStmt> do_while_stmt;
  std::unique_ptr<ForStmt> for_stmt;
  std::unique_ptr<SwitchStmt> switch_stmt;
  std::unique_ptr<WhileStmt> while_stmt;
  std::unique_ptr<BlockStmt> block_stmt;
  std::unique_ptr<ExprStmt> expr_stmt;
  unsigned line = 1;
  unsigned column = 1;
};

struct LetStmt {
  std::string name;
  std::unique_ptr<Expr> value;
  bool cleanup_attribute_declared = false;
  bool cleanup_sugar_declared = false;
  std::string cleanup_function_symbol;
  bool resource_attribute_declared = false;
  bool resource_sugar_declared = false;
  std::string resource_close_symbol;
  std::string resource_invalid_expression;
  bool cleanup_profile_is_normalized = false;
  std::string cleanup_profile;
  bool resource_profile_is_normalized = false;
  std::string resource_profile;
  unsigned line = 1;
  unsigned column = 1;
};

struct AssignStmt {
  std::string name;
  std::string op = "=";
  std::unique_ptr<Expr> value;
  unsigned line = 1;
  unsigned column = 1;
};

struct ReturnStmt {
  std::unique_ptr<Expr> value;
  unsigned line = 1;
  unsigned column = 1;
};

struct IfStmt {
  std::unique_ptr<Expr> condition;
  std::vector<std::unique_ptr<Stmt>> then_body;
  std::vector<std::unique_ptr<Stmt>> else_body;
  bool optional_binding_surface_enabled = false;
  bool guard_binding_surface_enabled = false;
  bool guard_condition_list_surface_enabled = false;
  std::size_t optional_binding_clause_count = 0;
  std::size_t guard_boolean_condition_clause_count = 0;
  std::vector<std::unique_ptr<Expr>> guard_condition_exprs;
  unsigned line = 1;
  unsigned column = 1;
};

struct DoWhileStmt {
  std::vector<std::unique_ptr<Stmt>> body;
  std::unique_ptr<Expr> condition;
  unsigned line = 1;
  unsigned column = 1;
};

struct ForClause {
  enum class Kind { None, Let, Assign, Expr };
  Kind kind = Kind::None;
  std::string name;
  std::string op = "=";
  std::unique_ptr<Expr> value;
  unsigned line = 1;
  unsigned column = 1;
};

struct ForStmt {
  ForClause init;
  std::unique_ptr<Expr> condition;
  ForClause step;
  std::vector<std::unique_ptr<Stmt>> body;
  unsigned line = 1;
  unsigned column = 1;
};

enum class MatchPatternKind {
  None,
  LiteralInteger,
  LiteralBool,
  LiteralNil,
  Wildcard,
  Binding,
  ResultCase,
};

struct SwitchCase {
  bool is_default = false;
  int value = 0;
  unsigned value_line = 1;
  unsigned value_column = 1;
  bool match_pattern_enabled = false;
  MatchPatternKind match_pattern_kind = MatchPatternKind::None;
  bool match_binding_mutable = false;
  std::string match_binding_name;
  std::string match_result_case_name;
  std::vector<std::unique_ptr<Stmt>> body;
  unsigned line = 1;
  unsigned column = 1;
};

struct SwitchStmt {
  std::unique_ptr<Expr> condition;
  std::vector<SwitchCase> cases;
  bool match_surface_enabled = false;
  unsigned line = 1;
  unsigned column = 1;
};

struct WhileStmt {
  std::unique_ptr<Expr> condition;
  std::vector<std::unique_ptr<Stmt>> body;
  unsigned line = 1;
  unsigned column = 1;
};

struct BlockStmt {
  struct CatchClause {
    bool catch_all = true;
    bool has_binding = false;
    std::string binding_name;
    std::string binding_type_spelling = "id<Error>";
    std::vector<std::unique_ptr<Stmt>> body;
    unsigned line = 1;
    unsigned column = 1;
  };
  std::vector<std::unique_ptr<Stmt>> body;
  bool is_autoreleasepool_scope = false;
  bool is_do_catch_scope = false;
  bool do_catch_is_normalized = false;
  std::vector<CatchClause> catch_clauses;
  std::string do_catch_profile;
  std::string autoreleasepool_scope_symbol;
  unsigned autoreleasepool_scope_depth = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct ExprStmt {
  std::unique_ptr<Expr> value;
  unsigned line = 1;
  unsigned column = 1;
};

struct FuncParam {
  std::string name;
  ValueType type = ValueType::I32;
  bool vector_spelling = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool id_spelling = false;
  bool class_spelling = false;
  bool sel_spelling = false;
  bool instancetype_spelling = false;
  bool object_pointer_type_spelling = false;
  std::string object_pointer_type_name;
  std::string typecheck_family_symbol;
  bool has_generic_suffix = false;
  bool generic_suffix_terminated = true;
  std::string generic_suffix_text;
  unsigned generic_line = 1;
  unsigned generic_column = 1;
  bool lightweight_generic_constraint_profile_is_normalized = false;
  std::string lightweight_generic_constraint_profile;
  bool nullability_flow_profile_is_normalized = false;
  std::string nullability_flow_profile;
  bool protocol_qualified_object_type_profile_is_normalized = false;
  std::string protocol_qualified_object_type_profile;
  bool variance_bridge_cast_profile_is_normalized = false;
  std::string variance_bridge_cast_profile;
  bool generic_metadata_abi_profile_is_normalized = false;
  std::string generic_metadata_abi_profile;
  bool module_import_graph_profile_is_normalized = false;
  std::string module_import_graph_profile;
  bool namespace_collision_shadowing_profile_is_normalized = false;
  std::string namespace_collision_shadowing_profile;
  bool public_private_api_partition_profile_is_normalized = false;
  std::string public_private_api_partition_profile;
  bool incremental_module_cache_invalidation_profile_is_normalized = false;
  std::string incremental_module_cache_invalidation_profile;
  bool cross_module_conformance_profile_is_normalized = false;
  std::string cross_module_conformance_profile;
  bool has_pointer_declarator = false;
  unsigned pointer_declarator_depth = 0;
  std::vector<Objc3SemaTokenMetadata> pointer_declarator_tokens;
  std::vector<Objc3SemaTokenMetadata> nullability_suffix_tokens;
  bool has_ownership_qualifier = false;
  std::string ownership_qualifier_spelling;
  std::string ownership_qualifier_symbol;
  std::vector<Objc3SemaTokenMetadata> ownership_qualifier_tokens;
  bool ownership_insert_retain = false;
  bool ownership_insert_release = false;
  bool ownership_insert_autorelease = false;
  std::string ownership_operation_profile;
  bool ownership_is_weak_reference = false;
  bool ownership_is_unowned_reference = false;
  bool ownership_is_unowned_safe_reference = false;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  bool ownership_arc_diagnostic_candidate = false;
  bool ownership_arc_fixit_available = false;
  std::string ownership_arc_diagnostic_profile;
  std::string ownership_arc_fixit_hint;
  bool borrowed_pointer_qualified = false;
  std::string borrowed_pointer_profile;
  unsigned line = 1;
  unsigned column = 1;
};

inline constexpr const char *kObjc3RuntimeMetadataSourceOwnershipContractId =
    "objc3c.runtime.metadata.source.ownership.freeze.v1";
inline constexpr const char *kObjc3RuntimeMetadataCanonicalSourceSchema =
    "objc3-runtime-metadata-source-boundary-v1";
inline constexpr const char *kObjc3RuntimeMetadataClassAstAnchor =
    "Objc3InterfaceDecl/Objc3ImplementationDecl";
inline constexpr const char *kObjc3RuntimeMetadataProtocolAstAnchor =
    "Objc3ProtocolDecl";
inline constexpr const char *kObjc3RuntimeMetadataCategoryAstAnchor =
    "Objc3InterfaceDecl.has_category/Objc3ImplementationDecl.has_category";
inline constexpr const char *kObjc3RuntimeMetadataPropertyAstAnchor =
    "Objc3PropertyDecl";
inline constexpr const char *kObjc3ExecutablePropertyIvarSourceClosureContractId =
    "objc3c.executable.property.ivar.source.closure.v1";
inline constexpr const char *kObjc3ExecutablePropertyIvarSourceSurfaceModel =
    "property-ivar-executable-source-closure-freezes-decls-synthesis-bindings-and-accessor-selectors-before-storage-realization";
inline constexpr const char *kObjc3ExecutablePropertyIvarSourceModelCompletionContractId =
    "objc3c.executable.property.ivar.source.model.completion.v1";
inline constexpr const char *kObjc3ExecutablePropertyIvarLayoutModel =
    "property-ivar-source-model-computes-deterministic-layout-slots-sizes-and-alignment-before-runtime-storage-realization";
inline constexpr const char *kObjc3ExecutablePropertyAttributeModel =
    "property-attribute-and-effective-accessor-source-model-publishes-deterministic-ownership-and-selector-profiles";
// accessor/layout lowering freeze anchor: AST remains the canonical
// source of property attribute/accessor profiles, synthesized-binding
// identities, and ivar layout identities. Lane-C lowering may only serialize
// this handoff into emitted metadata/object artifacts; it must not synthesize
// accessor bodies or invent runtime storage/layout beyond the sema-approved
// source model.
// ivar offset/layout emission anchor: AST-owned
// `Objc3PropertyDecl.executable_ivar_layout_symbol`,
// `.executable_ivar_layout_slot_index`, `.executable_ivar_layout_size_bytes`,
// and `.executable_ivar_layout_alignment_bytes` remain the only authoritative
// layout-shape inputs that lane-C may use when it materializes emitted offset
// globals or per-owner layout tables.
inline constexpr const char *kObjc3ExecutablePropertyIvarSemanticsContractId =
    "objc3c.executable.property.ivar.semantics.v1";
inline constexpr const char *kObjc3ExecutablePropertySynthesisSemanticsModel =
    "non-category-class-interface-properties-own-deterministic-implicit-ivar-and-synthesized-binding-identities-until-explicit-synthesize-lands";
inline constexpr const char *kObjc3ExecutablePropertyDefaultIvarBindingResolutionModel =
    "matched-class-implementations-resolve-interface-declared-properties-through-authoritative-default-ivar-bindings-with-or-without-implementation-redeclaration";
inline constexpr const char *kObjc3ExecutablePropertyAccessorSemanticsModel =
    "readonly-and-attribute-driven-accessor-selectors-resolve-to-one-declaration-level-profile-before-body-emission";
inline constexpr const char *kObjc3ExecutablePropertyAccessorSelectorUniquenessModel =
    "effective-getter-and-setter-selectors-must-be-unique-within-each-property-container-before-runtime-accessor-binding";
inline constexpr const char *kObjc3ExecutablePropertyOwnershipAtomicityInteractionModel =
    "runtime-managed-property-ownership-and-atomicity-combinations-fail-closed-until-executable-accessor-storage-semantics-land";
inline constexpr const char *kObjc3ExecutablePropertyStorageSemanticsModel =
    "interface-owned-property-layout-slots-sizes-and-alignment-remain-deterministic-before-runtime-allocation";
inline constexpr const char *kObjc3ExecutablePropertyCompatibilitySemanticsModel =
    "protocol-and-inheritance-compatibility-compare-declaration-level-attribute-accessor-ownership-profiles-not-storage-local-layout-symbols";
inline constexpr const char *kObjc3RuntimeMetadataMethodAstAnchor =
    "Objc3MethodDecl";
inline constexpr const char *kObjc3RuntimeMetadataIvarAstAnchor =
    "Objc3PropertyDecl.ivar_binding_symbol";
inline constexpr const char *kObjc3RuntimeMetadataIvarSourceModel =
    "property-synthesis-ivar-binding-symbols";
inline constexpr const char *kObjc3RuntimeMetadataSectionAbiContractId =
    "objc3c.runtime.metadata.section.abi.symbol.policy.freeze.v1";
inline constexpr const char *kObjc3RuntimeMetadataSectionPublicationContractId =
    "objc3c.runtime.metadata.section.publication.v1";
inline constexpr const char *kObjc3RuntimeStatePublicationSurfaceContractId =
    "objc3c.runtime.state.publication.surface.v1";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionContractId =
    "objc3c.runtime.metadata.object.inspection.harness.v1";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionContractId =
    "objc3c.executable.metadata.debug.projection.v1";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestPackagingContractId =
    "objc3c.executable.metadata.runtime.ingest.packaging.boundary.v1";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryBoundaryContractId =
    "objc3c.executable.metadata.runtime.ingest.binary.boundary.v1";
inline constexpr const char *kObjc3RuntimeSupportLibraryContractId =
    "objc3c.runtime.support.library.surface.build.contract.v1";
inline constexpr const char *kObjc3RuntimeSupportLibraryCoreFeatureContractId =
    "objc3c.runtime.support.library.core.feature.v1";
// emitted metadata inventory freeze anchor: these constants define
// the canonical logical sections, symbol families, linkage, visibility,
// retention root, and inspection commands for the currently supported runtime
// metadata inventory.
inline constexpr const char *kObjc3RuntimeMetadataLogicalImageInfoSection =
    "objc3.runtime.image_info";
inline constexpr const char *kObjc3RuntimeMetadataLogicalClassDescriptorSection =
    "objc3.runtime.class_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataLogicalProtocolDescriptorSection =
    "objc3.runtime.protocol_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataLogicalCategoryDescriptorSection =
    "objc3.runtime.category_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataLogicalPropertyDescriptorSection =
    "objc3.runtime.property_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataLogicalIvarDescriptorSection =
    "objc3.runtime.ivar_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataDescriptorSymbolPrefix =
    "__objc3_meta_";
inline constexpr const char *kObjc3RuntimeMetadataAggregateSymbolPrefix =
    "__objc3_sec_";
inline constexpr const char *kObjc3RuntimeMetadataImageInfoSymbol =
    "__objc3_image_info";
inline constexpr const char *kObjc3RuntimeMetadataClassDescriptorAggregateSymbol =
    "__objc3_sec_class_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataProtocolDescriptorAggregateSymbol =
    "__objc3_sec_protocol_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataCategoryDescriptorAggregateSymbol =
    "__objc3_sec_category_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataPropertyDescriptorAggregateSymbol =
    "__objc3_sec_property_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataIvarDescriptorAggregateSymbol =
    "__objc3_sec_ivar_descriptors";
inline constexpr const char *kObjc3RuntimeMetadataDescriptorLinkagePolicy =
    "private";
inline constexpr const char *kObjc3RuntimeMetadataAggregateLinkagePolicy =
    "internal";
inline constexpr const char *kObjc3RuntimeMetadataVisibilityPolicy =
    "hidden";
inline constexpr const char *kObjc3RuntimeMetadataRetentionPolicyRoot =
    "llvm.used";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionFixturePath =
    "tests/tooling/fixtures/native/runtime_metadata_object_inspection_zero_descriptor.objc3";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionEmitPrefix =
    "module";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionObjectRelativePath =
    "module.obj";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionSectionInventoryRowKey =
    "zero-descriptor-section-inventory";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionSymbolInventoryRowKey =
    "zero-descriptor-symbol-inventory";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionSectionCommand =
    "llvm-readobj --sections module.obj";
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionSymbolCommand =
    "llvm-objdump --syms module.obj";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionMatrixContractId =
    "objc3c.runtime.metadata.source.to.section.matrix.v1";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionMatrixSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_metadata_source_to_section_matrix";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionMatrixOrderingModel =
    "source-graph-node-kind-order-v1";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode =
    "standalone-descriptor-section";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode =
    "no-standalone-emission-yet";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode =
    "fixture-plus-object-inspection";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode =
    "source-graph-fixture";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue =
    "(none)";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior =
    "zero-sentinel-or-count-plus-pointer-vector";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior =
    "none";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionInterfaceRowKey =
    "interface-node-to-emission";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionImplementationRowKey =
    "implementation-node-to-emission";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionClassRowKey =
    "class-node-to-emission";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionMetaclassRowKey =
    "metaclass-node-to-emission";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionProtocolRowKey =
    "protocol-node-to-emission";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionCategoryRowKey =
    "category-node-to-emission";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionPropertyRowKey =
    "property-node-to-emission";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionMethodRowKey =
    "method-node-to-emission";
inline constexpr const char *kObjc3RuntimeMetadataSourceToSectionIvarRowKey =
    "ivar-node-to-emission";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionNamedMetadataName =
    "!objc3.objc_executable_metadata_debug_projection";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionManifestSurfacePath =
    "frontend.pipeline.semantic_surface.objc_executable_metadata_debug_projection";
inline constexpr const char *kObjc3ExecutableMetadataTypedLoweringHandoffManifestSurfacePath =
    "frontend.pipeline.semantic_surface.objc_executable_metadata_typed_lowering_handoff";
inline constexpr const char *kObjc3ExecutableMetadataSourceGraphManifestSurfacePath =
    "frontend.pipeline.semantic_surface.objc_executable_metadata_source_graph";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionClassFixturePath =
    "tests/tooling/fixtures/native/runtime_metadata_source_records_class_protocol_property_ivar.objc3";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionCategoryFixturePath =
    "tests/tooling/fixtures/native/runtime_metadata_source_records_category_protocol_property.objc3";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionIrFixturePath =
    "tests/tooling/fixtures/native/hello.objc3";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionEmitPrefix =
    "module";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionManifestRelativePath =
    "module.manifest.json";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionIrRelativePath =
    "module.ll";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionClassManifestRowKey =
    "class-protocol-property-ivar-manifest-projection";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionCategoryManifestRowKey =
    "category-protocol-property-manifest-projection";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionIrNamedMetadataRowKey =
    "hello-ir-named-metadata-anchor";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionClassProbeCommand =
    "artifacts/bin/objc3c-frontend-c-api-runner.exe "
    "tests/tooling/fixtures/native/runtime_metadata_source_records_class_protocol_property_ivar.objc3 "
    "--out-dir <probe-root>/class_protocol_property_ivar --emit-prefix module "
    "--no-emit-ir --no-emit-object";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionCategoryProbeCommand =
    "artifacts/bin/objc3c-frontend-c-api-runner.exe "
    "tests/tooling/fixtures/native/runtime_metadata_source_records_category_protocol_property.objc3 "
    "--out-dir <probe-root>/category_protocol_property --emit-prefix module "
    "--no-emit-ir --no-emit-object";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionIrProbeCommand =
    "artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/hello.objc3 "
    "--out-dir <probe-root>/hello_ir_anchor --emit-prefix module --no-emit-object";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionManifestInspectionCommand =
    "python -c \"import json,pathlib; "
    "payload=json.loads(pathlib.Path('module.manifest.json').read_text()); "
    "print(payload['frontend']['pipeline']['semantic_surface']['objc_executable_metadata_debug_projection'])\"";
inline constexpr const char *kObjc3ExecutableMetadataDebugProjectionIrInspectionCommand =
    "Select-String -Path module.ll -Pattern '!objc3.objc_executable_metadata_debug_projection'";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestPackagingSurfacePath =
    "frontend.pipeline.semantic_surface.objc_executable_metadata_runtime_ingest_packaging_contract";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestPackagingPayloadModel =
    "typed-handoff-plus-debug-projection-manifest-v1";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestPackagingTransportArtifact =
    "module.manifest.json";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySurfacePath =
    "frontend.pipeline.semantic_surface.objc_executable_metadata_runtime_ingest_binary_boundary";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeFormat =
    "objc3-runtime-metadata-envelope-v1";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryArtifactSuffix =
    ".runtime-metadata.bin";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryArtifactRelativePath =
    "module.runtime-metadata.bin";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryMagic =
    "OBJC3RM1";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryPackagingChunkName =
    "runtime_ingest_packaging_contract";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryTypedHandoffChunkName =
    "typed_lowering_handoff";
inline constexpr const char *kObjc3ExecutableMetadataRuntimeIngestBinaryDebugProjectionChunkName =
    "debug_projection";
inline constexpr std::uint32_t kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeVersion = 1u;
inline constexpr std::uint32_t kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeChunkCount = 3u;
inline constexpr const char *kObjc3RuntimeTranslationUnitRegistrationContractId =
    "objc3c.translation.unit.registration.surface.freeze.v1";
inline constexpr const char *kObjc3RuntimeTranslationUnitRegistrationSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_translation_unit_registration_contract";
inline constexpr const char *kObjc3RuntimeTranslationUnitRegistrationPayloadModel =
    "runtime-metadata-binary-plus-linker-retention-sidecars-v1";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationPayloadArtifactRelativePath =
        kObjc3ExecutableMetadataRuntimeIngestBinaryArtifactRelativePath;
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationLinkerResponseArtifactRelativePath =
        "module.runtime-metadata-linker-options.rsp";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationDiscoveryArtifactRelativePath =
        "module.runtime-metadata-discovery.json";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol =
        "__objc3_runtime_register_image_ctor";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationConstructorRootOwnershipModel =
        "compiler-emits-constructor-root-runtime-owns-registration-state";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationConstructorEmissionMode =
        "reserved-not-emitted-yet";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationConstructorPriorityPolicy =
        "deferred-until-next-runtime-phase";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol =
        "objc3_runtime_register_image";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationTranslationUnitIdentityModel =
        "input-path-plus-parse-and-lowering-replay";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestContractId =
        "objc3c.translation.unit.registration.manifest.v1";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_translation_unit_registration_manifest";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestPayloadModel =
        "translation-unit-registration-manifest-json-v1";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestArtifactSuffix =
        ".runtime-registration-manifest.json";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestArtifactRelativePath =
        "module.runtime-registration-manifest.json";
inline constexpr const char *kObjc3CrossModuleRuntimeLinkPlanContractId =
    "objc3c.cross.module.runtime.packaging.link.plan.v1";
inline constexpr const char *kObjc3CrossModuleRuntimeLinkPlanPayloadModel =
    "cross-module-runtime-link-plan-json-v1";
inline constexpr const char *kObjc3CrossModuleRuntimeLinkPlanArtifactSuffix =
    ".cross-module-runtime-link-plan.json";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkPlanArtifactRelativePath =
        "module.cross-module-runtime-link-plan.json";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkerResponseArtifactSuffix =
        ".cross-module-runtime-linker-options.rsp";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkerResponseArtifactRelativePath =
        "module.cross-module-runtime-linker-options.rsp";
inline constexpr const char
    *kObjc3Part10MacroHostProcessCacheArtifactSuffix =
        ".part10-macro-host-cache.json";
inline constexpr const char
    *kObjc3Part10MacroHostProcessCacheArtifactRelativePath =
        "module.part10-macro-host-cache.json";
inline constexpr const char
    *kObjc3Part11BridgeHeaderArtifactSuffix = ".part11-bridge.h";
inline constexpr const char
    *kObjc3Part11BridgeHeaderArtifactRelativePath =
        "module.part11-bridge.h";
inline constexpr const char
    *kObjc3Part11BridgeModuleArtifactSuffix = ".part11-bridge.modulemap";
inline constexpr const char
    *kObjc3Part11BridgeModuleArtifactRelativePath =
        "module.part11-bridge.modulemap";
inline constexpr const char
    *kObjc3Part11BridgeArtifactSuffix = ".part11-bridge.json";
inline constexpr const char
    *kObjc3Part11BridgeArtifactRelativePath = "module.part11-bridge.json";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkPlanAuthorityModel =
        "runtime-import-surface-plus-imported-registration-manifest-peer-artifacts-drive-cross-module-link-plan";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkPlanPackagingModel =
        "compiler-emits-cross-module-link-plan-and-merged-linker-response";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkPlanRegistrationScopeModel =
        "registration-ordinal-sorted-link-plan-drives-multi-image-startup-registration";
inline constexpr const char
    *kObjc3CrossModuleRuntimeLinkObjectOrderModel =
        "ascending-registration-ordinal-then-translation-unit-identity-key";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationInitStubSymbolPrefix =
        "__objc3_runtime_register_image_init_stub_";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationInitStubOwnershipModel =
        "lowering-emits-init-stub-from-registration-manifest";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestAuthorityModel =
        "registration-manifest-authoritative-for-constructor-root-shape";
inline constexpr const char
    *kObjc3RuntimeTranslationUnitRegistrationManifestPriorityPolicy =
        "deferred-until-next-runtime-phase";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapInvariantContractId =
        "objc3c.runtime.startup.bootstrap.invariants.v1";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapInvariantSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_startup_bootstrap_invariants";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapDuplicateRegistrationPolicy =
        "fail-closed-by-translation-unit-identity-key";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapRealizationOrderPolicy =
        "constructor-root-then-registration-manifest-order";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapFailureMode =
        "abort-before-user-main-no-partial-registration-commit";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapImageLocalInitializationScope =
        "runtime-owned-image-local-registration-state";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapConstructorRootUniquenessPolicy =
        "one-startup-root-per-translation-unit-identity";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapConsumptionModel =
        "startup-root-consumes-registration-manifest";
inline constexpr const char
    *kObjc3RuntimeStartupBootstrapExecutionMode =
        "deferred-until-next-runtime-phase";
inline constexpr const char *kObjc3RuntimeBootstrapSemanticsContractId =
    "objc3c.runtime.startup.bootstrap.semantics.v1";
inline constexpr const char *kObjc3RuntimeBootstrapSemanticsSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_startup_bootstrap_semantics";
inline constexpr const char *kObjc3RuntimeBootstrapResultModel =
    "zero-success-negative-fail-closed";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationOrderOrdinalModel =
        "strictly-monotonic-positive-registration-order-ordinal";
inline constexpr const char *kObjc3RuntimeBootstrapStateSnapshotSymbol =
    "objc3_runtime_copy_registration_state_for_testing";
inline constexpr const char
    *kObjc3RuntimeBootstrapDuplicateInstallDiagnosticModel =
        "duplicate-install-rejections-publish-the-rejected-module-identity-and-registration-ordinal-without-advancing-runtime-installation-state";
inline constexpr const char
    *kObjc3RuntimeBootstrapOutOfOrderDiagnosticModel =
        "out-of-order-install-rejections-publish-the-rejected-module-identity-and-registration-ordinal-without-advancing-runtime-installation-state";
inline constexpr const char
    *kObjc3RuntimeBootstrapRejectedModuleNameField =
        "last_rejected_module_name";
inline constexpr const char
    *kObjc3RuntimeBootstrapRejectedTranslationUnitIdentityKeyField =
        "last_rejected_translation_unit_identity_key";
inline constexpr const char
    *kObjc3RuntimeBootstrapNextExpectedRegistrationOrderField =
        "next_expected_registration_order_ordinal";
inline constexpr const char
    *kObjc3RuntimeBootstrapLastSuccessfulRegistrationOrderField =
        "last_successful_registration_order_ordinal";
inline constexpr const char
    *kObjc3RuntimeBootstrapLastRejectedRegistrationOrderField =
        "last_rejected_registration_order_ordinal";
inline constexpr int kObjc3RuntimeBootstrapSuccessStatusCode = 0;
inline constexpr int kObjc3RuntimeBootstrapInvalidDescriptorStatusCode = -1;
inline constexpr int
    kObjc3RuntimeBootstrapDuplicateRegistrationStatusCode = -2;
inline constexpr int kObjc3RuntimeBootstrapOutOfOrderStatusCode = -3;
inline constexpr int
    kObjc3RuntimeBootstrapInvalidRegistrationRootsStatusCode = -4;
inline constexpr std::uint64_t
    kObjc3RuntimeBootstrapTranslationUnitRegistrationOrderOrdinal = 1u;
inline constexpr const char *kObjc3RuntimeBootstrapApiContractId =
    "objc3c.runtime.bootstrap.api.freeze.v1";
inline constexpr const char *kObjc3RuntimeBootstrapApiSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_api_contract";
inline constexpr const char *kObjc3RuntimeBootstrapApiStatusEnumType =
    "objc3_runtime_registration_status_code";
inline constexpr const char *kObjc3RuntimeBootstrapApiImageDescriptorType =
    "objc3_runtime_image_descriptor";
inline constexpr const char *kObjc3RuntimeBootstrapApiSelectorHandleType =
    "objc3_runtime_selector_handle";
inline constexpr const char *kObjc3RuntimeBootstrapApiRegistrationSnapshotType =
    "objc3_runtime_registration_state_snapshot";
inline constexpr const char *kObjc3RuntimeBootstrapApiStateLockingModel =
    "process-global-mutex-serialized-runtime-state";
inline constexpr const char *kObjc3RuntimeBootstrapApiStartupInvocationModel =
    "generated-init-stub-calls-runtime-register-image";
inline constexpr const char *kObjc3RuntimeBootstrapApiImageWalkLifecycleModel =
    "deferred-until-next-runtime-phase";
inline constexpr const char
    *kObjc3RuntimeBootstrapApiDeterministicResetLifecycleModel =
        "deferred-until-next-runtime-phase";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationSourceSurfaceContractId =
        "objc3c.runtime.bootstrap.registration.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeBootstrapLoweringRegistrationArtifactSurfaceContractId =
        "objc3c.runtime.bootstrap.lowering.registration.artifact.surface.v1";
inline constexpr const char
    *kObjc3RuntimeMultiImageStartupOrderingSourceSurfaceContractId =
        "objc3c.runtime.multi.image.startup.ordering.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeObjectModelRealizationSourceSurfaceContractId =
        "objc3c.runtime.object.model.realization.source.surface.v1";
inline constexpr const char *kObjc3RuntimeReflectionQuerySurfaceContractId =
    "objc3c.runtime.reflection.query.surface.v1";
inline constexpr const char *kObjc3RuntimeRealizationLookupSemanticsSurfaceContractId =
    "objc3c.runtime.realization.lookup.semantics.v1";
inline constexpr const char *kObjc3RuntimeInstallationAbiSurfaceContractId =
    "objc3c.runtime.installation.abi.surface.v1";
inline constexpr const char *kObjc3RuntimeLoaderLifecycleSurfaceContractId =
    "objc3c.runtime.loader.lifecycle.surface.v1";
inline constexpr const char *kObjc3RuntimeBootstrapRegistrarContractId =
    "objc3c.runtime.bootstrap.registrar.image.walk.v1";
inline constexpr const char *kObjc3RuntimeBootstrapRegistrarSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_registrar_contract";
inline constexpr const char *kObjc3RuntimeBootstrapInternalHeaderPath =
    "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h";
inline constexpr const char
    *kObjc3RuntimeBootstrapStageRegistrationTableSymbol =
        "objc3_runtime_stage_registration_table_for_bootstrap";
inline constexpr const char
    *kObjc3RuntimeBootstrapImageWalkSnapshotSymbol =
        "objc3_runtime_copy_image_walk_state_for_testing";
inline constexpr const char *kObjc3RuntimeBootstrapImageWalkModel =
    "registration-table-roots-validated-and-staged-before-realization";
inline constexpr const char
    *kObjc3RuntimeBootstrapDiscoveryRootValidationModel =
        "linker-anchor-must-point-at-discovery-root-and-discovery-root-must-close-over-registration-roots";
inline constexpr const char
    *kObjc3RuntimeBootstrapSelectorPoolInterningModel =
        "canonical-selector-pool-preinterned-during-startup-image-walk";
inline constexpr const char *kObjc3RuntimeBootstrapRealizationStagingModel =
    "registration-table-roots-retained-for-later-realization";
inline constexpr const char *kObjc3RuntimeBootstrapResetContractId =
    "objc3c.runtime.bootstrap.reset.replay.v1";
inline constexpr const char *kObjc3RuntimeBootstrapResetSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_reset_contract";
inline constexpr const char
    *kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol =
        "objc3_runtime_replay_registered_images_for_testing";
inline constexpr const char
    *kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol =
        "objc3_runtime_copy_reset_replay_state_for_testing";
inline constexpr const char *kObjc3RuntimeInstallationLifecycleProbePath =
    "tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp";
inline constexpr const char *kObjc3RuntimeBootstrapResetLifecycleModel =
    "reset-clears-live-runtime-state-and-zeroes-image-local-init-cells";
inline constexpr const char *kObjc3RuntimeBootstrapReplayOrderModel =
    "replay-re-registers-retained-images-in-original-registration-order";
inline constexpr const char
    *kObjc3RuntimeBootstrapImageLocalInitStateResetModel =
        "retained-bootstrap-image-local-init-cells-reset-to-zero-before-replay";
inline constexpr const char *kObjc3RuntimeBootstrapCatalogRetentionModel =
    "bootstrap-catalog-retained-across-reset-for-deterministic-replay";
inline constexpr const char *kObjc3RuntimeSupportLibraryTargetName =
    "objc3_runtime";
inline constexpr const char *kObjc3RuntimeSupportLibrarySourceRoot =
    "native/objc3c/src/runtime";
inline constexpr const char *kObjc3RuntimeSupportLibraryPublicHeaderPath =
    "native/objc3c/src/runtime/objc3_runtime.h";
inline constexpr const char *kObjc3RuntimeSupportLibraryKind = "static";
inline constexpr const char *kObjc3RuntimeSupportLibraryArchiveBasename =
    "objc3_runtime";
inline constexpr const char *kObjc3RuntimeSupportLibraryArchiveRelativePath =
    "artifacts/lib/objc3_runtime.lib";
inline constexpr const char *kObjc3RuntimeSupportLibraryImplementationSourcePath =
    "native/objc3c/src/runtime/objc3_runtime.cpp";
inline constexpr const char *kObjc3RuntimeSupportLibraryProbeSourcePath =
    "tests/tooling/runtime/runtime_library_probe.cpp";
inline constexpr const char *kObjc3RuntimeSupportLibraryRegisterImageSymbol =
    "objc3_runtime_register_image";
inline constexpr const char *kObjc3RuntimeSupportLibraryLookupSelectorSymbol =
    "objc3_runtime_lookup_selector";
inline constexpr const char *kObjc3RuntimeSupportLibraryDispatchI32Symbol =
    "objc3_runtime_dispatch_i32";
inline constexpr const char *kObjc3RuntimeSupportLibraryResetForTestingSymbol =
    "objc3_runtime_reset_for_testing";
inline constexpr const char *kObjc3RuntimeSupportLibraryDriverLinkMode =
    "not-linked-until-next-runtime-phase";
inline constexpr const char *kObjc3RuntimeSupportLibraryLinkWiringContractId =
    "objc3c.runtime.support.library.link.wiring.v1";
inline constexpr const char
    *kObjc3RuntimeSupportLibraryCompatibilityDispatchSymbol =
        "objc3_msgsend_i32";
inline constexpr const char
    *kObjc3RuntimeSupportLibraryExecutionSmokeScriptPath =
        "scripts/check_objc3c_native_execution_smoke.ps1";
inline constexpr const char *kObjc3RuntimeSupportLibraryLinkWiringMode =
    "emitted-object-links-against-objc3_runtime-lib";
inline constexpr const char
    *kObjc3RuntimeSupportLibraryCompilerOwnershipBoundary =
        "compiler-emits-metadata-runtime-does-not-own-source-records";
inline constexpr const char
    *kObjc3RuntimeSupportLibraryRuntimeOwnershipBoundary =
        "runtime-owns-registration-lookup-and-dispatch-state";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceContractId =
        "objc3c.bootstrap.registration.descriptor.image.root.source.surface.v1";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorImageRootSourceSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_registration_descriptor_image_root_source_surface";
inline constexpr const char
    *kObjc3RuntimeBootstrapModuleIdentitySourceModel =
        "module-declaration-or-default";
inline constexpr const char
    *kObjc3RuntimeBootstrapDerivedIdentitySourcePragma =
        "source-pragma";
inline constexpr const char
    *kObjc3RuntimeBootstrapDerivedIdentitySourceModuleDefault =
        "module-derived-default";
inline constexpr const char
    *kObjc3RuntimeBootstrapVisibleMetadataOwnershipModel =
        "image-root-owns-registration-descriptor-runtime-owns-bootstrap-state";
inline constexpr const char
    *kObjc3RuntimeBootstrapRegistrationDescriptorDefaultSuffix =
        "_registration_descriptor";
inline constexpr const char
    *kObjc3RuntimeBootstrapImageRootDefaultSuffix = "_image_root";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosureContractId =
        "objc3c.runtime.registration.descriptor.frontend.closure.v1";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosureSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_runtime_registration_descriptor_frontend_closure";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosurePayloadModel =
        "runtime-registration-descriptor-json-v1";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosureArtifactSuffix =
        ".runtime-registration-descriptor.json";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendClosureArtifactRelativePath =
        "module.runtime-registration-descriptor.json";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorFrontendAuthorityModel =
        "registration-descriptor-artifact-derived-from-source-surface-and-registration-manifest";
inline constexpr const char
    *kObjc3RuntimeRegistrationDescriptorPayloadOwnershipModel =
        "compiler-emits-registration-descriptor-artifact-runtime-consumes-bootstrap-identity";

enum class Objc3ProtocolRequirementKind {
  NotApplicable,
  Required,
  Optional,
};

struct Objc3MethodDecl {
  struct SelectorPiece {
    std::string keyword;
    std::string parameter_name;
    bool has_parameter = false;
    unsigned line = 1;
    unsigned column = 1;
  };

  std::string selector;
  std::vector<SelectorPiece> selector_pieces;
  bool selector_is_normalized = false;
  std::vector<FuncParam> params;
  ValueType return_type = ValueType::I32;
  bool return_vector_spelling = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_id_spelling = false;
  bool return_class_spelling = false;
  bool return_sel_spelling = false;
  bool return_instancetype_spelling = false;
  bool return_object_pointer_type_spelling = false;
  std::string return_object_pointer_type_name;
  std::string return_typecheck_family_symbol;
  bool has_return_generic_suffix = false;
  bool return_generic_suffix_terminated = true;
  std::string return_generic_suffix_text;
  unsigned return_generic_line = 1;
  unsigned return_generic_column = 1;
  bool return_lightweight_generic_constraint_profile_is_normalized = false;
  std::string return_lightweight_generic_constraint_profile;
  bool return_nullability_flow_profile_is_normalized = false;
  std::string return_nullability_flow_profile;
  bool return_protocol_qualified_object_type_profile_is_normalized = false;
  std::string return_protocol_qualified_object_type_profile;
  bool return_variance_bridge_cast_profile_is_normalized = false;
  std::string return_variance_bridge_cast_profile;
  bool return_generic_metadata_abi_profile_is_normalized = false;
  std::string return_generic_metadata_abi_profile;
  bool return_module_import_graph_profile_is_normalized = false;
  std::string return_module_import_graph_profile;
  bool return_namespace_collision_shadowing_profile_is_normalized = false;
  std::string return_namespace_collision_shadowing_profile;
  bool return_public_private_api_partition_profile_is_normalized = false;
  std::string return_public_private_api_partition_profile;
  bool return_incremental_module_cache_invalidation_profile_is_normalized = false;
  std::string return_incremental_module_cache_invalidation_profile;
  bool return_cross_module_conformance_profile_is_normalized = false;
  std::string return_cross_module_conformance_profile;
  bool has_return_pointer_declarator = false;
  unsigned return_pointer_declarator_depth = 0;
  std::vector<Objc3SemaTokenMetadata> return_pointer_declarator_tokens;
  std::vector<Objc3SemaTokenMetadata> return_nullability_suffix_tokens;
  bool has_return_ownership_qualifier = false;
  std::string return_ownership_qualifier_spelling;
  std::string return_ownership_qualifier_symbol;
  std::vector<Objc3SemaTokenMetadata> return_ownership_qualifier_tokens;
  bool return_ownership_insert_retain = false;
  bool return_ownership_insert_release = false;
  bool return_ownership_insert_autorelease = false;
  std::string return_ownership_operation_profile;
  bool return_ownership_is_weak_reference = false;
  bool return_ownership_is_unowned_reference = false;
  bool return_ownership_is_unowned_safe_reference = false;
  std::string return_ownership_lifetime_profile;
  std::string return_ownership_runtime_hook_profile;
  bool return_ownership_arc_diagnostic_candidate = false;
  bool return_ownership_arc_fixit_available = false;
  std::string return_ownership_arc_diagnostic_profile;
  std::string return_ownership_arc_fixit_hint;
  std::string scope_owner_symbol;
  std::string scope_path_symbol;
  std::string method_lookup_symbol;
  std::string override_lookup_symbol;
  std::string conflict_lookup_symbol;
  bool async_declared = false;
  bool objc_nonisolated_declared = false;
  bool executor_affinity_declared = false;
  bool executor_affinity_named = false;
  std::string executor_affinity_kind;
  std::string executor_affinity_name;
  bool objc_macro_declared = false;
  std::string objc_macro_name;
  bool objc_macro_package_declared = false;
  std::string objc_macro_package_name;
  bool objc_macro_provenance_declared = false;
  std::string objc_macro_provenance_name;
  bool objc_foreign_declared = false;
  bool objc_import_module_declared = false;
  std::string objc_import_module_name;
  bool objc_swift_name_declared = false;
  std::string objc_swift_name;
  bool objc_swift_private_declared = false;
  bool objc_cxx_name_declared = false;
  std::string objc_cxx_name;
  bool objc_header_name_declared = false;
  std::string objc_header_name;
  bool objc_direct_declared = false;
  bool objc_final_declared = false;
  bool objc_dynamic_declared = false;
  bool objc_returns_borrowed_declared = false;
  std::size_t objc_returns_borrowed_owner_index = 0;
  bool return_borrowed_pointer_qualified = false;
  std::string returns_borrowed_profile;
  std::vector<std::string> retainable_c_family_callable_attributes;
  std::vector<std::string> retainable_c_family_names;
  bool retainable_c_family_profile_is_normalized = false;
  std::string retainable_c_family_profile;
  bool throws_declared = false;
  bool throws_declaration_profile_is_normalized = false;
  std::string throws_declaration_profile;
  bool result_like_profile_is_normalized = false;
  bool deterministic_result_like_lowering_handoff = false;
  std::size_t result_like_sites = 0;
  std::size_t result_success_sites = 0;
  std::size_t result_failure_sites = 0;
  std::size_t result_branch_sites = 0;
  std::size_t result_payload_sites = 0;
  std::size_t result_normalized_sites = 0;
  std::size_t result_branch_merge_sites = 0;
  std::size_t result_contract_violation_sites = 0;
  std::string result_like_profile;
  bool ns_error_bridging_profile_is_normalized = false;
  bool deterministic_ns_error_bridging_lowering_handoff = false;
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t ns_error_bridging_normalized_sites = 0;
  std::size_t ns_error_bridge_boundary_sites = 0;
  std::size_t ns_error_bridging_contract_violation_sites = 0;
  std::string ns_error_bridging_profile;
  bool objc_nserror_declared = false;
  bool objc_status_code_declared = false;
  bool error_bridge_marker_profile_is_normalized = false;
  std::size_t objc_nserror_attribute_sites = 0;
  std::size_t objc_status_code_attribute_sites = 0;
  std::size_t status_code_success_clause_sites = 0;
  std::size_t status_code_error_type_clause_sites = 0;
  std::size_t status_code_mapping_clause_sites = 0;
  std::size_t error_bridge_marker_contract_violation_sites = 0;
  std::string objc_status_code_success_literal;
  std::string objc_status_code_error_type_spelling;
  std::string objc_status_code_mapping_symbol;
  std::string error_bridge_marker_profile;
  bool unwind_cleanup_profile_is_normalized = false;
  bool deterministic_unwind_cleanup_handoff = false;
  std::size_t unwind_cleanup_sites = 0;
  std::size_t exceptional_exit_sites = 0;
  std::size_t cleanup_action_sites = 0;
  std::size_t cleanup_scope_sites = 0;
  std::size_t cleanup_resume_sites = 0;
  std::size_t unwind_cleanup_normalized_sites = 0;
  std::size_t unwind_cleanup_fail_closed_sites = 0;
  std::size_t unwind_cleanup_contract_violation_sites = 0;
  std::string unwind_cleanup_profile;
  bool error_diagnostics_recovery_profile_is_normalized = false;
  bool deterministic_error_diagnostics_recovery_handoff = false;
  std::size_t error_diagnostics_recovery_sites = 0;
  std::size_t diagnostic_emit_sites = 0;
  std::size_t recovery_anchor_sites = 0;
  std::size_t recovery_boundary_sites = 0;
  std::size_t fail_closed_diagnostic_sites = 0;
  std::size_t error_diagnostics_recovery_normalized_sites = 0;
  std::size_t error_diagnostics_recovery_gate_blocked_sites = 0;
  std::size_t error_diagnostics_recovery_contract_violation_sites = 0;
  std::string error_diagnostics_recovery_profile;
  bool async_continuation_profile_is_normalized = false;
  bool deterministic_async_continuation_handoff = false;
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t async_continuation_normalized_sites = 0;
  std::size_t async_continuation_gate_blocked_sites = 0;
  std::size_t async_continuation_contract_violation_sites = 0;
  std::string async_continuation_profile;
  bool await_suspension_profile_is_normalized = false;
  bool deterministic_await_suspension_handoff = false;
  std::size_t await_suspension_sites = 0;
  std::size_t await_keyword_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
  std::size_t await_suspension_normalized_sites = 0;
  std::size_t await_suspension_gate_blocked_sites = 0;
  std::size_t await_suspension_contract_violation_sites = 0;
  std::string await_suspension_profile;
  bool actor_isolation_sendability_profile_is_normalized = false;
  bool deterministic_actor_isolation_sendability_handoff = false;
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t actor_isolation_sendability_normalized_sites = 0;
  std::size_t actor_isolation_sendability_gate_blocked_sites = 0;
  std::size_t actor_isolation_sendability_contract_violation_sites = 0;
  std::string actor_isolation_sendability_profile;
  bool task_runtime_cancellation_profile_is_normalized = false;
  bool deterministic_task_runtime_cancellation_handoff = false;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t task_runtime_normalized_sites = 0;
  std::size_t task_runtime_gate_blocked_sites = 0;
  std::size_t task_runtime_contract_violation_sites = 0;
  std::size_t task_runtime_cancellation_normalized_sites = 0;
  std::size_t task_runtime_cancellation_gate_blocked_sites = 0;
  std::size_t task_runtime_cancellation_contract_violation_sites = 0;
  std::string task_runtime_cancellation_profile;
  bool concurrency_replay_race_guard_profile_is_normalized = false;
  bool deterministic_concurrency_replay_race_guard_handoff = false;
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t concurrency_replay_guard_blocked_sites = 0;
  std::size_t concurrency_replay_contract_violation_sites = 0;
  std::string concurrency_replay_race_guard_profile;
  bool unsafe_pointer_extension_profile_is_normalized = false;
  bool deterministic_unsafe_pointer_extension_handoff = false;
  std::size_t unsafe_pointer_extension_sites = 0;
  std::size_t unsafe_keyword_sites = 0;
  std::size_t pointer_arithmetic_sites = 0;
  std::size_t raw_pointer_type_sites = 0;
  std::size_t unsafe_operation_sites = 0;
  std::size_t unsafe_pointer_extension_normalized_sites = 0;
  std::size_t unsafe_pointer_extension_gate_blocked_sites = 0;
  std::size_t unsafe_pointer_extension_contract_violation_sites = 0;
  std::string unsafe_pointer_extension_profile;
  bool inline_asm_intrinsic_governance_profile_is_normalized = false;
  bool deterministic_inline_asm_intrinsic_governance_handoff = false;
  std::size_t inline_asm_intrinsic_sites = 0;
  std::size_t inline_asm_sites = 0;
  std::size_t intrinsic_sites = 0;
  std::size_t governed_intrinsic_sites = 0;
  std::size_t privileged_intrinsic_sites = 0;
  std::size_t inline_asm_intrinsic_normalized_sites = 0;
  std::size_t inline_asm_intrinsic_gate_blocked_sites = 0;
  std::size_t inline_asm_intrinsic_contract_violation_sites = 0;
  std::string inline_asm_intrinsic_governance_profile;
  Objc3ProtocolRequirementKind protocol_requirement_kind =
      Objc3ProtocolRequirementKind::NotApplicable;
  bool is_class_method = false;
  bool has_body = false;
  std::vector<std::unique_ptr<Stmt>> body;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3PropertyAttributeDecl {
  std::string name;
  std::string value;
  bool has_value = false;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3PropertyDecl {
  std::string name;
  ValueType type = ValueType::Unknown;
  bool vector_spelling = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool id_spelling = false;
  bool class_spelling = false;
  bool sel_spelling = false;
  bool instancetype_spelling = false;
  bool object_pointer_type_spelling = false;
  std::string object_pointer_type_name;
  std::string typecheck_family_symbol;
  bool has_generic_suffix = false;
  bool generic_suffix_terminated = true;
  std::string generic_suffix_text;
  unsigned generic_line = 1;
  unsigned generic_column = 1;
  bool lightweight_generic_constraint_profile_is_normalized = false;
  std::string lightweight_generic_constraint_profile;
  bool nullability_flow_profile_is_normalized = false;
  std::string nullability_flow_profile;
  bool protocol_qualified_object_type_profile_is_normalized = false;
  std::string protocol_qualified_object_type_profile;
  bool variance_bridge_cast_profile_is_normalized = false;
  std::string variance_bridge_cast_profile;
  bool generic_metadata_abi_profile_is_normalized = false;
  std::string generic_metadata_abi_profile;
  bool module_import_graph_profile_is_normalized = false;
  std::string module_import_graph_profile;
  bool namespace_collision_shadowing_profile_is_normalized = false;
  std::string namespace_collision_shadowing_profile;
  bool public_private_api_partition_profile_is_normalized = false;
  std::string public_private_api_partition_profile;
  bool incremental_module_cache_invalidation_profile_is_normalized = false;
  std::string incremental_module_cache_invalidation_profile;
  bool cross_module_conformance_profile_is_normalized = false;
  std::string cross_module_conformance_profile;
  bool has_pointer_declarator = false;
  unsigned pointer_declarator_depth = 0;
  std::vector<Objc3SemaTokenMetadata> pointer_declarator_tokens;
  std::vector<Objc3SemaTokenMetadata> nullability_suffix_tokens;
  bool has_ownership_qualifier = false;
  std::string ownership_qualifier_spelling;
  std::string ownership_qualifier_symbol;
  std::vector<Objc3SemaTokenMetadata> ownership_qualifier_tokens;
  bool ownership_insert_retain = false;
  bool ownership_insert_release = false;
  bool ownership_insert_autorelease = false;
  std::string ownership_operation_profile;
  std::vector<Objc3PropertyAttributeDecl> attributes;
  bool is_readonly = false;
  bool is_readwrite = false;
  bool is_atomic = false;
  bool is_nonatomic = false;
  bool is_copy = false;
  bool is_strong = false;
  bool is_weak = false;
  bool is_unowned = false;
  bool is_assign = false;
  bool has_weak_unowned_conflict = false;
  bool ownership_is_weak_reference = false;
  bool ownership_is_unowned_reference = false;
  bool ownership_is_unowned_safe_reference = false;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  bool ownership_arc_diagnostic_candidate = false;
  bool ownership_arc_fixit_available = false;
  std::string ownership_arc_diagnostic_profile;
  std::string ownership_arc_fixit_hint;
  bool has_getter = false;
  bool has_setter = false;
  std::string getter_selector;
  std::string setter_selector;
  Objc3ProtocolRequirementKind protocol_requirement_kind =
      Objc3ProtocolRequirementKind::NotApplicable;
  std::string scope_owner_symbol;
  std::string scope_path_symbol;
  std::string property_synthesis_symbol;
  std::string ivar_binding_symbol;
  // synthesized accessor/property lowering anchor: lane-C consumes
  // the effective accessor selectors plus synthesized binding identity below
  // to materialize executable getter/setter bodies without reopening property
  // parsing or sema ownership/layout derivation.
  // runtime property/layout consumption freeze anchor: lane-D must
  // consume the same emitted binding and layout identities below rather than
  // rederiving property storage or allocator state from source.
  // instance-allocation-layout-runtime anchor: the same emitted
  // binding and layout identities must also be sufficient for true
  // per-instance allocation and slot storage without rederiving layout from
  // source.
  // property-metadata-reflection anchor: private runtime reflection
  // helpers must likewise surface property/accessor/layout facts from these
  // same emitted identities rather than rediscovering them from source.
  // property-ivar-execution gate anchor: lane-E freezes the
  // executable claim over these same binding, accessor, and layout identities
  // before broader runnable sample expansion is allowed.
  // runnable property-ivar execution-matrix anchor: these same
  // emitted identities must now survive one live integrated storage,
  // synthesized-accessor, and reflection proof.
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string property_attribute_profile;
  bool property_behavior_declared = false;
  std::string property_behavior_name;
  std::string effective_getter_selector;
  bool effective_setter_available = false;
  std::string effective_setter_selector;
  std::string accessor_ownership_profile;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ProtocolDecl {
  std::string name;
  std::string scope_owner_symbol;
  std::vector<std::string> scope_path_lexicographic;
  std::vector<std::string> inherited_protocols;
  std::vector<std::string> inherited_protocols_lexicographic;
  std::string semantic_link_symbol;
  std::vector<std::string> method_lookup_symbols_lexicographic;
  std::vector<std::string> override_lookup_symbols_lexicographic;
  std::vector<std::string> conflict_lookup_symbols_lexicographic;
  std::vector<Objc3PropertyDecl> properties;
  std::vector<Objc3MethodDecl> methods;
  bool is_forward_declaration = false;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3InterfaceDecl {
  std::string name;
  std::string super_name;
  std::string category_name;
  bool has_category = false;
  bool is_actor = false;
  std::string scope_owner_symbol;
  std::vector<std::string> scope_path_lexicographic;
  std::vector<std::string> adopted_protocols;
  std::vector<std::string> adopted_protocols_lexicographic;
  std::string semantic_link_symbol;
  std::string semantic_link_super_symbol;
  std::string semantic_link_category_symbol;
  std::vector<std::string> property_synthesis_symbols_lexicographic;
  std::vector<std::string> ivar_binding_symbols_lexicographic;
  std::vector<std::string> method_lookup_symbols_lexicographic;
  std::vector<std::string> override_lookup_symbols_lexicographic;
  std::vector<std::string> conflict_lookup_symbols_lexicographic;
  bool prefixed_dispatch_control_attributes_declared = false;
  bool objc_direct_members_declared = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  bool objc_derive_declared = false;
  std::string objc_derive_name;
  std::vector<Objc3PropertyDecl> properties;
  std::vector<Objc3MethodDecl> methods;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ImplementationDecl {
  std::string name;
  std::string category_name;
  bool has_category = false;
  std::string scope_owner_symbol;
  std::vector<std::string> scope_path_lexicographic;
  std::string semantic_link_symbol;
  std::string semantic_link_interface_symbol;
  std::string semantic_link_category_symbol;
  std::vector<std::string> property_synthesis_symbols_lexicographic;
  std::vector<std::string> ivar_binding_symbols_lexicographic;
  std::vector<std::string> method_lookup_symbols_lexicographic;
  std::vector<std::string> override_lookup_symbols_lexicographic;
  std::vector<std::string> conflict_lookup_symbols_lexicographic;
  std::vector<Objc3PropertyDecl> properties;
  std::vector<Objc3MethodDecl> methods;
  unsigned line = 1;
  unsigned column = 1;
};

struct FunctionDecl {
  std::string name;
  std::string scope_owner_symbol;
  std::vector<std::string> scope_path_lexicographic;
  std::vector<FuncParam> params;
  ValueType return_type = ValueType::I32;
  bool return_vector_spelling = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_id_spelling = false;
  bool return_class_spelling = false;
  bool return_sel_spelling = false;
  bool return_instancetype_spelling = false;
  bool return_object_pointer_type_spelling = false;
  std::string return_object_pointer_type_name;
  std::string return_typecheck_family_symbol;
  bool has_return_generic_suffix = false;
  bool return_generic_suffix_terminated = true;
  std::string return_generic_suffix_text;
  unsigned return_generic_line = 1;
  unsigned return_generic_column = 1;
  bool return_lightweight_generic_constraint_profile_is_normalized = false;
  std::string return_lightweight_generic_constraint_profile;
  bool return_nullability_flow_profile_is_normalized = false;
  std::string return_nullability_flow_profile;
  bool return_protocol_qualified_object_type_profile_is_normalized = false;
  std::string return_protocol_qualified_object_type_profile;
  bool return_variance_bridge_cast_profile_is_normalized = false;
  std::string return_variance_bridge_cast_profile;
  bool return_generic_metadata_abi_profile_is_normalized = false;
  std::string return_generic_metadata_abi_profile;
  bool return_module_import_graph_profile_is_normalized = false;
  std::string return_module_import_graph_profile;
  bool return_namespace_collision_shadowing_profile_is_normalized = false;
  std::string return_namespace_collision_shadowing_profile;
  bool return_public_private_api_partition_profile_is_normalized = false;
  std::string return_public_private_api_partition_profile;
  bool return_incremental_module_cache_invalidation_profile_is_normalized = false;
  std::string return_incremental_module_cache_invalidation_profile;
  bool return_cross_module_conformance_profile_is_normalized = false;
  std::string return_cross_module_conformance_profile;
  bool has_return_pointer_declarator = false;
  unsigned return_pointer_declarator_depth = 0;
  std::vector<Objc3SemaTokenMetadata> return_pointer_declarator_tokens;
  std::vector<Objc3SemaTokenMetadata> return_nullability_suffix_tokens;
  bool has_return_ownership_qualifier = false;
  std::string return_ownership_qualifier_spelling;
  std::string return_ownership_qualifier_symbol;
  std::vector<Objc3SemaTokenMetadata> return_ownership_qualifier_tokens;
  bool return_ownership_insert_retain = false;
  bool return_ownership_insert_release = false;
  bool return_ownership_insert_autorelease = false;
  std::string return_ownership_operation_profile;
  bool return_ownership_is_weak_reference = false;
  bool return_ownership_is_unowned_reference = false;
  bool return_ownership_is_unowned_safe_reference = false;
  std::string return_ownership_lifetime_profile;
  std::string return_ownership_runtime_hook_profile;
  bool return_ownership_arc_diagnostic_candidate = false;
  bool return_ownership_arc_fixit_available = false;
  std::string return_ownership_arc_diagnostic_profile;
  std::string return_ownership_arc_fixit_hint;
  bool async_declared = false;
  bool objc_nonisolated_declared = false;
  bool executor_affinity_declared = false;
  bool executor_affinity_named = false;
  std::string executor_affinity_kind;
  std::string executor_affinity_name;
  bool objc_macro_declared = false;
  std::string objc_macro_name;
  bool objc_macro_package_declared = false;
  std::string objc_macro_package_name;
  bool objc_macro_provenance_declared = false;
  std::string objc_macro_provenance_name;
  bool objc_foreign_declared = false;
  bool objc_import_module_declared = false;
  std::string objc_import_module_name;
  bool objc_swift_name_declared = false;
  std::string objc_swift_name;
  bool objc_swift_private_declared = false;
  bool objc_cxx_name_declared = false;
  std::string objc_cxx_name;
  bool objc_header_name_declared = false;
  std::string objc_header_name;
  bool objc_direct_declared = false;
  bool objc_final_declared = false;
  bool objc_dynamic_declared = false;
  bool objc_returns_borrowed_declared = false;
  std::size_t objc_returns_borrowed_owner_index = 0;
  bool return_borrowed_pointer_qualified = false;
  std::string returns_borrowed_profile;
  std::vector<std::string> retainable_c_family_callable_attributes;
  std::vector<std::string> retainable_c_family_names;
  bool retainable_c_family_profile_is_normalized = false;
  std::string retainable_c_family_profile;
  bool throws_declared = false;
  bool throws_declaration_profile_is_normalized = false;
  std::string throws_declaration_profile;
  bool result_like_profile_is_normalized = false;
  bool deterministic_result_like_lowering_handoff = false;
  std::size_t result_like_sites = 0;
  std::size_t result_success_sites = 0;
  std::size_t result_failure_sites = 0;
  std::size_t result_branch_sites = 0;
  std::size_t result_payload_sites = 0;
  std::size_t result_normalized_sites = 0;
  std::size_t result_branch_merge_sites = 0;
  std::size_t result_contract_violation_sites = 0;
  std::string result_like_profile;
  bool ns_error_bridging_profile_is_normalized = false;
  bool deterministic_ns_error_bridging_lowering_handoff = false;
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t ns_error_bridging_normalized_sites = 0;
  std::size_t ns_error_bridge_boundary_sites = 0;
  std::size_t ns_error_bridging_contract_violation_sites = 0;
  std::string ns_error_bridging_profile;
  bool objc_nserror_declared = false;
  bool objc_status_code_declared = false;
  bool error_bridge_marker_profile_is_normalized = false;
  std::size_t objc_nserror_attribute_sites = 0;
  std::size_t objc_status_code_attribute_sites = 0;
  std::size_t status_code_success_clause_sites = 0;
  std::size_t status_code_error_type_clause_sites = 0;
  std::size_t status_code_mapping_clause_sites = 0;
  std::size_t error_bridge_marker_contract_violation_sites = 0;
  std::string objc_status_code_success_literal;
  std::string objc_status_code_error_type_spelling;
  std::string objc_status_code_mapping_symbol;
  std::string error_bridge_marker_profile;
  bool unwind_cleanup_profile_is_normalized = false;
  bool deterministic_unwind_cleanup_handoff = false;
  std::size_t unwind_cleanup_sites = 0;
  std::size_t exceptional_exit_sites = 0;
  std::size_t cleanup_action_sites = 0;
  std::size_t cleanup_scope_sites = 0;
  std::size_t cleanup_resume_sites = 0;
  std::size_t unwind_cleanup_normalized_sites = 0;
  std::size_t unwind_cleanup_fail_closed_sites = 0;
  std::size_t unwind_cleanup_contract_violation_sites = 0;
  std::string unwind_cleanup_profile;
  bool error_diagnostics_recovery_profile_is_normalized = false;
  bool deterministic_error_diagnostics_recovery_handoff = false;
  std::size_t error_diagnostics_recovery_sites = 0;
  std::size_t diagnostic_emit_sites = 0;
  std::size_t recovery_anchor_sites = 0;
  std::size_t recovery_boundary_sites = 0;
  std::size_t fail_closed_diagnostic_sites = 0;
  std::size_t error_diagnostics_recovery_normalized_sites = 0;
  std::size_t error_diagnostics_recovery_gate_blocked_sites = 0;
  std::size_t error_diagnostics_recovery_contract_violation_sites = 0;
  std::string error_diagnostics_recovery_profile;
  bool async_continuation_profile_is_normalized = false;
  bool deterministic_async_continuation_handoff = false;
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t async_continuation_normalized_sites = 0;
  std::size_t async_continuation_gate_blocked_sites = 0;
  std::size_t async_continuation_contract_violation_sites = 0;
  std::string async_continuation_profile;
  bool await_suspension_profile_is_normalized = false;
  bool deterministic_await_suspension_handoff = false;
  std::size_t await_suspension_sites = 0;
  std::size_t await_keyword_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
  std::size_t await_suspension_normalized_sites = 0;
  std::size_t await_suspension_gate_blocked_sites = 0;
  std::size_t await_suspension_contract_violation_sites = 0;
  std::string await_suspension_profile;
  bool actor_isolation_sendability_profile_is_normalized = false;
  bool deterministic_actor_isolation_sendability_handoff = false;
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t actor_isolation_sendability_normalized_sites = 0;
  std::size_t actor_isolation_sendability_gate_blocked_sites = 0;
  std::size_t actor_isolation_sendability_contract_violation_sites = 0;
  std::string actor_isolation_sendability_profile;
  bool task_runtime_cancellation_profile_is_normalized = false;
  bool deterministic_task_runtime_cancellation_handoff = false;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t task_runtime_normalized_sites = 0;
  std::size_t task_runtime_gate_blocked_sites = 0;
  std::size_t task_runtime_contract_violation_sites = 0;
  std::size_t task_runtime_cancellation_normalized_sites = 0;
  std::size_t task_runtime_cancellation_gate_blocked_sites = 0;
  std::size_t task_runtime_cancellation_contract_violation_sites = 0;
  std::string task_runtime_cancellation_profile;
  bool concurrency_replay_race_guard_profile_is_normalized = false;
  bool deterministic_concurrency_replay_race_guard_handoff = false;
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t concurrency_replay_guard_blocked_sites = 0;
  std::size_t concurrency_replay_contract_violation_sites = 0;
  std::string concurrency_replay_race_guard_profile;
  bool unsafe_pointer_extension_profile_is_normalized = false;
  bool deterministic_unsafe_pointer_extension_handoff = false;
  std::size_t unsafe_pointer_extension_sites = 0;
  std::size_t unsafe_keyword_sites = 0;
  std::size_t pointer_arithmetic_sites = 0;
  std::size_t raw_pointer_type_sites = 0;
  std::size_t unsafe_operation_sites = 0;
  std::size_t unsafe_pointer_extension_normalized_sites = 0;
  std::size_t unsafe_pointer_extension_gate_blocked_sites = 0;
  std::size_t unsafe_pointer_extension_contract_violation_sites = 0;
  std::string unsafe_pointer_extension_profile;
  bool inline_asm_intrinsic_governance_profile_is_normalized = false;
  bool deterministic_inline_asm_intrinsic_governance_handoff = false;
  std::size_t inline_asm_intrinsic_sites = 0;
  std::size_t inline_asm_sites = 0;
  std::size_t intrinsic_sites = 0;
  std::size_t governed_intrinsic_sites = 0;
  std::size_t privileged_intrinsic_sites = 0;
  std::size_t inline_asm_intrinsic_normalized_sites = 0;
  std::size_t inline_asm_intrinsic_gate_blocked_sites = 0;
  std::size_t inline_asm_intrinsic_contract_violation_sites = 0;
  std::string inline_asm_intrinsic_governance_profile;
  bool is_prototype = false;
  bool is_pure = false;
  std::vector<std::unique_ptr<Stmt>> body;
  unsigned line = 1;
  unsigned column = 1;
};

struct GlobalDecl {
  std::string name;
  std::string scope_owner_symbol;
  std::vector<std::string> scope_path_lexicographic;
  std::string semantic_link_symbol;
  std::unique_ptr<Expr> value;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3Program {
  std::string module_name = "objc3_module";
  std::vector<GlobalDecl> globals;
  std::vector<Objc3ProtocolDecl> protocols;
  std::vector<Objc3InterfaceDecl> interfaces;
  std::vector<Objc3ImplementationDecl> implementations;
  std::vector<FunctionDecl> functions;
  std::vector<std::string> diagnostics;
};

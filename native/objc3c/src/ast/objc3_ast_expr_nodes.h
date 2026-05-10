#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "ast/objc3_ast_value_type.h"

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
      "source-only-sema-rejects-escaping-byref-and-owned-object-captures-before-runnable-block-ownership-lowering";
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
      "native-lowering-emits-stack-byref-cells-and-runtime-copy-dispose-helper-bodies-for-nonescaping-and-escaping-block-captures";
  static inline constexpr const char *kObjc3ExecutableBlockByrefHelperLoweringDeferredModel =
      "public-block-object-abi-widening-and-generalized-foreign-block-interop-remain-deferred-to-later-runtime-work";
  static inline constexpr const char *kObjc3ExecutableBlockByrefHelperLoweringExecutionEvidenceModel =
      "native-compile-link-run-proves-byref-mutation-owned-capture-and-escaping-block-helper-lowering-through-emitted-block-helper-bodies";
  // escaping-block runtime-hook constants: lane-C widens runnable
  // block lowering to readonly-scalar escaping cases by lowering heap
  // promotion and invoke hooks against the native runtime while ownership-
  // sensitive escaping captures stay deferred to lane-D runtime work.
  static inline constexpr const char *kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId =
      "objc3c.executable.block.escape.runtime.hook.lowering.v1";
  static inline constexpr const char *kObjc3ExecutableBlockEscapeRuntimeHookLoweringActiveModel =
      "native-lowering-emits-runtime-block-promotion-invoke-byref-forwarding-and-owned-capture-hooks-for-escaping-block-values";
  static inline constexpr const char *kObjc3ExecutableBlockEscapeRuntimeHookLoweringDeferredModel =
      "public-block-object-abi-widening-and-generalized-foreign-block-interop-remain-deferred-to-later-runtime-work";
  static inline constexpr const char *kObjc3ExecutableBlockEscapeRuntimeHookLoweringExecutionEvidenceModel =
      "native-compile-link-run-proves-returned-argument-passed-byref-and-owned-capture-escaping-block-values-through-runtime-promotion-hooks";
  // runnable-block-runtime gate constants: lane-E now freezes the
  // truthful integrated block-runtime proof boundary above the retained source,
  // sema, lowering, and runtime evidence chain before broader block closeout
  // samples expand it.
  static inline constexpr const char *kObjc3RunnableBlockRuntimeGateContractId =
      "objc3c.runnable.block.runtime.gate.v1";
  static inline constexpr const char *kObjc3RunnableBlockRuntimeGateEvidenceModel =
      "source-sema-lowering-runtime-summary-chain";
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
      "source-sema-lowering-runtime-integrated-native-block-smoke-matrix";
  static inline constexpr const char *kObjc3RunnableBlockExecutionMatrixActiveModel =
      "closeout-matrix-runs-owned-nonowning-byref-and-escaping-block-fixtures-against-the-native-runtime";
  static inline constexpr const char *kObjc3RunnableBlockExecutionMatrixNonGoalModel =
      "no-public-block-object-abi-no-public-runtime-helper-header-no-generalized-foreign-block-interop-no-caller-frame-forwarding-bridge";
  static inline constexpr const char *kObjc3RunnableBlockExecutionMatrixFailClosedModel =
      "fail-closed-on-runnable-block-execution-matrix-drift-or-doc-mismatch";
  // ARC source-surface/mode-boundary constants: ownership
  // qualifiers, weak/unowned metadata, autoreleasepool profiling, and ARC
  // fix-it summaries are already parser/sema-visible, and the native driver
  // now admits the bounded helper-backed `-fobjc-arc` slice proven by the
  // executable ARC runtime probes.
  static inline constexpr const char *kObjc3ArcSourceModeBoundaryContractId =
      "objc3c.arc.source.mode.boundary.freeze.v1";
  static inline constexpr const char *kObjc3ArcSourceModeBoundarySourceModel =
      "ownership-qualifier-weak-unowned-autoreleasepool-and-arc-fixit-source-surfaces-remain-live-without-enabling-runnable-arc-mode";
  static inline constexpr const char *kObjc3ArcSourceModeBoundaryModeModel =
      "native-driver-admits-fobjc-arc-and-fno-objc-arc-while-runnable-arc-stays-bounded-to-the-helper-backed-supported-slice";
  static inline constexpr const char *kObjc3ArcSourceModeBoundaryNonGoalModel =
      "no-generalized-arc-cleanup-insertion-no-public-arc-runtime-abi-mode-split-no-full-arc-automation-beyond-the-supported-helper-backed-slice";
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
      "no-implicit-nonarc-promotion-no-cross-module-arc-mode-inference-no-full-arc-automation-beyond-the-supported-helper-backed-slice";
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
  // Canonical extraction anchor retained for strict parser/AST contract tests:
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
  bool runtime_link_host_link_required = true;
  bool runtime_link_host_link_elided = false;
  unsigned runtime_link_host_link_declaration_parameter_count = 0;
  std::string runtime_dispatch_bridge_symbol;
  std::string runtime_link_host_link_symbol;
  bool runtime_link_host_link_is_normalized = false;
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
  std::size_t block_explicit_capture_byref_count = 0;
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

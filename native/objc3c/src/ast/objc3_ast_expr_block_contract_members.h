#pragma once

#ifndef OBJC3_AST_EXPR_NODE_MEMBERS_IN_EXPR
#error "objc3_ast_expr_block_contract_members.h must be included inside struct Expr"
#endif

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

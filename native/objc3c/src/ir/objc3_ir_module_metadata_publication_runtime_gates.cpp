#include "ir/objc3_ir_module_metadata_publication_runtime_gates.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRModuleMetadataRuntimeSemanticsGatePublication(
    const Objc3IRFrontendMetadata &frontend_metadata_,
    std::ostringstream &out) {
  // ownership-runtime-gate freeze anchor: lane-E freezes the
  // supported ownership runtime slice and its explicit non-goals as a
  // dedicated emitted boundary so later smoke/docs work can validate the
  // correct baseline without rediscovering it from prose alone.
  // ownership-smoke closeout anchor: the runnable smoke matrix
  // consumes this exact emitted gate boundary as the integrated ownership
  // proof surface for closeout.
  out << "; ownership_runtime_gate = "
      << Objc3OwnershipRuntimeGateSummary() << "\n";
  // executable-block-source-closure anchor: emit the truthful
  // parser/AST boundary so lane-A can prove block literals entered the source
  // closure before runtime lowering remains fail closed.
  out << "; executable_block_source_closure = "
      << Objc3ExecutableBlockSourceClosureSummary() << "\n";
  // block-source-model-completion anchor: emit the completed
  // source-model replay boundary so source-only frontend runs and later
  // lowering work can agree on typed parameter signatures, capture storage
  // inventories, and invoke-surface symbols without reconstructing them.
  out << "; executable_block_source_model_completion = "
      << Objc3ExecutableBlockSourceModelCompletionSummary()
      << ";replay_key="
      << frontend_metadata_.lowering_block_source_model_completion_replay_key
      << "\n";
  // block-source-storage-annotation anchor: emit the truthful
  // byref/helper/escape-shape replay boundary so source-only manifests and
  // later runnable block lowering consume the same deterministic source
  // annotations while native block execution remains fail closed.
  out << "; executable_block_source_storage_annotations = "
      << Objc3ExecutableBlockSourceStorageAnnotationSummary()
      << ";replay_key="
      << frontend_metadata_
             .lowering_block_source_storage_annotation_replay_key
      << "\n";
  // block-runtime-semantic-rules freeze anchor: emit the current
  // semantic-rule boundary so block runtime follow-on work can preserve the
  // source-only admission/native fail-closed split without rediscovering it
  // from prose or historical issue packets.
  // capture-legality/escape/invocation implementation anchor:
  // the emitted summary line stays on the frozen runtime boundary while the
  // source-only sema path now enforces live capture legality and local
  // block-call typing ahead of runnable block-object lowering.
  // byref/copy-dispose/object-ownership anchor: helper-eligibility
  // totals in this emitted surface now reflect owned object captures as well as byref cells,
  // while native block execution still remains fail-closed.
  out << "; executable_block_runtime_semantic_rules = "
      << Objc3ExecutableBlockRuntimeSemanticRulesSummary() << "\n";
  // block-lowering-ABI/artifact-boundary freeze anchor: lane-C
  // now publishes the truthful lowering boundary required for runnable block
  // execution, while native emit still fails closed before any emitted block
  // object records, invoke thunks, byref cells, or helper bodies land.
  out << "; executable_block_lowering_abi_artifact_boundary = "
      << Objc3ExecutableBlockLoweringAbiArtifactBoundarySummary() << "\n";
  // executable-block-object/invoke-thunk anchor: native lowering
  // now emits stack block objects plus internal invoke thunks for the narrow
  // readonly-scalar capture slice, while byref/helper/ownership-sensitive
  // cases remain explicitly deferred to C003.
  out << "; executable_block_object_invoke_thunk_lowering = "
      << Objc3ExecutableBlockObjectInvokeThunkLoweringSummary() << "\n";
  // byref-cell/copy-helper/dispose-helper anchor: native lowering
  // now widens the runnable block slice to non-escaping byref and owned
  // capture cases through stack helper emission and helper call sites, while
  // escaping heap promotion remains deferred to later work.
  out << "; executable_block_byref_helper_lowering = "
      << Objc3ExecutableBlockByrefHelperLoweringSummary() << "\n";
  // escaping-block runtime-hook anchor: lane-C now publishes the
  // escaping readonly-scalar block slice that lowers through runtime
  // promotion/invoke hooks while pointer-managed escaping captures remain
  // explicitly deferred to later runtime issues.
  out << "; executable_block_escape_runtime_hook_lowering = "
      << Objc3ExecutableBlockEscapeRuntimeHookLoweringSummary() << "\n";
  // block-runtime API/object-layout freeze anchor: emitted IR now
  // republishes the current private helper ABI and private runtime layout
  // boundary so later runtime implementation issues preserve this exact
  // contract instead of widening it ad hoc.
  out << "; runtime_block_api_object_layout = "
      << Objc3RuntimeBlockApiObjectLayoutSummary() << "\n";
  // block-runtime allocation/copy-dispose/invoke anchor: emitted
  // IR now republishes the live runtime capability boundary for promoted
  // block records with helper-mediated copy/dispose support while byref and
  // ownership-interoperating escape paths remain deferred.
  out << "; runtime_block_allocation_copy_dispose_invoke_support = "
      << Objc3RuntimeBlockAllocationCopyDisposeInvokeSupportSummary() << "\n";
  // byref-forwarding/heap-promotion/ownership-interop anchor:
  // emitted IR now republishes that escaping pointer-capture block handles
  // rewrite capture slots onto runtime-owned forwarding cells before helper
  // execution, keeping byref mutation and owned capture lifetimes live after
  // the source frame returns.
  out << "; runtime_block_byref_forwarding_heap_promotion_ownership_interop = "
      << Objc3RuntimeBlockByrefForwardingHeapPromotionInteropSummary() << "\n";
  // runnable-block-runtime gate anchor: lane-E now freezes the
  // integrated block-runtime gate above the retained A003/B003/C004/D003
  // source, sema, lowering, and runtime proofs so later closeout work
  // cannot substitute metadata-only evidence.
  out << "; runnable_block_runtime_gate = "
      << Objc3RunnableBlockRuntimeGateSummary() << "\n";
  // runnable-block execution-matrix anchor: lane-E now closes the
  // current current slice by requiring integrated executable block probes above
  // the retained gate, without widening the public block ABI or helper
  // surface.
  out << "; runnable_block_execution_matrix = "
      << Objc3RunnableBlockExecutionMatrixSummary() << "\n";
  // ARC source-surface/mode-boundary anchor: emit the truthful
  // ARC-adjacent frontend/mode boundary so later ARC automation work cannot
  // silently claim a runnable `-fobjc-arc` mode before the driver and
  // executable ownership-qualified function/method path are actually live.
  out << "; arc_source_mode_boundary = "
      << Objc3ArcSourceModeBoundarySummary() << "\n";
  // ARC mode-handling core implementation anchor: publish the
  // explicit ARC-mode execution boundary so manifests and emitted IR stay
  // aligned on when ownership-qualified executable signatures are runnable.
  out << "; arc_mode_handling = "
      << Objc3ArcModeHandlingSummary(frontend_metadata_.arc_mode_enabled) << "\n";
  // ARC semantic-rule freeze anchor: publish the semantic
  // fail-closed boundary for property conflicts and deferred inference so IR
  // evidence stays aligned with semantic validation.
  out << "; arc_semantic_rules = "
      << Objc3ArcSemanticRulesSummary() << "\n";
  // ARC inference/lifetime implementation anchor: publish the
  // truthful semantic-upgrade boundary so emitted IR and manifests agree
  // when ARC mode has widened the supported slice from explicit-only
  // ownership spelling into inferred strong-owned retain/release activity.
  out << "; arc_inference_lifetime = "
      << Objc3ArcInferenceLifetimeSummary() << "\n";
  // ARC interaction-semantics expansion anchor: publish the
  // supported weak/autorelease-return/property-synthesis/block-interaction
  // semantic boundary so emitted IR stays aligned with the live ARC slice
  // instead of forcing later issues to reconstruct it out of older packets.
  out << "; arc_interaction_semantics = "
      << Objc3ArcInteractionSemanticsSummary() << "\n";
  // ARC lowering ABI/cleanup freeze anchor: publish the current
  // lowering/helper boundary directly into IR so later retain/release,
  // cleanup-scheduling, weak lowering, and autorelease-return work must
  // preserve one explicit contract instead of inferring it from older ARC
  // semantic packets.
  out << "; arc_lowering_abi_cleanup_model = "
      << Objc3ArcLoweringAbiCleanupModelSummary() << "\n";
  // ARC automatic-insertion implementation anchor: publish the
  // live param/return helper-insertion boundary so later ARC work extends a
  // real lowering surface instead of a summary-only semantic contract.
  out << "; arc_automatic_insertions = "
      << Objc3ArcAutomaticInsertionSummary() << "\n";
  // ARC cleanup/weak/lifetime implementation anchor: publish the
  // supported scope-exit cleanup, weak current-property helper, and block
  // lifetime cleanup boundary so later ARC/block widening extends a real
  // lowering/runtime surface rather than inferred behavior.
  out << "; arc_cleanup_weak_lifetime_hooks = "
      << Objc3ArcCleanupWeakLifetimeHooksSummary() << "\n";
  // ARC/block autorelease-return implementation anchor: publish
  // the supported escaping-block plus autoreleasing-return edge inventory so
  // later runtime ARC work extends a real branch-stable lowering surface
  // rather than re-deriving cleanup ordering from semantic summaries.
  out << "; arc_block_autorelease_return_lowering = "
      << Objc3ArcBlockAutoreleaseReturnLoweringSummary() << "\n";
  // runtime ARC helper API surface anchor: publish the private
  // helper ABI boundary that current ARC lowering already consumes so later
  // runtime implementation work extends a truthful runtime contract instead
  // of inferring helper availability from emitted calls alone.
  out << "; runtime_arc_helper_api_surface = "
      << Objc3RuntimeArcHelperApiSurfaceSummary() << "\n";
  // runtime ARC helper implementation anchor: publish the live
  // executable helper-runtime support boundary so later diagnostics or debug
  // instrumentation work extends a truthful runtime capability rather than
  // another summary-only marker.
  out << "; runtime_arc_helper_runtime_support = "
      << Objc3RuntimeArcHelperRuntimeSupportSummary() << "\n";
  // ownership-debug/runtime-validation anchor: publish the
  // private ARC debug snapshot boundary so lane-D validation can prove live
  // helper traffic without widening the public runtime ABI.
  out << "; runtime_arc_debug_instrumentation = "
      << Objc3RuntimeArcDebugInstrumentationSummary() << "\n";
  // runnable-arc-runtime gate anchor: lane-E freezes the supported
  // ARC slice above the existing mode-handling, interaction, lowering, and
  // runtime proof chain without claiming closeout-matrix coverage yet.
  out << "; runnable_arc_runtime_gate = "
      << Objc3RunnableArcRuntimeGateSummary() << "\n";
  // runnable-arc-closeout anchor: lane-E consumes the already-live
  // ARC proof chain plus integrated execution smoke as the closeout surface
  // without widening the supported ARC semantics or runtime ABI.
  out << "; runnable_arc_closeout = " << Objc3RunnableArcCloseoutSummary()
      << "\n";
  // runtime-backed semantics closure anchor: issue #8017 consumes the
  // existing block, ARC, error, async/task, and actor runtime-backed slices
  // through one emitted contract line plus a durable claimability report.
  out << "; runtime_backed_semantics_closure = "
      << Objc3RuntimeBackedSemanticsClosureSummary() << "\n";
}

  result.retain_release_operation_summary = result.integration_surface.retain_release_operation_summary;
  result.deterministic_retain_release_operation_handoff =
      result.type_metadata_handoff.retain_release_operation_summary.deterministic &&
      result.integration_surface.retain_release_operation_summary.deterministic &&
      IsEquivalentRetainReleaseOperationSummary(
          result.integration_surface.retain_release_operation_summary,
          result.type_metadata_handoff.retain_release_operation_summary) &&
      // runtime ARC helper API surface anchor: sema continues to
      // publish ownership and helper-need summaries only; the concrete helper
      // ABI itself remains frozen in the runtime/lowering boundary.
      // runtime ARC helper implementation anchor: sema continues to
      // publish ownership and helper-need summaries only, while the supported
      // property/weak/autorelease-return ARC slice now reaches a live linked
      // helper/runtime boundary rather than a semantic promise here.
      // ownership-debug/runtime-validation anchor: sema still does
      // not own ARC debug counters or helper-traffic probes; it only preserves
      // the packets that the private runtime/testing surface consumes.
      result.type_metadata_handoff.retain_release_operation_summary.retain_insertion_sites <=
          result.type_metadata_handoff.retain_release_operation_summary.ownership_qualified_sites +
              result.type_metadata_handoff.retain_release_operation_summary.contract_violation_sites &&
      result.type_metadata_handoff.retain_release_operation_summary.release_insertion_sites <=
          result.type_metadata_handoff.retain_release_operation_summary.ownership_qualified_sites +
              result.type_metadata_handoff.retain_release_operation_summary.contract_violation_sites &&
      result.type_metadata_handoff.retain_release_operation_summary.autorelease_insertion_sites <=
          result.type_metadata_handoff.retain_release_operation_summary.ownership_qualified_sites +
              result.type_metadata_handoff.retain_release_operation_summary.contract_violation_sites;
  result.weak_unowned_semantics_summary = result.integration_surface.weak_unowned_semantics_summary;
  result.deterministic_weak_unowned_semantics_handoff =
      result.type_metadata_handoff.weak_unowned_semantics_summary.deterministic &&
      result.integration_surface.weak_unowned_semantics_summary.deterministic &&
      IsEquivalentWeakUnownedSemanticsSummary(
          result.integration_surface.weak_unowned_semantics_summary,
          result.type_metadata_handoff.weak_unowned_semantics_summary) &&
      result.type_metadata_handoff.weak_unowned_semantics_summary.unowned_safe_reference_sites <=
          result.type_metadata_handoff.weak_unowned_semantics_summary.unowned_reference_sites &&
      result.type_metadata_handoff.weak_unowned_semantics_summary.weak_unowned_conflict_sites <=
          result.type_metadata_handoff.weak_unowned_semantics_summary.ownership_candidate_sites &&
      result.type_metadata_handoff.weak_unowned_semantics_summary.contract_violation_sites <=
          result.type_metadata_handoff.weak_unowned_semantics_summary.ownership_candidate_sites +
              result.type_metadata_handoff.weak_unowned_semantics_summary.weak_unowned_conflict_sites;
  // ARC mode-handling anchor: the handoff keeps ownership
  // qualifiers, weak/unowned summaries, ARC fix-it summaries, and
  // autoreleasepool scope accounting live and deterministic while explicit
  // ARC mode now widens executable ownership-qualified
  // function/method
  // signatures without claiming full ARC automation.
  // ARC semantic-rule freeze anchor: the pass-manager handoff still
  // treats ARC semantic legality as explicit-mode-only, preserving deterministic
  // weak/unowned and ARC-fixit surfaces while broader inference remains
  // deferred.
  // ARC inference/lifetime implementation anchor: the handoff now
  // treats explicit ARC mode as permission to infer strong-owned retain/release
  // activity for the supported unqualified object slice, while non-ARC still
  // preserves the zero-inference baseline and later ARC cleanup work remains
  // deferred.
  // ARC interaction-semantics expansion anchor: the handoff now also
  // preserves the semantic packets that distinguish weak/non-owning storage,
  // explicit autorelease returns, synthesized accessor ownership packets, and
  // block-capture interactions inside the supported ARC slice.
  // ARC lowering ABI/cleanup freeze anchor: this handoff remains
  // semantic-only and must not silently claim cleanup scheduling, helper-call
  // placement, or runtime ABI widening before the lane-C lowering issues land.
  // ARC automatic-insertion implementation anchor: the handoff now
  // also preserves the supported ARC insertion flags that lowering consumes for
  // helper placement, without moving retain/release/autorelease emission into
  // sema itself.
  // ARC cleanup/weak/lifetime implementation anchor: the handoff
  // continues to be semantic-only while preserving the packets lowering now
  // consumes for scope-exit cleanup ordering, weak current-property helper
  // usage, and deterministic block-capture lifetime cleanup.
  // ARC/block autorelease-return implementation anchor: the handoff
  // also preserves the explicit block-escape and return-autorelease packets,
  // while lane-C owns the branch-stable cleanup ordering needed to combine
  // them truthfully.
  // runnable-arc-runtime gate anchor: lane-E consumes this existing
  // source, semantic, lowering, and runtime proof chain and must not regress
  // into parser-only or metadata-only evidence.
  // runnable-arc-closeout anchor: the closeout matrix consumes this
  // unchanged proof chain plus integrated smoke instead of broadening parser
  // or semantic acceptance in lane E.
  result.arc_diagnostics_fixit_summary = result.integration_surface.arc_diagnostics_fixit_summary;
  result.deterministic_arc_diagnostics_fixit_handoff =
      result.type_metadata_handoff.arc_diagnostics_fixit_summary.deterministic &&
      result.integration_surface.arc_diagnostics_fixit_summary.deterministic &&
      IsEquivalentArcDiagnosticsFixitSummary(
          result.integration_surface.arc_diagnostics_fixit_summary,
          result.type_metadata_handoff.arc_diagnostics_fixit_summary) &&
      result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites <=
          result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.type_metadata_handoff.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites <=
          result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.type_metadata_handoff.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.type_metadata_handoff.arc_diagnostics_fixit_summary
              .ownership_arc_weak_unowned_conflict_diagnostic_sites <=
          result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
              result.type_metadata_handoff.arc_diagnostics_fixit_summary.contract_violation_sites &&
      result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites <=
          result.type_metadata_handoff.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites +
              result.type_metadata_handoff.arc_diagnostics_fixit_summary.contract_violation_sites;
  result.autoreleasepool_scope_summary = result.integration_surface.autoreleasepool_scope_summary;
  result.deterministic_autoreleasepool_scope_handoff =
      result.type_metadata_handoff.autoreleasepool_scope_summary.deterministic &&
      result.integration_surface.autoreleasepool_scope_summary.deterministic &&
      IsEquivalentAutoreleasePoolScopeSummary(
          result.integration_surface.autoreleasepool_scope_summary,
          result.type_metadata_handoff.autoreleasepool_scope_summary) &&
      result.type_metadata_handoff.autoreleasepool_scope_summary.scope_symbolized_sites <=
          result.type_metadata_handoff.autoreleasepool_scope_summary.scope_sites &&
      result.type_metadata_handoff.autoreleasepool_scope_summary.contract_violation_sites <=
          result.type_metadata_handoff.autoreleasepool_scope_summary.scope_sites &&
      (result.type_metadata_handoff.autoreleasepool_scope_summary.scope_sites > 0u ||
       result.type_metadata_handoff.autoreleasepool_scope_summary.max_scope_depth == 0u) &&
      result.type_metadata_handoff.autoreleasepool_scope_summary.max_scope_depth <=
          static_cast<unsigned>(result.type_metadata_handoff.autoreleasepool_scope_summary.scope_sites);

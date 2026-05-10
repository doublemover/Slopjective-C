#include "pipeline/frontend_executable_metadata_semantic_surface_helpers_owners.h"

namespace objc3c::pipeline::orchestration {

void PublishExecutableMetadataSemanticValidationReadiness(
    Objc3ExecutableMetadataSemanticValidationSurface &surface,
    const Objc3MethodLookupOverrideConflictSummary
        &method_lookup_override_conflict_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary) {
  surface.override_lookup_complete =
      method_lookup_override_conflict_summary.unresolved_base_interfaces == 0u;
  surface.override_conflicts_absent =
      method_lookup_override_conflict_summary.override_conflicts == 0u;
  surface.protocol_composition_valid =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
          0u &&
      class_protocol_category_linking_summary.protocol_composition_symbols >=
          class_protocol_category_linking_summary
              .category_composition_symbols &&
      class_protocol_category_linking_summary.protocol_composition_sites >=
          class_protocol_category_linking_summary.category_composition_sites;

  surface.inheritance_validation_ready =
      surface.class_inheritance_edges_complete &&
      surface.protocol_inheritance_edges_complete &&
      surface.inheritance_chain_cycle_free &&
      surface.superclass_targets_resolved &&
      surface.protocol_inheritance_targets_resolved;
  surface.override_validation_ready =
      surface.method_lookup_override_conflict_handoff_deterministic &&
      surface.method_override_edges_complete &&
      surface.override_lookup_complete &&
      surface.override_conflicts_absent;
  surface.protocol_composition_validation_ready =
      surface.class_protocol_category_linking_deterministic &&
      surface.protocol_composition_valid;
  surface.metaclass_relationship_validation_ready =
      surface.metaclass_edges_complete &&
      surface.metaclass_targets_resolved &&
      surface.metaclass_lineage_aligned;

  if (surface.contract_id.empty()) {
    surface.failure_reason =
        "executable metadata semantic validation contract id is empty";
  } else if (surface.executable_metadata_semantic_consistency_contract_id.empty()) {
    surface.failure_reason =
        "executable metadata semantic consistency dependency contract id is empty";
  } else if (!surface.semantic_consistency_ready) {
    surface.failure_reason =
        "executable metadata semantic consistency boundary is not ready";
  } else if (!surface.method_lookup_override_conflict_handoff_deterministic) {
    surface.failure_reason =
        "method lookup/override conflict handoff is not deterministic";
  } else if (!surface.class_protocol_category_linking_deterministic) {
    surface.failure_reason =
        "class/protocol/category linking handoff is not deterministic";
  } else if (!surface.inheritance_validation_ready) {
    surface.failure_reason =
        "inheritance validation is incomplete";
  } else if (!surface.override_validation_ready) {
    surface.failure_reason = "override validation is incomplete";
  } else if (!surface.protocol_composition_validation_ready) {
    surface.failure_reason =
        "protocol composition validation is incomplete";
  } else if (!surface.metaclass_relationship_validation_ready) {
    surface.failure_reason =
        "metaclass relationship validation is incomplete";
  }

  surface.semantic_validation_complete = surface.failure_reason.empty();
  surface.lowering_admission_ready = false;
  surface.fail_closed = surface.semantic_validation_complete &&
                        !surface.lowering_admission_ready;
  if (surface.failure_reason.empty() && !surface.fail_closed) {
    surface.failure_reason =
        "executable metadata semantic validation surface is not fail-closed";
  }
}

}  // namespace objc3c::pipeline::orchestration

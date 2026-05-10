#include "pipeline/frontend_runtime_metadata_boundary_helpers_owners.h"

namespace objc3c::pipeline::orchestration {

void PublishRuntimeExportLegalityFailureReason(
    Objc3RuntimeExportLegalityBoundary &boundary,
    const Objc3TypedSemaToLoweringContractSurface &typed_surface) {
  if (boundary.contract_id.empty()) {
    boundary.failure_reason = "runtime export legality contract id is empty";
  } else if (!boundary.sema_type_metadata_handoff_deterministic) {
    boundary.failure_reason =
        typed_surface.failure_reason.empty()
            ? "semantic type-metadata handoff is not deterministic"
            : typed_surface.failure_reason;
  } else if (!boundary.typed_sema_surface_ready) {
    boundary.failure_reason =
        "typed sema runtime-export handoff is not ready";
  } else if (!boundary.typed_sema_surface_deterministic) {
    boundary.failure_reason =
        "typed sema runtime-export handoff is not deterministic";
  } else if (!boundary.runtime_metadata_source_boundary_ready) {
    boundary.failure_reason =
        "runtime metadata source ownership boundary is not ready";
  } else if (!boundary.protocol_category_deterministic) {
    boundary.failure_reason =
        "protocol/category semantic handoff is not deterministic";
  } else if (!boundary.class_protocol_category_linking_deterministic) {
    boundary.failure_reason =
        "class/protocol/category linking handoff is not deterministic";
  } else if (!boundary.selector_normalization_deterministic) {
    boundary.failure_reason =
        "selector normalization handoff is not deterministic";
  } else if (!boundary.property_attribute_deterministic) {
    boundary.failure_reason = "property attribute handoff is not deterministic";
  } else if (!boundary.object_pointer_surface_deterministic) {
    boundary.failure_reason =
        "object-pointer/nullability/generics handoff is not deterministic";
  } else if (!boundary.symbol_graph_scope_resolution_deterministic) {
    boundary.failure_reason =
        "symbol-graph/scope-resolution handoff is not deterministic";
  } else if (!boundary.property_synthesis_ivar_binding_deterministic) {
    boundary.failure_reason =
        "property synthesis/ivar binding handoff is not deterministic";
  } else if (boundary.invalid_protocol_composition_sites >
             boundary.protocol_record_count + boundary.category_record_count) {
    boundary.failure_reason =
        "invalid protocol composition sites exceed export-bearing records";
  } else if (boundary.ivar_record_count > boundary.property_record_count) {
    boundary.failure_reason =
        "ivar export records exceed property export records";
  }
}

void PublishRuntimeExportLegalityFailClosed(
    Objc3RuntimeExportLegalityBoundary &boundary) {
  boundary.semantic_boundary_frozen = boundary.failure_reason.empty();
  boundary.metadata_export_enforcement_ready = false;
  boundary.fail_closed =
      boundary.semantic_boundary_frozen &&
      !boundary.metadata_export_enforcement_ready &&
      boundary.duplicate_runtime_identity_enforcement_pending &&
      boundary.incomplete_declaration_export_blocking_pending &&
      boundary.illegal_redeclaration_mix_export_blocking_pending;
  if (boundary.failure_reason.empty() && !boundary.fail_closed) {
    boundary.failure_reason =
        "runtime export legality freeze is not fail-closed";
  }
}

}  // namespace objc3c::pipeline::orchestration

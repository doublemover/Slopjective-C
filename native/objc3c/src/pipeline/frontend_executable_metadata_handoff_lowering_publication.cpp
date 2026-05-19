#include "pipeline/frontend_executable_metadata_handoff_owners.h"

#include "pipeline/frontend_executable_metadata_handoff_replay.h"

namespace objc3_frontend_executable_metadata_handoff {

void PublishExecutableMetadataLoweringHandoffSurfaceReadiness(
    Objc3ExecutableMetadataLoweringHandoffSurface &surface) {
  if (surface.contract_id.empty()) {
    surface.failure_reason =
        "metadata lowering handoff contract id is empty";
  } else if (surface.executable_metadata_source_graph_contract_id.empty()) {
    surface.failure_reason =
        "metadata lowering handoff graph contract id is empty";
  } else if (surface.executable_metadata_semantic_consistency_contract_id.empty()) {
    surface.failure_reason =
        "metadata lowering handoff semantic consistency contract id is empty";
  } else if (surface.executable_metadata_semantic_validation_contract_id.empty()) {
    surface.failure_reason =
        "metadata lowering handoff semantic validation contract id is empty";
  } else if (!surface.source_graph_ready) {
    surface.failure_reason =
        "executable metadata source graph is not ready for lowering handoff";
  } else if (!surface.semantic_consistency_ready) {
    surface.failure_reason =
        "executable metadata semantic consistency boundary is not ready";
  } else if (!surface.semantic_validation_ready) {
    surface.failure_reason =
        "executable metadata semantic validation surface is not ready";
  } else if (!surface.semantic_type_metadata_handoff_deterministic) {
    surface.failure_reason =
        "semantic type metadata handoff is not deterministic";
  } else if (!surface.protocol_category_handoff_deterministic) {
    surface.failure_reason =
        "protocol/category metadata handoff is not deterministic";
  } else if (!surface.class_protocol_category_linking_handoff_deterministic) {
    surface.failure_reason =
        "class/protocol/category metadata linking handoff is not deterministic";
  } else if (!surface.selector_normalization_handoff_deterministic) {
    surface.failure_reason =
        "selector normalization handoff is not deterministic";
  } else if (!surface.property_attribute_handoff_deterministic) {
    surface.failure_reason =
        "property attribute handoff is not deterministic";
  } else if (!surface.symbol_graph_scope_resolution_handoff_deterministic) {
    surface.failure_reason =
        "symbol graph and scope resolution handoff is not deterministic";
  } else if (!surface.property_synthesis_ivar_binding_handoff_deterministic) {
    surface.failure_reason =
        "property synthesis and ivar binding handoff is not deterministic";
  }

  surface.lowering_schema_frozen = surface.failure_reason.empty();
  surface.ready_for_lowering = false;
  surface.fail_closed =
      surface.lowering_schema_frozen && !surface.ready_for_lowering;
  surface.replay_key = BuildExecutableMetadataLoweringHandoffReplayKey(surface);
  if (surface.failure_reason.empty() && !surface.fail_closed) {
    surface.failure_reason =
        "metadata lowering handoff freeze is not fail-closed";
  }
  if (!surface.failure_reason.empty()) {
    surface.replay_key.clear();
  }
}

}  // namespace objc3_frontend_executable_metadata_handoff

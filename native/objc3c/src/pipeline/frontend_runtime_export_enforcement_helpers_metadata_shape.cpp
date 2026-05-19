#include "pipeline/frontend_runtime_export_enforcement_helpers_owners.h"

namespace objc3c::pipeline::orchestration {

std::size_t CountRuntimeExportMetadataShapeDriftSites(
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality) {
  return (runtime_export_legality.semantic_integration_surface_built ? 0u : 1u) +
         (runtime_export_legality.sema_type_metadata_handoff_deterministic ? 0u
                                                                           : 1u) +
         (runtime_export_legality.typed_sema_surface_ready ? 0u : 1u) +
         (runtime_export_legality.typed_sema_surface_deterministic ? 0u : 1u) +
         (runtime_export_legality.runtime_metadata_source_boundary_ready ? 0u
                                                                        : 1u) +
         (runtime_export_legality.protocol_category_deterministic ? 0u : 1u) +
         (runtime_export_legality.class_protocol_category_linking_deterministic
              ? 0u
              : 1u) +
         (runtime_export_legality.selector_normalization_deterministic ? 0u
                                                                       : 1u) +
         (runtime_export_legality.property_attribute_deterministic ? 0u : 1u) +
         (runtime_export_legality.object_pointer_surface_deterministic ? 0u
                                                                      : 1u) +
         (runtime_export_legality.symbol_graph_scope_resolution_deterministic ? 0u
                                                                             : 1u) +
         (runtime_export_legality.property_synthesis_ivar_binding_deterministic
              ? 0u
              : 1u) +
         (runtime_export_legality.invalid_protocol_composition_sites <=
                  runtime_export_legality.protocol_record_count +
                      runtime_export_legality.category_record_count
              ? 0u
              : 1u) +
         (runtime_export_legality.ivar_record_count <=
                  runtime_export_legality.property_record_count
              ? 0u
              : 1u);
}

}  // namespace objc3c::pipeline::orchestration

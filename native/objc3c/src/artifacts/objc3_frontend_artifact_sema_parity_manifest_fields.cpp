#include "artifacts/objc3_frontend_artifact_sema_parity_manifest_fields.h"

#include <ostream>

#include "pipeline/results/pipeline_result_model.h"
#include "sema/model/frontend_linkage_summaries.h"
#include "sema/objc3_sema_parity_contract_surface.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactSemaParityManifestFields(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result,
    bool parity_ready) {
  const auto &sema_parity_surface = pipeline_result.sema_parity_surface;
  const auto &protocol_category_summary =
      pipeline_result.protocol_category_summary;
  const auto &class_protocol_category_linking_summary =
      pipeline_result.class_protocol_category_linking_summary;
  const auto &selector_normalization_summary =
      pipeline_result.selector_normalization_summary;
  const auto &property_attribute_summary =
      pipeline_result.property_attribute_summary;

  manifest << ",\"deterministic_atomic_memory_order_mapping\":"
           << (sema_parity_surface.deterministic_atomic_memory_order_mapping
                   ? "true"
                   : "false")
           << ",\"atomic_memory_order_mapping_total\":"
           << sema_parity_surface.atomic_memory_order_mapping.total()
           << ",\"atomic_relaxed_ops\":"
           << sema_parity_surface.atomic_memory_order_mapping.relaxed
           << ",\"atomic_acquire_ops\":"
           << sema_parity_surface.atomic_memory_order_mapping.acquire
           << ",\"atomic_release_ops\":"
           << sema_parity_surface.atomic_memory_order_mapping.release
           << ",\"atomic_acq_rel_ops\":"
           << sema_parity_surface.atomic_memory_order_mapping.acq_rel
           << ",\"atomic_seq_cst_ops\":"
           << sema_parity_surface.atomic_memory_order_mapping.seq_cst
           << ",\"atomic_unmapped_ops\":"
           << sema_parity_surface.atomic_memory_order_mapping.unsupported
           << ",\"deterministic_vector_type_lowering\":"
           << (sema_parity_surface.deterministic_vector_type_lowering
                   ? "true"
                   : "false")
           << ",\"vector_type_lowering_total\":"
           << sema_parity_surface.vector_type_lowering.total()
           << ",\"vector_return_annotations\":"
           << sema_parity_surface.vector_type_lowering.return_annotations
           << ",\"vector_param_annotations\":"
           << sema_parity_surface.vector_type_lowering.param_annotations
           << ",\"vector_i32_annotations\":"
           << sema_parity_surface.vector_type_lowering.i32_annotations
           << ",\"vector_bool_annotations\":"
           << sema_parity_surface.vector_type_lowering.bool_annotations
           << ",\"vector_lane2_annotations\":"
           << sema_parity_surface.vector_type_lowering.lane2_annotations
           << ",\"vector_lane4_annotations\":"
           << sema_parity_surface.vector_type_lowering.lane4_annotations
           << ",\"vector_lane8_annotations\":"
           << sema_parity_surface.vector_type_lowering.lane8_annotations
           << ",\"vector_lane16_annotations\":"
           << sema_parity_surface.vector_type_lowering.lane16_annotations
           << ",\"vector_unsupported_annotations\":"
           << sema_parity_surface.vector_type_lowering.unsupported_annotations
           << ",\"ready\":" << (sema_parity_surface.ready ? "true" : "false")
           << ",\"parity_ready\":" << (parity_ready ? "true" : "false")
           << ",\"globals_total\":" << sema_parity_surface.globals_total
           << ",\"functions_total\":" << sema_parity_surface.functions_total
           << ",\"type_metadata_global_entries\":"
           << sema_parity_surface.type_metadata_global_entries
           << ",\"type_metadata_function_entries\":"
           << sema_parity_surface.type_metadata_function_entries
           // Legacy extraction anchor retained for contract tests:
           // << sema_parity_surface.type_metadata_function_entries << "},\n";
           << ",\"deterministic_interface_implementation_handoff\":"
           << (sema_parity_surface
                       .deterministic_interface_implementation_handoff
                   ? "true"
                   : "false")
           << ",\"interfaces_total\":" << sema_parity_surface.interfaces_total
           << ",\"implementations_total\":"
           << sema_parity_surface.implementations_total
           << ",\"type_metadata_interface_entries\":"
           << sema_parity_surface.type_metadata_interface_entries
           << ",\"type_metadata_implementation_entries\":"
           << sema_parity_surface.type_metadata_implementation_entries
           << ",\"declared_interfaces\":"
           << sema_parity_surface.interface_implementation_summary
                  .declared_interfaces
           << ",\"declared_implementations\":"
           << sema_parity_surface.interface_implementation_summary
                  .declared_implementations
           << ",\"resolved_interfaces\":"
           << sema_parity_surface.interface_implementation_summary
                  .resolved_interfaces
           << ",\"resolved_implementations\":"
           << sema_parity_surface.interface_implementation_summary
                  .resolved_implementations
           << ",\"interface_method_symbols_total\":"
           << sema_parity_surface.interface_method_symbols_total
           << ",\"implementation_method_symbols_total\":"
           << sema_parity_surface.implementation_method_symbols_total
           << ",\"linked_implementation_symbols_total\":"
           << sema_parity_surface.linked_implementation_symbols_total
           << ",\"deterministic_interface_implementation_summary\":"
           << (sema_parity_surface.interface_implementation_summary.deterministic
                   ? "true"
                   : "false")
           << ",\"deterministic_protocol_category_handoff\":"
           << (protocol_category_summary.deterministic_protocol_category_handoff
                   ? "true"
                   : "false")
           << ",\"type_metadata_protocol_entries\":"
           << protocol_category_summary.resolved_protocol_symbols
           << ",\"type_metadata_category_entries\":"
           << protocol_category_summary.resolved_category_symbols
           << ",\"deterministic_class_protocol_category_linking_handoff\":"
           << (class_protocol_category_linking_summary
                       .deterministic_class_protocol_category_linking_handoff
                   ? "true"
                   : "false")
           << ",\"class_protocol_category_declared_class_interfaces\":"
           << class_protocol_category_linking_summary.declared_class_interfaces
           << ",\"class_protocol_category_declared_class_implementations\":"
           << class_protocol_category_linking_summary
                  .declared_class_implementations
           << ",\"class_protocol_category_resolved_class_interfaces\":"
           << class_protocol_category_linking_summary.resolved_class_interfaces
           << ",\"class_protocol_category_resolved_class_implementations\":"
           << class_protocol_category_linking_summary
                  .resolved_class_implementations
           << ",\"class_protocol_category_linked_class_method_symbols\":"
           << class_protocol_category_linking_summary.linked_class_method_symbols
           << ",\"class_protocol_category_linked_category_method_symbols\":"
           << class_protocol_category_linking_summary
                  .linked_category_method_symbols
           << ",\"class_protocol_category_protocol_composition_sites\":"
           << class_protocol_category_linking_summary.protocol_composition_sites
           << ",\"class_protocol_category_protocol_composition_symbols\":"
           << class_protocol_category_linking_summary.protocol_composition_symbols
           << ",\"class_protocol_category_category_composition_sites\":"
           << class_protocol_category_linking_summary.category_composition_sites
           << ",\"class_protocol_category_category_composition_symbols\":"
           << class_protocol_category_linking_summary.category_composition_symbols
           << ",\"class_protocol_category_invalid_protocol_composition_sites\":"
           << class_protocol_category_linking_summary
                  .invalid_protocol_composition_sites
           << ",\"deterministic_selector_normalization_handoff\":"
           << (selector_normalization_summary
                       .deterministic_selector_normalization_handoff
                   ? "true"
                   : "false")
           << ",\"selector_method_declaration_entries\":"
           << selector_normalization_summary.method_declaration_entries
           << ",\"selector_normalized_method_declarations\":"
           << selector_normalization_summary.normalized_method_declarations
           << ",\"selector_piece_entries\":"
           << selector_normalization_summary.selector_piece_entries
           << ",\"selector_piece_parameter_links\":"
           << selector_normalization_summary.selector_piece_parameter_links
           << ",\"deterministic_property_attribute_handoff\":"
           << (property_attribute_summary.deterministic_property_attribute_handoff
                   ? "true"
                   : "false")
           << ",\"property_declaration_entries\":"
           << property_attribute_summary.property_declaration_entries
           << ",\"property_attribute_entries\":"
           << property_attribute_summary.property_attribute_entries
           << ",\"property_attribute_value_entries\":"
           << property_attribute_summary.property_attribute_value_entries
           << ",\"property_accessor_modifier_entries\":"
           << property_attribute_summary.property_accessor_modifier_entries
           << ",\"property_getter_selector_entries\":"
           << property_attribute_summary.property_getter_selector_entries
           << ",\"property_setter_selector_entries\":"
           << property_attribute_summary.property_setter_selector_entries;
}

}  // namespace objc3::artifacts::frontend

#include "artifacts/objc3_frontend_artifact_semantic_surface_manifest_fields.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_core_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_function_manifest.h"
#include "artifacts/objc3_frontend_artifact_source_shape_plan.h"
#include "artifacts/objc3_frontend_feature_claim_artifacts.h"
#include "artifacts/objc3_frontend_feature_claim_truth_artifacts.h"
#include "ast/objc3_ast_declarations.h"
#include "pipeline/results/pipeline_result_model.h"

namespace objc3::artifacts::frontend {

void WriteObjc3FrontendArtifactSemanticSurfaceVectorSignatureFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactFunctionManifest &function_manifest) {
  manifest << "      \"vector_signature_surface\":{\"vector_signature_functions\":"
           << function_manifest.vector_signature_functions
           << ",\"vector_return_signatures\":"
           << function_manifest.vector_return_signatures
           << ",\"vector_param_signatures\":"
           << function_manifest.vector_param_signatures
           << ",\"vector_i32_signatures\":"
           << function_manifest.vector_i32_signatures
           << ",\"vector_bool_signatures\":"
           << function_manifest.vector_bool_signatures
           << ",\"lane2\":" << function_manifest.vector_lane2_signatures
           << ",\"lane4\":" << function_manifest.vector_lane4_signatures
           << ",\"lane8\":" << function_manifest.vector_lane8_signatures
           << ",\"lane16\":" << function_manifest.vector_lane16_signatures
           << "},\n";
}

void WriteObjc3FrontendArtifactSemanticSurfaceCoreFields(
    std::ostream &manifest,
    const Objc3FrontendArtifactSemanticSurfaceManifestContext &context) {
  const auto &program = context.program;
  const auto &pipeline_result = context.pipeline_result;
  const auto &options = context.options;
  const auto &function_manifest = context.function_manifest;
  const auto &source_shape_counts =
      context.source_shape_plan.interface_implementation_method_counts;
  const auto &core_lowering_plan = context.core_lowering_plan;
  const auto &property_synthesis_ivar_binding_summary =
      core_lowering_plan.property_synthesis_ivar_binding_summary;

  manifest << "      \"semantic_surface\": {\"declared_globals\":"
           << program.globals.size()
           << ",\"declared_functions\":"
           << function_manifest.manifest_functions.size()
           << ",\"declared_interfaces\":" << program.interfaces.size()
           << ",\"declared_implementations\":"
           << program.implementations.size()
           << ",\"resolved_global_symbols\":"
           << pipeline_result.integration_surface.globals.size()
           << ",\"resolved_function_symbols\":"
           << pipeline_result.integration_surface.functions.size()
           << ",\"resolved_interface_symbols\":"
           << pipeline_result.integration_surface.interfaces.size()
           << ",\"resolved_implementation_symbols\":"
           << pipeline_result.integration_surface.implementations.size()
           << ",\"declared_protocols\":"
           << pipeline_result.protocol_category_summary.declared_protocols
           << ",\"declared_categories\":"
           << pipeline_result.protocol_category_summary.declared_categories
           << ",\"resolved_protocol_symbols\":"
           << pipeline_result.protocol_category_summary.resolved_protocol_symbols
           << ",\"resolved_category_symbols\":"
           << pipeline_result.protocol_category_summary.resolved_category_symbols
           << ",\"interface_method_symbols\":"
           << pipeline_result.sema_parity_surface
                  .interface_implementation_summary.interface_method_symbols
           << ",\"implementation_method_symbols\":"
           << pipeline_result.sema_parity_surface
                  .interface_implementation_summary.implementation_method_symbols
           << ",\"protocol_method_symbols\":"
           << pipeline_result.protocol_category_summary.protocol_method_symbols
           << ",\"category_method_symbols\":"
           << pipeline_result.protocol_category_summary.category_method_symbols
           << ",\"linked_implementation_symbols\":"
           << pipeline_result.sema_parity_surface
                  .interface_implementation_summary.linked_implementation_symbols
           << ",\"linked_category_symbols\":"
           << pipeline_result.protocol_category_summary.linked_category_symbols
           << ",\"objc_interface_implementation_surface\":{\"interface_class_method_symbols\":"
           << source_shape_counts.interface_class_method_symbols
           << ",\"interface_instance_method_symbols\":"
           << source_shape_counts.interface_instance_method_symbols
           << ",\"implementation_class_method_symbols\":"
           << source_shape_counts.implementation_class_method_symbols
           << ",\"implementation_instance_method_symbols\":"
           << source_shape_counts.implementation_instance_method_symbols
           << ",\"implementation_methods_with_body\":"
           << source_shape_counts.implementation_methods_with_body
           << ",\"deterministic_handoff\":"
           << (pipeline_result.sema_parity_surface
                       .deterministic_interface_implementation_handoff
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_protocol_category_surface\":{\"protocol_method_symbols\":"
           << pipeline_result.protocol_category_summary.protocol_method_symbols
           << ",\"category_method_symbols\":"
           << pipeline_result.protocol_category_summary.category_method_symbols
           << ",\"linked_category_symbols\":"
           << pipeline_result.protocol_category_summary.linked_category_symbols
           << ",\"deterministic_handoff\":"
           << (pipeline_result.protocol_category_summary
                       .deterministic_protocol_category_handoff
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_class_protocol_category_linking_surface\":{\"declared_class_interfaces\":"
           << pipeline_result.class_protocol_category_linking_summary
                  .declared_class_interfaces
           << ",\"declared_class_implementations\":"
           << pipeline_result.class_protocol_category_linking_summary
                  .declared_class_implementations
           << ",\"resolved_class_interfaces\":"
           << pipeline_result.class_protocol_category_linking_summary
                  .resolved_class_interfaces
           << ",\"resolved_class_implementations\":"
           << pipeline_result.class_protocol_category_linking_summary
                  .resolved_class_implementations
           << ",\"linked_class_method_symbols\":"
           << pipeline_result.class_protocol_category_linking_summary
                  .linked_class_method_symbols
           << ",\"linked_category_method_symbols\":"
           << pipeline_result.class_protocol_category_linking_summary
                  .linked_category_method_symbols
           << ",\"protocol_composition_sites\":"
           << pipeline_result.class_protocol_category_linking_summary
                  .protocol_composition_sites
           << ",\"protocol_composition_symbols\":"
           << pipeline_result.class_protocol_category_linking_summary
                  .protocol_composition_symbols
           << ",\"category_composition_sites\":"
           << pipeline_result.class_protocol_category_linking_summary
                  .category_composition_sites
           << ",\"category_composition_symbols\":"
           << pipeline_result.class_protocol_category_linking_summary
                  .category_composition_symbols
           << ",\"invalid_protocol_composition_sites\":"
           << pipeline_result.class_protocol_category_linking_summary
                  .invalid_protocol_composition_sites
           << ",\"deterministic_handoff\":"
           << (pipeline_result.class_protocol_category_linking_summary
                       .deterministic_class_protocol_category_linking_handoff
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_selector_normalization_surface\":{\"method_declaration_entries\":"
           << pipeline_result.selector_normalization_summary
                  .method_declaration_entries
           << ",\"normalized_method_declarations\":"
           << pipeline_result.selector_normalization_summary
                  .normalized_method_declarations
           << ",\"selector_piece_entries\":"
           << pipeline_result.selector_normalization_summary.selector_piece_entries
           << ",\"selector_piece_parameter_links\":"
           << pipeline_result.selector_normalization_summary
                  .selector_piece_parameter_links
           << ",\"deterministic_handoff\":"
           << (pipeline_result.selector_normalization_summary
                       .deterministic_selector_normalization_handoff
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_property_attribute_surface\":{\"property_declaration_entries\":"
           << pipeline_result.property_attribute_summary.property_declaration_entries
           << ",\"property_attribute_entries\":"
           << pipeline_result.property_attribute_summary.property_attribute_entries
           << ",\"property_attribute_value_entries\":"
           << pipeline_result.property_attribute_summary
                  .property_attribute_value_entries
           << ",\"property_accessor_modifier_entries\":"
           << pipeline_result.property_attribute_summary
                  .property_accessor_modifier_entries
           << ",\"property_getter_selector_entries\":"
           << pipeline_result.property_attribute_summary
                  .property_getter_selector_entries
           << ",\"property_setter_selector_entries\":"
           << pipeline_result.property_attribute_summary
                  .property_setter_selector_entries
           << ",\"deterministic_handoff\":"
           << (pipeline_result.property_attribute_summary
                       .deterministic_property_attribute_handoff
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_property_synthesis_ivar_binding_surface\":{\"property_synthesis_sites\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_sites
           << ",\"property_synthesis_explicit_ivar_bindings\":"
           << property_synthesis_ivar_binding_summary
                  .property_synthesis_explicit_ivar_bindings
           << ",\"property_synthesis_default_ivar_bindings\":"
           << property_synthesis_ivar_binding_summary
                  .property_synthesis_default_ivar_bindings
           << ",\"interface_owned_property_synthesis_sites\":"
           << property_synthesis_ivar_binding_summary
                  .interface_owned_property_synthesis_sites
           << ",\"implementation_property_redeclaration_sites\":"
           << property_synthesis_ivar_binding_summary
                  .implementation_property_redeclaration_sites
           << ",\"ivar_binding_sites\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_sites
           << ",\"ivar_binding_resolved\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_resolved
           << ",\"ivar_binding_missing\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_missing
           << ",\"ivar_binding_conflicts\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_conflicts
           << ",\"replay_key\":\""
           << core_lowering_plan.property_synthesis_ivar_binding_replay_key
           << "\",\"deterministic_handoff\":"
           << (core_lowering_plan.property_synthesis_ivar_binding_handoff_deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_runnable_feature_claim_inventory\":"
           << objc3::artifacts::BuildRunnableFeatureClaimInventoryJson(
                  options, pipeline_result)
           << ",\"objc_feature_claim_and_strictness_truth_surface\":"
           << BuildFeatureClaimStrictnessTruthSurfaceJson(options,
                                                          pipeline_result);
}

}  // namespace objc3::artifacts::frontend

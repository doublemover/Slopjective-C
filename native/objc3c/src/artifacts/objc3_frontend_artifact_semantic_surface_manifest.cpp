#include "artifacts/objc3_frontend_artifact_semantic_surface_manifest.h"

#include <ostream>

#include "artifacts/objc3_frontend_artifact_core_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_function_manifest.h"
#include "artifacts/objc3_frontend_artifact_interop_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_ownership_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_preservation_plan.h"
#include "artifacts/objc3_frontend_artifact_semantic_lowering_plan.h"
#include "artifacts/objc3_frontend_artifact_source_shape_plan.h"
#include "artifacts/objc3_frontend_concurrency_semantic_artifacts.h"
#include "artifacts/objc3_frontend_control_flow_semantic_artifacts.h"
#include "artifacts/objc3_frontend_dispatch_semantic_artifacts.h"
#include "artifacts/objc3_frontend_error_semantic_artifacts.h"
#include "artifacts/objc3_frontend_feature_claim_artifacts.h"
#include "artifacts/objc3_frontend_feature_claim_truth_artifacts.h"
#include "artifacts/objc3_frontend_interop_semantic_artifacts.h"
#include "artifacts/objc3_frontend_metaprogramming_semantic_artifacts.h"
#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"
#include "artifacts/objc3_frontend_source_closure_artifacts.h"
#include "artifacts/objc3_frontend_tooling_source_artifacts.h"
#include "artifacts/objc3_frontend_type_system_semantic_artifacts.h"

namespace objc3::artifacts::frontend {

void WriteObjc3FrontendArtifactSemanticSurfaceManifest(
    std::ostream &manifest,
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactFunctionManifest &function_manifest,
    const Objc3FrontendArtifactSourceShapePlan &source_shape_plan,
    const Objc3FrontendArtifactCoreLoweringPlan &core_lowering_plan,
    const Objc3FrontendArtifactSemanticLoweringPlan &semantic_lowering_plan,
    const Objc3FrontendArtifactOwnershipAwareLoweringPlan
        &ownership_aware_lowering_plan,
    const Objc3FrontendArtifactInteropLoweringPlan &interop_lowering_plan,
    const Objc3FrontendArtifactPreservationPlan &artifact_preservation_plan,
    const Objc3TypeSystemTypeSemanticModelSummary
        &type_system_type_semantic_model_summary) {
  const auto &source_shape_counts =
      source_shape_plan.interface_implementation_method_counts;
  const auto &property_synthesis_ivar_binding_summary =
      core_lowering_plan.property_synthesis_ivar_binding_summary;
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
                                                          pipeline_result)
           << ",\"objc_type_system_type_source_closure\":"
           << BuildTypeSystemTypeSourceClosureSummaryJson(
                  pipeline_result.type_system_type_source_closure_summary)
           << ",\"objc_control_flow_control_flow_source_closure\":"
           << BuildControlFlowControlFlowSourceClosureSummaryJson(
                  pipeline_result
                      .control_flow_control_flow_source_closure_summary)
           << ",\"objc_error_handling_error_source_closure\":"
           << BuildErrorHandlingErrorSourceClosureSummaryJson(
                  pipeline_result.error_handling_error_source_closure_summary)
           << ",\"objc_concurrency_async_source_closure\":"
           << BuildConcurrencyAsyncSourceClosureSummaryJson(
                  pipeline_result.concurrency_async_source_closure_summary)
           << ",\"objc_ownership_resource_borrowed_and_capture_list_source_closure\":"
           << BuildOwnershipSystemExtensionSourceClosureSummaryJson(
                  pipeline_result
                      .ownership_system_extension_source_closure_summary)
           << ",\"objc_ownership_cleanup_resource_and_capture_source_completion\":"
           << BuildOwnershipCleanupResourceCaptureSourceCompletionSummaryJson(
                  pipeline_result
                      .ownership_cleanup_resource_capture_source_completion_summary)
           << ",\"objc_ownership_retainable_c_family_source_completion\":"
           << BuildOwnershipRetainableCFamilySourceCompletionSummaryJson(
                  pipeline_result
                      .ownership_retainable_c_family_source_completion_summary)
           << ",\"objc_dispatch_dispatch_intent_and_dynamism_source_closure\":"
           << BuildDispatchDispatchIntentSourceClosureSummaryJson(
                  pipeline_result.dispatch_dispatch_intent_source_closure_summary)
           << ",\"objc_dispatch_dispatch_intent_attribute_and_defaulting_source_completion\":"
           << BuildDispatchDispatchIntentSourceCompletionSummaryJson(
                  pipeline_result
                      .dispatch_dispatch_intent_source_completion_summary)
           << ",\"objc_metaprogramming_derive_macro_property_behavior_source_closure\":"
           << BuildMetaprogrammingMetaprogrammingSourceClosureSummaryJson(
                  pipeline_result
                      .metaprogramming_metaprogramming_source_closure_summary)
           << ",\"objc_metaprogramming_macro_package_and_provenance_source_completion\":"
           << BuildMetaprogrammingMacroPackageProvenanceSourceCompletionSummaryJson(
                  pipeline_result
                      .metaprogramming_macro_package_provenance_source_completion_summary)
           << ",\"objc_metaprogramming_property_behavior_and_synthesized_declaration_source_completion\":"
           << BuildMetaprogrammingPropertyBehaviorSourceCompletionSummaryJson(
                  pipeline_result
                      .metaprogramming_property_behavior_source_completion_summary)
           << ",\"objc_interop_foreign_declaration_and_import_source_closure\":"
           << BuildInteropForeignImportSourceClosureSummaryJson(
                  pipeline_result.interop_foreign_import_source_closure_summary)
           << ",\"objc_interop_cpp_and_swift_interop_annotation_source_completion\":"
           << BuildInteropCppSwiftInteropAnnotationSourceCompletionSummaryJson(
                  pipeline_result
                      .interop_cpp_swift_interop_annotation_source_completion_summary)
           << ",\"objc_tooling_diagnostics_fixit_and_migrator_source_inventory\":"
           << BuildToolingDiagnosticsMigratorSourceInventorySummaryJson(
                  pipeline_result.tooling_diagnostics_migrator_source_inventory_summary)
           << ",\"objc_tooling_migration_and_canonicalization_source_completion\":"
           << BuildToolingMigrationCanonicalizationSourceCompletionSummaryJson(
                  pipeline_result
                      .tooling_migration_canonicalization_source_completion_summary)
           << ",\"objc_tooling_diagnostic_taxonomy_and_portability_contract\":"
           << BuildToolingDiagnosticTaxonomyPortabilityContractSummaryJson(
                  pipeline_result
                      .tooling_diagnostic_taxonomy_portability_contract_summary)
           << ",\"objc_tooling_feature_specific_fixit_synthesis\":"
           << BuildToolingFeatureSpecificFixitSynthesisSummaryJson(
                  pipeline_result.tooling_feature_specific_fixit_synthesis_summary)
           << ",\"objc_interop_interop_semantic_model\":"
           << BuildInteropInteropSemanticModelSummaryJson(
                  pipeline_result.interop_interop_semantic_model_summary)
           << ",\"objc_interop_c_and_objc_runtime_parity_semantics\":"
           << BuildInteropInteropRuntimeParitySummaryJson(
                  pipeline_result.interop_interop_runtime_parity_summary)
           << ",\"objc_interop_cpp_ownership_throws_and_async_interactions\":"
           << BuildInteropCppInteropInteractionSummaryJson(
                  pipeline_result.interop_cpp_interop_interaction_summary)
           << ",\"objc_interop_swift_metadata_and_isolation_mapping\":"
           << BuildInteropSwiftInteropIsolationSummaryJson(
                  pipeline_result.interop_swift_interop_isolation_summary)
           << ",\"objc_interop_foreign_surface_interface_and_module_preservation\":"
           << BuildInteropForeignSurfaceInterfacePreservationSummaryJson(
                  interop_lowering_plan
                      .interop_foreign_surface_interface_preservation_summary)
           << ",\"objc_interop_header_module_and_bridge_generation\":"
           << BuildInteropHeaderModuleBridgeGenerationSummaryJson(
                  interop_lowering_plan
                      .interop_header_module_bridge_generation_summary)
           << ",\"objc_interop_interop_lowering_and_abi_contract\":"
           << BuildInteropInteropLoweringContractJson(
                  pipeline_result.interop_interop_semantic_model_summary,
                  pipeline_result.interop_interop_runtime_parity_summary,
                  pipeline_result.interop_cpp_interop_interaction_summary,
                  pipeline_result.interop_swift_interop_isolation_summary,
                  interop_lowering_plan
                      .interop_foreign_surface_interface_preservation_summary,
                  interop_lowering_plan.interop_interop_lowering_contract,
                  interop_lowering_plan.interop_interop_lowering_replay_key)
           << ",\"objc_interop_foreign_call_and_lifetime_lowering\":"
           << BuildInteropForeignCallLifetimeLoweringContractJson(
                  interop_lowering_plan.interop_interop_lowering_contract,
                  pipeline_result.interop_cpp_interop_interaction_summary,
                  interop_lowering_plan
                      .interop_foreign_surface_interface_preservation_summary,
                  interop_lowering_plan
                      .interop_foreign_call_lifetime_lowering_contract,
                  interop_lowering_plan
                      .interop_foreign_call_lifetime_lowering_replay_key)
           << ",\"objc_interop_ffi_metadata_and_interface_preservation\":"
           << BuildInteropFfiMetadataInterfacePreservationContractJson(
                  interop_lowering_plan
                      .interop_foreign_call_lifetime_lowering_contract,
                  interop_lowering_plan
                      .interop_foreign_call_lifetime_lowering_replay_key,
                  interop_lowering_plan
                      .interop_foreign_surface_interface_preservation_summary,
                  interop_lowering_plan
                      .interop_ffi_metadata_interface_preservation_contract,
                  interop_lowering_plan
                      .interop_ffi_metadata_interface_preservation_replay_key)
           << ",\"objc_metaprogramming_expansion_and_behavior_semantic_model\":"
           << BuildMetaprogrammingExpansionBehaviorSemanticModelSummaryJson(
                  pipeline_result
                      .metaprogramming_expansion_behavior_semantic_model_summary)
           << ",\"objc_metaprogramming_derive_expansion_inventory\":"
           << BuildMetaprogrammingDeriveExpansionInventorySummaryJson(
                  pipeline_result.metaprogramming_derive_expansion_inventory_summary)
           << ",\"objc_metaprogramming_macro_safety_sandbox_and_determinism_semantics\":"
           << BuildMetaprogrammingMacroSafetySandboxDeterminismSummaryJson(
                  pipeline_result
                      .metaprogramming_macro_safety_sandbox_determinism_summary)
           << ",\"objc_metaprogramming_property_behavior_legality_and_interaction_completion\":"
           << BuildMetaprogrammingPropertyBehaviorLegalityCompatibilitySummaryJson(
                  pipeline_result
                      .metaprogramming_property_behavior_legality_compatibility_summary)
           << ",\"objc_metaprogramming_expansion_and_lowering_contract\":"
           << BuildMetaprogrammingExpansionLoweringContractJson(
                  pipeline_result
                      .metaprogramming_property_behavior_source_completion_summary,
                  pipeline_result.metaprogramming_derive_expansion_inventory_summary,
                  pipeline_result
                      .metaprogramming_macro_safety_sandbox_determinism_summary,
                  pipeline_result
                      .metaprogramming_property_behavior_legality_compatibility_summary,
                  semantic_lowering_plan
                      .metaprogramming_expansion_lowering_contract,
                  semantic_lowering_plan
                      .metaprogramming_expansion_lowering_replay_key)
           << ",\"objc_metaprogramming_synthesized_ast_and_ir_emission\":"
           << BuildMetaprogrammingSynthesizedArtifactEmissionContractJson(
                  semantic_lowering_plan
                      .metaprogramming_expansion_lowering_contract,
                  semantic_lowering_plan
                      .metaprogramming_synthesized_artifact_emission_contract,
                  semantic_lowering_plan
                      .metaprogramming_synthesized_artifact_emission_replay_key)
           << ",\"objc_metaprogramming_module_interface_and_replay_preservation\":"
           << BuildMetaprogrammingModuleInterfaceReplayPreservationSummaryJson(
                  artifact_preservation_plan
                      .metaprogramming_module_interface_replay_preservation_summary)
           << ",\"objc_dispatch_dynamism_and_dispatch_control_semantic_model\":"
           << BuildDispatchDispatchIntentSemanticModelSummaryJson(
                  pipeline_result.dispatch_dispatch_intent_semantic_model_summary)
           << ",\"objc_dispatch_override_finality_and_sealing_legality\":"
           << BuildDispatchDispatchIntentLegalitySummaryJson(
                  pipeline_result.dispatch_dispatch_intent_legality_summary)
           << ",\"objc_dispatch_dynamism_control_compatibility_diagnostics\":"
           << BuildDispatchDispatchIntentCompatibilitySummaryJson(
                  pipeline_result.dispatch_dispatch_intent_compatibility_summary)
           << ",\"objc_dispatch_dispatch_control_lowering_contract\":"
           << BuildDispatchDispatchControlLoweringContractJson(
                  pipeline_result.dispatch_dispatch_intent_semantic_model_summary,
                  pipeline_result.dispatch_dispatch_intent_legality_summary,
                  pipeline_result.dispatch_dispatch_intent_compatibility_summary,
                  semantic_lowering_plan
                      .dispatch_dispatch_control_lowering_contract,
                  semantic_lowering_plan
                      .dispatch_dispatch_control_lowering_replay_key)
           << ",\"objc_dispatch_dispatch_metadata_and_interface_preservation\":"
           << BuildDispatchDispatchMetadataInterfacePreservationSummaryJson(
                  artifact_preservation_plan
                      .dispatch_dispatch_metadata_interface_preservation_summary)
           << ",\"objc_concurrency_actor_member_and_isolation_source_closure\":"
           << BuildConcurrencyActorMemberIsolationSourceClosureSummaryJson(
                  pipeline_result
                      .concurrency_actor_member_isolation_source_closure_summary)
           << ",\"objc_concurrency_actor_isolation_and_sendable_semantic_model\":"
           << BuildConcurrencyActorIsolationSendableSemanticModelSummaryJson(
                  pipeline_result
                      .concurrency_actor_isolation_sendable_semantic_model_summary)
           << ",\"objc_concurrency_actor_isolation_and_sendability_enforcement\":"
           << BuildConcurrencyActorIsolationSendabilityEnforcementSummaryJson(
                  pipeline_result
                      .concurrency_actor_isolation_sendability_enforcement_summary)
           << ",\"objc_concurrency_actor_race_hazard_and_escape_diagnostics\":"
           << BuildConcurrencyActorRaceHazardEscapeDiagnosticsSummaryJson(
                  pipeline_result
                      .concurrency_actor_race_hazard_escape_diagnostics_summary)
           << ",\"objc_concurrency_actor_lowering_and_metadata_contract\":"
           << BuildConcurrencyActorLoweringMetadataContractJson(
                  pipeline_result
                      .concurrency_actor_member_isolation_source_closure_summary,
                  pipeline_result
                      .concurrency_actor_isolation_sendability_enforcement_summary,
                  pipeline_result
                      .concurrency_actor_race_hazard_escape_diagnostics_summary,
                  semantic_lowering_plan.concurrency_actor_lowering_metadata_contract,
                  semantic_lowering_plan
                      .concurrency_actor_lowering_metadata_replay_key)
           << ",\"objc_concurrency_task_group_and_cancellation_source_closure\":"
           << BuildConcurrencyTaskGroupCancellationSourceClosureSummaryJson(
                  pipeline_result
                      .concurrency_task_group_cancellation_source_closure_summary)
           << ",\"objc_concurrency_async_effect_and_suspension_semantic_model\":"
           << BuildConcurrencyAsyncEffectSuspensionSemanticModelSummaryJson(
                  pipeline_result
                      .concurrency_async_effect_suspension_semantic_model_summary)
           << ",\"objc_concurrency_task_executor_and_cancellation_semantic_model\":"
           << BuildConcurrencyTaskExecutorCancellationSemanticModelSummaryJson(
                  pipeline_result
                      .concurrency_task_executor_cancellation_semantic_model_summary)
           << ",\"objc_ownership_system_extension_semantic_model\":"
           << BuildOwnershipSystemExtensionSemanticModelSummaryJson(
                  pipeline_result.ownership_system_extension_semantic_model_summary)
           << ",\"objc_effects_ownership_semantic_model\":"
           << BuildEffectsOwnershipSemanticModelSummaryJson(
                  pipeline_result.effects_ownership_semantic_model_summary)
           << ",\"objc_cross_module_semantic_contracts_and_diagnostics\":"
           << BuildCrossModuleSemanticContractsDiagnosticsSummaryJson(
                  pipeline_result
                      .cross_module_semantic_contracts_diagnostics_summary)
           << ",\"objc_ownership_resource_move_and_use_after_move_semantics\":"
           << BuildOwnershipResourceMoveUseAfterMoveSemanticsSummaryJson(
                  pipeline_result
                      .ownership_resource_move_use_after_move_semantics_summary)
           << ",\"objc_ownership_borrowed_pointer_escape_analysis\":"
           << BuildOwnershipBorrowedPointerEscapeAnalysisSummaryJson(
                  pipeline_result.ownership_borrowed_pointer_escape_analysis_summary)
           << ",\"objc_ownership_capture_list_and_retainable_family_legality_completion\":"
           << BuildOwnershipCaptureListRetainableFamilyLegalityCompletionSummaryJson(
                  pipeline_result
                      .ownership_capture_list_retainable_family_legality_completion_summary)
           << ",\"objc_ownership_system_extension_lowering_contract\":"
           << BuildOwnershipSystemExtensionLoweringContractJson(
                  pipeline_result.ownership_system_extension_semantic_model_summary,
                  pipeline_result
                      .ownership_resource_move_use_after_move_semantics_summary,
                  pipeline_result.ownership_borrowed_pointer_escape_analysis_summary,
                  pipeline_result
                      .ownership_capture_list_retainable_family_legality_completion_summary,
                  semantic_lowering_plan
                      .ownership_system_extension_lowering_contract,
                  semantic_lowering_plan
                      .ownership_system_extension_lowering_replay_key)
           << ",\"objc_ownership_borrowed_pointer_and_retainable_family_abi_completion\":"
           << BuildOwnershipBorrowedRetainableAbiCompletionJson(
                  semantic_lowering_plan
                      .ownership_system_extension_lowering_contract,
                  pipeline_result
                      .ownership_system_extension_source_closure_summary,
                  pipeline_result
                      .ownership_retainable_c_family_source_completion_summary,
                  semantic_lowering_plan
                      .ownership_system_extension_lowering_replay_key,
                  semantic_lowering_plan
                      .ownership_borrowed_retainable_abi_completion_replay_key)
           << ",\"objc_concurrency_structured_task_and_cancellation_semantics\":"
           << BuildConcurrencyStructuredTaskCancellationSemanticSummaryJson(
                  pipeline_result
                      .concurrency_structured_task_cancellation_semantic_summary)
           << ",\"objc_concurrency_executor_hop_and_affinity_compatibility_completion\":"
           << BuildConcurrencyExecutorHopAffinityCompatibilitySummaryJson(
                  pipeline_result
                      .concurrency_executor_hop_affinity_compatibility_summary)
           << ",\"objc_concurrency_await_suspension_and_resume_semantics\":"
           << BuildConcurrencyAwaitSuspensionResumeSemanticSummaryJson(
                  pipeline_result.concurrency_await_suspension_resume_semantic_summary)
           << ",\"objc_concurrency_async_diagnostics_and_compatibility_completion\":"
           << BuildConcurrencyAsyncDiagnosticsCompatibilitySummaryJson(
                  pipeline_result.concurrency_async_diagnostics_compatibility_summary)
           << ",\"objc_concurrency_continuation_abi_and_async_lowering_contract\":"
           << BuildConcurrencyContinuationAbiAsyncLoweringContractJson(
                  semantic_lowering_plan
                      .concurrency_async_continuation_lowering_contract,
                  semantic_lowering_plan
                      .concurrency_await_lowering_suspension_state_lowering_contract,
                  semantic_lowering_plan
                      .concurrency_async_continuation_lowering_replay_key,
                  semantic_lowering_plan
                      .concurrency_await_lowering_suspension_state_lowering_replay_key)
           << ",\"objc_concurrency_task_runtime_lowering_contract\":"
           << BuildConcurrencyTaskRuntimeLoweringContractJson(
                  pipeline_result
                      .concurrency_task_executor_cancellation_semantic_model_summary,
                  pipeline_result
                      .concurrency_structured_task_cancellation_semantic_summary,
                  pipeline_result
                      .concurrency_executor_hop_affinity_compatibility_summary,
                  semantic_lowering_plan
                      .concurrency_actor_isolation_sendability_lowering_contract,
                  semantic_lowering_plan
                      .concurrency_actor_isolation_sendability_lowering_replay_key,
                  semantic_lowering_plan
                      .concurrency_task_runtime_interop_cancellation_lowering_contract,
                  semantic_lowering_plan
                      .concurrency_task_runtime_interop_cancellation_lowering_replay_key,
                  semantic_lowering_plan
                      .concurrency_concurrency_replay_race_guard_lowering_contract,
                  semantic_lowering_plan
                      .concurrency_concurrency_replay_race_guard_lowering_replay_key)
           << ",\"objc_concurrency_task_group_and_runtime_abi_completion\":"
           << BuildConcurrencyTaskRuntimeAbiCompletionJson(
                  semantic_lowering_plan
                      .concurrency_task_runtime_interop_cancellation_lowering_replay_key,
                  semantic_lowering_plan
                      .concurrency_concurrency_replay_race_guard_lowering_replay_key)
           << ",\"objc_concurrency_async_function_await_and_continuation_lowering\":"
           << BuildConcurrencyAsyncDirectCallLoweringJson(
                  pipeline_result.concurrency_async_source_closure_summary,
                  semantic_lowering_plan
                      .concurrency_async_continuation_lowering_contract,
                  semantic_lowering_plan
                      .concurrency_await_lowering_suspension_state_lowering_contract,
                  semantic_lowering_plan
                      .concurrency_async_continuation_lowering_replay_key,
                  semantic_lowering_plan
                      .concurrency_await_lowering_suspension_state_lowering_replay_key)
           << ",\"objc_concurrency_suspension_autorelease_and_cleanup_integration\":"
           << BuildConcurrencySuspensionCleanupIntegrationJson(
                  core_lowering_plan
                      .control_flow_control_flow_safety_lowering_contract,
                  core_lowering_plan
                      .control_flow_control_flow_safety_lowering_replay_key,
                  ownership_aware_lowering_plan
                      .autoreleasepool_scope_lowering_contract,
                  ownership_aware_lowering_plan
                      .autoreleasepool_scope_lowering_replay_key,
                  semantic_lowering_plan
                      .concurrency_async_continuation_lowering_contract,
                  semantic_lowering_plan
                      .concurrency_await_lowering_suspension_state_lowering_contract,
                  semantic_lowering_plan
                      .concurrency_async_continuation_lowering_replay_key,
                  semantic_lowering_plan
                      .concurrency_await_lowering_suspension_state_lowering_replay_key)
           << ",\"objc_error_handling_error_semantic_model\":"
           << BuildErrorHandlingErrorSemanticModelSummaryJson(
                  pipeline_result.error_handling_error_semantic_model_summary)
           << ",\"objc_error_handling_try_do_catch_semantics\":"
           << BuildErrorHandlingTryDoCatchSemanticSummaryJson(
                  pipeline_result.error_handling_try_do_catch_semantic_summary)
           << ",\"objc_error_handling_error_bridge_legality\":"
           << BuildErrorHandlingErrorBridgeLegalitySummaryJson(
                  pipeline_result.error_handling_error_bridge_legality_summary)
           << ",\"objc_control_flow_control_flow_semantic_model\":"
           << BuildControlFlowControlFlowSemanticModelSummaryJson(
                  pipeline_result.control_flow_control_flow_semantic_model_summary)
           << ",\"objc_control_flow_control_flow_safety_lowering_contract\":"
           << BuildControlFlowControlFlowSafetyLoweringContractJson(
                  core_lowering_plan
                      .control_flow_control_flow_safety_lowering_contract,
                  pipeline_result.control_flow_control_flow_semantic_model_summary,
                  pipeline_result.control_flow_control_flow_semantic_model_summary
                      .replay_key,
                  core_lowering_plan
                      .control_flow_control_flow_safety_lowering_replay_key)
           << ",\"objc_type_system_type_semantic_model\":"
           << BuildTypeSystemTypeSemanticModelSummaryJson(
                  type_system_type_semantic_model_summary);
}

}  // namespace objc3::artifacts::frontend

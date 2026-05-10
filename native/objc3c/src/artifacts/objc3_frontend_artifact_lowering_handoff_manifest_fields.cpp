#include "artifacts/objc3_frontend_artifact_lowering_handoff_manifest_fields.h"

#include "artifacts/objc3_frontend_artifact_lowering_handoff_block_manifest_fields.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactLoweringHandoffManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactCoreLoweringPlan &core_lowering_plan,
    const Objc3FrontendArtifactOwnershipAwareLoweringPlan
        &ownership_aware_lowering_plan,
    const Objc3FrontendArtifactBlockLoweringPlan &block_lowering_plan,
    const Objc3FrontendArtifactTypeSystemLoweringPlan
        &type_system_lowering_plan,
    const Objc3FrontendArtifactRuntimeImportPlan &runtime_import_plan,
    const Objc3FrontendArtifactModuleLoweringPlan &module_lowering_plan,
    const Objc3FrontendArtifactErrorLoweringPlan &error_lowering_plan,
    const Objc3FrontendObjectPointerNullabilityGenericsSummary
        &object_pointer_nullability_generics_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary) {
  const bool property_synthesis_ivar_binding_handoff_deterministic =
      core_lowering_plan.property_synthesis_ivar_binding_handoff_deterministic;
  const Objc3PropertySynthesisIvarBindingSummary
      &property_synthesis_ivar_binding_summary =
          core_lowering_plan.property_synthesis_ivar_binding_summary;
  const std::string &property_synthesis_ivar_binding_replay_key =
      core_lowering_plan.property_synthesis_ivar_binding_replay_key;
  const Objc3IdClassSelObjectPointerTypecheckContract
      &id_class_sel_object_pointer_typecheck_contract =
          core_lowering_plan.id_class_sel_object_pointer_typecheck_contract;
  const std::string &id_class_sel_object_pointer_typecheck_replay_key =
      core_lowering_plan.id_class_sel_object_pointer_typecheck_replay_key;
  const Objc3MessageSendSelectorLoweringContract
      &message_send_selector_lowering_contract =
          core_lowering_plan.message_send_selector_lowering_contract;
  const std::string &message_send_selector_lowering_replay_key =
      core_lowering_plan.message_send_selector_lowering_replay_key;
  const Objc3DispatchAbiMarshallingContract &dispatch_abi_marshalling_contract =
      core_lowering_plan.dispatch_abi_marshalling_contract;
  const std::string &dispatch_abi_marshalling_replay_key =
      core_lowering_plan.dispatch_abi_marshalling_replay_key;
  const Objc3NilReceiverSemanticsFoldabilityContract
      &nil_receiver_semantics_foldability_contract =
          core_lowering_plan.nil_receiver_semantics_foldability_contract;
  const std::string &nil_receiver_semantics_foldability_replay_key =
      core_lowering_plan.nil_receiver_semantics_foldability_replay_key;
  const Objc3SuperDispatchMethodFamilyContract
      &super_dispatch_method_family_contract =
          core_lowering_plan.super_dispatch_method_family_contract;
  const std::string &super_dispatch_method_family_replay_key =
      core_lowering_plan.super_dispatch_method_family_replay_key;
  const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract =
      core_lowering_plan.runtime_link_host_link_contract;
  const std::string &runtime_link_host_link_replay_key =
      core_lowering_plan.runtime_link_host_link_replay_key;
  const Objc3OwnershipQualifierLoweringContract
      &ownership_qualifier_lowering_contract =
          ownership_aware_lowering_plan.ownership_qualifier_lowering_contract;
  const std::string &ownership_qualifier_lowering_replay_key =
      ownership_aware_lowering_plan.ownership_qualifier_lowering_replay_key;
  const Objc3RetainReleaseOperationLoweringContract
      &retain_release_operation_lowering_contract =
          ownership_aware_lowering_plan
              .retain_release_operation_lowering_contract;
  const std::string &retain_release_operation_lowering_replay_key =
      ownership_aware_lowering_plan
          .retain_release_operation_lowering_replay_key;
  const Objc3AutoreleasePoolScopeLoweringContract
      &autoreleasepool_scope_lowering_contract =
          ownership_aware_lowering_plan.autoreleasepool_scope_lowering_contract;
  const std::string &autoreleasepool_scope_lowering_replay_key =
      ownership_aware_lowering_plan.autoreleasepool_scope_lowering_replay_key;
  const Objc3WeakUnownedSemanticsLoweringContract
      &weak_unowned_semantics_lowering_contract =
          ownership_aware_lowering_plan.weak_unowned_semantics_lowering_contract;
  const std::string &weak_unowned_semantics_lowering_replay_key =
      ownership_aware_lowering_plan.weak_unowned_semantics_lowering_replay_key;
  const Objc3ArcDiagnosticsFixitLoweringContract
      &arc_diagnostics_fixit_lowering_contract =
          ownership_aware_lowering_plan.arc_diagnostics_fixit_lowering_contract;
  const std::string &arc_diagnostics_fixit_lowering_replay_key =
      ownership_aware_lowering_plan.arc_diagnostics_fixit_lowering_replay_key;
  const auto &lightweight_generic_constraint_lowering_contract =
      type_system_lowering_plan.lightweight_generic_constraint_lowering_contract;
  const std::string &lightweight_generic_constraint_lowering_replay_key =
      type_system_lowering_plan
          .lightweight_generic_constraint_lowering_replay_key;
  const auto &nullability_flow_warning_precision_lowering_contract =
      type_system_lowering_plan
          .nullability_flow_warning_precision_lowering_contract;
  const std::string &nullability_flow_warning_precision_lowering_replay_key =
      type_system_lowering_plan
          .nullability_flow_warning_precision_lowering_replay_key;
  const auto &protocol_qualified_object_type_lowering_contract =
      type_system_lowering_plan
          .protocol_qualified_object_type_lowering_contract;
  const std::string &protocol_qualified_object_type_lowering_replay_key =
      type_system_lowering_plan
          .protocol_qualified_object_type_lowering_replay_key;
  const auto &variance_bridge_cast_lowering_contract =
      type_system_lowering_plan.variance_bridge_cast_lowering_contract;
  const std::string &variance_bridge_cast_lowering_replay_key =
      type_system_lowering_plan.variance_bridge_cast_lowering_replay_key;
  const auto &generic_metadata_abi_lowering_contract =
      type_system_lowering_plan.generic_metadata_abi_lowering_contract;
  const std::string &generic_metadata_abi_lowering_replay_key =
      type_system_lowering_plan.generic_metadata_abi_lowering_replay_key;
  const Objc3ModuleImportGraphLoweringContract
      &module_import_graph_lowering_contract =
          runtime_import_plan.module_import_graph_lowering_contract;
  const std::string &module_import_graph_lowering_replay_key =
      runtime_import_plan.module_import_graph_lowering_replay_key;
  const Objc3NamespaceCollisionShadowingLoweringContract
      &namespace_collision_shadowing_lowering_contract =
          module_lowering_plan.namespace_collision_shadowing_lowering_contract;
  const std::string &namespace_collision_shadowing_lowering_replay_key =
      module_lowering_plan.namespace_collision_shadowing_lowering_replay_key;
  const Objc3PublicPrivateApiPartitionLoweringContract
      &public_private_api_partition_lowering_contract =
          module_lowering_plan.public_private_api_partition_lowering_contract;
  const std::string &public_private_api_partition_lowering_replay_key =
      module_lowering_plan.public_private_api_partition_lowering_replay_key;
  const Objc3IncrementalModuleCacheInvalidationLoweringContract
      &incremental_module_cache_invalidation_lowering_contract =
          module_lowering_plan
              .incremental_module_cache_invalidation_lowering_contract;
  const std::string &incremental_module_cache_invalidation_lowering_replay_key =
      module_lowering_plan
          .incremental_module_cache_invalidation_lowering_replay_key;
  const Objc3CrossModuleConformanceLoweringContract
      &cross_module_conformance_lowering_contract =
          module_lowering_plan.cross_module_conformance_lowering_contract;
  const std::string &cross_module_conformance_lowering_replay_key =
      module_lowering_plan.cross_module_conformance_lowering_replay_key;
  const Objc3ThrowsPropagationLoweringContract
      &throws_propagation_lowering_contract =
          error_lowering_plan.throws_propagation_lowering_contract;
  const std::string &throws_propagation_lowering_replay_key =
      error_lowering_plan.throws_propagation_lowering_replay_key;
  manifest << ",\"deterministic_property_synthesis_ivar_binding_handoff\":"
           << (property_synthesis_ivar_binding_handoff_deterministic ? "true" : "false")
           << ",\"property_synthesis_sites\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_sites
           << ",\"property_synthesis_explicit_ivar_bindings\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings
           << ",\"property_synthesis_default_ivar_bindings\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings
           << ",\"interface_owned_property_synthesis_sites\":"
           << property_synthesis_ivar_binding_summary.interface_owned_property_synthesis_sites
           << ",\"implementation_property_redeclaration_sites\":"
           << property_synthesis_ivar_binding_summary.implementation_property_redeclaration_sites
           << ",\"ivar_binding_sites\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_sites
           << ",\"ivar_binding_resolved\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_resolved
           << ",\"ivar_binding_missing\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_missing
           << ",\"ivar_binding_conflicts\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_conflicts
           << ",\"lowering_property_synthesis_ivar_binding_replay_key\":\""
           << property_synthesis_ivar_binding_replay_key
           << "\""
           << ",\"deterministic_id_class_sel_object_pointer_typecheck_handoff\":"
           << (id_class_sel_object_pointer_typecheck_contract.deterministic ? "true" : "false")
           << ",\"id_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.id_typecheck_sites
           << ",\"class_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.class_typecheck_sites
           << ",\"sel_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.sel_typecheck_sites
           << ",\"object_pointer_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.object_pointer_typecheck_sites
           << ",\"id_class_sel_object_pointer_typecheck_sites_total\":"
           << id_class_sel_object_pointer_typecheck_contract.total_typecheck_sites
           << ",\"lowering_id_class_sel_object_pointer_typecheck_replay_key\":\""
           << id_class_sel_object_pointer_typecheck_replay_key
           << "\""
           << ",\"deterministic_message_send_selector_lowering_handoff\":"
           << (message_send_selector_lowering_contract.deterministic ? "true" : "false")
           << ",\"message_send_selector_lowering_sites\":"
           << message_send_selector_lowering_contract.message_send_sites
           << ",\"message_send_selector_lowering_unary_sites\":"
           << message_send_selector_lowering_contract.unary_selector_sites
           << ",\"message_send_selector_lowering_keyword_sites\":"
           << message_send_selector_lowering_contract.keyword_selector_sites
           << ",\"message_send_selector_lowering_selector_piece_sites\":"
           << message_send_selector_lowering_contract.selector_piece_sites
           << ",\"message_send_selector_lowering_argument_expression_sites\":"
           << message_send_selector_lowering_contract.argument_expression_sites
           << ",\"message_send_selector_lowering_receiver_sites\":"
           << message_send_selector_lowering_contract.receiver_expression_sites
           << ",\"message_send_selector_lowering_selector_literal_entries\":"
           << message_send_selector_lowering_contract.selector_literal_entries
           << ",\"message_send_selector_lowering_selector_literal_characters\":"
           << message_send_selector_lowering_contract.selector_literal_characters
           << ",\"lowering_message_send_selector_lowering_replay_key\":\""
           << message_send_selector_lowering_replay_key
           << "\""
           << ",\"deterministic_dispatch_abi_marshalling_handoff\":"
           << (dispatch_abi_marshalling_contract.deterministic ? "true" : "false")
           << ",\"dispatch_abi_marshalling_message_send_sites\":"
           << dispatch_abi_marshalling_contract.message_send_sites
           << ",\"dispatch_abi_marshalling_receiver_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.receiver_slots_marshaled
           << ",\"dispatch_abi_marshalling_selector_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.selector_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_value_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_value_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_padding_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_padding_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_total_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_total_slots_marshaled
           << ",\"dispatch_abi_marshalling_total_marshaled_slots\":"
           << dispatch_abi_marshalling_contract.total_marshaled_slots
           << ",\"dispatch_abi_marshalling_runtime_dispatch_arg_slots\":"
           << dispatch_abi_marshalling_contract.runtime_dispatch_arg_slots
           << ",\"lowering_dispatch_abi_marshalling_replay_key\":\""
           << dispatch_abi_marshalling_replay_key
           << "\""
           << ",\"deterministic_nil_receiver_semantics_foldability_handoff\":"
           << (nil_receiver_semantics_foldability_contract.deterministic ? "true" : "false")
           << ",\"nil_receiver_semantics_foldability_message_send_sites\":"
           << nil_receiver_semantics_foldability_contract.message_send_sites
           << ",\"nil_receiver_semantics_foldability_receiver_nil_literal_sites\":"
           << nil_receiver_semantics_foldability_contract.receiver_nil_literal_sites
           << ",\"nil_receiver_semantics_foldability_enabled_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_semantics_enabled_sites
           << ",\"nil_receiver_semantics_foldability_foldable_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_foldable_sites
           << ",\"nil_receiver_semantics_foldability_runtime_dispatch_required_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_runtime_dispatch_required_sites
           << ",\"nil_receiver_semantics_foldability_non_nil_receiver_sites\":"
           << nil_receiver_semantics_foldability_contract.non_nil_receiver_sites
           << ",\"nil_receiver_semantics_foldability_contract_violation_sites\":"
           << nil_receiver_semantics_foldability_contract.contract_violation_sites
           << ",\"lowering_nil_receiver_semantics_foldability_replay_key\":\""
           << nil_receiver_semantics_foldability_replay_key
           << "\""
           << ",\"deterministic_super_dispatch_method_family_handoff\":"
           << (super_dispatch_method_family_contract.deterministic ? "true" : "false")
           << ",\"super_dispatch_method_family_message_send_sites\":"
           << super_dispatch_method_family_contract.message_send_sites
           << ",\"super_dispatch_method_family_receiver_super_identifier_sites\":"
           << super_dispatch_method_family_contract.receiver_super_identifier_sites
           << ",\"super_dispatch_method_family_enabled_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_enabled_sites
           << ",\"super_dispatch_method_family_requires_class_context_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_requires_class_context_sites
           << ",\"super_dispatch_method_family_init_sites\":"
           << super_dispatch_method_family_contract.method_family_init_sites
           << ",\"super_dispatch_method_family_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_copy_sites
           << ",\"super_dispatch_method_family_mutable_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_mutable_copy_sites
           << ",\"super_dispatch_method_family_new_sites\":"
           << super_dispatch_method_family_contract.method_family_new_sites
           << ",\"super_dispatch_method_family_none_sites\":"
           << super_dispatch_method_family_contract.method_family_none_sites
           << ",\"super_dispatch_method_family_returns_retained_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_retained_result_sites
           << ",\"super_dispatch_method_family_returns_related_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_related_result_sites
           << ",\"super_dispatch_method_family_contract_violation_sites\":"
           << super_dispatch_method_family_contract.contract_violation_sites
           << ",\"lowering_super_dispatch_method_family_replay_key\":\""
           << super_dispatch_method_family_replay_key
           << "\""
           << ",\"deterministic_runtime_link_host_link_handoff\":"
           << (runtime_link_host_link_contract.deterministic ? "true" : "false")
           << ",\"runtime_link_host_link_message_send_sites\":"
           << runtime_link_host_link_contract.message_send_sites
           << ",\"runtime_link_host_link_required_runtime_link_sites\":"
           << runtime_link_host_link_contract.runtime_link_required_sites
           << ",\"runtime_link_host_link_elided_runtime_link_sites\":"
           << runtime_link_host_link_contract.runtime_link_elided_sites
           << ",\"runtime_link_host_link_runtime_dispatch_arg_slots\":"
           << runtime_link_host_link_contract.runtime_dispatch_arg_slots
           << ",\"runtime_link_host_link_runtime_dispatch_declaration_parameter_count\":"
           << runtime_link_host_link_contract.runtime_dispatch_declaration_parameter_count
           << ",\"runtime_link_host_link_runtime_dispatch_symbol\":\""
           << runtime_link_host_link_contract.runtime_dispatch_symbol
           << "\""
           << ",\"runtime_link_host_link_default_runtime_dispatch_symbol_binding\":"
           << (runtime_link_host_link_contract.default_runtime_dispatch_symbol_binding ? "true" : "false")
           << ",\"runtime_link_host_link_contract_violation_sites\":"
           << runtime_link_host_link_contract.contract_violation_sites
           << ",\"lowering_runtime_link_host_link_replay_key\":\""
           << runtime_link_host_link_replay_key
           << "\""
           << ",\"deterministic_ownership_qualifier_lowering_handoff\":"
           << (ownership_qualifier_lowering_contract.deterministic ? "true" : "false")
           << ",\"ownership_qualifier_lowering_type_annotation_ownership_qualifier_sites\":"
           << ownership_qualifier_lowering_contract.ownership_qualifier_sites
           << ",\"ownership_qualifier_lowering_type_annotation_invalid_ownership_qualifier_sites\":"
           << ownership_qualifier_lowering_contract.invalid_ownership_qualifier_sites
           << ",\"ownership_qualifier_lowering_type_annotation_object_pointer_type_sites\":"
           << ownership_qualifier_lowering_contract.object_pointer_type_annotation_sites
           << ",\"lowering_ownership_qualifier_replay_key\":\""
           << ownership_qualifier_lowering_replay_key
           << "\""
           << ",\"deterministic_retain_release_operation_lowering_handoff\":"
           << (retain_release_operation_lowering_contract.deterministic ? "true" : "false")
           << ",\"retain_release_operation_lowering_ownership_qualified_sites\":"
           << retain_release_operation_lowering_contract.ownership_qualified_sites
           << ",\"retain_release_operation_lowering_retain_insertion_sites\":"
           << retain_release_operation_lowering_contract.retain_insertion_sites
           << ",\"retain_release_operation_lowering_release_insertion_sites\":"
           << retain_release_operation_lowering_contract.release_insertion_sites
           << ",\"retain_release_operation_lowering_autorelease_insertion_sites\":"
           << retain_release_operation_lowering_contract.autorelease_insertion_sites
           << ",\"retain_release_operation_lowering_contract_violation_sites\":"
           << retain_release_operation_lowering_contract.contract_violation_sites
           << ",\"lowering_retain_release_operation_replay_key\":\""
           << retain_release_operation_lowering_replay_key
           << "\""
           << ",\"deterministic_autoreleasepool_scope_lowering_handoff\":"
           << (autoreleasepool_scope_lowering_contract.deterministic ? "true" : "false")
           << ",\"autoreleasepool_scope_lowering_scope_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_sites
           << ",\"autoreleasepool_scope_lowering_scope_symbolized_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_symbolized_sites
           << ",\"autoreleasepool_scope_lowering_max_scope_depth\":"
           << autoreleasepool_scope_lowering_contract.max_scope_depth
           << ",\"autoreleasepool_scope_lowering_scope_entry_transition_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_entry_transition_sites
           << ",\"autoreleasepool_scope_lowering_scope_exit_transition_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_exit_transition_sites
           << ",\"autoreleasepool_scope_lowering_contract_violation_sites\":"
           << autoreleasepool_scope_lowering_contract.contract_violation_sites
           << ",\"lowering_autoreleasepool_scope_replay_key\":\""
           << autoreleasepool_scope_lowering_replay_key
           << "\""
           << ",\"deterministic_weak_unowned_semantics_lowering_handoff\":"
           << (weak_unowned_semantics_lowering_contract.deterministic ? "true" : "false")
           << ",\"weak_unowned_semantics_lowering_ownership_candidate_sites\":"
           << weak_unowned_semantics_lowering_contract.ownership_candidate_sites
           << ",\"weak_unowned_semantics_lowering_weak_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.weak_reference_sites
           << ",\"weak_unowned_semantics_lowering_unowned_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.unowned_reference_sites
           << ",\"weak_unowned_semantics_lowering_unowned_safe_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.unowned_safe_reference_sites
           << ",\"weak_unowned_semantics_lowering_conflict_sites\":"
           << weak_unowned_semantics_lowering_contract.weak_unowned_conflict_sites
           << ",\"weak_unowned_semantics_lowering_contract_violation_sites\":"
           << weak_unowned_semantics_lowering_contract.contract_violation_sites
           << ",\"lowering_weak_unowned_semantics_replay_key\":\""
           << weak_unowned_semantics_lowering_replay_key
           << "\""
           << ",\"deterministic_arc_diagnostics_fixit_lowering_handoff\":"
           << (arc_diagnostics_fixit_lowering_contract.deterministic ? "true" : "false")
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_diagnostic_candidate_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_fixit_available_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_profiled_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_weak_unowned_conflict_diagnostic_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_empty_fixit_hint_sites
           << ",\"arc_diagnostics_fixit_lowering_contract_violation_sites\":"
           << arc_diagnostics_fixit_lowering_contract.contract_violation_sites
           << ",\"lowering_arc_diagnostics_fixit_replay_key\":\""
           << arc_diagnostics_fixit_lowering_replay_key
           << "\"";
  AppendObjc3FrontendArtifactLoweringHandoffBlockManifestFields(
      manifest, block_lowering_plan);
  manifest
           << ",\"deterministic_lightweight_generic_constraint_lowering_handoff\":"
           << (lightweight_generic_constraint_lowering_contract.deterministic ? "true" : "false")
           << ",\"lightweight_generic_constraint_lowering_sites\":"
           << lightweight_generic_constraint_lowering_contract.generic_constraint_sites
           << ",\"lightweight_generic_constraint_lowering_generic_suffix_sites\":"
           << lightweight_generic_constraint_lowering_contract.generic_suffix_sites
           << ",\"lightweight_generic_constraint_lowering_object_pointer_type_sites\":"
           << lightweight_generic_constraint_lowering_contract.object_pointer_type_sites
           << ",\"lightweight_generic_constraint_lowering_terminated_generic_suffix_sites\":"
           << lightweight_generic_constraint_lowering_contract.terminated_generic_suffix_sites
           << ",\"lightweight_generic_constraint_lowering_pointer_declarator_sites\":"
           << lightweight_generic_constraint_lowering_contract.pointer_declarator_sites
           << ",\"lightweight_generic_constraint_lowering_normalized_sites\":"
           << lightweight_generic_constraint_lowering_contract.normalized_constraint_sites
           << ",\"lightweight_generic_constraint_lowering_contract_violation_sites\":"
           << lightweight_generic_constraint_lowering_contract.contract_violation_sites
           << ",\"lowering_lightweight_generic_constraint_replay_key\":\""
           << lightweight_generic_constraint_lowering_replay_key
           << "\""
           << ",\"deterministic_nullability_flow_warning_precision_lowering_handoff\":"
           << (nullability_flow_warning_precision_lowering_contract.deterministic ? "true" : "false")
           << ",\"nullability_flow_warning_precision_lowering_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullability_flow_sites
           << ",\"nullability_flow_warning_precision_lowering_object_pointer_type_sites\":"
           << nullability_flow_warning_precision_lowering_contract.object_pointer_type_sites
           << ",\"nullability_flow_warning_precision_lowering_nullability_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullability_suffix_sites
           << ",\"nullability_flow_warning_precision_lowering_nullable_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullable_suffix_sites
           << ",\"nullability_flow_warning_precision_lowering_nonnull_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nonnull_suffix_sites
           << ",\"nullability_flow_warning_precision_lowering_normalized_sites\":"
           << nullability_flow_warning_precision_lowering_contract.normalized_sites
           << ",\"nullability_flow_warning_precision_lowering_contract_violation_sites\":"
           << nullability_flow_warning_precision_lowering_contract.contract_violation_sites
           << ",\"lowering_nullability_flow_warning_precision_replay_key\":\""
           << nullability_flow_warning_precision_lowering_replay_key
           << "\""
           << ",\"deterministic_protocol_qualified_object_type_lowering_handoff\":"
           << (protocol_qualified_object_type_lowering_contract.deterministic ? "true" : "false")
           << ",\"protocol_qualified_object_type_lowering_sites\":"
           << protocol_qualified_object_type_lowering_contract.protocol_qualified_object_type_sites
           << ",\"protocol_qualified_object_type_lowering_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.protocol_composition_sites
           << ",\"protocol_qualified_object_type_lowering_object_pointer_type_sites\":"
           << protocol_qualified_object_type_lowering_contract.object_pointer_type_sites
           << ",\"protocol_qualified_object_type_lowering_terminated_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.terminated_protocol_composition_sites
           << ",\"protocol_qualified_object_type_lowering_pointer_declarator_sites\":"
           << protocol_qualified_object_type_lowering_contract.pointer_declarator_sites
           << ",\"protocol_qualified_object_type_lowering_normalized_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.normalized_protocol_composition_sites
           << ",\"protocol_qualified_object_type_lowering_contract_violation_sites\":"
           << protocol_qualified_object_type_lowering_contract.contract_violation_sites
           << ",\"lowering_protocol_qualified_object_type_replay_key\":\""
           << protocol_qualified_object_type_lowering_replay_key
           << "\""
           << ",\"deterministic_variance_bridge_cast_lowering_handoff\":"
           << (variance_bridge_cast_lowering_contract.deterministic ? "true" : "false")
           << ",\"variance_bridge_cast_lowering_sites\":"
           << variance_bridge_cast_lowering_contract.variance_bridge_cast_sites
           << ",\"variance_bridge_cast_lowering_protocol_composition_sites\":"
           << variance_bridge_cast_lowering_contract.protocol_composition_sites
           << ",\"variance_bridge_cast_lowering_ownership_qualifier_sites\":"
           << variance_bridge_cast_lowering_contract.ownership_qualifier_sites
           << ",\"variance_bridge_cast_lowering_object_pointer_type_sites\":"
           << variance_bridge_cast_lowering_contract.object_pointer_type_sites
           << ",\"variance_bridge_cast_lowering_pointer_declarator_sites\":"
           << variance_bridge_cast_lowering_contract.pointer_declarator_sites
           << ",\"variance_bridge_cast_lowering_normalized_sites\":"
           << variance_bridge_cast_lowering_contract.normalized_sites
           << ",\"variance_bridge_cast_lowering_contract_violation_sites\":"
           << variance_bridge_cast_lowering_contract.contract_violation_sites
           << ",\"lowering_variance_bridge_cast_replay_key\":\""
           << variance_bridge_cast_lowering_replay_key
           << "\""
           << ",\"deterministic_generic_metadata_abi_lowering_handoff\":"
           << (generic_metadata_abi_lowering_contract.deterministic ? "true" : "false")
           << ",\"generic_metadata_abi_lowering_sites\":"
           << generic_metadata_abi_lowering_contract.generic_metadata_abi_sites
           << ",\"generic_metadata_abi_lowering_generic_suffix_sites\":"
           << generic_metadata_abi_lowering_contract.generic_suffix_sites
           << ",\"generic_metadata_abi_lowering_protocol_composition_sites\":"
           << generic_metadata_abi_lowering_contract.protocol_composition_sites
           << ",\"generic_metadata_abi_lowering_ownership_qualifier_sites\":"
           << generic_metadata_abi_lowering_contract.ownership_qualifier_sites
           << ",\"generic_metadata_abi_lowering_object_pointer_type_sites\":"
           << generic_metadata_abi_lowering_contract.object_pointer_type_sites
           << ",\"generic_metadata_abi_lowering_pointer_declarator_sites\":"
           << generic_metadata_abi_lowering_contract.pointer_declarator_sites
           << ",\"generic_metadata_abi_lowering_normalized_sites\":"
           << generic_metadata_abi_lowering_contract.normalized_sites
           << ",\"generic_metadata_abi_lowering_contract_violation_sites\":"
           << generic_metadata_abi_lowering_contract.contract_violation_sites
           << ",\"lowering_generic_metadata_abi_replay_key\":\""
           << generic_metadata_abi_lowering_replay_key
           << "\""
           << ",\"deterministic_module_import_graph_lowering_handoff\":"
           << (module_import_graph_lowering_contract.deterministic ? "true" : "false")
           << ",\"module_import_graph_lowering_sites\":"
           << module_import_graph_lowering_contract.module_import_graph_sites
           << ",\"module_import_graph_lowering_import_edge_candidate_sites\":"
           << module_import_graph_lowering_contract.import_edge_candidate_sites
           << ",\"module_import_graph_lowering_namespace_segment_sites\":"
           << module_import_graph_lowering_contract.namespace_segment_sites
           << ",\"module_import_graph_lowering_object_pointer_type_sites\":"
           << module_import_graph_lowering_contract.object_pointer_type_sites
           << ",\"module_import_graph_lowering_pointer_declarator_sites\":"
           << module_import_graph_lowering_contract.pointer_declarator_sites
           << ",\"module_import_graph_lowering_normalized_sites\":"
           << module_import_graph_lowering_contract.normalized_sites
           << ",\"module_import_graph_lowering_contract_violation_sites\":"
           << module_import_graph_lowering_contract.contract_violation_sites
           << ",\"lowering_module_import_graph_replay_key\":\""
           << module_import_graph_lowering_replay_key
           << "\""
           << ",\"deterministic_namespace_collision_shadowing_lowering_handoff\":"
           << (namespace_collision_shadowing_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"namespace_collision_shadowing_lowering_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .namespace_collision_shadowing_sites
           << ",\"namespace_collision_shadowing_lowering_namespace_segment_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .namespace_segment_sites
           << ",\"namespace_collision_shadowing_lowering_import_edge_candidate_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .import_edge_candidate_sites
           << ",\"namespace_collision_shadowing_lowering_object_pointer_type_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .object_pointer_type_sites
           << ",\"namespace_collision_shadowing_lowering_pointer_declarator_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .pointer_declarator_sites
           << ",\"namespace_collision_shadowing_lowering_normalized_sites\":"
           << namespace_collision_shadowing_lowering_contract.normalized_sites
           << ",\"namespace_collision_shadowing_lowering_contract_violation_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .contract_violation_sites
           << ",\"lowering_namespace_collision_shadowing_replay_key\":\""
           << namespace_collision_shadowing_lowering_replay_key
           << "\""
           << ",\"deterministic_public_private_api_partition_lowering_handoff\":"
           << (public_private_api_partition_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"public_private_api_partition_lowering_sites\":"
           << public_private_api_partition_lowering_contract
                  .public_private_api_partition_sites
           << ",\"public_private_api_partition_lowering_namespace_segment_sites\":"
           << public_private_api_partition_lowering_contract
                  .namespace_segment_sites
           << ",\"public_private_api_partition_lowering_import_edge_candidate_sites\":"
           << public_private_api_partition_lowering_contract
                  .import_edge_candidate_sites
           << ",\"public_private_api_partition_lowering_object_pointer_type_sites\":"
           << public_private_api_partition_lowering_contract
                  .object_pointer_type_sites
           << ",\"public_private_api_partition_lowering_pointer_declarator_sites\":"
           << public_private_api_partition_lowering_contract
                  .pointer_declarator_sites
           << ",\"public_private_api_partition_lowering_normalized_sites\":"
           << public_private_api_partition_lowering_contract.normalized_sites
           << ",\"public_private_api_partition_lowering_contract_violation_sites\":"
           << public_private_api_partition_lowering_contract
                  .contract_violation_sites
           << ",\"lowering_public_private_api_partition_replay_key\":\""
           << public_private_api_partition_lowering_replay_key
           << "\""
           << ",\"deterministic_incremental_module_cache_invalidation_lowering_handoff\":"
           << (incremental_module_cache_invalidation_lowering_contract
                       .deterministic
                   ? "true"
                   : "false")
           << ",\"incremental_module_cache_invalidation_lowering_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .incremental_module_cache_invalidation_sites
           << ",\"incremental_module_cache_invalidation_lowering_namespace_segment_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .namespace_segment_sites
           << ",\"incremental_module_cache_invalidation_lowering_import_edge_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .import_edge_candidate_sites
           << ",\"incremental_module_cache_invalidation_lowering_object_pointer_type_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .object_pointer_type_sites
           << ",\"incremental_module_cache_invalidation_lowering_pointer_declarator_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .pointer_declarator_sites
           << ",\"incremental_module_cache_invalidation_lowering_normalized_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .normalized_sites
           << ",\"incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"incremental_module_cache_invalidation_lowering_contract_violation_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .contract_violation_sites
           << ",\"lowering_incremental_module_cache_invalidation_replay_key\":\""
           << incremental_module_cache_invalidation_lowering_replay_key
           << "\""
           << ",\"deterministic_cross_module_conformance_lowering_handoff\":"
           << (cross_module_conformance_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"cross_module_conformance_lowering_sites\":"
           << cross_module_conformance_lowering_contract
                  .cross_module_conformance_sites
           << ",\"cross_module_conformance_lowering_namespace_segment_sites\":"
           << cross_module_conformance_lowering_contract.namespace_segment_sites
           << ",\"cross_module_conformance_lowering_import_edge_candidate_sites\":"
           << cross_module_conformance_lowering_contract
                  .import_edge_candidate_sites
           << ",\"cross_module_conformance_lowering_object_pointer_type_sites\":"
           << cross_module_conformance_lowering_contract.object_pointer_type_sites
           << ",\"cross_module_conformance_lowering_pointer_declarator_sites\":"
           << cross_module_conformance_lowering_contract.pointer_declarator_sites
           << ",\"cross_module_conformance_lowering_normalized_sites\":"
           << cross_module_conformance_lowering_contract.normalized_sites
           << ",\"cross_module_conformance_lowering_cache_invalidation_candidate_sites\":"
           << cross_module_conformance_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"cross_module_conformance_lowering_contract_violation_sites\":"
           << cross_module_conformance_lowering_contract.contract_violation_sites
           << ",\"lowering_cross_module_conformance_replay_key\":\""
           << cross_module_conformance_lowering_replay_key
           << "\""
           << ",\"deterministic_throws_propagation_lowering_handoff\":"
           << (throws_propagation_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"throws_propagation_lowering_sites\":"
           << throws_propagation_lowering_contract.throws_propagation_sites
           << ",\"throws_propagation_lowering_namespace_segment_sites\":"
           << throws_propagation_lowering_contract.namespace_segment_sites
           << ",\"throws_propagation_lowering_import_edge_candidate_sites\":"
           << throws_propagation_lowering_contract.import_edge_candidate_sites
           << ",\"throws_propagation_lowering_object_pointer_type_sites\":"
           << throws_propagation_lowering_contract.object_pointer_type_sites
           << ",\"throws_propagation_lowering_pointer_declarator_sites\":"
           << throws_propagation_lowering_contract.pointer_declarator_sites
           << ",\"throws_propagation_lowering_normalized_sites\":"
           << throws_propagation_lowering_contract.normalized_sites
           << ",\"throws_propagation_lowering_cache_invalidation_candidate_sites\":"
           << throws_propagation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"throws_propagation_lowering_contract_violation_sites\":"
           << throws_propagation_lowering_contract.contract_violation_sites
           << ",\"lowering_throws_propagation_replay_key\":\""
           << throws_propagation_lowering_replay_key
           << "\""
           << ",\"deterministic_object_pointer_nullability_generics_handoff\":"
           << (object_pointer_nullability_generics_summary.deterministic_object_pointer_nullability_generics_handoff
                   ? "true"
                   : "false")
           << ",\"object_pointer_type_spellings\":"
           << object_pointer_nullability_generics_summary.object_pointer_type_spellings
           << ",\"pointer_declarator_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_entries
           << ",\"pointer_declarator_depth_total\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_depth_total
           << ",\"pointer_declarator_token_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_token_entries
           << ",\"nullability_suffix_entries\":"
           << object_pointer_nullability_generics_summary.nullability_suffix_entries
           << ",\"generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.generic_suffix_entries
           << ",\"terminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.terminated_generic_suffix_entries
           << ",\"unterminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.unterminated_generic_suffix_entries
           << ",\"symbol_graph_global_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.global_symbol_nodes
           << ",\"symbol_graph_function_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.function_symbol_nodes
           << ",\"symbol_graph_interface_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_symbol_nodes
           << ",\"symbol_graph_implementation_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_symbol_nodes
           << ",\"symbol_graph_interface_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_property_symbol_nodes
           << ",\"symbol_graph_implementation_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes
           << ",\"symbol_graph_interface_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_method_symbol_nodes
           << ",\"symbol_graph_implementation_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes
           << ",\"scope_resolution_top_level_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.top_level_scope_symbols
           << ",\"scope_resolution_nested_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.nested_scope_symbols
           << ",\"scope_resolution_scope_frames_total\":"
           << symbol_graph_scope_resolution_summary.scope_frames_total
           << ",\"scope_resolution_implementation_interface_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites
           << ",\"scope_resolution_implementation_interface_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits
           << ",\"scope_resolution_implementation_interface_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses
           << ",\"scope_resolution_method_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.method_resolution_sites
           << ",\"scope_resolution_method_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.method_resolution_hits
           << ",\"scope_resolution_method_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.method_resolution_misses
           << ",\"deterministic_symbol_graph_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff ? "true" : "false")
           << ",\"deterministic_scope_resolution_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff ? "true" : "false")
           << ",\"symbol_graph_scope_resolution_handoff_key\":\""
           << symbol_graph_scope_resolution_summary.deterministic_handoff_key
           << "\"},\n";
}

}  // namespace objc3::artifacts::frontend

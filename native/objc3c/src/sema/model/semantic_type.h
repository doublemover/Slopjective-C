#pragma once

struct Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary {
  std::string contract_id =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationContractId;
  std::string source_frontend_closure_contract_id =
      kObjc3RuntimeAwareImportModuleFrontendClosureContractId;
  std::string frontend_surface_path =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationSurfacePath;
  std::string source_artifact_relative_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath;
  std::string authority_model =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationAuthorityModel;
  std::string conformance_shape_model =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationConformanceShapeModel;
  std::string dispatch_trait_model =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationDispatchTraitModel;
  std::string effect_trait_model =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationEffectTraitModel;
  std::string module_name;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::size_t runtime_owned_declaration_count = 0;
  std::size_t superclass_edge_count = 0;
  std::size_t protocol_conformance_edge_count = 0;
  std::size_t category_attachment_count = 0;
  std::size_t property_accessor_trait_count = 0;
  std::size_t property_ivar_binding_trait_count = 0;
  std::size_t method_selector_trait_count = 0;
  std::size_t class_method_trait_count = 0;
  std::size_t instance_method_trait_count = 0;
  std::size_t implemented_method_count = 0;
  std::size_t declaration_only_method_count = 0;
  std::size_t property_attribute_profile_count = 0;
  std::size_t ownership_effect_profile_count = 0;
  std::size_t executable_binding_trait_count = 0;
  std::size_t optional_send_site_count = 0;
  std::size_t typed_keypath_literal_site_count = 0;
  std::size_t live_optional_lowering_site_count = 0;
  std::size_t live_typed_keypath_artifact_site_count = 0;
  std::size_t imported_type_system_optional_keypath_module_count = 0;
  std::size_t imported_optional_runtime_ready_module_count = 0;
  std::size_t imported_typed_keypath_runtime_ready_module_count = 0;
  bool fail_closed = false;
  bool source_frontend_closure_ready = false;
  bool runtime_metadata_source_records_ready = false;
  bool semantic_surface_published = false;
  bool imported_conformance_shape_landed = false;
  bool imported_dispatch_traits_landed = false;
  bool imported_effect_traits_landed = false;
  bool imported_runtime_metadata_semantics_landed = false;
  bool imported_type_system_type_surface_landed = false;
  bool imported_optional_runtime_semantics_landed = false;
  bool imported_typed_keypath_runtime_semantics_landed = false;
  bool ready_for_imported_metadata_semantic_rules = false;
  bool ready_for_cross_module_dispatch_equivalence = false;
  std::string source_frontend_closure_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3CrossModuleRuntimeMetadataSemanticPreservationSummary(
    const Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.source_frontend_closure_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.source_artifact_relative_path.empty() &&
         !summary.authority_model.empty() &&
         !summary.conformance_shape_model.empty() &&
         !summary.dispatch_trait_model.empty() &&
         !summary.effect_trait_model.empty() && !summary.module_name.empty() &&
         summary.runtime_owned_declaration_count ==
             summary.class_record_count + summary.protocol_record_count +
                 summary.category_record_count + summary.property_record_count +
                 summary.method_record_count + summary.ivar_record_count &&
         summary.method_record_count ==
             summary.class_method_trait_count +
                 summary.instance_method_trait_count &&
         summary.method_record_count ==
             summary.implemented_method_count +
                 summary.declaration_only_method_count &&
         summary.fail_closed && summary.source_frontend_closure_ready &&
         summary.runtime_metadata_source_records_ready &&
         summary.semantic_surface_published &&
         !summary.imported_conformance_shape_landed &&
         !summary.imported_dispatch_traits_landed &&
         !summary.imported_effect_traits_landed &&
         !summary.imported_runtime_metadata_semantics_landed &&
         !summary.ready_for_imported_metadata_semantic_rules &&
         !summary.ready_for_cross_module_dispatch_equivalence &&
         !summary.source_frontend_closure_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3ImportedRuntimeMetadataSemanticRulesContractId =
    "objc3c.imported.runtime.metadata.semantic.rules.v1";
inline constexpr const char *kObjc3ImportedRuntimeMetadataSemanticRulesSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_imported_runtime_metadata_semantic_rules";
inline constexpr const char *kObjc3ImportedRuntimeMetadataSemanticRulesAuthorityModel =
    "import-surface-artifact-consumption-derived-semantic-rules";
inline constexpr const char *kObjc3ImportedRuntimeMetadataSemanticRulesInputModel =
    "filesystem-runtime-import-surface-artifact-path-list";

struct Objc3ImportedRuntimeMetadataSemanticRulesSummary {
  std::string contract_id =
      kObjc3ImportedRuntimeMetadataSemanticRulesContractId;
  std::string source_semantic_preservation_contract_id =
      kObjc3CrossModuleRuntimeMetadataSemanticPreservationContractId;
  std::string frontend_surface_path =
      kObjc3ImportedRuntimeMetadataSemanticRulesSurfacePath;
  std::string authority_model =
      kObjc3ImportedRuntimeMetadataSemanticRulesAuthorityModel;
  std::string input_model = kObjc3ImportedRuntimeMetadataSemanticRulesInputModel;
  std::vector<std::string> imported_module_names_lexicographic;
  std::size_t imported_input_path_count = 0;
  std::size_t imported_module_count = 0;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::size_t runtime_owned_declaration_count = 0;
  std::size_t superclass_edge_count = 0;
  std::size_t protocol_conformance_edge_count = 0;
  std::size_t category_attachment_count = 0;
  std::size_t property_accessor_trait_count = 0;
  std::size_t property_ivar_binding_trait_count = 0;
  std::size_t method_selector_trait_count = 0;
  std::size_t class_method_trait_count = 0;
  std::size_t instance_method_trait_count = 0;
  std::size_t implemented_method_count = 0;
  std::size_t declaration_only_method_count = 0;
  std::size_t property_attribute_profile_count = 0;
  std::size_t ownership_effect_profile_count = 0;
  std::size_t executable_binding_trait_count = 0;
  std::size_t optional_send_site_count = 0;
  std::size_t typed_keypath_literal_site_count = 0;
  std::size_t live_optional_lowering_site_count = 0;
  std::size_t live_typed_keypath_artifact_site_count = 0;
  std::size_t imported_type_system_optional_keypath_module_count = 0;
  std::size_t imported_optional_runtime_ready_module_count = 0;
  std::size_t imported_typed_keypath_runtime_ready_module_count = 0;
  std::size_t imported_type_system_generic_contract_module_count = 0;
  std::size_t imported_generic_interface_count = 0;
  std::size_t imported_generic_parameter_count = 0;
  std::size_t imported_generic_variance_annotation_count = 0;
  std::size_t imported_generic_argument_reference_count = 0;
  std::size_t imported_protocol_qualified_generic_argument_count = 0;
  std::size_t imported_type_system_nullability_contract_module_count = 0;
  std::size_t imported_nullability_canonical_type_count = 0;
  std::size_t imported_nullability_object_type_count = 0;
  std::size_t imported_nullable_entry_count = 0;
  std::size_t imported_nonnull_entry_count = 0;
  std::size_t imported_implicitly_unwrapped_entry_count = 0;
  std::size_t imported_null_resettable_entry_count = 0;
  std::size_t imported_unspecified_nullability_entry_count = 0;
  std::size_t imported_invalid_nullability_entry_count = 0;
  std::size_t imported_type_system_protocol_contract_module_count = 0;
  std::size_t imported_protocol_decl_count = 0;
  std::size_t imported_protocol_forward_declaration_count = 0;
  std::size_t imported_protocol_inheritance_edge_count = 0;
  std::size_t imported_protocol_required_method_count = 0;
  std::size_t imported_protocol_optional_method_count = 0;
  std::size_t imported_protocol_required_property_count = 0;
  std::size_t imported_protocol_optional_property_count = 0;
  std::size_t imported_class_protocol_adoption_count = 0;
  std::size_t imported_category_protocol_adoption_count = 0;
  bool fail_closed = false;
  bool source_semantic_preservation_contract_ready = false;
  bool semantic_surface_published = false;
  bool imported_runtime_surface_inputs_present = false;
  bool imported_runtime_surface_inputs_loaded = false;
  bool imported_conformance_shape_landed = false;
  bool imported_dispatch_traits_landed = false;
  bool imported_effect_traits_landed = false;
  bool imported_runtime_metadata_semantics_landed = false;
  bool imported_type_system_type_surface_landed = false;
  bool imported_optional_runtime_semantics_landed = false;
  bool imported_typed_keypath_runtime_semantics_landed = false;
  bool ready_for_imported_metadata_semantic_rules = false;
  bool ready_for_cross_module_dispatch_equivalence = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.source_semantic_preservation_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.authority_model.empty() && !summary.input_model.empty() &&
         summary.imported_module_count ==
             summary.imported_module_names_lexicographic.size() &&
         summary.runtime_owned_declaration_count ==
             summary.class_record_count + summary.protocol_record_count +
                 summary.category_record_count + summary.property_record_count +
                 summary.method_record_count + summary.ivar_record_count &&
         summary.method_record_count ==
             summary.class_method_trait_count +
                 summary.instance_method_trait_count &&
         summary.method_record_count ==
             summary.implemented_method_count +
                 summary.declaration_only_method_count &&
         summary.imported_input_path_count >= summary.imported_module_count &&
         summary.fail_closed &&
         summary.source_semantic_preservation_contract_ready &&
         summary.semantic_surface_published &&
         summary.imported_runtime_surface_inputs_loaded &&
         summary.imported_conformance_shape_landed &&
         summary.imported_dispatch_traits_landed &&
         summary.imported_effect_traits_landed &&
         summary.imported_runtime_metadata_semantics_landed &&
         summary.imported_type_system_type_surface_landed &&
         summary.ready_for_imported_metadata_semantic_rules &&
         summary.ready_for_cross_module_dispatch_equivalence &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3SerializedRuntimeMetadataImportLoweringContractId =
    "objc3c.serialized.runtime.metadata.import.lowering.v1";
inline constexpr const char *kObjc3SerializedRuntimeMetadataImportLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_serialized_runtime_metadata_import_lowering_contract";
inline constexpr const char *kObjc3SerializedRuntimeMetadataImportLoweringAuthorityModel =
    "serialized-metadata-import-lowering-freeze-derived-from-imported-runtime-semantic-rules";
inline constexpr const char *kObjc3SerializedRuntimeMetadataImportLoweringInputModel =
    "filesystem-runtime-import-surface-artifact-path-list";

struct Objc3SerializedRuntimeMetadataImportLoweringSummary {
  std::string contract_id =
      kObjc3SerializedRuntimeMetadataImportLoweringContractId;
  std::string source_imported_semantic_rules_contract_id =
      kObjc3ImportedRuntimeMetadataSemanticRulesContractId;
  std::string frontend_surface_path =
      kObjc3SerializedRuntimeMetadataImportLoweringSurfacePath;
  std::string source_artifact_relative_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath;
  std::string authority_model =
      kObjc3SerializedRuntimeMetadataImportLoweringAuthorityModel;
  std::string input_model =
      kObjc3SerializedRuntimeMetadataImportLoweringInputModel;
  std::size_t imported_input_path_count = 0;
  std::size_t imported_module_count = 0;
  bool fail_closed = false;
  bool source_imported_semantic_rules_ready = false;
  bool semantic_surface_published = false;
  bool imported_surface_ingest_landed = false;
  bool serialized_metadata_rehydration_landed = false;
  bool incremental_reuse_landed = false;
  bool imported_metadata_ir_lowering_landed = false;
  bool public_live_imported_payload_abi_landed = false;
  bool ready_for_serialized_metadata_lowering_impl = false;
  bool ready_for_incremental_reuse_impl = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3SerializedRuntimeMetadataImportLoweringSummary(
    const Objc3SerializedRuntimeMetadataImportLoweringSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.source_imported_semantic_rules_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.source_artifact_relative_path.empty() &&
         !summary.authority_model.empty() && !summary.input_model.empty() &&
         summary.imported_input_path_count >= summary.imported_module_count &&
         summary.fail_closed &&
         summary.source_imported_semantic_rules_ready &&
         summary.semantic_surface_published &&
         summary.imported_surface_ingest_landed &&
         !summary.serialized_metadata_rehydration_landed &&
         !summary.incremental_reuse_landed &&
         !summary.imported_metadata_ir_lowering_landed &&
         !summary.public_live_imported_payload_abi_landed &&
         !summary.ready_for_serialized_metadata_lowering_impl &&
         !summary.ready_for_incremental_reuse_impl &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3SerializedRuntimeMetadataArtifactReuseContractId =
    "objc3c.serialized.runtime.metadata.artifact.reuse.v1";
inline constexpr const char *kObjc3SerializedRuntimeMetadataArtifactReuseSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_serialized_runtime_metadata_artifact_reuse";
inline constexpr const char *kObjc3SerializedRuntimeMetadataArtifactReusePayloadMemberName =
    "serialized_runtime_metadata_reuse_payload";
inline constexpr const char *kObjc3SerializedRuntimeMetadataArtifactReuseAuthorityModel =
    "runtime-import-surface-nested-serialized-runtime-metadata-payload";
inline constexpr const char *kObjc3SerializedRuntimeMetadataArtifactReuseInputModel =
    "filesystem-runtime-import-surface-artifact-path-list";

struct Objc3SerializedRuntimeMetadataArtifactReuseSummary {
  std::string contract_id =
      kObjc3SerializedRuntimeMetadataArtifactReuseContractId;
  std::string source_serialized_import_lowering_contract_id =
      kObjc3SerializedRuntimeMetadataImportLoweringContractId;
  std::string frontend_surface_path =
      kObjc3SerializedRuntimeMetadataArtifactReuseSurfacePath;
  std::string artifact_relative_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath;
  std::string payload_member_name =
      kObjc3SerializedRuntimeMetadataArtifactReusePayloadMemberName;
  std::string authority_model =
      kObjc3SerializedRuntimeMetadataArtifactReuseAuthorityModel;
  std::string input_model =
      kObjc3SerializedRuntimeMetadataArtifactReuseInputModel;
  std::vector<std::string> reused_module_names_lexicographic;
  std::size_t reused_module_count = 0;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::size_t runtime_owned_declaration_count = 0;
  std::size_t metadata_reference_count = 0;
  bool fail_closed = false;
  bool source_serialized_import_lowering_ready = false;
  bool semantic_surface_published = false;
  bool serialized_metadata_rehydration_landed = false;
  bool artifact_reuse_landed = false;
  bool downstream_module_consumption_ready = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.source_serialized_import_lowering_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.artifact_relative_path.empty() &&
         !summary.payload_member_name.empty() &&
         !summary.authority_model.empty() && !summary.input_model.empty() &&
         summary.reused_module_count ==
             summary.reused_module_names_lexicographic.size() &&
         summary.runtime_owned_declaration_count ==
             summary.class_record_count + summary.protocol_record_count +
                 summary.category_record_count + summary.property_record_count +
                 summary.method_record_count + summary.ivar_record_count &&
         summary.fail_closed &&
         summary.source_serialized_import_lowering_ready &&
         summary.semantic_surface_published &&
         summary.serialized_metadata_rehydration_landed &&
         summary.artifact_reuse_landed &&
         summary.downstream_module_consumption_ready &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationContractId =
        "objc3c.cross.module.build.runtime.orchestration.v1";
inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_cross_module_build_runtime_orchestration_contract";
inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationAuthorityModel =
        "serialized-runtime-import-surface-reuse-payload-plus-local-registration-manifest";
inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationInputModel =
        "filesystem-runtime-import-surface-artifact-path-list-plus-local-registration-manifest";
inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationRegistrationScopeModel =
        "translation-unit-registration-manifests-remain-image-local-until-cross-module-registration-aggregation-lands";
inline constexpr const char
    *kObjc3CrossModuleBuildRuntimeOrchestrationPackagingModel =
        "no-cross-module-link-plan-artifact-or-imported-registration-manifest-ingest-during-freeze";

struct Objc3CrossModuleBuildRuntimeOrchestrationSummary {
  std::string contract_id =
      kObjc3CrossModuleBuildRuntimeOrchestrationContractId;
  std::string source_serialized_runtime_metadata_artifact_reuse_contract_id =
      kObjc3SerializedRuntimeMetadataArtifactReuseContractId;
  std::string source_local_registration_manifest_contract_id =
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
  std::string source_imported_runtime_metadata_semantic_rules_contract_id =
      kObjc3ImportedRuntimeMetadataSemanticRulesContractId;
  std::string frontend_surface_path =
      kObjc3CrossModuleBuildRuntimeOrchestrationSurfacePath;
  std::string import_artifact_relative_path =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath;
  std::string local_registration_manifest_artifact_relative_path =
      kObjc3RuntimeTranslationUnitRegistrationManifestArtifactRelativePath;
  std::string authority_model =
      kObjc3CrossModuleBuildRuntimeOrchestrationAuthorityModel;
  std::string input_model =
      kObjc3CrossModuleBuildRuntimeOrchestrationInputModel;
  std::string registration_scope_model =
      kObjc3CrossModuleBuildRuntimeOrchestrationRegistrationScopeModel;
  std::string packaging_model =
      kObjc3CrossModuleBuildRuntimeOrchestrationPackagingModel;
  std::vector<std::string> module_names_lexicographic;
  std::size_t module_image_count = 0;
  std::size_t direct_import_input_count = 0;
  std::size_t local_class_descriptor_count = 0;
  std::size_t local_protocol_descriptor_count = 0;
  std::size_t local_category_descriptor_count = 0;
  std::size_t local_property_descriptor_count = 0;
  std::size_t local_ivar_descriptor_count = 0;
  std::size_t local_total_descriptor_count = 0;
  std::size_t transitive_runtime_owned_declaration_count = 0;
  std::size_t transitive_metadata_reference_count = 0;
  std::size_t imported_optional_send_site_count = 0;
  std::size_t imported_typed_keypath_literal_site_count = 0;
  std::size_t imported_live_optional_lowering_site_count = 0;
  std::size_t imported_live_typed_keypath_artifact_site_count = 0;
  bool fail_closed = false;
  bool source_serialized_runtime_metadata_artifact_reuse_ready = false;
  bool source_local_registration_manifest_ready = false;
  bool source_imported_runtime_metadata_semantic_rules_ready = false;
  bool semantic_surface_published = false;
  bool local_registration_manifest_emitted = false;
  bool imported_type_system_type_surface_landed = false;
  bool imported_optional_runtime_semantics_landed = false;
  bool imported_typed_keypath_runtime_semantics_landed = false;
  bool cross_module_link_plan_artifact_landed = false;
  bool imported_registration_manifest_loading_landed = false;
  bool runtime_archive_aggregation_landed = false;
  bool cross_module_runtime_registration_landed = false;
  bool cross_module_launch_orchestration_landed = false;
  bool public_cross_module_orchestration_abi_landed = false;
  bool ready_for_packaging_and_runtime_registration_impl = false;
  std::string source_serialized_runtime_metadata_replay_key;
  std::string source_local_registration_manifest_replay_key;
  std::string source_imported_runtime_metadata_semantic_rules_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3CrossModuleBuildRuntimeOrchestrationSummary(
    const Objc3CrossModuleBuildRuntimeOrchestrationSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary
              .source_serialized_runtime_metadata_artifact_reuse_contract_id
              .empty() &&
         !summary.source_local_registration_manifest_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.import_artifact_relative_path.empty() &&
         !summary.local_registration_manifest_artifact_relative_path.empty() &&
         !summary.authority_model.empty() && !summary.input_model.empty() &&
         !summary.registration_scope_model.empty() &&
         !summary.packaging_model.empty() &&
         summary.module_image_count == summary.module_names_lexicographic.size() &&
         summary.module_image_count > 0 &&
         summary.direct_import_input_count + 1 <= summary.module_image_count &&
         summary.local_total_descriptor_count ==
             summary.local_class_descriptor_count +
                 summary.local_protocol_descriptor_count +
                 summary.local_category_descriptor_count +
                 summary.local_property_descriptor_count +
                 summary.local_ivar_descriptor_count &&
         summary.fail_closed &&
         summary.source_serialized_runtime_metadata_artifact_reuse_ready &&
         summary.source_local_registration_manifest_ready &&
         summary.semantic_surface_published &&
         summary.local_registration_manifest_emitted &&
         !summary.cross_module_link_plan_artifact_landed &&
         !summary.imported_registration_manifest_loading_landed &&
         !summary.runtime_archive_aggregation_landed &&
         !summary.cross_module_runtime_registration_landed &&
         !summary.cross_module_launch_orchestration_landed &&
         !summary.public_cross_module_orchestration_abi_landed &&
         !summary.ready_for_packaging_and_runtime_registration_impl &&
         !summary.source_serialized_runtime_metadata_replay_key.empty() &&
         !summary.source_local_registration_manifest_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char *kObjc3ExecutableMetadataSourceGraphContractId =
    "objc3c.executable.metadata.source.graph.completeness.v1";
inline constexpr const char *kObjc3ExecutableMetadataSourceGraphOwnerIdentityModel =
    "semantic-link-symbol-and-runtime-owner-identity";
inline constexpr const char *kObjc3ExecutableMetadataMetaclassNodePolicy =
    "first-class-metaclass-nodes-derived-from-interface-runtime-owner-identities";
inline constexpr const char *kObjc3ExecutableMetadataSourceGraphEdgeOrderingModel =
    "lexicographic-kind-source-target";
inline constexpr const char
    *kObjc3ExecutableMetadataClassMetaclassSourceClosureContractId =
        "objc3c.executable.class.metaclass.source.closure.v1";
inline constexpr const char
    *kObjc3ExecutableMetadataClassMetaclassParentIdentityModel =
        "declaration-owned-class-parent-plus-metaclass-parent-identities";
inline constexpr const char
    *kObjc3ExecutableMetadataClassMetaclassMethodOwnerIdentityModel =
        "declaration-owned-instance-class-method-owner-identities";
inline constexpr const char
    *kObjc3ExecutableMetadataClassMetaclassObjectIdentityModel =
        "declaration-owned-class-and-metaclass-object-identities";
inline constexpr const char
    *kObjc3ExecutableMetadataProtocolCategorySourceClosureContractId =
        "objc3c.executable.protocol.category.source.closure.v1";
inline constexpr const char
    *kObjc3ExecutableMetadataProtocolInheritanceIdentityModel =
        "protocol-declaration-owned-inherited-protocol-identities";
inline constexpr const char
    *kObjc3ExecutableMetadataCategoryAttachmentIdentityModel =
        "category-declaration-owned-class-interface-implementation-attachment-identities";
inline constexpr const char
    *kObjc3ExecutableMetadataProtocolCategoryConformanceIdentityModel =
        "category-declaration-owned-adopted-protocol-conformance-identities";
inline constexpr const char *kObjc3ExecutableMetadataSemanticConsistencyContractId =
    "objc3c.executable.metadata.semantic.consistency.freeze.v1";
inline constexpr const char *kObjc3ExecutableMetadataSemanticValidationContractId =
    "objc3c.executable.metadata.semantic.validation.v1";
inline constexpr const char *kObjc3ExecutableMetadataLoweringHandoffContractId =
    "objc3c.executable.metadata.lowering.handoff.freeze.v1";
inline constexpr const char *kObjc3ExecutableMetadataTypedLoweringHandoffContractId =
    "objc3c.executable.metadata.typed.lowering.handoff.v1";
inline constexpr const char *kObjc3ExecutableMetadataTypedLoweringManifestSchemaOrderingModel =
    "contract-header-then-source-graph-payload-v1";

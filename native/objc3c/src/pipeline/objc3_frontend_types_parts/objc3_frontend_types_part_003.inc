  std::size_t protocol_reference_count = 0;
  std::size_t property_accessor_reference_count = 0;
  std::size_t property_ivar_binding_reference_count = 0;
  std::size_t method_selector_reference_count = 0;
  std::size_t metadata_reference_count = 0;
  bool fail_closed = false;
  bool source_surface_contract_ready = false;
  bool runtime_metadata_source_records_ready = false;
  bool frontend_surface_published = false;
  bool import_artifact_template_published = false;
  bool runtime_aware_import_declarations_landed = false;
  bool module_metadata_import_surface_landed = false;
  bool runtime_owned_declaration_import_landed = false;
  bool runtime_metadata_reference_import_landed = false;
  bool public_frontend_api_module_surface_landed = false;
  bool ready_for_import_artifact_emission = false;
  bool ready_for_frontend_module_consumption = false;
  std::string source_surface_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.source_surface_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.payload_model.empty() &&
         !summary.artifact_relative_path.empty() &&
         !summary.authority_model.empty() &&
         !summary.payload_ownership_model.empty() &&
         !summary.module_name.empty() &&
         summary.runtime_owned_declaration_count ==
             summary.class_record_count + summary.protocol_record_count +
                 summary.category_record_count + summary.property_record_count +
                 summary.method_record_count + summary.ivar_record_count &&
         summary.metadata_reference_count ==
             summary.superclass_reference_count +
                 summary.protocol_reference_count +
                 summary.property_accessor_reference_count +
                 summary.property_ivar_binding_reference_count +
                 summary.method_selector_reference_count &&
         summary.fail_closed && summary.source_surface_contract_ready &&
         summary.runtime_metadata_source_records_ready &&
         summary.frontend_surface_published &&
         summary.import_artifact_template_published &&
         summary.runtime_aware_import_declarations_landed &&
         summary.module_metadata_import_surface_landed &&
         summary.runtime_owned_declaration_import_landed &&
         summary.runtime_metadata_reference_import_landed &&
         summary.public_frontend_api_module_surface_landed &&
         summary.ready_for_import_artifact_emission &&
         summary.ready_for_frontend_module_consumption &&
         !summary.source_surface_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationContractId =
        "objc3c.cross.module.runtime.metadata.semantic.preservation.v1";
inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_cross_module_runtime_metadata_semantic_preservation_contract";
inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationAuthorityModel =
        "semantic-preservation-freeze-derived-from-runtime-import-surface-and-runtime-metadata-source-records";
inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationConformanceShapeModel =
        "superclass-protocol-and-category-attachment-shape";
inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationDispatchTraitModel =
        "selector-classness-accessor-ivar-binding-and-body-availability";
inline constexpr const char
    *kObjc3CrossModuleRuntimeMetadataSemanticPreservationEffectTraitModel =
        "property-attribute-and-ownership-effect-profiles";

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

struct Objc3ExecutableMetadataInterfaceGraphNode {
  std::string class_name;
  std::string owner_identity;
  std::string class_owner_identity;
  std::string metaclass_owner_identity;
  std::string super_class_owner_identity;
  std::string super_metaclass_owner_identity;
  std::string instance_method_owner_identity;
  std::string class_method_owner_identity;
  bool has_super = false;
  bool declaration_complete = false;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  std::size_t class_method_count = 0;
  std::size_t instance_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataImplementationGraphNode {
  std::string class_name;
  std::string owner_identity;
  std::string interface_owner_identity;
  std::string class_owner_identity;
  std::string metaclass_owner_identity;
  std::string super_class_owner_identity;
  std::string super_metaclass_owner_identity;
  std::string instance_method_owner_identity;
  std::string class_method_owner_identity;
  bool has_matching_interface = false;
  bool has_super = false;
  bool declaration_complete = false;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  std::size_t class_method_count = 0;
  std::size_t instance_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataClassGraphNode {
  std::string class_name;
  std::string owner_identity;
  std::string interface_owner_identity;
  std::string implementation_owner_identity;
  std::string metaclass_owner_identity;
  std::string super_class_owner_identity;
  std::string super_metaclass_owner_identity;
  std::vector<std::string> adopted_protocol_owner_identities_lexicographic;
  std::string instance_method_owner_identity;
  std::string class_method_owner_identity;
  bool has_interface = false;
  bool has_implementation = false;
  bool has_super = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  bool realization_identity_complete = false;
  std::size_t interface_property_count = 0;
  std::size_t implementation_property_count = 0;
  std::size_t interface_method_count = 0;
  std::size_t implementation_method_count = 0;
  std::size_t interface_class_method_count = 0;
  std::size_t implementation_class_method_count = 0;
  std::size_t interface_instance_method_count = 0;
  std::size_t implementation_instance_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataMetaclassGraphNode {
  std::string class_name;
  std::string owner_identity;
  std::string class_owner_identity;
  std::string interface_owner_identity;
  std::string implementation_owner_identity;
  std::string super_metaclass_owner_identity;
  bool derived_from_interface = false;
  bool has_implementation = false;
  bool has_super = false;
  std::size_t interface_class_method_count = 0;
  std::size_t implementation_class_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataProtocolGraphNode {
  std::string protocol_name;
  std::string owner_identity;
  std::vector<std::string> inherited_protocol_owner_identities_lexicographic;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  bool is_forward_declaration = false;
  bool declaration_complete = false;
  bool inherited_protocol_identity_complete = false;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataCategoryGraphNode {
  std::string class_name;
  std::string category_name;
  std::string owner_identity;
  std::string interface_owner_identity;
  std::string implementation_owner_identity;
  std::string class_owner_identity;
  std::vector<std::string> adopted_protocol_owner_identities_lexicographic;
  bool has_interface = false;
  bool has_implementation = false;
  bool declaration_complete = false;
  bool attachment_identity_complete = false;
  bool conformance_identity_complete = false;
  std::size_t interface_property_count = 0;
  std::size_t implementation_property_count = 0;
  std::size_t interface_method_count = 0;
  std::size_t implementation_method_count = 0;
  std::size_t interface_class_method_count = 0;
  std::size_t implementation_class_method_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataPropertyGraphNode {
  std::string owner_kind;
  std::string owner_name;
  std::string owner_identity;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  std::string property_name;
  std::string type_name;
  bool has_getter = false;
  std::string getter_selector;
  bool has_setter = false;
  std::string setter_selector;
  std::string ivar_binding_symbol;
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string property_attribute_profile;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  std::string effective_getter_selector;
  bool effective_setter_available = false;
  std::string effective_setter_selector;
  std::string accessor_ownership_profile;
  bool synthesizes_executable_accessors = false;
  std::string getter_storage_runtime_helper_symbol;
  std::string setter_storage_runtime_helper_symbol;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
  std::size_t executable_ivar_layout_offset_bytes = 0;
  std::size_t executable_ivar_layout_padding_bytes = 0;
  std::size_t executable_ivar_layout_inherited_slot_count = 0;
  std::size_t executable_ivar_layout_inherited_size_bytes = 0;
  std::size_t executable_ivar_layout_owner_size_bytes = 0;
  std::size_t executable_ivar_init_order_index = 0;
  std::size_t executable_ivar_destroy_order_index = 0;
  bool executable_ivar_layout_valid = false;
  std::string executable_ivar_layout_replay_key;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataMethodGraphNode {
  std::string owner_kind;
  std::string owner_name;
  std::string owner_identity;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  std::string selector;
  bool is_class_method = false;
  bool has_body = false;
  bool effective_direct_dispatch = false;
  bool objc_final_declared = false;
  std::size_t parameter_count = 0;
  std::string return_type_name;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataIvarGraphNode {
  std::string owner_kind;
  std::string owner_name;
  std::string owner_identity;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  std::string property_owner_identity;
  std::string property_name;
  std::string ivar_binding_symbol;
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
  std::size_t executable_ivar_layout_offset_bytes = 0;
  std::size_t executable_ivar_layout_padding_bytes = 0;
  std::size_t executable_ivar_layout_inherited_slot_count = 0;
  std::size_t executable_ivar_layout_inherited_size_bytes = 0;
  std::size_t executable_ivar_layout_owner_size_bytes = 0;
  std::size_t executable_ivar_init_order_index = 0;
  std::size_t executable_ivar_destroy_order_index = 0;
  bool executable_ivar_layout_valid = false;
  std::string executable_ivar_layout_replay_key;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataGraphEdge {
  std::string edge_kind;
  std::string source_owner_identity;
  std::string target_owner_identity;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3ExecutableMetadataSourceGraph {
  std::string contract_id = kObjc3ExecutableMetadataSourceGraphContractId;
  std::string owner_identity_model =
      kObjc3ExecutableMetadataSourceGraphOwnerIdentityModel;
  std::string metaclass_node_policy =
      kObjc3ExecutableMetadataMetaclassNodePolicy;
  std::string edge_ordering_model =
      kObjc3ExecutableMetadataSourceGraphEdgeOrderingModel;
  std::string class_metaclass_source_closure_contract_id =
      kObjc3ExecutableMetadataClassMetaclassSourceClosureContractId;
  std::string class_metaclass_parent_identity_model =
      kObjc3ExecutableMetadataClassMetaclassParentIdentityModel;
  std::string class_metaclass_method_owner_identity_model =
      kObjc3ExecutableMetadataClassMetaclassMethodOwnerIdentityModel;
  std::string class_metaclass_object_identity_model =
      kObjc3ExecutableMetadataClassMetaclassObjectIdentityModel;
  std::string protocol_category_source_closure_contract_id =
      kObjc3ExecutableMetadataProtocolCategorySourceClosureContractId;
  std::string protocol_inheritance_identity_model =
      kObjc3ExecutableMetadataProtocolInheritanceIdentityModel;
  std::string category_attachment_identity_model =
      kObjc3ExecutableMetadataCategoryAttachmentIdentityModel;
  std::string protocol_category_conformance_identity_model =
      kObjc3ExecutableMetadataProtocolCategoryConformanceIdentityModel;
  std::vector<Objc3ExecutableMetadataInterfaceGraphNode>
      interface_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataImplementationGraphNode>
      implementation_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataClassGraphNode> class_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataMetaclassGraphNode>
      metaclass_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataProtocolGraphNode>
      protocol_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataCategoryGraphNode>
      category_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataPropertyGraphNode>
      property_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataMethodGraphNode>
      method_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataIvarGraphNode> ivar_nodes_lexicographic;
  std::vector<Objc3ExecutableMetadataGraphEdge> owner_edges_lexicographic;
  bool deterministic = false;
  bool class_metaclass_declaration_closure_complete = false;
  bool class_metaclass_parent_identity_closure_complete = false;
  bool class_metaclass_method_owner_identity_closure_complete = false;
  bool class_metaclass_object_identity_closure_complete = false;
  bool protocol_category_declaration_closure_complete = false;
  bool protocol_inheritance_identity_closure_complete = false;
  bool category_attachment_identity_closure_complete = false;
  bool protocol_category_conformance_identity_closure_complete = false;
  bool source_graph_complete = false;
  bool ready_for_semantic_closure = false;
  bool ready_for_lowering = false;
};

inline bool IsReadyObjc3ExecutableMetadataSourceGraph(
    const Objc3ExecutableMetadataSourceGraph &graph) {
  return graph.deterministic && graph.source_graph_complete &&
         graph.ready_for_semantic_closure && !graph.ready_for_lowering &&
         !graph.contract_id.empty() && !graph.owner_identity_model.empty() &&
         !graph.metaclass_node_policy.empty() &&
         !graph.edge_ordering_model.empty() &&
         !graph.class_metaclass_source_closure_contract_id.empty() &&
         !graph.class_metaclass_parent_identity_model.empty() &&
         !graph.class_metaclass_method_owner_identity_model.empty() &&
         !graph.class_metaclass_object_identity_model.empty() &&
         !graph.protocol_category_source_closure_contract_id.empty() &&
         !graph.protocol_inheritance_identity_model.empty() &&
         !graph.category_attachment_identity_model.empty() &&
         !graph.protocol_category_conformance_identity_model.empty() &&
         graph.class_metaclass_declaration_closure_complete &&
         graph.class_metaclass_parent_identity_closure_complete &&
         graph.class_metaclass_method_owner_identity_closure_complete &&
         graph.class_metaclass_object_identity_closure_complete &&
         graph.protocol_category_declaration_closure_complete &&
         graph.protocol_inheritance_identity_closure_complete &&
         graph.category_attachment_identity_closure_complete &&
         graph.protocol_category_conformance_identity_closure_complete;
}

struct Objc3ExecutableMetadataSemanticConsistencyBoundary {
  std::string contract_id =
      kObjc3ExecutableMetadataSemanticConsistencyContractId;
  std::string executable_metadata_source_graph_contract_id;
  bool semantic_boundary_frozen = false;
  bool lowering_admission_ready = false;
  bool fail_closed = false;
  bool source_graph_ready = false;
  bool protocol_category_handoff_deterministic = false;
  bool class_protocol_category_linking_deterministic = false;
  bool selector_normalization_deterministic = false;
  bool property_attribute_deterministic = false;
  bool symbol_graph_scope_resolution_deterministic = false;
  bool protocol_inheritance_edges_complete = false;
  bool category_attachment_edges_complete = false;
  bool declaration_export_owner_split_complete = false;
  bool property_method_ivar_owner_edges_complete = false;
  bool semantic_conflict_diagnostics_enforcement_pending = true;
  bool duplicate_export_owner_enforcement_pending = true;
  bool lowering_admission_pending = true;
  std::size_t protocol_node_count = 0;
  std::size_t category_node_count = 0;
  std::size_t property_node_count = 0;
  std::size_t method_node_count = 0;
  std::size_t ivar_node_count = 0;
  std::size_t owner_edge_count = 0;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataSemanticConsistencyBoundary(
    const Objc3ExecutableMetadataSemanticConsistencyBoundary &boundary) {
  return !boundary.contract_id.empty() &&
         !boundary.executable_metadata_source_graph_contract_id.empty() &&
         boundary.semantic_boundary_frozen &&
         !boundary.lowering_admission_ready &&
         boundary.fail_closed &&
         boundary.source_graph_ready &&
         boundary.protocol_category_handoff_deterministic &&
         boundary.class_protocol_category_linking_deterministic &&
         boundary.selector_normalization_deterministic &&
         boundary.property_attribute_deterministic &&
         boundary.symbol_graph_scope_resolution_deterministic &&
         boundary.protocol_inheritance_edges_complete &&
         boundary.category_attachment_edges_complete &&
         boundary.declaration_export_owner_split_complete &&
         boundary.property_method_ivar_owner_edges_complete &&
         boundary.semantic_conflict_diagnostics_enforcement_pending &&
         boundary.duplicate_export_owner_enforcement_pending &&
         boundary.lowering_admission_pending &&
         boundary.failure_reason.empty();
}

struct Objc3ExecutableMetadataSemanticValidationSurface {
  std::string contract_id =
      kObjc3ExecutableMetadataSemanticValidationContractId;
  std::string executable_metadata_semantic_consistency_contract_id;
  bool semantic_consistency_ready = false;
  bool method_lookup_override_conflict_handoff_deterministic = false;
  bool class_protocol_category_linking_deterministic = false;
  bool class_inheritance_edges_complete = false;
  bool protocol_inheritance_edges_complete = false;
  bool metaclass_edges_complete = false;
  bool inheritance_chain_cycle_free = false;
  bool superclass_targets_resolved = false;
  bool protocol_inheritance_targets_resolved = false;
  bool metaclass_targets_resolved = false;
  bool metaclass_lineage_aligned = false;
  bool method_override_edges_complete = false;
  bool override_lookup_complete = false;
  bool override_conflicts_absent = false;
  bool protocol_composition_valid = false;
  bool inheritance_validation_ready = false;
  bool override_validation_ready = false;
  bool protocol_composition_validation_ready = false;
  bool metaclass_relationship_validation_ready = false;
  bool semantic_validation_complete = false;
  bool lowering_admission_ready = false;
  bool fail_closed = false;
  std::size_t class_inheritance_edge_count = 0;
  std::size_t protocol_inheritance_edge_count = 0;
  std::size_t metaclass_super_edge_count = 0;
  std::size_t override_edge_count = 0;
  std::size_t class_method_override_edge_count = 0;
  std::size_t instance_method_override_edge_count = 0;
  std::size_t override_lookup_sites = 0;
  std::size_t override_lookup_hits = 0;
  std::size_t override_lookup_misses = 0;
  std::size_t override_conflicts = 0;
  std::size_t unresolved_base_interfaces = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t protocol_composition_symbols = 0;
  std::size_t category_composition_sites = 0;
  std::size_t category_composition_symbols = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataSemanticValidationSurface(
    const Objc3ExecutableMetadataSemanticValidationSurface &surface) {
  return !surface.contract_id.empty() &&
         !surface.executable_metadata_semantic_consistency_contract_id.empty() &&
         surface.semantic_consistency_ready &&
         surface.method_lookup_override_conflict_handoff_deterministic &&
         surface.class_protocol_category_linking_deterministic &&
         surface.class_inheritance_edges_complete &&
         surface.protocol_inheritance_edges_complete &&
         surface.metaclass_edges_complete &&
         surface.inheritance_chain_cycle_free &&
         surface.superclass_targets_resolved &&
         surface.protocol_inheritance_targets_resolved &&
         surface.metaclass_targets_resolved &&
         surface.metaclass_lineage_aligned &&
         surface.method_override_edges_complete &&
         surface.override_lookup_complete &&
         surface.override_conflicts_absent &&
         surface.protocol_composition_valid &&
         surface.inheritance_validation_ready &&
         surface.override_validation_ready &&
         surface.protocol_composition_validation_ready &&
         surface.metaclass_relationship_validation_ready &&
         surface.semantic_validation_complete &&
         !surface.lowering_admission_ready &&
         surface.fail_closed &&
         surface.failure_reason.empty();
}

struct Objc3ExecutableMetadataLoweringHandoffSurface {
  std::string contract_id = kObjc3ExecutableMetadataLoweringHandoffContractId;
  std::string executable_metadata_source_graph_contract_id;
  std::string executable_metadata_semantic_consistency_contract_id;
  std::string executable_metadata_semantic_validation_contract_id;
  bool source_graph_ready = false;
  bool semantic_consistency_ready = false;
  bool semantic_validation_ready = false;
  bool semantic_type_metadata_handoff_deterministic = false;
  bool protocol_category_handoff_deterministic = false;
  bool class_protocol_category_linking_handoff_deterministic = false;
  bool selector_normalization_handoff_deterministic = false;
  bool property_attribute_handoff_deterministic = false;
  bool symbol_graph_scope_resolution_handoff_deterministic = false;
  bool property_synthesis_ivar_binding_handoff_deterministic = false;
  bool lowering_schema_frozen = false;
  bool fail_closed = false;
  bool ready_for_lowering = false;
  std::size_t interface_node_count = 0;
  std::size_t implementation_node_count = 0;
  std::size_t class_node_count = 0;
  std::size_t metaclass_node_count = 0;
  std::size_t protocol_node_count = 0;
  std::size_t category_node_count = 0;
  std::size_t property_node_count = 0;
  std::size_t method_node_count = 0;
  std::size_t ivar_node_count = 0;
  std::size_t owner_edge_count = 0;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataLoweringHandoffSurface(
    const Objc3ExecutableMetadataLoweringHandoffSurface &surface) {
  return !surface.contract_id.empty() &&
         !surface.executable_metadata_source_graph_contract_id.empty() &&
         !surface.executable_metadata_semantic_consistency_contract_id.empty() &&
         !surface.executable_metadata_semantic_validation_contract_id.empty() &&
         surface.source_graph_ready &&
         surface.semantic_consistency_ready &&
         surface.semantic_validation_ready &&
         surface.semantic_type_metadata_handoff_deterministic &&
         surface.protocol_category_handoff_deterministic &&
         surface.class_protocol_category_linking_handoff_deterministic &&
         surface.selector_normalization_handoff_deterministic &&
         surface.property_attribute_handoff_deterministic &&
         surface.symbol_graph_scope_resolution_handoff_deterministic &&
         surface.property_synthesis_ivar_binding_handoff_deterministic &&
         surface.lowering_schema_frozen &&
         surface.fail_closed &&
         !surface.ready_for_lowering &&
         !surface.replay_key.empty() &&
         surface.failure_reason.empty();
}

struct Objc3ExecutableMetadataTypedLoweringHandoff {
  std::string contract_id = kObjc3ExecutableMetadataTypedLoweringHandoffContractId;
  std::string executable_metadata_lowering_handoff_contract_id;
  std::string executable_metadata_source_graph_contract_id;
  std::string executable_metadata_semantic_consistency_contract_id;
  std::string executable_metadata_semantic_validation_contract_id;
  std::string manifest_schema_ordering_model =
      kObjc3ExecutableMetadataTypedLoweringManifestSchemaOrderingModel;
  bool source_graph_ready = false;
  bool semantic_consistency_ready = false;
  bool semantic_validation_ready = false;
  bool lowering_handoff_surface_ready = false;
  bool deterministic = false;
  bool manifest_schema_frozen = false;
  bool fail_closed = false;
  bool ready_for_lowering = false;
  Objc3ExecutableMetadataSourceGraph source_graph;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
    const Objc3ExecutableMetadataTypedLoweringHandoff &surface) {
  return !surface.contract_id.empty() &&
         !surface.executable_metadata_lowering_handoff_contract_id.empty() &&
         !surface.executable_metadata_source_graph_contract_id.empty() &&
         !surface.executable_metadata_semantic_consistency_contract_id.empty() &&
         !surface.executable_metadata_semantic_validation_contract_id.empty() &&
         !surface.manifest_schema_ordering_model.empty() &&
         surface.source_graph_ready &&
         surface.semantic_consistency_ready &&
         surface.semantic_validation_ready &&
         surface.lowering_handoff_surface_ready &&
         surface.deterministic &&
         surface.manifest_schema_frozen &&
         surface.fail_closed &&
         surface.ready_for_lowering &&
         !surface.replay_key.empty() &&
         surface.failure_reason.empty();
}

struct Objc3RuntimeMetadataSourceOwnershipBoundary {
  std::string contract_id = kObjc3RuntimeMetadataSourceOwnershipContractId;
  std::string canonical_source_schema = kObjc3RuntimeMetadataCanonicalSourceSchema;
  std::string class_record_ast_anchor = kObjc3RuntimeMetadataClassAstAnchor;
  std::string protocol_record_ast_anchor = kObjc3RuntimeMetadataProtocolAstAnchor;
  std::string category_record_ast_anchor = kObjc3RuntimeMetadataCategoryAstAnchor;
  std::string property_record_ast_anchor = kObjc3RuntimeMetadataPropertyAstAnchor;
  std::string method_record_ast_anchor = kObjc3RuntimeMetadataMethodAstAnchor;
  std::string ivar_record_ast_anchor = kObjc3RuntimeMetadataIvarAstAnchor;
  std::string ivar_record_source_model = kObjc3RuntimeMetadataIvarSourceModel;
  bool frontend_owns_runtime_metadata_source_records = false;
  bool runtime_metadata_source_records_ready_for_lowering = false;
  bool native_runtime_library_present = false;
  bool runtime_link_test_only = true;
  bool deterministic_source_schema = false;
  bool fail_closed = false;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_interface_record_count = 0;
  std::size_t category_implementation_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::string failure_reason;

  std::size_t category_record_count() const {
    return category_interface_record_count + category_implementation_record_count;
  }
};

inline bool IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
    const Objc3RuntimeMetadataSourceOwnershipBoundary &boundary) {
  return boundary.frontend_owns_runtime_metadata_source_records &&
         !boundary.runtime_metadata_source_records_ready_for_lowering &&
         !boundary.native_runtime_library_present &&
         boundary.runtime_link_test_only &&
         boundary.deterministic_source_schema &&
         boundary.fail_closed &&
         !boundary.contract_id.empty() &&
         !boundary.canonical_source_schema.empty() &&
         !boundary.class_record_ast_anchor.empty() &&
         !boundary.protocol_record_ast_anchor.empty() &&
         !boundary.category_record_ast_anchor.empty() &&
         !boundary.property_record_ast_anchor.empty() &&
         !boundary.method_record_ast_anchor.empty() &&
         !boundary.ivar_record_ast_anchor.empty() &&
         !boundary.ivar_record_source_model.empty() &&
         boundary.ivar_record_count <= boundary.property_record_count &&
         boundary.failure_reason.empty();
}

struct Objc3RuntimeExportLegalityBoundary {
  std::string contract_id = kObjc3RuntimeExportLegalityContractId;
  bool semantic_boundary_frozen = false;
  bool metadata_export_enforcement_ready = false;
  bool fail_closed = false;
  bool semantic_integration_surface_built = false;
  bool sema_type_metadata_handoff_deterministic = false;
  bool typed_sema_surface_ready = false;
  bool typed_sema_surface_deterministic = false;
  bool runtime_metadata_source_boundary_ready = false;
  bool protocol_category_deterministic = false;
  bool class_protocol_category_linking_deterministic = false;
  bool selector_normalization_deterministic = false;
  bool property_attribute_deterministic = false;
  bool object_pointer_surface_deterministic = false;
  bool symbol_graph_scope_resolution_deterministic = false;
  bool property_synthesis_ivar_binding_deterministic = false;
  bool duplicate_runtime_identity_enforcement_pending = true;
  bool incomplete_declaration_export_blocking_pending = true;
  bool illegal_redeclaration_mix_export_blocking_pending = true;
  std::size_t class_record_count = 0;
  std::size_t protocol_record_count = 0;
  std::size_t category_record_count = 0;
  std::size_t property_record_count = 0;
  std::size_t method_record_count = 0;
  std::size_t ivar_record_count = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  std::size_t property_attribute_invalid_entries = 0;
  std::size_t property_attribute_contract_violations = 0;
  std::size_t invalid_type_annotation_sites = 0;
  std::size_t property_ivar_binding_missing = 0;
  std::size_t property_ivar_binding_conflicts = 0;
  std::size_t implementation_resolution_misses = 0;
  std::size_t method_resolution_misses = 0;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeExportLegalityBoundary(
    const Objc3RuntimeExportLegalityBoundary &boundary) {
  return !boundary.contract_id.empty() &&
         boundary.semantic_boundary_frozen &&
         !boundary.metadata_export_enforcement_ready &&
         boundary.fail_closed &&
         boundary.sema_type_metadata_handoff_deterministic &&
         boundary.typed_sema_surface_ready &&
         boundary.typed_sema_surface_deterministic &&
         boundary.runtime_metadata_source_boundary_ready &&
         boundary.protocol_category_deterministic &&
         boundary.class_protocol_category_linking_deterministic &&
         boundary.selector_normalization_deterministic &&
         boundary.property_attribute_deterministic &&
         boundary.object_pointer_surface_deterministic &&
         boundary.symbol_graph_scope_resolution_deterministic &&
         boundary.property_synthesis_ivar_binding_deterministic &&
         boundary.duplicate_runtime_identity_enforcement_pending &&
         boundary.incomplete_declaration_export_blocking_pending &&
         boundary.illegal_redeclaration_mix_export_blocking_pending &&
         boundary.invalid_protocol_composition_sites <=
             boundary.protocol_record_count + boundary.category_record_count &&
         boundary.ivar_record_count <= boundary.property_record_count &&
         boundary.failure_reason.empty();
}

struct Objc3RuntimeExportEnforcementSummary {
  std::string contract_id = kObjc3RuntimeExportEnforcementContractId;
  bool metadata_completeness_enforced = false;
  bool duplicate_runtime_identity_suppression_enforced = false;
  bool illegal_redeclaration_mix_blocking_enforced = false;
  bool metadata_shape_drift_blocking_enforced = false;
  bool fail_closed = false;
  bool ready_for_runtime_export = false;
  std::size_t duplicate_runtime_identity_sites = 0;
  std::size_t incomplete_declaration_sites = 0;
  std::size_t illegal_redeclaration_mix_sites = 0;
  std::size_t metadata_shape_drift_sites = 0;
  unsigned first_failure_line = 1;
  unsigned first_failure_column = 1;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeExportEnforcementSummary(
    const Objc3RuntimeExportEnforcementSummary &summary) {
  return !summary.contract_id.empty() &&
         summary.metadata_completeness_enforced &&
         summary.duplicate_runtime_identity_suppression_enforced &&
         summary.illegal_redeclaration_mix_blocking_enforced &&
         summary.metadata_shape_drift_blocking_enforced &&
         summary.fail_closed &&
         summary.ready_for_runtime_export &&
         summary.duplicate_runtime_identity_sites == 0 &&
         summary.incomplete_declaration_sites == 0 &&
         summary.illegal_redeclaration_mix_sites == 0 &&
         summary.metadata_shape_drift_sites == 0 &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeMetadataSectionAbiFreezeSummary {
  std::string contract_id = kObjc3RuntimeMetadataSectionAbiContractId;
  bool boundary_frozen = false;
  bool fail_closed = false;
  bool object_file_section_inventory_frozen = false;
  bool symbol_policy_frozen = false;
  bool visibility_model_frozen = false;
  bool retention_policy_frozen = false;
  bool runtime_metadata_source_boundary_ready = false;
  bool runtime_export_legality_boundary_ready = false;
  bool runtime_export_enforcement_ready = false;
  bool ready_for_section_scaffold = false;
  std::string logical_image_info_section =
      kObjc3RuntimeMetadataLogicalImageInfoSection;
  std::string logical_class_descriptor_section =
      kObjc3RuntimeMetadataLogicalClassDescriptorSection;
  std::string logical_protocol_descriptor_section =
      kObjc3RuntimeMetadataLogicalProtocolDescriptorSection;
  std::string logical_category_descriptor_section =
      kObjc3RuntimeMetadataLogicalCategoryDescriptorSection;
  std::string logical_property_descriptor_section =
      kObjc3RuntimeMetadataLogicalPropertyDescriptorSection;
  std::string logical_ivar_descriptor_section =
      kObjc3RuntimeMetadataLogicalIvarDescriptorSection;
  std::string descriptor_symbol_prefix =
      kObjc3RuntimeMetadataDescriptorSymbolPrefix;
  std::string aggregate_symbol_prefix =
      kObjc3RuntimeMetadataAggregateSymbolPrefix;
  std::string image_info_symbol = kObjc3RuntimeMetadataImageInfoSymbol;
  std::string descriptor_linkage =
      kObjc3RuntimeMetadataDescriptorLinkagePolicy;
  std::string aggregate_linkage =
      kObjc3RuntimeMetadataAggregateLinkagePolicy;
  std::string metadata_visibility = kObjc3RuntimeMetadataVisibilityPolicy;
  std::string retention_root = kObjc3RuntimeMetadataRetentionPolicyRoot;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary &summary) {
  return !summary.contract_id.empty() &&
         summary.boundary_frozen &&
         summary.fail_closed &&
         summary.object_file_section_inventory_frozen &&
         summary.symbol_policy_frozen &&
         summary.visibility_model_frozen &&
         summary.retention_policy_frozen &&
         summary.runtime_metadata_source_boundary_ready &&
         summary.runtime_export_legality_boundary_ready &&
         summary.runtime_export_enforcement_ready &&
         summary.ready_for_section_scaffold &&
         !summary.logical_image_info_section.empty() &&
         !summary.logical_class_descriptor_section.empty() &&
         !summary.logical_protocol_descriptor_section.empty() &&
         !summary.logical_category_descriptor_section.empty() &&
         !summary.logical_property_descriptor_section.empty() &&
         !summary.logical_ivar_descriptor_section.empty() &&
         !summary.descriptor_symbol_prefix.empty() &&
         !summary.aggregate_symbol_prefix.empty() &&
         !summary.image_info_symbol.empty() &&
         !summary.descriptor_linkage.empty() &&
         !summary.aggregate_linkage.empty() &&
         !summary.metadata_visibility.empty() &&
         !summary.retention_root.empty() &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeMetadataSectionPublicationSummary {
  std::string contract_id = kObjc3RuntimeMetadataSectionPublicationContractId;
  std::string abi_contract_id = kObjc3RuntimeMetadataSectionAbiContractId;
  bool publication_emitted = false;
  bool fail_closed = false;
  bool uses_llvm_used = false;
  bool image_info_emitted = false;
  std::size_t class_descriptor_count = 0;
  std::size_t protocol_descriptor_count = 0;
  std::size_t category_descriptor_count = 0;
  std::size_t property_descriptor_count = 0;
  std::size_t ivar_descriptor_count = 0;
  std::size_t total_descriptor_count = 0;
  std::size_t total_retained_global_count = 0;
  std::string image_info_symbol = kObjc3RuntimeMetadataImageInfoSymbol;
  std::string class_aggregate_symbol =
      kObjc3RuntimeMetadataClassDescriptorAggregateSymbol;
  std::string protocol_aggregate_symbol =
      kObjc3RuntimeMetadataProtocolDescriptorAggregateSymbol;
  std::string category_aggregate_symbol =
      kObjc3RuntimeMetadataCategoryDescriptorAggregateSymbol;
  std::string property_aggregate_symbol =
      kObjc3RuntimeMetadataPropertyDescriptorAggregateSymbol;
  std::string ivar_aggregate_symbol =
      kObjc3RuntimeMetadataIvarDescriptorAggregateSymbol;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeMetadataSectionPublicationSummary(
    const Objc3RuntimeMetadataSectionPublicationSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.abi_contract_id.empty() &&
         summary.publication_emitted &&
         summary.fail_closed &&
         summary.uses_llvm_used &&
         summary.image_info_emitted &&
         summary.total_descriptor_count ==
             summary.class_descriptor_count +
                 summary.protocol_descriptor_count +
                 summary.category_descriptor_count +
                 summary.property_descriptor_count +
                 summary.ivar_descriptor_count &&
         summary.total_retained_global_count ==
             summary.total_descriptor_count + 6u &&
         !summary.image_info_symbol.empty() &&
         !summary.class_aggregate_symbol.empty() &&
         !summary.protocol_aggregate_symbol.empty() &&
         !summary.category_aggregate_symbol.empty() &&
         !summary.property_aggregate_symbol.empty() &&
         !summary.ivar_aggregate_symbol.empty() &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeMetadataObjectInspectionHarnessSummary {
  std::string contract_id = kObjc3RuntimeMetadataObjectInspectionContractId;
  std::string publication_contract_id = kObjc3RuntimeMetadataSectionPublicationContractId;
  bool matrix_published = false;
  bool fail_closed = false;
  bool uses_llvm_readobj = false;
  bool uses_llvm_objdump = false;
  std::size_t matrix_row_count = 0;
  std::string fixture_path = kObjc3RuntimeMetadataObjectInspectionFixturePath;
  std::string emit_prefix = kObjc3RuntimeMetadataObjectInspectionEmitPrefix;
  std::string object_relative_path =
      kObjc3RuntimeMetadataObjectInspectionObjectRelativePath;
  std::string section_inventory_row_key =
      kObjc3RuntimeMetadataObjectInspectionSectionInventoryRowKey;
  std::string section_inventory_command =
      kObjc3RuntimeMetadataObjectInspectionSectionCommand;
  std::string symbol_inventory_row_key =
      kObjc3RuntimeMetadataObjectInspectionSymbolInventoryRowKey;
  std::string symbol_inventory_command =
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand;
  std::string failure_reason;
};


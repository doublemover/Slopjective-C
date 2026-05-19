#pragma once

#include "sema/model/frontend_concurrency_symbol_graph_summaries.h"

struct Objc3ToolingDiagnosticTaxonomyPortabilityContractSummary {
  std::string contract_id = kObjc3ToolingDiagnosticTaxonomyPortabilityContractId;
  std::string dependency_contract_id =
      kObjc3ToolingMigrationCanonicalizationSourceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3ToolingDiagnosticTaxonomyPortabilitySurfacePath;
  std::string semantic_model =
      kObjc3ToolingDiagnosticTaxonomyPortabilitySemanticModel;
  std::string portability_model =
      kObjc3ToolingDiagnosticTaxonomyPortabilityPortabilityModel;
  std::vector<std::string> portability_dependency_surface_ids = {
      "objc3c.releaseclaims.publicationmatrix.surface.v1", "objc3c.releaseclaims.compatibility.closeout.v1", "objc3c.releaseclaims.abiobjectmodel.closeout.v1", "objc3c.errors.closeout.surface.v1", "objc3c.concurrency.continuation.closeout.surface.v1",
      "objc3c.concurrency.taskruntime.closeout.surface.v1", "objc3c.concurrency.actor.closeout.surface.v1", "objc3c.ownership.closeout.surface.v1", "objc3c.dispatch.closeout.surface.v1", "objc3c.metaprogramming.closeout.surface.v1",
      "objc3c.interop.closeout.surface.v1",
  };
  std::string diagnostic_namespace =
      kObjc3ToolingDiagnosticTaxonomyPortabilityDiagnosticNamespace;
  std::size_t portability_dependency_count = 0;
  std::size_t diagnostics_total = 0;
  std::size_t diagnostics_after_pass_final = 0;
  std::size_t diagnostics_emitted_total = 0;
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t ownership_arc_contract_violation_sites = 0;
  std::size_t migration_canonicalization_candidate_sites = 0;
  bool migration_surface_ready = false;
  bool portability_dependencies_frozen = false;
  bool deterministic_handoff = false;
  bool ready_for_lowering_and_runtime = false;
  std::string recovery_replay_key;
  std::string arc_diagnostics_fixit_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3ToolingFeatureSpecificFixitSynthesisSummary {
  std::string contract_id = kObjc3ToolingFeatureSpecificFixitSynthesisContractId;
  std::string dependency_contract_id =
      kObjc3ToolingDiagnosticTaxonomyPortabilityContractId;
  std::string frontend_surface_path =
      kObjc3ToolingFeatureSpecificFixitSynthesisSurfacePath;
  std::string semantic_model =
      kObjc3ToolingFeatureSpecificFixitSynthesisSemanticModel;
  std::vector<std::string> fixit_family_ids = {
      "migration-canonicalization",
      "ownership-arc",
  };
  std::size_t fixit_family_count = 0;
  std::size_t migration_fixit_candidate_sites = 0;
  std::size_t migrator_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  bool diagnostic_taxonomy_ready = false;
  bool deterministic_handoff = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3ToolingLegacyCanonicalMigrationSemanticsSummary {
  std::string contract_id =
      kObjc3ToolingLegacyCanonicalMigrationSemanticsContractId;
  std::string dependency_contract_id =
      kObjc3ToolingFeatureSpecificFixitSynthesisContractId;
  std::string compatibility_semantics_contract_id =
      kObjc3CompatibilityStrictnessClaimSemanticsContractId;
  std::string frontend_surface_path =
      kObjc3ToolingLegacyCanonicalMigrationSemanticsSurfacePath;
  std::string semantic_model =
      kObjc3ToolingLegacyCanonicalMigrationSemanticsSemanticModel;
  std::string language_profile_model =
      kObjc3ToolingLegacyCanonicalMigrationSemanticsLanguageProfileModel;
  std::string effective_language_profile = "canonical";
  bool canonical_literal_rejection_diagnostics_enabled = false;
  std::size_t current_run_legacy_literal_sites = 0;
  std::size_t current_run_canonicalization_candidate_sites = 0;
  std::size_t fixit_family_count = 0;
  bool fail_closed = false;
  bool selected_configuration_valid = false;
  bool language_profile_semantics_landed = false;
  bool canonical_literal_rejection_semantics_landed = false;
  std::string canonical_mode_rejection_code =
      kObjc3ToolingLegacyCanonicalMigrationDiagnosticCode;
  bool canonical_mode_rejection_ready = false;
  bool feature_specific_fixit_surface_ready = false;
  bool deterministic_handoff = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3InteropForeignSurfaceInterfacePreservationSummary {
  std::string contract_id =
      kObjc3InteropForeignSurfaceInterfacePreservationContractId;
  std::string foreign_import_source_contract_id =
      kObjc3InteropForeignImportSourceClosureContractId;
  std::string cpp_swift_source_contract_id =
      kObjc3InteropCppSwiftInteropAnnotationSourceCompletionContractId;
  std::string surface_path =
      kObjc3InteropForeignSurfaceInterfacePreservationSurfacePath;
  std::string import_artifact_member_name =
      kObjc3InteropForeignSurfaceInterfacePreservationImportArtifactMemberName;
  std::string source_model =
      kObjc3InteropForeignSurfaceInterfacePreservationSourceModel;
  std::string preservation_model =
      kObjc3InteropForeignSurfaceInterfacePreservationModel;
  std::string fail_closed_model =
      kObjc3InteropForeignSurfaceInterfacePreservationFailClosedModel;
  std::string foreign_import_source_replay_key;
  std::string cpp_swift_source_replay_key;
  std::vector<std::string> local_import_module_names_lexicographic;
  std::vector<std::string> imported_provider_module_names_lexicographic;
  std::size_t local_foreign_callable_count = 0;
  std::size_t local_import_module_annotation_count = 0;
  std::size_t local_imported_module_name_count = 0;
  std::size_t local_swift_name_annotation_count = 0;
  std::size_t local_swift_private_annotation_count = 0;
  std::size_t local_cpp_name_annotation_count = 0;
  std::size_t local_header_name_annotation_count = 0;
  std::size_t local_named_annotation_payload_count = 0;
  std::size_t imported_module_count = 0;
  std::size_t imported_foreign_callable_count = 0;
  std::size_t imported_import_module_annotation_count = 0;
  std::size_t imported_imported_module_name_count = 0;
  std::size_t imported_swift_name_annotation_count = 0;
  std::size_t imported_swift_private_annotation_count = 0;
  std::size_t imported_cpp_name_annotation_count = 0;
  std::size_t imported_header_name_annotation_count = 0;
  std::size_t imported_named_annotation_payload_count = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_preservation_ready = false;
  bool deterministic = false;
  std::string replay_key;
};

struct Objc3InteropHeaderModuleBridgeGenerationSummary {
  std::string contract_id = kObjc3InteropHeaderModuleBridgeGenerationContractId;
  std::string source_contract_id =
      kObjc3InteropHeaderModuleBridgeGenerationSourceContractId;
  std::string preservation_contract_id =
      kObjc3InteropHeaderModuleBridgeGenerationPreservationContractId;
  std::string surface_path =
      kObjc3InteropHeaderModuleBridgeGenerationSurfacePath;
  std::string import_artifact_member_name =
      kObjc3InteropHeaderModuleBridgeGenerationImportArtifactMemberName;
  std::string generation_model = kObjc3InteropHeaderModuleBridgeGenerationModel;
  std::string packaging_model =
      kObjc3InteropHeaderModuleBridgeGenerationPackagingModel;
  std::string fail_closed_model =
      kObjc3InteropHeaderModuleBridgeGenerationFailClosedModel;
  std::string header_artifact_relative_path =
      kObjc3InteropBridgeHeaderArtifactRelativePath;
  std::string module_artifact_relative_path =
      kObjc3InteropBridgeModuleArtifactRelativePath;
  std::string bridge_artifact_relative_path =
      kObjc3InteropBridgeArtifactRelativePath;
  std::vector<std::string> local_import_module_names_lexicographic;
  std::vector<std::string> imported_provider_module_names_lexicographic;
  std::size_t local_foreign_callable_count = 0;
  std::size_t local_import_module_name_count = 0;
  std::size_t local_cpp_name_annotation_count = 0;
  std::size_t local_header_name_annotation_count = 0;
  std::size_t local_swift_name_annotation_count = 0;
  std::size_t imported_module_count = 0;
  bool runtime_generation_ready = false;
  bool cross_module_packaging_ready = false;
  bool deterministic = false;
  std::string preservation_replay_key;
  std::string replay_key;
};

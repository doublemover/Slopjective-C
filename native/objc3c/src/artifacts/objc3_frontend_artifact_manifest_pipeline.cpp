#include "artifacts/objc3_frontend_artifact_manifest_pipeline.h"

#include <ostream>

#include "artifacts/json/program_manifest_json.h"
#include "artifacts/json/runtime_metadata_manifest_json.h"
#include "artifacts/json/semantic_type_manifest_json.h"
#include "artifacts/objc3_frontend_artifacts.h"
#include "artifacts/objc3_frontend_parser_diagnostic_artifacts.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactManifestPipelineStages(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactBundle &bundle) {
  manifest << "    \"max_message_send_args\":"
           << options.lowering.max_message_send_args << ",\n";
  manifest << "    \"pipeline\": {\n";
  manifest << "      \"semantic_skipped\": "
           << (pipeline_result.integration_surface.built ? "false" : "true")
           << ",\n";
  manifest << "      \"stages\": {\n";
  const Objc3ParserDiagnosticCodeCoverage parser_diag_code_coverage =
      BuildObjc3ParserDiagnosticCodeCoverage(bundle.stage_diagnostics.parser);
  manifest << "        \"lexer\": {\"diagnostics\":"
           << bundle.stage_diagnostics.lexer.size() << "},\n";
  manifest << "        \"parser\": {\"diagnostics\":"
           << bundle.stage_diagnostics.parser.size()
           << ",\"token_count\":"
           << pipeline_result.parser_contract_snapshot.token_count
           << ",\"top_level_declarations\":"
           << pipeline_result.parser_contract_snapshot.top_level_declaration_count
           << ",\"globals\":"
           << pipeline_result.parser_contract_snapshot.global_decl_count
           << ",\"protocols\":"
           << pipeline_result.parser_contract_snapshot.protocol_decl_count
           << ",\"protocol_properties\":"
           << pipeline_result.parser_contract_snapshot.protocol_property_decl_count
           << ",\"protocol_methods\":"
           << pipeline_result.parser_contract_snapshot.protocol_method_decl_count
           << ",\"interfaces\":"
           << pipeline_result.parser_contract_snapshot.interface_decl_count
           << ",\"interface_properties\":"
           << pipeline_result.parser_contract_snapshot.interface_property_decl_count
           << ",\"interface_methods\":"
           << pipeline_result.parser_contract_snapshot.interface_method_decl_count
           << ",\"interface_categories\":"
           << pipeline_result.parser_contract_snapshot.interface_category_decl_count
           << ",\"implementations\":"
           << pipeline_result.parser_contract_snapshot.implementation_decl_count
           << ",\"implementation_properties\":"
           << pipeline_result.parser_contract_snapshot
                  .implementation_property_decl_count
           << ",\"implementation_methods\":"
           << pipeline_result.parser_contract_snapshot
                  .implementation_method_decl_count
           << ",\"implementation_categories\":"
           << pipeline_result.parser_contract_snapshot
                  .implementation_category_decl_count
           << ",\"functions\":"
           << pipeline_result.parser_contract_snapshot.function_decl_count
           << ",\"function_prototypes\":"
           << pipeline_result.parser_contract_snapshot.function_prototype_count
           << ",\"function_pure\":"
           << pipeline_result.parser_contract_snapshot.function_pure_count
           << ",\"draft_syntax_surface_count\":"
           << pipeline_result.parser_contract_snapshot.draft_syntax_surface_count
           << ",\"draft_syntax_surface_fingerprint\":"
           << pipeline_result.parser_contract_snapshot
                  .draft_syntax_surface_fingerprint
           << ",\"draft_syntax_surface_handoff_key\":\""
           << pipeline_result.parser_contract_snapshot
                  .draft_syntax_surface_handoff_key
           << "\",\"draft_syntax_surface_deterministic\":"
           << (pipeline_result.parser_contract_snapshot
                       .draft_syntax_surface_handoff_deterministic
                   ? "true"
                   : "false")
           << ",\"long_tail_grammar_constructs\":"
           << pipeline_result.parser_contract_snapshot
                  .long_tail_grammar_construct_count
           << ",\"long_tail_grammar_covered_constructs\":"
           << pipeline_result.parser_contract_snapshot
                  .long_tail_grammar_covered_construct_count
           << ",\"long_tail_grammar_fingerprint\":"
           << pipeline_result.parser_contract_snapshot.long_tail_grammar_fingerprint
           << ",\"long_tail_grammar_handoff_key\":\""
           << pipeline_result.parser_contract_snapshot.long_tail_grammar_handoff_key
           << "\",\"long_tail_grammar_deterministic\":"
           << (pipeline_result.parser_contract_snapshot
                       .long_tail_grammar_handoff_deterministic
                   ? "true"
                   : "false")
           << ",\"diagnostic_code_count\":"
           << parser_diag_code_coverage.unique_code_count
           << ",\"diagnostic_code_fingerprint\":"
           << parser_diag_code_coverage.unique_code_fingerprint
           << ",\"diagnostic_code_surface_deterministic\":"
           << (parser_diag_code_coverage.deterministic_surface ? "true"
                                                               : "false")
           << ",\"deterministic_handoff\":"
           << (pipeline_result.parser_contract_snapshot.deterministic_handoff
                   ? "true"
                   : "false")
           << ",\"recovery_replay_ready\":"
           << (pipeline_result.parser_contract_snapshot.parser_recovery_replay_ready
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "        \"semantic\": {\"diagnostics\":"
           << bundle.stage_diagnostics.semantic.size() << "}\n";
  manifest << "      },\n";
}

void AppendObjc3FrontendArtifactManifestRecordArrays(
    std::ostream &manifest,
    const Objc3Program &program,
    const std::vector<int> &resolved_global_values,
    const std::vector<const FunctionDecl *> &manifest_functions,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff,
    const Objc3RuntimeMetadataSourceRecordSet
        &runtime_metadata_source_records) {
  manifest << "  \"semantic_canonical_type_metadata\":";
  objc3::artifacts::json::WriteSemanticTypeMetadataHandoffManifestObject(
      manifest, type_metadata_handoff);
  manifest << ",\n";
  manifest << "  \"globals\": ";
  objc3::artifacts::json::WriteProgramGlobalsManifestArray(
      manifest, program.globals, resolved_global_values);
  manifest << ",\n";
  manifest << "  \"functions\": ";
  objc3::artifacts::json::WriteFunctionDeclarationsManifestArray(
      manifest, manifest_functions);
  manifest << ",\n";
  manifest << "  \"interfaces\": ";
  objc3::artifacts::json::WriteRuntimeMetadataInterfaceManifestArray(
      manifest, runtime_metadata_source_records);
  manifest << ",\n";
  manifest << "  \"implementations\": ";
  objc3::artifacts::json::WriteRuntimeMetadataImplementationManifestArray(
      manifest, runtime_metadata_source_records);
  manifest << ",\n";
  manifest << "  \"protocols\": ";
  objc3::artifacts::json::WriteRuntimeMetadataProtocolManifestArray(
      manifest, runtime_metadata_source_records);
  manifest << ",\n";
  manifest << "  \"categories\": ";
  objc3::artifacts::json::WriteRuntimeMetadataCategoryManifestArray(
      manifest, runtime_metadata_source_records);
  manifest << ",\n";
  manifest << "  \"runtime_metadata_source_records\": ";
  objc3::artifacts::json::WriteRuntimeMetadataSourceRecordSetManifestObject(
      manifest, runtime_metadata_source_records);
  manifest << "\n";
}

void PopulateObjc3FrontendArtifactBundleSummaryOutputs(
    Objc3FrontendArtifactBundle &bundle,
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary
        &runtime_aware_import_module_frontend_closure,
    const Objc3VersionedConformanceReportLoweringSummary
        &versioned_conformance_report_lowering,
    const Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary
        &runtime_registration_descriptor_image_root_source_surface,
    const Objc3RuntimeRegistrationDescriptorFrontendClosureSummary
        &runtime_registration_descriptor_frontend_closure,
    const Objc3RuntimeBlockOwnershipArtifactPreservationSummary
        &runtime_block_ownership_artifact_preservation_summary,
    const Objc3RuntimeStorageReflectionArtifactPreservationSummary
        &runtime_storage_reflection_artifact_preservation_summary,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    const Objc3RuntimeBootstrapLegalitySemanticsSummary
        &runtime_bootstrap_legality_semantics,
    const Objc3RuntimeBootstrapLegalityFailureContractSummary
        &runtime_bootstrap_legality_failure_contract,
    const Objc3RuntimeBootstrapFailureRestartSemanticsSummary
        &runtime_bootstrap_failure_restart_semantics,
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary
        &frontend_compatibility_strictness_claim_semantics,
    const Objc3ToolingLegacyCanonicalMigrationSemanticsSummary
        &tooling_legacy_canonical_migration_semantics_summary,
    const Objc3ToolingMachineReadableConformanceReportContractSummary
        &tooling_machine_readable_conformance_report_contract_summary,
    const Objc3ToolingFeatureAwareConformanceReportEmissionSummary
        &tooling_feature_aware_conformance_report_emission_summary,
    const Objc3ToolingCorpusShardingReleaseEvidencePackagingSummary
        &tooling_corpus_sharding_release_evidence_packaging_summary,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3RuntimeBootstrapSemanticsSummary &runtime_bootstrap_semantics,
    const Objc3RuntimeBootstrapLoweringSummary &runtime_bootstrap_lowering) {
  bundle.runtime_aware_import_module_frontend_closure_summary =
      runtime_aware_import_module_frontend_closure;
  bundle.versioned_conformance_report_lowering_summary =
      versioned_conformance_report_lowering;
  bundle.runtime_registration_descriptor_image_root_source_surface_summary =
      runtime_registration_descriptor_image_root_source_surface;
  bundle.runtime_registration_descriptor_frontend_closure_summary =
      runtime_registration_descriptor_frontend_closure;
  bundle.runtime_block_ownership_artifact_preservation_summary =
      runtime_block_ownership_artifact_preservation_summary;
  bundle.runtime_storage_reflection_artifact_preservation_summary =
      runtime_storage_reflection_artifact_preservation_summary;
  bundle.runtime_translation_unit_registration_manifest_summary =
      runtime_translation_unit_registration_manifest;
  bundle.runtime_bootstrap_legality_semantics_summary =
      runtime_bootstrap_legality_semantics;
  bundle.runtime_bootstrap_legality_failure_contract_summary =
      runtime_bootstrap_legality_failure_contract;
  bundle.runtime_bootstrap_failure_restart_semantics_summary =
      runtime_bootstrap_failure_restart_semantics;
  bundle.frontend_compatibility_strictness_claim_semantics_summary =
      frontend_compatibility_strictness_claim_semantics;
  bundle.tooling_legacy_canonical_migration_semantics_summary =
      tooling_legacy_canonical_migration_semantics_summary;
  bundle.tooling_machine_readable_conformance_report_contract_summary =
      tooling_machine_readable_conformance_report_contract_summary;
  bundle.tooling_feature_aware_conformance_report_emission_summary =
      tooling_feature_aware_conformance_report_emission_summary;
  bundle.tooling_corpus_sharding_release_evidence_packaging_summary =
      tooling_corpus_sharding_release_evidence_packaging_summary;
  bundle.runtime_bootstrap_api_summary = runtime_bootstrap_api;
  bundle.runtime_bootstrap_semantics_summary = runtime_bootstrap_semantics;
  bundle.runtime_bootstrap_lowering_summary = runtime_bootstrap_lowering;
}

}  // namespace objc3::artifacts::frontend

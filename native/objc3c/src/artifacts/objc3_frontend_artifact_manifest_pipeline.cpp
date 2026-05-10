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

void AppendObjc3FrontendArtifactManifestSemaPassDiagnostics(
    std::ostream &manifest,
    const Objc3FrontendPipelineResult &pipeline_result) {
  manifest << "      \"sema_pass_manager\": {\"diagnostics_after_build\":"
           << pipeline_result.sema_diagnostics_after_pass[0]
           << ",\"diagnostics_after_validate_bodies\":"
           << pipeline_result.sema_diagnostics_after_pass[1]
           << ",\"diagnostics_after_validate_pure_contract\":"
           << pipeline_result.sema_diagnostics_after_pass[2]
           << ",\"diagnostics_emitted_by_build\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[0]
           << ",\"diagnostics_emitted_by_validate_bodies\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[1]
           << ",\"diagnostics_emitted_by_validate_pure_contract\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[2]
           << ",\"diagnostics_monotonic\":"
           << (pipeline_result.sema_parity_surface
                       .diagnostics_after_pass_monotonic
                   ? "true"
                   : "false")
           << ",\"diagnostics_total\":"
           << pipeline_result.sema_parity_surface.diagnostics_total
           << ",\"deterministic_semantic_diagnostics\":"
           << (pipeline_result.sema_parity_surface
                       .deterministic_semantic_diagnostics
                   ? "true"
                   : "false")
           << ",\"diagnostics_accounting_consistent\":"
           << (pipeline_result.sema_parity_surface
                       .diagnostics_accounting_consistent
                   ? "true"
                   : "false")
           << ",\"diagnostics_bus_publish_consistent\":"
           << (pipeline_result.sema_parity_surface
                       .diagnostics_bus_publish_consistent
                   ? "true"
                   : "false")
           << ",\"diagnostics_canonicalized\":"
           << (pipeline_result.sema_parity_surface.diagnostics_canonicalized
                   ? "true"
                   : "false")
           << ",\"diagnostics_hardening_satisfied\":"
           << (pipeline_result.sema_parity_surface
                       .diagnostics_hardening_satisfied
                   ? "true"
                   : "false")
           << ",\"pass_flow_recovery_replay_contract_satisfied\":"
           << (pipeline_result.sema_parity_surface
                       .pass_flow_recovery_replay_contract_satisfied
                   ? "true"
                   : "false")
           << ",\"pass_flow_recovery_replay_key\":\""
           << pipeline_result.sema_parity_surface.pass_flow_recovery_replay_key
           << "\",\"pass_flow_recovery_replay_key_deterministic\":"
           << (pipeline_result.sema_parity_surface
                       .pass_flow_recovery_replay_key_deterministic
                   ? "true"
                   : "false")
           << ",\"pass_flow_recovery_determinism_hardening_satisfied\":"
           << (pipeline_result.sema_parity_surface
                       .pass_flow_recovery_determinism_hardening_satisfied
                   ? "true"
                   : "false")
           << ",\"deterministic_type_metadata_handoff\":"
           << (pipeline_result.sema_parity_surface
                       .deterministic_type_metadata_handoff
                   ? "true"
                   : "false")
           << ",\"pass_flow_configured_count\":"
           << pipeline_result.sema_pass_flow_summary.configured_pass_count
           << ",\"pass_flow_executed_count\":"
           << pipeline_result.sema_pass_flow_summary.executed_pass_count
           << ",\"pass_flow_language_profile\":\""
           << (pipeline_result.sema_pass_flow_summary.language_profile ==
                       Objc3SemaLanguageProfile::Canonical
                   ? "canonical"
                   : "canonical")
           << "\",\"pass_flow_canonical_literal_rejection_total\":"
           << pipeline_result.canonical_literal_rejection_counts
                  .total_literal_sites()
           << ",\"pass_flow_duplicate_execution_count\":"
           << pipeline_result.sema_pass_flow_summary
                  .duplicate_pass_execution_count
           << ",\"pass_flow_missing_execution_count\":"
           << pipeline_result.sema_pass_flow_summary.missing_pass_execution_count
           << ",\"pass_flow_diagnostics_total\":"
           << pipeline_result.sema_pass_flow_summary.diagnostics_total
           << ",\"pass_flow_diagnostics_emitted_by_build\":"
           << pipeline_result.sema_pass_flow_summary.diagnostics_emitted_by_pass[0]
           << ",\"pass_flow_diagnostics_emitted_by_validate_bodies\":"
           << pipeline_result.sema_pass_flow_summary.diagnostics_emitted_by_pass[1]
           << ",\"pass_flow_diagnostics_emitted_by_validate_pure_contract\":"
           << pipeline_result.sema_pass_flow_summary.diagnostics_emitted_by_pass[2]
           << ",\"pass_flow_transition_edge_count\":"
           << pipeline_result.sema_pass_flow_summary.transition_edge_count
           << ",\"pass_flow_order_matches_contract\":"
           << (pipeline_result.sema_pass_flow_summary
                       .pass_order_matches_contract
                   ? "true"
                   : "false")
           << ",\"pass_flow_diagnostics_emission_totals_consistent\":"
           << (pipeline_result.sema_pass_flow_summary
                       .diagnostics_emission_totals_consistent
                   ? "true"
                   : "false")
           << ",\"pass_flow_diagnostics_accounting_consistent\":"
           << (pipeline_result.sema_pass_flow_summary
                       .diagnostics_accounting_consistent
                   ? "true"
                   : "false")
           << ",\"pass_flow_diagnostics_bus_publish_consistent\":"
           << (pipeline_result.sema_pass_flow_summary
                       .diagnostics_bus_publish_consistent
                   ? "true"
                   : "false")
           << ",\"pass_flow_diagnostics_canonicalized\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_canonicalized
                   ? "true"
                   : "false")
           << ",\"pass_flow_diagnostics_hardening_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary
                       .diagnostics_hardening_satisfied
                   ? "true"
                   : "false")
           << ",\"pass_flow_parser_recovery_replay_ready\":"
           << (pipeline_result.sema_pass_flow_summary
                       .parser_recovery_replay_ready
                   ? "true"
                   : "false")
           << ",\"pass_flow_parser_recovery_replay_case_present\":"
           << (pipeline_result.sema_pass_flow_summary
                       .parser_recovery_replay_case_present
                   ? "true"
                   : "false")
           << ",\"pass_flow_parser_recovery_replay_case_passed\":"
           << (pipeline_result.sema_pass_flow_summary
                       .parser_recovery_replay_case_passed
                   ? "true"
                   : "false")
           << ",\"pass_flow_recovery_replay_contract_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary
                       .recovery_replay_contract_satisfied
                   ? "true"
                   : "false")
           << ",\"pass_flow_recovery_replay_key\":\""
           << pipeline_result.sema_pass_flow_summary.recovery_replay_key
           << "\",\"pass_flow_recovery_replay_key_deterministic\":"
           << (pipeline_result.sema_pass_flow_summary
                       .recovery_replay_key_deterministic
                   ? "true"
                   : "false")
           << ",\"pass_flow_recovery_determinism_hardening_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary
                       .recovery_determinism_hardening_satisfied
                   ? "true"
                   : "false")
           << ",\"pass_flow_compatibility_handoff_consistent\":"
           << (pipeline_result.sema_pass_flow_summary
                       .compatibility_handoff_consistent
                   ? "true"
                   : "false")
           << ",\"pass_flow_robustness_guardrails_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary
                       .robustness_guardrails_satisfied
                   ? "true"
                   : "false")
           << ",\"pass_flow_symbol_counts_consistent\":"
           << (pipeline_result.sema_pass_flow_summary
                       .symbol_flow_counts_consistent
                   ? "true"
                   : "false")
           << ",\"pass_flow_fingerprint\":"
           << pipeline_result.sema_pass_flow_summary.pass_execution_fingerprint
           << ",\"pass_flow_deterministic_handoff_key\":\""
           << pipeline_result.sema_pass_flow_summary.deterministic_handoff_key
           << "\",\"pass_flow_replay_key_deterministic\":"
           << (pipeline_result.sema_pass_flow_summary.replay_key_deterministic
                   ? "true"
                   : "false")
           << ",\"pass_flow_deterministic\":"
           << (pipeline_result.sema_pass_flow_summary.deterministic ? "true"
                                                                    : "false");
}

void AppendObjc3FrontendArtifactManifestLoweringHeader(
    std::ostream &manifest,
    const Objc3FrontendOptions &options,
    const Objc3FrontendArtifactManifestLoweringHeaderFields
        &lowering_header_fields) {
  manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\""
           << options.lowering.runtime_dispatch_symbol
           << "\",\"runtime_dispatch_arg_slots\":"
           << options.lowering.max_message_send_args
           << ",\"selector_global_ordering\":\"lexicographic\"},\n";
  manifest << "  \"lowering_vector_abi\":{\"replay_key\":\""
           << kObjc3ArtifactSimdVectorTypeLoweringReplayKey
           << "\",\"lane_contract\":\"" << kObjc3ArtifactSimdVectorLaneContract
           << "\",\"vector_signature_functions\":"
           << lowering_header_fields.vector_signature_functions
           << "},\n";
  manifest << "  \"lowering_property_synthesis_ivar_binding\":{\"replay_key\":\""
           << lowering_header_fields.property_synthesis_ivar_binding_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3ArtifactPropertySynthesisIvarBindingLaneContract
           << "\",\"deterministic_handoff\":"
           << (lowering_header_fields.property_synthesis_ivar_binding_deterministic
                   ? "true"
                   : "false")
           << "},\n";
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

}  // namespace objc3::artifacts::frontend

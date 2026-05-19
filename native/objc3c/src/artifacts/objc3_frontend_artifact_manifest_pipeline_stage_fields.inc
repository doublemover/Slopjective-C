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

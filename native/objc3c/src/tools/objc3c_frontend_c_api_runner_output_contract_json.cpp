#include "tools/objc3c_frontend_c_api_runner_output_contract_json.h"

#include "io/objc3_cli_reporting_output_contract_scaffold.h"
#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerOutputContractJson(
    std::ostream &out,
    const std::string &indent,
    const FrontendCApiRunnerOutputContract &output_contract) {
  const auto &matrix = output_contract.conformance_matrix_surface;
  const auto &corpus = output_contract.conformance_corpus_surface;
  const std::string child_indent = indent + "  ";

  out << "{\n";
  out << child_indent << "\"diagnostics_schema_version\": \""
      << kObjc3CliReportingDiagnosticsSchemaVersion << "\",\n";
  out << child_indent << "\"summary_mode\": \""
      << kObjc3CliReportingSummaryMode << "\",\n";
  out << child_indent << "\"scaffold_key\": \""
      << EscapeJsonString(matrix.scaffold_key) << "\",\n";
  out << child_indent << "\"core_feature_key\": \""
      << EscapeJsonString(matrix.core_feature_key) << "\",\n";
  out << child_indent << "\"core_feature_expansion_key\": \""
      << EscapeJsonString(matrix.core_feature_expansion_key) << "\",\n";
  out << child_indent << "\"edge_case_compatibility_key\": \""
      << EscapeJsonString(matrix.edge_case_compatibility_key) << "\",\n";
  out << child_indent << "\"edge_case_robustness_key\": \""
      << EscapeJsonString(matrix.edge_case_robustness_key) << "\",\n";
  out << child_indent << "\"diagnostics_hardening_key\": \""
      << EscapeJsonString(matrix.diagnostics_hardening_key) << "\",\n";
  out << child_indent << "\"recovery_determinism_key\": \""
      << EscapeJsonString(matrix.recovery_determinism_key) << "\",\n";
  out << child_indent << "\"conformance_matrix_key\": \""
      << EscapeJsonString(matrix.conformance_matrix_key) << "\",\n";
  out << child_indent << "\"summary_output_path\": \""
      << EscapeJsonString(matrix.summary_output_path) << "\",\n";
  out << child_indent << "\"diagnostics_output_path\": \""
      << EscapeJsonString(matrix.diagnostics_output_path) << "\",\n";
  out << child_indent << "\"summary_output_path_contract_consistent\": "
      << (matrix.summary_output_path_contract_consistent ? "true" : "false")
      << ",\n";
  out << child_indent << "\"diagnostics_output_path_contract_consistent\": "
      << (matrix.diagnostics_output_path_contract_consistent ? "true"
                                                             : "false")
      << ",\n";
  out << child_indent << "\"diagnostics_filename_matches_emit_prefix\": "
      << (matrix.diagnostics_filename_matches_emit_prefix ? "true" : "false")
      << ",\n";
  out << child_indent << "\"core_feature_expansion_ready\": "
      << (matrix.core_feature_expansion_ready ? "true" : "false") << ",\n";
  out << child_indent << "\"summary_output_extension_compatible\": "
      << (matrix.summary_output_extension_compatible ? "true" : "false")
      << ",\n";
  out << child_indent << "\"diagnostics_output_suffix_compatible\": "
      << (matrix.diagnostics_output_suffix_compatible ? "true" : "false")
      << ",\n";
  out << child_indent << "\"case_folded_paths_distinct\": "
      << (matrix.case_folded_paths_distinct ? "true" : "false") << ",\n";
  out << child_indent << "\"output_paths_control_char_free\": "
      << (matrix.output_paths_control_char_free ? "true" : "false") << ",\n";
  out << child_indent << "\"edge_case_compatibility_consistent\": "
      << (matrix.edge_case_compatibility_consistent ? "true" : "false")
      << ",\n";
  out << child_indent << "\"edge_case_compatibility_ready\": "
      << (matrix.edge_case_compatibility_ready ? "true" : "false") << ",\n";
  out << child_indent << "\"summary_output_parent_present\": "
      << (matrix.summary_output_parent_present ? "true" : "false") << ",\n";
  out << child_indent << "\"diagnostics_output_parent_present\": "
      << (matrix.diagnostics_output_parent_present ? "true" : "false")
      << ",\n";
  out << child_indent << "\"output_paths_within_length_budget\": "
      << (matrix.output_paths_within_length_budget ? "true" : "false")
      << ",\n";
  out << child_indent << "\"output_paths_no_trailing_space\": "
      << (matrix.output_paths_no_trailing_space ? "true" : "false") << ",\n";
  out << child_indent << "\"edge_case_expansion_consistent\": "
      << (matrix.edge_case_expansion_consistent ? "true" : "false") << ",\n";
  out << child_indent << "\"edge_case_robustness_consistent\": "
      << (matrix.edge_case_robustness_consistent ? "true" : "false") << ",\n";
  out << child_indent << "\"edge_case_robustness_ready\": "
      << (matrix.edge_case_robustness_ready ? "true" : "false") << ",\n";
  out << child_indent << "\"diagnostics_hardening_consistent\": "
      << (matrix.diagnostics_hardening_consistent ? "true" : "false")
      << ",\n";
  out << child_indent << "\"diagnostics_hardening_ready\": "
      << (matrix.diagnostics_hardening_ready ? "true" : "false") << ",\n";
  out << child_indent << "\"recovery_determinism_consistent\": "
      << (matrix.recovery_determinism_consistent ? "true" : "false") << ",\n";
  out << child_indent << "\"recovery_determinism_ready\": "
      << (matrix.recovery_determinism_ready ? "true" : "false") << ",\n";
  out << child_indent << "\"conformance_matrix_consistent\": "
      << (matrix.conformance_matrix_consistent ? "true" : "false") << ",\n";
  out << child_indent << "\"conformance_matrix_ready\": "
      << (matrix.conformance_matrix_ready ? "true" : "false") << ",\n";
  out << child_indent << "\"conformance_matrix_key_ready\": "
      << (matrix.conformance_matrix_key_ready ? "true" : "false") << ",\n";
  out << child_indent << "\"conformance_corpus_key\": \""
      << EscapeJsonString(corpus.conformance_corpus_key) << "\",\n";
  out << child_indent << "\"conformance_corpus_case_count\": "
      << corpus.conformance_corpus_case_count << ",\n";
  out << child_indent << "\"conformance_corpus_accept_case_count\": "
      << corpus.conformance_corpus_accept_case_count << ",\n";
  out << child_indent << "\"conformance_corpus_reject_case_count\": "
      << corpus.conformance_corpus_reject_case_count << ",\n";
  out << child_indent << "\"conformance_corpus_consistent\": "
      << (corpus.conformance_corpus_consistent ? "true" : "false") << ",\n";
  out << child_indent << "\"conformance_corpus_ready\": "
      << (corpus.conformance_corpus_ready ? "true" : "false") << ",\n";
  out << child_indent << "\"conformance_corpus_key_ready\": "
      << (corpus.conformance_corpus_key_ready ? "true" : "false") << ",\n";
  out << child_indent << "\"core_feature_impl_ready\": "
      << (corpus.core_feature_impl_ready ? "true" : "false") << "\n";
  out << indent << "}";
}

#include "tools/objc3c_frontend_c_api_runner_output_contract_matrix_json_rows.h"

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerOutputContractMatrixKeyJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"scaffold_key\": \""
      << EscapeJsonString(matrix.scaffold_key) << "\",\n";
  out << indent << "\"core_feature_key\": \""
      << EscapeJsonString(matrix.core_feature_key) << "\",\n";
  out << indent << "\"core_feature_expansion_key\": \""
      << EscapeJsonString(matrix.core_feature_expansion_key) << "\",\n";
  out << indent << "\"edge_case_consistency_contract_key\": \""
      << EscapeJsonString(matrix.edge_case_consistency_contract_key) << "\",\n";
  out << indent << "\"edge_case_robustness_key\": \""
      << EscapeJsonString(matrix.edge_case_robustness_key) << "\",\n";
  out << indent << "\"diagnostics_hardening_key\": \""
      << EscapeJsonString(matrix.diagnostics_hardening_key) << "\",\n";
  out << indent << "\"recovery_determinism_key\": \""
      << EscapeJsonString(matrix.recovery_determinism_key) << "\",\n";
  out << indent << "\"conformance_matrix_key\": \""
      << EscapeJsonString(matrix.conformance_matrix_key) << "\",\n";
}

void WriteFrontendCApiRunnerOutputContractMatrixPathIdentityJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"summary_output_path\": \""
      << EscapeJsonString(matrix.summary_output_path) << "\",\n";
  out << indent << "\"diagnostics_output_path\": \""
      << EscapeJsonString(matrix.diagnostics_output_path) << "\",\n";
  out << indent << "\"summary_output_path_contract_consistent\": "
      << (matrix.summary_output_path_contract_consistent ? "true" : "false")
      << ",\n";
  out << indent << "\"diagnostics_output_path_contract_consistent\": "
      << (matrix.diagnostics_output_path_contract_consistent ? "true"
                                                             : "false")
      << ",\n";
  out << indent << "\"diagnostics_filename_matches_emit_prefix\": "
      << (matrix.diagnostics_filename_matches_emit_prefix ? "true" : "false")
      << ",\n";
}

void WriteFrontendCApiRunnerOutputContractMatrixCoreJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"core_feature_expansion_ready\": "
      << (matrix.core_feature_expansion_ready ? "true" : "false") << ",\n";
}

void WriteFrontendCApiRunnerOutputContractMatrixPathCompatibilityJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"summary_output_extension_compatible\": "
      << (matrix.summary_output_extension_compatible ? "true" : "false")
      << ",\n";
  out << indent << "\"diagnostics_output_suffix_compatible\": "
      << (matrix.diagnostics_output_suffix_compatible ? "true" : "false")
      << ",\n";
  out << indent << "\"case_folded_paths_distinct\": "
      << (matrix.case_folded_paths_distinct ? "true" : "false") << ",\n";
  out << indent << "\"output_paths_control_char_free\": "
      << (matrix.output_paths_control_char_free ? "true" : "false") << ",\n";
}

void WriteFrontendCApiRunnerOutputContractMatrixEdgeCaseContractJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"edge_case_consistency_contract_consistent\": "
      << (matrix.edge_case_consistency_contract_consistent ? "true" : "false")
      << ",\n";
  out << indent << "\"edge_case_consistency_contract_ready\": "
      << (matrix.edge_case_consistency_contract_ready ? "true" : "false")
      << ",\n";
}

void WriteFrontendCApiRunnerOutputContractMatrixPathBudgetJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"summary_output_parent_present\": "
      << (matrix.summary_output_parent_present ? "true" : "false") << ",\n";
  out << indent << "\"diagnostics_output_parent_present\": "
      << (matrix.diagnostics_output_parent_present ? "true" : "false")
      << ",\n";
  out << indent << "\"output_paths_within_length_budget\": "
      << (matrix.output_paths_within_length_budget ? "true" : "false")
      << ",\n";
  out << indent << "\"output_paths_no_trailing_space\": "
      << (matrix.output_paths_no_trailing_space ? "true" : "false") << ",\n";
}

void WriteFrontendCApiRunnerOutputContractMatrixEdgeCaseRobustnessJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"edge_case_expansion_consistent\": "
      << (matrix.edge_case_expansion_consistent ? "true" : "false") << ",\n";
  out << indent << "\"edge_case_robustness_consistent\": "
      << (matrix.edge_case_robustness_consistent ? "true" : "false") << ",\n";
  out << indent << "\"edge_case_robustness_ready\": "
      << (matrix.edge_case_robustness_ready ? "true" : "false") << ",\n";
}

void WriteFrontendCApiRunnerOutputContractMatrixHardeningJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"diagnostics_hardening_consistent\": "
      << (matrix.diagnostics_hardening_consistent ? "true" : "false")
      << ",\n";
  out << indent << "\"diagnostics_hardening_ready\": "
      << (matrix.diagnostics_hardening_ready ? "true" : "false") << ",\n";
  out << indent << "\"recovery_determinism_consistent\": "
      << (matrix.recovery_determinism_consistent ? "true" : "false") << ",\n";
  out << indent << "\"recovery_determinism_ready\": "
      << (matrix.recovery_determinism_ready ? "true" : "false") << ",\n";
}

void WriteFrontendCApiRunnerOutputContractMatrixReadinessJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &matrix) {
  out << indent << "\"conformance_matrix_consistent\": "
      << (matrix.conformance_matrix_consistent ? "true" : "false") << ",\n";
  out << indent << "\"conformance_matrix_ready\": "
      << (matrix.conformance_matrix_ready ? "true" : "false") << ",\n";
  out << indent << "\"conformance_matrix_key_ready\": "
      << (matrix.conformance_matrix_key_ready ? "true" : "false") << ",\n";
}

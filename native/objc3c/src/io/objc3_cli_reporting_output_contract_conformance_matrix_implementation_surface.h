#pragma once

#include <sstream>
#include <string>

#include "io/objc3_cli_reporting_output_contract_recovery_determinism_hardening_surface.h"

struct Objc3CliReportingOutputContractConformanceMatrixImplementationSurface {
  bool summary_output_path_contract_consistent = false;
  bool diagnostics_output_path_contract_consistent = false;
  bool diagnostics_filename_matches_emit_prefix = false;
  bool core_feature_expansion_ready = false;
  bool summary_output_extension_compatible = false;
  bool diagnostics_output_suffix_compatible = false;
  bool case_folded_paths_distinct = false;
  bool output_paths_control_char_free = false;
  bool edge_case_compatibility_consistent = false;
  bool edge_case_compatibility_ready = false;
  bool summary_output_parent_present = false;
  bool diagnostics_output_parent_present = false;
  bool output_paths_within_length_budget = false;
  bool output_paths_no_trailing_space = false;
  bool edge_case_expansion_consistent = false;
  bool edge_case_robustness_consistent = false;
  bool edge_case_robustness_ready = false;
  bool diagnostics_hardening_consistent = false;
  bool diagnostics_hardening_ready = false;
  bool diagnostics_hardening_key_ready = false;
  bool recovery_determinism_consistent = false;
  bool recovery_determinism_ready = false;
  bool recovery_determinism_key_ready = false;
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  bool conformance_matrix_key_ready = false;
  bool core_feature_impl_ready = false;
  std::string scaffold_key;
  std::string core_feature_key;
  std::string core_feature_expansion_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string recovery_determinism_key;
  std::string conformance_matrix_key;
  std::string summary_output_path;
  std::string diagnostics_output_path;
  std::string failure_reason;
};

inline std::string
BuildObjc3CliReportingOutputContractConformanceMatrixImplementationKey(
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &surface) {
  std::ostringstream key;
  key << "cli-reporting-output-contract-conformance-matrix-implementation:v1:"
      << "recovery_determinism_ready="
      << (surface.recovery_determinism_ready ? "true" : "false")
      << ";recovery_determinism_key_ready="
      << (surface.recovery_determinism_key_ready ? "true" : "false")
      << ";summary_output_path_contract_consistent="
      << (surface.summary_output_path_contract_consistent ? "true" : "false")
      << ";diagnostics_output_path_contract_consistent="
      << (surface.diagnostics_output_path_contract_consistent ? "true" : "false")
      << ";conformance_matrix_consistent="
      << (surface.conformance_matrix_consistent ? "true" : "false")
      << ";conformance_matrix_ready="
      << (surface.conformance_matrix_ready ? "true" : "false")
      << ";conformance_matrix_key_ready="
      << (surface.conformance_matrix_key_ready ? "true" : "false")
      << ";core_feature_impl_ready="
      << (surface.core_feature_impl_ready ? "true" : "false")
      << ";recovery_determinism_key=" << surface.recovery_determinism_key
      << ";summary_output_path=" << surface.summary_output_path
      << ";diagnostics_output_path=" << surface.diagnostics_output_path;
  return key.str();
}

inline Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
BuildObjc3CliReportingOutputContractConformanceMatrixImplementationSurface(
    const Objc3CliReportingOutputContractRecoveryDeterminismHardeningSurface
        &recovery_surface) {
  Objc3CliReportingOutputContractConformanceMatrixImplementationSurface surface;
  surface.summary_output_path_contract_consistent =
      recovery_surface.summary_output_path_contract_consistent;
  surface.diagnostics_output_path_contract_consistent =
      recovery_surface.diagnostics_output_path_contract_consistent;
  surface.diagnostics_filename_matches_emit_prefix =
      recovery_surface.diagnostics_filename_matches_emit_prefix;
  surface.core_feature_expansion_ready =
      recovery_surface.core_feature_expansion_ready;
  surface.summary_output_extension_compatible =
      recovery_surface.summary_output_extension_compatible;
  surface.diagnostics_output_suffix_compatible =
      recovery_surface.diagnostics_output_suffix_compatible;
  surface.case_folded_paths_distinct = recovery_surface.case_folded_paths_distinct;
  surface.output_paths_control_char_free =
      recovery_surface.output_paths_control_char_free;
  surface.edge_case_compatibility_consistent =
      recovery_surface.edge_case_compatibility_consistent;
  surface.edge_case_compatibility_ready =
      recovery_surface.edge_case_compatibility_ready;
  surface.summary_output_parent_present =
      recovery_surface.summary_output_parent_present;
  surface.diagnostics_output_parent_present =
      recovery_surface.diagnostics_output_parent_present;
  surface.output_paths_within_length_budget =
      recovery_surface.output_paths_within_length_budget;
  surface.output_paths_no_trailing_space =
      recovery_surface.output_paths_no_trailing_space;
  surface.edge_case_expansion_consistent =
      recovery_surface.edge_case_expansion_consistent;
  surface.edge_case_robustness_consistent =
      recovery_surface.edge_case_robustness_consistent;
  surface.edge_case_robustness_ready = recovery_surface.edge_case_robustness_ready;
  surface.diagnostics_hardening_consistent =
      recovery_surface.diagnostics_hardening_consistent;
  surface.diagnostics_hardening_ready = recovery_surface.diagnostics_hardening_ready;
  surface.diagnostics_hardening_key_ready =
      recovery_surface.diagnostics_hardening_key_ready;
  surface.recovery_determinism_consistent =
      recovery_surface.recovery_determinism_consistent;
  surface.recovery_determinism_ready = recovery_surface.recovery_determinism_ready;
  surface.recovery_determinism_key_ready =
      recovery_surface.recovery_determinism_key_ready;
  surface.core_feature_impl_ready = recovery_surface.core_feature_impl_ready;
  surface.scaffold_key = recovery_surface.scaffold_key;
  surface.core_feature_key = recovery_surface.core_feature_key;
  surface.core_feature_expansion_key = recovery_surface.core_feature_expansion_key;
  surface.edge_case_compatibility_key = recovery_surface.edge_case_compatibility_key;
  surface.edge_case_robustness_key = recovery_surface.edge_case_robustness_key;
  surface.diagnostics_hardening_key = recovery_surface.diagnostics_hardening_key;
  surface.recovery_determinism_key = recovery_surface.recovery_determinism_key;
  surface.summary_output_path = recovery_surface.summary_output_path;
  surface.diagnostics_output_path = recovery_surface.diagnostics_output_path;

  surface.conformance_matrix_consistent =
      surface.recovery_determinism_consistent &&
      surface.recovery_determinism_key_ready &&
      surface.summary_output_path_contract_consistent &&
      surface.diagnostics_output_path_contract_consistent &&
      !surface.recovery_determinism_key.empty();
  surface.conformance_matrix_ready =
      surface.recovery_determinism_ready &&
      surface.conformance_matrix_consistent &&
      !surface.summary_output_path.empty() &&
      !surface.diagnostics_output_path.empty();
  surface.conformance_matrix_key =
      BuildObjc3CliReportingOutputContractConformanceMatrixImplementationKey(
          surface);
  surface.conformance_matrix_key_ready =
      surface.conformance_matrix_ready &&
      !surface.conformance_matrix_key.empty() &&
      surface.conformance_matrix_key.find(
          ";recovery_determinism_key=" + surface.recovery_determinism_key) !=
          std::string::npos &&
      surface.conformance_matrix_key.find(
          ";summary_output_path=" + surface.summary_output_path) !=
          std::string::npos &&
      surface.conformance_matrix_key.find(
          ";diagnostics_output_path=" + surface.diagnostics_output_path) !=
          std::string::npos;
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready &&
      surface.conformance_matrix_ready &&
      surface.conformance_matrix_key_ready;

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!surface.recovery_determinism_ready) {
    surface.failure_reason = recovery_surface.failure_reason.empty()
                                 ? "cli/reporting output recovery and determinism hardening surface is not ready"
                                 : recovery_surface.failure_reason;
  } else if (!surface.conformance_matrix_consistent) {
    surface.failure_reason =
        "cli/reporting output conformance matrix implementation is inconsistent";
  } else if (!surface.conformance_matrix_ready) {
    surface.failure_reason =
        "cli/reporting output conformance matrix implementation is not ready";
  } else if (!surface.conformance_matrix_key_ready) {
    surface.failure_reason =
        "cli/reporting output conformance matrix implementation key is not ready";
  } else {
    surface.failure_reason =
        "cli/reporting output conformance matrix implementation surface is not ready";
  }

  return surface;
}

inline bool
IsObjc3CliReportingOutputContractConformanceMatrixImplementationSurfaceReady(
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "cli/reporting output conformance matrix implementation surface not ready"
               : surface.failure_reason;
  return false;
}

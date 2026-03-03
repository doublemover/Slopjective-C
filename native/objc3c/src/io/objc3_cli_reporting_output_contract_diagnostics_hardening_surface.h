#pragma once

#include <sstream>
#include <string>

#include "io/objc3_cli_reporting_output_contract_edge_case_expansion_and_robustness_surface.h"

struct Objc3CliReportingOutputContractDiagnosticsHardeningSurface {
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
  bool core_feature_impl_ready = false;
  std::string scaffold_key;
  std::string core_feature_key;
  std::string core_feature_expansion_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string summary_output_path;
  std::string diagnostics_output_path;
  std::string failure_reason;
};

inline std::string BuildObjc3CliReportingOutputContractDiagnosticsHardeningKey(
    const Objc3CliReportingOutputContractDiagnosticsHardeningSurface &surface) {
  std::ostringstream key;
  key << "cli-reporting-output-contract-diagnostics-hardening:v1:"
      << "edge_case_robustness_ready="
      << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";summary_output_path_contract_consistent="
      << (surface.summary_output_path_contract_consistent ? "true" : "false")
      << ";diagnostics_output_path_contract_consistent="
      << (surface.diagnostics_output_path_contract_consistent ? "true" : "false")
      << ";diagnostics_filename_matches_emit_prefix="
      << (surface.diagnostics_filename_matches_emit_prefix ? "true" : "false")
      << ";output_paths_no_trailing_space="
      << (surface.output_paths_no_trailing_space ? "true" : "false")
      << ";diagnostics_hardening_consistent="
      << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diagnostics_hardening_ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";core_feature_impl_ready="
      << (surface.core_feature_impl_ready ? "true" : "false")
      << ";summary_output_path=" << surface.summary_output_path
      << ";diagnostics_output_path=" << surface.diagnostics_output_path
      << ";edge_case_robustness_key=" << surface.edge_case_robustness_key;
  return key.str();
}

inline Objc3CliReportingOutputContractDiagnosticsHardeningSurface
BuildObjc3CliReportingOutputContractDiagnosticsHardeningSurface(
    const Objc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface
        &edge_case_robustness_surface) {
  Objc3CliReportingOutputContractDiagnosticsHardeningSurface surface;
  surface.summary_output_path_contract_consistent =
      edge_case_robustness_surface.summary_output_path_contract_consistent;
  surface.diagnostics_output_path_contract_consistent =
      edge_case_robustness_surface.diagnostics_output_path_contract_consistent;
  surface.diagnostics_filename_matches_emit_prefix =
      edge_case_robustness_surface.diagnostics_filename_matches_emit_prefix;
  surface.core_feature_expansion_ready =
      edge_case_robustness_surface.core_feature_expansion_ready;
  surface.summary_output_extension_compatible =
      edge_case_robustness_surface.summary_output_extension_compatible;
  surface.diagnostics_output_suffix_compatible =
      edge_case_robustness_surface.diagnostics_output_suffix_compatible;
  surface.case_folded_paths_distinct =
      edge_case_robustness_surface.case_folded_paths_distinct;
  surface.output_paths_control_char_free =
      edge_case_robustness_surface.output_paths_control_char_free;
  surface.edge_case_compatibility_consistent =
      edge_case_robustness_surface.edge_case_compatibility_consistent;
  surface.edge_case_compatibility_ready =
      edge_case_robustness_surface.edge_case_compatibility_ready;
  surface.summary_output_parent_present =
      edge_case_robustness_surface.summary_output_parent_present;
  surface.diagnostics_output_parent_present =
      edge_case_robustness_surface.diagnostics_output_parent_present;
  surface.output_paths_within_length_budget =
      edge_case_robustness_surface.output_paths_within_length_budget;
  surface.output_paths_no_trailing_space =
      edge_case_robustness_surface.output_paths_no_trailing_space;
  surface.edge_case_expansion_consistent =
      edge_case_robustness_surface.edge_case_expansion_consistent;
  surface.edge_case_robustness_consistent =
      edge_case_robustness_surface.edge_case_robustness_consistent;
  surface.edge_case_robustness_ready =
      edge_case_robustness_surface.edge_case_robustness_ready;
  surface.core_feature_impl_ready =
      edge_case_robustness_surface.core_feature_impl_ready;
  surface.scaffold_key = edge_case_robustness_surface.scaffold_key;
  surface.core_feature_key = edge_case_robustness_surface.core_feature_key;
  surface.core_feature_expansion_key =
      edge_case_robustness_surface.core_feature_expansion_key;
  surface.edge_case_compatibility_key =
      edge_case_robustness_surface.edge_case_compatibility_key;
  surface.edge_case_robustness_key =
      edge_case_robustness_surface.edge_case_robustness_key;
  surface.summary_output_path = edge_case_robustness_surface.summary_output_path;
  surface.diagnostics_output_path =
      edge_case_robustness_surface.diagnostics_output_path;

  surface.diagnostics_hardening_consistent =
      surface.edge_case_expansion_consistent &&
      surface.edge_case_robustness_consistent &&
      surface.summary_output_path_contract_consistent &&
      surface.diagnostics_output_path_contract_consistent &&
      surface.diagnostics_filename_matches_emit_prefix &&
      surface.output_paths_no_trailing_space &&
      !surface.edge_case_robustness_key.empty();
  surface.diagnostics_hardening_key =
      BuildObjc3CliReportingOutputContractDiagnosticsHardeningKey(surface);
  surface.diagnostics_hardening_ready =
      surface.edge_case_robustness_ready &&
      surface.diagnostics_hardening_consistent;
  surface.diagnostics_hardening_key_ready =
      surface.diagnostics_hardening_ready &&
      !surface.diagnostics_hardening_key.empty() &&
      surface.diagnostics_hardening_key.find(
          "edge_case_robustness_key=" + surface.edge_case_robustness_key) !=
          std::string::npos &&
      surface.diagnostics_hardening_key.find(
          ";summary_output_path=" + surface.summary_output_path) !=
          std::string::npos &&
      surface.diagnostics_hardening_key.find(
          ";diagnostics_output_path=" + surface.diagnostics_output_path) !=
          std::string::npos;
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready &&
      surface.diagnostics_hardening_ready &&
      surface.diagnostics_hardening_key_ready;

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!surface.edge_case_robustness_ready) {
    surface.failure_reason = edge_case_robustness_surface.failure_reason.empty()
                                 ? "cli/reporting output edge-case robustness surface is not ready"
                                 : edge_case_robustness_surface.failure_reason;
  } else if (!surface.diagnostics_hardening_consistent) {
    surface.failure_reason =
        "cli/reporting output diagnostics hardening is inconsistent";
  } else if (!surface.diagnostics_hardening_ready) {
    surface.failure_reason =
        "cli/reporting output diagnostics hardening is not ready";
  } else if (!surface.diagnostics_hardening_key_ready) {
    surface.failure_reason =
        "cli/reporting output diagnostics hardening key is not ready";
  } else {
    surface.failure_reason =
        "cli/reporting output diagnostics hardening surface is not ready";
  }

  return surface;
}

inline bool IsObjc3CliReportingOutputContractDiagnosticsHardeningSurfaceReady(
    const Objc3CliReportingOutputContractDiagnosticsHardeningSurface &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "cli/reporting output diagnostics hardening surface not ready"
               : surface.failure_reason;
  return false;
}

#pragma once

#include <cctype>
#include <sstream>
#include <string>

#include "io/objc3_cli_reporting_output_contract_edge_case_compatibility_surface.h"

struct Objc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface {
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
  bool core_feature_impl_ready = false;
  std::string scaffold_key;
  std::string core_feature_key;
  std::string core_feature_expansion_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string summary_output_path;
  std::string diagnostics_output_path;
  std::string failure_reason;
};

inline bool Objc3CliReportingOutputContractHasParentPath(
    const std::string &path_text) {
  return path_text.find('/') != std::string::npos;
}

inline bool Objc3CliReportingOutputContractHasTrailingSpace(
    const std::string &path_text) {
  return !path_text.empty() &&
         std::isspace(static_cast<unsigned char>(path_text.back())) != 0;
}

inline std::string
BuildObjc3CliReportingOutputContractEdgeCaseRobustnessKey(
    const Objc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface
        &surface) {
  std::ostringstream key;
  key << "cli-reporting-output-contract-edge-case-robustness:v1:"
      << "edge_case_compatibility_ready="
      << (surface.edge_case_compatibility_ready ? "true" : "false")
      << ";summary_output_parent_present="
      << (surface.summary_output_parent_present ? "true" : "false")
      << ";diagnostics_output_parent_present="
      << (surface.diagnostics_output_parent_present ? "true" : "false")
      << ";output_paths_within_length_budget="
      << (surface.output_paths_within_length_budget ? "true" : "false")
      << ";output_paths_no_trailing_space="
      << (surface.output_paths_no_trailing_space ? "true" : "false")
      << ";edge_case_expansion_consistent="
      << (surface.edge_case_expansion_consistent ? "true" : "false")
      << ";edge_case_robustness_consistent="
      << (surface.edge_case_robustness_consistent ? "true" : "false")
      << ";edge_case_robustness_ready="
      << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";core_feature_impl_ready="
      << (surface.core_feature_impl_ready ? "true" : "false")
      << ";edge_case_compatibility_key=" << surface.edge_case_compatibility_key;
  return key.str();
}

inline Objc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface
BuildObjc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface(
    const Objc3CliReportingOutputContractEdgeCaseCompatibilitySurface
        &edge_case_compatibility_surface) {
  Objc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface surface;
  surface.edge_case_compatibility_consistent =
      edge_case_compatibility_surface.edge_case_compatibility_consistent;
  surface.edge_case_compatibility_ready =
      edge_case_compatibility_surface.edge_case_compatibility_ready;
  surface.summary_output_path_contract_consistent =
      edge_case_compatibility_surface.summary_output_path_contract_consistent;
  surface.diagnostics_output_path_contract_consistent =
      edge_case_compatibility_surface.diagnostics_output_path_contract_consistent;
  surface.diagnostics_filename_matches_emit_prefix =
      edge_case_compatibility_surface.diagnostics_filename_matches_emit_prefix;
  surface.core_feature_expansion_ready =
      edge_case_compatibility_surface.core_feature_expansion_ready;
  surface.summary_output_extension_compatible =
      edge_case_compatibility_surface.summary_output_extension_compatible;
  surface.diagnostics_output_suffix_compatible =
      edge_case_compatibility_surface.diagnostics_output_suffix_compatible;
  surface.case_folded_paths_distinct =
      edge_case_compatibility_surface.case_folded_paths_distinct;
  surface.output_paths_control_char_free =
      edge_case_compatibility_surface.output_paths_control_char_free;
  surface.core_feature_impl_ready =
      edge_case_compatibility_surface.core_feature_impl_ready;
  surface.scaffold_key = edge_case_compatibility_surface.scaffold_key;
  surface.core_feature_key = edge_case_compatibility_surface.core_feature_key;
  surface.core_feature_expansion_key =
      edge_case_compatibility_surface.core_feature_expansion_key;
  surface.edge_case_compatibility_key =
      edge_case_compatibility_surface.edge_case_compatibility_key;
  surface.summary_output_path = edge_case_compatibility_surface.summary_output_path;
  surface.diagnostics_output_path =
      edge_case_compatibility_surface.diagnostics_output_path;

  surface.summary_output_parent_present =
      Objc3CliReportingOutputContractHasParentPath(surface.summary_output_path);
  surface.diagnostics_output_parent_present =
      Objc3CliReportingOutputContractHasParentPath(
          surface.diagnostics_output_path);
  surface.output_paths_within_length_budget =
      !surface.summary_output_path.empty() &&
      !surface.diagnostics_output_path.empty() &&
      surface.summary_output_path.size() <= 240 &&
      surface.diagnostics_output_path.size() <= 240;
  surface.output_paths_no_trailing_space =
      !Objc3CliReportingOutputContractHasTrailingSpace(
          surface.summary_output_path) &&
      !Objc3CliReportingOutputContractHasTrailingSpace(
          surface.diagnostics_output_path);

  surface.edge_case_expansion_consistent =
      surface.edge_case_compatibility_consistent &&
      surface.summary_output_parent_present &&
      surface.diagnostics_output_parent_present;
  surface.edge_case_robustness_consistent =
      surface.output_paths_within_length_budget &&
      surface.output_paths_no_trailing_space;
  surface.edge_case_robustness_key =
      BuildObjc3CliReportingOutputContractEdgeCaseRobustnessKey(surface);
  surface.edge_case_robustness_ready =
      surface.edge_case_compatibility_ready &&
      surface.edge_case_expansion_consistent &&
      surface.edge_case_robustness_consistent &&
      !surface.edge_case_robustness_key.empty();
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.edge_case_robustness_ready;

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!surface.edge_case_compatibility_ready) {
    surface.failure_reason =
        edge_case_compatibility_surface.failure_reason.empty()
            ? "cli/reporting output edge-case compatibility surface is not ready"
            : edge_case_compatibility_surface.failure_reason;
  } else if (!surface.edge_case_expansion_consistent) {
    surface.failure_reason =
        "cli/reporting output edge-case expansion is inconsistent";
  } else if (!surface.edge_case_robustness_consistent) {
    surface.failure_reason =
        "cli/reporting output edge-case robustness is inconsistent";
  } else if (!surface.edge_case_robustness_ready) {
    surface.failure_reason =
        "cli/reporting output edge-case robustness is not ready";
  } else if (surface.edge_case_robustness_key.empty()) {
    surface.failure_reason =
        "cli/reporting output edge-case robustness key is empty";
  } else {
    surface.failure_reason =
        "cli/reporting output edge-case expansion and robustness is not ready";
  }

  return surface;
}

inline bool
IsObjc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurfaceReady(
    const Objc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface
        &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "cli/reporting output edge-case expansion and robustness surface not ready"
               : surface.failure_reason;
  return false;
}

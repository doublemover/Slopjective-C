#pragma once

#include <sstream>
#include <string>

#include "io/objc3_cli_reporting_output_contract_diagnostics_hardening_surface.h"

struct Objc3CliReportingOutputContractRecoveryDeterminismHardeningSurface {
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
  bool core_feature_impl_ready = false;
  std::string scaffold_key;
  std::string core_feature_key;
  std::string core_feature_expansion_key;
  std::string edge_case_compatibility_key;
  std::string edge_case_robustness_key;
  std::string diagnostics_hardening_key;
  std::string recovery_determinism_key;
  std::string summary_output_path;
  std::string diagnostics_output_path;
  std::string failure_reason;
};

inline std::string
BuildObjc3CliReportingOutputContractRecoveryDeterminismHardeningKey(
    const Objc3CliReportingOutputContractRecoveryDeterminismHardeningSurface
        &surface) {
  std::ostringstream key;
  key << "cli-reporting-output-contract-recovery-determinism-hardening:v1:"
      << "diagnostics_hardening_ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";diagnostics_hardening_key_ready="
      << (surface.diagnostics_hardening_key_ready ? "true" : "false")
      << ";edge_case_robustness_ready="
      << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";summary_output_path_contract_consistent="
      << (surface.summary_output_path_contract_consistent ? "true" : "false")
      << ";diagnostics_output_path_contract_consistent="
      << (surface.diagnostics_output_path_contract_consistent ? "true" : "false")
      << ";output_paths_no_trailing_space="
      << (surface.output_paths_no_trailing_space ? "true" : "false")
      << ";recovery_determinism_consistent="
      << (surface.recovery_determinism_consistent ? "true" : "false")
      << ";recovery_determinism_ready="
      << (surface.recovery_determinism_ready ? "true" : "false")
      << ";core_feature_impl_ready="
      << (surface.core_feature_impl_ready ? "true" : "false")
      << ";diagnostics_hardening_key=" << surface.diagnostics_hardening_key
      << ";edge_case_robustness_key=" << surface.edge_case_robustness_key
      << ";summary_output_path=" << surface.summary_output_path
      << ";diagnostics_output_path=" << surface.diagnostics_output_path;
  return key.str();
}

inline Objc3CliReportingOutputContractRecoveryDeterminismHardeningSurface
BuildObjc3CliReportingOutputContractRecoveryDeterminismHardeningSurface(
    const Objc3CliReportingOutputContractDiagnosticsHardeningSurface
        &diagnostics_surface) {
  Objc3CliReportingOutputContractRecoveryDeterminismHardeningSurface surface;
  surface.summary_output_path_contract_consistent =
      diagnostics_surface.summary_output_path_contract_consistent;
  surface.diagnostics_output_path_contract_consistent =
      diagnostics_surface.diagnostics_output_path_contract_consistent;
  surface.diagnostics_filename_matches_emit_prefix =
      diagnostics_surface.diagnostics_filename_matches_emit_prefix;
  surface.core_feature_expansion_ready =
      diagnostics_surface.core_feature_expansion_ready;
  surface.summary_output_extension_compatible =
      diagnostics_surface.summary_output_extension_compatible;
  surface.diagnostics_output_suffix_compatible =
      diagnostics_surface.diagnostics_output_suffix_compatible;
  surface.case_folded_paths_distinct = diagnostics_surface.case_folded_paths_distinct;
  surface.output_paths_control_char_free =
      diagnostics_surface.output_paths_control_char_free;
  surface.edge_case_compatibility_consistent =
      diagnostics_surface.edge_case_compatibility_consistent;
  surface.edge_case_compatibility_ready =
      diagnostics_surface.edge_case_compatibility_ready;
  surface.summary_output_parent_present =
      diagnostics_surface.summary_output_parent_present;
  surface.diagnostics_output_parent_present =
      diagnostics_surface.diagnostics_output_parent_present;
  surface.output_paths_within_length_budget =
      diagnostics_surface.output_paths_within_length_budget;
  surface.output_paths_no_trailing_space =
      diagnostics_surface.output_paths_no_trailing_space;
  surface.edge_case_expansion_consistent =
      diagnostics_surface.edge_case_expansion_consistent;
  surface.edge_case_robustness_consistent =
      diagnostics_surface.edge_case_robustness_consistent;
  surface.edge_case_robustness_ready = diagnostics_surface.edge_case_robustness_ready;
  surface.diagnostics_hardening_consistent =
      diagnostics_surface.diagnostics_hardening_consistent;
  surface.diagnostics_hardening_ready = diagnostics_surface.diagnostics_hardening_ready;
  surface.diagnostics_hardening_key_ready =
      diagnostics_surface.diagnostics_hardening_key_ready;
  surface.core_feature_impl_ready = diagnostics_surface.core_feature_impl_ready;
  surface.scaffold_key = diagnostics_surface.scaffold_key;
  surface.core_feature_key = diagnostics_surface.core_feature_key;
  surface.core_feature_expansion_key = diagnostics_surface.core_feature_expansion_key;
  surface.edge_case_compatibility_key = diagnostics_surface.edge_case_compatibility_key;
  surface.edge_case_robustness_key = diagnostics_surface.edge_case_robustness_key;
  surface.diagnostics_hardening_key = diagnostics_surface.diagnostics_hardening_key;
  surface.summary_output_path = diagnostics_surface.summary_output_path;
  surface.diagnostics_output_path = diagnostics_surface.diagnostics_output_path;

  surface.recovery_determinism_consistent =
      surface.diagnostics_hardening_consistent &&
      surface.diagnostics_hardening_key_ready &&
      surface.edge_case_robustness_consistent &&
      surface.summary_output_path_contract_consistent &&
      surface.diagnostics_output_path_contract_consistent &&
      surface.output_paths_no_trailing_space &&
      !surface.diagnostics_hardening_key.empty();
  surface.recovery_determinism_ready =
      surface.diagnostics_hardening_ready &&
      surface.recovery_determinism_consistent &&
      surface.edge_case_robustness_ready &&
      !surface.summary_output_path.empty() &&
      !surface.diagnostics_output_path.empty();
  surface.recovery_determinism_key =
      BuildObjc3CliReportingOutputContractRecoveryDeterminismHardeningKey(
          surface);
  surface.recovery_determinism_key_ready =
      surface.recovery_determinism_ready &&
      !surface.recovery_determinism_key.empty() &&
      surface.recovery_determinism_key.find(
          ";diagnostics_hardening_key=" + surface.diagnostics_hardening_key) !=
          std::string::npos &&
      surface.recovery_determinism_key.find(
          ";edge_case_robustness_key=" + surface.edge_case_robustness_key) !=
          std::string::npos &&
      surface.recovery_determinism_key.find(
          ";summary_output_path=" + surface.summary_output_path) !=
          std::string::npos &&
      surface.recovery_determinism_key.find(
          ";diagnostics_output_path=" + surface.diagnostics_output_path) !=
          std::string::npos;
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready &&
      surface.recovery_determinism_ready &&
      surface.recovery_determinism_key_ready;

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!surface.diagnostics_hardening_ready) {
    surface.failure_reason = diagnostics_surface.failure_reason.empty()
                                 ? "cli/reporting output diagnostics hardening surface is not ready"
                                 : diagnostics_surface.failure_reason;
  } else if (!surface.recovery_determinism_consistent) {
    surface.failure_reason =
        "cli/reporting output recovery and determinism hardening is inconsistent";
  } else if (!surface.recovery_determinism_ready) {
    surface.failure_reason =
        "cli/reporting output recovery and determinism hardening is not ready";
  } else if (!surface.recovery_determinism_key_ready) {
    surface.failure_reason =
        "cli/reporting output recovery and determinism hardening key is not ready";
  } else {
    surface.failure_reason =
        "cli/reporting output recovery and determinism hardening surface is not ready";
  }

  return surface;
}

inline bool
IsObjc3CliReportingOutputContractRecoveryDeterminismHardeningSurfaceReady(
    const Objc3CliReportingOutputContractRecoveryDeterminismHardeningSurface
        &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "cli/reporting output recovery and determinism hardening surface not ready"
               : surface.failure_reason;
  return false;
}

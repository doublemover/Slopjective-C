#pragma once

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>

#include "io/objc3_cli_reporting_output_contract_core_feature_expansion_surface.h"

struct Objc3CliReportingOutputContractEdgeCaseCompatibilitySurface {
  bool core_feature_expansion_ready = false;
  bool summary_output_path_contract_consistent = false;
  bool diagnostics_output_path_contract_consistent = false;
  bool diagnostics_filename_matches_emit_prefix = false;
  bool summary_output_extension_compatible = false;
  bool diagnostics_output_suffix_compatible = false;
  bool case_folded_paths_distinct = false;
  bool output_paths_control_char_free = false;
  bool edge_case_compatibility_consistent = false;
  bool edge_case_compatibility_ready = false;
  bool core_feature_impl_ready = false;
  std::string scaffold_key;
  std::string core_feature_key;
  std::string core_feature_expansion_key;
  std::string edge_case_compatibility_key;
  std::string summary_output_path;
  std::string diagnostics_output_path;
  std::string failure_reason;
};

inline bool Objc3CliReportingOutputContractHasTextSuffix(
    const std::string &value,
    const std::string &suffix) {
  return value.size() >= suffix.size() &&
         value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline bool Objc3CliReportingOutputContractContainsControlCharacters(
    const std::string &value) {
  for (unsigned char code_point : value) {
    if (code_point < 0x20) {
      return true;
    }
  }
  return false;
}

inline std::string Objc3CliReportingOutputContractToCaseFoldedAscii(
    std::string value) {
  std::transform(
      value.begin(),
      value.end(),
      value.begin(),
      [](unsigned char code_point) {
        return static_cast<char>(std::tolower(code_point));
      });
  return value;
}

inline std::string BuildObjc3CliReportingOutputContractEdgeCaseCompatibilityKey(
    const Objc3CliReportingOutputContractEdgeCaseCompatibilitySurface &surface) {
  std::ostringstream key;
  key << "cli-reporting-output-contract-edge-case-compatibility:v1:"
      << "core_feature_expansion_ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";summary_output_extension_compatible="
      << (surface.summary_output_extension_compatible ? "true" : "false")
      << ";diagnostics_output_suffix_compatible="
      << (surface.diagnostics_output_suffix_compatible ? "true" : "false")
      << ";case_folded_paths_distinct="
      << (surface.case_folded_paths_distinct ? "true" : "false")
      << ";output_paths_control_char_free="
      << (surface.output_paths_control_char_free ? "true" : "false")
      << ";edge_case_compatibility_consistent="
      << (surface.edge_case_compatibility_consistent ? "true" : "false")
      << ";edge_case_compatibility_ready="
      << (surface.edge_case_compatibility_ready ? "true" : "false")
      << ";core_feature_impl_ready="
      << (surface.core_feature_impl_ready ? "true" : "false")
      << ";core_feature_expansion_key=" << surface.core_feature_expansion_key;
  return key.str();
}

inline Objc3CliReportingOutputContractEdgeCaseCompatibilitySurface
BuildObjc3CliReportingOutputContractEdgeCaseCompatibilitySurface(
    const Objc3CliReportingOutputContractCoreFeatureExpansionSurface
        &core_feature_expansion_surface) {
  Objc3CliReportingOutputContractEdgeCaseCompatibilitySurface surface;
  surface.core_feature_expansion_ready =
      core_feature_expansion_surface.core_feature_expansion_ready;
  surface.summary_output_path_contract_consistent =
      core_feature_expansion_surface.summary_output_path_contract_consistent;
  surface.diagnostics_output_path_contract_consistent =
      core_feature_expansion_surface.diagnostics_output_path_contract_consistent;
  surface.diagnostics_filename_matches_emit_prefix =
      core_feature_expansion_surface.diagnostics_filename_matches_emit_prefix;
  surface.core_feature_impl_ready =
      core_feature_expansion_surface.core_feature_impl_ready;
  surface.scaffold_key = core_feature_expansion_surface.scaffold_key;
  surface.core_feature_key = core_feature_expansion_surface.core_feature_key;
  surface.core_feature_expansion_key =
      core_feature_expansion_surface.core_feature_expansion_key;
  surface.summary_output_path = core_feature_expansion_surface.summary_output_path;
  surface.diagnostics_output_path =
      core_feature_expansion_surface.diagnostics_output_path;

  surface.summary_output_extension_compatible =
      Objc3CliReportingOutputContractHasTextSuffix(
          surface.summary_output_path,
          ".json");
  surface.diagnostics_output_suffix_compatible =
      Objc3CliReportingOutputContractHasTextSuffix(
          surface.diagnostics_output_path,
          ".diagnostics.json");
  surface.case_folded_paths_distinct =
      !surface.summary_output_path.empty() &&
      !surface.diagnostics_output_path.empty() &&
      Objc3CliReportingOutputContractToCaseFoldedAscii(
          surface.summary_output_path) !=
          Objc3CliReportingOutputContractToCaseFoldedAscii(
              surface.diagnostics_output_path);
  surface.output_paths_control_char_free =
      !Objc3CliReportingOutputContractContainsControlCharacters(
          surface.summary_output_path) &&
      !Objc3CliReportingOutputContractContainsControlCharacters(
          surface.diagnostics_output_path);

  surface.edge_case_compatibility_consistent =
      surface.summary_output_extension_compatible &&
      surface.diagnostics_output_suffix_compatible &&
      surface.case_folded_paths_distinct &&
      surface.output_paths_control_char_free;
  surface.edge_case_compatibility_key =
      BuildObjc3CliReportingOutputContractEdgeCaseCompatibilityKey(surface);
  surface.edge_case_compatibility_ready =
      surface.core_feature_expansion_ready &&
      surface.edge_case_compatibility_consistent &&
      !surface.edge_case_compatibility_key.empty();
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready &&
      surface.edge_case_compatibility_ready;

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!surface.core_feature_expansion_ready) {
    surface.failure_reason =
        core_feature_expansion_surface.failure_reason.empty()
            ? "cli/reporting output core feature expansion surface is not ready"
            : core_feature_expansion_surface.failure_reason;
  } else if (!surface.summary_output_extension_compatible) {
    surface.failure_reason =
        "summary output path does not satisfy edge-case compatibility";
  } else if (!surface.diagnostics_output_suffix_compatible) {
    surface.failure_reason =
        "diagnostics output path does not satisfy edge-case compatibility";
  } else if (!surface.case_folded_paths_distinct) {
    surface.failure_reason =
        "summary and diagnostics output paths must remain distinct after case folding";
  } else if (!surface.output_paths_control_char_free) {
    surface.failure_reason =
        "summary and diagnostics output paths must not include control characters";
  } else if (surface.edge_case_compatibility_key.empty()) {
    surface.failure_reason = "edge-case compatibility key is empty";
  } else {
    surface.failure_reason =
        "cli/reporting output edge-case compatibility is not ready";
  }

  return surface;
}

inline bool IsObjc3CliReportingOutputContractEdgeCaseCompatibilitySurfaceReady(
    const Objc3CliReportingOutputContractEdgeCaseCompatibilitySurface &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "cli/reporting output edge-case compatibility surface not ready"
               : surface.failure_reason;
  return false;
}

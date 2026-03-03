#pragma once

#include <filesystem>
#include <sstream>
#include <string>

#include "io/objc3_cli_reporting_output_contract_core_feature_surface.h"

struct Objc3CliReportingOutputContractCoreFeatureExpansionSurface {
  bool core_feature_surface_ready = false;
  bool summary_output_path_contract_consistent = false;
  bool diagnostics_output_path_contract_consistent = false;
  bool diagnostics_filename_matches_emit_prefix = false;
  bool core_feature_expansion_ready = false;
  bool core_feature_impl_ready = false;
  std::string scaffold_key;
  std::string core_feature_key;
  std::string summary_output_path;
  std::string diagnostics_output_path;
  std::string core_feature_expansion_key;
  std::string failure_reason;
};

inline std::string NormalizeObjc3CliReportingOutputContractPath(
    const std::filesystem::path &path) {
  if (path.empty()) {
    return "";
  }
  return path.lexically_normal().generic_string();
}

inline std::string NormalizeObjc3CliReportingOutputContractPath(
    const std::string &path_text) {
  if (path_text.empty()) {
    return "";
  }
  return NormalizeObjc3CliReportingOutputContractPath(std::filesystem::path(path_text));
}

inline std::string BuildObjc3CliReportingOutputContractCoreFeatureExpansionKey(
    const Objc3CliReportingOutputContractCoreFeatureExpansionSurface &surface) {
  std::ostringstream key;
  key << "cli-reporting-output-contract-core-feature-expansion:v1:"
      << "core_feature_surface_ready="
      << (surface.core_feature_surface_ready ? "true" : "false")
      << ";summary_output_path_contract_consistent="
      << (surface.summary_output_path_contract_consistent ? "true" : "false")
      << ";diagnostics_output_path_contract_consistent="
      << (surface.diagnostics_output_path_contract_consistent ? "true" : "false")
      << ";diagnostics_filename_matches_emit_prefix="
      << (surface.diagnostics_filename_matches_emit_prefix ? "true" : "false")
      << ";core_feature_expansion_ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";core_feature_impl_ready="
      << (surface.core_feature_impl_ready ? "true" : "false")
      << ";core_feature_key=" << surface.core_feature_key
      << ";summary_output_path=" << surface.summary_output_path
      << ";diagnostics_output_path=" << surface.diagnostics_output_path;
  return key.str();
}

inline Objc3CliReportingOutputContractCoreFeatureExpansionSurface
BuildObjc3CliReportingOutputContractCoreFeatureExpansionSurface(
    const Objc3CliReportingOutputContractCoreFeatureSurface &core_feature_surface,
    const std::string &emit_prefix,
    const std::filesystem::path &summary_output_path,
    const std::filesystem::path &diagnostics_output_path) {
  Objc3CliReportingOutputContractCoreFeatureExpansionSurface surface;
  surface.core_feature_surface_ready = core_feature_surface.core_feature_impl_ready;
  surface.core_feature_impl_ready = core_feature_surface.core_feature_impl_ready;
  surface.scaffold_key = core_feature_surface.scaffold_key;
  surface.core_feature_key = core_feature_surface.core_feature_key;
  surface.summary_output_path =
      NormalizeObjc3CliReportingOutputContractPath(summary_output_path);
  surface.diagnostics_output_path =
      NormalizeObjc3CliReportingOutputContractPath(diagnostics_output_path);

  const std::string expected_summary_output_path =
      NormalizeObjc3CliReportingOutputContractPath(
          core_feature_surface.summary_output_path);
  const std::string expected_diagnostics_output_path =
      NormalizeObjc3CliReportingOutputContractPath(
          core_feature_surface.diagnostics_output_path);

  surface.summary_output_path_contract_consistent =
      !surface.summary_output_path.empty() &&
      surface.summary_output_path == expected_summary_output_path;
  surface.diagnostics_output_path_contract_consistent =
      !surface.diagnostics_output_path.empty() &&
      surface.diagnostics_output_path == expected_diagnostics_output_path;
  surface.diagnostics_filename_matches_emit_prefix =
      !emit_prefix.empty() &&
      diagnostics_output_path.has_filename() &&
      diagnostics_output_path.filename().string() ==
          (emit_prefix + ".diagnostics.json");

  surface.core_feature_expansion_ready =
      surface.core_feature_surface_ready &&
      surface.summary_output_path_contract_consistent &&
      surface.diagnostics_output_path_contract_consistent &&
      surface.diagnostics_filename_matches_emit_prefix &&
      !surface.scaffold_key.empty() &&
      !surface.core_feature_key.empty();
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.core_feature_expansion_ready;
  surface.core_feature_expansion_key =
      BuildObjc3CliReportingOutputContractCoreFeatureExpansionKey(surface);

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!surface.core_feature_surface_ready) {
    surface.failure_reason = core_feature_surface.failure_reason.empty()
                                 ? "cli/reporting output core feature surface is not ready"
                                 : core_feature_surface.failure_reason;
  } else if (!surface.summary_output_path_contract_consistent) {
    surface.failure_reason = "summary output path contract payload is inconsistent";
  } else if (!surface.diagnostics_output_path_contract_consistent) {
    surface.failure_reason = "diagnostics output path contract payload is inconsistent";
  } else if (!surface.diagnostics_filename_matches_emit_prefix) {
    surface.failure_reason =
        "diagnostics output filename does not match emit-prefix contract";
  } else if (surface.scaffold_key.empty()) {
    surface.failure_reason = "cli/reporting output scaffold key is empty";
  } else if (surface.core_feature_key.empty()) {
    surface.failure_reason = "cli/reporting output core feature key is empty";
  } else {
    surface.failure_reason = "cli/reporting output core feature expansion is not ready";
  }

  return surface;
}

inline bool IsObjc3CliReportingOutputContractCoreFeatureExpansionSurfaceReady(
    const Objc3CliReportingOutputContractCoreFeatureExpansionSurface &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "cli/reporting output core feature expansion surface not ready"
               : surface.failure_reason;
  return false;
}

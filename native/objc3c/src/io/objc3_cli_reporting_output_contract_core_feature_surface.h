#pragma once

#include <filesystem>
#include <sstream>
#include <string>

#include "io/objc3_cli_reporting_output_contract_scaffold.h"

struct Objc3CliReportingOutputContractCoreFeatureSurface {
  bool scaffold_ready = false;
  bool summary_mode_pinned = false;
  bool diagnostics_schema_version_pinned = false;
  bool summary_output_path_deterministic = false;
  bool diagnostics_output_path_deterministic = false;
  bool output_paths_distinct = false;
  bool stage_report_output_contract_ready = false;
  bool core_feature_impl_ready = false;
  std::string scaffold_key;
  std::string summary_output_path;
  std::string diagnostics_output_path;
  std::string core_feature_key;
  std::string failure_reason;
};

inline bool Objc3CliReportingOutputContractHasSuffix(
    const std::string &value,
    const std::string &suffix) {
  return value.size() >= suffix.size() &&
         value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline std::string BuildObjc3CliReportingOutputContractCoreFeatureKey(
    const Objc3CliReportingOutputContractCoreFeatureSurface &surface) {
  std::ostringstream key;
  key << "cli-reporting-output-contract-core-feature:v1:"
      << "scaffold_ready=" << (surface.scaffold_ready ? "true" : "false")
      << ";summary_mode_pinned=" << (surface.summary_mode_pinned ? "true" : "false")
      << ";diagnostics_schema_version_pinned="
      << (surface.diagnostics_schema_version_pinned ? "true" : "false")
      << ";summary_output_path_deterministic="
      << (surface.summary_output_path_deterministic ? "true" : "false")
      << ";diagnostics_output_path_deterministic="
      << (surface.diagnostics_output_path_deterministic ? "true" : "false")
      << ";output_paths_distinct=" << (surface.output_paths_distinct ? "true" : "false")
      << ";stage_report_output_contract_ready="
      << (surface.stage_report_output_contract_ready ? "true" : "false")
      << ";core_feature_impl_ready=" << (surface.core_feature_impl_ready ? "true" : "false");
  return key.str();
}

inline Objc3CliReportingOutputContractCoreFeatureSurface
BuildObjc3CliReportingOutputContractCoreFeatureSurface(
    const Objc3CliReportingOutputContractScaffold &scaffold,
    const std::filesystem::path &summary_output_path,
    const std::filesystem::path &diagnostics_output_path) {
  Objc3CliReportingOutputContractCoreFeatureSurface surface;
  surface.scaffold_ready = scaffold.modular_split_ready;
  surface.summary_mode_pinned =
      std::string(kObjc3CliReportingSummaryMode) == "objc3c-frontend-c-api-runner-v1";
  surface.diagnostics_schema_version_pinned =
      std::string(kObjc3CliReportingDiagnosticsSchemaVersion) == "1.0.0";
  surface.summary_output_path = summary_output_path.generic_string();
  surface.diagnostics_output_path = diagnostics_output_path.generic_string();
  surface.summary_output_path_deterministic =
      summary_output_path.has_filename() &&
      summary_output_path.extension() == ".json";
  surface.diagnostics_output_path_deterministic =
      diagnostics_output_path.has_filename() &&
      Objc3CliReportingOutputContractHasSuffix(
          diagnostics_output_path.filename().string(),
          ".diagnostics.json");
  surface.output_paths_distinct =
      !surface.summary_output_path.empty() &&
      !surface.diagnostics_output_path.empty() &&
      summary_output_path.lexically_normal() != diagnostics_output_path.lexically_normal();
  surface.stage_report_output_contract_ready =
      scaffold.stage_report_output_contract_ready;
  surface.scaffold_key = scaffold.scaffold_key;

  surface.core_feature_impl_ready =
      surface.scaffold_ready &&
      surface.summary_mode_pinned &&
      surface.diagnostics_schema_version_pinned &&
      surface.summary_output_path_deterministic &&
      surface.diagnostics_output_path_deterministic &&
      surface.output_paths_distinct &&
      surface.stage_report_output_contract_ready &&
      !surface.scaffold_key.empty();
  surface.core_feature_key = BuildObjc3CliReportingOutputContractCoreFeatureKey(surface);

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!surface.scaffold_ready) {
    surface.failure_reason = scaffold.failure_reason.empty()
                                 ? "cli/reporting output scaffold is not ready"
                                 : scaffold.failure_reason;
  } else if (!surface.summary_mode_pinned) {
    surface.failure_reason = "cli/reporting summary mode contract is not pinned";
  } else if (!surface.diagnostics_schema_version_pinned) {
    surface.failure_reason = "cli/reporting diagnostics schema version contract is not pinned";
  } else if (!surface.summary_output_path_deterministic) {
    surface.failure_reason = "summary output path contract is not deterministic";
  } else if (!surface.diagnostics_output_path_deterministic) {
    surface.failure_reason = "diagnostics output path contract is not deterministic";
  } else if (!surface.output_paths_distinct) {
    surface.failure_reason = "summary and diagnostics output paths must be distinct";
  } else if (!surface.stage_report_output_contract_ready) {
    surface.failure_reason = "stage-report output contract is not ready";
  } else if (surface.scaffold_key.empty()) {
    surface.failure_reason = "cli/reporting output scaffold key is empty";
  } else {
    surface.failure_reason = "cli/reporting output core feature implementation is not ready";
  }

  return surface;
}

inline bool IsObjc3CliReportingOutputContractCoreFeatureSurfaceReady(
    const Objc3CliReportingOutputContractCoreFeatureSurface &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "cli/reporting output core feature implementation surface not ready"
               : surface.failure_reason;
  return false;
}

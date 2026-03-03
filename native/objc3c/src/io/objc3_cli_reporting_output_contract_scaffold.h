#pragma once

#include <filesystem>
#include <sstream>
#include <string>

inline constexpr const char *kObjc3CliReportingDiagnosticsSchemaVersion = "1.0.0";
inline constexpr const char *kObjc3CliReportingSummaryMode = "objc3c-frontend-c-api-runner-v1";

struct Objc3CliReportingOutputContractScaffold {
  bool diagnostics_schema_version_pinned = false;
  bool summary_mode_pinned = false;
  bool out_dir_ready = false;
  bool emit_prefix_ready = false;
  bool diagnostics_output_path_ready = false;
  bool summary_output_path_ready = false;
  bool stage_report_output_contract_ready = false;
  bool modular_split_ready = false;
  std::string scaffold_key;
  std::string failure_reason;
};

inline std::string BuildObjc3CliReportingOutputContractScaffoldKey(
    const Objc3CliReportingOutputContractScaffold &scaffold) {
  std::ostringstream key;
  key << "cli-reporting-output-contract-scaffold:v1:"
      << "diagnostics_schema_version_pinned="
      << (scaffold.diagnostics_schema_version_pinned ? "true" : "false")
      << ";summary_mode_pinned="
      << (scaffold.summary_mode_pinned ? "true" : "false")
      << ";out_dir_ready="
      << (scaffold.out_dir_ready ? "true" : "false")
      << ";emit_prefix_ready="
      << (scaffold.emit_prefix_ready ? "true" : "false")
      << ";diagnostics_output_path_ready="
      << (scaffold.diagnostics_output_path_ready ? "true" : "false")
      << ";summary_output_path_ready="
      << (scaffold.summary_output_path_ready ? "true" : "false")
      << ";stage_report_output_contract_ready="
      << (scaffold.stage_report_output_contract_ready ? "true" : "false")
      << ";modular_split_ready="
      << (scaffold.modular_split_ready ? "true" : "false");
  return key.str();
}

inline Objc3CliReportingOutputContractScaffold BuildObjc3CliReportingOutputContractScaffold(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    const std::filesystem::path &summary_path,
    bool stage_report_output_contract_ready) {
  Objc3CliReportingOutputContractScaffold scaffold;
  scaffold.diagnostics_schema_version_pinned =
      std::string(kObjc3CliReportingDiagnosticsSchemaVersion) == "1.0.0";
  scaffold.summary_mode_pinned =
      std::string(kObjc3CliReportingSummaryMode) == "objc3c-frontend-c-api-runner-v1";
  scaffold.out_dir_ready = !out_dir.empty();
  scaffold.emit_prefix_ready = !emit_prefix.empty();

  const std::filesystem::path diagnostics_path = out_dir / (emit_prefix + ".diagnostics.json");
  scaffold.diagnostics_output_path_ready =
      diagnostics_path.has_filename() &&
      diagnostics_path.extension() == ".json";
  scaffold.summary_output_path_ready =
      summary_path.has_filename() &&
      summary_path.extension() == ".json";
  scaffold.stage_report_output_contract_ready = stage_report_output_contract_ready;
  scaffold.modular_split_ready =
      scaffold.diagnostics_schema_version_pinned &&
      scaffold.summary_mode_pinned &&
      scaffold.out_dir_ready &&
      scaffold.emit_prefix_ready &&
      scaffold.diagnostics_output_path_ready &&
      scaffold.summary_output_path_ready &&
      scaffold.stage_report_output_contract_ready;
  scaffold.scaffold_key = BuildObjc3CliReportingOutputContractScaffoldKey(scaffold);

  if (scaffold.modular_split_ready) {
    return scaffold;
  }

  if (!scaffold.diagnostics_schema_version_pinned) {
    scaffold.failure_reason = "diagnostics schema version contract is not pinned";
  } else if (!scaffold.summary_mode_pinned) {
    scaffold.failure_reason = "summary mode contract is not pinned";
  } else if (!scaffold.out_dir_ready) {
    scaffold.failure_reason = "output directory contract is not ready";
  } else if (!scaffold.emit_prefix_ready) {
    scaffold.failure_reason = "emit-prefix contract is not ready";
  } else if (!scaffold.diagnostics_output_path_ready) {
    scaffold.failure_reason = "diagnostics output path contract is not ready";
  } else if (!scaffold.summary_output_path_ready) {
    scaffold.failure_reason = "summary output path contract is not ready";
  } else if (!scaffold.stage_report_output_contract_ready) {
    scaffold.failure_reason = "stage-report output contract is not ready";
  } else {
    scaffold.failure_reason = "cli/reporting output modular split scaffold not ready";
  }

  return scaffold;
}

inline bool IsObjc3CliReportingOutputContractScaffoldReady(
    const Objc3CliReportingOutputContractScaffold &scaffold,
    std::string &reason) {
  if (scaffold.modular_split_ready) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty() ? "cli/reporting output contract scaffold not ready"
                                           : scaffold.failure_reason;
  return false;
}

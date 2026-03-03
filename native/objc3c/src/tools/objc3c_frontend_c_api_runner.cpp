#include "libobjc3c_frontend/c_api.h"

#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <system_error>

#include "io/objc3_cli_reporting_output_contract_core_feature_expansion_surface.h"
#include "io/objc3_cli_reporting_output_contract_diagnostics_hardening_surface.h"
#include "io/objc3_cli_reporting_output_contract_edge_case_expansion_and_robustness_surface.h"
#include "io/objc3_cli_reporting_output_contract_edge_case_compatibility_surface.h"
#include "io/objc3_cli_reporting_output_contract_core_feature_surface.h"
#include "io/objc3_cli_reporting_output_contract_recovery_determinism_hardening_surface.h"
#include "io/objc3_cli_reporting_output_contract_scaffold.h"

namespace fs = std::filesystem;

namespace {

constexpr std::size_t kMaxMessageSendArgs = 16;

struct RunnerOptions {
  fs::path input_path;
  fs::path out_dir = fs::path("tmp") / "artifacts" / "compilation" / "objc3c-native";
  std::string emit_prefix = "module";
  fs::path clang_path = fs::path("clang");
  fs::path llc_path = fs::path("llc");
  objc3c_frontend_c_ir_object_backend_t ir_object_backend = OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG;
  std::uint8_t compatibility_mode = OBJC3C_FRONTEND_COMPATIBILITY_MODE_CANONICAL;
  bool migration_assist = false;
  std::uint32_t max_message_send_args = 0;
  std::string runtime_dispatch_symbol;
  bool emit_manifest = true;
  bool emit_ir = true;
  bool emit_object = true;
  fs::path summary_out;
};

std::string Usage() {
  return "usage: objc3c-frontend-c-api-runner <input> [--out-dir <dir>] [--emit-prefix <name>] "
         "[--clang <path>] [--llc <path>] [--summary-out <path>] [--objc3-max-message-args <0-" +
         std::to_string(kMaxMessageSendArgs) +
         ">] [--objc3-runtime-dispatch-symbol <symbol>] [--objc3-compat-mode <canonical|legacy>] "
         "[--objc3-migration-assist] [--objc3-ir-object-backend <clang|llvm-direct>] "
         "[--no-emit-manifest] [--no-emit-ir] [--no-emit-object]";
}

bool ParseIrObjectBackend(const std::string &value, objc3c_frontend_c_ir_object_backend_t &backend) {
  if (value == "clang") {
    backend = OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG;
    return true;
  }
  if (value == "llvm-direct") {
    backend = OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT;
    return true;
  }
  return false;
}

bool ParseCompatibilityMode(const std::string &value, std::uint8_t &mode) {
  if (value == "canonical") {
    mode = OBJC3C_FRONTEND_COMPATIBILITY_MODE_CANONICAL;
    return true;
  }
  if (value == "legacy") {
    mode = OBJC3C_FRONTEND_COMPATIBILITY_MODE_LEGACY;
    return true;
  }
  return false;
}

bool ParseOptions(int argc, char **argv, RunnerOptions &options, std::string &error) {
  if (argc < 2) {
    error = Usage();
    return false;
  }

  options = RunnerOptions{};
  options.input_path = fs::path(argv[1]);

  for (int i = 2; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--out-dir" && i + 1 < argc) {
      options.out_dir = fs::path(argv[++i]);
    } else if (arg == "--emit-prefix" && i + 1 < argc) {
      options.emit_prefix = argv[++i];
    } else if (arg == "--clang" && i + 1 < argc) {
      options.clang_path = fs::path(argv[++i]);
    } else if (arg == "--llc" && i + 1 < argc) {
      options.llc_path = fs::path(argv[++i]);
    } else if (arg == "--summary-out" && i + 1 < argc) {
      options.summary_out = fs::path(argv[++i]);
    } else if (arg == "--objc3-max-message-args" && i + 1 < argc) {
      const std::string value = argv[++i];
      errno = 0;
      char *end = nullptr;
      const unsigned long parsed = std::strtoul(value.c_str(), &end, 10);
      if (value.empty() || end == value.c_str() || *end != '\0' || errno == ERANGE || parsed > kMaxMessageSendArgs) {
        error = "invalid --objc3-max-message-args (expected integer 0-" +
                std::to_string(kMaxMessageSendArgs) + "): " + value;
        return false;
      }
      options.max_message_send_args = static_cast<std::uint32_t>(parsed);
    } else if (arg == "--objc3-runtime-dispatch-symbol" && i + 1 < argc) {
      options.runtime_dispatch_symbol = argv[++i];
    } else if (arg == "--objc3-compat-mode" && i + 1 < argc) {
      const std::string mode_text = argv[++i];
      if (!ParseCompatibilityMode(mode_text, options.compatibility_mode)) {
        error = "invalid --objc3-compat-mode (expected canonical|legacy): " + mode_text;
        return false;
      }
    } else if (arg == "--objc3-migration-assist") {
      options.migration_assist = true;
    } else if (arg == "--objc3-ir-object-backend" && i + 1 < argc) {
      const std::string backend = argv[++i];
      if (!ParseIrObjectBackend(backend, options.ir_object_backend)) {
        error = "invalid --objc3-ir-object-backend (expected clang|llvm-direct): " + backend;
        return false;
      }
    } else if (arg == "--no-emit-manifest") {
      options.emit_manifest = false;
    } else if (arg == "--no-emit-ir") {
      options.emit_ir = false;
    } else if (arg == "--no-emit-object") {
      options.emit_object = false;
    } else if (arg == "--help" || arg == "-h") {
      error = Usage();
      return false;
    } else {
      error = "unknown arg: " + arg;
      return false;
    }
  }

  return true;
}

std::string EscapeJsonString(const std::string &value) {
  std::ostringstream out;
  for (unsigned char c : value) {
    switch (c) {
      case '"':
        out << "\\\"";
        break;
      case '\\':
        out << "\\\\";
        break;
      case '\b':
        out << "\\b";
        break;
      case '\f':
        out << "\\f";
        break;
      case '\n':
        out << "\\n";
        break;
      case '\r':
        out << "\\r";
        break;
      case '\t':
        out << "\\t";
        break;
      default:
        if (c < 0x20) {
          std::ostringstream code;
          code << std::hex << std::uppercase << static_cast<int>(c);
          std::string hex = code.str();
          while (hex.size() < 4) {
            hex = "0" + hex;
          }
          out << "\\u" << hex;
        } else {
          out << static_cast<char>(c);
        }
        break;
    }
  }
  return out.str();
}

std::string OptionalPath(const char *value) {
  if (value == nullptr || value[0] == '\0') {
    return "";
  }
  return value;
}

void WriteStageSummaryJson(std::ostringstream &out,
                           const char *name,
                           const objc3c_frontend_c_stage_summary_t &summary,
                           bool trailing_comma) {
  out << "    \"" << name << "\": {\n";
  out << "      \"stage\": " << static_cast<unsigned>(summary.stage) << ",\n";
  out << "      \"attempted\": " << (summary.attempted != 0 ? "true" : "false") << ",\n";
  out << "      \"skipped\": " << (summary.skipped != 0 ? "true" : "false") << ",\n";
  out << "      \"diagnostics_total\": " << summary.diagnostics_total << ",\n";
  out << "      \"diagnostics_notes\": " << summary.diagnostics_notes << ",\n";
  out << "      \"diagnostics_warnings\": " << summary.diagnostics_warnings << ",\n";
  out << "      \"diagnostics_errors\": " << summary.diagnostics_errors << ",\n";
  out << "      \"diagnostics_fatals\": " << summary.diagnostics_fatals << "\n";
  out << "    }";
  if (trailing_comma) {
    out << ",";
  }
  out << "\n";
}

std::string BuildSummaryJson(const RunnerOptions &options,
                             objc3c_frontend_c_status_t status,
                             const objc3c_frontend_c_compile_result_t &result,
                             const std::string &last_error,
                             const Objc3CliReportingOutputContractRecoveryDeterminismHardeningSurface
                                 &output_contract_recovery_determinism_surface) {
  const char *backend_name =
      options.ir_object_backend == OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT ? "llvm-direct" : "clang";
  const char *compatibility_mode_name =
      options.compatibility_mode == OBJC3C_FRONTEND_COMPATIBILITY_MODE_LEGACY ? "legacy" : "canonical";
  std::ostringstream out;
  out << "{\n";
  out << "  \"mode\": \"objc3c-frontend-c-api-runner-v1\",\n";
  out << "  \"input_path\": \"" << EscapeJsonString(options.input_path.generic_string()) << "\",\n";
  out << "  \"out_dir\": \"" << EscapeJsonString(options.out_dir.generic_string()) << "\",\n";
  out << "  \"emit_prefix\": \"" << EscapeJsonString(options.emit_prefix) << "\",\n";
  out << "  \"ir_object_backend\": \"" << backend_name << "\",\n";
  out << "  \"compatibility_mode\": \"" << compatibility_mode_name << "\",\n";
  out << "  \"migration_assist\": " << (options.migration_assist ? "true" : "false") << ",\n";
  out << "  \"status\": " << static_cast<unsigned>(status) << ",\n";
  out << "  \"process_exit_code\": " << result.process_exit_code << ",\n";
  out << "  \"success\": " << (result.success != 0 ? "true" : "false") << ",\n";
  out << "  \"semantic_skipped\": " << (result.semantic_skipped != 0 ? "true" : "false") << ",\n";
  out << "  \"paths\": {\n";
  out << "    \"diagnostics\": \"" << EscapeJsonString(OptionalPath(result.diagnostics_path)) << "\",\n";
  out << "    \"manifest\": \"" << EscapeJsonString(OptionalPath(result.manifest_path)) << "\",\n";
  out << "    \"ir\": \"" << EscapeJsonString(OptionalPath(result.ir_path)) << "\",\n";
  out << "    \"object\": \"" << EscapeJsonString(OptionalPath(result.object_path)) << "\"\n";
  out << "  },\n";
  out << "  \"last_error\": \"" << EscapeJsonString(last_error) << "\",\n";
  out << "  \"stages\": {\n";
  WriteStageSummaryJson(out, "lex", result.lex, true);
  WriteStageSummaryJson(out, "parse", result.parse, true);
  WriteStageSummaryJson(out, "sema", result.sema, true);
  WriteStageSummaryJson(out, "lower", result.lower, true);
  WriteStageSummaryJson(out, "emit", result.emit, false);
  out << "  },\n";
  out << "  \"output_contract\": {\n";
  out << "    \"diagnostics_schema_version\": \""
      << kObjc3CliReportingDiagnosticsSchemaVersion << "\",\n";
  out << "    \"summary_mode\": \"" << kObjc3CliReportingSummaryMode << "\",\n";
  out << "    \"scaffold_key\": \""
      << EscapeJsonString(output_contract_recovery_determinism_surface.scaffold_key) << "\",\n";
  out << "    \"core_feature_key\": \""
      << EscapeJsonString(output_contract_recovery_determinism_surface.core_feature_key) << "\",\n";
  out << "    \"core_feature_expansion_key\": \""
      << EscapeJsonString(output_contract_recovery_determinism_surface.core_feature_expansion_key) << "\",\n";
  out << "    \"edge_case_compatibility_key\": \""
      << EscapeJsonString(output_contract_recovery_determinism_surface.edge_case_compatibility_key) << "\",\n";
  out << "    \"edge_case_robustness_key\": \""
      << EscapeJsonString(output_contract_recovery_determinism_surface.edge_case_robustness_key) << "\",\n";
  out << "    \"diagnostics_hardening_key\": \""
      << EscapeJsonString(output_contract_recovery_determinism_surface.diagnostics_hardening_key) << "\",\n";
  out << "    \"recovery_determinism_key\": \""
      << EscapeJsonString(output_contract_recovery_determinism_surface.recovery_determinism_key) << "\",\n";
  out << "    \"summary_output_path\": \""
      << EscapeJsonString(output_contract_recovery_determinism_surface.summary_output_path) << "\",\n";
  out << "    \"diagnostics_output_path\": \""
      << EscapeJsonString(output_contract_recovery_determinism_surface.diagnostics_output_path) << "\",\n";
  out << "    \"summary_output_path_contract_consistent\": "
      << (output_contract_recovery_determinism_surface
                  .summary_output_path_contract_consistent
              ? "true"
              : "false")
      << ",\n";
  out << "    \"diagnostics_output_path_contract_consistent\": "
      << (output_contract_recovery_determinism_surface
                  .diagnostics_output_path_contract_consistent
              ? "true"
              : "false")
      << ",\n";
  out << "    \"diagnostics_filename_matches_emit_prefix\": "
      << (output_contract_recovery_determinism_surface
                  .diagnostics_filename_matches_emit_prefix
              ? "true"
              : "false")
      << ",\n";
  out << "    \"core_feature_expansion_ready\": "
      << (output_contract_recovery_determinism_surface.core_feature_expansion_ready
              ? "true"
              : "false")
      << ",\n";
  out << "    \"summary_output_extension_compatible\": "
      << (output_contract_recovery_determinism_surface.summary_output_extension_compatible
              ? "true"
              : "false")
      << ",\n";
  out << "    \"diagnostics_output_suffix_compatible\": "
      << (output_contract_recovery_determinism_surface.diagnostics_output_suffix_compatible
              ? "true"
              : "false")
      << ",\n";
  out << "    \"case_folded_paths_distinct\": "
      << (output_contract_recovery_determinism_surface.case_folded_paths_distinct
              ? "true"
              : "false")
      << ",\n";
  out << "    \"output_paths_control_char_free\": "
      << (output_contract_recovery_determinism_surface
                  .output_paths_control_char_free
              ? "true"
              : "false")
      << ",\n";
  out << "    \"edge_case_compatibility_consistent\": "
      << (output_contract_recovery_determinism_surface
                  .edge_case_compatibility_consistent
              ? "true"
              : "false")
      << ",\n";
  out << "    \"edge_case_compatibility_ready\": "
      << (output_contract_recovery_determinism_surface.edge_case_compatibility_ready
              ? "true"
              : "false")
      << ",\n";
  out << "    \"summary_output_parent_present\": "
      << (output_contract_recovery_determinism_surface.summary_output_parent_present
              ? "true"
              : "false")
      << ",\n";
  out << "    \"diagnostics_output_parent_present\": "
      << (output_contract_recovery_determinism_surface.diagnostics_output_parent_present
              ? "true"
              : "false")
      << ",\n";
  out << "    \"output_paths_within_length_budget\": "
      << (output_contract_recovery_determinism_surface.output_paths_within_length_budget
              ? "true"
              : "false")
      << ",\n";
  out << "    \"output_paths_no_trailing_space\": "
      << (output_contract_recovery_determinism_surface.output_paths_no_trailing_space
              ? "true"
              : "false")
      << ",\n";
  out << "    \"edge_case_expansion_consistent\": "
      << (output_contract_recovery_determinism_surface.edge_case_expansion_consistent
              ? "true"
              : "false")
      << ",\n";
  out << "    \"edge_case_robustness_consistent\": "
      << (output_contract_recovery_determinism_surface.edge_case_robustness_consistent
              ? "true"
              : "false")
      << ",\n";
  out << "    \"edge_case_robustness_ready\": "
      << (output_contract_recovery_determinism_surface.edge_case_robustness_ready
              ? "true"
              : "false")
      << ",\n";
  out << "    \"diagnostics_hardening_consistent\": "
      << (output_contract_recovery_determinism_surface.diagnostics_hardening_consistent
              ? "true"
              : "false")
      << ",\n";
  out << "    \"diagnostics_hardening_ready\": "
      << (output_contract_recovery_determinism_surface.diagnostics_hardening_ready
              ? "true"
              : "false")
      << ",\n";
  out << "    \"recovery_determinism_consistent\": "
      << (output_contract_recovery_determinism_surface.recovery_determinism_consistent
              ? "true"
              : "false")
      << ",\n";
  out << "    \"recovery_determinism_ready\": "
      << (output_contract_recovery_determinism_surface.recovery_determinism_ready
              ? "true"
              : "false")
      << ",\n";
  out << "    \"core_feature_impl_ready\": "
      << (output_contract_recovery_determinism_surface.core_feature_impl_ready
              ? "true"
              : "false")
      << "\n";
  out << "  }\n";
  out << "}\n";
  return out.str();
}

int ExitCodeFromStatus(objc3c_frontend_c_status_t status, const objc3c_frontend_c_compile_result_t &result) {
  switch (status) {
    case OBJC3C_FRONTEND_STATUS_OK:
      return 0;
    case OBJC3C_FRONTEND_STATUS_DIAGNOSTICS:
      return 1;
    case OBJC3C_FRONTEND_STATUS_USAGE_ERROR:
      return 2;
    case OBJC3C_FRONTEND_STATUS_EMIT_ERROR:
      return result.process_exit_code != 0 ? result.process_exit_code : 3;
    case OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR:
    default:
      return result.process_exit_code != 0 ? result.process_exit_code : 2;
  }
}

std::string ReadLastError(const objc3c_frontend_c_context_t *context) {
  const size_t required = objc3c_frontend_c_copy_last_error(context, nullptr, 0);
  if (required == 0) {
    return "";
  }
  std::string message(required, '\0');
  const size_t written = objc3c_frontend_c_copy_last_error(context, message.data(), message.size());
  if (written == 0) {
    return "";
  }
  if (!message.empty() && message.back() == '\0') {
    message.pop_back();
  }
  return message;
}

bool WriteSummary(const fs::path &summary_path, const std::string &summary_json, std::string &error) {
  std::error_code mkdir_error;
  if (!summary_path.parent_path().empty()) {
    fs::create_directories(summary_path.parent_path(), mkdir_error);
  }
  if (mkdir_error) {
    error = "failed to create summary directory '" + summary_path.parent_path().string() + "': " + mkdir_error.message();
    return false;
  }

  std::ofstream out(summary_path, std::ios::binary);
  if (!out.is_open()) {
    error = "failed to open summary file '" + summary_path.string() + "' for writing";
    return false;
  }
  out << summary_json;
  if (!out.good()) {
    error = "failed while writing summary file '" + summary_path.string() + "'";
    return false;
  }
  return true;
}

}  // namespace

int main(int argc, char **argv) {
  RunnerOptions options;
  std::string parse_error;
  if (!ParseOptions(argc, argv, options, parse_error)) {
    std::cerr << parse_error << "\n";
    return 2;
  }

  objc3c_frontend_c_context_t *context = objc3c_frontend_c_context_create();
  if (context == nullptr) {
    std::cerr << "failed to allocate frontend context\n";
    return 2;
  }

  std::string input_path_text = options.input_path.string();
  std::string out_dir_text = options.out_dir.string();
  std::string clang_path_text = options.clang_path.string();
  std::string llc_path_text = options.llc_path.string();
  const char *runtime_symbol = options.runtime_dispatch_symbol.empty() ? nullptr : options.runtime_dispatch_symbol.c_str();

  objc3c_frontend_c_compile_options_t compile_options = {};
  compile_options.input_path = input_path_text.c_str();
  compile_options.out_dir = out_dir_text.c_str();
  compile_options.emit_prefix = options.emit_prefix.c_str();
  compile_options.clang_path =
      options.emit_object && options.ir_object_backend == OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG
          ? clang_path_text.c_str()
          : nullptr;
  compile_options.llc_path =
      options.emit_object && options.ir_object_backend == OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT
          ? llc_path_text.c_str()
          : nullptr;
  compile_options.runtime_dispatch_symbol = runtime_symbol;
  compile_options.max_message_send_args = options.max_message_send_args;
  compile_options.compatibility_mode = options.compatibility_mode;
  compile_options.migration_assist = options.migration_assist ? 1u : 0u;
  compile_options.emit_manifest = options.emit_manifest ? 1u : 0u;
  compile_options.emit_ir = options.emit_ir ? 1u : 0u;
  compile_options.emit_object = options.emit_object ? 1u : 0u;
  compile_options.ir_object_backend = options.ir_object_backend;

  objc3c_frontend_c_compile_result_t result = {};
  const objc3c_frontend_c_status_t status = objc3c_frontend_c_compile_file(context, &compile_options, &result);
  const std::string last_error = ReadLastError(context);
  const int exit_code = ExitCodeFromStatus(status, result);

  const fs::path summary_path =
      options.summary_out.empty() ? (options.out_dir / (options.emit_prefix + ".c_api_summary.json")) : options.summary_out;
  const bool stage_report_output_contract_ready =
      result.lex.stage == OBJC3C_FRONTEND_STAGE_LEX &&
      result.parse.stage == OBJC3C_FRONTEND_STAGE_PARSE &&
      result.sema.stage == OBJC3C_FRONTEND_STAGE_SEMA &&
      result.lower.stage == OBJC3C_FRONTEND_STAGE_LOWER &&
      result.emit.stage == OBJC3C_FRONTEND_STAGE_EMIT;
  const Objc3CliReportingOutputContractScaffold cli_reporting_output_contract_scaffold =
      BuildObjc3CliReportingOutputContractScaffold(
          options.out_dir,
          options.emit_prefix,
          summary_path,
          stage_report_output_contract_ready);
  std::string output_contract_scaffold_reason;
  if (!IsObjc3CliReportingOutputContractScaffoldReady(
          cli_reporting_output_contract_scaffold,
          output_contract_scaffold_reason)) {
    std::cerr << "cli/reporting output scaffold fail-closed: "
              << output_contract_scaffold_reason << "\n";
    objc3c_frontend_c_context_destroy(context);
    return 2;
  }

  const std::string diagnostics_path_text = OptionalPath(result.diagnostics_path);
  const fs::path diagnostics_output_path = diagnostics_path_text.empty()
                                               ? (options.out_dir / (options.emit_prefix + ".diagnostics.json"))
                                               : fs::path(diagnostics_path_text);
  const Objc3CliReportingOutputContractCoreFeatureSurface cli_reporting_output_contract_core_feature_surface =
      BuildObjc3CliReportingOutputContractCoreFeatureSurface(
          cli_reporting_output_contract_scaffold,
          summary_path,
          diagnostics_output_path);
  std::string output_contract_core_feature_reason;
  if (!IsObjc3CliReportingOutputContractCoreFeatureSurfaceReady(
          cli_reporting_output_contract_core_feature_surface,
          output_contract_core_feature_reason)) {
    std::cerr << "cli/reporting output core feature fail-closed: "
              << output_contract_core_feature_reason << "\n";
    objc3c_frontend_c_context_destroy(context);
    return 2;
  }

  const Objc3CliReportingOutputContractCoreFeatureExpansionSurface
      cli_reporting_output_contract_core_feature_expansion_surface =
          BuildObjc3CliReportingOutputContractCoreFeatureExpansionSurface(
              cli_reporting_output_contract_core_feature_surface,
              options.emit_prefix,
              summary_path,
              diagnostics_output_path);
  std::string output_contract_core_feature_expansion_reason;
  if (!IsObjc3CliReportingOutputContractCoreFeatureExpansionSurfaceReady(
          cli_reporting_output_contract_core_feature_expansion_surface,
          output_contract_core_feature_expansion_reason)) {
    std::cerr << "cli/reporting output core feature expansion fail-closed: "
              << output_contract_core_feature_expansion_reason << "\n";
    objc3c_frontend_c_context_destroy(context);
    return 2;
  }

  const Objc3CliReportingOutputContractEdgeCaseCompatibilitySurface
      cli_reporting_output_contract_edge_case_compatibility_surface =
          BuildObjc3CliReportingOutputContractEdgeCaseCompatibilitySurface(
              cli_reporting_output_contract_core_feature_expansion_surface);
  std::string output_contract_edge_case_compatibility_reason;
  if (!IsObjc3CliReportingOutputContractEdgeCaseCompatibilitySurfaceReady(
          cli_reporting_output_contract_edge_case_compatibility_surface,
          output_contract_edge_case_compatibility_reason)) {
    std::cerr << "cli/reporting output edge-case compatibility fail-closed: "
              << output_contract_edge_case_compatibility_reason << "\n";
    objc3c_frontend_c_context_destroy(context);
    return 2;
  }

  const Objc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface
      cli_reporting_output_contract_edge_case_robustness_surface =
          BuildObjc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface(
              cli_reporting_output_contract_edge_case_compatibility_surface);
  std::string output_contract_edge_case_robustness_reason;
  if (!IsObjc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurfaceReady(
          cli_reporting_output_contract_edge_case_robustness_surface,
          output_contract_edge_case_robustness_reason)) {
    std::cerr << "cli/reporting output edge-case expansion and robustness fail-closed: "
              << output_contract_edge_case_robustness_reason << "\n";
    objc3c_frontend_c_context_destroy(context);
    return 2;
  }

  const Objc3CliReportingOutputContractDiagnosticsHardeningSurface
      cli_reporting_output_contract_diagnostics_hardening_surface =
          BuildObjc3CliReportingOutputContractDiagnosticsHardeningSurface(
              cli_reporting_output_contract_edge_case_robustness_surface);
  std::string output_contract_diagnostics_hardening_reason;
  if (!IsObjc3CliReportingOutputContractDiagnosticsHardeningSurfaceReady(
          cli_reporting_output_contract_diagnostics_hardening_surface,
          output_contract_diagnostics_hardening_reason)) {
    std::cerr << "cli/reporting output diagnostics hardening fail-closed: "
              << output_contract_diagnostics_hardening_reason << "\n";
    objc3c_frontend_c_context_destroy(context);
    return 2;
  }

  const Objc3CliReportingOutputContractRecoveryDeterminismHardeningSurface
      cli_reporting_output_contract_recovery_determinism_surface =
          BuildObjc3CliReportingOutputContractRecoveryDeterminismHardeningSurface(
              cli_reporting_output_contract_diagnostics_hardening_surface);
  std::string output_contract_recovery_determinism_reason;
  if (!IsObjc3CliReportingOutputContractRecoveryDeterminismHardeningSurfaceReady(
          cli_reporting_output_contract_recovery_determinism_surface,
          output_contract_recovery_determinism_reason)) {
    std::cerr
        << "cli/reporting output recovery and determinism hardening fail-closed: "
        << output_contract_recovery_determinism_reason << "\n";
    objc3c_frontend_c_context_destroy(context);
    return 2;
  }

  const std::string summary_json = BuildSummaryJson(
      options,
      status,
      result,
      last_error,
      cli_reporting_output_contract_recovery_determinism_surface);
  std::string summary_error;
  if (!WriteSummary(summary_path, summary_json, summary_error)) {
    std::cerr << summary_error << "\n";
    objc3c_frontend_c_context_destroy(context);
    return 2;
  }

  std::cout << "wrote summary: " << summary_path.generic_string() << "\n";
  if (!last_error.empty()) {
    std::cerr << last_error << "\n";
  }

  objc3c_frontend_c_context_destroy(context);
  return exit_code;
}

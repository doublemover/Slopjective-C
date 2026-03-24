#include "libobjc3c_frontend/api.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <new>
#include <limits>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

#include "ast/objc3_ast.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process.h"
#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h"
#include "io/objc3_toolchain_runtime_ga_operations_scaffold.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"

struct objc3c_frontend_context {
  std::string last_error;
  std::string diagnostics_path;
  // M266-E001 control-flow execution gate anchor: the C frontend bridge must
  // keep surfacing manifest/IR/object publication paths unchanged so lane-E
  // can validate the same native artifact triplet without a synthetic proof
  // channel.
  // M266-E002 runnable control-flow matrix anchor: the closeout matrix relies
  // on the same surfaced native artifact triplet instead of a second
  // publication mechanism.
  std::string manifest_path;
  std::string runtime_metadata_binary_path;
  std::string ir_path;
  std::string object_path;
};

// M265-E001 type-surface executable gate anchor: the C frontend bridge must
// keep surfacing manifest/IR/object publication paths so the lane-E gate can
// validate the currently runnable Part 3 slice without inventing a parallel
// proof channel.
// M265-E002 runnable-type-surface closeout anchor: the milestone closeout
// matrix relies on the same publication paths instead of creating a separate
// synthetic reporting surface.
// M267-E001 error-model conformance gate anchor: lane-E consumes those same
// publication paths and the same runtime-import surface without inventing a
// second frontend proof channel.
// M268-E001 async executable conformance gate anchor: lane-E consumes those
// same publication paths for the runnable Part 7 slice instead of inventing a
// second frontend async proof channel.
// M268-E002 runnable async closeout matrix anchor: milestone closeout keeps
// consuming those same publication paths instead of inventing a second
// frontend matrix-only reporting path.
// M269-E001 task/executor conformance gate anchor: lane-E keeps consuming the
// same surfaced publication paths while using the hardened D003 runtime proof
// as the truthful runnable task/executor capability anchor.
// M269-E002 runnable task/executor closeout matrix anchor: milestone closeout
// keeps consuming those same surfaced publication paths instead of inventing a
// second frontend matrix-only reporting path.
// M270-E001 strict concurrency conformance gate anchor: lane-E keeps consuming
// the same surfaced publication paths while using the D002 mailbox runtime
// proof plus the D003 cross-module preservation artifacts as the truthful
// runnable actor/isolation capability anchor.
// M270-E002 runnable actor/isolation closeout matrix anchor: milestone
// closeout keeps consuming those same surfaced publication paths instead of
// inventing a second frontend matrix-only reporting path.
// M271-E001 strict system conformance gate anchor: lane-E keeps consuming the
// same surfaced publication paths while using the D002 live cleanup/runtime proof
// as the truthful runnable Part 8 capability anchor. The D002 live cleanup/runtime
// proof remains the canonical linked helperSurface evidence.
// M271-E002 runnable system-extension closeout matrix anchor: milestone
// closeout keeps consuming those same surfaced publication paths instead of
// inventing a second frontend matrix-only reporting path for Part 8.
// M272-E001 performance/dynamism conformance gate anchor: lane-E keeps
// consuming those same surfaced publication paths while using the D002 live
// fast-path/runtime proof as the truthful runnable Part 9 capability anchor.
// The D002 live fast-path/runtime proof remains the canonical executable
// evidence boundary for the current dispatch-control slice.
// M272-E002 runnable dispatch-control matrix closeout anchor: Part 9 closeout
// keeps consuming those same surfaced publication paths while reusing the D002
// runtime proof for direct/final/sealed matrix rows instead of inventing a
// second frontend-only summary surface.
// M273-E001 metaprogramming conformance gate anchor: lane-E keeps consuming
// those same surfaced publication paths while using the D002 live macro
// host-process/cache proof as the truthful runnable Part 10 capability anchor.
// M273-E002 runnable metaprogramming closeout matrix anchor: milestone
// closeout keeps consuming those same surfaced publication paths instead of
// inventing a second frontend-only Part 10 matrix reporting path.

static void objc3c_frontend_set_error(objc3c_frontend_context_t *context, const char *message) {
  if (context == nullptr) {
    return;
  }
  context->last_error = message == nullptr ? "" : message;
}

static bool IsNullOrEmpty(const char *text) {
  return text == nullptr || text[0] == '\0';
}

static std::filesystem::path OptionalFilesystemPath(const char *text) {
  if (IsNullOrEmpty(text)) {
    return std::filesystem::path();
  }
  return std::filesystem::path(text);
}

static std::string ToLowerCopy(std::string value) {
  for (char &ch : value) {
    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
  return value;
}

static const char *DefaultEmitPrefix = "module";
static const char *DefaultMemoryInputPath = "<memory>";

static uint8_t NormalizeLanguageVersion(uint8_t requested_language_version) {
  if (requested_language_version == 0u) {
    return static_cast<uint8_t>(OBJC3C_FRONTEND_LANGUAGE_VERSION_DEFAULT);
  }
  return requested_language_version;
}

static uint8_t NormalizeCompatibilityMode(uint8_t requested_compatibility_mode) {
  if (requested_compatibility_mode == 0u) {
    return static_cast<uint8_t>(OBJC3C_FRONTEND_COMPATIBILITY_MODE_DEFAULT);
  }
  return requested_compatibility_mode;
}

static bool ValidateSupportedLanguageVersion(uint8_t requested_language_version, std::string &error) {
  const uint8_t normalized_language_version = NormalizeLanguageVersion(requested_language_version);
  if (normalized_language_version == static_cast<uint8_t>(OBJC3C_FRONTEND_LANGUAGE_VERSION_OBJECTIVE_C_3)) {
    return true;
  }

  error = "unsupported compile_options.language_version: " + std::to_string(normalized_language_version) +
          " (only Objective-C version 3 is supported).";
  return false;
}

static bool ValidateSupportedCompatibilityMode(uint8_t requested_compatibility_mode, std::string &error) {
  const uint8_t normalized_compatibility_mode = NormalizeCompatibilityMode(requested_compatibility_mode);
  if (normalized_compatibility_mode == static_cast<uint8_t>(OBJC3C_FRONTEND_COMPATIBILITY_MODE_CANONICAL) ||
      normalized_compatibility_mode == static_cast<uint8_t>(OBJC3C_FRONTEND_COMPATIBILITY_MODE_LEGACY)) {
    return true;
  }

  error = "unsupported compile_options.compatibility_mode: " + std::to_string(normalized_compatibility_mode) +
          " (expected canonical=0 or legacy=1).";
  return false;
}

static std::filesystem::path ResolveInputPath(const objc3c_frontend_compile_options_t &options) {
  if (!IsNullOrEmpty(options.input_path)) {
    return std::filesystem::path(options.input_path);
  }
  return std::filesystem::path(DefaultMemoryInputPath);
}

static std::filesystem::path ResolveOutputDir(const objc3c_frontend_compile_options_t &options) {
  if (IsNullOrEmpty(options.out_dir)) {
    return std::filesystem::path();
  }
  return std::filesystem::path(options.out_dir);
}

static std::string ResolveEmitPrefix(const objc3c_frontend_compile_options_t &options,
                                     const std::filesystem::path &input_path) {
  if (!IsNullOrEmpty(options.emit_prefix)) {
    return std::string(options.emit_prefix);
  }
  const std::string stem = input_path.stem().string();
  if (!stem.empty()) {
    return stem;
  }
  return DefaultEmitPrefix;
}

static std::string EscapeJsonString(const std::string &value) {
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

struct ParsedFrontendDiagnostic {
  std::string severity = "unknown";
  unsigned line = std::numeric_limits<unsigned>::max();
  unsigned column = std::numeric_limits<unsigned>::max();
  std::string code;
  std::string message;
  std::string raw;
};

static bool IsNativeDiagCode(const std::string &candidate) {
  if (candidate.size() != 6) {
    return false;
  }
  if (candidate[0] != 'O' || candidate[1] != '3') {
    return false;
  }
  if (std::isupper(static_cast<unsigned char>(candidate[2])) == 0) {
    return false;
  }
  return std::isdigit(static_cast<unsigned char>(candidate[3])) != 0 &&
         std::isdigit(static_cast<unsigned char>(candidate[4])) != 0 &&
         std::isdigit(static_cast<unsigned char>(candidate[5])) != 0;
}

static ParsedFrontendDiagnostic ParseFrontendDiagnostic(const std::string &diag) {
  ParsedFrontendDiagnostic parsed;
  parsed.raw = diag;
  parsed.message = diag;

  const std::size_t severity_end = diag.find(':');
  if (severity_end == std::string::npos) {
    return parsed;
  }
  parsed.severity = ToLowerCopy(diag.substr(0, severity_end));

  const std::size_t line_end = diag.find(':', severity_end + 1);
  const std::size_t column_end = line_end == std::string::npos ? std::string::npos : diag.find(':', line_end + 1);
  if (line_end == std::string::npos || column_end == std::string::npos) {
    return parsed;
  }

  try {
    parsed.line = static_cast<unsigned>(std::stoul(diag.substr(severity_end + 1, line_end - (severity_end + 1))));
    parsed.column = static_cast<unsigned>(std::stoul(diag.substr(line_end + 1, column_end - (line_end + 1))));
  } catch (const std::exception &) {
    parsed.line = std::numeric_limits<unsigned>::max();
    parsed.column = std::numeric_limits<unsigned>::max();
  }

  std::size_t message_begin = column_end + 1;
  while (message_begin < diag.size() && std::isspace(static_cast<unsigned char>(diag[message_begin])) != 0) {
    ++message_begin;
  }

  const std::size_t code_begin = diag.rfind(" [");
  if (code_begin != std::string::npos && code_begin > message_begin && !diag.empty() && diag.back() == ']') {
    const std::string candidate_code = diag.substr(code_begin + 2, diag.size() - (code_begin + 3));
    if (IsNativeDiagCode(candidate_code)) {
      parsed.message = diag.substr(message_begin, code_begin - message_begin);
      parsed.code = candidate_code;
      return parsed;
    }
  }

  parsed.message = diag.substr(message_begin);
  return parsed;
}

static bool WriteTextFile(const std::filesystem::path &path, const std::string &contents, std::string &error) {
  std::error_code mkdir_error;
  if (!path.parent_path().empty()) {
    std::filesystem::create_directories(path.parent_path(), mkdir_error);
  }
  if (mkdir_error) {
    error = "failed to create output directory '" + path.parent_path().string() + "': " + mkdir_error.message();
    return false;
  }

  std::ofstream out(path, std::ios::binary);
  if (!out.is_open()) {
    error = "failed to open output file '" + path.string() + "' for writing";
    return false;
  }
  out << contents;
  if (!out.good()) {
    error = "failed while writing output file '" + path.string() + "'";
    return false;
  }
  return true;
}

static bool WriteBinaryFile(const std::filesystem::path &path,
                            const std::string &contents,
                            std::string &error) {
  std::error_code mkdir_error;
  if (!path.parent_path().empty()) {
    std::filesystem::create_directories(path.parent_path(), mkdir_error);
  }
  if (mkdir_error) {
    error = "failed to create output directory '" + path.parent_path().string() + "': " + mkdir_error.message();
    return false;
  }

  std::ofstream out(path, std::ios::binary);
  if (!out.is_open()) {
    error = "failed to open output file '" + path.string() + "' for writing";
    return false;
  }
  out.write(contents.data(), static_cast<std::streamsize>(contents.size()));
  if (!out.good()) {
    error = "failed while writing output file '" + path.string() + "'";
    return false;
  }
  return true;
}

static bool ReadTextFile(const std::filesystem::path &path, std::string &contents, std::string &error) {
  std::ifstream input(path, std::ios::binary);
  if (!input.is_open()) {
    error = "failed to open input source '" + path.string() + "'";
    return false;
  }
  std::ostringstream buffer;
  buffer << input.rdbuf();
  if (!input.good() && !input.eof()) {
    error = "failed while reading input source '" + path.string() + "'";
    return false;
  }
  contents = buffer.str();
  return true;
}

static std::string BuildDiagnosticsJson(const std::vector<std::string> &diagnostics) {
  std::ostringstream out;
  out << "{\n";
  out << "  \"schema_version\": \"1.0.0\",\n";
  out << "  \"diagnostics\": [\n";
  for (std::size_t i = 0; i < diagnostics.size(); ++i) {
    const ParsedFrontendDiagnostic parsed = ParseFrontendDiagnostic(diagnostics[i]);
    const unsigned line = parsed.line == std::numeric_limits<unsigned>::max() ? 0U : parsed.line;
    const unsigned column = parsed.column == std::numeric_limits<unsigned>::max() ? 0U : parsed.column;
    out << "    {\"severity\":\"" << EscapeJsonString(parsed.severity) << "\",\"line\":" << line
        << ",\"column\":" << column << ",\"code\":\"" << EscapeJsonString(parsed.code) << "\",\"message\":\""
        << EscapeJsonString(parsed.message) << "\",\"raw\":\"" << EscapeJsonString(parsed.raw) << "\"}";
    if (i + 1 != diagnostics.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "  ]\n";
  out << "}\n";
  return out.str();
}

struct StageDiagnosticCounts {
  uint32_t notes = 0;
  uint32_t warnings = 0;
  uint32_t errors = 0;
  uint32_t fatals = 0;
};

static StageDiagnosticCounts CountDiagnosticsBySeverity(const std::vector<std::string> &diagnostics) {
  StageDiagnosticCounts counts;
  for (const std::string &diag : diagnostics) {
    const std::size_t colon = diag.find(':');
    const std::string prefix = ToLowerCopy(diag.substr(0, colon));
    if (prefix == "note") {
      ++counts.notes;
    } else if (prefix == "warning") {
      ++counts.warnings;
    } else if (prefix == "fatal") {
      ++counts.fatals;
    } else {
      ++counts.errors;
    }
  }
  return counts;
}

static objc3c_frontend_stage_summary_t BuildStageSummary(objc3c_frontend_stage_id_t stage_id,
                                                         bool attempted,
                                                         bool skipped,
                                                         const std::vector<std::string> &diagnostics) {
  const StageDiagnosticCounts counts = CountDiagnosticsBySeverity(diagnostics);
  objc3c_frontend_stage_summary_t summary = {};
  summary.stage = stage_id;
  summary.attempted = attempted ? 1u : 0u;
  summary.skipped = skipped ? 1u : 0u;
  summary.diagnostics_total = static_cast<uint32_t>(diagnostics.size());
  summary.diagnostics_notes = counts.notes;
  summary.diagnostics_warnings = counts.warnings;
  summary.diagnostics_errors = counts.errors;
  summary.diagnostics_fatals = counts.fatals;
  return summary;
}

static void ClearCompileResultPaths(objc3c_frontend_context_t *context) {
  if (context == nullptr) {
    return;
  }
  context->diagnostics_path.clear();
  context->manifest_path.clear();
  context->runtime_metadata_binary_path.clear();
  context->ir_path.clear();
  context->object_path.clear();
}

static const char *OptionalCString(const std::string &text) {
  if (text.empty()) {
    return nullptr;
  }
  return text.c_str();
}

static void PopulateResultPaths(objc3c_frontend_context_t *context, objc3c_frontend_compile_result_t *result) {
  if (context == nullptr || result == nullptr) {
    return;
  }
  result->diagnostics_path = OptionalCString(context->diagnostics_path);
  result->manifest_path = OptionalCString(context->manifest_path);
  result->ir_path = OptionalCString(context->ir_path);
  result->object_path = OptionalCString(context->object_path);
}

static objc3c_frontend_status_t SetUsageError(objc3c_frontend_context_t *context,
                                              objc3c_frontend_compile_result_t *result,
                                              const std::string &message) {
  *result = {};
  result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
  result->process_exit_code = 2;
  result->success = 0;
  objc3c_frontend_set_error(context, message.c_str());
  return result->status;
}

static Objc3FrontendOptions BuildFrontendOptions(const objc3c_frontend_compile_options_t &options) {
  Objc3FrontendOptions frontend_options;
  frontend_options.language_version = NormalizeLanguageVersion(options.language_version);
  frontend_options.compatibility_mode =
      NormalizeCompatibilityMode(options.compatibility_mode) ==
              static_cast<uint8_t>(OBJC3C_FRONTEND_COMPATIBILITY_MODE_LEGACY)
          ? Objc3FrontendCompatibilityMode::kLegacy
          : Objc3FrontendCompatibilityMode::kCanonical;
  frontend_options.migration_assist = options.migration_assist != 0;
  frontend_options.emit_manifest = options.emit_manifest != 0;
  frontend_options.emit_ir = options.emit_ir != 0;
  frontend_options.emit_object = options.emit_object != 0;
  if (options.translation_unit_registration_order_ordinal > 0) {
    frontend_options.bootstrap_registration_order_ordinal =
        options.translation_unit_registration_order_ordinal;
  }
  if (options.max_message_send_args > 0) {
    frontend_options.lowering.max_message_send_args = options.max_message_send_args;
  }
  if (!IsNullOrEmpty(options.runtime_dispatch_symbol)) {
    frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;
  }
  return frontend_options;
}

static objc3c_frontend_status_t CompileObjc3SourceImpl(objc3c_frontend_context_t *context,
                                                       const std::filesystem::path &input_path,
                                                       const std::string &source_text,
                                                       const objc3c_frontend_compile_options_t *options,
                                                       objc3c_frontend_compile_result_t *result) {
  *result = {};
  ClearCompileResultPaths(context);

  Objc3FrontendOptions frontend_options = BuildFrontendOptions(*options);
  Objc3LoweringContract normalized_lowering;
  std::string lowering_error;
  if (!TryNormalizeObjc3LoweringContract(frontend_options.lowering, normalized_lowering, lowering_error)) {
    result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
    result->process_exit_code = 2;
    result->success = 0;
    objc3c_frontend_set_error(context, lowering_error.c_str());
    return result->status;
  }
  frontend_options.lowering = normalized_lowering;

  Objc3FrontendCompileProduct product = CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options);

  const bool sema_attempted =
      product.pipeline_result.stage_diagnostics.lexer.empty() && product.pipeline_result.stage_diagnostics.parser.empty();
  const bool lower_attempted = sema_attempted && product.pipeline_result.stage_diagnostics.semantic.empty();
  std::vector<std::string> emit_diagnostics = product.artifact_bundle.post_pipeline_diagnostics;

  const std::filesystem::path out_dir = ResolveOutputDir(*options);
  const bool has_out_dir = !out_dir.empty();
  const std::string emit_prefix = ResolveEmitPrefix(*options, input_path);

  if (has_out_dir) {
    const std::filesystem::path diagnostics_out = out_dir / (emit_prefix + ".diagnostics.json");
    std::string io_error;
    if (!WriteTextFile(diagnostics_out, BuildDiagnosticsJson(product.artifact_bundle.diagnostics), io_error)) {
      result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
      objc3c_frontend_set_error(context, io_error.c_str());
      return result->status;
    }
    context->diagnostics_path = diagnostics_out.generic_string();
  }

  if (!product.artifact_bundle.diagnostics.empty()) {
    result->status = OBJC3C_FRONTEND_STATUS_DIAGNOSTICS;
    result->process_exit_code = 1;
    result->success = 0;
  } else {
    result->status = OBJC3C_FRONTEND_STATUS_OK;
    result->process_exit_code = 0;
    result->success = 1;
  }

  if (options->emit_manifest != 0 && has_out_dir && !product.artifact_bundle.manifest_json.empty()) {
    const std::filesystem::path manifest_out = out_dir / (emit_prefix + ".manifest.json");
    std::string io_error;
    if (!WriteTextFile(manifest_out, product.artifact_bundle.manifest_json, io_error)) {
      result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(context, io_error.c_str());
    } else {
      context->manifest_path = manifest_out.generic_string();
    }
  }

  if (has_out_dir && !product.artifact_bundle.runtime_metadata_binary.empty()) {
    const std::filesystem::path runtime_metadata_out =
        BuildRuntimeMetadataBinaryArtifactPath(out_dir, emit_prefix);
    std::string io_error;
    if (!WriteBinaryFile(runtime_metadata_out,
                         product.artifact_bundle.runtime_metadata_binary,
                         io_error)) {
      result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(context, io_error.c_str());
    } else {
      context->runtime_metadata_binary_path =
          runtime_metadata_out.generic_string();
    }
  }

  if (result->status == OBJC3C_FRONTEND_STATUS_OK && has_out_dir &&
      !product.artifact_bundle.part6_result_bridge_artifact_replay_json.empty()) {

    const std::filesystem::path replay_out =
        BuildPart6ResultBridgeArtifactReplayPath(out_dir, emit_prefix);
    std::string io_error;
    if (!WriteTextFile(
            replay_out,
            product.artifact_bundle.part6_result_bridge_artifact_replay_json,
            io_error)) {
      result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(context, io_error.c_str());
    }
  }

  if (result->status == OBJC3C_FRONTEND_STATUS_OK && has_out_dir) {
    const bool has_runtime_import_artifact =
        !product.artifact_bundle.runtime_aware_import_module_artifact_json
             .empty();
    if (has_runtime_import_artifact &&
        !IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
            product.artifact_bundle
                .runtime_aware_import_module_frontend_closure_summary)) {
      result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(
          context,
          "runtime-aware import/module frontend closure not ready");
    } else {

      const std::filesystem::path runtime_import_surface_out =
          BuildRuntimeAwareImportModuleArtifactPath(out_dir, emit_prefix);
      std::string io_error;
      if (has_runtime_import_artifact &&
          !WriteTextFile(runtime_import_surface_out,
                         product.artifact_bundle
                             .runtime_aware_import_module_artifact_json,
                         io_error)) {
        result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
        result->process_exit_code = 2;
        result->success = 0;
        objc3c_frontend_set_error(context, io_error.c_str());
      }
    }
  }

  if (result->status == OBJC3C_FRONTEND_STATUS_OK && has_out_dir) {
    if (!IsReadyObjc3VersionedConformanceReportLoweringSummary(
            product.artifact_bundle
                .versioned_conformance_report_lowering_summary)) {
      result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(
          context,
          "versioned conformance-report lowering summary not ready");
    } else if (product.artifact_bundle
                   .versioned_conformance_report_artifact_json.empty()) {
      result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(
          context,
          "versioned conformance-report artifact payload missing");
    } else {
      const std::filesystem::path conformance_report_out =
          BuildVersionedConformanceReportArtifactPath(out_dir, emit_prefix);
      std::string io_error;
      if (!WriteTextFile(
              conformance_report_out,
              product.artifact_bundle
                  .versioned_conformance_report_artifact_json,
              io_error)) {
        result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
        result->process_exit_code = 2;
        result->success = 0;
        objc3c_frontend_set_error(context, io_error.c_str());
      } else {
        // M264-E001 versioning/conformance truth-gate anchor: the frontend C
        // API path must continue to publish the same truthful lowered report
        // plus D001 publication sidecar that the native CLI consumes and the
        // D002 validation path checks.
        // M264-E002 release/runtime-claim-matrix anchor: the milestone closeout
        // matrix compares this frontend publication surface against the native
        // CLI surface and must keep them equivalent for the runnable core
        // profile.
        std::string conformance_publication_artifact_json;
        std::string conformance_publication_error;
        if (!TryBuildObjc3ConformanceReportPublicationArtifact(
                {.contract_id =
                     "objc3c-driver-conformance-report-publication/m264-d001-v1",
                 .schema_id = "objc3c-driver-conformance-publication-v1",
                 .selected_profile = "core",
                 .selected_profile_supported = true,
                 .supported_profile_ids = {"core"},
                 .rejected_profile_ids = {"strict", "strict-concurrency",
                                          "strict-system"},
                 .effective_compatibility_mode =
                     (frontend_options.compatibility_mode ==
                              Objc3FrontendCompatibilityMode::kLegacy
                          ? "legacy"
                          : "canonical"),
                 .migration_assist_enabled =
                     frontend_options.migration_assist,
                 .publication_model =
                     "driver-publishes-lowered-conformance-sidecar-and-runtime-capability-sidecar-next-to-manifest",
                 .publication_surface_kind = "frontend-c-api",
                 .fail_closed_diagnostic_model =
                     "core-profile-live-other-known-profiles-fail-closed-before-publication",
                 .lowered_report_contract_id =
                     "objc3c-versioned-conformance-report-lowering/m264-c001-v1",
                 .runtime_capability_contract_id =
                     "objc3c-runtime-capability-reporting/m264-c002-v1",
                 .public_conformance_schema_id =
                     "objc3-conformance-report/v1",
                 .report_artifact_relative_path =
                     (emit_prefix +
                      kObjc3VersionedConformanceReportLoweringArtifactSuffix)},
                conformance_publication_artifact_json,
                conformance_publication_error)) {
          result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
          result->process_exit_code = 2;
          result->success = 0;
          objc3c_frontend_set_error(context,
                                    conformance_publication_error.c_str());
        } else {
          const std::filesystem::path conformance_publication_out =
              BuildConformancePublicationArtifactPath(out_dir, emit_prefix);
          if (!WriteTextFile(conformance_publication_out,
                             conformance_publication_artifact_json, io_error)) {
            result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
            result->process_exit_code = 2;
            result->success = 0;
            objc3c_frontend_set_error(context, io_error.c_str());
          }
        }
      }
    }
  }

  const bool wants_ir_file = options->emit_ir != 0 || options->emit_object != 0;
  std::filesystem::path ir_out;
  if (result->status == OBJC3C_FRONTEND_STATUS_OK && wants_ir_file) {
    if (!has_out_dir) {
      result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(context, "emit_ir/emit_object require out_dir in compile options.");
    } else {
      ir_out = out_dir / (emit_prefix + ".ll");
      std::string io_error;
      if (!WriteTextFile(ir_out, product.artifact_bundle.ir_text, io_error)) {
        result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
        result->process_exit_code = 2;
        result->success = 0;
        objc3c_frontend_set_error(context, io_error.c_str());
      } else {
        context->ir_path = ir_out.generic_string();
      }
    }
  }

  if (result->status == OBJC3C_FRONTEND_STATUS_OK && options->emit_object != 0) {
    const bool wants_clang_backend =
        options->ir_object_backend == static_cast<uint8_t>(OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG);
    const bool wants_llvm_direct_backend =
        options->ir_object_backend == static_cast<uint8_t>(OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT);
    if (!wants_clang_backend && !wants_llvm_direct_backend) {
      result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(context, "emit_object requires a valid ir_object_backend (clang|llvm-direct).");
      emit_diagnostics.push_back(
          "error:1:1: emit_object requires valid ir_object_backend (clang|llvm-direct) [O3E001]");
    } else if (wants_clang_backend && IsNullOrEmpty(options->clang_path)) {
      result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(context, "emit_object requires clang_path in compile options.");
      emit_diagnostics.push_back("error:1:1: emit_object requires clang_path in compile options [O3E001]");
    } else if (wants_llvm_direct_backend && IsNullOrEmpty(options->llc_path)) {
      result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(context, "emit_object requires llc_path in compile options for llvm-direct backend.");
      emit_diagnostics.push_back(
          "error:1:1: emit_object requires llc_path in compile options for llvm-direct backend [O3E001]");
    } else {
      const std::filesystem::path object_out = out_dir / (emit_prefix + ".obj");
      const std::filesystem::path backend_out = out_dir / (emit_prefix + ".object-backend.txt");
      const std::string backend_text = wants_clang_backend ? "clang\n" : "llvm-direct\n";
      int compile_status = 0;
#if defined(OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION)
      const bool llvm_direct_backend_enabled = true;
#else
      const bool llvm_direct_backend_enabled = false;
#endif
      const std::filesystem::path clang_path = OptionalFilesystemPath(options->clang_path);
      const std::filesystem::path llc_path = OptionalFilesystemPath(options->llc_path);
      const Objc3ToolchainRuntimeGaOperationsScaffold toolchain_runtime_ga_operations_scaffold =
          BuildObjc3ToolchainRuntimeGaOperationsScaffold(
              wants_clang_backend,
              wants_llvm_direct_backend,
              clang_path,
              llc_path,
              llvm_direct_backend_enabled,
              ir_out,
              object_out);
      std::string toolchain_runtime_scaffold_reason;
      if (!IsObjc3ToolchainRuntimeGaOperationsScaffoldReady(
              toolchain_runtime_ga_operations_scaffold,
              toolchain_runtime_scaffold_reason)) {
        result->status = OBJC3C_FRONTEND_STATUS_EMIT_ERROR;
        result->process_exit_code = 125;
        result->success = 0;
        const std::string emit_error =
            "error:1:1: LLVM object emission failed: toolchain/runtime readiness scaffold fail-closed: " +
            toolchain_runtime_scaffold_reason + " [O3E002]";
        emit_diagnostics.push_back(emit_error);
        objc3c_frontend_set_error(context, emit_error.c_str());
      } else {
        bool backend_output_recorded = false;
        std::string backend_output_payload;
        std::string backend_output_error;
        std::string backend_error;
        if (wants_clang_backend) {
          compile_status = RunIRCompile(clang_path, ir_out, object_out);
        } else {
          compile_status = RunIRCompileLLVMDirect(llc_path, ir_out, object_out, backend_error);
        }
        if (compile_status == 0) {
          if (!WriteTextFile(backend_out, backend_text, backend_output_error)) {
            compile_status = 125;
          } else {
            backend_output_recorded = true;
            backend_output_payload = backend_text;
            Objc3RuntimeMetadataLinkerRetentionArtifacts
                linker_retention_artifacts;
            std::string linker_retention_error;
            if (!TryBuildObjc3RuntimeMetadataLinkerRetentionArtifacts(
                    ir_out,
                    object_out,
                    linker_retention_artifacts,
                    linker_retention_error)) {
              compile_status = 125;
              backend_error = linker_retention_error;
            } else {
              const std::filesystem::path linker_response_out =
                  BuildRuntimeMetadataLinkerResponseArtifactPath(out_dir,
                                                                 emit_prefix);
              if (!WriteTextFile(linker_response_out,
                                 linker_retention_artifacts
                                     .linker_response_file_payload,
                                 backend_output_error)) {
                compile_status = 125;
              } else {
                const std::filesystem::path discovery_out =
                    BuildRuntimeMetadataDiscoveryArtifactPath(out_dir,
                                                              emit_prefix);
                if (!WriteTextFile(discovery_out,
                                   linker_retention_artifacts.discovery_json,
                                   backend_output_error)) {
                  compile_status = 125;
                } else if (!IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
                               product.artifact_bundle
                                   .runtime_translation_unit_registration_manifest_summary)) {
                  compile_status = 125;
                  backend_error =
                      "translation-unit registration manifest template not ready";
                } else if (!IsReadyObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
                               product.artifact_bundle
                                   .runtime_registration_descriptor_image_root_source_surface_summary)) {
                  compile_status = 125;
                  backend_error =
                      "registration descriptor/image-root source surface not ready";
                } else if (!IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
                               product.artifact_bundle
                                   .runtime_registration_descriptor_frontend_closure_summary)) {
                  compile_status = 125;
                  backend_error =
                      "registration descriptor frontend closure not ready";
                } else {
                  Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs
                      manifest_inputs;
                  const auto &registration_manifest_summary =
                      product.artifact_bundle
                          .runtime_translation_unit_registration_manifest_summary;
                  const auto &registration_descriptor_source_surface_summary =
                      product.artifact_bundle
                          .runtime_registration_descriptor_image_root_source_surface_summary;
                  const auto &registration_descriptor_frontend_closure_summary =
                      product.artifact_bundle
                          .runtime_registration_descriptor_frontend_closure_summary;
                  const auto &runtime_bootstrap_api_summary =
                      product.artifact_bundle.runtime_bootstrap_api_summary;
                  const auto &runtime_bootstrap_semantics_summary =
                      product.artifact_bundle
                          .runtime_bootstrap_semantics_summary;
                  const auto &runtime_bootstrap_lowering_summary =
                      product.artifact_bundle.runtime_bootstrap_lowering_summary;
                  manifest_inputs.contract_id =
                      registration_manifest_summary.contract_id;
                  manifest_inputs.translation_unit_registration_contract_id =
                      registration_manifest_summary
                          .translation_unit_registration_contract_id;
                  manifest_inputs.runtime_support_library_link_wiring_contract_id =
                      registration_manifest_summary
                          .runtime_support_library_link_wiring_contract_id;
                  manifest_inputs.manifest_payload_model =
                      registration_manifest_summary.manifest_payload_model;
                  manifest_inputs.manifest_artifact_relative_path =
                      registration_manifest_summary
                          .manifest_artifact_relative_path;
                  manifest_inputs.runtime_owned_payload_artifacts.assign(
                      registration_manifest_summary
                          .runtime_owned_payload_artifacts.begin(),
                      registration_manifest_summary
                          .runtime_owned_payload_artifacts.end());
                  manifest_inputs.runtime_support_library_archive_relative_path =
                      registration_manifest_summary
                          .runtime_support_library_archive_relative_path;
                  manifest_inputs.constructor_root_symbol =
                      registration_manifest_summary.constructor_root_symbol;
                  manifest_inputs.constructor_root_ownership_model =
                      registration_manifest_summary
                          .constructor_root_ownership_model;
                  manifest_inputs.manifest_authority_model =
                      registration_manifest_summary.manifest_authority_model;
                  manifest_inputs.constructor_init_stub_symbol_prefix =
                      registration_manifest_summary
                          .constructor_init_stub_symbol_prefix;
                  manifest_inputs.constructor_init_stub_ownership_model =
                      registration_manifest_summary
                          .constructor_init_stub_ownership_model;
                  manifest_inputs.constructor_priority_policy =
                      registration_manifest_summary
                          .constructor_priority_policy;
                  manifest_inputs.registration_entrypoint_symbol =
                      registration_manifest_summary
                          .registration_entrypoint_symbol;
                  manifest_inputs.translation_unit_identity_model =
                      registration_manifest_summary
                          .translation_unit_identity_model;
                  manifest_inputs.launch_integration_contract_id =
                      registration_manifest_summary
                          .launch_integration_contract_id;
                  manifest_inputs.runtime_library_resolution_model =
                      registration_manifest_summary
                          .runtime_library_resolution_model;
                  manifest_inputs.driver_linker_flag_consumption_model =
                      registration_manifest_summary
                          .driver_linker_flag_consumption_model;
                  manifest_inputs.compile_wrapper_command_surface =
                      registration_manifest_summary
                          .compile_wrapper_command_surface;
                  manifest_inputs.compile_proof_command_surface =
                      registration_manifest_summary
                          .compile_proof_command_surface;
                  manifest_inputs.execution_smoke_command_surface =
                      registration_manifest_summary
                          .execution_smoke_command_surface;
                  manifest_inputs.registration_descriptor_source_contract_id =
                      registration_descriptor_source_surface_summary.contract_id;
                  manifest_inputs.registration_descriptor_source_surface_path =
                      registration_descriptor_source_surface_summary
                          .source_surface_path;
                  manifest_inputs.registration_descriptor_pragma_name =
                      registration_descriptor_source_surface_summary
                          .registration_descriptor_pragma_name;
                  manifest_inputs.image_root_pragma_name =
                      registration_descriptor_source_surface_summary
                          .image_root_pragma_name;
                  manifest_inputs.module_identity_source =
                      registration_descriptor_source_surface_summary
                          .module_identity_source;
                  manifest_inputs.registration_descriptor_identifier =
                      registration_descriptor_source_surface_summary
                          .registration_descriptor_identifier;
                  manifest_inputs.registration_descriptor_identity_source =
                      registration_descriptor_source_surface_summary
                          .registration_descriptor_identity_source;
                  manifest_inputs.image_root_identifier =
                      registration_descriptor_source_surface_summary
                          .image_root_identifier;
                  manifest_inputs.image_root_identity_source =
                      registration_descriptor_source_surface_summary
                          .image_root_identity_source;
                  manifest_inputs.bootstrap_visible_metadata_ownership_model =
                      registration_descriptor_source_surface_summary
                          .bootstrap_visible_metadata_ownership_model;
                  manifest_inputs.class_descriptor_count =
                      registration_manifest_summary.class_descriptor_count;
                  manifest_inputs.protocol_descriptor_count =
                      registration_manifest_summary.protocol_descriptor_count;
                  manifest_inputs.category_descriptor_count =
                      registration_manifest_summary.category_descriptor_count;
                  manifest_inputs.property_descriptor_count =
                      registration_manifest_summary.property_descriptor_count;
                  manifest_inputs.ivar_descriptor_count =
                      registration_manifest_summary.ivar_descriptor_count;
                  manifest_inputs.total_descriptor_count =
                      registration_manifest_summary.total_descriptor_count;
                  manifest_inputs.bootstrap_semantics_contract_id =
                      runtime_bootstrap_semantics_summary.contract_id;
                  manifest_inputs.duplicate_registration_policy =
                      runtime_bootstrap_semantics_summary
                          .duplicate_registration_policy;
                  manifest_inputs.realization_order_policy =
                      runtime_bootstrap_semantics_summary
                          .realization_order_policy;
                  manifest_inputs.failure_mode =
                      runtime_bootstrap_semantics_summary.failure_mode;
                  manifest_inputs.registration_result_model =
                      runtime_bootstrap_semantics_summary
                          .registration_result_model;
                  manifest_inputs.registration_order_ordinal_model =
                      runtime_bootstrap_semantics_summary
                          .registration_order_ordinal_model;
                  manifest_inputs.runtime_state_snapshot_symbol =
                      runtime_bootstrap_semantics_summary
                          .runtime_state_snapshot_symbol;
                  manifest_inputs.bootstrap_runtime_api_contract_id =
                      runtime_bootstrap_api_summary.contract_id;
                  manifest_inputs.bootstrap_runtime_api_public_header_path =
                      runtime_bootstrap_api_summary.public_header_path;
                  manifest_inputs.bootstrap_runtime_api_archive_relative_path =
                      runtime_bootstrap_api_summary.archive_relative_path;
                  manifest_inputs
                      .bootstrap_runtime_api_registration_status_enum_type =
                      runtime_bootstrap_api_summary
                          .registration_status_enum_type;
                  manifest_inputs.bootstrap_runtime_api_image_descriptor_type =
                      runtime_bootstrap_api_summary.image_descriptor_type;
                  manifest_inputs.bootstrap_runtime_api_selector_handle_type =
                      runtime_bootstrap_api_summary.selector_handle_type;
                  manifest_inputs
                      .bootstrap_runtime_api_registration_snapshot_type =
                      runtime_bootstrap_api_summary
                          .registration_snapshot_type;
                  manifest_inputs
                      .bootstrap_runtime_api_registration_entrypoint_symbol =
                      runtime_bootstrap_api_summary
                          .registration_entrypoint_symbol;
                  manifest_inputs.bootstrap_runtime_api_selector_lookup_symbol =
                      runtime_bootstrap_api_summary.selector_lookup_symbol;
                  manifest_inputs
                      .bootstrap_runtime_api_dispatch_entrypoint_symbol =
                      runtime_bootstrap_api_summary.dispatch_entrypoint_symbol;
                  manifest_inputs.bootstrap_runtime_api_state_snapshot_symbol =
                      runtime_bootstrap_api_summary.state_snapshot_symbol;
                  manifest_inputs.bootstrap_runtime_api_reset_for_testing_symbol =
                      runtime_bootstrap_api_summary.reset_for_testing_symbol;
                  manifest_inputs.bootstrap_registrar_contract_id =
                      kObjc3RuntimeBootstrapRegistrarContractId;
                  manifest_inputs.bootstrap_registrar_internal_header_path =
                      kObjc3RuntimeBootstrapInternalHeaderPath;
                  manifest_inputs
                      .bootstrap_registrar_stage_registration_table_symbol =
                      kObjc3RuntimeBootstrapStageRegistrationTableSymbol;
                  manifest_inputs.bootstrap_registrar_image_walk_snapshot_symbol =
                      kObjc3RuntimeBootstrapImageWalkSnapshotSymbol;
                  manifest_inputs.bootstrap_registrar_image_walk_model =
                      kObjc3RuntimeBootstrapImageWalkModel;
                  manifest_inputs
                      .bootstrap_registrar_discovery_root_validation_model =
                      kObjc3RuntimeBootstrapDiscoveryRootValidationModel;
                  manifest_inputs
                      .bootstrap_registrar_selector_pool_interning_model =
                      kObjc3RuntimeBootstrapSelectorPoolInterningModel;
                  manifest_inputs.bootstrap_registrar_realization_staging_model =
                      kObjc3RuntimeBootstrapRealizationStagingModel;
                  manifest_inputs.bootstrap_reset_contract_id =
                      kObjc3RuntimeBootstrapResetContractId;
                  manifest_inputs.bootstrap_reset_internal_header_path =
                      kObjc3RuntimeBootstrapInternalHeaderPath;
                  manifest_inputs
                      .bootstrap_reset_replay_registered_images_symbol =
                      kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol;
                  manifest_inputs
                      .bootstrap_reset_reset_replay_state_snapshot_symbol =
                      kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol;
                  manifest_inputs.bootstrap_reset_lifecycle_model =
                      kObjc3RuntimeBootstrapResetLifecycleModel;
                  manifest_inputs.bootstrap_reset_replay_order_model =
                      kObjc3RuntimeBootstrapReplayOrderModel;
                  manifest_inputs
                      .bootstrap_reset_image_local_init_state_reset_model =
                      kObjc3RuntimeBootstrapImageLocalInitStateResetModel;
                  manifest_inputs
                      .bootstrap_reset_bootstrap_catalog_retention_model =
                      kObjc3RuntimeBootstrapCatalogRetentionModel;
                  manifest_inputs.bootstrap_lowering_contract_id =
                      runtime_bootstrap_lowering_summary.contract_id;
                  manifest_inputs.bootstrap_lowering_boundary_model =
                      runtime_bootstrap_lowering_summary.lowering_boundary_model;
                  manifest_inputs.bootstrap_global_ctor_list_model =
                      runtime_bootstrap_lowering_summary.global_ctor_list_model;
                  manifest_inputs.bootstrap_registration_table_layout_model =
                      runtime_bootstrap_lowering_summary
                          .registration_table_layout_model;
                  manifest_inputs.bootstrap_image_local_initialization_model =
                      runtime_bootstrap_lowering_summary
                          .image_local_initialization_model;
                  manifest_inputs.bootstrap_constructor_root_emission_state =
                      runtime_bootstrap_lowering_summary
                          .constructor_root_emission_state;
                  manifest_inputs.bootstrap_init_stub_emission_state =
                      runtime_bootstrap_lowering_summary
                          .init_stub_emission_state;
                  manifest_inputs.bootstrap_registration_table_emission_state =
                      runtime_bootstrap_lowering_summary
                          .registration_table_emission_state;
                  manifest_inputs.bootstrap_registration_table_symbol_prefix =
                      runtime_bootstrap_lowering_summary
                          .registration_table_symbol_prefix;
                  manifest_inputs.bootstrap_image_local_init_state_symbol_prefix =
                      runtime_bootstrap_lowering_summary
                          .image_local_init_state_symbol_prefix;
                  manifest_inputs.bootstrap_registration_table_abi_version =
                      runtime_bootstrap_lowering_summary
                          .registration_table_abi_version;
                  manifest_inputs.bootstrap_registration_table_pointer_field_count =
                      runtime_bootstrap_lowering_summary
                          .registration_table_pointer_field_count;
                  manifest_inputs.success_status_code =
                      runtime_bootstrap_semantics_summary.success_status_code;
                  manifest_inputs.invalid_descriptor_status_code =
                      runtime_bootstrap_semantics_summary
                          .invalid_descriptor_status_code;
                  manifest_inputs.duplicate_registration_status_code =
                      runtime_bootstrap_semantics_summary
                          .duplicate_registration_status_code;
                  manifest_inputs.out_of_order_status_code =
                      runtime_bootstrap_semantics_summary
                          .out_of_order_status_code;
                  manifest_inputs.translation_unit_registration_order_ordinal =
                      registration_manifest_summary
                          .translation_unit_registration_order_ordinal;
                  manifest_inputs.object_artifact_relative_path =
                      object_out.filename().generic_string();
                  manifest_inputs.backend_artifact_relative_path =
                      backend_out.filename().generic_string();

                  std::string registration_manifest_json;
                  std::string registration_manifest_error;
                  if (!TryBuildObjc3RuntimeTranslationUnitRegistrationManifestArtifact(
                          manifest_inputs,
                          linker_retention_artifacts,
                          product.artifact_bundle.runtime_metadata_binary.size(),
                          registration_manifest_json,
                          registration_manifest_error)) {
                    compile_status = 125;
                    backend_error = registration_manifest_error;
                  } else {
                    // M263-E001 bootstrap-completion gate anchor: the frontend
                    // C API publishes the same manifest/descriptor artifact pair as the CLI path
                    // so lane-E bootstrap completion evidence stays identical across host entry points.
                    // M263-E002 bootstrap-matrix closeout anchor: the published
                    // bootstrap matrix stays identical across CLI and frontend C API entry points.
                    const std::filesystem::path registration_manifest_out =
                        BuildRuntimeRegistrationManifestArtifactPath(out_dir,
                                                                     emit_prefix);
                    if (!WriteTextFile(registration_manifest_out,
                                       registration_manifest_json,
                                       backend_output_error)) {
                      compile_status = 125;
                    } else {
                      Objc3RuntimeRegistrationDescriptorArtifactInputs
                          descriptor_inputs;
                      descriptor_inputs.contract_id =
                          registration_descriptor_frontend_closure_summary
                              .contract_id;
                      descriptor_inputs.registration_manifest_contract_id =
                          registration_descriptor_frontend_closure_summary
                              .registration_manifest_contract_id;
                      descriptor_inputs.source_surface_contract_id =
                          registration_descriptor_frontend_closure_summary
                              .source_surface_contract_id;
                      descriptor_inputs.payload_model =
                          registration_descriptor_frontend_closure_summary
                              .payload_model;
                      descriptor_inputs.artifact_relative_path =
                          registration_descriptor_frontend_closure_summary
                              .artifact_relative_path;
                      descriptor_inputs.authority_model =
                          registration_descriptor_frontend_closure_summary
                              .authority_model;
                      descriptor_inputs.translation_unit_identity_model =
                          registration_descriptor_frontend_closure_summary
                              .translation_unit_identity_model;
                      descriptor_inputs.payload_ownership_model =
                          registration_descriptor_frontend_closure_summary
                              .payload_ownership_model;
                      descriptor_inputs.runtime_support_library_archive_relative_path =
                          registration_manifest_summary
                              .runtime_support_library_archive_relative_path;
                      descriptor_inputs.registration_entrypoint_symbol =
                          registration_manifest_summary.registration_entrypoint_symbol;
                      descriptor_inputs.registration_descriptor_pragma_name =
                          registration_descriptor_source_surface_summary
                              .registration_descriptor_pragma_name;
                      descriptor_inputs.image_root_pragma_name =
                          registration_descriptor_source_surface_summary
                              .image_root_pragma_name;
                      descriptor_inputs.module_identity_source =
                          registration_descriptor_source_surface_summary
                              .module_identity_source;
                      descriptor_inputs.registration_descriptor_identifier =
                          registration_descriptor_frontend_closure_summary
                              .registration_descriptor_identifier;
                      descriptor_inputs.registration_descriptor_identity_source =
                          registration_descriptor_frontend_closure_summary
                              .registration_descriptor_identity_source;
                      descriptor_inputs.image_root_identifier =
                          registration_descriptor_frontend_closure_summary
                              .image_root_identifier;
                      descriptor_inputs.image_root_identity_source =
                          registration_descriptor_frontend_closure_summary
                              .image_root_identity_source;
                      descriptor_inputs.bootstrap_visible_metadata_ownership_model =
                          registration_descriptor_frontend_closure_summary
                              .bootstrap_visible_metadata_ownership_model;
                      descriptor_inputs.constructor_root_symbol =
                          registration_manifest_summary.constructor_root_symbol;
                      descriptor_inputs.constructor_init_stub_symbol_prefix =
                          registration_manifest_summary
                              .constructor_init_stub_symbol_prefix;
                      descriptor_inputs.bootstrap_registration_table_symbol_prefix =
                          runtime_bootstrap_lowering_summary
                              .registration_table_symbol_prefix;
                      descriptor_inputs.bootstrap_image_local_init_state_symbol_prefix =
                          runtime_bootstrap_lowering_summary
                              .image_local_init_state_symbol_prefix;
                      descriptor_inputs.class_descriptor_count =
                          registration_descriptor_frontend_closure_summary
                              .class_descriptor_count;
                      descriptor_inputs.protocol_descriptor_count =
                          registration_descriptor_frontend_closure_summary
                              .protocol_descriptor_count;
                      descriptor_inputs.category_descriptor_count =
                          registration_descriptor_frontend_closure_summary
                              .category_descriptor_count;
                      descriptor_inputs.property_descriptor_count =
                          registration_descriptor_frontend_closure_summary
                              .property_descriptor_count;
                      descriptor_inputs.ivar_descriptor_count =
                          registration_descriptor_frontend_closure_summary
                              .ivar_descriptor_count;
                      descriptor_inputs.total_descriptor_count =
                          registration_descriptor_frontend_closure_summary
                              .total_descriptor_count;
                      descriptor_inputs.translation_unit_registration_order_ordinal =
                          registration_descriptor_frontend_closure_summary
                              .translation_unit_registration_order_ordinal;
                      descriptor_inputs.object_artifact_relative_path =
                          object_out.filename().generic_string();
                      descriptor_inputs.backend_artifact_relative_path =
                          backend_out.filename().generic_string();

                      std::string registration_descriptor_json;
                      std::string registration_descriptor_error;
                      if (!TryBuildObjc3RuntimeRegistrationDescriptorArtifact(
                              descriptor_inputs,
                              linker_retention_artifacts,
                              registration_descriptor_json,
                              registration_descriptor_error)) {
                        compile_status = 125;
                        backend_error = registration_descriptor_error;
                      } else {
                        // M263-E001 bootstrap-completion gate anchor: the
                        // frontend C API publishes the same manifest/descriptor artifact pair as the CLI path.
                        // M263-E002 bootstrap-matrix closeout anchor: the
                        // published bootstrap matrix stays identical across CLI and frontend C API entry points.
                        const std::filesystem::path
                            registration_descriptor_out =
                                BuildRuntimeRegistrationDescriptorArtifactPath(
                                    out_dir, emit_prefix);
                        if (!WriteTextFile(registration_descriptor_out,
                                           registration_descriptor_json,
                                           backend_output_error)) {
                          compile_status = 125;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
        const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface toolchain_runtime_core_feature_surface =
            BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface(
                toolchain_runtime_ga_operations_scaffold,
                compile_status,
                backend_output_recorded,
                backend_out,
                backend_output_payload);
        std::string toolchain_runtime_core_feature_reason;
        const bool toolchain_runtime_core_feature_ready =
            IsObjc3ToolchainRuntimeGaOperationsCoreFeatureSurfaceReady(
                toolchain_runtime_core_feature_surface,
                toolchain_runtime_core_feature_reason);
        if (compile_status != 0) {
          result->status = OBJC3C_FRONTEND_STATUS_EMIT_ERROR;
          result->process_exit_code = compile_status;
          result->success = 0;
          std::string emit_error;
          if (!backend_output_error.empty()) {
            emit_error = "error:1:1: LLVM object emission failed: " + backend_output_error + " [O3E002]";
          } else if (!backend_error.empty()) {
            emit_error = "error:1:1: LLVM object emission failed: " + backend_error + " [O3E002]";
          } else if (wants_clang_backend) {
            emit_error =
                "error:1:1: LLVM object emission failed: clang exited with status " + std::to_string(compile_status) +
                " [O3E002]";
          } else {
            emit_error =
                "error:1:1: LLVM object emission failed: llc exited with status " + std::to_string(compile_status) +
                " [O3E002]";
          }
          emit_diagnostics.push_back(emit_error);
          objc3c_frontend_set_error(context, emit_error.c_str());
        } else if (!toolchain_runtime_core_feature_ready) {
          result->status = OBJC3C_FRONTEND_STATUS_EMIT_ERROR;
          result->process_exit_code = 125;
          result->success = 0;
          const std::string emit_error =
              "error:1:1: LLVM object emission failed: toolchain/runtime core feature fail-closed: " +
              toolchain_runtime_core_feature_reason + " [O3E002]";
          emit_diagnostics.push_back(emit_error);
          objc3c_frontend_set_error(context, emit_error.c_str());
        } else {
          context->object_path = object_out.generic_string();
        }
      }
    }
  }

  if (result->status == OBJC3C_FRONTEND_STATUS_DIAGNOSTICS) {
    if (!product.artifact_bundle.diagnostics.empty()) {
      objc3c_frontend_set_error(context, product.artifact_bundle.diagnostics.front().c_str());
    } else {
      objc3c_frontend_set_error(context, "compilation reported diagnostics.");
    }
  } else if (result->status == OBJC3C_FRONTEND_STATUS_OK) {
    objc3c_frontend_set_error(context, "");
  }

  const bool wants_emit_stage = options->emit_ir != 0 || options->emit_object != 0;
  const bool emit_attempted = lower_attempted && wants_emit_stage;
  const bool emit_skipped = !emit_attempted;
  result->semantic_skipped = product.pipeline_result.integration_surface.built ? 0u : 1u;

  result->lex = BuildStageSummary(OBJC3C_FRONTEND_STAGE_LEX, true, false, product.pipeline_result.stage_diagnostics.lexer);
  result->parse =
      BuildStageSummary(OBJC3C_FRONTEND_STAGE_PARSE, true, false, product.pipeline_result.stage_diagnostics.parser);
  result->sema = BuildStageSummary(OBJC3C_FRONTEND_STAGE_SEMA,
                                   sema_attempted,
                                   !sema_attempted,
                                   product.pipeline_result.stage_diagnostics.semantic);
  result->lower = BuildStageSummary(OBJC3C_FRONTEND_STAGE_LOWER, lower_attempted, !lower_attempted, {});
  result->emit = BuildStageSummary(OBJC3C_FRONTEND_STAGE_EMIT, emit_attempted, emit_skipped, emit_diagnostics);

  PopulateResultPaths(context, result);
  return result->status;
}

extern "C" OBJC3C_FRONTEND_API uint8_t objc3c_frontend_is_abi_compatible(uint32_t requested_abi_version) {
  return requested_abi_version >= OBJC3C_FRONTEND_MIN_COMPATIBILITY_ABI_VERSION &&
                 requested_abi_version <= OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION
             ? 1u
             : 0u;
}

extern "C" OBJC3C_FRONTEND_API uint32_t objc3c_frontend_abi_version(void) {
  return OBJC3C_FRONTEND_ABI_VERSION;
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_version_t objc3c_frontend_version(void) {
  const objc3c_frontend_version_t version = OBJC3C_FRONTEND_VERSION_INIT;
  return version;
}

extern "C" OBJC3C_FRONTEND_API const char *objc3c_frontend_version_string(void) {
  return OBJC3C_FRONTEND_VERSION_STRING;
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_context_t *objc3c_frontend_context_create(void) {
  objc3c_frontend_context_t *context = new (std::nothrow) objc3c_frontend_context_t();
  if (context != nullptr) {
    context->last_error.clear();
  }
  return context;
}

extern "C" OBJC3C_FRONTEND_API void objc3c_frontend_context_destroy(objc3c_frontend_context_t *context) {
  delete context;
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_status_t objc3c_frontend_compile_file(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result) {
  if (context == nullptr || options == nullptr || result == nullptr) {
    return OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
  }
  std::string language_version_error;
  if (!ValidateSupportedLanguageVersion(options->language_version, language_version_error)) {
    return SetUsageError(context, result, language_version_error);
  }
  std::string compatibility_mode_error;
  if (!ValidateSupportedCompatibilityMode(options->compatibility_mode, compatibility_mode_error)) {
    return SetUsageError(context, result, compatibility_mode_error);
  }
  if (IsNullOrEmpty(options->input_path)) {
    return SetUsageError(context, result, "compile_file requires compile_options.input_path.");
  }

  std::string source_text;
  std::string io_error;
  const std::filesystem::path input_path(options->input_path);
  if (!ReadTextFile(input_path, source_text, io_error)) {
    return SetUsageError(context, result, io_error);
  }
  return CompileObjc3SourceImpl(context, input_path, source_text, options, result);
}

extern "C" OBJC3C_FRONTEND_API objc3c_frontend_status_t objc3c_frontend_compile_source(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result) {
  if (context == nullptr || options == nullptr || result == nullptr) {
    return OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
  }
  std::string language_version_error;
  if (!ValidateSupportedLanguageVersion(options->language_version, language_version_error)) {
    return SetUsageError(context, result, language_version_error);
  }
  std::string compatibility_mode_error;
  if (!ValidateSupportedCompatibilityMode(options->compatibility_mode, compatibility_mode_error)) {
    return SetUsageError(context, result, compatibility_mode_error);
  }
  if (IsNullOrEmpty(options->source_text)) {
    return SetUsageError(context, result, "compile_source requires compile_options.source_text.");
  }
  const std::filesystem::path input_path = ResolveInputPath(*options);
  return CompileObjc3SourceImpl(context, input_path, std::string(options->source_text), options, result);
}

extern "C" OBJC3C_FRONTEND_API size_t objc3c_frontend_copy_last_error(
    const objc3c_frontend_context_t *context,
    char *buffer,
    size_t buffer_size) {
  const std::string message = context == nullptr ? "" : context->last_error;
  const size_t required = message.size() + 1;

  if (buffer == nullptr || buffer_size == 0) {
    return required;
  }

  const size_t bytes_to_copy = std::min(required - 1, buffer_size - 1);
  if (bytes_to_copy > 0) {
    std::memcpy(buffer, message.data(), bytes_to_copy);
  }
  buffer[bytes_to_copy] = '\0';
  return required;
}

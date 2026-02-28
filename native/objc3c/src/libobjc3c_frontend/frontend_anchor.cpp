#include "libobjc3c_frontend/api.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <new>
#include <limits>
#include <process.h>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

#include "libobjc3c_frontend/objc3_cli_frontend.h"

struct objc3c_frontend_context {
  std::string last_error;
  std::string diagnostics_path;
  std::string manifest_path;
  std::string ir_path;
  std::string object_path;
};

static void objc3c_frontend_set_error(objc3c_frontend_context_t *context, const char *message) {
  if (context == nullptr) {
    return;
  }
  context->last_error = message == nullptr ? "" : message;
}

static bool IsNullOrEmpty(const char *text) {
  return text == nullptr || text[0] == '\0';
}

static std::string ToLowerCopy(std::string value) {
  for (char &ch : value) {
    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
  return value;
}

static const char *DefaultEmitPrefix = "module";
static const char *DefaultMemoryInputPath = "<memory>";

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
  } catch (...) {
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
  std::filesystem::create_directories(path.parent_path(), mkdir_error);
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

static int RunProcess(const std::string &executable, const std::vector<std::string> &args) {
  std::vector<const char *> argv;
  argv.reserve(args.size() + 2);
  argv.push_back(executable.c_str());
  for (const auto &arg : args) {
    argv.push_back(arg.c_str());
  }
  argv.push_back(nullptr);

  const int status = _spawnvp(_P_WAIT, executable.c_str(), argv.data());
  if (status == -1) {
    return 127;
  }
  return status;
}

static int RunIrCompile(const std::filesystem::path &clang_path,
                        const std::filesystem::path &ir_path,
                        const std::filesystem::path &object_path) {
  return RunProcess(clang_path.string(),
                    {"-x", "ir", "-c", ir_path.string(), "-o", object_path.string(), "-fno-color-diagnostics"});
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

static Objc3FrontendOptions BuildFrontendOptions(const objc3c_frontend_compile_options_t &options) {
  Objc3FrontendOptions frontend_options;
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

  if (result->status == OBJC3C_FRONTEND_STATUS_OK && options->emit_manifest != 0 && has_out_dir) {
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
    if (IsNullOrEmpty(options->clang_path)) {
      result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(context, "emit_object requires clang_path in compile options.");
      emit_diagnostics.push_back("error:1:1: emit_object requires clang_path in compile options [O3E001]");
    } else {
      const std::filesystem::path object_out = out_dir / (emit_prefix + ".obj");
      const int compile_status = RunIrCompile(std::filesystem::path(options->clang_path), ir_out, object_out);
      if (compile_status != 0) {
        result->status = OBJC3C_FRONTEND_STATUS_EMIT_ERROR;
        result->process_exit_code = compile_status;
        result->success = 0;
        const std::string emit_error =
            "error:1:1: LLVM object emission failed: clang exited with status " + std::to_string(compile_status) +
            " [O3E002]";
        emit_diagnostics.push_back(emit_error);
        objc3c_frontend_set_error(context, emit_error.c_str());
      } else {
        context->object_path = object_out.generic_string();
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

  const bool emit_attempted = lower_attempted;
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
  return (objc3c_frontend_version_t)OBJC3C_FRONTEND_VERSION_INIT;
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
  if (IsNullOrEmpty(options->input_path)) {
    *result = {};
    result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
    objc3c_frontend_set_error(context, "compile_file requires compile_options.input_path.");
    return result->status;
  }

  std::string source_text;
  std::string io_error;
  const std::filesystem::path input_path(options->input_path);
  if (!ReadTextFile(input_path, source_text, io_error)) {
    *result = {};
    result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
    objc3c_frontend_set_error(context, io_error.c_str());
    return result->status;
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
  if (IsNullOrEmpty(options->source_text)) {
    *result = {};
    result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
    objc3c_frontend_set_error(context, "compile_source requires compile_options.source_text.");
    return result->status;
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

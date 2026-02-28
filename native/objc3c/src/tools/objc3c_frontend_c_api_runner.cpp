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

namespace fs = std::filesystem;

namespace {

constexpr std::size_t kMaxMessageSendArgs = 16;

struct RunnerOptions {
  fs::path input_path;
  fs::path out_dir = fs::path(".");
  std::string emit_prefix = "module";
  fs::path clang_path = fs::path("clang");
  std::uint32_t max_message_send_args = 0;
  std::string runtime_dispatch_symbol;
  bool emit_manifest = true;
  bool emit_ir = true;
  bool emit_object = true;
  fs::path summary_out;
};

std::string Usage() {
  return "usage: objc3c-frontend-c-api-runner <input> [--out-dir <dir>] [--emit-prefix <name>] "
         "[--clang <path>] [--summary-out <path>] [--objc3-max-message-args <0-" +
         std::to_string(kMaxMessageSendArgs) +
         ">] [--objc3-runtime-dispatch-symbol <symbol>] [--no-emit-manifest] [--no-emit-ir] [--no-emit-object]";
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
                             const std::string &last_error) {
  std::ostringstream out;
  out << "{\n";
  out << "  \"mode\": \"objc3c-frontend-c-api-runner-v1\",\n";
  out << "  \"input_path\": \"" << EscapeJsonString(options.input_path.generic_string()) << "\",\n";
  out << "  \"out_dir\": \"" << EscapeJsonString(options.out_dir.generic_string()) << "\",\n";
  out << "  \"emit_prefix\": \"" << EscapeJsonString(options.emit_prefix) << "\",\n";
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
  const char *runtime_symbol = options.runtime_dispatch_symbol.empty() ? nullptr : options.runtime_dispatch_symbol.c_str();

  objc3c_frontend_c_compile_options_t compile_options = {};
  compile_options.input_path = input_path_text.c_str();
  compile_options.out_dir = out_dir_text.c_str();
  compile_options.emit_prefix = options.emit_prefix.c_str();
  compile_options.clang_path = options.emit_object ? clang_path_text.c_str() : nullptr;
  compile_options.runtime_dispatch_symbol = runtime_symbol;
  compile_options.max_message_send_args = options.max_message_send_args;
  compile_options.emit_manifest = options.emit_manifest ? 1u : 0u;
  compile_options.emit_ir = options.emit_ir ? 1u : 0u;
  compile_options.emit_object = options.emit_object ? 1u : 0u;

  objc3c_frontend_c_compile_result_t result = {};
  const objc3c_frontend_c_status_t status = objc3c_frontend_c_compile_file(context, &compile_options, &result);
  const std::string last_error = ReadLastError(context);
  const int exit_code = ExitCodeFromStatus(status, result);

  const fs::path summary_path =
      options.summary_out.empty() ? (options.out_dir / (options.emit_prefix + ".c_api_summary.json")) : options.summary_out;
  const std::string summary_json = BuildSummaryJson(options, status, result, last_error);
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

#include "tools/objc3c_frontend_c_api_runner_options.h"

#include <cerrno>
#include <cstddef>
#include <cstdlib>

#include "diagnostics/modes/objc3_removed_mode_options.h"
#include "support/objc3_ir_object_backend_token.h"
#include "support/objc3_runtime_dispatch_symbol.h"

namespace {

constexpr std::size_t kFrontendCApiRunnerMaxMessageSendArgs = 16;

bool ParseFrontendRunnerIrObjectBackend(
    const std::string &value,
    objc3c_frontend_c_ir_object_backend_t &backend) {
  objc3c::support::IrObjectBackendToken token;
  if (!objc3c::support::ParseIrObjectBackendToken(value, token)) {
    return false;
  }
  if (token == objc3c::support::IrObjectBackendToken::Clang) {
    backend = OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG;
    return true;
  }
  backend = OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT;
  return true;
}

}  // namespace

std::string FrontendCApiRunnerUsage() {
  return "usage: objc3c-frontend-c-api-runner <input> [--out-dir <dir>] [--emit-prefix <name>] "
         "[--clang <path>] [--llc <path>] [--summary-out <path>] [--objc3-max-message-args <0-" +
         std::to_string(kFrontendCApiRunnerMaxMessageSendArgs) +
         ">] [--objc3-runtime-dispatch-symbol <symbol>] "
         "[--objc3-bootstrap-registration-order-ordinal <positive-int>] "
         "[--objc3-ir-object-backend <clang|llvm-direct>] "
         "[--no-emit-manifest] [--no-emit-ir] [--no-emit-object] "
         "[--dump-summary-json] [--dump-observability-json] [--dump-playground-repro-json] "
         "[--dump-runtime-inspector-json] [--dump-stage-trace-json]";
}

bool ParseFrontendCApiRunnerOptions(int argc,
                                    char **argv,
                                    FrontendCApiRunnerOptions &options,
                                    std::string &error) {
  if (argc < 2) {
    error = FrontendCApiRunnerUsage();
    return false;
  }

  options = FrontendCApiRunnerOptions{};
  options.input_path = std::filesystem::path(argv[1]);

  for (int i = 2; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--out-dir" && i + 1 < argc) {
      options.out_dir = std::filesystem::path(argv[++i]);
    } else if (arg == "--emit-prefix" && i + 1 < argc) {
      options.emit_prefix = argv[++i];
    } else if (arg == "--clang" && i + 1 < argc) {
      options.clang_path = std::filesystem::path(argv[++i]);
    } else if (arg == "--llc" && i + 1 < argc) {
      options.llc_path = std::filesystem::path(argv[++i]);
    } else if (arg == "--summary-out" && i + 1 < argc) {
      options.summary_out = std::filesystem::path(argv[++i]);
    } else if (arg == "--objc3-max-message-args" && i + 1 < argc) {
      const std::string value = argv[++i];
      errno = 0;
      char *end = nullptr;
      const unsigned long parsed = std::strtoul(value.c_str(), &end, 10);
      if (value.empty() || end == value.c_str() || *end != '\0' ||
          errno == ERANGE || parsed > kFrontendCApiRunnerMaxMessageSendArgs) {
        error = "invalid --objc3-max-message-args (expected integer 0-" +
                std::to_string(kFrontendCApiRunnerMaxMessageSendArgs) + "): " +
                value;
        return false;
      }
      options.max_message_send_args = static_cast<std::uint32_t>(parsed);
    } else if (arg == "--objc3-runtime-dispatch-symbol" && i + 1 < argc) {
      options.runtime_dispatch_symbol = argv[++i];
      if (!objc3c::support::IsValidRuntimeDispatchSymbol(
              options.runtime_dispatch_symbol)) {
        error =
            "invalid --objc3-runtime-dispatch-symbol (expected [A-Za-z_.$][A-Za-z0-9_.$]*): " +
            options.runtime_dispatch_symbol;
        return false;
      }
    } else if (arg == "--objc3-bootstrap-registration-order-ordinal" &&
               i + 1 < argc) {
      const std::string value = argv[++i];
      errno = 0;
      char *end = nullptr;
      const unsigned long long parsed =
          std::strtoull(value.c_str(), &end, 10);
      if (value.empty() || end == value.c_str() || *end != '\0' ||
          errno == ERANGE || parsed == 0) {
        error =
            "invalid --objc3-bootstrap-registration-order-ordinal (expected "
            "positive integer): " +
            value;
        return false;
      }
      options.translation_unit_registration_order_ordinal =
          static_cast<std::uint64_t>(parsed);
    } else if (objc3c::diagnostics::modes::BuildRemovedModeOptionDiagnostic(
                   arg, error)) {
      return false;
    } else if (arg == "--objc3-ir-object-backend" && i + 1 < argc) {
      const std::string backend = argv[++i];
      if (!ParseFrontendRunnerIrObjectBackend(backend,
                                              options.ir_object_backend)) {
        error = "invalid --objc3-ir-object-backend (expected clang|llvm-direct): " +
                backend;
        return false;
      }
    } else if (arg == "--no-emit-manifest") {
      options.emit_manifest = false;
    } else if (arg == "--no-emit-ir") {
      options.emit_ir = false;
    } else if (arg == "--no-emit-object") {
      options.emit_object = false;
    } else if (arg == "--dump-summary-json") {
      options.dump_summary_json = true;
    } else if (arg == "--dump-observability-json") {
      options.dump_observability_json = true;
    } else if (arg == "--dump-playground-repro-json") {
      options.dump_playground_repro_json = true;
    } else if (arg == "--dump-runtime-inspector-json") {
      options.dump_runtime_inspector_json = true;
    } else if (arg == "--dump-stage-trace-json") {
      options.dump_stage_trace_json = true;
    } else if (arg == "--help" || arg == "-h") {
      error = FrontendCApiRunnerUsage();
      return false;
    } else {
      error = "unknown arg: " + arg;
      return false;
    }
  }

  return true;
}

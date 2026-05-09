#include "tools/objc3c_frontend_c_api_runner_options.h"

#include "diagnostics/modes/objc3_removed_mode_options.h"
#include "tools/objc3c_frontend_c_api_runner_dump_options.h"
#include "tools/objc3c_frontend_c_api_runner_option_values.h"

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
      if (!ParseFrontendCApiRunnerMaxMessageSendArgs(
              argv[++i], options.max_message_send_args, error)) {
        return false;
      }
    } else if (arg == "--objc3-runtime-dispatch-symbol" && i + 1 < argc) {
      if (!ParseFrontendCApiRunnerRuntimeDispatchSymbol(
              argv[++i], options.runtime_dispatch_symbol, error)) {
        return false;
      }
    } else if (arg == "--objc3-bootstrap-registration-order-ordinal" &&
               i + 1 < argc) {
      if (!ParseFrontendCApiRunnerRegistrationOrderOrdinal(
              argv[++i],
              options.translation_unit_registration_order_ordinal,
              error)) {
        return false;
      }
    } else if (objc3c::diagnostics::modes::BuildRemovedModeOptionDiagnostic(
                   arg, error)) {
      return false;
    } else if (arg == "--objc3-ir-object-backend" && i + 1 < argc) {
      const std::string backend = argv[++i];
      if (!ParseFrontendCApiRunnerIrObjectBackend(backend,
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
    } else if (ApplyFrontendCApiRunnerDumpOption(arg, options)) {
      continue;
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

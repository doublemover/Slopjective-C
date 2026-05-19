#include "driver/objc3_cli_runtime_options.h"

#include "driver/objc3_cli_integer_parsers.h"
#include "driver/objc3_cli_option_reader.h"
#include "support/objc3_runtime_dispatch_symbol.h"

bool TryApplyObjc3CliRuntimeOption(const std::string &flag,
                                   int &index,
                                   int argc,
                                   char **argv,
                                   Objc3CliOptions &options,
                                   std::string &error,
                                   bool &matched) {
  matched = true;
  std::string value;
  if (flag == "--objc3-bootstrap-registration-order-ordinal") {
    if (!ReadObjc3CliRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    std::uint64_t parsed_ordinal = 0;
    if (!ParseObjc3PositiveOrdinal(value, parsed_ordinal)) {
      error =
          "invalid --objc3-bootstrap-registration-order-ordinal (expected positive integer): " +
          value;
      return false;
    }
    options.bootstrap_registration_order_ordinal = parsed_ordinal;
    return true;
  }
  if (flag == "--objc3-metaprogramming-cache-root") {
    if (!ReadObjc3CliRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    if (value.empty()) {
      error =
          "invalid --objc3-metaprogramming-cache-root (expected non-empty path)";
      return false;
    }
    options.metaprogramming_cache_root = value;
    return true;
  }
  if (flag == "--objc3-import-runtime-surface") {
    if (!ReadObjc3CliRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    options.imported_runtime_surface_paths.push_back(value);
    return true;
  }
  if (flag == "--objc3-enable-live-error-runtime-surface") {
    options.allow_live_error_runtime_surface = true;
    return true;
  }
  if (flag == "--objc3-max-message-args") {
    if (!ReadObjc3CliRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    std::size_t parsed = 0;
    if (!ParseObjc3MessageSendArgLimit(value, parsed)) {
      error = "invalid --objc3-max-message-args (expected integer 0-" +
              std::to_string(kObjc3CliMaxMessageSendArgs) + "): " + value;
      return false;
    }
    options.max_message_send_args = parsed;
    return true;
  }
  if (flag == "--objc3-runtime-dispatch-symbol") {
    if (!ReadObjc3CliRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    if (!objc3c::support::IsValidRuntimeDispatchSymbol(value)) {
      error =
          "invalid --objc3-runtime-dispatch-symbol (expected [A-Za-z_.$][A-Za-z0-9_.$]*): " +
          value;
      return false;
    }
    options.runtime_dispatch_symbol = value;
    return true;
  }
  matched = false;
  return true;
}

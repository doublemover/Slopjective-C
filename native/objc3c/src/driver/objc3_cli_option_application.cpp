#include "driver/objc3_cli_option_application.h"

#include "diagnostics/modes/objc3_removed_mode_options.h"
#include "driver/objc3_cli_value_parsers.h"
#include "support/objc3_runtime_dispatch_symbol.h"

namespace {

bool ReadRequiredValue(const std::string &flag,
                       int &index,
                       int argc,
                       char **argv,
                       std::string &value,
                       std::string &error) {
  if (index + 1 >= argc) {
    error = "missing value for " + flag;
    return false;
  }
  value = argv[++index];
  return true;
}

}  // namespace

bool ApplyObjc3CliOption(int &index,
                         int argc,
                         char **argv,
                         Objc3CliOptions &options,
                         std::string &error) {
  const std::string flag = argv[index];
  if (flag.rfind("-fobjc-version=", 0) == 0) {
    const std::string version_value =
        flag.substr(std::string("-fobjc-version=").size());
    std::uint32_t parsed_version = 0;
    if (!ParseObjc3LanguageVersion(version_value, parsed_version)) {
      error =
          "invalid -fobjc-version (expected unsigned integer): " +
          version_value;
      return false;
    }
    options.language_version = parsed_version;
    return true;
  }
  if (flag == "-fobjc-arc") {
    options.arc_mode = Objc3ArcMode::kEnabled;
    return true;
  }
  if (flag == "-fno-objc-arc") {
    options.arc_mode = Objc3ArcMode::kDisabled;
    return true;
  }

  std::string value;
  if (flag == "--out-dir") {
    if (!ReadRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    options.out_dir = value;
    return true;
  }
  if (flag == "--emit-prefix") {
    if (!ReadRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    options.emit_prefix = value;
    return true;
  }
  if (flag == "-fobjc-version" || flag == "--objc3-language-version") {
    if (!ReadRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    std::uint32_t parsed_version = 0;
    if (!ParseObjc3LanguageVersion(value, parsed_version)) {
      error = "invalid " + flag + " (expected unsigned integer): " + value;
      return false;
    }
    options.language_version = parsed_version;
    return true;
  }
  if (objc3c::diagnostics::modes::BuildRemovedModeOptionDiagnostic(flag,
                                                                   error)) {
    return false;
  }
  if (flag == "--objc3-conformance-profile") {
    if (!ReadRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    if (!ParseObjc3ConformanceProfile(value, options.conformance_profile)) {
      error =
          "invalid --objc3-conformance-profile (expected core|strict|strict-concurrency|strict-system): " +
          value;
      return false;
    }
    return true;
  }
  if (flag == "--emit-objc3-conformance") {
    options.emit_objc3_conformance = true;
    return true;
  }
  if (flag == "--emit-objc3-conformance-format") {
    if (!ReadRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    options.emit_objc3_conformance_format = value;
    return true;
  }
  if (flag == "--validate-objc3-conformance") {
    if (!ReadRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    options.command_mode = Objc3CliCommandMode::kValidateConformance;
    options.validate_conformance_report_path = value;
    return true;
  }
  if (flag == "--objc3-bootstrap-registration-order-ordinal") {
    if (!ReadRequiredValue(flag, index, argc, argv, value, error)) {
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
    if (!ReadRequiredValue(flag, index, argc, argv, value, error)) {
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
  if (flag == "--clang") {
    if (!ReadRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    options.clang_path = value;
    options.clang_path_explicit = true;
    return true;
  }
  if (flag == "--llc") {
    if (!ReadRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    options.llc_path = value;
    options.llc_path_explicit = true;
    return true;
  }
  if (flag == "--objc3-import-runtime-surface") {
    if (!ReadRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    options.imported_runtime_surface_paths.push_back(value);
    return true;
  }
  if (flag == "--objc3-ir-object-backend") {
    if (!ReadRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    if (!ParseObjc3CliIrObjectBackend(value, options.ir_object_backend)) {
      error =
          "invalid --objc3-ir-object-backend (expected clang|llvm-direct): " +
          value;
      return false;
    }
    return true;
  }
  if (flag == "--llvm-capabilities-summary") {
    if (!ReadRequiredValue(flag, index, argc, argv, value, error)) {
      return false;
    }
    options.llvm_capabilities_summary = value;
    return true;
  }
  if (flag == "--objc3-route-backend-from-capabilities") {
    options.route_backend_from_capabilities = true;
    return true;
  }
  if (flag == "--objc3-max-message-args") {
    if (!ReadRequiredValue(flag, index, argc, argv, value, error)) {
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
    if (!ReadRequiredValue(flag, index, argc, argv, value, error)) {
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

  error = "unknown arg: " + flag;
  return false;
}

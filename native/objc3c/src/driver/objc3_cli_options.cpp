#include "driver/objc3_cli_options.h"

#include <string>

#include "config/objc3_language_profile.h"
#include "diagnostics/modes/objc3_removed_mode_options.h"
#include "driver/objc3_cli_environment.h"
#include "driver/objc3_cli_value_parsers.h"
#include "support/objc3_runtime_dispatch_symbol.h"

bool ParseObjc3CliOptions(int argc, char **argv, Objc3CliOptions &options, std::string &error) {
  if (argc < 2) {
    error = Objc3CliUsage();
    return false;
  }

  options = Objc3CliOptions{};
  ApplyObjc3CliEnvironmentDefaults(options);
  int index = 1;
  if (argv[1][0] != '-') {
    options.input = argv[1];
    index = 2;
  }

  for (int i = index; i < argc; ++i) {
    std::string flag = argv[i];
    if (flag.rfind("-fobjc-version=", 0) == 0) {
      const std::string version_value = flag.substr(std::string("-fobjc-version=").size());
      std::uint32_t parsed_version = 0;
      if (!ParseObjc3LanguageVersion(version_value, parsed_version)) {
        error = "invalid -fobjc-version (expected unsigned integer): " + version_value;
        return false;
      }
      options.language_version = parsed_version;
      continue;
    }
    if (flag == "-fobjc-arc") {
      options.arc_mode = Objc3ArcMode::kEnabled;
      continue;
    }
    if (flag == "-fno-objc-arc") {
      options.arc_mode = Objc3ArcMode::kDisabled;
      continue;
    }

    if (flag == "--out-dir" && i + 1 < argc) {
      options.out_dir = argv[++i];
    } else if (flag == "--emit-prefix" && i + 1 < argc) {
      options.emit_prefix = argv[++i];
    } else if ((flag == "-fobjc-version" || flag == "--objc3-language-version") && i + 1 < argc) {
      const std::string version_value = argv[++i];
      std::uint32_t parsed_version = 0;
      if (!ParseObjc3LanguageVersion(version_value, parsed_version)) {
        error = "invalid " + flag + " (expected unsigned integer): " + version_value;
        return false;
      }
      options.language_version = parsed_version;
    } else if (objc3c::diagnostics::modes::BuildRemovedModeOptionDiagnostic(flag, error)) {
      return false;
    } else if (flag == "--objc3-conformance-profile" && i + 1 < argc) {
      const std::string profile_text = argv[++i];
      if (!ParseObjc3ConformanceProfile(profile_text,
                                        options.conformance_profile)) {
        error =
            "invalid --objc3-conformance-profile (expected core|strict|strict-concurrency|strict-system): " +
            profile_text;
        return false;
      }
    } else if (flag == "--emit-objc3-conformance") {
      options.emit_objc3_conformance = true;
    } else if (flag == "--emit-objc3-conformance-format" && i + 1 < argc) {
      options.emit_objc3_conformance_format = argv[++i];
    } else if (flag == "--validate-objc3-conformance" && i + 1 < argc) {
      options.command_mode = Objc3CliCommandMode::kValidateConformance;
      options.validate_conformance_report_path = argv[++i];
    } else if (flag == "--objc3-bootstrap-registration-order-ordinal" &&
               i + 1 < argc) {
      const std::string ordinal_text = argv[++i];
      std::uint64_t parsed_ordinal = 0;
      if (!ParseObjc3PositiveOrdinal(ordinal_text, parsed_ordinal)) {
        error =
            "invalid --objc3-bootstrap-registration-order-ordinal (expected positive integer): " +
            ordinal_text;
        return false;
      }
      options.bootstrap_registration_order_ordinal = parsed_ordinal;
    } else if (flag == "--objc3-metaprogramming-cache-root" && i + 1 < argc) {
      options.metaprogramming_cache_root = argv[++i];
      if (options.metaprogramming_cache_root.empty()) {
        error = "invalid --objc3-metaprogramming-cache-root (expected non-empty path)";
        return false;
      }
    } else if (flag == "--clang" && i + 1 < argc) {
      options.clang_path = argv[++i];
      options.clang_path_explicit = true;
    } else if (flag == "--llc" && i + 1 < argc) {
      options.llc_path = argv[++i];
      options.llc_path_explicit = true;
    } else if (flag == "--objc3-import-runtime-surface" && i + 1 < argc) {
      options.imported_runtime_surface_paths.push_back(argv[++i]);
    } else if (flag == "--objc3-ir-object-backend" && i + 1 < argc) {
      const std::string backend = argv[++i];
      if (!ParseObjc3CliIrObjectBackend(backend, options.ir_object_backend)) {
        error = "invalid --objc3-ir-object-backend (expected clang|llvm-direct): " + backend;
        return false;
      }
    } else if (flag == "--llvm-capabilities-summary" && i + 1 < argc) {
      options.llvm_capabilities_summary = argv[++i];
    } else if (flag == "--objc3-route-backend-from-capabilities") {
      options.route_backend_from_capabilities = true;
    } else if (flag == "--objc3-max-message-args" && i + 1 < argc) {
      const std::string value = argv[++i];
      std::size_t parsed = 0;
      if (!ParseObjc3MessageSendArgLimit(value, parsed)) {
        error = "invalid --objc3-max-message-args (expected integer 0-" +
                std::to_string(kObjc3CliMaxMessageSendArgs) + "): " + value;
        return false;
      }
      options.max_message_send_args = parsed;
    } else if (flag == "--objc3-runtime-dispatch-symbol" && i + 1 < argc) {
      const std::string symbol = argv[++i];
      if (!objc3c::support::IsValidRuntimeDispatchSymbol(symbol)) {
        error = "invalid --objc3-runtime-dispatch-symbol (expected [A-Za-z_.$][A-Za-z0-9_.$]*): " + symbol;
        return false;
      }
      options.runtime_dispatch_symbol = symbol;
    } else {
      error = "unknown arg: " + flag;
      return false;
    }
  }

  if (options.command_mode == Objc3CliCommandMode::kValidateConformance) {
    if (!options.input.empty()) {
      error =
          "--validate-objc3-conformance is a validation-only mode and does not accept a source input";
      return false;
    }
    if (options.validate_conformance_report_path.empty()) {
      error = "missing --validate-objc3-conformance <report.json> path";
      return false;
    }
  } else if (options.input.empty()) {
    error = Objc3CliUsage();
    return false;
  }

  if (options.emit_objc3_conformance_format != "json") {
    error =
        "invalid --emit-objc3-conformance-format (expected json): " +
        options.emit_objc3_conformance_format;
    return false;
  }

  if (options.command_mode == Objc3CliCommandMode::kCompile &&
      !objc3c::config::IsCanonicalLanguageVersion(options.language_version)) {
    error = objc3c::config::UnsupportedLanguageVersionDiagnostic(
        options.language_version);
    return false;
  }

  return true;
}

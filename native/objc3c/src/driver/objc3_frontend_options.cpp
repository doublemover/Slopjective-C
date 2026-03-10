#include "driver/objc3_frontend_options.h"

Objc3FrontendOptions BuildObjc3FrontendOptions(const Objc3CliOptions &cli_options) {
  Objc3FrontendOptions options;
  options.language_version = static_cast<std::uint8_t>(cli_options.language_version);
  options.compatibility_mode = cli_options.compat_mode == Objc3CompatMode::kLegacy
                                   ? Objc3FrontendCompatibilityMode::kLegacy
                                   : Objc3FrontendCompatibilityMode::kCanonical;
  options.migration_assist = cli_options.migration_assist;
  options.emit_manifest = true;
  options.emit_ir = true;
  options.emit_object = true;
  options.bootstrap_registration_order_ordinal =
      cli_options.bootstrap_registration_order_ordinal;
  for (const auto &path : cli_options.imported_runtime_surface_paths) {
    options.imported_runtime_surface_paths.push_back(path.generic_string());
  }
  options.lowering.max_message_send_args = cli_options.max_message_send_args;
  options.lowering.runtime_dispatch_symbol = cli_options.runtime_dispatch_symbol;
  return options;
}

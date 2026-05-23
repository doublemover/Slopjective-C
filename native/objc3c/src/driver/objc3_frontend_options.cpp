#include "driver/objc3_frontend_options.h"

#include "config/objc3_language_profile.h"

Objc3FrontendOptions BuildObjc3FrontendOptions(const Objc3CliOptions &cli_options) {
  Objc3FrontendOptions options;
  options.language_version =
      static_cast<std::uint8_t>(objc3c::config::kCanonicalLanguageVersion);
  options.language_profile = Objc3FrontendLanguageProfile::kCanonical;
  options.arc_mode = cli_options.arc_mode == Objc3ArcMode::kEnabled
                         ? Objc3FrontendArcMode::kEnabled
                         : Objc3FrontendArcMode::kDisabled;
  options.emit_manifest = true;
  options.emit_ir = true;
  options.emit_object = true;
  options.allow_live_error_runtime_surface =
      cli_options.allow_live_error_runtime_surface || options.emit_ir ||
      options.emit_object;
  options.bootstrap_registration_order_ordinal =
      cli_options.bootstrap_registration_order_ordinal;
  options.metaprogramming_cache_root_relative_path =
      cli_options.metaprogramming_cache_root.generic_string();
  for (const auto &path : cli_options.imported_runtime_surface_paths) {
    options.imported_runtime_surface_paths.push_back(path.generic_string());
  }
  options.lowering.max_message_send_args = cli_options.max_message_send_args;
  options.lowering.runtime_dispatch_symbol = cli_options.runtime_dispatch_symbol;
  return options;
}

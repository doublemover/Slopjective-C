#include "libobjc3c_frontend/objc3c_frontend_compile_contract.h"

namespace objc3c::frontend {

Objc3FrontendOptions BuildFrontendPipelineOptions(
    const objc3c_frontend_compile_options_t &options) {
  Objc3FrontendOptions frontend_options;
  frontend_options.language_version = options.language_version;
  frontend_options.language_profile = Objc3FrontendLanguageProfile::kCanonical;
  frontend_options.emit_manifest = options.emit_manifest != 0;
  frontend_options.emit_ir = options.emit_ir != 0;
  frontend_options.emit_object = options.emit_object != 0;
  frontend_options.allow_live_error_runtime_surface =
      options.allow_live_error_runtime_surface != 0;
  if (options.translation_unit_registration_order_ordinal > 0) {
    frontend_options.bootstrap_registration_order_ordinal =
        options.translation_unit_registration_order_ordinal;
  }
  if (options.max_message_send_args > 0) {
    frontend_options.lowering.max_message_send_args =
        options.max_message_send_args;
  }
  if (!IsMissingFrontendBorrowedText(options.runtime_dispatch_symbol)) {
    frontend_options.lowering.runtime_dispatch_symbol =
        options.runtime_dispatch_symbol;
  }
  return frontend_options;
}

}  // namespace objc3c::frontend

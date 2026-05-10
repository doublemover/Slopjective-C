#include "tools/objc3c_frontend_c_api_runner_compile_options_fields.h"

void ApplyFrontendCApiRunnerCompileRuntimeOptions(
    objc3c_frontend_c_compile_options_t &compile_options,
    const FrontendCApiRunnerOptions &runner_options) {
  compile_options.runtime_dispatch_symbol =
      runner_options.runtime_dispatch_symbol.empty()
          ? nullptr
          : runner_options.runtime_dispatch_symbol.c_str();
  compile_options.max_message_send_args = runner_options.max_message_send_args;
  compile_options.translation_unit_registration_order_ordinal =
      runner_options.translation_unit_registration_order_ordinal;
}

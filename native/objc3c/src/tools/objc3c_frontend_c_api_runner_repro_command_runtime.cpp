#include "tools/objc3c_frontend_c_api_runner_repro_command_segments.h"

#include <string>

#include "tools/objc3c_frontend_c_api_runner_shell_quote.h"

void AppendFrontendCApiRunnerReproRuntimeArgs(
    std::ostream &command,
    const FrontendCApiRunnerOptions &options) {
  if (options.max_message_send_args != 0) {
    command << " --objc3-max-message-args "
            << std::to_string(options.max_message_send_args);
  }
  if (!options.runtime_dispatch_symbol.empty()) {
    command << " --objc3-runtime-dispatch-symbol "
            << QuoteFrontendCApiRunnerPowerShellArg(
                   options.runtime_dispatch_symbol);
  }
  if (options.translation_unit_registration_order_ordinal != 0) {
    command << " --objc3-bootstrap-registration-order-ordinal "
            << std::to_string(
                   options.translation_unit_registration_order_ordinal);
  }
}

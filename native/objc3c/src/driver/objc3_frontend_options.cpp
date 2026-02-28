#include "driver/objc3_frontend_options.h"

Objc3FrontendOptions BuildObjc3FrontendOptions(const Objc3CliOptions &cli_options) {
  Objc3FrontendOptions options;
  options.lowering.max_message_send_args = cli_options.max_message_send_args;
  options.lowering.runtime_dispatch_symbol = cli_options.runtime_dispatch_symbol;
  return options;
}

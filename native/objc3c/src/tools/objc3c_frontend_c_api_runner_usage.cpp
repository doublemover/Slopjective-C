#include "tools/objc3c_frontend_c_api_runner_usage.h"

#include <string>

#include "tools/objc3c_frontend_c_api_runner_option_values.h"

std::string FrontendCApiRunnerUsage() {
  return "usage: objc3c-frontend-c-api-runner <input> [--out-dir <dir>] [--emit-prefix <name>] "
         "[--clang <path>] [--llc <path>] [--summary-out <path>] [--objc3-max-message-args <0-" +
         std::to_string(kFrontendCApiRunnerMaxMessageSendArgs) +
         ">] [--objc3-runtime-dispatch-symbol <symbol>] "
         "[--objc3-enable-live-error-runtime-surface] "
         "[--objc3-bootstrap-registration-order-ordinal <positive-int>] "
         "[--objc3-ir-object-backend <clang|llvm-direct>] "
         "[--no-emit-manifest] [--no-emit-ir] [--no-emit-object] "
         "[--dump-summary-json] [--dump-observability-json] [--dump-playground-repro-json] "
         "[--dump-runtime-inspector-json] [--dump-stage-trace-json]";
}

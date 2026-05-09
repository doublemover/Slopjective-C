#include "tools/objc3c_frontend_c_api_runner_public_result_backend.h"

const char *FrontendCApiRunnerPublicResultBackendName(
    const FrontendCApiRunnerOptions &options) {
  return options.ir_object_backend == OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT
             ? "llvm-direct"
             : "clang";
}

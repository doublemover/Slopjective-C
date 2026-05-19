#include "tools/objc3c_frontend_c_api_runner_option_values.h"

#include "support/objc3_ir_object_backend_token.h"

bool ParseFrontendCApiRunnerIrObjectBackend(
    const std::string &value,
    objc3c_frontend_c_ir_object_backend_t &backend) {
  objc3c::support::IrObjectBackendToken token;
  if (!objc3c::support::ParseIrObjectBackendToken(value, token)) {
    return false;
  }
  if (token == objc3c::support::IrObjectBackendToken::Clang) {
    backend = OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG;
    return true;
  }
  backend = OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT;
  return true;
}

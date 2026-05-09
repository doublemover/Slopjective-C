#include "driver/objc3_cli_ir_backend_options.h"

#include "support/objc3_ir_object_backend_token.h"

bool ParseObjc3CliIrObjectBackend(const std::string &value,
                                  Objc3IrObjectBackend &backend) {
  objc3c::support::IrObjectBackendToken token;
  if (!objc3c::support::ParseIrObjectBackendToken(value, token)) {
    return false;
  }
  if (token == objc3c::support::IrObjectBackendToken::Clang) {
    backend = Objc3IrObjectBackend::kClang;
    return true;
  }
  backend = Objc3IrObjectBackend::kLLVMDirect;
  return true;
}

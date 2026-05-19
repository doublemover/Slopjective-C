#include "support/objc3_ir_object_backend_parser.h"

namespace objc3c::support {

bool ParseIrObjectBackendToken(std::string_view value,
                               IrObjectBackendToken &backend) {
  if (value == "clang") {
    backend = IrObjectBackendToken::Clang;
    return true;
  }
  if (value == "llvm-direct") {
    backend = IrObjectBackendToken::LLVMDirect;
    return true;
  }
  return false;
}

}  // namespace objc3c::support

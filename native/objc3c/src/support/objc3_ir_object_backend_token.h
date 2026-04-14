#pragma once

#include <string_view>

namespace objc3c::support {

enum class IrObjectBackendToken {
  Clang,
  LLVMDirect,
};

inline bool ParseIrObjectBackendToken(std::string_view value, IrObjectBackendToken &backend) {
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

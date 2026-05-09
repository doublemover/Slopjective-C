#pragma once

#include <string_view>

namespace objc3c::support {

enum class IrObjectBackendToken {
  Clang,
  LLVMDirect,
};

bool ParseIrObjectBackendToken(std::string_view value,
                               IrObjectBackendToken &backend);

}  // namespace objc3c::support

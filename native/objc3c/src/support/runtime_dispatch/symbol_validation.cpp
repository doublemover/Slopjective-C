#include "support/runtime_dispatch/symbol_validation.h"

#include <cstddef>

#include "support/objc3_runtime_dispatch_symbol_characters.h"

namespace objc3c::support {

bool IsValidRuntimeDispatchSymbol(std::string_view symbol) {
  if (symbol.empty() || !IsRuntimeDispatchSymbolStart(symbol.front())) {
    return false;
  }
  for (std::size_t index = 1u; index < symbol.size(); ++index) {
    if (!IsRuntimeDispatchSymbolBody(symbol[index])) {
      return false;
    }
  }
  return true;
}

}  // namespace objc3c::support

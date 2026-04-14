#pragma once

#include <string_view>

#include "support/objc3_ascii_predicates.h"

namespace objc3c::support {

inline bool IsRuntimeDispatchSymbolStart(char c) {
  return IsAsciiAlpha(c) || c == '_' || c == '$' || c == '.';
}

inline bool IsRuntimeDispatchSymbolBody(char c) {
  return IsAsciiAlphaNumeric(c) || c == '_' || c == '$' || c == '.';
}

inline bool IsValidRuntimeDispatchSymbol(std::string_view symbol) {
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

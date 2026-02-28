#include "lower/objc3_lowering_contract.h"

#include <cctype>
#include <string>

namespace {

bool IsRuntimeDispatchSymbolStart(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '$' || c == '.';
}

bool IsRuntimeDispatchSymbolBody(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '$' || c == '.';
}

}  // namespace

bool IsValidRuntimeDispatchSymbol(const std::string &symbol) {
  if (symbol.empty() || !IsRuntimeDispatchSymbolStart(symbol[0])) {
    return false;
  }
  for (std::size_t i = 1; i < symbol.size(); ++i) {
    if (!IsRuntimeDispatchSymbolBody(symbol[i])) {
      return false;
    }
  }
  return true;
}

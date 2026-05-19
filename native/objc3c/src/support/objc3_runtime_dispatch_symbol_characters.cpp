#include "support/objc3_runtime_dispatch_symbol_characters.h"

#include "support/objc3_ascii_predicates.h"

namespace objc3c::support {

bool IsRuntimeDispatchSymbolStart(char c) {
  return IsAsciiAlpha(c) || c == '_' || c == '$' || c == '.';
}

bool IsRuntimeDispatchSymbolBody(char c) {
  return IsAsciiAlphaNumeric(c) || c == '_' || c == '$' || c == '.';
}

}  // namespace objc3c::support

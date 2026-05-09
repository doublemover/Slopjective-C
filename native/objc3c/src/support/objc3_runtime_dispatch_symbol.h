#pragma once

#include <string_view>

namespace objc3c::support {

bool IsRuntimeDispatchSymbolStart(char c);
bool IsRuntimeDispatchSymbolBody(char c);
bool IsValidRuntimeDispatchSymbol(std::string_view symbol);

}  // namespace objc3c::support

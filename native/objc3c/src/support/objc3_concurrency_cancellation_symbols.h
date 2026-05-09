#pragma once

#include <string_view>

namespace objc3c::support {

bool IsConcurrencyCancellationCheckSymbol(std::string_view symbol);
bool IsConcurrencyCancellationHandlerSymbol(std::string_view symbol);

}  // namespace objc3c::support

#pragma once

#include <string_view>

namespace objc3c::support {

bool IsConcurrencyTaskCreationSymbol(std::string_view symbol);
bool IsConcurrencyDetachedTaskCreationSymbol(std::string_view symbol);

}  // namespace objc3c::support

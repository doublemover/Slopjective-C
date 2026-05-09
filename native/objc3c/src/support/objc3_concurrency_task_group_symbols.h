#pragma once

#include <string_view>

namespace objc3c::support {

bool IsConcurrencyTaskGroupScopeSymbol(std::string_view symbol);
bool IsConcurrencyTaskGroupAddTaskSymbol(std::string_view symbol);
bool IsConcurrencyTaskGroupWaitNextSymbol(std::string_view symbol);
bool IsConcurrencyTaskGroupCancelAllSymbol(std::string_view symbol);

}  // namespace objc3c::support

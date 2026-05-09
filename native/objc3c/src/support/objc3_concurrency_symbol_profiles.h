#pragma once

#include <string>
#include <string_view>

namespace objc3c::support {

std::string BuildConcurrencyLowercaseProfileToken(std::string_view token);
bool IsConcurrencyTaskCreationSymbol(std::string_view symbol);
bool IsConcurrencyDetachedTaskCreationSymbol(std::string_view symbol);
bool IsConcurrencyTaskGroupScopeSymbol(std::string_view symbol);
bool IsConcurrencyTaskGroupAddTaskSymbol(std::string_view symbol);
bool IsConcurrencyTaskGroupWaitNextSymbol(std::string_view symbol);
bool IsConcurrencyTaskGroupCancelAllSymbol(std::string_view symbol);
bool IsConcurrencyCancellationCheckSymbol(std::string_view symbol);
bool IsConcurrencyCancellationHandlerSymbol(std::string_view symbol);

}  // namespace objc3c::support

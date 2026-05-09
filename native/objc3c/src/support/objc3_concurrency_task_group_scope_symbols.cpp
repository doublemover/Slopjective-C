#include "support/objc3_concurrency_task_group_scope_symbols.h"

#include "support/objc3_profile_token_match.h"

namespace objc3c::support {

bool IsConcurrencyTaskGroupScopeSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(symbol,
                                          {"with_task_group",
                                           "task_group_scope"});
}

}  // namespace objc3c::support

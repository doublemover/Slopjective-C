#include "support/objc3_concurrency_task_group_add_task_symbols.h"

#include "support/objc3_profile_token_match.h"

namespace objc3c::support {

bool IsConcurrencyTaskGroupAddTaskSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(symbol,
                                          {"task_group_add_task",
                                           "group_add_task"});
}

}  // namespace objc3c::support

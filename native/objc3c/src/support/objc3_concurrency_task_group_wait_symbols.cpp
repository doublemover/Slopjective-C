#include "support/objc3_concurrency_task_group_wait_symbols.h"

#include "support/objc3_profile_token_match.h"

namespace objc3c::support {

bool IsConcurrencyTaskGroupWaitNextSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(symbol,
                                          {"task_group_wait_next",
                                           "group_wait_next", "wait_next"});
}

}  // namespace objc3c::support

#include "support/objc3_concurrency_task_group_cancel_symbols.h"

#include "support/objc3_profile_token_match.h"

namespace objc3c::support {

bool IsConcurrencyTaskGroupCancelAllSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(
             symbol,
             {"task_group_cancel_all", "group_cancel_all", "cancel_all"});
}

}  // namespace objc3c::support

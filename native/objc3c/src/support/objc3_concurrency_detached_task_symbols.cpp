#include "support/objc3_concurrency_detached_task_symbols.h"

#include "support/objc3_profile_token_match.h"

namespace objc3c::support {

bool IsConcurrencyDetachedTaskCreationSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(symbol,
                                          {"detached_task", "task_detach"});
}

}  // namespace objc3c::support

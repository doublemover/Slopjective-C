#include "support/objc3_concurrency_task_symbols.h"

#include "support/objc3_profile_token_match.h"

namespace objc3c::support {

bool IsConcurrencyTaskCreationSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(
             symbol,
             {"task_spawn", "spawn_task", "detached_task", "task_detach"});
}

bool IsConcurrencyDetachedTaskCreationSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(symbol,
                                          {"detached_task", "task_detach"});
}

}  // namespace objc3c::support

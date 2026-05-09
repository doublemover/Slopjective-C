#include "support/objc3_concurrency_symbol_profiles.h"

#include "support/objc3_string_predicates.h"

namespace objc3c::support {

std::string BuildConcurrencyLowercaseProfileToken(std::string_view token) {
  return LowercaseAscii(token);
}

bool IsConcurrencyTaskCreationSymbol(std::string_view symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildConcurrencyLowercaseProfileToken(symbol);
  return lowered.find("task_spawn") != std::string::npos ||
         lowered.find("spawn_task") != std::string::npos ||
         lowered.find("detached_task") != std::string::npos ||
         lowered.find("task_detach") != std::string::npos;
}

bool IsConcurrencyDetachedTaskCreationSymbol(std::string_view symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildConcurrencyLowercaseProfileToken(symbol);
  return lowered.find("detached_task") != std::string::npos ||
         lowered.find("task_detach") != std::string::npos;
}

bool IsConcurrencyTaskGroupScopeSymbol(std::string_view symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildConcurrencyLowercaseProfileToken(symbol);
  return lowered.find("with_task_group") != std::string::npos ||
         lowered.find("task_group_scope") != std::string::npos;
}

bool IsConcurrencyTaskGroupAddTaskSymbol(std::string_view symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildConcurrencyLowercaseProfileToken(symbol);
  return lowered.find("task_group_add_task") != std::string::npos ||
         lowered.find("group_add_task") != std::string::npos;
}

bool IsConcurrencyTaskGroupWaitNextSymbol(std::string_view symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildConcurrencyLowercaseProfileToken(symbol);
  return lowered.find("task_group_wait_next") != std::string::npos ||
         lowered.find("group_wait_next") != std::string::npos ||
         lowered.find("wait_next") != std::string::npos;
}

bool IsConcurrencyTaskGroupCancelAllSymbol(std::string_view symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildConcurrencyLowercaseProfileToken(symbol);
  return lowered.find("task_group_cancel_all") != std::string::npos ||
         lowered.find("group_cancel_all") != std::string::npos ||
         lowered.find("cancel_all") != std::string::npos;
}

bool IsConcurrencyCancellationCheckSymbol(std::string_view symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildConcurrencyLowercaseProfileToken(symbol);
  return lowered.find("cancelled") != std::string::npos ||
         lowered.find("is_cancelled") != std::string::npos ||
         lowered.find("cancellation") != std::string::npos;
}

bool IsConcurrencyCancellationHandlerSymbol(std::string_view symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildConcurrencyLowercaseProfileToken(symbol);
  return lowered.find("on_cancel") != std::string::npos ||
         lowered.find("cancel_handler") != std::string::npos ||
         lowered.find("with_cancellation_handler") != std::string::npos;
}

}  // namespace objc3c::support

#include "support/objc3_concurrency_cancellation_symbols.h"

#include "support/objc3_profile_token_match.h"

namespace objc3c::support {

bool IsConcurrencyCancellationCheckSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(
             symbol, {"cancelled", "is_cancelled", "cancellation"});
}

bool IsConcurrencyCancellationHandlerSymbol(std::string_view symbol) {
  return !symbol.empty() &&
         LowercaseProfileContainsAnyToken(
             symbol,
             {"on_cancel", "cancel_handler",
              "with_cancellation_handler"});
}

}  // namespace objc3c::support

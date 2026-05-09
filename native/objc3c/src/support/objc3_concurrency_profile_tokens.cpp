#include "support/objc3_concurrency_profile_tokens.h"

#include "support/objc3_string_predicates.h"

namespace objc3c::support {

std::string BuildConcurrencyLowercaseProfileToken(std::string_view token) {
  return LowercaseAscii(token);
}

}  // namespace objc3c::support

#include "runtime/errors/error_bridge_catch_operations.h"

#include "runtime/errors/catch_filter.h"
#include "runtime/errors/error_bridge_kind.h"
#include "runtime/errors/error_bridge_state.h"

namespace objc3c::runtime {

int RuntimeCatchMatchesErrorI32(int error_value, int catch_kind,
                                int catch_all) {
  RuntimeErrorBridgeState &state = RuntimeErrorBridgeThreadState();
  ++state.catch_match_call_count;
  state.last_catch_match_error_value = error_value;
  state.last_catch_match_kind = catch_kind;
  state.last_catch_match_is_catch_all = catch_all != 0 ? 1 : 0;
  state.last_catch_kind_name = RuntimeErrorCatchKindName(catch_kind);
  const int matches =
      RuntimeCatchFilterMatches(error_value, catch_kind, catch_all) ? 1 : 0;
  state.last_catch_match_result = matches;
  return matches;
}

}  // namespace objc3c::runtime

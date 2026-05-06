#include "runtime/errors/catch_filter.h"

namespace objc3c::runtime {

bool RuntimeCatchFilterCanMatch(int error_value, int catch_all) {
  return catch_all != 0 || error_value != 0;
}

bool RuntimeCatchKindIsSupported(int catch_kind) {
  return catch_kind == 1 || catch_kind == 2;
}

bool RuntimeCatchFilterMatches(int error_value, int catch_kind, int catch_all) {
  if (catch_all != 0) {
    return true;
  }
  return RuntimeCatchFilterCanMatch(error_value, catch_all) &&
         RuntimeCatchKindIsSupported(catch_kind);
}

}  // namespace objc3c::runtime

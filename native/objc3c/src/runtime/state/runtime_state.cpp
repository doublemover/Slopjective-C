#include "runtime/state/runtime_state.h"

namespace objc3c::runtime {

const char *RuntimeStateLockDiscipline() {
  return "single-process-runtime-state-guarded-by-runtime-mutex";
}

const char *RuntimeStateOwnershipModel() {
  return "runtime-state-owns-registrations-selectors-class-graph-storage-and-dispatch-caches";
}

}  // namespace objc3c::runtime

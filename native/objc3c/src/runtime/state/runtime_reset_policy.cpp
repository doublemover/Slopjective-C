#include "runtime/state/runtime_reset_policy.h"

namespace objc3c::runtime {

bool RuntimeResetPreservesBootstrapCatalog() {
  return true;
}

bool RuntimeResetClearsLiveExecutionState() {
  return true;
}

}  // namespace objc3c::runtime

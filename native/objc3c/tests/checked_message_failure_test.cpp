#define OBJC3_RUNTIME_SERVICE_FAILURE_ONLY
#include "runtime/public/objc3_task.hpp"
#undef OBJC3_RUNTIME_SERVICE_FAILURE_ONLY

#include <cassert>

// The compiler bridge is a C-linkage symbol with native C++ exception identity.
// Linking this test must extract no Objective-C runtime archive members.
int main() {
  bool caught = false;
  try {
    objc3_checked_nil_result();
  } catch (const objc3::service_failure &failure) {
    assert(failure.status() == OBJC3_SERVICE_INVALID_ARGUMENT);
    caught = true;
  }
  assert(caught);
}

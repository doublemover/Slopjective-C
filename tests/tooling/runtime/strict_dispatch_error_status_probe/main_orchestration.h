#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_MAIN_ORCHESTRATION_H_

#include "orchestration.h"

namespace objc3c::runtime::strict_dispatch_error_status_probe {

inline int RunProbe() {
  return RunStrictDispatchErrorStatusScenarios();
}

}  // namespace objc3c::runtime::strict_dispatch_error_status_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_STRICT_DISPATCH_ERROR_STATUS_PROBE_MAIN_ORCHESTRATION_H_

#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_PROBE_HELPER_OUTPUT_EQUIVALENCE_TEST_MAIN_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_PROBE_HELPER_OUTPUT_EQUIVALENCE_TEST_MAIN_ORCHESTRATION_H_

#include "equivalence_assertion_helpers.h"

namespace objc3c::runtime::probe::runtime_probe_helper_output_equivalence {

inline int RunTestMain() {
  int result = VerifyRepresentativeJsonOutput();
  if (result != 0) {
    return result;
  }

  result = VerifyLabeledMethodCacheStateOutput();
  if (result != 0) {
    return result;
  }

  result = VerifyLabeledFastPathMethodCacheStateOutput();
  if (result != 0) {
    return result;
  }

  return VerifyLabeledDispatchStateOutput();
}

}  // namespace objc3c::runtime::probe::runtime_probe_helper_output_equivalence

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_PROBE_HELPER_OUTPUT_EQUIVALENCE_TEST_MAIN_ORCHESTRATION_H_

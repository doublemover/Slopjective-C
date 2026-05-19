#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_PROBE_HELPER_OUTPUT_EQUIVALENCE_TEST_EQUIVALENCE_ASSERTION_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_PROBE_HELPER_OUTPUT_EQUIVALENCE_TEST_EQUIVALENCE_ASSERTION_HELPERS_H_

#include "expected_output_fixtures.h"
#include "serialization_report_helpers.h"
#include "../support/output_expectations.h"

namespace objc3c::runtime::probe::runtime_probe_helper_output_equivalence {

inline int VerifyRepresentativeJsonOutput() {
  return ::objc3c::runtime::probe::ExpectTextEqual(
      SerializeRepresentativeJsonObject(), kRepresentativeJsonOutput,
      "representative JSON helper output", 1);
}

inline int VerifyLabeledMethodCacheStateOutput() {
  return ::objc3c::runtime::probe::ExpectTextEqual(
      SerializeLabeledMethodCacheState(), kLabeledMethodCacheStateOutput,
      "labeled method cache state output", 2);
}

inline int VerifyLabeledFastPathMethodCacheStateOutput() {
  return ::objc3c::runtime::probe::ExpectTextEqual(
      SerializeLabeledFastPathMethodCacheState(),
      kLabeledFastPathMethodCacheStateOutput,
      "labeled fast-path cache state output", 3);
}

inline int VerifyLabeledDispatchStateOutput() {
  return ::objc3c::runtime::probe::ExpectTextEqual(
      SerializeLabeledDispatchState(), kLabeledDispatchStateOutput,
      "labeled dispatch state output", 4);
}

}  // namespace objc3c::runtime::probe::runtime_probe_helper_output_equivalence

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_RUNTIME_PROBE_HELPER_OUTPUT_EQUIVALENCE_TEST_EQUIVALENCE_ASSERTION_HELPERS_H_

#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_SEMANTICS_PROBE_ORCHESTRATION_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_SEMANTICS_PROBE_ORCHESTRATION_H_

#include "fixture_setup.h"
#include "report_helpers.h"
#include "runtime_initialization_helpers.h"
#include "semantic_assertions.h"

#include <iostream>

namespace objc3c::runtime::bootstrap_semantics_probe {

inline void PrintBootstrapSemanticsReport(
    const BootstrapFixture &fixture,
    const BootstrapSemanticsAssertions &assertions) {
  using objc3c::runtime::probe::JsonEscape;

  std::cout << "{"
            << "\"success_status\":" << assertions.success.status << ","
            << "\"duplicate_status\":" << assertions.duplicate.status << ","
            << "\"out_of_order_status\":" << assertions.out_of_order.status
            << ","
            << "\"invalid_status\":" << assertions.invalid.status << ","
            << "\"translation_unit_identity_key\":\""
            << JsonEscape(fixture.identity_key) << "\","
            << "\"translation_unit_registration_order_ordinal\":"
            << fixture.order << ","
            << "\"snapshots\":{";
  PrintSnapshot("after_success", assertions.success.snapshot.snapshot, true);
  PrintSnapshot("after_duplicate", assertions.duplicate.snapshot.snapshot,
                true);
  PrintSnapshot("after_out_of_order",
                assertions.out_of_order.snapshot.snapshot, true);
  PrintSnapshot("after_invalid", assertions.invalid.snapshot.snapshot, false);
  std::cout << "}}\n";
}

inline int RunBootstrapSemanticsProbe(int argc, char **argv) {
  if (argc != 9) {
    return kUsageError;
  }

  BootstrapFixture fixture;
  if (!ParseBootstrapFixture(argc, argv, fixture)) {
    return kArgumentParseError;
  }

  ResetRuntimeForProbe();
  const BootstrapSemanticsAssertions assertions =
      RunBootstrapSemanticAssertions(fixture);

  if (assertions.success.snapshot.copy_status != 0) {
    return kAfterSuccessSnapshotError;
  }
  if (assertions.duplicate.snapshot.copy_status != 0) {
    return kAfterDuplicateSnapshotError;
  }
  if (assertions.out_of_order.snapshot.copy_status != 0) {
    return kAfterOutOfOrderSnapshotError;
  }
  if (assertions.invalid.snapshot.copy_status != 0) {
    return kAfterInvalidSnapshotError;
  }

  PrintBootstrapSemanticsReport(fixture, assertions);
  return 0;
}

}  // namespace objc3c::runtime::bootstrap_semantics_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_SEMANTICS_PROBE_ORCHESTRATION_H_

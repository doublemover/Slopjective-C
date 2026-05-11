#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_SEMANTICS_PROBE_SEMANTIC_ASSERTIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_SEMANTICS_PROBE_SEMANTIC_ASSERTIONS_H_

#include "fixture_setup.h"
#include "runtime_initialization_helpers.h"
#include "runtime/public/objc3_runtime_api.h"

namespace objc3c::runtime::bootstrap_semantics_probe {

struct RegistrationAssertionResult {
  int status = 0;
  SnapshotCapture snapshot;
};

struct BootstrapSemanticsAssertions {
  RegistrationAssertionResult success;
  RegistrationAssertionResult duplicate;
  RegistrationAssertionResult out_of_order;
  RegistrationAssertionResult invalid;
};

inline RegistrationAssertionResult AssertRegistrationSemantics(
    const objc3_runtime_image_descriptor &image) {
  return {
      objc3_runtime_register_image(&image),
      CaptureRegistrationSnapshot(),
  };
}

inline BootstrapSemanticsAssertions RunBootstrapSemanticAssertions(
    const BootstrapFixture &fixture) {
  BootstrapSemanticsAssertions assertions;
  const objc3_runtime_image_descriptor success_image = fixture.SuccessImage();
  assertions.success = AssertRegistrationSemantics(success_image);

  const objc3_runtime_image_descriptor duplicate_image =
      fixture.DuplicateImage();
  assertions.duplicate = AssertRegistrationSemantics(duplicate_image);

  const objc3_runtime_image_descriptor out_of_order_image =
      fixture.OutOfOrderImage();
  assertions.out_of_order = AssertRegistrationSemantics(out_of_order_image);

  const objc3_runtime_image_descriptor invalid_image = fixture.InvalidImage();
  assertions.invalid = AssertRegistrationSemantics(invalid_image);

  return assertions;
}

}  // namespace objc3c::runtime::bootstrap_semantics_probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_BOOTSTRAP_SEMANTICS_PROBE_SEMANTIC_ASSERTIONS_H_

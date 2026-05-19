#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_RUNTIME_ASSERTION_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_RUNTIME_ASSERTION_HELPERS_H_

#include "object_model_fixture_setup.h"
#include "probe_result.h"

namespace objc3c::runtime::probe::object_model_lookup_reflection_runtime {

inline void CaptureWidgetRuntimeDispatchAssertions(
    const ObjectModelFixture &fixture, RuntimeDispatchAssertions &assertions) {
  assertions = RuntimeDispatchAssertions{};
  assertions.traced_value = objc3_runtime_dispatch_i32(
      fixture.initialized_widget, kTracedValueSelector, 0, 0, 0, 0);
  (void)objc3_runtime_dispatch_i32(fixture.initialized_widget,
                                   kSetCountSelector, kWrittenCountValue, 0, 0,
                                   0);
  assertions.count_value = objc3_runtime_dispatch_i32(
      fixture.initialized_widget, kCountPropertyName, 0, 0, 0, 0);
}

}  // namespace objc3c::runtime::probe::object_model_lookup_reflection_runtime

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_RUNTIME_ASSERTION_HELPERS_H_

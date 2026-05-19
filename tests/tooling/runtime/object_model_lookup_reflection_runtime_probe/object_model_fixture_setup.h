#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_OBJECT_MODEL_FIXTURE_SETUP_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_OBJECT_MODEL_FIXTURE_SETUP_H_

#include "probe_result.h"

namespace objc3c::runtime::probe::object_model_lookup_reflection_runtime {

inline constexpr const char *kWidgetClassName = "Widget";
inline constexpr const char *kTracerProtocolName = "Tracer";
inline constexpr const char *kCountPropertyName = "count";
inline constexpr const char *kAllocSelector = "alloc";
inline constexpr const char *kInitSelector = "init";
inline constexpr const char *kTracedValueSelector = "tracedValue";
inline constexpr const char *kSetCountSelector = "setCount:";
inline constexpr int kWrittenCountValue = 41;

inline int WidgetClassReceiverIdentity(
    const objc3_runtime_realized_class_entry_snapshot &widget_entry) {
  return static_cast<int>(widget_entry.base_identity + 2U);
}

inline void CaptureObjectModelFixture(ObjectModelFixture &fixture) {
  fixture = ObjectModelFixture{};
  (void)objc3_runtime_copy_realized_class_entry_for_testing(
      kWidgetClassName, &fixture.widget_class.snapshot);
  fixture.widget_class_receiver =
      WidgetClassReceiverIdentity(fixture.widget_class.snapshot);
  fixture.widget_instance = objc3_runtime_dispatch_i32(
      fixture.widget_class_receiver, kAllocSelector, 0, 0, 0, 0);
  fixture.initialized_widget = objc3_runtime_dispatch_i32(
      fixture.widget_instance, kInitSelector, 0, 0, 0, 0);
}

}  // namespace objc3c::runtime::probe::object_model_lookup_reflection_runtime

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_OBJECT_MODEL_LOOKUP_REFLECTION_RUNTIME_PROBE_OBJECT_MODEL_FIXTURE_SETUP_H_

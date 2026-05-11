#pragma once

namespace objc3c {
namespace tooling {
namespace method_cache_slow_path_probe {

constexpr int kWidgetClassObjectId = 1024;
constexpr int kWidgetInstanceClassId = 1025;

constexpr const char *kCurrentValueSelector = "currentValue";
constexpr const char *kSharedSelector = "shared";
constexpr const char *kStrictErrorSelector = "missingRuntimeSelector:";

extern "C" int objc3_method_Widget_instance_callCurrentValue(void);
extern "C" int objc3_method_Widget_class_callSharedThroughSelf(void);
extern "C" int callSharedThroughKnownClass(void);

inline int CallWidgetCurrentValue() {
  return objc3_method_Widget_instance_callCurrentValue();
}

inline int CallWidgetSharedThroughSelf() {
  return objc3_method_Widget_class_callSharedThroughSelf();
}

inline int CallWidgetSharedThroughKnownClass() {
  return callSharedThroughKnownClass();
}

} // namespace method_cache_slow_path_probe
} // namespace tooling
} // namespace objc3c

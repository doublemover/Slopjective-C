#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_FIXTURE_DEFINITIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_FIXTURE_DEFINITIONS_H_

namespace objc3c::runtime::probe::instance_allocation_runtime {

inline constexpr int kWidgetClassReceiver = 1024;
inline constexpr const char *kWidgetClassName = "Widget";
inline constexpr const char *kAllocSelector = "alloc";
inline constexpr const char *kCountGetterSelector = "count";
inline constexpr const char *kCountSetterSelector = "setCount:";
inline constexpr const char *kEnabledGetterSelector = "enabled";
inline constexpr const char *kEnabledSetterSelector = "setEnabled:";
inline constexpr const char *kValueGetterSelector = "value";
inline constexpr const char *kValueSetterSelector = "setValue:";

inline constexpr int kFirstCountValue = 37;
inline constexpr int kFirstEnabledValue = 1;
inline constexpr int kFirstStoredValue = 55;
inline constexpr int kSecondCountValue = 9;
inline constexpr int kSecondStoredValue = 91;

}  // namespace objc3c::runtime::probe::instance_allocation_runtime

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_FIXTURE_DEFINITIONS_H_

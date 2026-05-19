#pragma once

namespace objc3c {
namespace tooling {
namespace method_binding_probe {

struct RuntimeDispatch {
  int receiver = 0;
  const char *selector = nullptr;
  int arg0 = 0;
  int arg1 = 0;
  int arg2 = 0;
  int arg3 = 0;
};

struct MethodCacheEntryKey {
  int receiver = 0;
  const char *selector = nullptr;
};

constexpr int kWidgetClassObjectReceiver = 1024;
constexpr int kWidgetInstanceReceiver = 1025;
constexpr int kWidgetMetaclassReceiver = 1026;

constexpr const char *kValueExtraSelector = "value:extra:";
constexpr const char *kClassValueSelector = "classValue";
constexpr const char *kTracedValueSelector = "tracedValue";

constexpr RuntimeDispatch kInstanceValueDispatch{
    kWidgetInstanceReceiver, kValueExtraSelector, 7, 8, 0, 0};
constexpr RuntimeDispatch kClassValueDispatch{
    kWidgetMetaclassReceiver, kClassValueSelector, 0, 0, 0, 0};
constexpr RuntimeDispatch kKnownClassValueDispatch{
    kWidgetClassObjectReceiver, kClassValueSelector, 0, 0, 0, 0};
constexpr RuntimeDispatch kCategoryValueDispatch{
    kWidgetInstanceReceiver, kTracedValueSelector, 0, 0, 0, 0};

constexpr MethodCacheEntryKey kInstanceValueEntry{
    kWidgetInstanceReceiver, kValueExtraSelector};
constexpr MethodCacheEntryKey kClassValueEntry{
    kWidgetClassObjectReceiver, kClassValueSelector};
constexpr MethodCacheEntryKey kCategoryValueEntry{
    kWidgetInstanceReceiver, kTracedValueSelector};

constexpr int kExpectedInstanceValue = 20;
constexpr int kExpectedClassValue = 44;
constexpr int kExpectedCategoryValue = 33;

} // namespace method_binding_probe
} // namespace tooling
} // namespace objc3c

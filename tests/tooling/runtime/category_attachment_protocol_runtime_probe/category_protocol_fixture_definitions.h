#pragma once

namespace objc3c::runtime::probe::category_attachment_protocol_runtime {

struct RuntimeDispatch {
  int receiver = 0;
  const char *selector = nullptr;
};

inline constexpr const char *kWidgetClassName = "Widget";
inline constexpr const char *kBaseClassName = "Base";
inline constexpr const char *kDerivedClassName = "Derived";
inline constexpr const char *kWorkerProtocolName = "Worker";
inline constexpr const char *kTracerProtocolName = "Tracer";

inline constexpr const char *kCategorySelector = "tracedValue";
inline constexpr const char *kClassSelector = "classValue";
inline constexpr const char *kProtocolStrictErrorSelector = "ignoredValue";

}  // namespace objc3c::runtime::probe::category_attachment_protocol_runtime

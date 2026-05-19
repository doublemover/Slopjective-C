#pragma once

namespace objc3c::runtime::probe::category_attachment_protocol_runtime {

struct RuntimeDispatch {
  int receiver;
  const char *selector;
};

inline constexpr const char *kWidgetClassName = "Widget";
inline constexpr const char *kBaseClassName = "Base";
inline constexpr const char *kWorkerProtocolName = "Worker";
inline constexpr const char *kTracerProtocolName = "Tracer";

inline constexpr RuntimeDispatch kCategoryDispatch{1042, "tracedValue"};
inline constexpr RuntimeDispatch kClassDispatch{1043, "classValue"};
inline constexpr RuntimeDispatch kProtocolStrictErrorDispatch{
    1042, "ignoredValue"};

}  // namespace objc3c::runtime::probe::category_attachment_protocol_runtime

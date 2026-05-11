#pragma once

namespace objc3c::runtime::probe::protocol_category_runtime {

struct RuntimeDispatch {
  int receiver;
  const char *selector;
};

inline constexpr RuntimeDispatch kInstanceDispatch{1025, "tracedValue"};
inline constexpr RuntimeDispatch kClassSelfDispatch{1026, "tracerClassValue"};
inline constexpr RuntimeDispatch kKnownClassDispatch{1024, "tracerClassValue"};
inline constexpr RuntimeDispatch kStrictErrorDispatch{1025,
                                                      "protocolDeclaredOnly"};

}  // namespace objc3c::runtime::probe::protocol_category_runtime

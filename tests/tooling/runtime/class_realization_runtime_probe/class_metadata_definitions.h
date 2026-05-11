#pragma once

namespace objc3c::runtime::probe::class_realization_runtime {

struct RuntimeDispatch {
  int receiver;
  const char *selector;
};

inline constexpr RuntimeDispatch kInheritedMethodDispatch{1042,
                                                          "inheritedValue"};
inline constexpr RuntimeDispatch kCategoryMethodDispatch{1042, "tracedValue"};
inline constexpr RuntimeDispatch kClassMethodDispatch{1043, "classValue"};
inline constexpr RuntimeDispatch kKnownClassMethodDispatch{1041,
                                                           "classValue"};
inline constexpr RuntimeDispatch kProtocolStrictErrorDispatch{1042,
                                                              "ignoredValue"};

}  // namespace objc3c::runtime::probe::class_realization_runtime

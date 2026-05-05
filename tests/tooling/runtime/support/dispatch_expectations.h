#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_DISPATCH_EXPECTATIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_DISPATCH_EXPECTATIONS_H_

namespace objc3c::runtime::probe {

inline constexpr int kStrictDispatchErrorI32 = 0;

inline int ExpectedStrictDispatchErrorValue(int receiver, const char *selector,
                                            int a0, int a1, int a2, int a3) {
  (void)receiver;
  (void)selector;
  (void)a0;
  (void)a1;
  (void)a2;
  (void)a3;
  return kStrictDispatchErrorI32;
}

inline int ComputeFallbackDispatch(int receiver, const char *selector, int a0,
                                   int a1, int a2, int a3) {
  return ExpectedStrictDispatchErrorValue(receiver, selector, a0, a1, a2, a3);
}

inline int ExpectedDispatch(int receiver, const char *selector, int a0, int a1,
                            int a2, int a3) {
  return ExpectedStrictDispatchErrorValue(receiver, selector, a0, a1, a2, a3);
}

} // namespace objc3c::runtime::probe

#endif // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_DISPATCH_EXPECTATIONS_H_

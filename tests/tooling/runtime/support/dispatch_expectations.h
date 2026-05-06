#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_DISPATCH_EXPECTATIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_DISPATCH_EXPECTATIONS_H_

namespace objc3c::runtime::probe {

inline constexpr int kStrictDispatchErrorValueI32 = 0;

inline int ExpectedStrictDispatchErrorValue(int receiver, const char *selector,
                                            int a0, int a1, int a2, int a3) {
  (void)receiver;
  (void)selector;
  (void)a0;
  (void)a1;
  (void)a2;
  (void)a3;
  return kStrictDispatchErrorValueI32;
}

inline int ExpectedDispatch(int receiver, const char *selector, int a0, int a1,
                            int a2, int a3) {
  return ExpectedStrictDispatchErrorValue(receiver, selector, a0, a1, a2, a3);
}

inline bool IsStrictDispatchError(
    const objc3_runtime_dispatch_i32_result &result,
    objc3_runtime_dispatch_status_code expected_status) {
  return result.status_code == expected_status && result.value == 0 &&
         result.diagnostic_code != nullptr &&
         result.diagnostic_code[0] != '\0' &&
         result.diagnostic_message != nullptr &&
         result.diagnostic_message[0] != '\0';
}

} // namespace objc3c::runtime::probe

#endif // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_DISPATCH_EXPECTATIONS_H_

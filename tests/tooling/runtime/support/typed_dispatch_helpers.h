#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c::runtime::probe {

inline objc3_runtime_dispatch_typed_result DispatchTyped(
    int receiver, const char *selector, int a0 = 0, int a1 = 0, int a2 = 0,
    int a3 = 0) {
  return objc3_runtime_dispatch_typed_checked(receiver, selector, a0, a1, a2,
                                             a3);
}

inline int DispatchTypedStatus(int receiver, const char *selector, int a0 = 0,
                               int a1 = 0, int a2 = 0, int a3 = 0) {
  return DispatchTyped(receiver, selector, a0, a1, a2, a3).status_code;
}

inline int DispatchTypedObjectReference(int receiver, const char *selector,
                                        int a0 = 0, int a1 = 0, int a2 = 0,
                                        int a3 = 0) {
  const objc3_runtime_dispatch_typed_result result =
      DispatchTyped(receiver, selector, a0, a1, a2, a3);
  return result.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK
             ? result.object_reference
             : 0;
}

inline int DispatchTypedBoolValue(int receiver, const char *selector,
                                  int a0 = 0, int a1 = 0, int a2 = 0,
                                  int a3 = 0) {
  const objc3_runtime_dispatch_typed_result result =
      DispatchTyped(receiver, selector, a0, a1, a2, a3);
  return result.status_code == OBJC3_RUNTIME_DISPATCH_STATUS_OK
             ? result.bool_value
             : 0;
}

}  // namespace objc3c::runtime::probe

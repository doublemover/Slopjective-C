#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_ALLOCATION_SETUP_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_ALLOCATION_SETUP_H_

#include "fixture_definitions.h"
#include "probe_state.h"
#include "support/typed_dispatch_helpers.h"

namespace objc3c::runtime::probe::instance_allocation_runtime {

inline int DispatchWidgetMessage(int receiver, const char *selector,
                                 int argument = 0) {
  return objc3_runtime_dispatch_i32(receiver, selector, argument, 0, 0, 0);
}

inline int DispatchWidgetObjectReference(int receiver, const char *selector) {
  return ::objc3c::runtime::probe::DispatchTypedObjectReference(receiver,
                                                                selector);
}

inline int DispatchWidgetVoidStatus(int receiver, const char *selector,
                                    int argument) {
  return ::objc3c::runtime::probe::DispatchTypedStatus(receiver, selector,
                                                       argument);
}

inline int DispatchWidgetBoolValue(int receiver, const char *selector) {
  return ::objc3c::runtime::probe::DispatchTypedBoolValue(receiver, selector);
}

inline AllocationFixture AllocateWidgetInstances() {
  AllocationFixture fixture;
  fixture.first_alloc =
      DispatchWidgetObjectReference(kWidgetClassReceiver, kAllocSelector);
  fixture.second_alloc =
      DispatchWidgetObjectReference(kWidgetClassReceiver, kAllocSelector);
  return fixture;
}

}  // namespace objc3c::runtime::probe::instance_allocation_runtime

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_ALLOCATION_SETUP_H_

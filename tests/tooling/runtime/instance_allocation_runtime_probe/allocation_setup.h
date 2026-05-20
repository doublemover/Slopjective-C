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

inline objc3_runtime_dispatch_typed_result DispatchWidgetTyped(
    int receiver, const char *selector) {
  return ::objc3c::runtime::probe::DispatchTyped(receiver, selector);
}

inline int DispatchWidgetVoidStatus(int receiver, const char *selector,
                                    int argument) {
  return ::objc3c::runtime::probe::DispatchTypedStatus(receiver, selector,
                                                       argument);
}

inline int DispatchWidgetBoolValue(int receiver, const char *selector) {
  return ::objc3c::runtime::probe::DispatchTypedBoolValue(receiver, selector);
}

inline int WidgetClassReceiverIdentity() {
  objc3_runtime_realized_class_entry_snapshot widget_entry{};
  (void)objc3_runtime_copy_realized_class_entry_for_testing(kWidgetClassName,
                                                            &widget_entry);
  return static_cast<int>(widget_entry.class_receiver_identity);
}

inline AllocationFixture AllocateWidgetInstances() {
  AllocationFixture fixture;
  const int widget_class_receiver = WidgetClassReceiverIdentity();
  fixture.first_alloc =
      DispatchWidgetObjectReference(widget_class_receiver, kAllocSelector);
  fixture.second_alloc =
      DispatchWidgetObjectReference(widget_class_receiver, kAllocSelector);
  fixture.first_init_result =
      DispatchWidgetTyped(fixture.first_alloc, kInitSelector);
  fixture.initialized_new_result =
      DispatchWidgetTyped(widget_class_receiver, kNewSelector);
  if (fixture.initialized_new_result.status_code ==
      OBJC3_RUNTIME_DISPATCH_STATUS_OK) {
    fixture.initialized_new =
        fixture.initialized_new_result.object_reference;
  }
  fixture.double_init_result =
      DispatchWidgetTyped(fixture.first_alloc, kInitSelector);
  return fixture;
}

}  // namespace objc3c::runtime::probe::instance_allocation_runtime

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_ALLOCATION_SETUP_H_

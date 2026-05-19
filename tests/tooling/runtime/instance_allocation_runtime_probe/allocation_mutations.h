#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_ALLOCATION_MUTATIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_ALLOCATION_MUTATIONS_H_

#include "allocation_setup.h"
#include "fixture_definitions.h"
#include "probe_state.h"

namespace objc3c::runtime::probe::instance_allocation_runtime {

inline AllocationMutationResults CaptureAllocationMutationResults(
    const AllocationFixture &fixture) {
  AllocationMutationResults results;

  results.set_count_first = DispatchWidgetVoidStatus(
      fixture.first_alloc, kCountSetterSelector, kFirstCountValue);
  results.count_value_first =
      DispatchWidgetMessage(fixture.first_alloc, kCountGetterSelector);
  results.count_value_second_before =
      DispatchWidgetMessage(fixture.second_alloc, kCountGetterSelector);

  results.set_enabled_first = DispatchWidgetVoidStatus(
      fixture.first_alloc, kEnabledSetterSelector, kFirstEnabledValue);
  results.enabled_value_first =
      DispatchWidgetBoolValue(fixture.first_alloc, kEnabledGetterSelector);
  results.enabled_value_second =
      DispatchWidgetBoolValue(fixture.second_alloc, kEnabledGetterSelector);

  results.set_value_first = DispatchWidgetVoidStatus(
      fixture.first_alloc, kValueSetterSelector, kFirstStoredValue);
  results.value_result_first =
      DispatchWidgetObjectReference(fixture.first_alloc, kValueGetterSelector);
  results.value_result_second_before =
      DispatchWidgetObjectReference(fixture.second_alloc, kValueGetterSelector);

  results.set_count_second = DispatchWidgetVoidStatus(
      fixture.second_alloc, kCountSetterSelector, kSecondCountValue);
  results.count_value_first_after_second =
      DispatchWidgetMessage(fixture.first_alloc, kCountGetterSelector);
  results.count_value_second_after =
      DispatchWidgetMessage(fixture.second_alloc, kCountGetterSelector);

  results.set_value_second = DispatchWidgetVoidStatus(
      fixture.second_alloc, kValueSetterSelector, kSecondStoredValue);
  results.value_result_first_after_second =
      DispatchWidgetObjectReference(fixture.first_alloc, kValueGetterSelector);
  results.value_result_second_after =
      DispatchWidgetObjectReference(fixture.second_alloc, kValueGetterSelector);

  return results;
}

}  // namespace objc3c::runtime::probe::instance_allocation_runtime

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_ALLOCATION_MUTATIONS_H_

#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_REPORT_HELPERS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_REPORT_HELPERS_H_

#include "probe_state.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>

namespace objc3c::runtime::probe::instance_allocation_runtime {

inline void PrintInstanceEntryAllocation(
    const objc3_runtime_instance_entry_snapshot &snapshot) {
  std::printf("{");
  ::objc3c::runtime::probe::PrintIntField("found", snapshot.found);
  ::objc3c::runtime::probe::PrintUint64Field(
      "receiver_identity",
      static_cast<unsigned long long>(snapshot.receiver_identity));
  ::objc3c::runtime::probe::PrintUint64Field(
      "base_identity", static_cast<unsigned long long>(snapshot.base_identity));
  ::objc3c::runtime::probe::PrintUint64Field(
      "normalized_receiver_identity",
      static_cast<unsigned long long>(snapshot.normalized_receiver_identity));
  ::objc3c::runtime::probe::PrintUint64Field(
      "class_receiver_identity",
      static_cast<unsigned long long>(snapshot.class_receiver_identity));
  ::objc3c::runtime::probe::PrintUint64Field(
      "allocation_ordinal",
      static_cast<unsigned long long>(snapshot.allocation_ordinal));
  ::objc3c::runtime::probe::PrintUint64Field(
      "initialized", static_cast<unsigned long long>(snapshot.initialized));
  ::objc3c::runtime::probe::PrintUint64Field(
      "initialization_ordinal",
      static_cast<unsigned long long>(snapshot.initialization_ordinal));
  ::objc3c::runtime::probe::PrintUint64Field(
      "instance_size_bytes",
      static_cast<unsigned long long>(snapshot.instance_size_bytes));
  ::objc3c::runtime::probe::PrintUint64Field(
      "storage_size_bytes",
      static_cast<unsigned long long>(snapshot.storage_size_bytes));
  ::objc3c::runtime::probe::PrintUint64Field(
      "zero_initialized_storage_byte_count",
      static_cast<unsigned long long>(
          snapshot.zero_initialized_storage_byte_count));
  ::objc3c::runtime::probe::PrintUint64Field(
      "retain_count", static_cast<unsigned long long>(snapshot.retain_count));
  ::objc3c::runtime::probe::PrintStringField("class_name",
                                             snapshot.class_name);
  ::objc3c::runtime::probe::PrintStringField(
      "class_owner_identity", snapshot.class_owner_identity);
  ::objc3c::runtime::probe::PrintStringField(
      "metaclass_owner_identity", snapshot.metaclass_owner_identity);
  ::objc3c::runtime::probe::PrintStringField(
      "instance_isa_owner_identity", snapshot.instance_isa_owner_identity);
  ::objc3c::runtime::probe::PrintStringField(
      "class_object_isa_owner_identity",
      snapshot.class_object_isa_owner_identity, false);
  std::printf("}");
}

inline void PrintTypedDispatchResult(
    const objc3_runtime_dispatch_typed_result &result) {
  using ::objc3c::runtime::probe::PrintIntField;
  using ::objc3c::runtime::probe::PrintStringField;

  std::printf("{");
  PrintIntField("abi_version", static_cast<int>(result.abi_version));
  PrintIntField("result_size", static_cast<int>(result.result_size));
  PrintIntField("status_code", static_cast<int>(result.status_code));
  PrintIntField("return_kind", static_cast<int>(result.return_kind));
  PrintStringField("return_kind_name", result.return_kind_name);
  PrintIntField("i32_value", result.i32_value);
  PrintIntField("bool_value", result.bool_value);
  PrintIntField("object_reference", result.object_reference);
  PrintIntField("class_reference", result.class_reference);
  PrintIntField("selector_reference", result.selector_reference);
  PrintIntField("protocol_reference", result.protocol_reference);
  PrintStringField("diagnostic_code", result.diagnostic_code);
  PrintStringField("diagnostic_message", result.diagnostic_message);
  PrintStringField("result_contract", result.result_contract);
  PrintStringField("diagnostic_owner_model", result.diagnostic_owner_model);
  PrintStringField("fail_closed_ownership_model",
                   result.fail_closed_ownership_model, false);
  std::printf("}");
}

inline void PrintInstanceAllocationRuntimeReport(const ProbeRun &run) {
  const AllocationFixture &fixture = run.fixture;
  const AllocationMutationResults &mutations = run.mutations;
  const AllocationInvariantSnapshots &invariants = run.invariants;

  std::printf("{");
  std::printf("\"first_alloc\":%d,", fixture.first_alloc);
  std::printf("\"second_alloc\":%d,", fixture.second_alloc);
  std::printf("\"initialized_new\":%d,", fixture.initialized_new);
  std::printf("\"first_init_result\":");
  PrintTypedDispatchResult(fixture.first_init_result);
  std::printf(",\"initialized_new_result\":");
  PrintTypedDispatchResult(fixture.initialized_new_result);
  std::printf(",\"double_init_result\":");
  PrintTypedDispatchResult(fixture.double_init_result);
  std::printf(",");
  std::printf("\"set_base_count_first\":%d,",
              mutations.set_base_count_first);
  std::printf("\"base_count_value_first\":%d,",
              mutations.base_count_value_first);
  std::printf("\"base_count_value_second_before\":%d,",
              mutations.base_count_value_second_before);
  std::printf("\"set_count_first\":%d,", mutations.set_count_first);
  std::printf("\"count_value_first\":%d,", mutations.count_value_first);
  std::printf("\"count_value_second_before\":%d,",
              mutations.count_value_second_before);
  std::printf("\"set_enabled_first\":%d,", mutations.set_enabled_first);
  std::printf("\"enabled_value_first\":%d,", mutations.enabled_value_first);
  std::printf("\"enabled_value_second\":%d,", mutations.enabled_value_second);
  std::printf("\"set_value_first\":%d,", mutations.set_value_first);
  std::printf("\"value_result_first\":%d,", mutations.value_result_first);
  std::printf("\"value_result_second_before\":%d,",
              mutations.value_result_second_before);
  std::printf("\"set_count_second\":%d,", mutations.set_count_second);
  std::printf("\"count_value_first_after_second\":%d,",
              mutations.count_value_first_after_second);
  std::printf("\"count_value_second_after\":%d,",
              mutations.count_value_second_after);
  std::printf("\"set_base_count_second\":%d,",
              mutations.set_base_count_second);
  std::printf("\"base_count_value_first_after_second\":%d,",
              mutations.base_count_value_first_after_second);
  std::printf("\"base_count_value_second_after\":%d,",
              mutations.base_count_value_second_after);
  std::printf("\"set_value_second\":%d,", mutations.set_value_second);
  std::printf("\"value_result_first_after_second\":%d,",
              mutations.value_result_first_after_second);
  std::printf("\"value_result_second_after\":%d,",
              mutations.value_result_second_after);
  std::printf("\"registration_state\":");
  ::objc3c::runtime::probe::PrintRegistrationStateBasic(
      run.registration_state.state);
  std::printf(",\"selector_table_state\":");
  ::objc3c::runtime::probe::PrintSelectorTableStateBasic(
      run.selector_table_state.state);
  std::printf(",\"graph_state\":");
  ::objc3c::runtime::probe::PrintRealizedGraphStateAllocation(
      invariants.graph_state.state);
  std::printf(",\"base_entry\":");
  ::objc3c::runtime::probe::PrintRealizedClassEntryAllocation(
      invariants.base_entry.entry);
  std::printf(",\"widget_entry\":");
  ::objc3c::runtime::probe::PrintRealizedClassEntryAllocation(
      invariants.widget_entry.entry);
  std::printf(",\"first_instance\":");
  PrintInstanceEntryAllocation(invariants.first_instance.entry);
  std::printf(",\"second_instance\":");
  PrintInstanceEntryAllocation(invariants.second_instance.entry);
  std::printf(",\"initialized_new_instance\":");
  PrintInstanceEntryAllocation(invariants.initialized_new_instance.entry);
  std::printf(",\"base_count_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryFull(
      invariants.base_count_property.entry);
  std::printf(",\"count_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryFull(
      invariants.count_property.entry);
  std::printf(",\"value_property\":");
  ::objc3c::runtime::probe::PrintPropertyEntryFull(
      invariants.value_property.entry);
  std::printf(",\"base_count_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      invariants.base_count_entry.entry);
  std::printf(",\"set_base_count_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      invariants.set_base_count_entry.entry);
  std::printf(",\"count_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      invariants.count_entry.entry);
  std::printf(",\"set_count_entry\":");
  ::objc3c::runtime::probe::PrintMethodCacheEntryBasic(
      invariants.set_count_entry.entry);
  std::printf("}");
}

}  // namespace objc3c::runtime::probe::instance_allocation_runtime

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_INSTANCE_ALLOCATION_RUNTIME_PROBE_REPORT_HELPERS_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_json.h"
#include "support/runtime_snapshot_stabilizers.h"

#include <cstdio>
#include <string>

namespace {

using objc3c::runtime::probe::PrintConformanceQueryCanonicalSummary;
using objc3c::runtime::probe::PrintPropertyEntryCanonicalSummary;
using objc3c::runtime::probe::PrintRealizedClassEntryCanonicalSummary;

using objc3c::runtime::probe::StabilizeConformanceQuery;
using objc3c::runtime::probe::StabilizeNullableCString;
using objc3c::runtime::probe::StabilizePropertyEntry;
using objc3c::runtime::probe::StabilizeRealizedClassEntry;

using objc3c::runtime::probe::PrintJsonStringOrNull;

} // namespace

int main() {
  objc3_runtime_realized_class_entry_snapshot widget_entry{};
  (void)objc3_runtime_copy_realized_class_entry_for_testing("Widget",
                                                            &widget_entry);

  std::string widget_module_storage;
  std::string widget_identity_storage;
  std::string widget_class_storage;
  std::string widget_class_owner_storage;
  std::string widget_metaclass_owner_storage;
  std::string widget_super_class_storage;
  std::string widget_super_metaclass_storage;
  std::string widget_category_owner_storage;
  std::string widget_category_name_storage;
  StabilizeRealizedClassEntry(
      widget_entry, widget_module_storage, widget_identity_storage,
      widget_class_storage, widget_class_owner_storage,
      widget_metaclass_owner_storage, widget_super_class_storage,
      widget_super_metaclass_storage, widget_category_owner_storage,
      widget_category_name_storage);

  const int widget_class_receiver =
      static_cast<int>(widget_entry.base_identity + 2U);
  const int widget_instance =
      objc3_runtime_dispatch_i32(widget_class_receiver, "alloc", 0, 0, 0, 0);
  const int init_value =
      objc3_runtime_dispatch_i32(widget_instance, "init", 0, 0, 0, 0);
  const int traced_value =
      objc3_runtime_dispatch_i32(init_value, "tracedValue", 0, 0, 0, 0);
  const int inherited_value =
      objc3_runtime_dispatch_i32(init_value, "inheritedValue", 0, 0, 0, 0);
  const int class_value = objc3_runtime_dispatch_i32(widget_class_receiver,
                                                     "classValue", 0, 0, 0, 0);
  const int shared_value =
      objc3_runtime_dispatch_i32(widget_class_receiver, "shared", 0, 0, 0, 0);
  (void)objc3_runtime_dispatch_i32(init_value, "setCount:", 37, 0, 0, 0);
  const int count_value =
      objc3_runtime_dispatch_i32(init_value, "count", 0, 0, 0, 0);
  (void)objc3_runtime_dispatch_i32(init_value, "setEnabled:", 1, 0, 0, 0);
  const int enabled_value =
      objc3_runtime_dispatch_i32(init_value, "enabled", 0, 0, 0, 0);
  (void)objc3_runtime_dispatch_i32(init_value, "setCurrentValue:", 55, 0, 0, 0);
  const int current_value =
      objc3_runtime_dispatch_i32(init_value, "currentValue", 0, 0, 0, 0);
  const int token_value =
      objc3_runtime_dispatch_i32(init_value, "tokenValue", 0, 0, 0, 0);

  objc3_runtime_protocol_conformance_query_snapshot worker_query{};
  objc3_runtime_protocol_conformance_query_snapshot tracer_query{};
  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      "Widget", "Worker", &worker_query);
  (void)objc3_runtime_copy_protocol_conformance_query_for_testing(
      "Widget", "Tracer", &tracer_query);

  std::string worker_class_storage;
  std::string worker_protocol_storage;
  std::string worker_protocol_owner_storage;
  std::string worker_attachment_owner_storage;
  std::string tracer_class_storage;
  std::string tracer_protocol_storage;
  std::string tracer_protocol_owner_storage;
  std::string tracer_attachment_owner_storage;
  StabilizeConformanceQuery(
      worker_query, worker_class_storage, worker_protocol_storage,
      worker_protocol_owner_storage, worker_attachment_owner_storage);
  StabilizeConformanceQuery(
      tracer_query, tracer_class_storage, tracer_protocol_storage,
      tracer_protocol_owner_storage, tracer_attachment_owner_storage);

  objc3_runtime_property_entry_snapshot count_property{};
  objc3_runtime_property_entry_snapshot value_property{};
  objc3_runtime_property_entry_snapshot token_property{};
  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "count",
                                                      &count_property);
  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "value",
                                                      &value_property);
  (void)objc3_runtime_copy_property_entry_for_testing("Widget", "token",
                                                      &token_property);

  std::string property_storage[45];
  StabilizePropertyEntry(
      count_property, property_storage[0], property_storage[1],
      property_storage[2], property_storage[3], property_storage[4],
      property_storage[5], property_storage[6], property_storage[7],
      property_storage[8], property_storage[9], property_storage[10],
      property_storage[11], property_storage[12], property_storage[13]);
  StabilizePropertyEntry(
      value_property, property_storage[14], property_storage[15],
      property_storage[16], property_storage[17], property_storage[18],
      property_storage[19], property_storage[20], property_storage[21],
      property_storage[22], property_storage[23], property_storage[24],
      property_storage[25], property_storage[26], property_storage[27]);
  StabilizePropertyEntry(
      token_property, property_storage[28], property_storage[29],
      property_storage[30], property_storage[31], property_storage[32],
      property_storage[33], property_storage[34], property_storage[35],
      property_storage[36], property_storage[37], property_storage[38],
      property_storage[39], property_storage[40], property_storage[41]);

  std::printf("{");
  std::printf("\"widget_entry\":");
  PrintRealizedClassEntryCanonicalSummary(widget_entry);
  std::printf(",\"init_value\":%d", init_value);
  std::printf(",\"traced_value\":%d", traced_value);
  std::printf(",\"inherited_value\":%d", inherited_value);
  std::printf(",\"class_value\":%d", class_value);
  std::printf(",\"shared_value\":%d", shared_value);
  std::printf(",\"count_value\":%d", count_value);
  std::printf(",\"enabled_value\":%d", enabled_value);
  std::printf(",\"current_value\":%d", current_value);
  std::printf(",\"token_value\":%d", token_value);
  std::printf(",\"worker_query\":");
  PrintConformanceQueryCanonicalSummary(worker_query);
  std::printf(",\"tracer_query\":");
  PrintConformanceQueryCanonicalSummary(tracer_query);
  std::printf(",\"count_property\":");
  PrintPropertyEntryCanonicalSummary(count_property);
  std::printf(",\"value_property\":");
  PrintPropertyEntryCanonicalSummary(value_property);
  std::printf(",\"token_property\":");
  PrintPropertyEntryCanonicalSummary(token_property);
  std::printf("}\n");
  return 0;
}

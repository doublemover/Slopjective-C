#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "support/json_probe_writer.h"
#include "support/runtime_snapshot_stabilizers.h"
#include "support/runtime_snapshot_json.h"

#include <cstdio>
#include <string>

namespace {

using objc3c::runtime::probe::PrintMethodCacheEntryMetaclassMinimal;
using objc3c::runtime::probe::PrintMethodCacheStateMetaclass;
using objc3c::runtime::probe::PrintRegistrationStateBasic;

using objc3c::runtime::probe::StabilizeMethodCacheEntry;
using objc3c::runtime::probe::StabilizeMethodCacheState;
using objc3c::runtime::probe::StabilizeNullableCString;
using objc3c::runtime::probe::StabilizeRealizedEntry;
using objc3c::runtime::probe::StabilizeRealizedGraphState;
using objc3c::runtime::probe::StabilizeRegistrationState;

using objc3c::runtime::probe::PrintJsonStringOrNull;


void PrintRealizedGraphState(
    const objc3_runtime_realized_class_graph_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"realized_class_count\":%llu,",
              static_cast<unsigned long long>(snapshot.realized_class_count));
  std::printf("\"root_class_count\":%llu,",
              static_cast<unsigned long long>(snapshot.root_class_count));
  std::printf("\"metaclass_edge_count\":%llu,",
              static_cast<unsigned long long>(snapshot.metaclass_edge_count));
  std::printf("\"receiver_class_binding_count\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.receiver_class_binding_count));
  std::printf("\"last_realized_class_name\":");
  PrintJsonStringOrNull(snapshot.last_realized_class_name);
  std::printf(",\"last_realized_class_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_realized_class_owner_identity);
  std::printf(",\"last_realized_metaclass_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_realized_metaclass_owner_identity);
  std::printf("}");
}

void PrintRealizedEntry(
    const objc3_runtime_realized_class_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"base_identity\":%llu,",
              static_cast<unsigned long long>(snapshot.base_identity));
  std::printf("\"registration_order_ordinal\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.registration_order_ordinal));
  std::printf("\"is_root_class\":%d,", snapshot.is_root_class);
  std::printf("\"implementation_backed\":%d,",
              snapshot.implementation_backed);
  std::printf("\"module_name\":");
  PrintJsonStringOrNull(snapshot.module_name);
  std::printf(",\"translation_unit_identity_key\":");
  PrintJsonStringOrNull(snapshot.translation_unit_identity_key);
  std::printf(",\"class_name\":");
  PrintJsonStringOrNull(snapshot.class_name);
  std::printf(",\"class_owner_identity\":");
  PrintJsonStringOrNull(snapshot.class_owner_identity);
  std::printf(",\"metaclass_owner_identity\":");
  PrintJsonStringOrNull(snapshot.metaclass_owner_identity);
  std::printf(",\"super_class_owner_identity\":");
  PrintJsonStringOrNull(snapshot.super_class_owner_identity);
  std::printf(",\"super_metaclass_owner_identity\":");
  PrintJsonStringOrNull(snapshot.super_metaclass_owner_identity);
  std::printf("}");
}

}  // namespace

int main() {
  objc3_runtime_registration_state_snapshot registration_state{};
  objc3_runtime_realized_class_graph_state_snapshot graph_state{};
  objc3_runtime_realized_class_entry_snapshot root_entry{};
  objc3_runtime_realized_class_entry_snapshot widget_entry{};
  objc3_runtime_method_cache_state_snapshot root_class_state{};
  objc3_runtime_method_cache_state_snapshot widget_class_state{};
  objc3_runtime_method_cache_state_snapshot widget_known_class_state{};
  objc3_runtime_method_cache_state_snapshot widget_inherited_state{};
  objc3_runtime_method_cache_state_snapshot widget_own_state{};
  objc3_runtime_method_cache_entry_snapshot root_shared_entry{};
  objc3_runtime_method_cache_entry_snapshot widget_shared_entry{};
  objc3_runtime_method_cache_entry_snapshot widget_inherited_entry{};
  objc3_runtime_method_cache_entry_snapshot widget_own_entry{};

  std::string registration_module_storage;
  std::string registration_identity_storage;
  std::string registration_rejected_module_storage;
  std::string registration_rejected_identity_storage;
  std::string graph_class_storage;
  std::string graph_class_owner_storage;
  std::string graph_metaclass_owner_storage;
  std::string root_module_storage;
  std::string root_identity_storage;
  std::string root_class_storage;
  std::string root_class_owner_storage;
  std::string root_metaclass_owner_storage;
  std::string root_super_class_owner_storage;
  std::string root_super_metaclass_owner_storage;
  std::string widget_module_storage;
  std::string widget_identity_storage;
  std::string widget_class_storage;
  std::string widget_class_owner_storage;
  std::string widget_metaclass_owner_storage;
  std::string widget_super_class_owner_storage;
  std::string widget_super_metaclass_owner_storage;
  std::string root_class_selector_storage;
  std::string root_class_class_storage;
  std::string root_class_owner_storage2;
  std::string widget_class_selector_storage;
  std::string widget_class_class_storage;
  std::string widget_class_owner_storage2;
  std::string widget_known_class_selector_storage;
  std::string widget_known_class_class_storage;
  std::string widget_known_class_owner_storage;
  std::string widget_inherited_selector_storage;
  std::string widget_inherited_class_storage;
  std::string widget_inherited_owner_storage;
  std::string widget_own_selector_storage;
  std::string widget_own_class_storage;
  std::string widget_own_owner_storage;
  std::string root_shared_entry_selector_storage;
  std::string root_shared_entry_class_storage;
  std::string root_shared_entry_owner_storage;
  std::string widget_shared_entry_selector_storage;
  std::string widget_shared_entry_class_storage;
  std::string widget_shared_entry_owner_storage;
  std::string widget_inherited_entry_selector_storage;
  std::string widget_inherited_entry_class_storage;
  std::string widget_inherited_entry_owner_storage;
  std::string widget_own_entry_selector_storage;
  std::string widget_own_entry_class_storage;
  std::string widget_own_entry_owner_storage;

  (void)objc3_runtime_copy_registration_state_for_testing(&registration_state);
  (void)objc3_runtime_copy_realized_class_graph_state_for_testing(&graph_state);
  (void)objc3_runtime_copy_realized_class_entry_for_testing("RootObject",
                                                            &root_entry);
  (void)objc3_runtime_copy_realized_class_entry_for_testing("Widget",
                                                            &widget_entry);
  StabilizeRegistrationState(registration_state, registration_module_storage,
                             registration_identity_storage,
                             registration_rejected_module_storage,
                             registration_rejected_identity_storage);
  StabilizeRealizedGraphState(graph_state, graph_class_storage,
                              graph_class_owner_storage,
                              graph_metaclass_owner_storage);
  StabilizeRealizedEntry(root_entry, root_module_storage, root_identity_storage,
                         root_class_storage, root_class_owner_storage,
                         root_metaclass_owner_storage,
                         root_super_class_owner_storage,
                         root_super_metaclass_owner_storage);
  StabilizeRealizedEntry(widget_entry, widget_module_storage,
                         widget_identity_storage, widget_class_storage,
                         widget_class_owner_storage,
                         widget_metaclass_owner_storage,
                         widget_super_class_owner_storage,
                         widget_super_metaclass_owner_storage);

  const int root_class_value =
      objc3_runtime_dispatch_i32(1026, "shared", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&root_class_state);
  StabilizeMethodCacheState(root_class_state, root_class_selector_storage,
                            root_class_class_storage,
                            root_class_owner_storage2);

  const int widget_class_value =
      objc3_runtime_dispatch_i32(1043, "shared", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&widget_class_state);
  StabilizeMethodCacheState(widget_class_state, widget_class_selector_storage,
                            widget_class_class_storage,
                            widget_class_owner_storage2);

  const int widget_known_class_value =
      objc3_runtime_dispatch_i32(1041, "shared", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &widget_known_class_state);
  StabilizeMethodCacheState(widget_known_class_state,
                            widget_known_class_selector_storage,
                            widget_known_class_class_storage,
                            widget_known_class_owner_storage);

  const int widget_inherited_instance_value =
      objc3_runtime_dispatch_i32(1042, "rootValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(
      &widget_inherited_state);
  StabilizeMethodCacheState(widget_inherited_state,
                            widget_inherited_selector_storage,
                            widget_inherited_class_storage,
                            widget_inherited_owner_storage);

  const int widget_own_instance_value =
      objc3_runtime_dispatch_i32(1042, "widgetValue", 0, 0, 0, 0);
  (void)objc3_runtime_copy_method_cache_state_for_testing(&widget_own_state);
  StabilizeMethodCacheState(widget_own_state, widget_own_selector_storage,
                            widget_own_class_storage,
                            widget_own_owner_storage);

  (void)objc3_runtime_copy_method_cache_entry_for_testing(1026, "shared",
                                                          &root_shared_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(1041, "shared",
                                                          &widget_shared_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(1042, "rootValue",
                                                          &widget_inherited_entry);
  (void)objc3_runtime_copy_method_cache_entry_for_testing(1042, "widgetValue",
                                                          &widget_own_entry);
  StabilizeMethodCacheEntry(root_shared_entry,
                            root_shared_entry_selector_storage,
                            root_shared_entry_class_storage,
                            root_shared_entry_owner_storage);
  StabilizeMethodCacheEntry(widget_shared_entry,
                            widget_shared_entry_selector_storage,
                            widget_shared_entry_class_storage,
                            widget_shared_entry_owner_storage);
  StabilizeMethodCacheEntry(widget_inherited_entry,
                            widget_inherited_entry_selector_storage,
                            widget_inherited_entry_class_storage,
                            widget_inherited_entry_owner_storage);
  StabilizeMethodCacheEntry(widget_own_entry,
                            widget_own_entry_selector_storage,
                            widget_own_entry_class_storage,
                            widget_own_entry_owner_storage);

  std::printf("{");
  std::printf("\"root_class_value\":%d,", root_class_value);
  std::printf("\"widget_class_value\":%d,", widget_class_value);
  std::printf("\"widget_known_class_value\":%d,",
              widget_known_class_value);
  std::printf("\"widget_inherited_instance_value\":%d,",
              widget_inherited_instance_value);
  std::printf("\"widget_own_instance_value\":%d,",
              widget_own_instance_value);
  std::printf("\"registration_state\":");
  PrintRegistrationStateBasic(registration_state);
  std::printf(",\"graph_state\":");
  PrintRealizedGraphState(graph_state);
  std::printf(",\"root_entry\":");
  PrintRealizedEntry(root_entry);
  std::printf(",\"widget_entry\":");
  PrintRealizedEntry(widget_entry);
  std::printf(",\"root_class_state\":");
  PrintMethodCacheStateMetaclass(root_class_state);
  std::printf(",\"widget_class_state\":");
  PrintMethodCacheStateMetaclass(widget_class_state);
  std::printf(",\"widget_known_class_state\":");
  PrintMethodCacheStateMetaclass(widget_known_class_state);
  std::printf(",\"widget_inherited_state\":");
  PrintMethodCacheStateMetaclass(widget_inherited_state);
  std::printf(",\"widget_own_state\":");
  PrintMethodCacheStateMetaclass(widget_own_state);
  std::printf(",\"root_shared_entry\":");
  PrintMethodCacheEntryMetaclassMinimal(root_shared_entry);
  std::printf(",\"widget_shared_entry\":");
  PrintMethodCacheEntryMetaclassMinimal(widget_shared_entry);
  std::printf(",\"widget_inherited_entry\":");
  PrintMethodCacheEntryMetaclassMinimal(widget_inherited_entry);
  std::printf(",\"widget_own_entry\":");
  PrintMethodCacheEntryMetaclassMinimal(widget_own_entry);
  std::printf("}\n");
  return 0;
}

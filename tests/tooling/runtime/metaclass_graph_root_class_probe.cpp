#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <cstdio>
#include <string>

namespace {

void PrintJsonStringOrNull(const char *value) {
  if (value == nullptr) {
    std::printf("null");
    return;
  }
  std::printf("\"");
  for (const unsigned char *cursor =
           reinterpret_cast<const unsigned char *>(value);
       *cursor != 0U; ++cursor) {
    switch (*cursor) {
      case '\\':
        std::printf("\\\\");
        break;
      case '"':
        std::printf("\\\"");
        break;
      case '\n':
        std::printf("\\n");
        break;
      case '\r':
        std::printf("\\r");
        break;
      case '\t':
        std::printf("\\t");
        break;
      default:
        std::printf("%c", static_cast<char>(*cursor));
        break;
    }
  }
  std::printf("\"");
}

void StabilizeNullableCString(const char *source, std::string &storage,
                              const char *&field) {
  storage = source != nullptr ? source : "";
  field = storage.empty() ? nullptr : storage.c_str();
}

void StabilizeRegistrationState(
    objc3_runtime_registration_state_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage,
    std::string &rejected_module_storage,
    std::string &rejected_identity_storage) {
  StabilizeNullableCString(snapshot.last_registered_module_name, module_storage,
                           snapshot.last_registered_module_name);
  StabilizeNullableCString(snapshot.last_registered_translation_unit_identity_key,
                           identity_storage,
                           snapshot.last_registered_translation_unit_identity_key);
  StabilizeNullableCString(snapshot.last_rejected_module_name,
                           rejected_module_storage,
                           snapshot.last_rejected_module_name);
  StabilizeNullableCString(snapshot.last_rejected_translation_unit_identity_key,
                           rejected_identity_storage,
                           snapshot.last_rejected_translation_unit_identity_key);
}

void StabilizeMethodCacheState(
    objc3_runtime_method_cache_state_snapshot &snapshot,
    std::string &selector_storage, std::string &class_storage,
    std::string &owner_storage) {
  StabilizeNullableCString(snapshot.last_selector, selector_storage,
                           snapshot.last_selector);
  StabilizeNullableCString(snapshot.last_resolved_class_name, class_storage,
                           snapshot.last_resolved_class_name);
  StabilizeNullableCString(snapshot.last_resolved_owner_identity, owner_storage,
                           snapshot.last_resolved_owner_identity);
}

void StabilizeMethodCacheEntry(
    objc3_runtime_method_cache_entry_snapshot &snapshot,
    std::string &selector_storage, std::string &class_storage,
    std::string &owner_storage) {
  StabilizeNullableCString(snapshot.selector, selector_storage, snapshot.selector);
  StabilizeNullableCString(snapshot.resolved_class_name, class_storage,
                           snapshot.resolved_class_name);
  StabilizeNullableCString(snapshot.resolved_owner_identity, owner_storage,
                           snapshot.resolved_owner_identity);
}

void StabilizeRealizedGraphState(
    objc3_runtime_realized_class_graph_state_snapshot &snapshot,
    std::string &class_storage, std::string &class_owner_storage,
    std::string &metaclass_owner_storage) {
  StabilizeNullableCString(snapshot.last_realized_class_name, class_storage,
                           snapshot.last_realized_class_name);
  StabilizeNullableCString(snapshot.last_realized_class_owner_identity,
                           class_owner_storage,
                           snapshot.last_realized_class_owner_identity);
  StabilizeNullableCString(snapshot.last_realized_metaclass_owner_identity,
                           metaclass_owner_storage,
                           snapshot.last_realized_metaclass_owner_identity);
}

void StabilizeRealizedEntry(
    objc3_runtime_realized_class_entry_snapshot &snapshot,
    std::string &module_storage, std::string &identity_storage,
    std::string &class_storage, std::string &class_owner_storage,
    std::string &metaclass_owner_storage,
    std::string &super_class_owner_storage,
    std::string &super_metaclass_owner_storage) {
  StabilizeNullableCString(snapshot.module_name, module_storage,
                           snapshot.module_name);
  StabilizeNullableCString(snapshot.translation_unit_identity_key,
                           identity_storage,
                           snapshot.translation_unit_identity_key);
  StabilizeNullableCString(snapshot.class_name, class_storage,
                           snapshot.class_name);
  StabilizeNullableCString(snapshot.class_owner_identity, class_owner_storage,
                           snapshot.class_owner_identity);
  StabilizeNullableCString(snapshot.metaclass_owner_identity,
                           metaclass_owner_storage,
                           snapshot.metaclass_owner_identity);
  StabilizeNullableCString(snapshot.super_class_owner_identity,
                           super_class_owner_storage,
                           snapshot.super_class_owner_identity);
  StabilizeNullableCString(snapshot.super_metaclass_owner_identity,
                           super_metaclass_owner_storage,
                           snapshot.super_metaclass_owner_identity);
}

void PrintRegistrationState(
    const objc3_runtime_registration_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"registered_image_count\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_image_count));
  std::printf("\"registered_descriptor_total\":%llu,",
              static_cast<unsigned long long>(snapshot.registered_descriptor_total));
  std::printf("\"last_registration_status\":%d,",
              snapshot.last_registration_status);
  std::printf("\"last_registered_module_name\":");
  PrintJsonStringOrNull(snapshot.last_registered_module_name);
  std::printf(",\"last_registered_translation_unit_identity_key\":");
  PrintJsonStringOrNull(snapshot.last_registered_translation_unit_identity_key);
  std::printf("}");
}

void PrintMethodCacheState(
    const objc3_runtime_method_cache_state_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"cache_entry_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_entry_count));
  std::printf("\"cache_hit_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_hit_count));
  std::printf("\"cache_miss_count\":%llu,",
              static_cast<unsigned long long>(snapshot.cache_miss_count));
  std::printf("\"slow_path_lookup_count\":%llu,",
              static_cast<unsigned long long>(snapshot.slow_path_lookup_count));
  std::printf("\"live_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.live_dispatch_count));
  std::printf("\"fallback_dispatch_count\":%llu,",
              static_cast<unsigned long long>(snapshot.fallback_dispatch_count));
  std::printf("\"last_normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.last_normalized_receiver_identity));
  std::printf("\"last_dispatch_used_cache\":%d,",
              snapshot.last_dispatch_used_cache);
  std::printf("\"last_dispatch_resolved_live_method\":%d,",
              snapshot.last_dispatch_resolved_live_method);
  std::printf("\"last_dispatch_fell_back\":%d,",
              snapshot.last_dispatch_fell_back);
  std::printf("\"last_selector\":");
  PrintJsonStringOrNull(snapshot.last_selector);
  std::printf(",\"last_resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.last_resolved_class_name);
  std::printf(",\"last_resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.last_resolved_owner_identity);
  std::printf("}");
}

void PrintMethodCacheEntry(
    const objc3_runtime_method_cache_entry_snapshot &snapshot) {
  std::printf("{");
  std::printf("\"found\":%d,", snapshot.found);
  std::printf("\"resolved\":%d,", snapshot.resolved);
  std::printf("\"dispatch_family_is_class\":%d,",
              snapshot.dispatch_family_is_class);
  std::printf("\"normalized_receiver_identity\":%llu,",
              static_cast<unsigned long long>(
                  snapshot.normalized_receiver_identity));
  std::printf("\"selector\":");
  PrintJsonStringOrNull(snapshot.selector);
  std::printf(",\"resolved_class_name\":");
  PrintJsonStringOrNull(snapshot.resolved_class_name);
  std::printf(",\"resolved_owner_identity\":");
  PrintJsonStringOrNull(snapshot.resolved_owner_identity);
  std::printf("}");
}

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
  PrintRegistrationState(registration_state);
  std::printf(",\"graph_state\":");
  PrintRealizedGraphState(graph_state);
  std::printf(",\"root_entry\":");
  PrintRealizedEntry(root_entry);
  std::printf(",\"widget_entry\":");
  PrintRealizedEntry(widget_entry);
  std::printf(",\"root_class_state\":");
  PrintMethodCacheState(root_class_state);
  std::printf(",\"widget_class_state\":");
  PrintMethodCacheState(widget_class_state);
  std::printf(",\"widget_known_class_state\":");
  PrintMethodCacheState(widget_known_class_state);
  std::printf(",\"widget_inherited_state\":");
  PrintMethodCacheState(widget_inherited_state);
  std::printf(",\"widget_own_state\":");
  PrintMethodCacheState(widget_own_state);
  std::printf(",\"root_shared_entry\":");
  PrintMethodCacheEntry(root_shared_entry);
  std::printf(",\"widget_shared_entry\":");
  PrintMethodCacheEntry(widget_shared_entry);
  std::printf(",\"widget_inherited_entry\":");
  PrintMethodCacheEntry(widget_inherited_entry);
  std::printf(",\"widget_own_entry\":");
  PrintMethodCacheEntry(widget_own_entry);
  std::printf("}\n");
  return 0;
}

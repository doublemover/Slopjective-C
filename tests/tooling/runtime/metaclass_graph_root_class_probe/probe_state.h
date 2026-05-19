#pragma once

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c {
namespace runtime {
namespace probe {
namespace metaclass_graph_root_class {

inline constexpr int kRootClassReceiver = 1026;
inline constexpr int kWidgetKnownClassReceiver = 1041;
inline constexpr int kWidgetInstanceReceiver = 1042;
inline constexpr int kWidgetClassReceiver = 1043;
inline constexpr const char *kSharedSelector = "shared";
inline constexpr const char *kRootValueSelector = "rootValue";
inline constexpr const char *kWidgetValueSelector = "widgetValue";
inline constexpr const char *kRootClassName = "RootObject";
inline constexpr const char *kWidgetClassName = "Widget";

struct RegistrationReportStorage {
  std::string module_name;
  std::string translation_unit_identity_key;
  std::string rejected_module_name;
  std::string rejected_translation_unit_identity_key;
};

struct RealizedGraphReportStorage {
  std::string class_name;
  std::string class_owner_identity;
  std::string metaclass_owner_identity;
};

struct RealizedEntryReportStorage {
  std::string module_name;
  std::string translation_unit_identity_key;
  std::string class_name;
  std::string class_owner_identity;
  std::string metaclass_owner_identity;
  std::string super_class_owner_identity;
  std::string super_metaclass_owner_identity;
};

struct MethodCacheStateReportStorage {
  std::string selector;
  std::string class_name;
  std::string owner_identity;
};

struct MethodCacheEntryReportStorage {
  std::string selector;
  std::string class_name;
  std::string owner_identity;
};

struct RuntimeBootstrapFixture {
  objc3_runtime_registration_state_snapshot registration_state{};
  objc3_runtime_realized_class_graph_state_snapshot graph_state{};
  objc3_runtime_realized_class_entry_snapshot root_entry{};
  objc3_runtime_realized_class_entry_snapshot widget_entry{};
  RegistrationReportStorage registration_state_storage;
  RealizedGraphReportStorage graph_state_storage;
  RealizedEntryReportStorage root_entry_storage;
  RealizedEntryReportStorage widget_entry_storage;
};

struct MetaclassGraphAssertions {
  objc3_runtime_method_cache_state_snapshot root_class_state{};
  objc3_runtime_method_cache_state_snapshot widget_class_state{};
  objc3_runtime_method_cache_state_snapshot widget_known_class_state{};
  MethodCacheStateReportStorage root_class_state_storage;
  MethodCacheStateReportStorage widget_class_state_storage;
  MethodCacheStateReportStorage widget_known_class_state_storage;
  int root_class_value = 0;
  int widget_class_value = 0;
  int widget_known_class_value = 0;
};

struct RootClassInvariants {
  objc3_runtime_method_cache_state_snapshot widget_inherited_state{};
  objc3_runtime_method_cache_state_snapshot widget_own_state{};
  objc3_runtime_method_cache_entry_snapshot root_shared_entry{};
  objc3_runtime_method_cache_entry_snapshot widget_shared_entry{};
  objc3_runtime_method_cache_entry_snapshot widget_inherited_entry{};
  objc3_runtime_method_cache_entry_snapshot widget_own_entry{};
  objc3_runtime_method_cache_entry_snapshot widget_super_entry{};
  MethodCacheStateReportStorage widget_inherited_state_storage;
  MethodCacheStateReportStorage widget_own_state_storage;
  MethodCacheEntryReportStorage root_shared_entry_storage;
  MethodCacheEntryReportStorage widget_shared_entry_storage;
  MethodCacheEntryReportStorage widget_inherited_entry_storage;
  MethodCacheEntryReportStorage widget_own_entry_storage;
  MethodCacheEntryReportStorage widget_super_entry_storage;
  int widget_inherited_instance_value = 0;
  int widget_own_instance_value = 0;
  int widget_super_instance_value = 0;
  int widget_super_own_selector_status = 0;
};

struct ProbeRun {
  RuntimeBootstrapFixture fixture;
  MetaclassGraphAssertions graph_assertions;
  RootClassInvariants root_class_invariants;
};

}  // namespace metaclass_graph_root_class
}  // namespace probe
}  // namespace runtime
}  // namespace objc3c

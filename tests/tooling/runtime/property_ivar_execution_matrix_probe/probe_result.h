#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_PROBE_RESULT_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_PROBE_RESULT_H_

#include "runtime/objc3_runtime_bootstrap_internal.h"

#include <string>

namespace objc3c::runtime::probe::property_ivar_execution_matrix {

inline constexpr int kWidgetClassReceiver = 1024;
inline constexpr const char *kWidgetClassName = "Widget";
inline constexpr const char *kAllocSelector = "alloc";
inline constexpr const char *kCountPropertyName = "count";
inline constexpr const char *kCountGetterSelector = "count";
inline constexpr const char *kCountSetterSelector = "setCount:";
inline constexpr const char *kEnabledPropertyName = "enabled";
inline constexpr const char *kEnabledGetterSelector = "enabled";
inline constexpr const char *kEnabledSetterSelector = "setEnabled:";
inline constexpr const char *kValuePropertyName = "value";
inline constexpr const char *kValueGetterSelector = "currentValue";
inline constexpr const char *kValueSetterSelector = "setCurrentValue:";
inline constexpr const char *kTokenPropertyName = "token";
inline constexpr const char *kTokenGetterSelector = "tokenValue";

struct DispatchObservation {
  objc3_runtime_dispatch_state_snapshot state{};
  std::string selector;
  std::string fast_path_reason;
  std::string path;
  std::string implementation_kind;
  std::string property_name;
  std::string class_name;
  std::string owner_identity;
};

struct RealizedClassObservation {
  objc3_runtime_realized_class_entry_snapshot entry{};
  std::string module;
  std::string identity;
  std::string class_name;
  std::string class_owner;
  std::string metaclass_owner;
  std::string super_class;
  std::string super_metaclass;
  std::string category_owner;
  std::string category_name;
};

struct PropertyEntryObservation {
  objc3_runtime_property_entry_snapshot entry{};
  std::string queried_class;
  std::string resolved_class;
  std::string property_name;
  std::string declaration_owner;
  std::string export_owner;
  std::string getter_selector;
  std::string setter_selector;
  std::string effective_getter_selector;
  std::string effective_setter_selector;
  std::string ivar_binding;
  std::string synthesized_binding;
  std::string layout_symbol;
  std::string getter_owner;
  std::string setter_owner;
};

struct PropertyRegistryObservation {
  objc3_runtime_property_registry_state_snapshot state{};
  std::string queried_class;
  std::string queried_property;
  std::string resolved_class;
  std::string resolved_owner;
};

struct MethodCacheEntryObservation {
  objc3_runtime_method_cache_entry_snapshot entry{};
  std::string selector;
  std::string class_name;
  std::string owner;
};

struct WidgetFixture {
  int widget_instance = 0;
  RealizedClassObservation widget_entry;
};

struct PropertyIvarExecutionCases {
  int set_count_result = 0;
  int count_value = 0;
  int set_enabled_result = 0;
  int enabled_value = 0;
  int set_value_result = 0;
  int value_result = 0;
  int token_value = 0;
  DispatchObservation set_count_dispatch;
  DispatchObservation count_dispatch;
  DispatchObservation set_enabled_dispatch;
  DispatchObservation enabled_dispatch;
  DispatchObservation set_value_dispatch;
  DispatchObservation value_dispatch;
  DispatchObservation token_dispatch;
};

struct PropertyIvarExecutionAssertions {
  PropertyRegistryObservation registry_state;
  PropertyEntryObservation count_property;
  PropertyEntryObservation enabled_property;
  PropertyEntryObservation value_property;
  PropertyEntryObservation token_property;
  MethodCacheEntryObservation count_method;
  MethodCacheEntryObservation enabled_method;
  MethodCacheEntryObservation value_method;
  MethodCacheEntryObservation token_method;
};

struct ProbeResult {
  WidgetFixture fixture;
  PropertyIvarExecutionCases execution;
  PropertyIvarExecutionAssertions assertions;
};

}  // namespace objc3c::runtime::probe::property_ivar_execution_matrix

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_PROPERTY_IVAR_EXECUTION_MATRIX_PROBE_PROBE_RESULT_H_

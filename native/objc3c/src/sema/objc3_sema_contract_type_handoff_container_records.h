#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "sema/objc3_sema_contract_type_handoff_callable_records.h"

struct Objc3InterfaceInfo {
  std::string super_name;
  std::vector<std::string> generic_parameter_names_source_order;
  std::vector<std::string> generic_parameter_variance_source_order;
  std::vector<std::string> adopted_protocols_lexicographic;
  bool objc_direct_members_declared = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  std::unordered_map<std::string, Objc3PropertyInfo> properties;
  std::unordered_map<std::string, Objc3MethodInfo> methods;
};

struct Objc3ImplementationInfo {
  bool has_matching_interface = false;
  std::unordered_map<std::string, Objc3PropertyInfo> properties;
  std::unordered_map<std::string, Objc3MethodInfo> methods;
};

struct Objc3CategoryMergeInfo {
  std::vector<std::string> category_owner_identities_in_merge_order;
  std::unordered_map<std::string, Objc3PropertyInfo> merged_properties;
  std::unordered_map<std::string, std::string>
      merged_property_owner_identities;
  std::unordered_map<std::string, Objc3MethodInfo> merged_methods;
  std::unordered_map<std::string, std::string> merged_method_owner_identities;
  bool deterministic = true;
};

struct Objc3InterfaceImplementationSummary {
  std::size_t declared_interfaces = 0;
  std::size_t resolved_interfaces = 0;
  std::size_t declared_implementations = 0;
  std::size_t resolved_implementations = 0;
  std::size_t interface_method_symbols = 0;
  std::size_t implementation_method_symbols = 0;
  std::size_t linked_implementation_symbols = 0;
  bool deterministic = true;
};

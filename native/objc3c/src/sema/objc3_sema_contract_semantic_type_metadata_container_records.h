#pragma once

#include <string>
#include <vector>

#include "sema/objc3_sema_contract_semantic_type_metadata_method_records.h"
#include "sema/objc3_sema_contract_semantic_type_metadata_property_records.h"

struct Objc3SemanticInterfaceTypeMetadata {
  std::string name;
  std::string super_name;
  std::vector<std::string> generic_parameter_names_source_order;
  std::vector<std::string> generic_parameter_variance_source_order;
  std::vector<std::string> adopted_protocols_lexicographic;
  std::vector<Objc3SemanticPropertyTypeMetadata> properties_lexicographic;
  std::vector<Objc3SemanticMethodTypeMetadata> methods_lexicographic;
};

struct Objc3SemanticImplementationTypeMetadata {
  std::string name;
  bool has_matching_interface = false;
  std::vector<Objc3SemanticPropertyTypeMetadata> properties_lexicographic;
  std::vector<Objc3SemanticMethodTypeMetadata> methods_lexicographic;
};

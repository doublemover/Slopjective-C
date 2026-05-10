#pragma once

#include <cstddef>
#include <string>

struct Objc3IRMetaprogrammingDerivedMethodBundle {
  std::string implementation_name;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  std::string derive_name;
  std::string selector;
  std::string emitted_symbol;
  std::size_t parameter_count = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3IRMetaprogrammingMacroArtifactBundle {
  std::string function_name;
  std::string macro_name;
  std::string package_name;
  std::string provenance_name;
  std::string cache_key_name;
  std::string sandbox_policy_name;
  std::string emitted_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle {
  std::string owner_kind;
  std::string owner_name;
  std::string declaration_owner_identity;
  std::string export_owner_identity;
  std::string property_name;
  std::string behavior_name;
  std::string binding_symbol;
  std::string emitted_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

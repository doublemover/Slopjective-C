#pragma once

#include <cstddef>
#include <string>
#include <vector>

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

struct Objc3IRFrontendMetaprogrammingDerivedMethodModelMetadata {
  std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
      metaprogramming_derived_method_bundles_lexicographic;
};

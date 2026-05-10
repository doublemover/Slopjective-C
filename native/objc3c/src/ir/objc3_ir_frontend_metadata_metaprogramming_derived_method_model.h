#pragma once

#include <vector>

#include "runtime/metadata/runtime_metadata_typed_bundles.h"

struct Objc3IRFrontendMetaprogrammingDerivedMethodModelMetadata {
  std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
      metaprogramming_derived_method_bundles_lexicographic;
};

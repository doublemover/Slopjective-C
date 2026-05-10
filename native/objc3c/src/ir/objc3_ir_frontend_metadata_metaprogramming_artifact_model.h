#pragma once

#include <vector>

#include "ir/objc3_ir_frontend_metadata_metaprogramming_bundles.h"

struct Objc3IRFrontendMetaprogrammingArtifactModelMetadata {
  std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
      metaprogramming_derived_method_bundles_lexicographic;
  std::vector<Objc3IRMetaprogrammingMacroArtifactBundle>
      metaprogramming_macro_artifact_bundles_lexicographic;
  std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
      metaprogramming_property_behavior_artifact_bundles_lexicographic;
};

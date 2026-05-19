#pragma once

#include <string>
#include <vector>

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

struct Objc3IRFrontendMetaprogrammingPropertyBehaviorModelMetadata {
  std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
      metaprogramming_property_behavior_artifact_bundles_lexicographic;
};

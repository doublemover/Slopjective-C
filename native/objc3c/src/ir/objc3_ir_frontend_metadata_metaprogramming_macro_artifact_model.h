#pragma once

#include <string>
#include <vector>

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

struct Objc3IRFrontendMetaprogrammingMacroArtifactModelMetadata {
  std::vector<Objc3IRMetaprogrammingMacroArtifactBundle>
      metaprogramming_macro_artifact_bundles_lexicographic;
};

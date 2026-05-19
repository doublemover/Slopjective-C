  std::vector<std::string> implementation_names;
  implementation_names.reserve(surface.implementations.size());
  for (const auto &entry : surface.implementations) {
    implementation_names.push_back(entry.first);
  }
  std::sort(implementation_names.begin(), implementation_names.end());

  handoff.implementations_lexicographic.reserve(implementation_names.size());
  for (const std::string &name : implementation_names) {
    const auto implementation_it = surface.implementations.find(name);
    if (implementation_it == surface.implementations.end()) {
      continue;
    }

    Objc3SemanticImplementationTypeMetadata metadata;
    metadata.name = name;
    metadata.has_matching_interface = implementation_it->second.has_matching_interface;

#include "sema/objc3_semantic_passes_type_metadata_implementation_properties.inc"

#include "sema/objc3_semantic_passes_type_metadata_implementation_methods.inc"

    handoff.implementations_lexicographic.push_back(std::move(metadata));
  }

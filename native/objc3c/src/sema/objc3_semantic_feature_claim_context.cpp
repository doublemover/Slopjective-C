#include "sema/objc3_semantic_feature_claims.h"

#include <string>
#include <unordered_set>

namespace {

bool IsRuntimeBackedObjectProperty(const Objc3PropertyDecl &property) {
  return !property.vector_spelling &&
         (property.id_spelling || property.class_spelling ||
          property.instancetype_spelling ||
          property.object_pointer_type_spelling);
}

bool IsOwnedRuntimeBackedObjectProperty(const Objc3PropertyDecl &property) {
  return IsRuntimeBackedObjectProperty(property) &&
         (property.is_copy || property.is_strong ||
          property.ownership_qualifier_spelling == "__strong");
}

}  // namespace

Objc3UnsupportedFeatureClaimContext BuildUnsupportedFeatureClaimContext(
    const Objc3Program &ast,
    bool allow_source_only_block_literals,
    bool allow_source_only_defer_statements,
    bool allow_source_only_error_runtime_surface,
    bool arc_mode_enabled) {
  Objc3UnsupportedFeatureClaimContext context;
  std::unordered_set<std::string> seen_owned_storage_sites;
  const auto record_properties = [&](const auto &owner_name,
                                     const auto &properties) {
    for (const auto &property : properties) {
      if (!IsOwnedRuntimeBackedObjectProperty(property)) {
        continue;
      }
      const std::string key = owner_name + "::" + property.name;
      if (!seen_owned_storage_sites.emplace(key).second) {
        continue;
      }
      ++context.owned_runtime_backed_object_property_sites;
    }
  };

  for (const auto &interface_decl : ast.interfaces) {
    record_properties(interface_decl.name, interface_decl.properties);
  }
  for (const auto &implementation_decl : ast.implementations) {
    record_properties(implementation_decl.name, implementation_decl.properties);
  }
  context.has_owned_runtime_backed_object_storage =
      context.owned_runtime_backed_object_property_sites > 0u;
  context.allow_source_only_block_literals = allow_source_only_block_literals;
  context.allow_source_only_defer_statements =
      allow_source_only_defer_statements;
  context.allow_source_only_error_runtime_surface =
      allow_source_only_error_runtime_surface;
  context.arc_mode_enabled = arc_mode_enabled;
  return context;
}

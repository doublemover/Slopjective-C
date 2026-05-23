#include "sema/objc3_semantic_symbol_lookup.h"

#include "sema/objc3_semantic_type_factory.h"

SemanticTypeInfo ScopeLookupType(
    const std::vector<SemanticScope> &scopes,
    const std::string &name) {
  if (const SemanticTypeInfo *found_type =
          ScopeLookupTypeOrNull(scopes, name)) {
    return *found_type;
  }
  return MakeScalarSemanticType(ValueType::Unknown);
}

const SemanticTypeInfo *ScopeLookupTypeOrNull(
    const std::vector<SemanticScope> &scopes,
    const std::string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return &found->second;
    }
  }
  return nullptr;
}

OwnershipResourceMoveBindingState *LookupOwnershipResourceMoveBinding(
    std::vector<OwnershipResourceMoveScope> &scopes,
    const std::string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return &found->second;
    }
  }
  return nullptr;
}

const OwnershipResourceMoveBindingState *LookupOwnershipResourceMoveBinding(
    const std::vector<OwnershipResourceMoveScope> &scopes,
    const std::string &name) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return &found->second;
    }
  }
  return nullptr;
}

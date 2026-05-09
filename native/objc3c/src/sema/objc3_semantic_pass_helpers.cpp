#include "sema/objc3_semantic_pass_helpers.h"

#include <sstream>

std::string JoinStringVector(const std::vector<std::string> &items,
                             const std::string &separator) {
  std::ostringstream out;
  for (std::size_t index = 0; index < items.size(); ++index) {
    if (index != 0u) {
      out << separator;
    }
    out << items[index];
  }
  return out.str();
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

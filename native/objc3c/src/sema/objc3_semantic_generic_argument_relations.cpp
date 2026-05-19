#include "sema/objc3_semantic_generic_argument_relations.h"

#include <algorithm>
#include <cctype>
#include <unordered_set>

std::string NormalizeGenericArgumentTypeSpelling(const std::string &argument) {
  std::string normalized;
  normalized.reserve(argument.size());
  for (const unsigned char c : argument) {
    if (std::isspace(c) == 0) {
      normalized.push_back(static_cast<char>(c));
    }
  }
  return normalized;
}

bool InterfaceAdoptsProtocolDirectly(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &interface_name,
    const std::string &protocol_name) {
  const auto interface_it = surface.interfaces.find(interface_name);
  if (interface_it == surface.interfaces.end()) {
    return false;
  }
  const auto &adopted_protocols =
      interface_it->second.adopted_protocols_lexicographic;
  return std::binary_search(adopted_protocols.begin(), adopted_protocols.end(),
                            protocol_name);
}

bool IsSameOrDerivedInterface(const Objc3SemanticIntegrationSurface &surface,
                              const std::string &target_interface_name,
                              const std::string &value_interface_name) {
  if (target_interface_name.empty() || value_interface_name.empty()) {
    return false;
  }
  if (target_interface_name == value_interface_name) {
    return true;
  }
  std::unordered_set<std::string> visited;
  std::string current = value_interface_name;
  while (!current.empty() && visited.insert(current).second) {
    const auto interface_it = surface.interfaces.find(current);
    if (interface_it == surface.interfaces.end()) {
      return false;
    }
    current = interface_it->second.super_name;
    if (current == target_interface_name) {
      return true;
    }
  }
  return false;
}

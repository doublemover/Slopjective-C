#pragma once

#include <string>
#include <vector>

#include "clang_foundation/interface_contract.h"

namespace objc3::clang_foundation {

struct InterfaceDependencyNode {
  InterfaceModuleIdentity identity;
  std::vector<InterfaceDependency> dependencies;
};

struct InterfaceDependencyGraphDiagnostic {
  std::string code;
  std::string message;
  std::vector<std::string> modules;
};

struct InterfaceDependencyGraphResult {
  // Dependency-first deterministic order. Empty when any cycle or identity
  // conflict makes the graph unusable.
  std::vector<std::string> topological_order;
  // Publicly visible dependency identities reachable through reexports.
  std::vector<std::string> reexport_closure;
  std::vector<InterfaceDependencyGraphDiagnostic> diagnostics;
  [[nodiscard]] bool ok() const { return diagnostics.empty(); }
};

[[nodiscard]] InterfaceDependencyGraphResult ResolveInterfaceDependencyGraph(
    const std::vector<InterfaceDependencyNode> &nodes);

}  // namespace objc3::clang_foundation

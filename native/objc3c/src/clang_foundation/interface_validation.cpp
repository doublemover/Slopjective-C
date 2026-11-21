#include "clang_foundation/interface_validation.h"

#include <algorithm>
#include <map>
#include <set>
#include <tuple>

namespace objc3::clang_foundation {
namespace {
std::string IdentityKey(const InterfaceModuleIdentity &i) {
  return i.canonical_name + "@" + i.semantic_fingerprint + "#" +
         i.target_triple + "#" + i.runtime_abi + "#" + i.extension_abi + "#" +
         i.language_revision + "#" +
         i.language_profile + "#" + i.package_identity;
}
void Add(InterfaceDependencyGraphResult &r, std::string code,
         std::string message, std::vector<std::string> modules = {}) {
  std::sort(modules.begin(), modules.end());
  r.diagnostics.push_back({std::move(code), std::move(message), std::move(modules)});
}
void SortDiagnostics(InterfaceDependencyGraphResult &r) {
  std::sort(r.diagnostics.begin(), r.diagnostics.end(), [](const auto &a, const auto &b) {
    return std::tie(a.code, a.modules, a.message) <
           std::tie(b.code, b.modules, b.message);
  });
}
}  // namespace

InterfaceDependencyGraphResult ResolveInterfaceDependencyGraph(
    const std::vector<InterfaceDependencyNode> &nodes,
    const InterfaceResourceLimits &limits) {
  InterfaceDependencyGraphResult result;
  std::uint64_t edges = 0;
  if (nodes.size() > limits.selected_modules) {
    Add(result, kInterfaceResourceDiagnostic, "selected interface graph exceeds its module budget");
    return result;
  }
  for (const auto &node : nodes)
    if (!ChargeInterfaceResource(edges, node.dependencies.size(), limits.dependency_edges)) {
      Add(result, kInterfaceResourceDiagnostic, "selected interface graph exceeds its dependency-edge budget");
      return result;
    }
  std::map<std::string, const InterfaceDependencyNode *> by_name;
  for (const auto &node : nodes) {
    const auto &id = node.identity;
    if (id.canonical_name.empty() || id.semantic_fingerprint.empty()) {
      Add(result, "O3IFC020", "module identity is incomplete", {id.canonical_name});
      continue;
    }
    const auto [it, inserted] = by_name.emplace(id.canonical_name, &node);
    if (!inserted && IdentityKey(it->second->identity) != IdentityKey(id))
      Add(result, "O3IFC021", "duplicate canonical module name has conflicting identity", {id.canonical_name});
    else if (!inserted)
      Add(result, "O3IFC022", "duplicate canonical module identity", {id.canonical_name});
  }
  if (!result.diagnostics.empty()) {
    SortDiagnostics(result);
    return result;
  }

  std::map<std::string, unsigned> color;
  std::map<std::string, std::uint64_t> depths;
  struct Frame {
    std::string name;
    std::vector<const InterfaceDependency *> dependencies;
    std::size_t cursor = 0;
  };
  std::vector<Frame> stack;
  auto push = [&](const std::string &name) {
    if (stack.size() >= limits.dependency_depth) {
      Add(result, kInterfaceResourceDiagnostic,
          "selected interface graph exceeds its dependency-depth budget", {name});
      return false;
    }
    Frame frame{name, {}, 0};
    for (const auto &dependency : by_name.at(name)->dependencies)
      frame.dependencies.push_back(&dependency);
    std::sort(frame.dependencies.begin(), frame.dependencies.end(), [](auto a, auto b) {
      return std::tie(a->module.canonical_name, a->module.semantic_fingerprint) <
             std::tie(b->module.canonical_name, b->module.semantic_fingerprint);
    });
    color[name] = 1;
    stack.push_back(std::move(frame));
    return true;
  };
  for (const auto &[root, _] : by_name) {
    if (color[root]) continue;
    if (!push(root)) break;
    while (!stack.empty()) {
      auto &frame = stack.back();
      if (frame.cursor == frame.dependencies.size()) {
        // A dependency may already have been visited from another root.
        // Bound the longest graph path, not only the current DFS stack.
        std::uint64_t depth = 1;
        for (const auto *dependency : frame.dependencies) {
          const auto found = depths.find(dependency->module.canonical_name);
          if (found == depths.end()) continue;
          if (found->second >= limits.dependency_depth) {
            Add(result, kInterfaceResourceDiagnostic,
                "selected interface graph exceeds its dependency-depth budget",
                {frame.name});
            result.topological_order.clear();
            SortDiagnostics(result);
            return result;
          }
          depth = std::max(depth, found->second + 1);
        }
        depths[frame.name] = depth;
        color[frame.name] = 2;
        result.topological_order.push_back(frame.name);
        stack.pop_back();
        continue;
      }
      const auto *dep = frame.dependencies[frame.cursor++];
      const auto name = frame.name;
      const auto &dependency_name = dep->module.canonical_name;
      const auto found = by_name.find(dependency_name);
      if (found == by_name.end()) {
        Add(result, "O3IFC023", "imported module is absent from resolved interface set", {name, dependency_name});
        continue;
      }
      if (IdentityKey(found->second->identity) != IdentityKey(dep->module))
        Add(result, "O3IFC024", "dependency lock identity is stale", {name, dependency_name});
      if (dep->module.target_triple != by_name.at(name)->identity.target_triple ||
          dep->module.runtime_abi != by_name.at(name)->identity.runtime_abi ||
          dep->module.extension_abi != by_name.at(name)->identity.extension_abi ||
          dep->module.language_revision != by_name.at(name)->identity.language_revision)
        Add(result, "O3IFC025", "dependency target or runtime ABI mismatch", {name, dependency_name});
      if (color[dependency_name] == 1) {
        std::vector<std::string> cycle;
        const auto start = std::find_if(stack.begin(), stack.end(),
            [&](const auto &entry) { return entry.name == dependency_name; });
        for (auto current = start; current != stack.end(); ++current)
          cycle.push_back(current->name);
        Add(result, "O3IFC026", "unsupported module dependency cycle", std::move(cycle));
      } else if (!color[dependency_name] && !push(dependency_name)) {
        result.topological_order.clear();
        SortDiagnostics(result);
        return result;
      }
    }
  }
  if (!result.diagnostics.empty()) {
    result.topological_order.clear();
    SortDiagnostics(result);
    return result;
  }
  // Reexport closure is transitively visible only over explicitly reexported
  // edges; ordinary private imports do not leak through a module boundary.
  std::set<std::string> closure;
  // Every resolved node is a root for this union. Collecting all direct
  // reexport edges therefore also includes every transitive reexport target.
  for (const auto &[name, node] : by_name)
    for (const auto &dependency : node->dependencies)
      if (dependency.reexported) closure.insert(IdentityKey(dependency.module));
  result.reexport_closure.assign(closure.begin(), closure.end());
  return result;
}

}  // namespace objc3::clang_foundation

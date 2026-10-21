#include "clang_foundation/interface_validation.h"

#include <algorithm>
#include <functional>
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
    const std::vector<InterfaceDependencyNode> &nodes) {
  InterfaceDependencyGraphResult result;
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
  std::vector<std::string> stack;
  bool cycle = false;
  std::function<void(const std::string &)> visit = [&](const std::string &name) {
    color[name] = 1;
    stack.push_back(name);
    std::vector<const InterfaceDependency *> deps;
    for (const auto &dep : by_name.at(name)->dependencies) deps.push_back(&dep);
    std::sort(deps.begin(), deps.end(), [](auto a, auto b) {
      return std::tie(a->module.canonical_name, a->module.semantic_fingerprint) <
             std::tie(b->module.canonical_name, b->module.semantic_fingerprint);
    });
    for (const auto *dep : deps) {
      const auto found = by_name.find(dep->module.canonical_name);
      if (found == by_name.end()) {
        Add(result, "O3IFC023", "imported module is absent from resolved interface set", {name, dep->module.canonical_name});
        continue;
      }
      if (IdentityKey(found->second->identity) != IdentityKey(dep->module))
        Add(result, "O3IFC024", "dependency lock identity is stale", {name, dep->module.canonical_name});
      if (dep->module.target_triple != by_name.at(name)->identity.target_triple ||
          dep->module.runtime_abi != by_name.at(name)->identity.runtime_abi ||
          dep->module.extension_abi != by_name.at(name)->identity.extension_abi ||
          dep->module.language_revision != by_name.at(name)->identity.language_revision)
        Add(result, "O3IFC025", "dependency target or runtime ABI mismatch", {name, dep->module.canonical_name});
      const auto state = color[dep->module.canonical_name];
      if (state == 1) {
        cycle = true;
        auto start = std::find(stack.begin(), stack.end(), dep->module.canonical_name);
        Add(result, "O3IFC026", "unsupported module dependency cycle",
            std::vector<std::string>(start, stack.end()));
      } else if (state == 0) visit(dep->module.canonical_name);
    }
    stack.pop_back();
    color[name] = 2;
    result.topological_order.push_back(name);
  };
  for (const auto &[name, _] : by_name) if (color[name] == 0) visit(name);
  if (cycle || !result.diagnostics.empty()) {
    result.topological_order.clear();
    SortDiagnostics(result);
    return result;
  }
  // Reexport closure is transitively visible only over explicitly reexported
  // edges; ordinary private imports do not leak through a module boundary.
  std::set<std::string> closure;
  std::function<void(const std::string &)> collect = [&](const std::string &name) {
    for (const auto &dep : by_name.at(name)->dependencies) {
      if (!dep.reexported) continue;
      const auto key = IdentityKey(dep.module);
      if (closure.insert(key).second && by_name.count(dep.module.canonical_name))
        collect(dep.module.canonical_name);
    }
  };
  for (const auto &[name, _] : by_name) collect(name);
  result.reexport_closure.assign(closure.begin(), closure.end());
  return result;
}

}  // namespace objc3::clang_foundation

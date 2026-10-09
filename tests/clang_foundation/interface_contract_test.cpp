#include "clang_foundation/interface_contract.h"
#include "clang_foundation/interface_validation.h"

#include <cassert>
#include <string>
#include <vector>

using namespace objc3::clang_foundation;

static InterfaceModuleIdentity Id(std::string name, std::string fp) {
  return {std::move(name), std::move(fp), "x86_64-unknown-linux-gnu",
          "objc3-runtime-v1", "objc3-abi-v1", "c18-r1",
          "canonical-c18", "pkg-main"};
}

int main() {
  auto module = Id("A", "sha256:aaa");
  InterfaceDeclarationIdentity decl{"c:@F@answer", "function", "A::answer",
                                    "int ()", {}};
  SemanticInterfaceSidecar sidecar;
  sidecar.module = module;
  sidecar.canonical_source_sha256 = std::string(64, 'a');
  sidecar.declarations = {decl};
  assert(ValidateSemanticInterface(sidecar, module, sidecar.canonical_source_sha256,
                                   {decl}, {}).ok());
  const auto fingerprint_input = BuildSemanticFingerprintInput(sidecar);
  assert(!fingerprint_input.empty());
  auto formatting_only = sidecar;
  formatting_only.canonical_source_sha256 = std::string(64, 'b');
  assert(BuildSemanticFingerprintInput(formatting_only) == fingerprint_input);
  auto unknown = sidecar;
  unknown.required_semantics = {"effects.async.v9"};
  assert(!ValidateSemanticInterface(unknown, module, unknown.canonical_source_sha256,
                                    {decl}, {}).ok());
  auto revision = Id("A", "sha256:aaa");
  revision.language_revision = "c23-r1";
  assert(!ValidateSemanticInterface(sidecar, revision,
                                    sidecar.canonical_source_sha256,
                                    {decl}, {}).ok());
  auto target = Id("A", "sha256:aaa");
  target.target_triple = "aarch64-unknown-linux-gnu";
  assert(!ValidateSemanticInterface(sidecar, target, sidecar.canonical_source_sha256,
                                    {decl}, {}).ok());
  target = Id("A", "sha256:aaa");
  target.extension_abi = "objc3-abi-v2";
  assert(!ValidateSemanticInterface(sidecar, target, sidecar.canonical_source_sha256,
                                    {decl}, {}).ok());
  auto drift = decl;
  drift.canonical_type_identity = "long ()";
  assert(!ValidateSemanticInterface(sidecar, module, sidecar.canonical_source_sha256,
                                    {drift}, {}).ok());
  auto effectful = decl;
  effectful.effects.is_throwing = true;
  auto effect_sidecar = sidecar;
  effect_sidecar.declarations = {effectful};
  assert(!ValidateSemanticInterface(effect_sidecar, module,
                                    effect_sidecar.canonical_source_sha256,
                                    {effectful}, {}).ok());
  auto hash_drift = sidecar;
  hash_drift.canonical_source_sha256 = std::string(64, 'b');
  assert(!ValidateSemanticInterface(hash_drift, module, std::string(64, 'a'),
                                    {decl}, {}).ok());

  InterfaceDependencyNode c{Id("C", "sha256:c"), {}};
  InterfaceDependencyNode b{Id("B", "sha256:b"), {{c.identity, true, true}}};
  InterfaceDependencyNode a{module, {{b.identity, false, false}}};
  auto graph = ResolveInterfaceDependencyGraph({a, c, b});
  assert(graph.ok());
  assert((graph.topological_order == std::vector<std::string>{"C", "B", "A"}));
  assert(graph.reexport_closure.size() == 1);
  auto cycle = c;
  cycle.dependencies = {{module, false, false}};
  auto cyclic = ResolveInterfaceDependencyGraph({a, b, cycle});
  assert(!cyclic.ok() && cyclic.topological_order.empty());
  InterfaceDependencyNode missing{Id("D", "sha256:d"),
                                  {{Id("ghost", "sha256:x"), false, false}}};
  assert(!ResolveInterfaceDependencyGraph({missing}).ok());
  InterfaceDependencyNode stale{Id("B", "sha256:b"),
                                {{Id("C", "sha256:old"), false, false}}};
  assert(!ResolveInterfaceDependencyGraph({stale, c}).ok());
  assert(!ResolveInterfaceDependencyGraph(
      {a, b, c, {Id("A", "sha256:other"), {}}}).ok());
}

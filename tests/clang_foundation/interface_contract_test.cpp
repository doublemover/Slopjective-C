#include "clang_foundation/interface_contract.h"
#include "clang_foundation/interface_validation.h"

#include <cassert>
#include <string>
#include <vector>

using namespace objc3::clang_foundation;

static InterfaceModuleIdentity Id(std::string name, std::string fp) {
  return {std::move(name),    std::move(fp),  "x86_64-unknown-linux-gnu",
          "objc3-runtime-v1", "objc3-abi-v1", "c18-r1",
          "canonical-c18",    "pkg-main"};
}

static void TestNativeContractCapabilities() {
  auto module = Id("contracts", "sha256:contracts");
  InterfaceDeclarationIdentity declaration{
      "c:@F@borrow", "function", "borrow", "char *(int)", {}};
  declaration.export_root = "borrow";
  declaration.effects.ownership_identity =
      "native-contracts:v1:returns-borrowed:0";
  SemanticInterfaceSidecar sidecar;
  sidecar.module = module;
  sidecar.canonical_source_sha256 = std::string(64, 'a');
  sidecar.declarations = {declaration};
  sidecar.required_semantics = {"objc3.canonical-function-effects.v1"};
  const std::vector<std::string> supported = {
      "objc3.canonical-function-effects.v1",
      "objc3.native-ownership-contracts.v1", "objc3.isolation-contracts.v1",
      "objc3.typed-keypaths.v1"};
  assert(!ValidateSemanticInterface(sidecar, module,
                                    sidecar.canonical_source_sha256,
                                    {declaration}, supported)
              .ok());
  sidecar.required_semantics.push_back("objc3.native-ownership-contracts.v1");
  assert(ValidateSemanticInterface(sidecar, module,
                                   sidecar.canonical_source_sha256,
                                   {declaration}, supported)
             .ok());
  const auto fingerprint = BuildSemanticFingerprintInput(sidecar);
  auto changed = declaration;
  changed.effects.ownership_identity += ";different-owner";
  assert(!SameInterfaceDeclarationIdentity(declaration, changed));
  auto different = sidecar;
  different.declarations = {changed};
  assert(fingerprint != BuildSemanticFingerprintInput(different));
  declaration.effects.actor_isolation_identity = "actor-isolated";
  declaration.effects.executor_affinity_identity = "named:ui";
  sidecar.declarations = {declaration};
  assert(!ValidateSemanticInterface(sidecar, module,
                                    sidecar.canonical_source_sha256,
                                    {declaration}, supported)
              .ok());
  sidecar.required_semantics.push_back("objc3.isolation-contracts.v1");
  assert(ValidateSemanticInterface(sidecar, module,
                                   sidecar.canonical_source_sha256,
                                   {declaration}, supported)
             .ok());
  declaration.canonical_type_identity = "KeyPath<Root *, int>";
  sidecar.declarations = {declaration};
  assert(!ValidateSemanticInterface(sidecar, module,
                                    sidecar.canonical_source_sha256,
                                    {declaration}, supported)
              .ok());
  sidecar.required_semantics.push_back("objc3.typed-keypaths.v1");
  assert(ValidateSemanticInterface(sidecar, module,
                                   sidecar.canonical_source_sha256,
                                   {declaration}, supported)
             .ok());
}

static void TestAsyncABIContracts() {
  auto module = Id("async.frames", "sha256:frames");
  InterfaceDeclarationIdentity frame{
      "c:@F@fetch", "Function", "fetch", "int (int) __objc3_async_frame", {}};
  frame.export_root = "fetch";
  frame.effects.is_async = true;
  frame.effects.async_abi_identity = "v1:4:type10:c-frame-v1";
  assert(IsValidAsyncABIIdentity(frame.effects));
  auto task = frame;
  task.effects.async_abi_identity = "v1:4:type11:cxx-task-v1";
  assert(IsValidAsyncABIIdentity(task.effects));
  assert(!SameInterfaceDeclarationIdentity(frame, task));
  SemanticInterfaceSidecar sidecar;
  sidecar.module = module;
  sidecar.canonical_source_sha256 = std::string(64, 'a');
  sidecar.declarations = {frame};
  sidecar.required_semantics = {"objc3.canonical-function-effects.v1",
                                "objc3.sdk-runtime-frames.v4.test"};
  const std::vector<std::string> supported = {
      "objc3.canonical-function-effects.v1", kAsyncEntryABISemantic,
      "objc3.sdk-runtime-frames.v4.test"};
  auto check = [&] {
    return ValidateSemanticInterface(sidecar, module,
                                     sidecar.canonical_source_sha256,
                                     sidecar.declarations, supported)
        .ok();
  };
  assert(!check());
  sidecar.required_semantics.push_back(kAsyncEntryABISemantic);
  assert(check());
  const auto fingerprint = BuildSemanticFingerprintInput(sidecar);
  sidecar.declarations = {task};
  assert(fingerprint != BuildSemanticFingerprintInput(sidecar));
  assert(!ValidateSemanticInterface(sidecar, module,
                                    sidecar.canonical_source_sha256, {frame},
                                    supported)
              .ok());
  for (const char *bad :
       {"", "v1:", "v1:4:type10:c-frame-v9", "v1:04:type10:c-frame-v1",
        "v1:4:type10:c-frame-v1junk",
        "v1:999999999999999999999999:type10:c-frame-v1",
        "v1:4:type10:c-frame-v14:type11:cxx-task-v1"}) {
    sidecar.declarations[0].effects.async_abi_identity = bad;
    assert(!check());
  }
  // A record need not itself be async to contain an ABI-bearing callback.
  sidecar.declarations[0].effects.is_async = false;
  sidecar.declarations[0].effects.async_abi_identity =
      "v1:21:type.field[0].pointee10:c-frame-v1";
  assert(check());
}

int main() {
  TestNativeContractCapabilities();
  TestAsyncABIContracts();
  auto module = Id("A", "sha256:aaa");
  InterfaceDeclarationIdentity decl{
      "c:@F@answer", "function", "A::answer", "int ()", {}};
  decl.export_root = "A::answer";
  SemanticInterfaceSidecar sidecar;
  sidecar.module = module;
  sidecar.canonical_source_sha256 = std::string(64, 'a');
  sidecar.declarations = {decl};
  auto forged_root = sidecar;
  forged_root.declarations.front().export_root = "Unrelated";
  assert(!SameInterfaceDeclarationIdentity(forged_root.declarations.front(), decl));
  assert(!ValidateSemanticInterface(forged_root, module,
          sidecar.canonical_source_sha256, {decl}, {}).ok());
  assert(ValidateSemanticInterface(sidecar, module,
                                   sidecar.canonical_source_sha256, {decl}, {})
             .ok());
  const auto fingerprint_input = BuildSemanticFingerprintInput(sidecar);
  assert(!fingerprint_input.empty());
  auto formatting_only = sidecar;
  formatting_only.canonical_source_sha256 = std::string(64, 'b');
  assert(BuildSemanticFingerprintInput(formatting_only) == fingerprint_input);
  auto unknown = sidecar;
  unknown.required_semantics = {"effects.async.v9"};
  assert(!ValidateSemanticInterface(unknown, module,
                                    unknown.canonical_source_sha256, {decl}, {})
              .ok());
  auto revision = Id("A", "sha256:aaa");
  revision.language_revision = "c23-r1";
  assert(!ValidateSemanticInterface(sidecar, revision,
                                    sidecar.canonical_source_sha256, {decl}, {})
              .ok());
  auto target = Id("A", "sha256:aaa");
  target.target_triple = "aarch64-unknown-linux-gnu";
  assert(!ValidateSemanticInterface(sidecar, target,
                                    sidecar.canonical_source_sha256, {decl}, {})
              .ok());
  target = Id("A", "sha256:aaa");
  target.extension_abi = "objc3-abi-v2";
  assert(!ValidateSemanticInterface(sidecar, target,
                                    sidecar.canonical_source_sha256, {decl}, {})
              .ok());
  auto drift = decl;
  auto availability = sidecar;
  auto annotated = decl;
  annotated.canonical_type_identity += ";api-availability=deprecated";
  availability.declarations = {annotated};
  assert(!ValidateSemanticInterface(availability, module,
                                    availability.canonical_source_sha256,
                                    {annotated}, {})
              .ok());
  availability.required_semantics = {"objc3.api-availability.v1"};
  assert(ValidateSemanticInterface(availability, module,
                                   availability.canonical_source_sha256,
                                   {annotated}, {"objc3.api-availability.v1"})
             .ok());
  availability.declarations = {decl};
  assert(!ValidateSemanticInterface(availability, module,
                                    availability.canonical_source_sha256,
                                    {decl}, {"objc3.api-availability.v1"})
              .ok());
  auto local_types = sidecar;
  auto local_declaration = decl;
  local_declaration.canonical_type_identity += ";cxx-local-types:v1;";
  local_types.declarations = {local_declaration};
  assert(!ValidateSemanticInterface(local_types, module,
                                    local_types.canonical_source_sha256,
                                    {local_declaration}, {kCxxLocalTypeSemantic})
              .ok());
  local_types.required_semantics = {kCxxLocalTypeSemantic};
  assert(ValidateSemanticInterface(local_types, module,
                                   local_types.canonical_source_sha256,
                                   {local_declaration}, {kCxxLocalTypeSemantic})
             .ok());
  local_types.declarations = {decl};
  assert(!ValidateSemanticInterface(local_types, module,
                                    local_types.canonical_source_sha256,
                                    {decl}, {kCxxLocalTypeSemantic})
              .ok());
  drift.canonical_type_identity = "long ()";
  assert(!ValidateSemanticInterface(
              sidecar, module, sidecar.canonical_source_sha256, {drift}, {})
              .ok());
  auto effectful = decl;
  effectful.effects.is_throwing = true;
  auto effect_sidecar = sidecar;
  effect_sidecar.declarations = {effectful};
  assert(!ValidateSemanticInterface(effect_sidecar, module,
                                    effect_sidecar.canonical_source_sha256,
                                    {effectful}, {})
              .ok());
  auto hash_drift = sidecar;
  hash_drift.canonical_source_sha256 = std::string(64, 'b');
  assert(!ValidateSemanticInterface(hash_drift, module, std::string(64, 'a'),
                                    {decl}, {})
              .ok());

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
  assert(
      !ResolveInterfaceDependencyGraph({a, b, c, {Id("A", "sha256:other"), {}}})
           .ok());
}

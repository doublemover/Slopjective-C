#include "clang_foundation/interface_source_import_bridge.h"
#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_artifact_io.h"

#include <cassert>
#include <chrono>
#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "clang/AST/ASTImporter.h"
#include "clang/AST/Decl.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Sema/Lookup.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/Error.h"

using namespace objc3::clang_foundation;

namespace {
std::unique_ptr<clang::ASTUnit> Parse(llvm::StringRef source,
                                      llvm::StringRef filename) {
  const std::vector<std::string> args = {"-x", "objective-c++", "-std=c++20"};
  return clang::tooling::buildASTFromCodeWithArgs(source, args, filename);
}

std::vector<clang::NamedDecl *> ImportNamedDeclarations(
    clang::ASTUnit &provider, clang::ASTUnit &consumer) {
  std::vector<clang::NamedDecl *> imported_declarations;
  clang::ASTImporter importer(consumer.getASTContext(), consumer.getFileManager(),
      provider.getASTContext(), provider.getFileManager(), /*MinimalImport=*/false);
  for (auto *decl : provider.getASTContext().getTranslationUnitDecl()->decls()) {
    auto *named = llvm::dyn_cast<clang::NamedDecl>(decl);
    if (!named || named->isImplicit()) continue;
    auto imported = importer.Import(named);
    assert(imported && "provider declaration must import into the consumer AST");
    auto *target = llvm::dyn_cast<clang::NamedDecl>(*imported);
    assert(target && "named provider declaration must remain named after import");
    imported_declarations.push_back(target);
  }
  return imported_declarations;
}

InterfaceModuleRequest RegistryEntry(llvm::StringRef name) {
  InterfaceModuleRequest entry;
  entry.canonical_name = name.str();
  entry.locked_identity.canonical_name = entry.canonical_name;
  entry.locked_identity.semantic_fingerprint = "fingerprint-" + name.str();
  entry.locked_identity.package_identity = "package-" + name.str();
  entry.initially_visible = false;
  return entry;
}

InterfaceSourceArtifact Artifact(llvm::StringRef name) {
  InterfaceSourceArtifact artifact;
  artifact.module = RegistryEntry(name).locked_identity;
  artifact.virtual_include_path = BuildInterfaceIncludePath(name);
  return artifact;
}
} // namespace

void TestPreloadedLookups() {
  auto provider_a = Parse("typedef struct Error { int code; } Error;\n"
                          "typedef Error ErrorAlias;\n", "A.objc3i");
  auto provider_b = Parse("int answer(int);\n", "B.objc3i");
  auto provider_c = Parse("long answer(long);\n", "C.objc3i");
  auto provider_reexport = Parse("int reexported_api(void);\n", "R.objc3i");
  auto consumer = Parse("", "consumer.m");
  assert(provider_a && provider_b && provider_c && provider_reexport && consumer);

  InterfaceImportReport imported;
  imported.success = true;
  imported.declarations_by_module["fixture.A"] =
      ImportNamedDeclarations(*provider_a, *consumer);
  imported.declarations_by_module["fixture.B"] =
      ImportNamedDeclarations(*provider_b, *consumer);
  imported.declarations_by_module["fixture.C"] =
      ImportNamedDeclarations(*provider_c, *consumer);
  imported.declarations_by_module["fixture.Reexport"] =
      ImportNamedDeclarations(*provider_reexport, *consumer);

  auto a = Artifact("fixture.A");
  auto b = Artifact("fixture.B");
  auto c = Artifact("fixture.C");
  auto reexport = Artifact("fixture.Reexport");
  b.dependencies.push_back({a.module, false, true});
  reexport.dependencies.push_back({a.module, true, true});
  std::vector<InterfaceSourceArtifact> artifacts = {a, b, c, reexport};
  std::vector<InterfaceModuleRequest> registry = {
      RegistryEntry("fixture.A"), RegistryEntry("fixture.B"),
      RegistryEntry("fixture.C"), RegistryEntry("fixture.Reexport")};
  InterfaceSourceImportBridge bridge(registry, imported, artifacts);
  const llvm::StringRef qualified_answer[] = {"fixture", "B", "answer"};
  const llvm::StringRef overly_qualified[] = {
      "fixture", "A", "ErrorAlias", "missing"};

  // Source names are unavailable before the parser reaches @import.
  assert(bridge.LookupUnqualified("answer", 0).empty());
  assert(bridge.LookupQualified("fixture.B", "answer", 0).status ==
         QualifiedModuleLookupStatus::ModuleNotVisible);
  assert(bridge.LookupQualifiedSpelling(qualified_answer, 0).status ==
         QualifiedModuleLookupStatus::ModuleNotVisible);
  llvm::SmallVector<clang::NamedDecl *, 4> callback_declarations;
  clang::IdentifierInfo *qualified_components[] = {
      &consumer->getASTContext().Idents.get("fixture"),
      &consumer->getASTContext().Idents.get("B"),
      &consumer->getASTContext().Idents.get("answer")};
  assert(bridge.LookupObjC3QualifiedName(qualified_components, 0,
                                        callback_declarations) ==
         clang::ObjC3QualifiedNameLookupStatus::ModuleNotVisible);
  assert(callback_declarations.empty());
  assert(bridge.Activate("fixture.B") == SourceModuleImportStatus::Imported);
  assert(bridge.LookupUnqualified("answer", 0).size() == 1);
  assert(bridge.LookupQualified("fixture.B", "answer", 0).status ==
         QualifiedModuleLookupStatus::Found);
  assert(bridge.LookupQualifiedSpelling(qualified_answer, 0).status ==
         QualifiedModuleLookupStatus::Found);
  callback_declarations.clear();
  assert(bridge.LookupObjC3QualifiedName(qualified_components, 0,
                                        callback_declarations) ==
         clang::ObjC3QualifiedNameLookupStatus::Found);
  assert(bridge.LookupQualifiedSpelling(overly_qualified, 0).status ==
         QualifiedModuleLookupStatus::NotFound);

  // B references A's type but does not re-export A. Importing B must not
  // leak A's declaration into unqualified or qualified consumer lookup.
  assert(bridge.LookupUnqualified("ErrorAlias", 0).empty());
  assert(bridge.LookupQualified("fixture.A", "ErrorAlias", 0).status ==
         QualifiedModuleLookupStatus::ModuleNotVisible);
  // A hidden module is diagnosed only when the requested name is one of its
  // exports; a typo must remain a missing declaration, not an import fix-it.
  assert(bridge.LookupQualified("fixture.A", "not_exported", 0).status ==
         QualifiedModuleLookupStatus::NotFound);
  assert(bridge.LookupQualified("fixture.A", "ErrorAlias",
                                clang::Decl::IDNS_Tag).status ==
         QualifiedModuleLookupStatus::NotFound);
  clang::IdentifierInfo *unknown_components[] = {
      &consumer->getASTContext().Idents.get("fixture"),
      &consumer->getASTContext().Idents.get("Unknown"),
      &consumer->getASTContext().Idents.get("answer")};
  callback_declarations.clear();
  assert(bridge.LookupObjC3QualifiedName(unknown_components, 0,
                                        callback_declarations) ==
         clang::ObjC3QualifiedNameLookupStatus::NotHandled);
  assert(callback_declarations.empty());
  assert(bridge.Activate("fixture.C") == SourceModuleImportStatus::Imported);
  assert(bridge.LookupUnqualified("answer", 0).size() == 2);
  assert(bridge.LookupQualified("fixture.B", "answer", 0).status ==
         QualifiedModuleLookupStatus::Found);
  assert(bridge.Activate("fixture.Reexport") ==
         SourceModuleImportStatus::Imported);
  assert(bridge.LookupQualified("fixture.A", "ErrorAlias", 0).status ==
         QualifiedModuleLookupStatus::Found);
  assert(bridge.LookupUnqualified("ErrorAlias", 0).size() == 1);
  const auto paired = bridge.LookupQualified("fixture.A", "Error", 0);
  assert(paired.status == QualifiedModuleLookupStatus::Found);
  assert(paired.declarations.size() == 1 &&
         llvm::isa<clang::TypedefNameDecl>(paired.declarations.front()));

  assert(consumer->hasSema());
  {
    // ExternalSemaSource must resolve its accumulated candidates itself:
    // ordinary lookup prefers the typedef and tag lookup prefers the record.
    auto name = clang::DeclarationName(&consumer->getASTContext().Idents.get("Error"));
    clang::LookupResult ordinary(consumer->getSema(), name, {},
                                clang::Sema::LookupOrdinaryName);
    assert(bridge.LookupUnqualified(ordinary, nullptr));
    assert(ordinary.isSingleResult());
    assert(llvm::isa<clang::TypedefNameDecl>(ordinary.getFoundDecl()));
    clang::LookupResult tag(consumer->getSema(), name, {}, clang::Sema::LookupTagName);
    assert(bridge.LookupUnqualified(tag, nullptr));
    assert(tag.isSingleResult());
    assert(llvm::isa<clang::RecordDecl>(tag.getFoundDecl()));

    // Two function candidates form a native overload result, rather than
    // retaining addDecl's temporary Found state with multiple declarations.
    clang::LookupResult overloads(consumer->getSema(),
        clang::DeclarationName(&consumer->getASTContext().Idents.get("answer")),
        {}, clang::Sema::LookupOrdinaryName);
    assert(bridge.LookupUnqualified(overloads, nullptr));
    assert(overloads.isOverloadedResult());
  }
  assert(bridge.Activate("fixture.Unknown") ==
         SourceModuleImportStatus::NotRegistered);
  assert(bridge.HandleObjC3SourceImport("fixture.Unknown", {}) ==
         clang::ObjC3SourceImportResult::NotHandled);

  // Re-export visibility does not require a separate direct registry entry.
  InterfaceSourceImportBridge reexport_only(
      {RegistryEntry("fixture.Reexport")}, imported, artifacts);
  assert(reexport_only.Activate("fixture.Reexport") ==
         SourceModuleImportStatus::Imported);
  const llvm::StringRef qualified_alias[] = {"fixture", "A", "ErrorAlias"};
  assert(!reexport_only.IsRegistered("fixture.A"));
  assert(reexport_only.LookupQualifiedSpelling(qualified_alias, 0).status ==
         QualifiedModuleLookupStatus::Found);

  // The two independent export sets remain selectable through qualification.
  imported.declarations_by_module["fixture.B"].insert(
      imported.declarations_by_module["fixture.B"].end(),
      imported.declarations_by_module["fixture.C"].begin(),
      imported.declarations_by_module["fixture.C"].end());
  InterfaceSourceImportBridge ambiguous(registry, imported, artifacts);
  assert(ambiguous.Activate("fixture.B") == SourceModuleImportStatus::Imported);
  assert(ambiguous.LookupQualified("fixture.B", "answer", 0).status ==
         QualifiedModuleLookupStatus::Ambiguous);

  // Repeated AST redeclarations describe one entity, not an ambiguity.
  auto redeclarations = Parse("int repeated(int); int repeated(int);\n", "D.objc3i");
  auto redeclaration_consumer = Parse("", "redecl-consumer.mm");
  assert(redeclarations && redeclaration_consumer);
  InterfaceImportReport repeated;
  repeated.success = true;
  repeated.declarations_by_module["fixture.D"] =
      ImportNamedDeclarations(*redeclarations, *redeclaration_consumer);
  InterfaceSourceImportBridge canonical({RegistryEntry("fixture.D")},
                                        repeated, {Artifact("fixture.D")});
  assert(canonical.Activate("fixture.D") == SourceModuleImportStatus::Imported);
  assert(canonical.LookupQualified("fixture.D", "repeated", 0).status ==
         QualifiedModuleLookupStatus::Found);
  assert(canonical.LookupUnqualified("repeated", 0).size() == 1);
}

void TestConfigurationAndActivationFailures() {
  InterfaceImportReport imported;
  imported.success = true;
  auto a = Artifact("fixture.A");
  auto b = Artifact("fixture.B");
  auto missing = Artifact("fixture.Missing");
  b.dependencies = {{a.module, true, true}, {missing.module, true, true}};
  InterfaceSourceImportBridge bridge(
      {RegistryEntry("fixture.A"), RegistryEntry("fixture.B")}, imported, {a, b});
  assert(bridge.Activate("fixture.A") == SourceModuleImportStatus::Imported);
  assert(bridge.Activate("fixture.B") == SourceModuleImportStatus::Rejected);
  assert(bridge.GetVisibleModules() == std::vector<std::string>{"fixture.A"});
  assert(bridge.GetDirectDependencies().size() == 1);
  assert(!bridge.GetLastFailure().empty());
  assert(bridge.Activate("fixture.A") == SourceModuleImportStatus::AlreadyVisible);
  assert(bridge.GetLastFailure().empty());

  // A rejected report and invalid registry cannot activate declarations.
  auto failed = imported;
  failed.success = false;
  bridge.ConfigureSourceRegistry({RegistryEntry("fixture.A")}, failed, {a});
  assert(bridge.Activate("fixture.A") == SourceModuleImportStatus::Rejected);
  assert(bridge.GetVisibleModules().empty());
  bridge.ConfigureSourceRegistry(
      {RegistryEntry("fixture.A"), RegistryEntry("fixture.A")}, imported, {a});
  assert(bridge.Activate("fixture.A") == SourceModuleImportStatus::Rejected);
  assert(bridge.GetLastFailure().contains("duplicate"));

  auto wrong_lock = RegistryEntry("fixture.A");
  wrong_lock.locked_identity.package_identity = "other-package";
  bridge.ConfigureSourceRegistry({wrong_lock}, imported, {a});
  assert(bridge.Activate("fixture.A") == SourceModuleImportStatus::Rejected);
  assert(bridge.GetVisibleModules().empty());

  // Each dependency edge must be both public and re-exported.
  for (const auto &dependency : std::vector<InterfaceDependency>{
           {a.module, true, false}, {a.module, false, true}}) {
    b.dependencies = {dependency};
    bridge.ConfigureSourceRegistry({RegistryEntry("fixture.B")}, imported, {a, b});
    assert(bridge.Activate("fixture.B") == SourceModuleImportStatus::Imported);
    assert(!bridge.IsVisible("fixture.A"));
  }

  // A mismatched dependency identity fails without leaking any visibility.
  auto wrong_dependency = a.module;
  wrong_dependency.package_identity = "other-package";
  b.dependencies = {{wrong_dependency, true, true}};
  bridge.ConfigureSourceRegistry({RegistryEntry("fixture.B")}, imported, {a, b});
  assert(bridge.Activate("fixture.B") == SourceModuleImportStatus::Rejected);
  assert(bridge.GetVisibleModules().empty());

  // Reconfiguration clears old on-demand mode and its diagnostic state.
  auto consumer = Parse("", "configuration-consumer.mm");
  assert(consumer);
  bridge.ConfigureOnDemand({}, {RegistryEntry("fixture.A")}, {},
                           consumer->getFileManager());
  assert(bridge.LookupQualified("fixture.A", "not_loaded_yet", 0).status ==
         QualifiedModuleLookupStatus::ModuleNotVisible);
  assert(bridge.GetImportedModuleIdentities().empty());
  assert(bridge.GetDiagnostics().empty());
  assert(bridge.Activate("fixture.A") == SourceModuleImportStatus::Rejected);
  assert(!bridge.GetDiagnostics().empty());
  bridge.ConfigureSourceRegistry({RegistryEntry("fixture.A")}, imported, {a});
  assert(bridge.GetDiagnostics().empty());
  assert(bridge.Activate("fixture.A") == SourceModuleImportStatus::Imported);
}

InterfaceModuleRequest Locked(const InterfaceModuleIdentity &identity) {
  InterfaceModuleRequest request;
  request.canonical_name = identity.canonical_name;
  request.locked_identity = identity;
  return request;
}

CompileRequest SourceRequest(const std::filesystem::path &root,
                            std::string source) {
  CompileRequest request;
  request.input_path = "source-import-bridge-consumer.m";
  request.language = BaseLanguage::ObjectiveC;
  request.objc3 = true;
  request.arguments = {"--target=x86_64-unknown-linux-gnu"};
  request.source_text = std::move(source);
  request.interface_search_paths = {root.string()};
  return request;
}

InterfaceEmissionArtifact Produce(const std::filesystem::path &root,
                                  const std::string &name, std::string source,
                                  std::vector<InterfaceModuleRequest> dependencies = {}) {
  auto request = SourceRequest(root, std::move(source));
  request.input_path = name + ".m";
  request.module_name = name;
  request.package_identity = "package-" + name;
  request.emit_semantic_interface = true;
  if (dependencies.empty()) request.interface_search_paths.clear();
  request.module_imports = std::move(dependencies);
  auto compiled = Compile(request);
  assert(compiled.success && compiled.interface_artifact &&
         compiled.interface_artifact->success);
  auto artifact = *compiled.interface_artifact;
  assert(WriteSemanticInterfaceArtifact(
      artifact, (root / (name + ".objc3i")).string()).success);
  return artifact;
}

void TestOnDemandRecoveryAndMetadata() {
  const auto root = std::filesystem::temp_directory_path() /
      ("objc3c-source-import-bridge-" + std::to_string(
          std::chrono::high_resolution_clock::now().time_since_epoch().count()));
  std::filesystem::create_directories(root);
  auto a = Produce(root, "bridge.A",
      "typedef struct ImportedType { int value; } ImportedType;\n");
  auto b = Produce(root, "bridge.B", "int api(ImportedType *);\n", {Locked(a.identity)});
  auto c = Produce(root, "bridge.C", "long api(long);\n");
  auto reexport = Locked(a.identity);
  reexport.reexported = true;
  auto r = Produce(root, "bridge.R", "int reexport_api(ImportedType *);\n", {reexport});

  // A resolution error is preserved, but it does not poison the next module's
  // preparation or publish rejected-import metadata.
  auto missing = RegistryEntry("bridge.Missing");
  auto request = SourceRequest(root,
      "@import bridge.Missing;\n@import bridge.B;\n"
      "int use_api(void) { return api(0); }\n");
  request.source_directed_imports = true;
  request.module_imports = {missing, Locked(b.identity)};
  auto recovered = Compile(request);
  assert(!recovered.success);
  assert(recovered.visible_module_names == std::vector<std::string>{"bridge.B"});
  assert(recovered.resolved_module_identities.size() == 2);
  assert(recovered.resolved_interface_dependencies.size() == 1);
  assert(!recovered.interface_import_diagnostics.empty());

  // The second incompatible C import fails before exposing or recording C.
  request.source_text = "@import bridge.B;\n@import bridge.C;\n";
  request.module_imports = {Locked(b.identity), Locked(c.identity)};
  auto conflicting = Compile(request);
  assert(!conflicting.success);
  assert(conflicting.visible_module_names == std::vector<std::string>{"bridge.B"});
  assert(conflicting.resolved_module_identities.size() == 2);
  assert(conflicting.resolved_interface_dependencies.size() == 1);
  assert(conflicting.interface_dependency_order ==
         (std::vector<std::string>{"bridge.A", "bridge.B"}));
  assert(conflicting.failure.find("conflicts with visible declaration") != std::string::npos);

  // A loaded private dependency is activated by a direct import, even when
  // that import itself is private to this consumer.
  auto private_a = Locked(a.identity);
  private_a.public_import = false;
  request.source_text = "@import bridge.B;\n@import bridge.A;\n"
      "ImportedType value;\n";
  request.module_imports = {Locked(b.identity), private_a};
  auto activated = Compile(request);
  assert(activated.success);
  assert(activated.visible_module_names ==
         (std::vector<std::string>{"bridge.A", "bridge.B"}));
  assert(activated.resolved_module_identities.size() == 2);
  assert(activated.resolved_interface_dependencies.size() == 2);
  assert(!activated.resolved_interface_dependencies.front().public_import);

  auto wrong_lock = private_a;
  wrong_lock.locked_identity.package_identity = "other-package";
  request.module_imports = {Locked(b.identity), wrong_lock};
  auto locked_conflict = Compile(request);
  assert(!locked_conflict.success);
  assert(locked_conflict.visible_module_names == std::vector<std::string>{"bridge.B"});
  assert(locked_conflict.resolved_interface_dependencies.size() == 1);
  assert(locked_conflict.failure.find("source import lock conflicts") != std::string::npos);

  // Independent source roots sharing a dependency must preserve the same
  // consumer type, including after the first import hid that dependency.
  request.source_text = "@import bridge.B;\n@import bridge.R;\n"
      "_Static_assert(__builtin_types_compatible_p(__typeof__(api), "
      "__typeof__(reexport_api)), \"shared imported record type\");\n"
      "int use_shared_dependency(void) { ImportedType value = {1}; "
      "return api(&value) + reexport_api(&value); }\n";
  request.module_imports = {Locked(b.identity), Locked(r.identity)};
  auto shared = Compile(request);
  assert(shared.success);
  assert(shared.resolved_module_identities.size() == 3);
  assert(shared.resolved_interface_dependencies.size() == 2);
  assert(shared.visible_module_names ==
         (std::vector<std::string>{"bridge.A", "bridge.B", "bridge.R"}));

  // Re-importing a re-exported dependency records it as a direct private
  // import and does not re-import its declarations.
  request.source_text = "@import bridge.R;\n@import bridge.A;\n"
      "@import bridge.A;\n@bridge.A.ImportedType value;\n";
  request.module_imports = {Locked(r.identity), private_a};
  auto direct = Compile(request);
  assert(direct.success);
  assert(direct.visible_module_names ==
         (std::vector<std::string>{"bridge.A", "bridge.R"}));
  assert(direct.resolved_interface_dependencies.size() == 2);
  assert(!direct.resolved_interface_dependencies.front().public_import);
  std::filesystem::remove(root / "bridge.A.objc3i");
  std::filesystem::remove(root / "bridge.A.objc3i.json");
  std::filesystem::remove(root / "bridge.B.objc3i");
  std::filesystem::remove(root / "bridge.B.objc3i.json");
  std::filesystem::remove(root / "bridge.C.objc3i");
  std::filesystem::remove(root / "bridge.C.objc3i.json");
  std::filesystem::remove(root / "bridge.R.objc3i");
  std::filesystem::remove(root / "bridge.R.objc3i.json");
  std::filesystem::remove(root);
}

int main() {
  TestPreloadedLookups();
  TestConfigurationAndActivationFailures();
  TestOnDemandRecoveryAndMetadata();
}

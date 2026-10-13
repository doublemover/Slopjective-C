#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_artifact_io.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/StringRef.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>

using namespace objc3::clang_foundation;
namespace fs = std::filesystem;

namespace {
CompileRequest ModuleRequest(const fs::path &input, const std::string &name,
                            const std::string &source) {
  CompileRequest request;
  request.input_path = input.string();
  request.source_text = source;
  request.language = BaseLanguage::ObjectiveC;
  request.objc3 = true;
  request.target_triple = "x86_64-unknown-linux-gnu";
  request.objc_runtime = "gnustep-2.0";
  request.arguments = {"-std=c17"};
  request.emit_semantic_interface = true;
  request.module_name = name;
  request.package_identity = "test-package-v1";
  return request;
}

InterfaceImportInput ImportOf(const fs::path &path,
                              const InterfaceModuleIdentity &identity,
                              bool public_import = true) {
  InterfaceImportInput input;
  input.source_path = path.string();
  input.locked_identity = identity;
  input.public_import = public_import;
  return input;
}

CompileResult EmitModule(const fs::path &input, const fs::path &interface_path,
                         const std::string &name, const std::string &source,
                         const std::vector<InterfaceImportInput> &imports = {}) {
  auto request = ModuleRequest(input, name, source);
  request.imported_interfaces = imports;
  request.interface_output_path = interface_path.string();
  return Compile(request);
}

bool HasImported(const CompileResult &result, const std::string &name) {
  for (const auto &decl : result.imported_declarations)
    if (decl.qualified_name == name || decl.qualified_name.ends_with("::" + name))
      return true;
  return false;
}

void TestThreeModuleSemanticLookupAndIdentity() {
  const auto root = fs::temp_directory_path() / "objc3-interface-import-test";
  fs::remove_all(root);
  fs::create_directories(root);
  const auto a_path = root / "a.objc3i";
  const auto b_path = root / "b.objc3i";

  auto a = EmitModule(root / "a.m", a_path, "test.provider_a",
      "typedef struct ErrorA { int code; } ErrorA; "
      "typedef __objc3_optional(int) MaybeInt; "
      "typedef __objc3_result(int, ErrorA) IntResult; "
      "int provider(void) __objc3_throws(ErrorA);");
  assert(a.success && a.interface_artifact && a.interface_artifact->success);
  assert(a.interface_artifact->source_bytes.find("__objc3_optional") != std::string::npos);
  assert(a.interface_artifact->sidecar_json.find("canonical-function-effects.v1") != std::string::npos);
  assert(a.interface_artifact->sidecar_json.find("native-carrier-types.v1") != std::string::npos);

  auto b = EmitModule(root / "b.m", b_path, "test.provider_b",
      "int relay(void) { return 17; }",
      {ImportOf(a_path, a.interface_artifact->identity)});
  assert(b.success && b.interface_artifact && b.interface_artifact->success);
  assert(b.interface_artifact->sidecar_json.find("test.provider_a") != std::string::npos);
  assert(b.resolved_interface_dependencies.size() == 1);
  assert(b.resolved_interface_dependencies.front().module.canonical_name ==
         "test.provider_a");

  auto consumer = ModuleRequest(root / "consumer.m", "",
      "int consume(void) __objc3_throws(ErrorA) { "
      "  MaybeInt zero = __objc3_some(int, 0); "
      "  IntResult value = __objc3_ok(int, ErrorA, 0); "
      "  return relay() + __objc3_try(provider()) + "
      "         __objc3_has_value(zero) + __objc3_is_ok(value); "
      "}");
  consumer.emit_semantic_interface = false;
  consumer.imported_interfaces = {
      ImportOf(a_path, a.interface_artifact->identity),
      ImportOf(b_path, b.interface_artifact->identity)};
  auto c = Compile(consumer);
  assert(c.success && c.imported_declarations.size() >= 6);
  assert(HasImported(c, "provider") && HasImported(c, "relay"));
  assert(c.interface_dependency_order.size() == 2);
  assert(c.interface_dependency_order[0] == "test.provider_a");
  assert(c.interface_dependency_order[1] == "test.provider_b");
  assert(c.resolved_interface_dependencies.size() == 2);
  assert(c.resolved_interface_dependencies[0].module.canonical_name ==
         "test.provider_a");
  assert(c.resolved_interface_dependencies[1].module.canonical_name ==
         "test.provider_b");
  bool saw_typed_effect = false, saw_optional = false, saw_result = false;
  for (const auto &decl : c.imported_declarations) {
    if (decl.qualified_name == "provider") {
      saw_typed_effect = decl.effects.is_throwing &&
          decl.effects.throws_kind == "typed" &&
          decl.effects.error_type_identity.find("ErrorA") != std::string::npos;
    }
    if (decl.qualified_name == "MaybeInt")
      saw_optional = decl.canonical_type_identity.find("__objc3_optional") != std::string::npos;
    if (decl.qualified_name == "IntResult")
      saw_result = decl.canonical_type_identity.find("__objc3_result") != std::string::npos;
  }
  assert(saw_typed_effect && saw_optional && saw_result);
  fs::remove_all(root);
}

void TestBadLockAndMissingTransitiveArtifactFailBeforeImport() {
  const auto root = fs::temp_directory_path() / "objc3-interface-import-invalid-test";
  fs::remove_all(root);
  fs::create_directories(root);
  const auto a_path = root / "a.objc3i";
  const auto b_path = root / "b.objc3i";
  auto a = EmitModule(root / "a.m", a_path, "test.locked_a", "int a(void);");
  assert(a.success && a.interface_artifact);
  auto b = EmitModule(root / "b.m", b_path, "test.locked_b", "int b(void);",
      {ImportOf(a_path, a.interface_artifact->identity)});
  assert(b.success && b.interface_artifact);

  auto missing = ModuleRequest(root / "missing.m", "", "int use_b(void) { return b(); }");
  missing.emit_semantic_interface = false;
  missing.imported_interfaces = {ImportOf(b_path, b.interface_artifact->identity)};
  auto missing_result = Compile(missing);
  assert(!missing_result.success);
  bool missing_dependency = false;
  for (const auto &d : missing_result.interface_import_diagnostics)
    missing_dependency |= d.code == "O3IFC023";
  assert(missing_dependency);

  auto stale = ImportOf(a_path, a.interface_artifact->identity);
  stale.locked_identity.package_identity = "wrong-package";
  auto wrong_lock = ModuleRequest(root / "wrong.m", "", "int use_a(void) { return a(); }");
  wrong_lock.emit_semantic_interface = false;
  wrong_lock.imported_interfaces = {stale};
  auto wrong_result = Compile(wrong_lock);
  assert(!wrong_result.success);
  bool lock_diagnostic = false;
  for (const auto &d : wrong_result.interface_import_diagnostics)
    lock_diagnostic |= d.code == "O3IFC134";
  assert(lock_diagnostic);
  fs::remove_all(root);
}

void TestDuplicateLookupNamesAreRejected() {
  const auto root = fs::temp_directory_path() / "objc3-interface-import-ambiguity-test";
  fs::remove_all(root);
  fs::create_directories(root);
  const auto a_path = root / "a.objc3i";
  const auto b_path = root / "b.objc3i";
  auto a = EmitModule(root / "a.m", a_path, "test.duplicate_a", "int collide(void);");
  auto b = EmitModule(root / "b.m", b_path, "test.duplicate_b", "long collide(void);");
  assert(a.success && b.success && a.interface_artifact && b.interface_artifact);
  auto consumer = ModuleRequest(root / "use.m", "", "int use(void) { return collide(); }");
  consumer.emit_semantic_interface = false;
  consumer.imported_interfaces = {
      ImportOf(a_path, a.interface_artifact->identity),
      ImportOf(b_path, b.interface_artifact->identity)};
  auto result = Compile(consumer);
  assert(!result.success);
  bool duplicate_diagnostic = false;
  for (const auto &d : result.interface_import_diagnostics)
    duplicate_diagnostic |= d.code == "O3IFC132";
  assert(duplicate_diagnostic);
  fs::remove_all(root);
}

void TestCompleteExportIdentitiesAndPrivateDirectLookup() {
  const auto root = fs::temp_directory_path() / "objc3-interface-import-identities-test";
  fs::remove_all(root);
  fs::create_directories(root);
  const auto path = root / "identities.objc3i";
  auto provider = EmitModule(root / "identities.m", path, "test.identities",
      "struct ExportedRecord { int code; };\n"
      "enum ExportedStatus { StatusGood = 7, StatusBad = -3 };\n"
      "int record_code(struct ExportedRecord value);\n");
  assert(provider.success && provider.interface_artifact);
  const std::vector<llvm::StringRef> semantics = {
      "objc3.native-carrier-types.v1", "objc3.canonical-function-effects.v1"};
  auto sidecar = DecodeSemanticInterfaceSidecar(
      provider.interface_artifact->sidecar_json,
      provider.interface_artifact->source_bytes, semantics);
  assert(sidecar);

  auto private_direct = ImportOf(path, provider.interface_artifact->identity, false);
  auto transitive = ImportOf(path, provider.interface_artifact->identity, true);
  transitive.direct_import = false;
  transitive.reexported = true;
  auto consumer = ModuleRequest(root / "consumer.m", "",
      "int use_exports(void) { struct ExportedRecord value = { StatusGood }; "
      "return record_code(value) + StatusBad; }\n");
  consumer.emit_semantic_interface = false;
  // A repeated transitive input must not erase a later direct private import,
  // or leak its public/reexport flags into that direct dependency record.
  for (bool direct_first : {false, true}) {
    consumer.imported_interfaces = direct_first
        ? std::vector<InterfaceImportInput>{private_direct, transitive}
        : std::vector<InterfaceImportInput>{transitive, private_direct};
    auto result = Compile(consumer);
    assert(result.success);
    assert(result.visible_module_names == std::vector<std::string>{"test.identities"});
    assert(result.resolved_interface_dependencies.size() == 1);
    assert(!result.resolved_interface_dependencies.front().public_import);
    assert(!result.resolved_interface_dependencies.front().reexported);
    assert(result.imported_declarations.size() == sidecar->declarations.size());
    for (const auto &expected : sidecar->declarations) {
      auto actual = std::find_if(result.imported_declarations.begin(),
          result.imported_declarations.end(), [&](const auto &decl) {
            return decl.stable_id == expected.stable_id;
          });
      assert(actual != result.imported_declarations.end());
      assert(SameInterfaceDeclarationIdentity(expected, *actual));
    }
  }
  consumer.source_text = "int field_is_not_global(void) { return code; }\n";
  assert(!Compile(consumer).success);
  fs::remove_all(root);
}

void TestRelativeArtifactsAndResponseFileLayoutFlags() {
  const auto caller_directory = fs::current_path();
  const auto root = fs::temp_directory_path() / "objc3-interface-import-working-directory";
  fs::remove_all(root);
  fs::create_directories(root);
  {
    std::ofstream response(root / "import flags.rsp");
    response << "-fshort-enums -std=c17\n";
    assert(response);
  }
  auto provider = ModuleRequest("packet.m", "test.response_layout",
      "enum Tiny { TinyGood = 1, TinyBad = 2 };\n"
      "int packet_code(enum Tiny value);\n");
  provider.working_directory = root.string();
  provider.arguments = {"@import flags.rsp"};
  provider.interface_output_path = (root / "packet.objc3i").string();
  auto emitted = Compile(provider);
  assert(emitted.success && emitted.interface_artifact);
  assert(fs::current_path() == caller_directory);

  auto consumer = ModuleRequest("consumer.m", "",
      "int use_packet(void) { return packet_code(TinyGood); }\n");
  consumer.emit_semantic_interface = false;
  consumer.working_directory = root.string();
  consumer.arguments = {"@import flags.rsp"};
  auto input = ImportOf("packet.objc3i", emitted.interface_artifact->identity);
  input.sidecar_path = "packet.objc3i.json";
  consumer.imported_interfaces = {input};
  assert(Compile(consumer).success);
  assert(fs::current_path() == caller_directory);

  // The same relative search root and response file apply on resolver and
  // source-directed paths, including the canonical interface's enum layout.
  InterfaceModuleRequest locked;
  locked.canonical_name = emitted.interface_artifact->identity.canonical_name;
  locked.locked_identity = emitted.interface_artifact->identity;
  consumer.imported_interfaces.clear();
  consumer.interface_search_paths = {"."};
  consumer.module_imports = {locked};
  assert(Compile(consumer).success);
  assert(fs::current_path() == caller_directory);
  consumer.source_directed_imports = true;
  consumer.source_text = "@import test.response_layout;\n"
      "int use_packet(void) { return packet_code(TinyGood); }\n";
  assert(Compile(consumer).success);
  assert(fs::current_path() == caller_directory);

  // Without the expanded ABI flag, reconstruction must reject the changed
  // enum representation even though the selected module/package lock is unchanged.
  consumer.arguments = {"-std=c17"};
  assert(!Compile(consumer).success);
  assert(fs::current_path() == caller_directory);
  fs::remove_all(root);
}
} // namespace

int main() {
  TestThreeModuleSemanticLookupAndIdentity();
  TestBadLockAndMissingTransitiveArtifactFailBeforeImport();
  TestDuplicateLookupNamesAreRejected();
  TestCompleteExportIdentitiesAndPrivateDirectLookup();
  TestRelativeArtifactsAndResponseFileLayoutFlags();
  return 0;
}

#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_artifact_io.h"
#include "clang_foundation/interface_codec.h"
#include "clang_foundation/runtime_interface_types.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/ArrayRef.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
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
  request.objc_runtime = "gnustep-2.2";
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

void TestPreparedImportBudgets() {
  const auto root = fs::temp_directory_path() / "objc3-prepared-import-budgets";
  fs::remove_all(root);
  fs::create_directories(root);
  const auto path_a = root / "a.objc3i", path_b = root / "b.objc3i";
  auto a = EmitModule(root / "a.m", path_a, "budget.A", "typedef struct BudgetValue { int value; } BudgetValue;");
  assert(a.success && a.interface_artifact);
  const auto input_a = ImportOf(path_a, a.interface_artifact->identity);
  auto b = EmitModule(root / "b.m", path_b, "budget.B", "BudgetValue echo(BudgetValue value);", {input_a});
  assert(b.success && b.interface_artifact);
  const std::vector<InterfaceImportInput> inputs = {input_a, ImportOf(path_b, b.interface_artifact->identity)};
  InterfaceImportTarget target;
  const auto &id = a.interface_artifact->identity;
  target.target_triple = id.target_triple;
  target.runtime_abi = id.runtime_abi;
  target.extension_abi = id.extension_abi;
  target.language_revision = id.language_revision;
  target.language_profile = id.language_profile;
  target.objc = target.objc3 = true;
  target.objc_runtime = "gnustep-2.2";
  target.ast_arguments = BuildInterfaceASTArguments({}, "objective-c", "c17", target);
  std::vector<InterfaceContractDiagnostic> diagnostics;
  auto prepared = InterfaceImportSession::Prepare(inputs, target, diagnostics);
  for (const auto &d : diagnostics) std::fprintf(stderr, "%s: %s\n", d.code.c_str(), d.message.c_str());
  assert(prepared && diagnostics.empty());
  const auto usage = prepared->GetResourceUsage();
  assert(usage.selected_modules == 2 && usage.dependency_edges == 1);
  assert(usage.parser_input_bytes > 0 && usage.parser_declarations > 0);
  using Field = std::uint64_t InterfaceResourceLimits::*;
  for (auto [field, exact] : std::vector<std::pair<Field, std::uint64_t>>{
      {&InterfaceResourceLimits::read_bytes, usage.read_bytes},
      {&InterfaceResourceLimits::selected_bytes, usage.selected_bytes},
      {&InterfaceResourceLimits::decoded_declarations, usage.decoded_declarations},
      {&InterfaceResourceLimits::selected_declarations, usage.selected_declarations},
      {&InterfaceResourceLimits::selected_modules, usage.selected_modules},
      {&InterfaceResourceLimits::dependency_edges, usage.dependency_edges},
      {&InterfaceResourceLimits::parser_input_bytes, usage.parser_input_bytes},
      {&InterfaceResourceLimits::parser_declarations, usage.parser_declarations}}) {
    assert(exact > 0);
    auto policy = target;
    policy.resource_limits.*field = exact;
    diagnostics.clear();
    assert(InterfaceImportSession::Prepare(inputs, policy, diagnostics) && diagnostics.empty());
    policy.resource_limits.*field = exact - 1;
    diagnostics.clear();
    assert(!InterfaceImportSession::Prepare(inputs, policy, diagnostics));
    assert(std::any_of(diagnostics.begin(), diagnostics.end(),
        [](const auto &d) { return d.code == kInterfaceResourceDiagnostic; }));
  }
  // Resolving a lock does not authorize subsequent byte changes. Preparation
  // must fail before any declarations can enter the consumer AST.
  std::ofstream(path_a, std::ios::app) << "\n// changed after resolution\n";
  diagnostics.clear();
  assert(!InterfaceImportSession::Prepare(inputs, target, diagnostics));
  assert(!diagnostics.empty());
  fs::remove_all(root);
}

void TestPrimitiveABIProfilesRejectMismatchedConsumers() {
  const auto root = fs::temp_directory_path() / "objc3-primitive-abi-import-test";
  fs::remove_all(root);
  fs::create_directories(root);
  for (const auto &flag : {"-funsigned-char", "-fshort-wchar", "-fshort-enums",
                           "-fpack-struct=1", "-mavx2"}) {
    const auto path = root / "provider.objc3i";
    auto provider = ModuleRequest(root / "provider.m", "fixture.abi",
                                  "char byte_value(void); extern int counter;");
    provider.arguments.push_back(flag);
    provider.interface_output_path = path.string();
    auto emitted = Compile(provider);
    assert(emitted.success && emitted.interface_artifact);
    auto consumer = ModuleRequest(root / "consumer.m", "",
                                  "int use(void) { return byte_value() + counter; }");
    consumer.emit_semantic_interface = false;
    consumer.imported_interfaces = {ImportOf(path, emitted.interface_artifact->identity)};
    auto rejected = Compile(consumer);
    assert(!rejected.success && rejected.imported_declarations.empty());
    assert(std::any_of(rejected.interface_import_diagnostics.begin(),
                       rejected.interface_import_diagnostics.end(),
                       [](const auto &d) { return d.code == "O3IFC135"; }));
    consumer.arguments.push_back(flag);
    auto admitted = Compile(consumer);
    for (const auto &d : admitted.interface_import_diagnostics)
      std::fprintf(stderr, "%s: %s\n", d.code.c_str(), d.message.c_str());
    assert(admitted.success && HasImported(admitted, "byte_value"));
  }
  fs::remove_all(root);
}

void TestThreeModuleSemanticLookupAndIdentity() {
  const auto root = fs::temp_directory_path() / "objc3-interface-import-test";
  fs::remove_all(root);
  fs::create_directories(root);
  const auto a_path = root / "a.objc3i";
  const auto b_path = root / "b.objc3i";

  auto a = EmitModule(root / "a.m", a_path, "test.provider_a",
                      "typedef struct ErrorA { int code; } ErrorA; "
                      "typedef Optional(int) MaybeInt; "
                      "typedef Result(int, ErrorA) IntResult; "
                      "int provider(void) throws(ErrorA);");
  assert(a.success && a.interface_artifact && a.interface_artifact->success);
  assert(a.interface_artifact->source_bytes.find("Optional") !=
         std::string::npos);
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
                                "int consume(void) throws(ErrorA) { "
                                "  MaybeInt zero = Some(int, 0); "
                                "  IntResult value = Ok(int, ErrorA, 0); "
                                "  return relay() + tryValue(provider()) + "
                                "         hasValue(zero) + isOk(value); "
                                "}");
  consumer.emit_semantic_interface = false;
  consumer.interface_output_path.clear();
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
      saw_optional =
          decl.canonical_type_identity.find("Optional") != std::string::npos;
    if (decl.qualified_name == "IntResult")
      saw_result =
          decl.canonical_type_identity.find("Result") != std::string::npos;
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
  consumer.interface_output_path.clear();
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

void TestCxxBaseAndDefaultedImports() {
  const auto root = fs::temp_directory_path() / "objc3-cxx-structure-import";
  fs::remove_all(root); fs::create_directories(root);
  for (const char *standard : {"-std=c++20", "-std=c++23"}) {
    auto provider = ModuleRequest(root / "provider.cpp", "fixture.structure",
        "struct Empty {}; struct Base { int n; }; "
        "class Access : private Base, protected Empty { public: Access(int); int get() const; }; "
        "struct Derived : Base, Empty { Derived()=default; Derived(const Derived&)=default; ~Derived()=default; }; ");
    provider.language = BaseLanguage::Cxx; provider.objc3 = false; provider.objc_runtime.clear();
    provider.arguments = {standard};
    provider.interface_output_path = (root / "structure.objc3i").string();
    auto built = Compile(provider);
    assert(built.success && built.interface_artifact);
    auto consumer = provider;
    consumer.module_name.clear(); consumer.package_identity.clear();
    consumer.interface_output_path.clear(); consumer.emit_semantic_interface = false;
    consumer.input_path = (root / "consumer.cpp").string();
    consumer.source_text =
        "template<class T> concept public_base = requires(T *p) { static_cast<Base *>(p); }; "
        "static_assert(public_base<Derived>); static_assert(!public_base<Access>); "
        "static_assert(__is_standard_layout(Access)); "
        "int use() { Derived d{}; d.n=42; Derived copy(d); Access a(42); return copy.n+a.get(); }";
    consumer.imported_interfaces = {ImportOf(provider.interface_output_path, built.interface_artifact->identity)};
    auto imported = Compile(consumer);
    assert(imported.success && HasImported(imported, "Derived"));
  }
  fs::remove_all(root);
}

void TestLaterCxxOverloadConflicts() {
  const auto root = fs::temp_directory_path() / "objc3-cxx-overload-collision";
  fs::remove_all(root);
  fs::create_directories(root);
  auto cpp = [&](const char *file, const char *module, const char *source) {
    auto request = ModuleRequest(root / file, module, source);
    request.language = BaseLanguage::Cxx;
    request.objc3 = false;
    request.objc_runtime.clear();
    request.arguments = {"-std=c++20"};
    request.interface_output_path =
        (root / (std::string(module) + ".objc3i")).string();
    return request;
  };
  auto left = cpp("left.cpp", "left", "int clash(int); long clash(long);");
  auto right = cpp("right.cpp", "right", "double clash(long);");
  auto a = Compile(left), b = Compile(right);
  assert(a.success && b.success);
  auto consumer = cpp("client.cpp", "", "void use() {}");
  consumer.emit_semantic_interface = false;
  consumer.interface_output_path.clear();
  consumer.imported_interfaces = {
      ImportOf(left.interface_output_path, a.interface_artifact->identity),
      ImportOf(right.interface_output_path, b.interface_artifact->identity)};
  auto rejected = Compile(consumer);
  assert(!rejected.success);
  assert(std::any_of(rejected.interface_import_diagnostics.begin(),
                     rejected.interface_import_diagnostics.end(),
                     [](const auto &d) { return d.code == "O3IFC132"; }));
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
void TestPinnedRuntimeTypesSurvivePreparedImport() {
  const auto root=fs::temp_directory_path()/"objc3-runtime-declaration-import";
  fs::remove_all(root);
  fs::create_directories(root/"include/objc3c/runtime");
  {
    std::ofstream header(root/"include/objc3c/runtime/objc3_runtime_services.h");
    header<<RuntimeInterfaceHeaderSource().str();
  }
  auto provider=ModuleRequest(root/"provider.m","test.runtime_types",
      "#include <objc3c/runtime/objc3_runtime_services.h>\n"
      "int runtime_actor(objc3_actor_t *actor);\n"
      "objc3_task_t *runtime_task(objc3_task_t *task);\n");
  provider.arguments.push_back("-I"+(root/"include").string());
  const auto interface_path=root/"runtime.objc3i";
  provider.interface_output_path=interface_path.string();
  auto emitted=Compile(provider);
  assert(emitted.success && emitted.interface_artifact && emitted.interface_artifact->success);
  fs::remove_all(root/"include");
  auto consumer=ModuleRequest(root/"consumer.m","",
      "int use_runtime(void) {return runtime_actor((objc3_actor_t *)0) +\n"
      "    (runtime_task((objc3_task_t *)0)!=(objc3_task_t *)0);}\n");
  consumer.emit_semantic_interface=false;
  consumer.imported_interfaces={ImportOf(interface_path,emitted.interface_artifact->identity)};
  auto imported=Compile(consumer);
  assert(imported.success && HasImported(imported,"runtime_actor") &&
         HasImported(imported,"runtime_task"));
  fs::remove_all(root);
}

void TestNativeAttributeTemplateImports() {
  const auto root=fs::temp_directory_path()/"objc3-native-attribute-import";
  fs::remove_all(root);fs::create_directories(root);
  std::ifstream input(fs::path(OBJC3_INTERFACE_TEST_PACKAGE_FIXTURES)/"provider/declaration_attributes.h");
  assert(input);const std::string source((std::istreambuf_iterator<char>(input)),{});
  for(const char *standard:{"-std=c++20","-std=c++23"}) {
    auto provider=ModuleRequest(root/"provider.mm","test.native_attributes",source);
    provider.language=BaseLanguage::ObjectiveCxx;provider.arc=true;
    provider.arguments={standard};provider.module_owned_names=true;
    provider.interface_exports={"NativeDeclarationAttributes"};
    const auto interface_path=root/"test.native_attributes.objc3i";
    provider.interface_output_path=interface_path.string();
    auto emitted=Compile(provider);assert(emitted.success && emitted.interface_artifact);
    for(bool directed:{false,true}) {
      auto consumer=ModuleRequest(root/"consumer.mm","",
          std::string(directed ? "@import test.native_attributes;\n" : "")+
          "static_assert(sizeof(NativeDeclarationAttributes::Packet)==5);"
          "static_assert(alignof(NativeDeclarationAttributes::Slot<int,32>)==32);"
          "int use(){NativeDeclarationAttributes::ForeignBox<int> value{42};"
          "NativeDeclarationAttributes::EnabledMember conditional{40};"
          "return value.value+NativeDeclarationAttributes::Family<int>{40}.call(2)+"
          "NativeDeclarationAttributes::enabled(40)+NativeDeclarationAttributes::enabled(-40)+"
          "NativeDeclarationAttributes::enabled_template(40)+conditional.choose(-2);}");
      consumer.language=BaseLanguage::ObjectiveCxx;consumer.arc=true;
      consumer.arguments={standard};consumer.emit_semantic_interface=false;
      consumer.source_directed_imports=directed;
      if(directed) {
        InterfaceModuleRequest locked;locked.canonical_name="test.native_attributes";
        locked.locked_identity=emitted.interface_artifact->identity;
        consumer.interface_search_paths={root.string()};consumer.module_imports={locked};
      } else consumer.imported_interfaces={ImportOf(interface_path,emitted.interface_artifact->identity)};
      auto result=Compile(consumer);
      if(!result.success)std::fprintf(stderr,"%s\n",result.failure.c_str());
      for(const auto &diagnostic:result.diagnostics)std::fprintf(stderr,"%s\n",diagnostic.message.c_str());
      assert(result.success);
      auto invalid=consumer;
      invalid.source_text=std::string(directed?"@import test.native_attributes;\n":"")+
          "void invalid(){NativeDeclarationAttributes::checked(-1);"
          "NativeDeclarationAttributes::checked_template(-1);"
          "NativeDeclarationAttributes::inline_enabled(-1);NativeDeclarationAttributes::nonnegative_template(-1);"
          "auto pointer=&NativeDeclarationAttributes::provider_object_size;}";
      auto rejected=Compile(invalid);assert(!rejected.success);
      for(const char *message:{"negative value","negative template value","pass_object_size"})
        assert(std::any_of(rejected.diagnostics.begin(),rejected.diagnostics.end(),[&](const auto &d){return d.message.find(message)!=std::string::npos;}));
    }
  }
  fs::remove_all(root);
}

void TestUnnamedTemplateProjectionImports() {
  const auto root=fs::temp_directory_path()/"objc3-unnamed-template-import";
  fs::remove_all(root);fs::create_directories(root);
  std::ifstream input(fs::path(OBJC3_INTERFACE_TEST_PACKAGE_FIXTURES)/"provider/unnamed_template_projections.h");
  assert(input);const std::string source((std::istreambuf_iterator<char>(input)),{});
  for (const char *standard:{"-std=c++20","-std=c++23"}) {
    auto provider=ModuleRequest(root/"provider.mm","test.unnamed_projections",source);
    provider.language=BaseLanguage::ObjectiveCxx;provider.arc=true;provider.arguments={standard};
    provider.module_owned_names=true;provider.interface_exports={"NativeUnnamedTemplateProjections"};
    auto path=root/"test.unnamed_projections.objc3i";provider.interface_output_path=path.string();
    auto emitted=Compile(provider);
    if(!emitted.success)std::fprintf(stderr,"UNNAMED PROVIDER %s\n",emitted.failure.c_str());
    assert(emitted.success && emitted.interface_artifact);
    for(bool directed:{false,true}) {
      auto consumer=ModuleRequest(root/"consumer.mm","",std::string(directed?"@import test.unnamed_projections;\n":"")+R"(
        namespace P=NativeUnnamedTemplateProjections;
        static_assert(!__is_same(P::First,P::Second));
        static_assert(!__is_same(P::Ordered,P::Reversed));
        int use(){P::First a{17};P::Second b{25};P::Wrapped box{{42}};
          return P::provider_read(a,b)+P::provider_wrapped(box).value.value;}
      )");
      consumer.language=BaseLanguage::ObjectiveCxx;consumer.arc=true;consumer.arguments={standard};
      consumer.emit_semantic_interface=false;consumer.source_directed_imports=directed;
      if(directed) {
        InterfaceModuleRequest locked;locked.canonical_name="test.unnamed_projections";locked.locked_identity=emitted.interface_artifact->identity;
        consumer.interface_search_paths={root.string()};consumer.module_imports={locked};
      } else consumer.imported_interfaces={ImportOf(path,emitted.interface_artifact->identity)};
      auto result=Compile(consumer);
      if(!result.success)std::fprintf(stderr,"UNNAMED IMPORT %s\n",result.failure.c_str());
      for(const auto &d:result.diagnostics)std::fprintf(stderr,"%s\n",d.message.c_str());
      assert(result.success);
    }
    auto bridge=ModuleRequest(root/"bridge.mm","test.unnamed_bridge",R"(
      namespace Bridge {
        using First=NativeUnnamedTemplateProjections::First;
        using Wrapped=NativeUnnamedTemplateProjections::Wrapped;
        using Kind=NativeUnnamedTemplateProjections::Kind;
        inline Kind pass_kind(Kind value){return value;}
        inline First pass(First value){return value;}
        inline Wrapped pass_box(Wrapped value){return value;}
      }
    )");
    bridge.language=BaseLanguage::ObjectiveCxx;bridge.arc=true;bridge.arguments={standard};bridge.module_owned_names=true;
    bridge.interface_exports={"Bridge"};bridge.imported_interfaces={ImportOf(path,emitted.interface_artifact->identity)};
    auto bridge_path=root/"test.unnamed_bridge.objc3i";bridge.interface_output_path=bridge_path.string();
    auto projected=Compile(bridge);
    if(!projected.success)std::fprintf(stderr,"UNNAMED BRIDGE %s\n",projected.failure.c_str());
    assert(projected.success && projected.interface_artifact);
    auto consumer=ModuleRequest(root/"second-consumer.mm","",R"(
      int use(){Bridge::First value{42};Bridge::Wrapped box{{42}};
        return Bridge::pass(value).value+Bridge::pass_box(box).value.value+static_cast<int>(Bridge::pass_kind(Bridge::Kind::answer));}
    )");
    consumer.language=BaseLanguage::ObjectiveCxx;consumer.arc=true;consumer.arguments={standard};consumer.emit_semantic_interface=false;
    consumer.imported_interfaces={ImportOf(path,emitted.interface_artifact->identity),ImportOf(bridge_path,projected.interface_artifact->identity)};
    auto result=Compile(consumer);
    if(!result.success) {
      std::fprintf(stderr,"UNNAMED SECOND IMPORT %s\n",result.failure.c_str());
      for(const auto &diagnostic:result.diagnostics)std::fprintf(stderr,"%s\n",diagnostic.message.c_str());
    }
    assert(result.success);
  }
  fs::remove_all(root);
}

void TestStandardCoroutineTypesAndTraitsImports() {
  const auto root=fs::temp_directory_path()/"objc3-standard-coroutine-import";
  fs::remove_all(root);fs::create_directories(root);
  std::ifstream input(fs::path(OBJC3_INTERFACE_TEST_PACKAGE_FIXTURES)/"provider/standard_coroutines.h");
  assert(input);const std::string source((std::istreambuf_iterator<char>(input)),{});
  for(const char *standard:{"-std=c++20","-std=c++23"}) {
    auto provider=ModuleRequest(root/"provider.mm","test.standard_coroutines",source);
    provider.language=BaseLanguage::ObjectiveCxx;provider.arc=true;
    provider.arguments={standard};provider.module_owned_names=true;
    provider.interface_exports={"NativeStandardCoroutines"};
    const auto interface_path=root/"test.standard_coroutines.objc3i";
    provider.interface_output_path=interface_path.string();
    auto emitted=Compile(provider);assert(emitted.success && emitted.interface_artifact);
    auto other=provider;other.module_name="test.other_coroutines";
    other.interface_output_path=(root/"test.other_coroutines.objc3i").string();
    for(std::size_t at=0;(at=other.source_text->find("NativeStandardCoroutines",at))!=std::string::npos;at+=23)
      other.source_text->replace(at,24,"OtherStandardCoroutines");
    other.interface_exports={"OtherStandardCoroutines"};
    auto second=Compile(other);assert(second.success && second.interface_artifact);
    for(unsigned mode=0;mode!=4;++mode) {
      bool directed=mode & 1,late_header=mode & 2;
      auto consumer=ModuleRequest(root/"consumer.mm","",
          std::string(late_header ? "" : "#include <coroutine>\n")+(directed ? "@import test.standard_coroutines;\n@import test.other_coroutines;\n" : "")+
          (late_header ? "#include <coroutine>\n" : "")+
          "int use(){auto factory=NativeStandardCoroutines::closure(40);auto task=factory(2);"
          "auto generic=NativeStandardCoroutines::generic(40L);auto partial=NativeStandardCoroutines::external_generic(40);"
          "auto full=NativeStandardCoroutines::external_full(40L);auto no_op=NativeStandardCoroutines::noop();"
          "auto other=OtherStandardCoroutines::external_generic(40);"
          "return task.run()+int(generic.run())+partial.run()+int(full.run())+int(no_op.done())+other.run();}");
      consumer.language=BaseLanguage::ObjectiveCxx;consumer.arc=true;
      consumer.arguments={standard};consumer.emit_semantic_interface=false;
      consumer.source_directed_imports=directed;
      if(directed) {
        InterfaceModuleRequest locked;locked.canonical_name="test.standard_coroutines";
        locked.locked_identity=emitted.interface_artifact->identity;
        InterfaceModuleRequest other_locked;other_locked.canonical_name="test.other_coroutines";
        other_locked.locked_identity=second.interface_artifact->identity;
        consumer.interface_search_paths={root.string()};consumer.module_imports={locked,other_locked};
      } else consumer.imported_interfaces={ImportOf(interface_path,emitted.interface_artifact->identity),
          ImportOf(root/"test.other_coroutines.objc3i",second.interface_artifact->identity)};
      auto result=Compile(consumer);
      if(!result.success)std::fprintf(stderr,"%s\n",result.failure.c_str());
      for(const auto &diagnostic:result.diagnostics)std::fprintf(stderr,"%s\n",diagnostic.message.c_str());
      assert(result.success);
    }
    auto only=provider;only.module_name="test.standard_types_only";
    only.interface_output_path=(root/"test.standard_types_only.objc3i").string();
    only.source_text="#include <coroutine>\nnamespace NativeStandardCoroutines {using Handle=std::coroutine_handle<>; std::suspend_always suspend(Handle);}";
    auto typed=Compile(only);assert(typed.success && typed.interface_artifact);
    assert(typed.interface_artifact->sidecar_json.find("objc3.cxx-standard-coroutines.v1")!=std::string::npos);
    assert(typed.interface_artifact->sidecar_json.find("objc3.cxx-coroutine-bodies.v1")==std::string::npos);
    for(unsigned mode=0;mode!=4;++mode) {
      const bool directed=mode&1,late_header=mode&2;
      auto consumer=ModuleRequest(root/"types-only-consumer.mm","",
          std::string(late_header?"":"#include <coroutine>\n")+
          (directed?"@import test.standard_types_only;\n":"")+
          (late_header?"#include <coroutine>\n":"")+
          "void *use(NativeStandardCoroutines::Handle handle){return handle.address();}");
      consumer.language=BaseLanguage::ObjectiveCxx;consumer.arc=true;consumer.arguments={standard};
      consumer.emit_semantic_interface=false;consumer.source_directed_imports=directed;
      if(directed) {
        InterfaceModuleRequest locked;locked.canonical_name="test.standard_types_only";locked.locked_identity=typed.interface_artifact->identity;
        consumer.interface_search_paths={root.string()};consumer.module_imports={locked};
      } else consumer.imported_interfaces={ImportOf(root/"test.standard_types_only.objc3i",typed.interface_artifact->identity)};
      auto result=Compile(consumer);if(!result.success)std::fprintf(stderr,"%s\n",result.failure.c_str());
      for(const auto &diagnostic:result.diagnostics)std::fprintf(stderr,"%s\n",diagnostic.message.c_str());
      assert(result.success);
    }
    auto single=provider;single.module_name="test.single_coroutine_trait";
    const auto single_path=root/"test.single_coroutine_trait.objc3i";
    single.interface_output_path=single_path.string();
    single.source_text="#include <coroutine>\nnamespace NativeStandardCoroutines {struct Token {};struct Promise {}; }"
        "namespace std {template<>struct coroutine_traits<NativeStandardCoroutines::Token>{using promise_type=NativeStandardCoroutines::Promise;};}";
    auto trait=Compile(single);assert(trait.success && trait.interface_artifact);
    auto scope_check=ModuleRequest(root/"traits-lookup.mm","",
        "@import test.single_coroutine_trait;\n"
        "using Promise=std::coroutine_traits<NativeStandardCoroutines::Token>::promise_type;"
        "static_assert(__is_same(Promise,NativeStandardCoroutines::Promise));");
    scope_check.language=BaseLanguage::ObjectiveCxx;scope_check.arc=true;scope_check.arguments={standard};
    scope_check.emit_semantic_interface=false;scope_check.source_directed_imports=true;
    scope_check.interface_search_paths={root.string()};
    InterfaceModuleRequest single_locked;single_locked.canonical_name="test.single_coroutine_trait";single_locked.locked_identity=trait.interface_artifact->identity;
    scope_check.module_imports={single_locked};
    auto scoped=Compile(scope_check);assert(scoped.success);
    scope_check.source_text=*scope_check.source_text+"using Leaked=coroutine_traits;";
    auto leaked=Compile(scope_check);assert(!leaked.success);
    assert(std::any_of(leaked.diagnostics.begin(),leaked.diagnostics.end(),[](const auto &d){return d.message.find("coroutine_traits")!=std::string::npos;}));
  }
  fs::remove_all(root);
}

void TestCustomCoroutinePromiseRedeclarations() {
  const auto root=fs::temp_directory_path()/"objc3-custom-coroutine-import";
  fs::remove_all(root);fs::create_directories(root);
  std::ifstream input(fs::path(OBJC3_INTERFACE_TEST_PACKAGE_FIXTURES)/"provider/custom_coroutines.h");
  assert(input);const std::string source((std::istreambuf_iterator<char>(input)),{});
  for(const char *standard:{"-std=c++20","-std=c++23"}) {
    auto provider=ModuleRequest(root/"provider.mm","test.custom_coroutines",source);
    provider.language=BaseLanguage::ObjectiveCxx;provider.arc=true;
    provider.arguments={standard};provider.module_owned_names=true;
    provider.interface_exports={"NativeCustomCoroutines"};
    const auto interface_path=root/"test.custom_coroutines.objc3i";
    provider.interface_output_path=interface_path.string();
    auto emitted=Compile(provider);assert(emitted.success && emitted.interface_artifact);
    for(unsigned mode=0;mode!=4;++mode) {
      bool directed=mode & 1,late_header=mode & 2;
      auto consumer=ModuleRequest(root/"consumer.mm","",
          std::string(late_header ? "" : "#include <coroutine>\n")+(directed ? "@import test.custom_coroutines;\n" : "")+
          (late_header ? "#include <coroutine>\n" : "")+
          "int use(){auto factory=NativeCustomCoroutines::closure(40);auto task=factory(2);"
          "auto generic=NativeCustomCoroutines::generic(40L);return task.run()+int(generic.run());}");
      consumer.language=BaseLanguage::ObjectiveCxx;consumer.arc=true;
      consumer.arguments={standard};consumer.emit_semantic_interface=false;
      consumer.source_directed_imports=directed;
      if(directed) {
        InterfaceModuleRequest locked;locked.canonical_name="test.custom_coroutines";
        locked.locked_identity=emitted.interface_artifact->identity;
        consumer.interface_search_paths={root.string()};consumer.module_imports={locked};
      } else consumer.imported_interfaces={ImportOf(interface_path,emitted.interface_artifact->identity)};
      auto result=Compile(consumer);
      if(!result.success)std::fprintf(stderr,"%s\n",result.failure.c_str());
      for(const auto &diagnostic:result.diagnostics)std::fprintf(stderr,"%s\n",diagnostic.message.c_str());
      assert(result.success);
    }
  }
  fs::remove_all(root);
}

void TestCompilerVarargsMembersAreReused() {
  const auto root=fs::temp_directory_path()/"objc3-builtin-varargs-import";
  fs::remove_all(root);fs::create_directories(root);
  for(const char *standard:{"-std=c++20","-std=c++23"}) {
    auto provider=ModuleRequest(root/"provider.mm","test.varargs",
        "template<class T> inline T sum(int count,...) {"
        "__builtin_va_list args;__builtin_va_start(args,count);T value{};"
        "for(int i=0;i<count;++i)value+=__builtin_va_arg(args,T);"
        "__builtin_va_end(args);return value;}\n");
    provider.language=BaseLanguage::ObjectiveCxx;provider.arc=true;
    provider.arguments={standard};provider.module_owned_names=true;
    const auto interface_path=root/"test.varargs.objc3i";
    provider.interface_output_path=interface_path.string();
    auto emitted=Compile(provider);assert(emitted.success && emitted.interface_artifact);
    auto consumer=ModuleRequest(root/"consumer.mm","",
        "#include <memory>\n@import test.varargs;\n"
        "int use(){return sum<int>(2,20,22);}");
    consumer.language=BaseLanguage::ObjectiveCxx;consumer.arc=true;
    consumer.arguments={standard};consumer.emit_semantic_interface=false;
    consumer.source_directed_imports=true;
    InterfaceModuleRequest locked;
    locked.canonical_name="test.varargs";
    locked.locked_identity=emitted.interface_artifact->identity;
    consumer.interface_search_paths={root.string()};
    consumer.module_imports={locked};
    auto result=Compile(consumer);
    for(const auto &diagnostic:result.diagnostics)
      std::fprintf(stderr,"%s\n",diagnostic.message.c_str());
    if(!result.success)std::fprintf(stderr,"%s\n",result.failure.c_str());
    assert(result.success);
  }
  fs::remove_all(root);
}

} // namespace

int main() {
  TestPreparedImportBudgets();
  TestPrimitiveABIProfilesRejectMismatchedConsumers();
  TestThreeModuleSemanticLookupAndIdentity();
  TestPinnedRuntimeTypesSurvivePreparedImport();
  TestCompilerVarargsMembersAreReused();
  TestStandardCoroutineTypesAndTraitsImports();
  TestCustomCoroutinePromiseRedeclarations();
  TestNativeAttributeTemplateImports();
  TestUnnamedTemplateProjectionImports();
  TestBadLockAndMissingTransitiveArtifactFailBeforeImport();
  TestDuplicateLookupNamesAreRejected();
  TestCompleteExportIdentitiesAndPrivateDirectLookup();
  TestRelativeArtifactsAndResponseFileLayoutFlags();
  TestLaterCxxOverloadConflicts();
  TestCxxBaseAndDefaultedImports();
  return 0;
}

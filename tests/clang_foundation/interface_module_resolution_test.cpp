#include "clang_foundation/compile_session.h"
#include "clang_foundation/frontend_api.h"
#include "clang_foundation/interface_artifact_io.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

using namespace objc3::clang_foundation;

namespace {
bool HasSha256Field(const std::string &json, const std::string &field) {
  const std::string prefix = "\"" + field + "\":\"";
  const auto start = json.find(prefix);
  if (start == std::string::npos) return false;
  const auto digest_start = start + prefix.size();
  if (digest_start + 65 > json.size() || json[digest_start + 64] != '"')
    return false;
  for (std::size_t index = digest_start; index < digest_start + 64; ++index) {
    const char value = json[index];
    if (!((value >= '0' && value <= '9') || (value >= 'a' && value <= 'f')))
      return false;
  }
  return true;
}

CompileRequest Producer(std::string input, std::string module,
                        std::string package, std::string source) {
  CompileRequest request;
  request.input_path = std::move(input);
  request.source_text = std::move(source);
  request.language = BaseLanguage::ObjectiveC;
  request.objc3 = true;
  request.arguments = {"--target=x86_64-unknown-linux-gnu"};
  request.emit_semantic_interface = true;
  request.module_name = std::move(module);
  request.package_identity = std::move(package);
  return request;
}

InterfaceModuleRequest Locked(const InterfaceModuleIdentity &identity,
                              bool reexported = false) {
  InterfaceModuleRequest request;
  request.canonical_name = identity.canonical_name;
  request.locked_identity = identity;
  request.public_import = true;
  request.reexported = reexported;
  return request;
}

bool CompileV3Consumer(const std::filesystem::path &root,
                       const InterfaceModuleIdentity &identity,
                       std::string source, bool source_imports = false,
                       const InterfaceModuleIdentity *second = nullptr,
                       bool objective_cxx = false) {
  const std::string search_path = root.string();
  const char *search_paths[] = {search_path.c_str()};
  std::vector<objc3c_frontend_v3_import> imports(second ? 2 : 1);
  auto fill = [](objc3c_frontend_v3_import &imported,
                 const InterfaceModuleIdentity &module) {
    imported.struct_size = sizeof(imported);
    imported.canonical_name = module.canonical_name.c_str();
    imported.semantic_fingerprint = module.semantic_fingerprint.c_str();
    imported.package_identity = module.package_identity.c_str();
    imported.public_import = 1;
  };
  fill(imports[0], identity);
  if (second) fill(imports[1], *second);
  objc3c_frontend_v3_options options{};
  options.struct_size = sizeof(options);
  options.abi_version = OBJC3C_FRONTEND_V3_ABI;
  options.base.struct_size = sizeof(options.base);
  options.base.abi_version = OBJC3C_FRONTEND_V2_ABI;
  options.base.input_path = objective_cxx ? "consumer.mm" : "consumer.m";
  options.base.source = source.data();
  options.base.source_size = source.size();
  options.base.target_triple = "x86_64-unknown-linux-gnu";
  options.base.language = objective_cxx ? 3 : 2;
  options.base.emit = 0;
  options.base.profile = 0;
  options.base.objc3 = 1;
  options.interface_search_paths = search_paths;
  options.interface_search_path_count = 1;
  options.imports = imports.data();
  options.import_count = imports.size();
  options.source_directed_imports = source_imports;
  objc3c_frontend_v2_result *result = nullptr;
  const int status = objc3c_frontend_v3_compile(&options, &result);
  const bool success = status == 0 && objc3c_frontend_v2_result_success(result);
  if (success) {
    const char *record = objc3c_frontend_v3_result_dependency_record(result);
    const bool has_extension_import = source.find("@import") != std::string::npos;
    if (!source_imports || has_extension_import) {
      assert(record);
      const std::string json(record);
      assert(json.find("\"schema\":\"objc3c.dependency-record.v2\"") != std::string::npos);
      assert(json.find("\"driver_path\":") != std::string::npos);
      assert(HasSha256Field(json, "driver_sha256"));
      assert(json.find("\"resource_directory\":") != std::string::npos);
      assert(HasSha256Field(json, "resource_sha256"));
      assert(json.find("\"patch_digest\":") != std::string::npos);
      assert(json.find("ObjC3-patches=") != std::string::npos);
      assert(json.find("\"target\":\"x86_64-unknown-linux-gnu\"") != std::string::npos);
      assert(json.find("\"runtime\":\"gnustep-2.2\"") != std::string::npos);
      assert(json.find("\"name\":\"" + identity.canonical_name +
          "\",\"fingerprint\":\"" + identity.semantic_fingerprint + "\"") !=
          std::string::npos);
      if (identity.canonical_name == "fixture.B" && !second) {
        assert(json.find("\"dependency_order\":[\"fixture.A\",\"fixture.B\"]") != std::string::npos);
        assert(json.find("\"from\":\"fixture.B\",\"to\":\"fixture.A\",\"public\":true,\"reexported\":true") != std::string::npos);
      }
    } else {
      assert(!record && "an unused source-import registry must not enter the dependency record");
    }
  }
  objc3c_frontend_v2_result_destroy(result);
  return success;
}

void WriteArtifact(const InterfaceEmissionArtifact &artifact,
                   const std::filesystem::path &path) {
  const auto written = WriteSemanticInterfaceArtifact(artifact, path.string());
  assert(written.success);
}
} // namespace

int main() {
  const auto root = std::filesystem::temp_directory_path() /
      "objc3c-module-resolution-three-module";
  std::filesystem::remove_all(root);
  std::filesystem::create_directories(root);

  auto a = Compile(Producer("provider-a.m", "fixture.A", "pkg-A",
      "typedef struct Error { int code; } Error;\n"
      "typedef Error ErrorAlias;\n"
      "typedef __objc3_optional(int) MaybeInt;\n"));
  assert(a.success && a.interface_artifact && a.interface_artifact->success);
  WriteArtifact(*a.interface_artifact, root / "A.objc3i");

  InterfaceImportTarget target;
  const auto &a_identity = a.interface_artifact->identity;
  target.target_triple = a_identity.target_triple;
  target.runtime_abi = a_identity.runtime_abi;
  target.extension_abi = a_identity.extension_abi;
  target.language_revision = a_identity.language_revision;
  target.language_profile = a_identity.language_profile;
  auto private_request = Locked(a_identity);
  private_request.public_import = false;
  auto private_resolution = ResolveInterfaceModules({root.string()}, {private_request}, target);
  assert(private_resolution.success);
  assert(private_resolution.visible_module_names == std::vector<std::string>{"fixture.A"});
  assert(private_resolution.artifacts.size() == 1);
  assert(private_resolution.artifacts.front().direct_import);
  assert(!private_resolution.artifacts.front().public_import);
  private_request.initially_visible = false;
  auto deferred_resolution = ResolveInterfaceModules({root.string()}, {private_request}, target);
  assert(deferred_resolution.success && deferred_resolution.visible_module_names.empty());
  assert(!deferred_resolution.artifacts.front().direct_import);

  auto b = Producer("provider-b.m", "fixture.B", "pkg-B",
      "int answer(void);\n"
      "MaybeInt maybe_value(void);\n"
      "int error_code(ErrorAlias value) __objc3_throws(ErrorAlias);\n");
  b.interface_search_paths = {root.string()};
  b.module_imports = {Locked(a.interface_artifact->identity, true)};
  auto built_b = Compile(b);
  assert(built_b.success && built_b.interface_artifact &&
         built_b.interface_artifact->success);
  assert(built_b.interface_dependency_order.size() == 1);
  WriteArtifact(*built_b.interface_artifact, root / "B.objc3i");

  auto c = Producer("provider-c.m", "fixture.C", "pkg-C",
      "long answer(long value);\n");
  auto built_c = Compile(c);
  assert(built_c.success && built_c.interface_artifact &&
         built_c.interface_artifact->success);
  WriteArtifact(*built_c.interface_artifact, root / "C.objc3i");

  // The C consumer is given only B's locked identity and the artifact root.
  // Neither provider source file is an input to this compile.
  assert(CompileV3Consumer(root, built_b.interface_artifact->identity,
      "int use_imported_types(void) { ErrorAlias e = {0}; "
      "MaybeInt m = maybe_value(); (void)m; "
      "return error_code(e) + answer(); }\n"));

  // Source mode keeps the lock-backed registry invisible until the matching
  // directive. Provider source paths are absent from this consumer request.
  assert(CompileV3Consumer(root, built_b.interface_artifact->identity,
      "@import fixture.B;\n"
      "int use_source_import(void) { @fixture.A.ErrorAlias e = {0}; "
      "return @fixture.B.answer() + error_code(e); }\n", true));
  assert(!CompileV3Consumer(root, built_b.interface_artifact->identity,
      "int use_before_import(void) { return answer(); }\n", true));
  assert(CompileV3Consumer(root, built_b.interface_artifact->identity,
      "int no_extension_import_used(void) { return 0; }\n", true));
  assert(!CompileV3Consumer(root, built_b.interface_artifact->identity,
      "int use_qualified_before_import(void) { return @fixture.B.answer(); }\n",
      true));
  assert(!CompileV3Consumer(root, built_b.interface_artifact->identity,
      "@import fixture.B;\n"
      "int missing_export(void) { return @fixture.B.not_exported(); }\n",
      true));
  assert(!CompileV3Consumer(root, built_b.interface_artifact->identity,
      "@import fixture.B;\n@import fixture.C;\n"
      "int conflicting_c_imports(void) { return answer(1); }\n", true,
      &built_c.interface_artifact->identity));
  // Provider language/linkage interchange is not yet represented by the
  // interface contract. Keep Objective-C++ consumers explicitly unsupported.
  auto cxx_consumer = Producer("consumer.mm", "", "",
      "@import fixture.B;\n@import fixture.C;\n"
      "int overloads(void) { return @fixture.B.answer() + "
      "(int)@fixture.C.answer(2L); }\n");
  cxx_consumer.language = BaseLanguage::ObjectiveCxx;
  cxx_consumer.emit_semantic_interface = false;
  cxx_consumer.source_directed_imports = true;
  cxx_consumer.interface_search_paths = {root.string()};
  cxx_consumer.module_imports = {Locked(built_b.interface_artifact->identity),
                                Locked(built_c.interface_artifact->identity)};
  for (bool source_directed : {false, true}) {
    cxx_consumer.source_directed_imports = source_directed;
    auto unsupported_cxx = Compile(cxx_consumer);
    assert(!unsupported_cxx.success);
    assert(unsupported_cxx.resolved_module_identities.empty());
    bool unsupported_language = false;
    for (const auto &diagnostic : unsupported_cxx.interface_import_diagnostics)
      unsupported_language |= diagnostic.code == "O3IFC101";
    assert(unsupported_language);
  }

  auto hidden_b = Producer("provider-b-private.m", "fixture.B.private",
                           "pkg-B-private",
      "int answer(void);\n"
      "int error_code(ErrorAlias value) __objc3_throws(ErrorAlias);\n");
  hidden_b.interface_search_paths = {root.string()};
  hidden_b.module_imports = {Locked(a.interface_artifact->identity, false)};
  auto private_artifact = Compile(hidden_b);
  assert(private_artifact.success && private_artifact.interface_artifact &&
         private_artifact.interface_artifact->success);
  WriteArtifact(*private_artifact.interface_artifact, root / "B-private.objc3i");
  assert(CompileV3Consumer(root, private_artifact.interface_artifact->identity,
      "int use_visible_api(void) { return answer(); }\n"));
  assert(!CompileV3Consumer(root, private_artifact.interface_artifact->identity,
      "ErrorAlias hidden_name; int use_visible_api(void) { return answer(); }\n"));
  assert(!CompileV3Consumer(root, private_artifact.interface_artifact->identity,
      "@import fixture.B.private;\nErrorAlias hidden_name;\n", true));
  assert(!CompileV3Consumer(root, private_artifact.interface_artifact->identity,
      "@import fixture.B.private;\n"
      "int use_hidden_qualified(void) { return @fixture.A.ErrorAlias.code; }\n",
      true));
  assert(CompileV3Consumer(root, private_artifact.interface_artifact->identity,
      "@import fixture.B.private;\n"
      "@import fixture.A;\n"
      "int use_after_direct_import(void) { ErrorAlias e = {0}; "
      "return error_code(e) + answer(); }\n", true,
      &a.interface_artifact->identity));

  // A reexport marker on a private edge does not publish its dependency, but
  // that direct private dependency is still usable while compiling B itself.
  auto private_reexport = Producer("provider-private-reexport.m", "fixture.PrivateReexport",
      "pkg-private-reexport", "int private_api(ErrorAlias value);\n");
  private_reexport.interface_search_paths = {root.string()};
  auto private_edge = Locked(a_identity, true);
  private_edge.public_import = false;
  private_reexport.module_imports = {private_edge};
  auto private_reexport_artifact = Compile(private_reexport);
  assert(private_reexport_artifact.success && private_reexport_artifact.interface_artifact);
  WriteArtifact(*private_reexport_artifact.interface_artifact, root / "PrivateReexport.objc3i");
  auto private_reexport_resolution = ResolveInterfaceModules({root.string()},
      {Locked(private_reexport_artifact.interface_artifact->identity)}, target);
  assert(private_reexport_resolution.success);
  assert(private_reexport_resolution.visible_module_names ==
      std::vector<std::string>{"fixture.PrivateReexport"});

  // A native Clang module that is not in the extension registry still follows
  // Clang's normal Objective-C module loader. The unrelated locked extension
  // artifact remains unimported.
  {
    const auto native = root / "NativeKit";
    std::filesystem::create_directories(native);
    { std::ofstream header(native / "NativeKit.h");
      header << "static inline int native_value(void) { return 17; }\n"; }
    { std::ofstream map(native / "module.modulemap");
      map << "module NativeKit { header \"NativeKit.h\" export * }\n"; }
    CompileRequest native_consumer = Producer("native-fallback.m", "consumer", "pkg-native",
        "@import NativeKit;\n"
        "int use_native_module(void) { return native_value(); }\n");
    native_consumer.emit_semantic_interface = false;
    native_consumer.module_name.clear();
    native_consumer.package_identity.clear();
    native_consumer.interface_search_paths = {root.string()};
    native_consumer.module_imports = {Locked(built_b.interface_artifact->identity)};
    native_consumer.source_directed_imports = true;
    native_consumer.arguments = {"-fmodules", "-fmodule-map-file=" +
        (native / "module.modulemap").string()};
    assert(Compile(native_consumer).success);
  }

  const auto bad_root = root.parent_path() / "objc3-module-resolution-bad-version";
  std::filesystem::remove_all(bad_root);
  std::filesystem::copy(root, bad_root,
      std::filesystem::copy_options::recursive);
  {
    const auto sidecar = bad_root / "B.objc3i.json";
    std::string json;
    { std::ifstream input(sidecar, std::ios::binary);
      json.assign(std::istreambuf_iterator<char>(input), {}); }
    const auto version = json.find("\"schema_major\":1");
    assert(version != std::string::npos);
    json.replace(version, std::string("\"schema_major\":1").size(),
                 "\"schema_major\":2");
    std::ofstream output(sidecar, std::ios::binary | std::ios::trunc);
    output << json;
  }
  assert(CompileV3Consumer(bad_root, built_b.interface_artifact->identity,
      "int unused_bad_registry_entry(void) { return 0; }\n", true));
  assert(!CompileV3Consumer(bad_root, built_b.interface_artifact->identity,
      "@import fixture.B;\nint invalid_import(void) { return answer(); }\n",
      true));
  assert(!CompileV3Consumer(bad_root, built_b.interface_artifact->identity,
      "int use_visible_api(void) { return answer(); }\n"));

  const auto conflict_root = root.parent_path() / "objc3-module-resolution-conflict";
  std::filesystem::remove_all(conflict_root);
  std::filesystem::create_directories(conflict_root);
  auto changed_b = Producer("provider-b-conflict.m", "fixture.B", "pkg-B",
      "int answer(void);\nint other_api(long value);\n");
  changed_b.interface_search_paths = {root.string()};
  changed_b.module_imports = {Locked(a.interface_artifact->identity, true)};
  auto conflict = Compile(changed_b);
  assert(conflict.success && conflict.interface_artifact &&
         conflict.interface_artifact->success);
  WriteArtifact(*conflict.interface_artifact, conflict_root / "B.objc3i");
  CompileRequest conflict_consumer = Producer("consumer-conflict.m", "consumer", "pkg-C",
      "int use_visible_api(void) { return answer(); }\n");
  conflict_consumer.emit_semantic_interface = false;
  conflict_consumer.module_name.clear();
  conflict_consumer.package_identity.clear();
  conflict_consumer.interface_search_paths = {root.string(), conflict_root.string()};
  conflict_consumer.module_imports = {Locked(built_b.interface_artifact->identity)};
  assert(!Compile(conflict_consumer).success);
  std::filesystem::remove_all(bad_root);
  std::filesystem::remove_all(conflict_root);

  auto stale = built_b.interface_artifact->identity;
  stale.semantic_fingerprint = "stale-fingerprint";
  assert(!CompileV3Consumer(root, stale,
      "int use_visible_api(void) { return answer(); }\n"));
  auto wrong_profile = Producer("consumer-invalid-profile.m", "consumer", "pkg-C",
      "int use_visible_api(void) { return answer(); }\n");
  wrong_profile.interface_search_paths = {root.string()};
  auto wrong_identity = Locked(built_b.interface_artifact->identity);
  wrong_identity.locked_identity.language_profile += ";wrong-profile";
  wrong_profile.module_imports = {wrong_identity};
  assert(!Compile(wrong_profile).success);

  auto wrong_runtime = Producer("consumer-invalid-runtime.m", "consumer", "pkg-C",
      "int use_visible_api(void) { return answer(); }\n");
  wrong_runtime.emit_semantic_interface = false;
  wrong_runtime.module_name.clear();
  wrong_runtime.package_identity.clear();
  wrong_runtime.interface_search_paths = {root.string()};
  wrong_runtime.module_imports = {Locked(built_b.interface_artifact->identity)};
  wrong_runtime.objc_runtime = "gcc";
  assert(!Compile(wrong_runtime).success);

  const auto mutated_root = root.parent_path() / "objc3-module-resolution-mutated-source";
  std::filesystem::remove_all(mutated_root);
  std::filesystem::copy(root, mutated_root,
      std::filesystem::copy_options::recursive);
  {
    std::ofstream source(mutated_root / "A.objc3i", std::ios::binary | std::ios::app);
    source << "// same lock, changed source bytes\n";
  }
  assert(!CompileV3Consumer(mutated_root, built_b.interface_artifact->identity,
      "int use_visible_api(void) { return answer(); }\n"));

  std::filesystem::remove_all(mutated_root);
  std::filesystem::remove_all(root);
}

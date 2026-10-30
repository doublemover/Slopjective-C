#include "clang_foundation/compile_session.h"
#include "clang_foundation/frontend_api.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/Support/Error.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace objc3::clang_foundation;
namespace fs = std::filesystem;
namespace {
CompileRequest Request(const fs::path &path, BaseLanguage language,
                       const std::string &standard, bool arc,
                       const std::string &source) {
  CompileRequest r;
  r.input_path = path.string();
  r.source_text = source;
  r.language = language;
  r.objc3 = language == BaseLanguage::ObjectiveC ||
            language == BaseLanguage::ObjectiveCxx;
  r.arc = arc;
  if (r.objc3)
    r.objc_runtime = "gnustep-2.2";
  r.target_triple = "x86_64-unknown-linux-gnu";
  r.arguments = {"-std=" + standard, "-fblocks", "-Werror=objc-method-access"};
  return r;
}
void CheckResult(const CompileResult &result) {
  if (!result.success) {
    std::cerr << result.failure << '\n';
    for (const auto &d : result.diagnostics)
      std::cerr << d.message << '\n';
    for (const auto &d : result.interface_import_diagnostics)
      std::cerr << d.code << ": " << d.message << '\n';
    if (result.interface_artifact)
      for (const auto &d : result.interface_artifact->diagnostics)
        std::cerr << d.code << ": " << d.message << ' ' << d.subject << '\n';
  }
  assert(result.success);
}
const std::vector<llvm::StringRef> semantics = {
    "objc3.canonical-function-effects.v1", "objc3.native-carrier-types.v1",
    "objc3.native-return-ownership.v1",    "objc3.direct-dispatch.v1",
    "objc3.cxx-data-interfaces.v1",        "objc3.ownership-projection.v1",
    "objc3.api-availability.v1",           "objc3.api-partitions.v1",
    "objc3.cxx-namespaces.v1", "objc3.cxx-namespace-attributes.v1",
    "objc3.cxx-definitions.v1", "objc3.cxx-friends.v1"};
void Check(const fs::path &root, BaseLanguage language,
           const std::string &standard, bool arc) {
  const bool objc = language == BaseLanguage::ObjectiveC ||
                    language == BaseLanguage::ObjectiveCxx;
  std::string source = R"(
    #define SPI(g) __attribute__((annotate("objc3.spi:" #g)))
    int visible(void) { return 20; }
    SPI(Testing) int secret(void) { return 22; }
    extern int secret_global SPI(Testing);
    struct SPI(Testing) Secret { int field; };
    enum SPI(Testing) SecretChoice { secret_choice = 1 };
    typedef int SecretAlias SPI(Testing);
    SPI(Other) int other(void);
  )";
  if (objc)
    source += R"(
    __attribute__((objc_root_class)) @interface Root
    - (int)publicMethod;
    @end
    SPI(Testing) __attribute__((objc_root_class)) @interface SecretRoot
    - (int)onlySecret;
    @property(readonly) int secretProperty;
    @end
    SPI(Testing) @protocol SecretProtocol @end
  )";
  const bool cpp = language == BaseLanguage::Cxx || language == BaseLanguage::ObjectiveCxx;
  if (cpp) source += R"(
    struct SPI(Testing) HiddenFriend { int n; friend int hidden_probe(HiddenFriend v) {return v.n;} };
    namespace SPI(Testing) GroupSPI {
      int secret() { return 22; }
      struct Secret { int n; };
    }
    namespace ScopedSPI {
      int visible() { return 20; }
      SPI(Testing) int secret() { return 22; }
      struct SPI(Testing) Secret { int n; };
    }
  )";
  auto provider = Request(root / "provider.m", language, standard, arc, source);
  provider.emit_semantic_interface = true;
  provider.module_name = "SPI.Leaf";
  provider.package_identity = "spi-leaf-v1";
  provider.interface_output_path = (root / "Leaf.objc3i").string();
  auto built = Compile(provider);
  CheckResult(built);
  const auto artifact = *built.interface_artifact;
  assert(artifact.sidecar_json.find("objc3.api-partitions.v1") !=
         std::string::npos);
  auto decoded = DecodeSemanticInterfaceSidecar(
      artifact.sidecar_json, artifact.source_bytes, semantics);
  assert(decoded);
  for (const auto &d : decoded->declarations) {
    if (d.qualified_name == "hidden_probe" || d.qualified_name == "Secret::field" ||
        d.qualified_name == "secret_choice")
      assert(d.spi_group == "Testing");
  }
  auto client = Request(root / "client.m", language, standard, arc,
                        "int client(void) { return visible(); }");
  InterfaceImportInput input;
  input.source_path = provider.interface_output_path;
  input.locked_identity = artifact.identity;
  client.imported_interfaces = {input};
  auto unprivileged = Compile(client);
  CheckResult(unprivileged);
  for (const auto &d : unprivileged.imported_declarations)
    assert(d.spi_group.empty());
  std::vector<std::string> expressions = {
      "secret()",      "secret_global",  "sizeof(struct Secret)",
      "secret_choice", "(SecretAlias)0", "other()"};
  if (cpp)
    expressions.insert(expressions.end(), {"sizeof(HiddenFriend)", "ScopedSPI::secret()", "sizeof(ScopedSPI::Secret)", "GroupSPI::secret()", "sizeof(GroupSPI::Secret)"});
  if (objc)
    expressions.insert(expressions.end(),
                       {"sizeof(SecretRoot*)", "sizeof(id<SecretProtocol>)"});
  for (const auto &expression : expressions) {
    client.source_text = "int client(void) { return " + expression + "; }";
    assert(!Compile(client).success);
  }
  if (objc) {
    client.source_text = "int client(id v) { return [v onlySecret]; }";
    assert(!Compile(client).success);
  }
  client.imported_interfaces[0].spi_groups = {"Testing"};
  client.source_text =
      "int client(void) { return secret() + secret_global + sizeof(struct "
      "Secret) + secret_choice + (SecretAlias)0; }";
  CheckResult(Compile(client));
  if (cpp) {
    client.source_text="int client() {return hidden_probe(HiddenFriend{42});}";
    CheckResult(Compile(client));
  }
  if (objc) {
    client.source_text = "int client(SecretRoot *v, id<SecretProtocol> p) { "
                         "return [v onlySecret] + v.secretProperty + !!p; }";
    CheckResult(Compile(client));
  }
  client.source_text = "int client(void) { return other(); }";
  assert(!Compile(client).success);
  for (const auto &groups : std::vector<std::vector<std::string>>{
           {"Testing", "Testing"}, {"Missing"}, {"*"}, {""}}) {
    client.imported_interfaces[0].spi_groups = groups;
    client.source_text = "int client(void) { return visible(); }";
    assert(!Compile(client).success);
  }
  client.imported_interfaces[0].spi_groups = {"Testing"};
  // SPI can be used in an implementation without granting it to the consumers
  // of that implementation's public API or its re-exported dependency.
  auto top = client;
  top.source_text = "int top_value(void) { return visible() + secret(); }";
  top.emit_semantic_interface = true;
  top.module_name = "SPI.Top";
  top.package_identity = "spi-top-v1";
  top.interface_output_path = (root / "Top.objc3i").string();
  top.imported_interfaces[0].public_import = true;
  top.imported_interfaces[0].reexported = true;
  CheckResult(Compile(top));
  auto top_artifact = *Compile(top).interface_artifact;
  InterfaceImportInput top_input;
  top_input.source_path = top.interface_output_path;
  top_input.locked_identity = top_artifact.identity;
  client.imported_interfaces = {input, top_input};
  client.imported_interfaces[0].direct_import = false;
  client.source_text = "int client(void) { return visible() + top_value(); }";
  CheckResult(Compile(client));
  client.source_text = "int client(void) { return secret(); }";
  assert(!Compile(client).success);
  if (objc) {
    client.imported_interfaces.clear();
    client.interface_search_paths = {root.string()};
    client.source_directed_imports = true;
    InterfaceModuleRequest leaf;
    leaf.canonical_name = "SPI.Leaf";
    leaf.locked_identity = artifact.identity;
    leaf.spi_groups = {"Testing"};
    InterfaceModuleRequest parent;
    parent.canonical_name = "SPI.Top";
    parent.locked_identity = top_artifact.identity;
    client.module_imports = {leaf, parent};
    client.source_text =
        "@import SPI.Top;\nint before(void) { return visible(); }\n@import "
        "SPI.Leaf;\nint client(void) { return @SPI.Leaf.secret(); }";
    CheckResult(Compile(client));
    client.source_text = "@import SPI.Top;\nint before(void) { return "
                         "@SPI.Leaf.secret(); }\n@import SPI.Leaf;\n";
    assert(!Compile(client).success);
    client.module_imports[0].spi_groups.clear();
    client.source_text =
        "@import SPI.Leaf;\nint client(void) { return @SPI.Leaf.secret(); }";
    assert(!Compile(client).success);
  }
  if (cpp) {
    auto scoped = Request(root / "scoped.m", language, standard, arc,
                          "int f() { return ScopedSPI::visible(); }");
    scoped.imported_interfaces = {input};
    CheckResult(Compile(scoped));
    scoped.source_text = "int f() { return ScopedSPI::secret(); }";
    assert(!Compile(scoped).success);
    scoped.imported_interfaces[0].spi_groups={"Testing"};
    CheckResult(Compile(scoped));
    scoped.source_text="int f() { return GroupSPI::secret()+sizeof(GroupSPI::Secret); }";
    CheckResult(Compile(scoped));
    if (objc) {
      scoped.imported_interfaces.clear();
      scoped.source_directed_imports=true;
      scoped.interface_search_paths={root.string()};
      InterfaceModuleRequest scoped_leaf;
      scoped_leaf.canonical_name="SPI.Leaf";
      scoped_leaf.locked_identity=artifact.identity;
      scoped_leaf.spi_groups={"Testing"};
      InterfaceModuleRequest scoped_parent;
      scoped_parent.canonical_name="SPI.Top";
      scoped_parent.locked_identity=top_artifact.identity;
      scoped.module_imports={scoped_leaf,scoped_parent};
      scoped.source_text="@import SPI.Top;\nint before() { return ScopedSPI::visible(); } "
                         "\n@import SPI.Leaf;\nint after() { return ScopedSPI::secret()+GroupSPI::secret(); }";
      CheckResult(Compile(scoped));
      scoped.module_imports[0].spi_groups.clear();
      scoped.source_text="@import SPI.Leaf;\nint f() { return ScopedSPI::secret(); }";
      assert(!Compile(scoped).success);
    }
  }
  if (language == BaseLanguage::ObjectiveC && !arc) {
    const std::string source =
        "@import SPI.Leaf;\nint client(void) { return secret(); }";
    const std::string directory = root.string();
    const char *paths[] = {directory.c_str()};
    const char *arguments[] = {"-std=c17", "-fblocks"};
    objc3c_frontend_v3_import module{};
    module.struct_size = sizeof(module);
    module.canonical_name = "SPI.Leaf";
    module.semantic_fingerprint =
        artifact.identity.semantic_fingerprint.c_str();
    module.package_identity = "spi-leaf-v1";
    objc3c_frontend_v3_options options{};
    options.struct_size = sizeof(options);
    options.abi_version = OBJC3C_FRONTEND_V3_ABI;
    options.base.struct_size = sizeof(options.base);
    options.base.abi_version = OBJC3C_FRONTEND_V2_ABI;
    options.base.input_path = "spi-api.m";
    options.base.source = source.c_str();
    options.base.source_size = source.size();
    options.base.target_triple = "x86_64-unknown-linux-gnu";
    options.base.objc_runtime = "gnustep-2.2";
    options.base.language = 2;
    options.base.objc3 = 1;
    options.base.arguments = arguments;
    options.base.argument_count = 2;
    options.interface_search_paths = paths;
    options.interface_search_path_count = 1;
    options.imports = &module;
    options.import_count = 1;
    options.source_directed_imports = 1;
    objc3c_frontend_v2_result *result = nullptr;
    assert(objc3c_frontend_v3_compile(&options, &result) == 1);
    objc3c_frontend_v2_result_destroy(result);
    objc3c_frontend_v3_spi_grant grant{"SPI.Leaf", "Testing"};
    options.spi_grants = &grant;
    options.spi_grant_count = 1;
    const auto granted_status = objc3c_frontend_v3_compile(&options, &result);
    if (granted_status != 0) {
      std::cerr << "SPI embedding failure: "
                << objc3c_frontend_v2_result_failure(result) << '\n';
      for (size_t i = 0; i < objc3c_frontend_v2_diagnostic_count(result); ++i)
        std::cerr << objc3c_frontend_v2_diagnostic_message(result, i) << '\n';
    }
    assert(granted_status == 0);
    assert(std::string(objc3c_frontend_v3_result_dependency_record(result))
               .find("Testing") != std::string::npos);
    objc3c_frontend_v2_result_destroy(result);
    grant.canonical_name = "NotLocked";
    assert(objc3c_frontend_v3_compile(&options, &result) == 2 && !result);
    grant.canonical_name = "SPI.Leaf";
    options.struct_size = offsetof(objc3c_frontend_v3_options, spi_grants) + 1;
    assert(objc3c_frontend_v3_compile(&options, &result) == 2 && !result);
    options.struct_size = sizeof(options);
    options.spi_grants = nullptr;
    assert(objc3c_frontend_v3_compile(&options, &result) == 2 && !result);
  }
  if (objc) {
    auto projected = provider;
    projected.source_text = R"(
      #ifdef __cplusplus
      extern "C" {
      #endif
      int visible(void) { return 20; }
      __attribute__((annotate("objc3.spi:Testing"))) int secret(void) { return 22; }
      #ifdef __cplusplus
      }
      #endif
    )";
    projected.module_name = "SPI.Projected";
    projected.package_identity = "spi-projected-v1";
    projected.interface_exports = {"visible", "secret"};
    projected.projected_arc = !arc;
    projected.interface_output_path = (root / "Projected.objc3i").string();
    auto projection = Compile(projected);
    CheckResult(projection);
    auto consumer = Request(root / "projected-client.m", language, standard,
                            !arc, "int client(void) { return visible(); }");
    InterfaceImportInput imported;
    imported.source_path = projected.interface_output_path;
    imported.locked_identity = projection.interface_artifact->identity;
    consumer.imported_interfaces = {imported};
    CheckResult(Compile(consumer));
    consumer.source_text = "int client(void) { return secret(); }";
    assert(!Compile(consumer).success);
    consumer.imported_interfaces[0].spi_groups = {"Testing"};
    CheckResult(Compile(consumer));
  }
  auto external_leak = top;
  external_leak.source_text = "__attribute__((annotate(\"objc3.spi:Testing\")))"
                              " struct Secret leak(void);";
  assert(!Compile(external_leak).success);
  auto leaked = provider;
  leaked.source_text = source + "\nstruct Secret leak(void);";
  assert(!Compile(leaked).success);
  leaked.source_text = source + "\nSPI(Other) struct Secret leak(void);";
  assert(!Compile(leaked).success);
  leaked.source_text = source + "\nSPI(Testing) struct Secret okay(void);";
  CheckResult(Compile(leaked));
  for (const auto &invalid :
       {"int bad(void) __attribute__((annotate(\"objc3.spi:\")));",
        "struct Bad { int field "
        "__attribute__((annotate(\"objc3.spi:Testing\"))); };",
        "int bad(void) __attribute__((annotate(\"objc3.spi:Testing\", 1)));"}) {
    leaked.source_text = source + invalid;
    assert(!Compile(leaked).success);
  }
  CheckResult(Compile(provider));
  auto changed = provider;
  auto position = changed.source_text->find("SPI(Testing) int secret");
  changed.source_text->replace(position, 12, "SPI(Testing2)");
  auto evolved = Compile(changed);
  CheckResult(evolved);
  assert(evolved.interface_artifact->identity.semantic_fingerprint !=
         artifact.identity.semantic_fingerprint);
  CheckResult(Compile(provider));
  // Rebind a forged partition's fingerprint: reparsing native attributes must
  // still reject it before consumer declarations or products are published.
  for (auto &d : decoded->declarations)
    if (d.qualified_name == "secret")
      d.spi_group = "Other";
  decoded->module.semantic_fingerprint =
      ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(*decoded));
  auto forged = EncodeSemanticInterfaceSidecar(*decoded, semantics);
  assert(forged);
  std::ofstream(provider.interface_output_path + ".json") << *forged;
  auto forged_client = Request(root / "forged.m", language, standard, arc,
                               "int client(void) { return visible(); }");
  input.locked_identity = decoded->module;
  forged_client.imported_interfaces = {input};
  assert(!Compile(forged_client).success);
  decoded->required_semantics.erase(
      std::find(decoded->required_semantics.begin(),
                decoded->required_semantics.end(), "objc3.api-partitions.v1"));
  auto missing = EncodeSemanticInterfaceSidecar(*decoded, semantics);
  assert(!missing);
  llvm::consumeError(missing.takeError());
}
} // namespace
int main() {
  const auto root =
      fs::temp_directory_path() /
      ("objc3-spi-" +
       std::to_string(
           std::chrono::steady_clock::now().time_since_epoch().count()));
  fs::create_directories(root);
  Check(root, BaseLanguage::C, "c17", false);
  for (bool arc : {false, true}) {
    Check(root, BaseLanguage::ObjectiveC, "c17", arc);
    for (const auto &standard : {"c++20", "c++23"})
      Check(root, BaseLanguage::ObjectiveCxx, standard, arc);
  }
  for (const auto &standard : {"c++20", "c++23"})
    Check(root, BaseLanguage::Cxx, standard, false);
  fs::remove_all(root);
}

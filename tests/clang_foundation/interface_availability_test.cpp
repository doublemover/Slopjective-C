#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/JSON.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
using namespace objc3::clang_foundation;
namespace fs = std::filesystem;
namespace {
void Write(const fs::path &p, const std::string &s) { std::ofstream(p) << s; }
CompileRequest Request(const fs::path &p, BaseLanguage language,
                       std::string standard, bool arc, std::string source) {
  CompileRequest r;
  r.input_path = p.string();
  r.source_text = std::move(source);
  r.language = language;
  r.objc3 = language == BaseLanguage::ObjectiveC ||
            language == BaseLanguage::ObjectiveCxx;
  r.arc = arc;
  if (r.objc3)
    r.objc_runtime = "gnustep-2.2";
  r.target_triple = "x86_64-unknown-linux-gnu";
  r.arguments = {"-std=" + standard, "-fblocks"};
  return r;
}
void Check(const fs::path &root, BaseLanguage language, std::string standard,
           bool arc) {
  const bool objc = language == BaseLanguage::ObjectiveC ||
                    language == BaseLanguage::ObjectiveCxx;
  const bool cpp =
      language == BaseLanguage::Cxx || language == BaseLanguage::ObjectiveCxx;
  std::string source = R"(
    struct Value { int old_field __attribute__((deprecated("old field"))); };
    struct __attribute__((deprecated("old record"))) OldRecord { int value; };
    enum Choice { current_choice=0, old_choice __attribute__((deprecated("old choice")))=1 };
    typedef int OldAlias __attribute__((deprecated("old alias")));
    extern int old_global __attribute__((deprecated("old global")));
    int current(void);
    int old_fn(void) __attribute__((deprecated("use current", "current")));
    int gone_fn(void) __attribute__((unavailable("removed API")));
    int platform_fn(void) __attribute__((availability(macos,introduced=10.0,deprecated=11.0,obsoleted=12.0,replacement="current",message="platform API")));
    int escaped_fn(void) __attribute__((deprecated("quotes \" and \\ slash \n")));
    int simple_deprecated(void) __attribute__((deprecated));
    int current(void) { return 42; }
  )";
  if (objc)
    source += R"(
    __attribute__((objc_root_class)) @interface Root
    - (int)oldMethod __attribute__((deprecated("old method")));
    @property(readonly) int oldProperty __attribute__((deprecated("old property")));
    @end
    __attribute__((deprecated("old protocol"))) @protocol OldProtocol @end
    __attribute__((deprecated("old category"))) @interface Root (Old)
    - (int)categoryMethod;
    @end
    __attribute__((objc_root_class,deprecated("old class"))) @interface OldRoot @end
  )";
  auto r = Request(root / (cpp ? "provider.mm" : "provider.m"), language,
                   standard, arc, source);
  r.emit_semantic_interface = true;
  r.module_name = "Availability";
  r.package_identity = "availability-v1";
  r.interface_output_path = (root / "Availability.objc3i").string();
  auto built = Compile(r);
  if (!built.success) {
    std::cerr << built.failure << "\n";
    for (const auto &d : built.diagnostics)
      std::cerr << d.message << "\n";
    if (built.interface_artifact)
      for (const auto &d : built.interface_artifact->diagnostics)
        std::cerr << d.code << ": " << d.message << " " << d.subject << "\n";
  }
  assert(built.success);
  const auto first = *built.interface_artifact;
  assert(first.sidecar_json.find("objc3.api-availability.v1") !=
         std::string::npos);
  assert(first.source_bytes.find("use current") != std::string::npos);
  assert(first.source_bytes.find("removed API") != std::string::npos);
  if (objc) {
    assert(first.source_bytes.find("old protocol") != std::string::npos);
    assert(first.source_bytes.find("old category") != std::string::npos);
  }
  auto changed = r;
  auto pos = changed.source_text->find("use current");
  changed.source_text->replace(pos, 11, "new contract");
  auto updated = Compile(changed);
  assert(updated.success);
  assert(updated.interface_artifact->identity.semantic_fingerprint !=
         first.identity.semantic_fingerprint);
  if (cpp) {
    auto spelled = r;
    const std::string original =
        "int simple_deprecated(void) __attribute__((deprecated));";
    auto pos = spelled.source_text->find(original);
    spelled.source_text->replace(pos, original.size(),
                                 "[[deprecated]] int simple_deprecated(void);");
    auto alternate = Compile(spelled);
    assert(alternate.success);
    assert(alternate.interface_artifact->identity.semantic_fingerprint ==
           first.identity.semantic_fingerprint);
  }
  assert(Compile(r).success);
  auto client = Request(root / "client.m", language, standard, arc,
                        "int client(void) { return current(); }");
  InterfaceImportInput input;
  input.source_path = r.interface_output_path;
  input.locked_identity = first.identity;
  client.imported_interfaces = {input};
  assert(Compile(client).success);
  client.arguments.push_back("-Werror=deprecated-declarations");
  for (const auto &expression : {"old_fn()", "old_global", "(int)(OldAlias)0",
                                 "old_choice", "((struct Value*)0)->old_field",
                                 "gone_fn()", "sizeof(struct OldRecord)"}) {
    client.source_text =
        "int client(void) { return " + std::string(expression) + "; }";
    {
      auto rejected = Compile(client);
      if (rejected.success)
        std::cerr << "Unexpectedly accepted: " << *client.source_text << "\n";
      assert(!rejected.success);
    }
  }
  if (objc) {
    client.source_text =
        "int client(Root *value) { return [value oldMethod]; }";
    {
      auto rejected = Compile(client);
      if (rejected.success)
        std::cerr << "Unexpectedly accepted: " << *client.source_text << "\n";
      assert(!rejected.success);
    }
    client.source_text =
        "int client(Root *value) { return value.oldProperty; }";
    {
      auto rejected = Compile(client);
      if (rejected.success)
        std::cerr << "Unexpectedly accepted: " << *client.source_text << "\n";
      assert(!rejected.success);
    }
    client.source_text = "int client(OldRoot *value) { return !!value; }";
    {
      auto rejected = Compile(client);
      if (rejected.success)
        std::cerr << "Unexpectedly accepted: " << *client.source_text << "\n";
      assert(!rejected.success);
    }
    client.source_text =
        "int client(id<OldProtocol> value) { return !!value; }";
    {
      auto rejected = Compile(client);
      if (rejected.success)
        std::cerr << "Unexpectedly accepted: " << *client.source_text << "\n";
      assert(!rejected.success);
    }
    // Native Clang retains category deprecation metadata without diagnosing
    // an otherwise available member; importing must preserve that behavior.
    client.source_text =
        "int client(Root *value) { return [value categoryMethod]; }";
    assert(Compile(client).success);
  }
  // Rebind altered source bytes while retaining the native declaration lock:
  // accepting it would silently discard the provider's availability contract.
  auto stripped = first.source_bytes;
  pos = stripped.find("use current");
  stripped.replace(pos, 11, "tampered API");
  auto json = llvm::json::parse(first.sidecar_json);
  assert(json);
  (*json->getAsObject())["canonical_source_sha256"] =
      ComputeInterfaceSourceSha256(stripped);
  Write(r.interface_output_path, stripped);
  Write(r.interface_output_path + ".json", llvm::formatv("{0}", *json).str());
  client.source_text = "int client(void) { return current(); }";
  {
    auto rejected = Compile(client);
    if (rejected.success)
      std::cerr << "Unexpectedly accepted: " << *client.source_text << "\n";
    assert(!rejected.success);
  }
  auto metadata = llvm::json::parse(first.sidecar_json);
  assert(metadata);
  auto &required = *metadata->getAsObject()->getArray("required_semantics");
  for (auto it = required.begin(); it != required.end(); ++it)
    if (it->getAsString() == "objc3.api-availability.v1") {
      required.erase(it);
      break;
    }
  Write(r.interface_output_path, first.source_bytes);
  Write(r.interface_output_path + ".json",
        llvm::formatv("{0}", *metadata).str());
  {
    auto rejected = Compile(client);
    if (rejected.success)
      std::cerr << "Unexpectedly accepted: " << *client.source_text << "\n";
    assert(!rejected.success);
  }
  // Export filtering still permits unsupported private implementation
  // attributes.
  *r.source_text += "int private_attr(void) __attribute__((noinline));";
  r.interface_exports = {"current"};
  assert(Compile(r).success);
  assert(Compile(r).interface_artifact->sidecar_json.find("api-availability") ==
         std::string::npos);
}
void Projection(const fs::path &root, bool arc) {
  auto r = Request(
      root / "projection.m", BaseLanguage::ObjectiveC, "c17", arc,
      "id factory(void) __attribute__((ns_returns_retained,deprecated(\"use "
      "new factory\"))); ");
  r.emit_semantic_interface = true;
  r.projected_arc = !arc;
  r.module_name = "Projection";
  r.package_identity = "projection-v1";
  auto result = Compile(r);
  assert(result.success);
  assert(result.interface_artifact->source_bytes.find("use new factory") !=
         std::string::npos);
}
void ScopedRedeclarations(const fs::path &root, const std::string &standard,
                          bool arc) {
  auto provider =
      Request(root / "scoped.mm", BaseLanguage::ObjectiveCxx, standard, arc,
              "struct SharedValue;\n"
              "typedef struct SharedValue { int value; int old_field "
              "__attribute__((deprecated(\"old shared field\"))); } "
              "SharedValue;\n"
              "enum SharedChoice { shared_choice = 42 };\n");
  provider.emit_semantic_interface = true;
  provider.module_name = "ScopedLeaf";
  provider.package_identity = "scoped-leaf";
  provider.interface_output_path = (root / "ScopedLeaf.objc3i").string();
  auto leaf = Compile(provider);
  assert(leaf.success && leaf.interface_artifact);
  InterfaceImportInput input;
  input.source_path = provider.interface_output_path;
  input.locked_identity = leaf.interface_artifact->identity;
  input.public_import = input.reexported = true;
  auto top = provider;
  top.source_text = "int top_api(SharedValue *);\n";
  top.module_name = "ScopedTop";
  top.package_identity = "scoped-top";
  top.interface_output_path = (root / "ScopedTop.objc3i").string();
  top.imported_interfaces = {input};
  auto exported = Compile(top);
  assert(exported.success && exported.interface_artifact);

  auto client = Request(root / "scoped-client.mm", BaseLanguage::ObjectiveCxx,
                        standard, arc, "");
  client.source_directed_imports = true;
  client.interface_search_paths = {root.string()};
  InterfaceModuleRequest locked;
  locked.canonical_name = "ScopedTop";
  locked.locked_identity = exported.interface_artifact->identity;
  locked.initially_visible = false;
  client.module_imports = {locked};
  client.arguments.push_back("-Werror=deprecated-declarations");
  // Source-scoped imports hide the full redeclaration set before activating
  // a re-export. Different preceding declarations vary AST allocation order.
  for (unsigned iteration = 0; iteration != 8; ++iteration) {
    const auto prefix = "int allocation_pad[" + std::to_string(iteration + 1) +
                        "];\n@import ScopedTop;\n";
    client.source_text = prefix + "int use(SharedValue *v) { return v->value + "
                                  "shared_choice; }\n";
    assert(Compile(client).success);
    client.source_text =
        prefix + "int use(SharedValue *v) { return v->old_field; }\n";
    auto rejected = Compile(client);
    assert(!rejected.success);
    bool diagnostic = false;
    for (const auto &row : rejected.diagnostics)
      diagnostic |= row.message.find("old shared field") != std::string::npos;
    assert(diagnostic);
  }
  client.source_text = "int use(SharedValue *v) { return shared_choice; }\n";
  assert(!Compile(client).success);
}
} // namespace
int main() {
  auto root = fs::temp_directory_path() / "objc3 native availability";
  fs::remove_all(root);
  fs::create_directories(root);
  Check(root, BaseLanguage::C, "c17", false);
  for (const auto &standard : {"c++20", "c++23"})
    Check(root, BaseLanguage::Cxx, standard, false);
  for (bool arc : {false, true}) {
    Check(root, BaseLanguage::ObjectiveC, "c17", arc);
    for (const auto &standard : {"c++20", "c++23"}) {
      Check(root, BaseLanguage::ObjectiveCxx, standard, arc);
      ScopedRedeclarations(root, standard, arc);
    }
    Projection(root, arc);
  }
  fs::remove_all(root);
}

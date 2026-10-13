#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_artifact_io.h"
#include "llvm/ADT/StringRef.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <string>
#include <utility>
#include <vector>

using namespace objc3::clang_foundation;

namespace {
CompileRequest Module(std::string input, std::string name, std::string package,
                      std::string source) {
  CompileRequest request;
  request.input_path = std::move(input);
  request.source_text = std::move(source);
  request.language = BaseLanguage::ObjectiveC;
  request.objc3 = true;
  request.arguments = {"--target=x86_64-unknown-linux-gnu"};
  request.emit_semantic_interface = true;
  request.module_name = std::move(name);
  request.package_identity = std::move(package);
  return request;
}

InterfaceModuleRequest Locked(const InterfaceModuleIdentity &identity,
                              bool reexported = false,
                              bool public_import = true) {
  InterfaceModuleRequest request;
  request.canonical_name = identity.canonical_name;
  request.locked_identity = identity;
  request.public_import = public_import;
  request.reexported = reexported;
  return request;
}

std::filesystem::path ArtifactPath(const std::filesystem::path &root,
                                   llvm::StringRef canonical_name) {
  std::string file = canonical_name.str();
  std::replace(file.begin(), file.end(), '.', '_');
  return root / (file + ".objc3i");
}

void WriteArtifact(const InterfaceEmissionArtifact &artifact,
                   const std::filesystem::path &path) {
  const auto result = WriteSemanticInterfaceArtifact(artifact, path.string());
  assert(result.success);
}

std::string Read(const std::filesystem::path &path) {
  std::ifstream input(path, std::ios::binary);
  assert(input);
  return std::string(std::istreambuf_iterator<char>(input), {});
}

void Write(const std::filesystem::path &path, const std::string &bytes) {
  std::ofstream output(path, std::ios::binary | std::ios::trunc);
  assert(output);
  output.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
  assert(output);
}

CompileResult SourceConsumer(const std::filesystem::path &root,
                             const InterfaceModuleIdentity &direct_module,
                             std::string source, std::string input = "consumer.m") {
  CompileRequest request;
  request.input_path = std::move(input);
  request.source_text = std::move(source);
  request.language = BaseLanguage::ObjectiveC;
  request.objc3 = true;
  request.source_directed_imports = true;
  request.arguments = {"--target=x86_64-unknown-linux-gnu"};
  request.interface_search_paths = {root.string()};
  request.module_imports = {Locked(direct_module)};
  return Compile(request);
}

InterfaceModuleIdentity BuildModule(const std::filesystem::path &root,
                                    const std::string &name,
                                    const std::string &package,
                                    const std::string &source,
                                    const std::vector<InterfaceModuleRequest> &imports = {}) {
  auto request = Module(name + ".m", name, package, source);
  request.interface_search_paths = {root.string()};
  request.module_imports = imports;
  auto result = Compile(request);
  assert(result.success && result.interface_artifact &&
         result.interface_artifact->success);
  auto identity = result.interface_artifact->identity;
  WriteArtifact(*result.interface_artifact, ArtifactPath(root, name));
  return identity;
}
} // namespace

int main() {
  const auto root = std::filesystem::temp_directory_path() /
      "objc3c-module-provenance-locked-chain";
  std::filesystem::remove_all(root);
  std::filesystem::create_directories(root);

  const auto a = BuildModule(root, "fixture.A", "pkg-a",
      "int root_api(void);\n"
      "enum RootStatus { RootOK = 13 };\n");
  const auto b_private = BuildModule(root, "fixture.B.Private", "pkg-b-private",
      "int bridge(void) { return root_api(); }\n", {Locked(a, false, false)});
  const auto c_private = BuildModule(root, "fixture.C.Private", "pkg-c-private",
      "int top_api(void) { return bridge(); }\n", {Locked(b_private)});

  auto private_chain = SourceConsumer(root, c_private,
      "@import fixture.C.Private;\n"
      "int use_top(void) { return @fixture.C.Private.top_api(); }\n");
  assert(private_chain.success);
  assert((private_chain.interface_dependency_order == std::vector<std::string>{
      "fixture.A", "fixture.B.Private", "fixture.C.Private"}));
  assert(private_chain.visible_module_names ==
      std::vector<std::string>{"fixture.C.Private"});
  assert(private_chain.semantic_dependency_record.find(
      "\"from\":\"fixture.B.Private\",\"to\":\"fixture.A\",\"public\":false,\"reexported\":false") !=
      std::string::npos);
  assert(!SourceConsumer(root, c_private,
      "@import fixture.C.Private;\n"
      "int hidden_a(void) { return @fixture.A.root_api(); }\n").success);
  assert(!SourceConsumer(root, c_private,
      "@import fixture.C.Private;\n"
      "int hidden_b(void) { return @fixture.B.Private.bridge(); }\n").success);
  assert(!SourceConsumer(root, c_private,
      "@import fixture.C.Private;\n"
      "int hidden_enum(void) { return RootOK; }\n").success);

  const auto b_public = BuildModule(root, "fixture.B.Public", "pkg-b-public",
      "int bridge(void) { return root_api(); }\n", {Locked(a, true)});
  const auto c_public = BuildModule(root, "fixture.C.Public", "pkg-c-public",
      "int top_api(void) { return bridge(); }\n", {Locked(b_public, true)});
  auto public_chain = SourceConsumer(root, c_public,
      "@import fixture.C.Public;\n"
      "int use_reexports(void) { return @fixture.A.root_api() + "
      "@fixture.B.Public.bridge() + @fixture.C.Public.top_api() + "
      "RootOK + @fixture.A.RootOK; }\n");
  assert(public_chain.success);
  assert((public_chain.interface_dependency_order == std::vector<std::string>{
      "fixture.A", "fixture.B.Public", "fixture.C.Public"}));
  assert((public_chain.visible_module_names == std::vector<std::string>{
      "fixture.A", "fixture.B.Public", "fixture.C.Public"}));
  assert(public_chain.semantic_dependency_record.find(
      "\"from\":\"fixture.B.Public\",\"to\":\"fixture.A\",\"public\":true,\"reexported\":true") !=
      std::string::npos);
  // Re-emitting dependency includes cannot transfer their export provenance
  // to the including module, even when the dependency itself is visible.
  assert(!SourceConsumer(root, c_public,
      "@import fixture.C.Public;\n"
      "int wrong_origin(void) { return @fixture.C.Public.root_api(); }\n").success);
  assert(!SourceConsumer(root, c_public,
      "@import fixture.C.Public;\n"
      "int wrong_enum_origin(void) { return @fixture.C.Public.RootOK; }\n").success);

  // Hiding a dependency's names must preserve a declaration already owned by
  // this consumer, including its publication in the consumer's own sidecar.
  auto own_declaration = Module("own-declaration.m", "fixture.ConsumerOwn", "pkg-own",
      "int root_api(void);\n@import fixture.C.Private;\n"
      "int own_use(void) { return root_api() + @fixture.C.Private.top_api(); }\n");
  own_declaration.interface_search_paths = {root.string()};
  own_declaration.module_imports = {Locked(c_private)};
  own_declaration.source_directed_imports = true;
  auto own_result = Compile(own_declaration);
  assert(own_result.success && own_result.interface_artifact);
  assert(own_result.interface_artifact->source_bytes.find("int root_api(void)") !=
      std::string::npos);

  // Different locked canonical modules may export the same source identifier.
  // Each isolated consumer must resolve the qualified spelling against its
  // exact requested module identity and artifact, independent of scan order.
  const auto first = BuildModule(root, "fixture.Overload.First", "pkg-first",
      "int shared_name(void);\n");
  const auto second = BuildModule(root, "fixture.Alternate.First", "pkg-second",
      "long shared_name(long);\n");
  assert(first.semantic_fingerprint != second.semantic_fingerprint);
  assert(first.package_identity != second.package_identity);
  auto wrong_lock = first;
  wrong_lock.package_identity = "another-package";
  assert(!SourceConsumer(root, wrong_lock,
      "@import fixture.Overload.First;\n"
      "int wrong_lock(void) { return @fixture.Overload.First.shared_name(); }\n").success);
  assert(SourceConsumer(root, first,
      "@import fixture.Overload.First;\n"
      "int first_use(void) { return @fixture.Overload.First.shared_name(); }\n").success);
  assert(SourceConsumer(root, second,
      "@import fixture.Alternate.First;\n"
      "long second_use(void) { return @fixture.Alternate.First.shared_name(4L); }\n").success);

  // Preserve the canonical path while changing the source bytes. The existing
  // lock/sidecar must reject the changed artifact; restoring bytes restores use.
  const auto first_source = ArtifactPath(root, "fixture.Overload.First");
  const auto original_source = Read(first_source);
  Write(first_source, original_source + "// content changed in place\n");
  assert(!SourceConsumer(root, first,
      "@import fixture.Overload.First;\n"
      "int changed_source(void) { return shared_name(); }\n").success);
  Write(first_source, original_source);
  assert(SourceConsumer(root, first,
      "@import fixture.Overload.First;\n"
      "int restored_source(void) { return shared_name(); }\n").success);

  // A sidecar export identity edit at the same path must also be rejected.
  auto sidecar = first_source;
  sidecar += ".json";
  const auto original_sidecar = Read(sidecar);
  auto tampered_sidecar = original_sidecar;
  const auto export_name = tampered_sidecar.find("shared_name");
  assert(export_name != std::string::npos);
  tampered_sidecar.replace(export_name, std::string("shared_name").size(),
                           "different_name");
  Write(sidecar, tampered_sidecar);
  assert(!SourceConsumer(root, first,
      "@import fixture.Overload.First;\n"
      "int tampered_export(void) { return shared_name(); }\n").success);
  Write(sidecar, original_sidecar);
  assert(SourceConsumer(root, first,
      "@import fixture.Overload.First;\n"
      "int intact_export(void) { return shared_name(); }\n").success);

  std::filesystem::remove_all(root);
}

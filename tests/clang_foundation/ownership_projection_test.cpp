#include "clang_foundation/compile_session.h"
#include "clang_foundation/interface_codec.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using namespace objc3::clang_foundation;
namespace fs = std::filesystem;
namespace {
const std::vector<llvm::StringRef> semantics = {
    "objc3.canonical-function-effects.v1", "objc3.native-carrier-types.v1",
    "objc3.native-return-ownership.v1",    "objc3.direct-dispatch.v1",
    "objc3.cxx-data-interfaces.v1",        "objc3.ownership-projection.v1"};
const char *api =
    "id projected_new(void) __attribute__((ns_returns_retained));\n"
    "id projected_borrow(void) __attribute__((ns_returns_not_retained));\n"
    "id projected_autoreleased(void) "
    "__attribute__((ns_returns_autoreleased));\n"
    "int projected_observe(id value);\n";
CompileRequest Request(const fs::path &path, bool arc, bool cpp,
                       const std::string &standard, std::string source) {
  CompileRequest r;
  r.input_path = path.string();
  r.source_text = std::move(source);
  r.language = cpp ? BaseLanguage::ObjectiveCxx : BaseLanguage::ObjectiveC;
  r.objc3 = true;
  r.arc = arc;
  r.objc_runtime = "gnustep-2.2";
  r.target_triple = "x86_64-unknown-linux-gnu";
  r.arguments = {"-std=" + standard, "-fblocks"};
  return r;
}
void Write(const fs::path &path, const std::string &bytes) {
  std::ofstream stream(path, std::ios::binary | std::ios::trunc);
  stream << bytes;
}
std::string Read(const fs::path &path) {
  std::ifstream stream(path, std::ios::binary);
  return {std::istreambuf_iterator<char>(stream), {}};
}
void TestProvenanceAndModeLocks(const fs::path &root, bool producer_arc,
                                bool cpp, const std::string &standard) {
  auto source = std::string("#include \"native-runtime.h\"\n") +
                (cpp ? std::string("extern \"C\" {\n") + api + "}\n" : api);
  auto emit = Request(root / (cpp ? "provider.mm" : "provider.m"), producer_arc,
                      cpp, standard, source);
  emit.emit_semantic_interface = true;
  emit.arguments.push_back("-I" + root.string());
  emit.projected_arc = !producer_arc;
  emit.module_name = "Projected";
  emit.package_identity = "projection-v1";
  const auto path = root / "Projected.objc3i";
  emit.interface_output_path = path.string();
  auto built = Compile(emit);
  assert(built.success && built.interface_artifact->success);
  auto sidecar = llvm::cantFail(DecodeSemanticInterfaceSidecar(
      built.interface_artifact->sidecar_json,
      built.interface_artifact->source_bytes, semantics));
  assert(sidecar.schema_major == 2 && !sidecar.ownership_projection.empty());
  auto producer = llvm::cantFail(DecodeSemanticInterfaceSidecar(
      sidecar.ownership_projection, built.interface_artifact->source_bytes,
      semantics));
  assert(producer.ownership_projection.empty());
  assert(producer.module.canonical_name == "Projected.__ownership_producer");
  assert(producer.module.runtime_abi != sidecar.module.runtime_abi);
  auto client = Request(
      root / (cpp ? "client.mm" : "client.m"), !producer_arc, cpp, standard,
      "int main(void) { return projected_observe(projected_new()); }");
  InterfaceImportInput input;
  input.source_path = path.string();
  input.locked_identity = sidecar.module;
  client.imported_interfaces = {input};
  assert(Compile(client).success);
  client.arc = producer_arc;
  assert(!Compile(client).success);
  client.arc = !producer_arc;
  emit.projected_arc = producer_arc;
  assert(!Compile(emit).success);
  emit.projected_arc = !producer_arc;

  auto reject = [&](SemanticInterfaceSidecar forged) {
    forged.module.semantic_fingerprint =
        ComputeInterfaceSourceSha256(BuildSemanticFingerprintInput(forged));
    Write(path.string() + ".json",
          llvm::cantFail(EncodeSemanticInterfaceSidecar(forged, semantics)));
    client.imported_interfaces.front().locked_identity = forged.module;
    assert(!Compile(client).success);
    Write(path.string() + ".json", built.interface_artifact->sidecar_json);
    client.imported_interfaces.front().locked_identity = sidecar.module;
  };
  auto forged = sidecar;
  auto foreign = producer;
  foreign.module.package_identity = "wrong-producer-package";
  forged.ownership_projection =
      llvm::cantFail(EncodeSemanticInterfaceSidecar(foreign, semantics));
  reject(forged);
  foreign = producer;
  foreign.ownership_projection = producer.ownership_projection + "nested";
  foreign.required_semantics.push_back("objc3.ownership-projection.v1");
  forged.ownership_projection =
      llvm::cantFail(EncodeSemanticInterfaceSidecar(foreign, semantics));
  reject(forged);
  foreign = producer;
  foreign.module.runtime_abi = sidecar.module.runtime_abi;
  forged.ownership_projection =
      llvm::cantFail(EncodeSemanticInterfaceSidecar(foreign, semantics));
  reject(forged);
  assert(Compile(client).success);
}
void TestRejectedAPIsAndAtomicOutputs(const fs::path &root) {
  const std::vector<std::string> rejected = {
      "id missing_contract(void);",
      "Optional(int) carrier(void);",
      "int callback(int (*work)(int));",
      "void block(void (^work)(void));",
      "struct Record { int value; }; int record(struct Record value);",
      "extern id shared;",
      "int variadic(int first, ...);",
      "int unprototyped();",
      "id indirect(id *value) __attribute__((ns_returns_retained));",
      "id consumed(id value __attribute__((ns_consumed))) "
      "__attribute__((ns_returns_retained));",
      "int throwing(void) throws(int);"};
  for (const auto &source : rejected) {
    auto r = Request(root / "rejected.m", false, false, "c17", source);
    r.emit_semantic_interface = true;
    r.projected_arc = true;
    r.module_name = "Rejected";
    r.package_identity = "rejected-v1";
    r.emit = EmitKind::Object;
    r.output_path = (root / "prior.o").string();
    r.interface_output_path = (root / "prior.objc3i").string();
    Write(r.output_path, "prior object");
    Write(r.interface_output_path, "prior source");
    Write(r.interface_output_path + ".json", "prior sidecar");
    assert(!Compile(r).success);
    assert(Read(r.output_path) == "prior object");
    assert(Read(r.interface_output_path) == "prior source");
    assert(Read(r.interface_output_path + ".json") == "prior sidecar");
  }
  auto cpp = Request(root / "cpp.mm", false, true, "c++20", api);
  cpp.emit_semantic_interface = true;
  cpp.projected_arc = true;
  cpp.module_name = "CppLinkage";
  cpp.package_identity = "rejected-v1";
  assert(!Compile(cpp).success);
  for (const auto &standard : {"c11", "gnu17"}) {
    auto unsupported =
        Request(root / "unsupported.m", false, false, standard, api);
    unsupported.emit_semantic_interface = true;
    unsupported.projected_arc = true;
    unsupported.module_name = "Unsupported";
    unsupported.package_identity = "rejected-v1";
    assert(!Compile(unsupported).success);
  }
}
void TestNativeRuntimeAliases(const fs::path &root, bool arc, bool cpp,
                              const std::string &standard) {
  auto declarations =
      std::string("id native_id(void); Class native_class(void);\n");
  auto r = Request(
      root / (cpp ? "runtime.mm" : "runtime.m"), arc, cpp, standard,
      "#include \"native-runtime.h\"\n" +
          (cpp ? "extern \"C\" {\n" + declarations + "}\n" : declarations));
  r.arguments.push_back("-I" + root.string());
  r.emit_semantic_interface = true;
  r.module_name = "NativeRuntime";
  r.package_identity = "runtime-v1";
  assert(Compile(r).success);
  r.source_text =
      "#include \"native-runtime.h\"\nForeignId external_alias(void);";
  // Canonical type equality alone must not erase an unresolved public alias.
  assert(!Compile(r).success);
}
} // namespace
int main() {
  auto root = fs::temp_directory_path() / "objc3 ownership projection test";
  fs::remove_all(root);
  fs::create_directories(root);
  Write(root / "native-runtime.h",
        "typedef struct objc_class *Class;\n"
        "typedef struct objc_selector *SEL;\n"
        "typedef struct objc_object { Class isa; } *id;\n"
        "typedef id ForeignId;\n");
  for (const auto &standard : {"c17", "c++20", "c++23"})
    for (bool producer_arc : {false, true}) {
      TestProvenanceAndModeLocks(root, producer_arc,
                                 std::string(standard) != "c17", standard);
      TestNativeRuntimeAliases(root, producer_arc,
                               std::string(standard) != "c17", standard);
    }
  TestRejectedAPIsAndAtomicOutputs(root);
  fs::remove_all(root);
}

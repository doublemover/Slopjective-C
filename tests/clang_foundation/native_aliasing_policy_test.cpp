#include "native_target_test_support.h"
#include "clang_foundation/compile_session.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Program.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using namespace objc3::clang_foundation;
namespace fs = std::filesystem;
namespace {
void Require(bool value, const std::string &message) {
  if (!value) { std::cerr << message << '\n'; std::abort(); }
}
void Compiled(const CompileResult &result) {
  if (result.success) return;
  std::cerr << result.failure << '\n';
  for (const auto &item : result.diagnostics) std::cerr << item.message << '\n';
  if (result.interface_artifact)
    for (const auto &item : result.interface_artifact->diagnostics)
      std::cerr << item.code << ": " << item.message << '\n';
  std::abort();
}
void Write(const fs::path &path, const std::string &bytes) {
  std::ofstream file(path); file << bytes; Require(file.good(), "write failed: " + path.string());
}
void Execute(const std::vector<std::string> &command) {
  std::vector<llvm::StringRef> args(command.begin(), command.end());
  Require(llvm::sys::ExecuteAndWait(command.front(), args) == 0, "command failed: " + command.front());
}
void Check(const std::string &standard) {
  llvm::SmallString<128> temporary;
  Require(!llvm::sys::fs::createUniqueDirectory("objc3-aliasing", temporary), "temporary directory failed");
  const fs::path root(temporary.c_str());
  struct Cleanup { fs::path path; ~Cleanup() { fs::remove_all(path); } } cleanup{root};
  const bool cxx = standard.starts_with("c++");
  const auto provider_path = root / (cxx ? "provider.cpp" : "provider.c");
  const std::string provider_source =
      "typedef int NativeAliasInt __attribute__((may_alias));"
      "struct __attribute__((may_alias)) NativeAliasRecord;struct NativeAliasRecord {int value;};"
      "float alias_provider(float *value,NativeAliasInt *alias){*value=1.0f;*alias=0;return *value;}"
      "float alias_record_provider(float *value,struct NativeAliasRecord *alias){*value=1.0f;alias->value=0;return *value;}";
  Write(provider_path, provider_source);
  CompileRequest provider;
  provider.input_path = provider_path.string(); provider.language = cxx ? BaseLanguage::Cxx : BaseLanguage::C;
  provider.target_triple = OBJC3_TEST_NATIVE_LINUX_TARGET;
  provider.arguments = {"-std=" + standard, "-O2", "-fstrict-aliasing", "-Werror=attributes"};
  provider.emit = EmitKind::Object; provider.output_path = (root / "provider.o").string();
  provider.emit_semantic_interface = true; provider.module_name = "test.Aliasing";
  provider.package_identity = "aliasing-v1";
  provider.interface_exports = {"NativeAliasInt", "NativeAliasRecord", "alias_provider", "alias_record_provider"};
  auto published = Compile(provider); Compiled(published);
  Require(published.interface_artifact && published.interface_artifact->success, "interface missing");
  const auto &artifact = *published.interface_artifact;
  Require(artifact.source_bytes.find("may_alias") != std::string::npos, "alias policy omitted");
  const auto interface = root / "Aliasing.objc3i";
  Write(interface, artifact.source_bytes); Write(interface.string() + ".json", artifact.sidecar_json);
  Require(fs::remove(provider_path), "provider source removal failed");
  CompileRequest consumer;
  consumer.input_path = (root / (cxx ? "consumer.cpp" : "consumer.c")).string();
  consumer.language = provider.language; consumer.target_triple = provider.target_triple;
  consumer.arguments = provider.arguments; consumer.emit = EmitKind::Object;
  consumer.output_path = (root / "consumer.o").string();
  consumer.source_text =
      "__attribute__((noinline)) float local_alias(float *value,NativeAliasInt *alias){*value=1.0f;*alias=0;return *value;}"
      "__attribute__((noinline)) float local_record(float *value,struct NativeAliasRecord *alias){*value=1.0f;alias->value=0;return *value;}"
      "int main(void){float a=2,b=3,c=4,d=5;"
      "return local_alias(&a,(NativeAliasInt *)&a)==0 && local_record(&b,(struct NativeAliasRecord *)&b)==0"
      "&& alias_provider(&c,(NativeAliasInt *)&c)==0 && alias_record_provider(&d,(struct NativeAliasRecord *)&d)==0"
      "&& a==0 && b==0 && c==0 && d==0 ? 0:1;}";
  InterfaceImportInput input; input.source_path = interface.string();
  input.sidecar_path = interface.string() + ".json"; input.locked_identity = artifact.identity;
  consumer.imported_interfaces = {input};
  auto consumed = Compile(consumer); Compiled(consumed);
  const auto program = (root / "program").string();
  Execute({consumed.compiler_driver_path, "--no-default-config", "--target=" + provider.target_triple,
      "-fuse-ld=lld", "-O2", provider.output_path, consumer.output_path, "-o", program});
  Execute({program});
  std::cout << "Strict-aliasing source-free execution " << standard << '\n';
}
}
int main() {
  for (const std::string standard : {"c17", "c23", "c++20", "c++23"}) Check(standard);
}

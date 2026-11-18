#include "native_target_test_support.h"
#include "clang_foundation/compile_session.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/StringSaver.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
using namespace objc3::clang_foundation;
namespace fs = std::filesystem;
namespace {
void Require(bool condition, const std::string &message) {
  if (!condition) throw std::runtime_error(message);
}
void Compiled(const CompileResult &result) {
  if (result.success) return;
  std::cerr << result.failure << '\n';
  for (const auto &item : result.diagnostics) std::cerr << item.message << '\n';
  for (const auto &item : result.interface_import_diagnostics) std::cerr << item.message << '\n';
  if (result.interface_artifact)
    for (const auto &item : result.interface_artifact->diagnostics)
      std::cerr << item.code << ": " << item.message << '\n';
  throw std::runtime_error("native compilation failed");
}
void Write(const fs::path &path, const std::string &text) {
  std::ofstream stream(path); stream << text; Require(stream.good(), "write failed: " + path.string());
}
void Execute(const std::vector<std::string> &command) {
  std::vector<llvm::StringRef> arguments(command.begin(), command.end());
  Require(llvm::sys::ExecuteAndWait(command.front(), arguments) == 0, "command failed: " + command.front());
}
void Check(const std::string &standard, bool arc) {
  llvm::SmallString<128> temporary;
  Require(!llvm::sys::fs::createUniqueDirectory("objc3-public-async", temporary), "temporary directory failed");
  const fs::path root(temporary.c_str());
  struct Cleanup { fs::path path; ~Cleanup() { fs::remove_all(path); } } cleanup{root};
  const bool cxx = standard.starts_with("c++");
  const auto source = root / (cxx ? "provider.mm" : "provider.m");
  const std::string implementation = R"(
#include "objc3_runtime_services.h"
static int calls;
int keyword_count(void){return calls;}
int keyword_leaf(int value) async {++calls;return value+1;}
int keyword_parent(int value) async {return await keyword_leaf(value)+1;}
int keyword_indirect(int value) async {
  int (*const work)(int) async=keyword_leaf;
  return await work(value);
}
void keyword_void(void) async {++calls;}
)";
  Write(source, implementation);
  CompileRequest provider;
  provider.input_path = source.string(); provider.language = cxx ? BaseLanguage::ObjectiveCxx : BaseLanguage::ObjectiveC;
  provider.objc3 = true; provider.arc = arc; provider.objc_runtime = "gnustep-2.2";
  provider.target_triple = OBJC3_TEST_NATIVE_LINUX_TARGET;
  provider.arguments = {"-std=" + standard, "-O2", "-fblocks", std::string("-I") + OBJC3_PUBLIC_ASYNC_HEADERS};
  provider.emit = EmitKind::Object; provider.output_path = (root / "provider.o").string();
  provider.emit_semantic_interface = true; provider.module_name = "test.PublicAsync";
  provider.package_identity = "public-async-v1";
  provider.interface_exports = {"keyword_count", "keyword_leaf", "keyword_parent", "keyword_indirect", "keyword_void"};
  auto built = Compile(provider); Compiled(built);
  Require(built.interface_artifact && built.interface_artifact->success, "interface missing");
  const auto artifact = *built.interface_artifact;
  Require(artifact.source_bytes.find("__objc3_async_frame") != std::string::npos, "physical frame identity omitted");
  // Public syntax normalizes to the existing persistent frame ABI, preserving
  // the separate C++ task ABI rather than conflating two physical entries.
  auto intrinsic = implementation;
  const auto replace_all = [&](const std::string &from, const std::string &to) {
    for (auto position = intrinsic.find(from); position != std::string::npos;
         position = intrinsic.find(from, position + to.size()))
      intrinsic.replace(position, from.size(), to);
  };
  replace_all(" async", " __objc3_async_frame");
  replace_all("await ", "__objc3_await ");
  auto normalized = provider; normalized.source_text = intrinsic; normalized.emit = EmitKind::Syntax; normalized.output_path.clear();
  auto replay = Compile(normalized); Compiled(replay);
  Require(replay.interface_artifact && replay.interface_artifact->success, "normalized interface missing");
  Require(replay.interface_artifact->source_bytes == artifact.source_bytes &&
          replay.interface_artifact->identity.semantic_fingerprint == artifact.identity.semantic_fingerprint,
          "public spelling changed the physical or semantic interface");
  const auto interface = root / "PublicAsync.objc3i";
  Write(interface, artifact.source_bytes); Write(interface.string() + ".json", artifact.sidecar_json);
  Require(fs::remove(source), "provider source removal failed");
  CompileRequest consumer = provider;
  consumer.input_path = (root / (cxx ? "consumer.mm" : "consumer.m")).string();
  consumer.output_path = (root / "consumer.o").string();
  consumer.emit_semantic_interface = false; consumer.module_name.clear();
  consumer.package_identity.clear(); consumer.interface_exports.clear();
  InterfaceImportInput input; input.source_path = interface.string();
  input.sidecar_path = interface.string() + ".json"; input.locked_identity = artifact.identity;
  consumer.imported_interfaces = {input};
  consumer.source_text = R"(
#include "objc3_runtime_services.h"
static int execute(objc3_async_frame *frame,objc3_executor_t *executor,int expected,int empty,int cancelled){
  objc3_task_t *task=0;
  if(objc3_async_frame_start(frame,executor,0,&task)!=OBJC3_SERVICE_OK || frame->context)return 1;
  if(cancelled)objc3_task_request_cancel(task);
  unsigned executed=0;
  if(objc3_executor_pump(executor,1000,&executed)!=OBJC3_SERVICE_OK)return 3;
  objc3_completion completion={0};
  if(objc3_task_get_completion(task,&completion)!=OBJC3_SERVICE_OK)return 4;
  if(cancelled){if(completion.kind!=OBJC3_COMPLETION_CANCELLED)return 5;}
  else if(completion.kind!=OBJC3_COMPLETION_SUCCESS || completion.has_payload==empty)return 6;
  else if(!empty && (!completion.payload || *(const int *)completion.payload!=expected))return 7;
  objc3_task_release(task);return 0;
}
int main(void){
  objc3_executor_t *executor=0;
  if(objc3_executor_create(1,&executor)!=OBJC3_SERVICE_OK || keyword_count()!=0)return 10;
  objc3_async_frame parent=keyword_parent(40);
  if(keyword_count()!=0 || execute(&parent,executor,42,0,0))return 11;
  objc3_async_frame indirect=keyword_indirect(41);
  if(execute(&indirect,executor,42,0,0))return 12;
  objc3_async_frame empty=keyword_void();
  if(execute(&empty,executor,0,1,0) || keyword_count()!=3)return 13;
  objc3_async_frame cancelled=keyword_parent(40);
  if(execute(&cancelled,executor,0,0,1) || keyword_count()!=3)return 14;
  objc3_executor_release(executor);return 0;
}
)";
  Compiled(Compile(consumer));
  const auto program = (root / "program").string();
  std::vector<std::string> link{OBJC3_PUBLIC_ASYNC_LINKER, "--no-default-config", "--target=" OBJC3_TEST_NATIVE_LINUX_TARGET};
  llvm::BumpPtrAllocator allocator; llvm::StringSaver saver(allocator);
  llvm::SmallVector<const char *, 16> flags;
  llvm::cl::TokenizeGNUCommandLine(OBJC3_PUBLIC_ASYNC_LINK_FLAGS, saver, flags);
  link.insert(link.end(), flags.begin(), flags.end());
  for (const auto &argument : {std::string("-fuse-ld=lld"), provider.output_path, consumer.output_path,
       std::string(OBJC3_PUBLIC_ASYNC_RUNTIME), std::string(OBJC3_PUBLIC_ASYNC_OBJC),
       "-Wl,-rpath," + fs::path(OBJC3_PUBLIC_ASYNC_OBJC).parent_path().string(),
       std::string("-pthread"), std::string("-ldl"), std::string("-o"), program})
    link.push_back(argument);
  Execute(link);
  Execute({program});
  std::cout << "Public async source-free execution " << standard << (arc ? " ARC\n" : " MRC\n");
}
}
int main() {
  try {
    for (const std::string standard : {"c17", "c23", "c++20", "c++23"})
      for (const bool arc : {false, true}) Check(standard, arc);
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
}

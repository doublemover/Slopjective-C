#include "native_target_test_support.h"
#include "clang_foundation/compile_session.h"
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using namespace objc3::clang_foundation;
namespace fs = std::filesystem;
namespace {
void Require(const CompileResult &result, const std::string &diagnostic = {}) {
  bool found = diagnostic.empty();
  for (const auto &item : result.diagnostics)
    found |= item.message.find(diagnostic) != std::string::npos;
  if (result.success != diagnostic.empty() || !found) {
    std::cerr << result.failure << '\n';
    for (const auto &item : result.diagnostics) std::cerr << item.message << '\n';
    std::abort();
  }
}
void Check(bool arc, const std::string &standard) {
  const bool cxx = standard != "c17";
  const auto root = fs::temp_directory_path() /
      ("objc3-global-provenance-" + std::to_string(
          std::chrono::steady_clock::now().time_since_epoch().count()));
  fs::create_directory(root);
  struct Cleanup { fs::path path; ~Cleanup() { fs::remove_all(path); } } cleanup{root};
  CompileRequest provider;
  provider.input_path = (root / (cxx ? "provider.mm" : "provider.m")).string();
  provider.source_text = "extern const char *published_pointer;extern int published_number;extern const char *published_array[2];"
      "struct PublishedSlots {const char *pointer;int number;};"
      "extern struct PublishedSlots published_slots;";
  provider.language = cxx ? BaseLanguage::ObjectiveCxx : BaseLanguage::ObjectiveC;
  provider.objc3 = true; provider.arc = arc; provider.strict_concurrency = true;
  provider.objc_runtime = "gnustep-2.2";
  provider.target_triple = OBJC3_TEST_NATIVE_LINUX_TARGET;
  provider.arguments = {"-std=" + standard, "-fblocks",
      std::string("-I") + OBJC3_GLOBAL_RUNTIME_HEADERS};
  provider.emit_semantic_interface = true;
  provider.module_name = "test.GlobalSlots"; provider.package_identity = "global-slots-v1";
  provider.interface_exports = {"published_pointer", "published_number", "PublishedSlots", "published_slots", "published_array"};
  auto published = Compile(provider); Require(published);
  assert(published.interface_artifact && published.interface_artifact->success);
  const auto &artifact = *published.interface_artifact;
  const auto interface = root / "GlobalSlots.objc3i";
  { std::ofstream file(interface); file << artifact.source_bytes; assert(file.good()); }
  { std::ofstream file(interface.string() + ".json"); file << artifact.sidecar_json; assert(file.good()); }
  assert(!fs::exists(provider.input_path));
  auto consumer = provider;
  consumer.input_path = (root / (cxx ? "consumer.mm" : "consumer.m")).string();
  consumer.emit_semantic_interface = false; consumer.module_name.clear();
  consumer.package_identity.clear(); consumer.interface_exports.clear();
  InterfaceImportInput input; input.source_path = interface.string();
  input.sidecar_path = interface.string() + ".json"; input.locked_identity = artifact.identity;
  consumer.imported_interfaces = {input};
  const std::string preamble =
      "#include \"objc3_runtime_services.h\"\n"
      "typedef void (^GlobalWork)(void) __objc3_async_frame;"
      "void *global_detached(GlobalWork) __attribute__((objc_task_detached));"
      "void global_close(int) __attribute__((nothrow));";
  auto run = [&](const std::string &body, const std::string &diagnostic = {}) {
    consumer.source_text = preamble + body; Require(Compile(consumer), diagnostic);
  };
  for (const char *slot : {"published_pointer", "published_slots.pointer", "published_array[1]"}) {
    run("void rejected(borrowed const char *loan){" + std::string(slot) +
        "=loan;const char *value=" + slot +
        ";global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
    run("void ordinary(void){" + std::string(slot) + "=\"static\";const char *value=" + slot +
        ";global_detached(^void(void) __objc3_async_frame {(void)value;});}");
  }
  run("void rejected(borrowed const char *loan){const char **const slot=&published_pointer;"
      "*slot=loan;const char *value=*slot;global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
  run("void rejected(borrowed const char *loan){extern const char *published_pointer;published_pointer=loan;"
      "const char *value=published_pointer;global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
  run("void rejected(borrowed const char *loan){static const char *slot;slot=loan;"
      "const char *value=slot;global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
  run(std::string(cxx ? "thread_local" : "_Thread_local") +
      " const char *thread_slot;void rejected(borrowed const char *loan){thread_slot=loan;"
      "const char *value=thread_slot;global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
  if (cxx)
    run("void rejected(borrowed const char *loan){auto &slot=published_pointer;slot=loan;"
        "const char *value=slot;global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
  for (const char *slot : {"published_number", "published_slots.number"})
    run("void rejected(void){int descriptor __attribute__((objc_resource(close=global_close,invalid=-1)))=3;" +
        std::string(slot) + "=descriptor;int value=" + slot +
        ";global_detached(^void(void) __objc3_async_frame {(void)value;});}", "cleanup-managed capture");
  run("void observed(borrowed const char *loan){published_pointer=loan;int value=published_pointer!=0;"
      "global_detached(^void(void) __objc3_async_frame {(void)value;});}");
  const std::string crossing = preamble +
      (cxx ? "\n#include \"objc3_task.hpp\"\n" : "") +
      "typedef const char *GlobalUnchecked __attribute__((objc_unsafe_sendable));"
      "int global_remote(GlobalUnchecked) __attribute__((nothrow,objc_executor(main)));"
      "int global_remote_number(int) __attribute__((nothrow,objc_executor(main)));";
  consumer.source_text = crossing +
      "int rejected(borrowed const char *loan) __objc3_async_frame {published_pointer=loan;"
      "return __objc3_await global_remote(published_pointer);}";
  Require(Compile(consumer), "borrowed concurrent value");
  consumer.source_text = crossing +
      "int rejected(void) __objc3_async_frame {int descriptor __attribute__((objc_resource(close=global_close,invalid=-1)))=3;"
      "published_number=descriptor;return __objc3_await global_remote_number(published_number);}";
  Require(Compile(consumer), "cleanup-managed concurrent value");
  std::cout << "Global provenance source-free " << standard << (arc ? " ARC\n" : " MRC\n");
}
}
int main() {
  for (bool arc : {false, true})
    for (const std::string standard : {"c17", "c++20", "c++23"}) Check(arc, standard);
}

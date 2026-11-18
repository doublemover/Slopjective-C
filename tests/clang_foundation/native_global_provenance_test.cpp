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
      "extern struct PublishedSlots published_slots;"
      "inline void published_store(const char *value){published_pointer=value;}"
      "inline const char *published_load(void){return published_pointer;}"
      "inline void published_chain(const char *value){published_store(value);}"
      "inline const char *published_roundtrip(const char *value){published_chain(value);return published_load();}"
      "inline void published_store_number(int value){published_number=value;}"
      "inline int published_load_number(void){return published_number;}"
      "inline int published_pointer_number(const int *value){return *value;}"
      "inline const char *published_pointer_load(const char *const *value){return *value;}"
      "inline char published_pointer_byte(const char *value){return *value;}"
      "inline void published_indirect_store(const char *value){void (*const call)(const char *)=published_store;call(value);}"
      "inline const char *published_indirect_load(void){const char *(*const call)(void)=published_load;return call();}"
      "inline int published_indirect_pointer(const int *value){int (*const call)(const int *)=published_pointer_number;return call(value);}";
  provider.language = cxx ? BaseLanguage::ObjectiveCxx : BaseLanguage::ObjectiveC;
  provider.objc3 = true; provider.arc = arc; provider.strict_concurrency = true;
  provider.objc_runtime = "gnustep-2.2";
  provider.target_triple = OBJC3_TEST_NATIVE_LINUX_TARGET;
  provider.arguments = {"-std=" + standard, "-fblocks",
      std::string("-I") + OBJC3_GLOBAL_RUNTIME_HEADERS};
  provider.emit_semantic_interface = true;
  provider.module_name = "test.GlobalSlots"; provider.package_identity = "global-slots-v1";
  provider.interface_exports = {"published_pointer", "published_number", "PublishedSlots", "published_slots", "published_array",
      "published_store", "published_load", "published_chain", "published_roundtrip", "published_store_number", "published_load_number",
      "published_pointer_number", "published_pointer_load", "published_pointer_byte",
      "published_indirect_store", "published_indirect_load", "published_indirect_pointer"};
  if (cxx) {
    *provider.source_text +=
        "inline void published_reference_store(const char *&slot,const char *value){slot=value;}"
        "inline const char *published_reference_load(const char *const &slot){return slot;}"
        "inline void published_reference_chain(const char *&slot,const char *value){published_reference_store(slot,value);}"
        "inline void published_reference_number(int &slot,int value){slot=value;}"
        "struct PublishedReferenceReceiver {"
        "void store(const char *&slot,const char *value){slot=value;}"
        "void chain(const char *&slot,const char *value){store(slot,value);}"
        "virtual void virtual_store(const char *&slot,const char *value){slot=value;}"
        "};struct PublishedReferenceFinal final:PublishedReferenceReceiver {"
        "void virtual_store(const char *&slot,const char *value)override{slot=value;}};"
        "inline void published_stateless_store(const char *value){auto work=[](const char *input){published_pointer=input;};work(value);}"
        "inline void published_stateless_reference(const char *&slot,const char *value){auto work=[](const char *&target,const char *input){target=input;};work(slot,value);}";
    for (const char *name : {"published_reference_store", "published_reference_load", "published_reference_chain", "published_reference_number"})
      provider.interface_exports.push_back(name);
    provider.interface_exports.push_back("PublishedReferenceReceiver");
    provider.interface_exports.push_back("PublishedReferenceFinal");
    provider.interface_exports.push_back("published_stateless_store");
    provider.interface_exports.push_back("published_stateless_reference");
    if (standard == "c++23") {
      *provider.source_text += "inline void published_static_store(const char *value){auto work=[](const char *input) static {published_pointer=input;};work.operator()(value);work(value);}";
      provider.interface_exports.push_back("published_static_store");
    }
  }
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
  run("void rejected(borrowed const char *loan){published_store(loan);const char *value=published_pointer;"
      "global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
  run("void rejected(borrowed const char *loan){published_chain(loan);const char *value=published_load();"
      "global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
  run("void rejected(borrowed const char *loan){const char *value=published_roundtrip(loan);"
      "global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
  run("void ordinary(void){const char *value=published_roundtrip(\"static\");"
      "global_detached(^void(void) __objc3_async_frame {(void)value;});}");
  run("void rejected(void){int descriptor __attribute__((objc_resource(close=global_close,invalid=-1)))=3;"
      "published_store_number(descriptor);int value=published_load_number();"
      "global_detached(^void(void) __objc3_async_frame {(void)value;});}", "cleanup-managed capture");
  run("void rejected(borrowed const char *loan){const char *slot=loan;const char *value=published_pointer_load(&slot);"
      "global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
  run("void rejected(void){int descriptor __attribute__((objc_resource(close=global_close,invalid=-1)))=3;"
      "int value=published_pointer_number(&descriptor);"
      "global_detached(^void(void) __objc3_async_frame {(void)value;});}", "cleanup-managed capture");
  run("void ordinary(borrowed const char *loan){const char *slot=\"static\";const char *value=published_pointer_load(&slot);"
      "char byte=published_pointer_byte(loan);int number=42;int scalar=published_pointer_number(&number);"
      "global_detached(^void(void) __objc3_async_frame {(void)value;(void)byte;(void)scalar;});}");
  run("void rejected(borrowed const char *loan){published_indirect_store(loan);const char *value=published_indirect_load();"
      "global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
  run("void ordinary(void){published_indirect_store(\"static\");const char *value=published_indirect_load();"
      "global_detached(^void(void) __objc3_async_frame {(void)value;});}");
  run("void rejected(void){int descriptor __attribute__((objc_resource(close=global_close,invalid=-1)))=3;"
      "int value=published_indirect_pointer(&descriptor);global_detached(^void(void) __objc3_async_frame {(void)value;});}", "cleanup-managed capture");
  if (cxx) {
    for (const char *slot : {"local", "published_pointer", "published_slots.pointer", "published_array[1]"})
      run("void rejected(borrowed const char *loan){const char *local=\"static\";published_reference_chain(" +
          std::string(slot) + ",loan);const char *value=published_reference_load(" + slot +
          ");global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
    run("void ordinary(void){const char *slot=\"static\";published_reference_chain(slot,\"other\");"
        "const char *value=published_reference_load(slot);global_detached(^void(void) __objc3_async_frame {(void)value;});}");
    run("void rejected(void){int descriptor __attribute__((objc_resource(close=global_close,invalid=-1)))=3;"
        "int slot=0;published_reference_number(slot,descriptor);int value=slot;"
        "global_detached(^void(void) __objc3_async_frame {(void)value;});}", "cleanup-managed capture");
    run("void ordinary(void){int slot=0;published_reference_number(slot,42);int value=slot;"
        "global_detached(^void(void) __objc3_async_frame {(void)value;});}");
    for (const char *selected : {"first", "second"})
      run("void rejected(borrowed const char *loan){const char *first=\"static\",*second=\"static\";"
          "published_reference_store(first,loan);published_reference_store(second,loan);const char *value=" +
          std::string(selected) + ";global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
    for (const char *call : {"source.chain", "source.PublishedReferenceReceiver::virtual_store", "final_source.virtual_store"}) {
      const std::string locals = "PublishedReferenceReceiver source;PublishedReferenceFinal final_source;const char *slot=\"static\";";
      run("void rejected(borrowed const char *loan){" + locals + call + "(slot,loan);const char *value=slot;"
          "global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
      run("void ordinary(void){" + locals + call + "(slot,\"other\");const char *value=slot;"
          "global_detached(^void(void) __objc3_async_frame {(void)value;});}");
    }
    run("void rejected(borrowed const char *loan){published_stateless_store(loan);const char *value=published_pointer;"
        "global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
    run("void rejected(borrowed const char *loan){const char *slot=\"static\";published_stateless_reference(slot,loan);const char *value=slot;"
        "global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
    run("void ordinary(void){const char *slot=\"static\";published_stateless_reference(slot,\"other\");const char *value=slot;"
        "global_detached(^void(void) __objc3_async_frame {(void)value;});}");
    if (standard == "c++23")
      run("void rejected(borrowed const char *loan){published_static_store(loan);const char *value=published_pointer;"
          "global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
    run("\n#include <utility>\nvoid ordinary(void){auto const work=&published_reference_store;const char *slot=\"static\";"
        "std::move(work)(slot,\"other\");const char *value=slot;"
        "global_detached(^void(void) __objc3_async_frame {(void)value;});}");
    run("\n#include <utility>\nvoid rejected(borrowed const char *loan){auto const work=&published_reference_store;"
        "auto const &alias=work;const char *slot=\"static\";std::as_const(alias)(slot,loan);const char *value=slot;"
        "global_detached(^void(void) __objc3_async_frame {(void)value;});}", "borrowed capture");
  }
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

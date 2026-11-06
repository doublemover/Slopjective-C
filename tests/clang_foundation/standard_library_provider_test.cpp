#include "clang_foundation/compile_session.h"
#include "llvm/ADT/ScopeExit.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/StringSaver.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace objc3::clang_foundation;
namespace fs=std::filesystem;
namespace {
void Success(const CompileResult &result) {
  if(!result.success) {
    std::cerr<<result.failure<<'\n';
    for(const auto &d:result.diagnostics)std::cerr<<d.spelling.file<<':'<<d.spelling.line<<": "<<d.message<<'\n';
    for(const auto &d:result.interface_import_diagnostics)std::cerr<<d.code<<": "<<d.message<<'\n';
    if(result.interface_artifact)for(const auto &d:result.interface_artifact->diagnostics)std::cerr<<d.code<<": "<<d.message<<'\n';
  }
  assert(result.success);
}
void Write(const fs::path &path,const std::string &text) {
  std::ofstream file(path);file<<text;assert(file.good());
}
void Execute(const std::vector<std::string> &owned) {
  std::vector<llvm::StringRef> argv(owned.begin(),owned.end());
  std::string error;bool failed=false;
  const auto code=llvm::sys::ExecuteAndWait(owned.front(),argv,std::nullopt,{},60,0,&error,&failed);
  if(code || failed)std::cerr<<owned.front()<<": "<<error<<" ("<<code<<")\n";
  assert(!failed && code==0);
}
std::vector<std::string> Instrumentation() {
  llvm::BumpPtrAllocator allocator;llvm::StringSaver saver(allocator);llvm::SmallVector<const char *,16> flags;
  llvm::cl::TokenizeGNUCommandLine(OBJC3_STANDARD_LIBRARY_TEST_LINK_FLAGS,saver,flags);
  std::vector<std::string> result;
  for(const char *flag:flags) {
    const std::string value(flag);
    if(value.starts_with("-fsanitize=") || value.starts_with("-fno-sanitize") ||
        value=="-fno-omit-frame-pointer")result.push_back(value);
  }
  return result;
}
void Run(const fs::path &root,const char *standard,int ownership,const char *optimization,
         const std::vector<std::string> &library_arguments) {
  fs::create_directories(root/"provider");
  const bool objc=ownership!=0;
  auto request=[&](const fs::path &input,const fs::path &output) {
    CompileRequest value;value.input_path=input.string();value.output_path=output.string();
    value.emit=EmitKind::Object;value.target_triple="x86_64-unknown-linux-gnu";
    value.language=objc?BaseLanguage::ObjectiveCxx:BaseLanguage::Cxx;
    value.objc3=objc;value.arc=ownership==2;if(objc)value.objc_runtime="gnustep-2.2";
    value.arguments={std::string("-std=")+standard,optimization,"-resource-dir=" OBJC3_STANDARD_LIBRARY_TEST_RESOURCE_DIR};
    auto instrumentation=Instrumentation();value.arguments.insert(value.arguments.end(),instrumentation.begin(),instrumentation.end());
    value.arguments.insert(value.arguments.end(),library_arguments.begin(),library_arguments.end());
    return value;
  };
  const auto provider_path=root/"provider/module.mm";
  Write(provider_path,R"(
#include <functional>
#include <memory>
#include <numeric>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
namespace NativeStandard {
using Word=std::string;
using Words=std::vector<Word>;
Words words() {return {"one","three","seven"};}
std::optional<Word> maybe_word(bool present) {
  if(present)return Word("verified");
  return std::nullopt;
}
std::function<int(int)> adder(int base) {
  auto owner=std::make_shared<int>(base);
  return [owner](int value){return *owner+value;};
}
inline int score(const Words &values) {
  return std::accumulate(values.begin(),values.end(),0,
      [](int total,const Word &word){return total+static_cast<int>(word.size());});
}
template<class T> requires std::is_integral_v<T>
inline constexpr T twice(T value) {return std::move(value)*2;}
}
)");
  auto provider=request(provider_path,root/"provider.o");
  provider.module_owned_names=true;provider.module_source_root=(root/"provider").string();
  provider.module_name="fixture.StandardLibrary";provider.package_identity="standard-library-v1";
  provider.interface_exports={"NativeStandard"};provider.emit_semantic_interface=true;
  auto built=Compile(provider);Success(built);
  assert(built.interface_artifact && built.interface_artifact->success);
  const auto &artifact=*built.interface_artifact;
  const auto interface=root/"StandardLibrary.objc3i";
  Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
  assert(fs::remove(provider_path));
  for(const char *name:{"a","b"}) {
    const auto path=root/(std::string("consumer-")+name+".mm");
    Write(path,"static_assert(NativeStandard::twice(21)==42);\nextern \"C\" int check_"+std::string(name)+R"(() {
  auto values=NativeStandard::words();
  if(values.size()!=3 || NativeStandard::score(values)!=13)return 1;
  auto present=NativeStandard::maybe_word(true),absent=NativeStandard::maybe_word(false);
  if(!present || *present!="verified" || absent)return 2;
  auto callable=NativeStandard::adder(40);auto copied=callable;callable={};
  if(copied(2)!=42)return 3;
  values.push_back("consumer");return NativeStandard::score(values)==21?0:4;
}
)"+"extern \"C\" void *address_"+name+"(){return reinterpret_cast<void *>(&NativeStandard::words);}\n");
    auto consumer=request(path,root/(std::string(name)+".o"));
    InterfaceImportInput imported;imported.source_path=interface.string();imported.locked_identity=artifact.identity;
    consumer.imported_interfaces={imported};Success(Compile(consumer));
  }
  const auto main=root/"main.cpp";
  Write(main,R"(extern "C" int check_a();extern "C" int check_b();
extern "C" void *address_a();extern "C" void *address_b();
int main(){return check_a() || check_b() || address_a()!=address_b();}
)");
  auto entry=request(main,root/"main.o");Success(Compile(entry));
  std::vector<std::string> link={OBJC3_STANDARD_LIBRARY_TEST_LINKER,"-fuse-ld=lld",
      (root/"provider.o").string(),(root/"a.o").string(),(root/"b.o").string(),entry.output_path,
      "-o",(root/"witness").string()};
  link.insert(link.end(),library_arguments.begin(),library_arguments.end());
  llvm::BumpPtrAllocator allocator;llvm::StringSaver saver(allocator);llvm::SmallVector<const char *,16> flags;
  llvm::cl::TokenizeGNUCommandLine(OBJC3_STANDARD_LIBRARY_TEST_LINK_FLAGS,saver,flags);
  for(const char *flag:flags)link.emplace_back(flag);
  if(objc) {
    const fs::path runtime=OBJC3_STANDARD_LIBRARY_TEST_OBJC_LIBRARY;
    link.push_back(runtime.string());link.push_back("-Wl,-rpath,"+runtime.parent_path().string());
  }
  Execute(link);Execute({(root/"witness").string()});
}
}
int main(int argc,char **argv) {
  bool full=false;std::vector<std::string> library_arguments;
  for(int i=1;i<argc;++i)if(std::string(argv[i])=="--full")full=true;else library_arguments.emplace_back(argv[i]);
  llvm::SmallString<128> temporary;
  assert(!llvm::sys::fs::createUniqueDirectory("objc3-standard-library-provider",temporary));
  const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
  for(const char *standard:{"c++20","c++23"})for(int ownership=0;ownership!=3;++ownership)
    for(const char *optimization:{"-O0","-O2"}) {
      if(!full && !(std::string(standard)=="c++20" && ownership==0 && std::string(optimization)=="-O0") &&
          !(std::string(standard)=="c++23" && ownership==2 && std::string(optimization)=="-O2"))continue;
      const auto label=std::string(standard)+"-"+std::to_string(ownership)+optimization;
      std::cerr<<"STANDARD LIBRARY PROVIDER "<<label<<'\n';
      Run(root/label,standard,ownership,optimization,library_arguments);
    }
}

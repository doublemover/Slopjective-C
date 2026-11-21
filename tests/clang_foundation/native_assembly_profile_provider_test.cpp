#include "standard_library_test_support.h"
#include "clang_foundation/interface_module_resolver.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;

namespace {
constexpr const char *Source=R"assembly(
namespace AssemblyProfile {
inline int copied(int n){
#if defined(__x86_64__) || defined(__i386__)
 int result;
#if TEST_INTEL
 __asm__("mov %0, %1" : "=r"(result) : "r"(n));
#else
 __asm__("movl %1, %0" : "=r"(result) : "r"(n));
#endif
 return result;
#else
 __asm__("" : "+r"(n));return n;
#endif
}
inline int dual(int n){
#if defined(__x86_64__) || defined(__i386__)
 int result;__asm__("{movl %1, %0|mov %0, %1}" : "=r"(result) : "r"(n));return result;
#else
 return n;
#endif
}
int provider(int);
inline int run(){return copied(19)!=19 || dual(23)!=23 || provider(29)!=29;}
int provider(int n){return copied(n);}
}
)assembly";

void Run(const fs::path &root,const char *standard,int ownership,bool intel){
 fs::create_directories(root/"provider");const auto source=root/"provider/module.mm";
 auto request=[&](const fs::path &input,const fs::path &output){
  auto value=Request(input,output,standard,ownership,"-O2",{});
#if defined(__x86_64__) || defined(__i386__)
  value.arguments.push_back(intel?"-masm=intel":"-masm=att");
#endif
  value.arguments.push_back(intel?"-DTEST_INTEL=1":"-DTEST_INTEL=0");return value;
 };
 auto emit=[&](const std::string &text,bool owned){
  Write(source,text);auto value=request(source,root/"provider.o");value.emit_semantic_interface=true;
  value.module_name="fixture.AssemblyProfile";value.package_identity="assembly-dialect-v1";value.interface_exports={"AssemblyProfile"};
  value.module_owned_names=owned;if(owned)value.module_source_root=(root/"provider").string();
  auto result=Compile(value);Success(result);assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;
 };
 const auto artifact=emit(Source,true);const auto replay=emit(artifact.source_bytes,false);
 assert(artifact.source_bytes==replay.source_bytes && artifact.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
 assert(artifact.identity.language_profile.find(std::string(";native-inline-asm-dialect:v1;mode=")+(intel?"1":"0"))!=std::string::npos);
 emit(Source,true);assert(fs::remove(source));
 const auto interface=root/"AssemblyProfile.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 auto dependency=[&](CompileRequest &value){InterfaceImportInput input;input.source_path=interface.string();input.locked_identity=artifact.identity;value.imported_interfaces={input};};
 for(const char *name:{"a","b"}){
  auto input=root/(std::string(name)+".mm");Write(input,"extern \"C\" int check_"+std::string(name)+"(){return AssemblyProfile::run();}");
  auto value=request(input,root/(std::string(name)+".o"));dependency(value);Success(Compile(value));
 }
#if defined(__x86_64__) || defined(__i386__)
 const auto wrong=root/"wrong.mm";Write(wrong,"int wrong(){return AssemblyProfile::run();}");
 auto mismatch=request(wrong,root/"wrong.o");dependency(mismatch);mismatch.arguments.push_back(intel?"-masm=att":"-masm=intel");
 const auto rejected=Compile(mismatch);
 assert(!rejected.success && std::any_of(rejected.interface_import_diagnostics.begin(),rejected.interface_import_diagnostics.end(),
     [](const auto &d){return d.code=="O3IFC135";}));
#endif
 if(ownership){
  const auto directive=root/"directive.mm";Write(directive,"@import fixture.AssemblyProfile;\nint directive(){return AssemblyProfile::run();}");
  auto value=request(directive,root/"directive.o");value.source_directed_imports=true;value.interface_search_paths={root.string()};
  InterfaceModuleRequest locked;locked.canonical_name=artifact.identity.canonical_name;locked.locked_identity=artifact.identity;locked.initially_visible=false;
  value.module_imports={locked};Success(Compile(value));
#if defined(__x86_64__) || defined(__i386__)
  value.arguments.push_back(intel?"-masm=att":"-masm=intel");const auto rejected=Compile(value);assert(!rejected.success);
#endif
 }
 const auto main=root/"main.cpp";Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();int main(){return check_a() || check_b();}");
 auto entry=request(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,ownership!=0,{});
}
}
int main(){
 test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-assembly-profile",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 std::vector<bool> dialects={false};
#if defined(__x86_64__) || defined(__i386__)
 dialects.push_back(true);
#endif
 for(const auto &[standard,ownership]:std::vector<std::pair<const char *,int>>{{"c++20",0},{"c++23",0},{"c++20",1},{"c++23",2}})
  for(bool intel:dialects){const auto name=std::string(standard)+"-"+std::to_string(ownership)+(intel?"-intel":"-att");
   cases.Run(name,[&]{Run(root/name,standard,ownership,intel);});}
 return cases.Finish();
}

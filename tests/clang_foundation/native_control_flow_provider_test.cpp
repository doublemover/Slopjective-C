#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;

namespace {
constexpr const char *Source=R"control(
namespace NativeControl {
int leaf(int);
inline int branch(int n){if(n>0)[[likely]]return n;else [[unlikely]]return -n;}
inline int fall(int n){int value=0;switch(n){case 0:value=3;[[fallthrough]];default:value+=4;}return value;}
inline int assumed(int n){[[clang::assume(n>0)]];return n+1;}
inline int loop(int n){int value=0;
#pragma clang loop unroll_count(4) vectorize(disable)
 for(int i=0;i<n;++i)value+=i;return value;
}
template<int N> int repeated(int n){int value=0;
#pragma clang loop unroll_count(N)
 for(int i=0;i<n;++i)value+=i;return value;
}
struct Record {int run(int n)const{int value=0;[[clang::code_align(32)]]for(int i=0;i<n;++i)value+=i;return value;}};
inline int tail(int n){[[clang::musttail]]return leaf(n);}
inline int separate(int n){[[clang::nomerge]]return leaf(n);}
inline int no_inline(int n){[[clang::noinline]]return leaf(n);}
inline int inlined(int n){[[clang::always_inline]]return leaf(n);}
inline int suppressed(int n){[[clang::suppress("control-contract")]]{return n;}}
inline int labels(int n){if(n)goto positive;else goto negative;positive:return 7;negative:return 11;}
inline int indirect(bool n){void *target=n?&&one:&&two;goto *target;one:return 13;two:return 17;}
inline int basic(){asm volatile("");return 1;}
inline int empty_extended(){asm volatile("" :);return 2;}
inline int barrier(){asm volatile("" ::: "memory");return 3;}
inline int tied(int n){asm volatile("" : [value] "+r"(n) : : "memory");return n;}
inline int asm_goto(int n){asm goto("" : : "r"(n) : "memory" : finish);return n;finish:return n+1;}
inline int run(){return branch(-5)!=5 || fall(0)!=7 || assumed(3)!=4 || loop(5)!=10 || repeated<4>(5)!=10 ||
 Record{}.run(5)!=10 || tail(19)!=21 || separate(23)!=25 || no_inline(29)!=31 || inlined(31)!=33 || suppressed(37)!=37 ||
 labels(1)!=7 || labels(0)!=11 || indirect(true)!=13 || indirect(false)!=17 || basic()!=1 || empty_extended()!=2 ||
 barrier()!=3 || tied(41)!=41 || asm_goto(43)!=43;}
int leaf(int n){return n+2;}
}
)control";

void Run(const fs::path &root,const char *standard,int ownership){
 fs::create_directories(root/"provider");const auto source=root/"provider/module.mm";
 auto request=[&](const fs::path &input,const fs::path &output){return Request(input,output,standard,ownership,"-O2",{});};
 auto emit=[&](const std::string &text,const std::vector<std::string> &exports,bool owned=true){
  Write(source,text);auto value=request(source,root/"provider.o");
  value.module_owned_names=owned;if(owned)value.module_source_root=(root/"provider").string();
  value.module_name="fixture.NativeControl";value.package_identity="control-flow-v1";
  value.interface_exports=exports;value.emit_semantic_interface=true;return Compile(value);
 };
 auto publish=[&](const std::string &text){
  const auto result=emit(text,{"NativeControl"});Success(result);
  assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;
 };
 const auto artifact=publish(Source);const auto repeated=publish(Source);
 assert(artifact.source_bytes==repeated.source_bytes && artifact.sidecar_json==repeated.sidecar_json);
 assert(artifact.sidecar_json.find(kNativeControlFlowSemantic)!=std::string::npos);
 const auto replay=emit(artifact.source_bytes,{"NativeControl"},false);Success(replay);assert(replay.interface_artifact);
 if(replay.interface_artifact->source_bytes!=artifact.source_bytes)
  std::cerr<<"ORIGINAL\n"<<artifact.source_bytes<<"REPLAY\n"<<replay.interface_artifact->source_bytes;
 assert(replay.interface_artifact->source_bytes==artifact.source_bytes);
 assert(replay.interface_artifact->identity.semantic_fingerprint==artifact.identity.semantic_fingerprint);
 for(const auto &[from,to]:std::vector<std::pair<std::string,std::string>>{
     {"[[likely]]","[[unlikely]]"},{"n>0)]]","n>1)]]"},
     {"unroll_count(4)","unroll_count(8)"},{"vectorize(disable)","vectorize(enable)"},
     {"code_align(32)","code_align(64)"},{"[[clang::nomerge]]","[[clang::noinline]]"},
     {"control-contract","another-contract"},{"if(n)goto positive","if(n)goto negative"},
     {"asm volatile(\"\" : [value]","asm(\"\" : [value]"},
     {"[value] \"+r\"","[value] \"+rm\""},{"::: \"memory\"","::: \"cc\""}}){
  std::string changed=Source;const auto at=changed.find(from);assert(at!=std::string::npos);changed.replace(at,from.size(),to);
  assert(publish(changed).identity.semantic_fingerprint!=artifact.identity.semantic_fingerprint);
 }
 const auto hidden=publish("namespace NativeControl{int unpublished(int n){[[likely]]if(n)return 1;return 2;}}");
 assert(hidden.sidecar_json.find(kNativeControlFlowSemantic)==std::string::npos);
 const auto excluded=emit("namespace NativeControl{constexpr bool allowed(int n){return n>0;}inline int gated(int n){[[clang::assume(allowed(n))]];return n;}}",{"NativeControl::gated"});
 assert(!excluded.success && excluded.interface_artifact && !excluded.interface_artifact->success);
 const auto unknown=emit("namespace NativeControl{inline int gated(int n){[[clang::annotate(\"opaque\")]]return n;}}",{"NativeControl"});
 assert(!unknown.success && unknown.interface_artifact && !unknown.interface_artifact->success);
 publish(Source);assert(fs::remove(source));
 const auto interface=root/"NativeControl.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 for(const char *name:{"a","b"}){
  const auto input=root/(std::string(name)+".mm");Write(input,"extern \"C\" int check_"+std::string(name)+"(){return NativeControl::run();}");
  auto value=request(input,root/(std::string(name)+".o"));InterfaceImportInput dependency;
  dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;value.imported_interfaces={dependency};Success(Compile(value));
 }
 std::vector<llvm::StringRef> reader;for(const auto &cap:SupportedSemanticInterfaceCapabilities())if(cap!=kNativeControlFlowSemantic)reader.push_back(cap);
 auto old=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(!old);llvm::consumeError(old.takeError());
 reader.push_back(kNativeControlFlowSemantic);auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(decoded);
 auto missing=*decoded;std::erase(missing.required_semantics,kNativeControlFlowSemantic);auto invalid=EncodeSemanticInterfaceSidecar(missing,reader);
 assert(!invalid);llvm::consumeError(invalid.takeError());
 const auto main=root/"main.cpp";Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();int main(){return check_a() || check_b();}");
 auto entry=request(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,ownership!=0,{});
}
}
int main(){
 test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-control-flow",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(const auto &[standard,ownership]:std::vector<std::pair<const char *,int>>{{"c++20",0},{"c++23",0},{"c++20",1},{"c++23",2}})
  cases.Run(std::string(standard)+"-"+std::to_string(ownership),[&]{Run(root/(std::string(standard)+"-"+std::to_string(ownership)),standard,ownership);});
 return cases.Finish();
}

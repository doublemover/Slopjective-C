#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;

namespace {
constexpr const char *Source=R"states(
namespace NativeStates {
struct [[clang::consumable(unconsumed)]] Token {
 int value;
 Token() __attribute__((return_typestate(consumed))):value(0){}
 explicit Token(int n) __attribute__((return_typestate(unconsumed))):value(n){}
 [[clang::callable_when(unconsumed)]] int read() const {return value;}
 [[clang::set_typestate(consumed)]] void close(){value=0;}
 [[clang::set_typestate(unconsumed)]] void reset(int n){value=n;}
 [[clang::test_typestate(unconsumed)]] bool valid() const {return value!=0;}
 __attribute__((callable_when("unconsumed","consumed"),set_typestate(consumed))) void clear(){value=0;}
};
struct __attribute__((consumable(unconsumed),consumable_auto_cast_state,consumable_set_state_on_read)) AutoToken {
 int value;
 int read() const __attribute__((callable_when(unconsumed))){return value;}
 void close() __attribute__((set_typestate(consumed))){value=0;}
};
template<class T> struct __attribute__((consumable(unconsumed))) Generic {
 T value;
 explicit Generic(T n) __attribute__((return_typestate(unconsumed))):value(n){}
 T read() const __attribute__((callable_when(unconsumed))){return value;}
 void close() __attribute__((set_typestate(consumed))){value=T();}
};
struct Nest {
 enum class [[clang::enum_extensibility(closed)]] State:unsigned short {ready=7,done=9};
 enum [[clang::enum_extensibility(open),clang::flag_enum]] Flags:unsigned {first=1,second=2,both=3};
};
enum __attribute__((enum_extensibility(closed))) Closed:int {zero=0,one=1};
enum __attribute__((enum_extensibility(closed),flag_enum)) Bits:unsigned {low=1,high=2,all=3};
struct Packet {Nest::State state;unsigned short padding;Nest::Flags flags;};
Packet transform(Packet);
inline void need(Token t __attribute__((param_typestate(unconsumed)))){(void)t;}
inline void reopen(Token &t __attribute__((return_typestate(unconsumed)))){t.reset(13);}
inline Token make() __attribute__((return_typestate(unconsumed))){return Token(17);}
inline int run(){
 Token value(42);if(value.read()!=42)return 1;value.close();value.clear();value.reset(9);
 if(!value.valid() || value.read()!=9)return 2;need(value);reopen(value);
 if(value.read()!=13)return 3;Token made=make();if(made.read()!=17)return 4;
 Generic<int> generic{21};if(generic.read()!=21)return 5;generic.close();
 Packet result=transform(Packet{Nest::State::ready,0,Nest::both});
 return result.state!=Nest::State::done || result.flags!=Nest::second || sizeof(Nest::State)!=2;
}
Packet transform(Packet value){value.state=Nest::State::done;value.flags=Nest::second;return value;}
}
)states";

void Run(const fs::path &root,const char *standard,int ownership) {
 fs::create_directories(root/"provider");
 auto request=[&](const fs::path &input,const fs::path &output){
  return Request(input,output,standard,ownership,"-O2",{"-Wconsumed"});
 };
 const auto source=root/"provider/module.mm";
 auto publish=[&](const std::string &text){
  Write(source,text);auto provider=request(source,root/"provider.o");
  provider.module_owned_names=true;provider.module_source_root=(root/"provider").string();
  provider.module_name="fixture.NativeStates";provider.package_identity="typestate-enums-v1";
  provider.interface_exports={"NativeStates"};provider.emit_semantic_interface=true;
  const auto result=Compile(provider);Success(result);assert(result.interface_artifact && result.interface_artifact->success);
  return *result.interface_artifact;
 };
 const auto artifact=publish(Source);const auto repeated=publish(Source);
 assert(artifact.source_bytes==repeated.source_bytes && artifact.sidecar_json==repeated.sidecar_json);
 std::string alternative=Source;
 for(const auto &[from,to]:std::vector<std::pair<std::string,std::string>>{
     {"[[clang::","__attribute__(("},{",clang::",","},{"]]","))"}})
  for(size_t at=0;(at=alternative.find(from,at))!=std::string::npos;at+=to.size())alternative.replace(at,from.size(),to);
 const auto aliases=publish(alternative);
 assert(aliases.source_bytes==artifact.source_bytes && aliases.identity.semantic_fingerprint==artifact.identity.semantic_fingerprint);
 std::string reordered=Source;
 const std::string states="callable_when(\"unconsumed\",\"consumed\")";
 reordered.replace(reordered.find(states),states.size(),"callable_when(\"consumed\",\"unconsumed\",\"consumed\")");
 const auto state_set=publish(reordered);
 assert(state_set.source_bytes==artifact.source_bytes && state_set.identity.semantic_fingerprint==artifact.identity.semantic_fingerprint);
 const std::string local="namespace NativeStates{inline int local(){enum __attribute__((enum_extensibility(closed))) Mode{one=1};return one;}}";
 const auto local_only=publish(local);
 assert(local_only.sidecar_json.find(kNativeDeclarationAttributeSemantic)!=std::string::npos);
 auto changed_local=local;changed_local.replace(changed_local.find("closed"),6,"open");
 assert(publish(changed_local).identity.semantic_fingerprint!=local_only.identity.semantic_fingerprint);
 auto unpublished=local;unpublished.erase(unpublished.find("inline "),7);
 assert(publish(unpublished).sidecar_json.find(kNativeDeclarationAttributeSemantic)==std::string::npos);
 assert(artifact.sidecar_json.find(kNativeDeclarationAttributeSemantic)!=std::string::npos);
 for(const char *name:{"consumable(","consumable_auto_cast_state","consumable_set_state_on_read",
     "callable_when(","param_typestate(","return_typestate(","set_typestate(","test_typestate(",
     "enum_extensibility(","flag_enum"})assert(artifact.source_bytes.find(name)!=std::string::npos);
 for(const auto &[before,after]:std::vector<std::pair<std::string,std::string>>{
     {"consumable(unconsumed)","consumable(unknown)"},
     {"return_typestate(consumed)","return_typestate(unknown)"},
     {"callable_when(unconsumed)","callable_when(consumed)"},
     {"set_typestate(consumed)","set_typestate(unknown)"},
     {"test_typestate(unconsumed)","test_typestate(consumed)"},
     {"param_typestate(unconsumed)","param_typestate(unknown)"},
     {"enum_extensibility(closed)","enum_extensibility(open)"},
     {"consumable_auto_cast_state,",""},
     {",consumable_set_state_on_read",""},
     {",flag_enum)",")"}}) {
  std::string changed=Source;const auto at=changed.find(before);assert(at!=std::string::npos);
  changed.replace(at,before.size(),after);assert(publish(changed).identity.semantic_fingerprint!=artifact.identity.semantic_fingerprint);
 }
 publish(Source);assert(fs::remove(source));
 const auto interface=root/"NativeStates.objc3i";
 Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 auto imported_request=[&](const fs::path &input,const fs::path &output){
  auto value=request(input,output);value.arguments.push_back("-Werror=consumed");
  InterfaceImportInput imported;imported.source_path=interface.string();imported.locked_identity=artifact.identity;
  value.imported_interfaces={imported};return value;
 };
 for(const char *name:{"a","b"}){
  const auto input=root/(std::string(name)+".mm");
  Write(input,"extern \"C\" int check_"+std::string(name)+"(){return NativeStates::run();}");
  Success(Compile(imported_request(input,root/(std::string(name)+".o"))));
 }
 unsigned ordinal=0;
 for(const auto &[text,message]:std::vector<std::pair<std::string,std::string>>{
     {"int bad(){NativeStates::Token t(42);t.close();return t.read();}","invalid invocation"},
     {"void bad(){NativeStates::Token t;NativeStates::need(t);}","argument not in expected state"},
     {"int bad(){NativeStates::Generic<int> t{42};t.close();return t.read();}","invalid invocation"},
     {"NativeStates::Token bad() __attribute__((return_typestate(unconsumed))){NativeStates::Token t;t.close();return t;}","return value not in expected state"}}){
  const auto input=root/("bad-"+std::to_string(ordinal++)+".mm");Write(input,text);
  const auto result=Compile(imported_request(input,root/"bad.o"));
  const bool rejected=!result.success && std::any_of(result.diagnostics.begin(),result.diagnostics.end(),
      [&](const auto &d){return d.message.find(message)!=std::string::npos;});
  if(!rejected)test::ReportFailure(result);assert(rejected);
 }
 std::vector<llvm::StringRef> reader;
 for(const auto &capability:SupportedSemanticInterfaceCapabilities())
  if(capability!=kNativeDeclarationAttributeSemantic)reader.push_back(capability);
 reader.push_back("objc3.native-declaration-attributes.v5");
 auto old=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);
 assert(!old);llvm::consumeError(old.takeError());reader.push_back(kNativeDeclarationAttributeSemantic);
 auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(decoded);
 auto missing=*decoded;std::erase(missing.required_semantics,kNativeDeclarationAttributeSemantic);
 auto invalid=EncodeSemanticInterfaceSidecar(missing,reader);assert(!invalid);llvm::consumeError(invalid.takeError());
 const auto main=root/"main.cpp";Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();int main(){return check_a() || check_b();}");
 auto entry=request(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,ownership!=0,{});
}
}

int main(){
 test::IsolatedCases cases;llvm::SmallString<128> temporary;
 assert(!llvm::sys::fs::createUniqueDirectory("objc3-native-typestate",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(const auto &[standard,ownership]:std::vector<std::pair<const char *,int>>{
     {"c++20",0},{"c++23",0},{"c++20",1},{"c++23",2}})
  cases.Run(std::string(standard)+"-"+std::to_string(ownership),[&]{Run(root/(std::string(standard)+"-"+std::to_string(ownership)),standard,ownership);});
 return cases.Finish();
}

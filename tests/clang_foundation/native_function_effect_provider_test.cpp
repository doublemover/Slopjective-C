#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>
using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
template<class T> bool Fails(llvm::Expected<T> result){if(result)return false;llvm::consumeError(result.takeError());return true;}
constexpr const char *Source=R"effects(
typedef int (*EffectFastPointer)(int) __attribute__((nonblocking));
typedef int (*EffectNoAllocPointer)(int) __attribute__((nonallocating));
int effect_fast(int n) __attribute__((nonblocking,noinline)){return n+2;}
int effect_noalloc(int n) __attribute__((nonallocating,noinline)){return n+3;}
int effect_blocking(int n) __attribute__((blocking,noinline)){return n+4;}
int effect_allocating(int n) __attribute__((allocating,noinline)){return n+5;}
struct EffectRecord {EffectFastPointer pointer;};
inline int effect_record(struct EffectRecord value,int n) __attribute__((nonblocking)){return value.pointer(n);}
inline EffectFastPointer effect_address(void){return &effect_fast;}
typedef EffectFastPointer (*EffectFactory)(void) __attribute__((nonblocking));
EffectFastPointer effect_factory(void) __attribute__((nonblocking)){return effect_fast;}
inline EffectFactory effect_factory_address(void){return effect_factory;}
inline int effect_invoke(EffectFastPointer pointer,int n) __attribute__((nonblocking)){return pointer(n);}
#if __cplusplus
namespace EffectTemplates {
template<class T>concept EffectConstraint=requires(T value){[]() __attribute__((nonblocking)){return 42;}();};
template<class T> requires requires{[]() __attribute__((nonallocating)){return 42;}();} inline int constrained(T value){return value+2;}
template<class T>struct Identity{T value;};
template<template<class>class Wrapper>struct NestedTemplate{Wrapper<int> value;};
template<bool Enabled> inline int conditional(int n) __attribute__((nonblocking(Enabled))){return n+2;}
template<class T> inline T conditional_size(T n) __attribute__((nonallocating(sizeof(T)<=8))){return n+3;}
template<bool Enabled> using Callback=int (__attribute__((nonblocking(Enabled))) *)(int);
template<bool Enabled> struct Box {
 Callback<Enabled> pointer;
 int apply(int n) const __attribute__((nonblocking(Enabled))){return pointer(n);}
};
inline int lambda(int n){auto step=[](int value) __attribute__((nonblocking)){return value+2;};return step(n);}
inline const auto stored=[](int value) __attribute__((nonallocating)){return value+3;};
inline auto factory(){return [](int value) __attribute__((nonblocking)){return value+2;};}
inline int block(int n){int (^step)(int)=^int(int value) __attribute__((nonblocking)){return value+2;};return step(n);}
}
#endif
)effects";
void Run(const fs::path &root,const char *standard,int ownership){
 const bool c=std::string(standard)=="c17";fs::create_directories(root/"provider");
 auto request=[&](const fs::path &input,const fs::path &output){
  auto value=Request(input,output,standard,ownership,"-O2",{"-Werror=function-effects","-fblocks"});
  if(c)value.language=ownership?BaseLanguage::ObjectiveC:BaseLanguage::C;return value;
 };
 const auto source=root/"provider/module.mm";
 auto publish=[&](const std::string &text,bool owned=true){
  Write(source,text);auto value=request(source,root/"provider.o");value.module_name="fixture.FunctionEffects";value.package_identity="function-effects-v1";
  value.module_owned_names=owned;if(owned)value.module_source_root=(root/"provider").string();value.emit_semantic_interface=true;
  auto result=Compile(value);Success(result);assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;
 };
 const auto artifact=publish(Source),replay=publish(artifact.source_bytes,false);
 assert(artifact.source_bytes==replay.source_bytes && artifact.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
 const auto &reader=SupportedSemanticInterfaceCapabilities();auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(decoded);
 assert(std::find(decoded->required_semantics.begin(),decoded->required_semantics.end(),kNativeFunctionEffectSemantic)!=decoded->required_semantics.end());
 std::vector<llvm::StringRef> old(reader.begin(),reader.end());std::erase(old,kNativeFunctionEffectSemantic);assert(Fails(DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,old)));
 auto missing=*decoded;std::erase(missing.required_semantics,kNativeFunctionEffectSemantic);assert(Fails(EncodeSemanticInterfaceSidecar(missing,reader)));
 for(const char *kind:{"Namespace","Enum","EnumConstant"}) {
  auto forged=*decoded;auto owner=forged.declarations.front();owner.kind=kind;owner.stable_id="forged-effect-owner";owner.qualified_name=owner.export_root="forged_effect_owner";
  owner.canonical_type_identity="forged;native-function-effects:v1;";forged.declarations.push_back(owner);assert(Fails(EncodeSemanticInterfaceSidecar(forged,reader)));
 }
 publish(Source);assert(fs::remove(source));
 const auto interface=root/"FunctionEffects.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 InterfaceImportInput dependency;dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;
 for(const char *name:{"a","b"}) {
  const auto input=root/(std::string(name)+".mm");Write(input,std::string(R"consumer(
typedef int (*ConsumerFast)(int) __attribute__((nonblocking));
typedef ConsumerFast (*ConsumerFactory)(void) __attribute__((nonblocking));
static int verified(ConsumerFast pointer) __attribute__((nonblocking)){return pointer(40);}
)consumer")+(c?"":"extern \"C\" ")+"int check_"+name+R"consumer((void){
 ConsumerFactory factory=effect_factory_address();if(factory()(40)!=42)return 6;
 ConsumerFast pointer=effect_address();if(verified(pointer)!=42 || effect_invoke(effect_fast,40)!=42)return 1;
 struct EffectRecord record={effect_fast};if(effect_record(record,40)!=42)return 5;
 if(effect_noalloc(39)!=42 || effect_blocking(38)!=42 || effect_allocating(37)!=42)return 2;
#if __cplusplus
 static_assert(EffectTemplates::EffectConstraint<int>);if(EffectTemplates::constrained(40)!=42)return 8;
 EffectTemplates::NestedTemplate<EffectTemplates::Identity> nested{{42}};if(nested.value.value!=42)return 7;
 EffectTemplates::Box<true> box{effect_fast};
 if(box.apply(40)!=42 || EffectTemplates::conditional<true>(40)!=42 || EffectTemplates::conditional<false>(40)!=42)return 3;
 if(EffectTemplates::conditional_size(39)!=42 || EffectTemplates::lambda(40)!=42 || EffectTemplates::stored(39)!=42 || EffectTemplates::factory()(40)!=42 || EffectTemplates::block(40)!=42)return 4;
#endif
 return 0;})consumer");
  auto value=request(input,root/(std::string(name)+".o"));value.imported_interfaces={dependency};Success(Compile(value));
 }
 for(const auto &[text,message]:std::vector<std::pair<std::string,std::string>>{
     {"void rejected(void){EffectFastPointer pointer=effect_blocking;}","nonblocking"},
     {"void rejected(void){EffectNoAllocPointer pointer=effect_allocating;}","nonallocating"},
     {"int rejected(int n) __attribute__((nonblocking)){return effect_blocking(n);}","nonblocking"}}) {
  const auto input=root/"bad.mm";Write(input,text);auto value=request(input,root/"bad.o");value.imported_interfaces={dependency};auto result=Compile(value);assert(!result.success);
  if(!std::any_of(result.diagnostics.begin(),result.diagnostics.end(),[&](const auto &d){return d.message.find(message)!=std::string::npos;})) {std::cerr<<"Missing diagnostic: "<<message<<"\n"<<result.failure<<"\n";for(const auto &d:result.diagnostics)std::cerr<<d.message<<"\n";}
  assert(std::any_of(result.diagnostics.begin(),result.diagnostics.end(),[&](const auto &d){return d.message.find(message)!=std::string::npos;}));
 }
 if(!c) {
  for(const auto &text:{
      "constexpr bool hidden_effect=true;template<bool Enabled> inline int bad_effect(int n) __attribute__((nonblocking(Enabled && hidden_effect))){return n;}",
      "struct HiddenEffect{int n;};template<class T> inline int bad_effect(int n) __attribute__((nonallocating(sizeof(T)<=sizeof(HiddenEffect)))){return n;}"}) {
   const auto input=root/"hidden.mm";Write(input,text);auto value=request(input,root/"hidden.o");
   value.module_name="fixture.HiddenEffects";value.package_identity="hidden-effect-v1";value.emit_semantic_interface=true;value.interface_exports={"bad_effect"};
   auto result=Compile(value);assert(result.interface_artifact && !result.interface_artifact->success);
   if(!std::any_of(result.interface_artifact->diagnostics.begin(),result.interface_artifact->diagnostics.end(),[](const auto &d){return d.code=="O3IFC112" || d.code=="O3IFC156";}))for(const auto &d:result.interface_artifact->diagnostics)std::cerr<<d.code<<": "<<d.message<<" ["<<d.subject<<"]\n";
   assert(std::any_of(result.interface_artifact->diagnostics.begin(),result.interface_artifact->diagnostics.end(),[](const auto &d){return d.code=="O3IFC112" || d.code=="O3IFC156";}));
  }
 }
 // Validly rehashed canonical text cannot change a native effect promise.
 std::string changed=artifact.source_bytes;const std::string before="__attribute__((blocking))",after="__attribute__((allocating))";
 const auto at=changed.find(before);assert(at!=std::string::npos);changed.replace(at,before.size(),after);
 auto mutation=*decoded;mutation.canonical_source_sha256=ComputeInterfaceSourceSha256(changed);auto metadata=EncodeSemanticInterfaceSidecar(mutation,reader);assert(metadata);
 const auto altered=root/"Altered.objc3i";Write(altered,changed);Write(altered.string()+".json",*metadata);
 Write(root/"mutation.mm","int mutation_control(void){return 42;}");
 auto bad=request(root/"mutation.mm",root/"altered.o");auto altered_dependency=dependency;altered_dependency.source_path=altered.string();bad.imported_interfaces={altered_dependency};auto altered_result=Compile(bad);assert(!altered_result.success);
 assert(std::any_of(altered_result.interface_import_diagnostics.begin(),altered_result.interface_import_diagnostics.end(),[](const auto &d){return d.code=="O3IFC139";}));
 const auto main=root/(c?"main.c":"main.cpp");Write(main,std::string(c?"":"extern \"C\" ")+"int check_a(void);"+(c?"":"extern \"C\" ")+"int check_b(void);int main(void){return check_a()||check_b();}");
 auto entry=request(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,ownership!=0 || !c,{});
}
}
int main(){
 test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-function-effects",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(const char *standard:{"c17","c++20","c++23"})for(int ownership:{0,1,2}){
  const auto name=std::string(standard)+"-"+std::to_string(ownership);cases.Run(name,[&]{Run(root/name,standard,ownership);});}
 return cases.Finish();
}

#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "clang_foundation/interface_module_resolver.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>
#include <map>
#include <sstream>

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
constexpr const char *Source=R"policy(

__attribute__((used,noinline,no_builtin("abs","strlen"),minsize,no_split_stack))
inline int policy_builtins(int n){return abs(n)+(int)strlen("policy");}
__attribute__((used,noinline,no_builtin)) inline int policy_all_builtins(int n){return abs(n);}
__attribute__((used,noinline,convergent)) inline int policy_convergent(int n){return policy_external(n);}
__attribute__((used,noinline,noconvergent)) inline int policy_nonconvergent(int n){return policy_external(n);}
__attribute__((used,noinline,speculative_load_hardening)) inline int policy_hardened(const int *p){return *p;}
__attribute__((used,noinline,no_speculative_load_hardening)) inline int policy_unhardened(const int *p){return *p;}
__attribute__((used,noinline,patchable_function_entry(5,2))) inline int policy_patched(int n){return n+1;}
__attribute__((used,noinline,patchable_function_entry(7,3,".objc3.entries"))) inline int policy_section(int n){return n+5;}
__attribute__((used,noinline,zero_call_used_regs("used-gpr"))) inline int policy_cleared(int n){return n+2;}
__attribute__((used,noinline,enforce_tcb_leaf("native-policy"))) inline int policy_leaf(int n){return n+3;}
__attribute__((used,noinline,enforce_tcb("native-policy"))) inline int policy_trusted(int n){return policy_leaf(n);}
// A private body policy must not make this external prototype ill-formed.
__attribute__((no_builtin("abs"),noinline)) int policy_out_of_line(int n){return abs(n);}
#if __cplusplus
[[clang::no_builtin("abs"),gnu::minsize,gnu::noinline,gnu::used]] __attribute__((no_builtin("abs"),minsize,used)) inline int policy_standard(int n){return abs(n);}
namespace PolicyTemplates {
#if __cplusplus >= 202302L
inline int standard_closure(int n){auto fn=[] [[gnu::minsize,clang::no_builtin("abs")]](int value){return abs(value);};return fn(n);}
#endif
inline const auto stored=[](int n) __attribute__((no_builtin("abs"),minsize,noinline)){return abs(n);};
inline int defaults(int n=[]() __attribute__((minsize)){return 42;}()){return n;}
template<class T> struct Box {
 int seed=[]() __attribute__((minsize)){return 42;}();
 __attribute__((no_builtin("abs"),minsize,noinline)) T apply(T n){return (T)abs((int)n);}
};
template<class T> __attribute__((no_builtin("abs"),noinline)) T apply(T n){return (T)abs((int)n);}
inline int local(int n){
 struct Local {__attribute__((no_builtin("abs"),minsize,noinline)) int apply(int value){return abs(value);}};
 return Local{}.apply(n);
}
inline int closure(int n){auto fn=[](auto value) __attribute__((no_builtin("abs"),minsize,noinline)){return abs((int)value);};return fn(n);}
}
#endif
)policy";
std::string Read(const fs::path &path){std::ifstream file(path);assert(file.good());return {std::istreambuf_iterator<char>(file),{}};}
std::string Attributes(const std::string &ir,const std::string &symbol){
 std::size_t at=0;
 while((at=ir.find("define ",at))!=std::string::npos){
  const auto end=ir.find('\n',at);const auto name=ir.find("@"+symbol+"(",at);
  if(name<end)break;++at;
 }
 assert(at!=std::string::npos);const auto end=ir.find('\n',at);const auto hash=ir.find('#',at);assert(hash<end);
 const auto stop=ir.find_first_not_of("0123456789",hash+1);const auto group=ir.substr(hash,stop-hash);
 const auto begin=ir.find("attributes "+group+" = ");assert(begin!=std::string::npos);return ir.substr(begin,ir.find('\n',begin)-begin);
}
void CheckIR(const fs::path &path,const std::map<std::string,std::string> &symbols,bool split_stack=false){
 const auto ir=Read(path);auto attrs=[&](const char *name){return Attributes(ir,symbols.at(name));};
 auto builtin=attrs("policy_builtins");assert(builtin.find("split-stack")==std::string::npos);assert(builtin.find("minsize")!=std::string::npos);
 assert(builtin.find("\"no-builtin-abs\"")!=std::string::npos && builtin.find("\"no-builtin-strlen\"")!=std::string::npos);
 assert(attrs("policy_all_builtins").find("\"no-builtins\"")!=std::string::npos);
 assert(attrs("policy_hardened").find("speculative_load_hardening")!=std::string::npos);
 assert(attrs("policy_unhardened").find("speculative_load_hardening")==std::string::npos);
 auto patched=attrs("policy_patched");assert(patched.find("\"patchable-function-entry\"=\"3\"")!=std::string::npos && patched.find("\"patchable-function-prefix\"=\"2\"")!=std::string::npos);
 auto section=attrs("policy_section");assert(section.find("\"patchable-function-entry-section\"=\".objc3.entries\"")!=std::string::npos);
 assert(attrs("policy_cleared").find("\"zero-call-used-regs\"=\"used-gpr\"")!=std::string::npos);
 assert(attrs("policy_convergent").find("convergent")!=std::string::npos);
 assert(attrs("policy_nonconvergent").find("convergent")==std::string::npos);
 if(split_stack)assert(attrs("policy_convergent").find("split-stack")!=std::string::npos);
}
void Run(const fs::path &root,const char *standard,int ownership,const char *optimization,bool global_policy=false){
 const bool c=std::string(standard)=="c17";fs::create_directories(root/"provider");fs::create_directories(root/"include");
 Write(root/"include/policy_external.h",R"header(
#if __cplusplus
extern "C" {
#endif
int abs(int);
unsigned long strlen(const char *);
int policy_external(int) __attribute__((convergent));
long labs(long) __attribute__((annotate("objc3.spi:policy_internal")));
#if __cplusplus
}
#endif
)header");
 auto request=[&](const fs::path &input,const fs::path &output){
  auto value=Request(input,output,standard,ownership,optimization,{"-I"+(root/"include").string(),"-Werror=tcb-enforcement"});
  if(c)value.language=ownership?BaseLanguage::ObjectiveC:BaseLanguage::C;
  if(global_policy)value.arguments.insert(value.arguments.end(),{"-fsplit-stack","-mspeculative-load-hardening"});
  return value;
 };
 const auto external_source=root/"external.mm";Write(external_source,Read(root/"include/policy_external.h"));
 auto external_request=request(external_source,root/"external.o");external_request.module_name="fixture.PolicyExternal";external_request.package_identity="policy-external-v1";external_request.emit_semantic_interface=true;
 auto external_built=Compile(external_request);Success(external_built);assert(external_built.interface_artifact && external_built.interface_artifact->success);
 const auto external_interface=root/"PolicyExternal.objc3i";Write(external_interface,external_built.interface_artifact->source_bytes);Write(external_interface.string()+".json",external_built.interface_artifact->sidecar_json);
 InterfaceImportInput external;external.source_path=external_interface.string();external.locked_identity=external_built.interface_artifact->identity;
 // Raw canonical-text replay uses the actual portable dependency bytes at
 // the include path named by that artifact, alongside its source directory.
 const auto replay_dependency=root/"provider"/BuildInterfaceIncludePath(external.locked_identity.canonical_name);
 fs::create_directories(replay_dependency.parent_path());Write(replay_dependency,external_built.interface_artifact->source_bytes);
 const auto source=root/"provider/module.mm";
 auto publish=[&](const std::string &text,bool owned=true){
  Write(source,text);auto value=request(source,root/"provider.o");value.imported_interfaces={external};value.module_name="fixture.FunctionPolicies";value.package_identity="function-policies-v1";value.emit_semantic_interface=true;
  value.module_owned_names=owned;if(owned)value.module_source_root=(root/"provider").string();
  value.interface_exports={"policy_builtins","policy_all_builtins","policy_convergent","policy_nonconvergent","policy_hardened","policy_unhardened","policy_patched","policy_section","policy_cleared","policy_leaf","policy_trusted","policy_out_of_line"};
  if(!c){value.interface_exports.push_back("PolicyTemplates");value.interface_exports.push_back("policy_standard");}
  auto result=Compile(value);Success(result);assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;
 };
 const auto artifact=publish(Source),replay=publish(artifact.source_bytes,false);
 assert(artifact.source_bytes==replay.source_bytes && artifact.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
 publish(Source);
 const auto &reader=SupportedSemanticInterfaceCapabilities();auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(decoded);
 std::map<std::string,std::string> symbols;unsigned marked=0;
 for(const auto &d:decoded->declarations){if(d.kind=="Function")symbols[d.qualified_name]=d.native_symbol_name;marked+=HasNativeFunctionPolicyIdentity(d);}
 assert(marked>=10);
 auto ir=request(source,root/"provider.ll");ir.module_name="fixture.FunctionPolicies";ir.package_identity="function-policies-v1";ir.module_owned_names=true;ir.module_source_root=(root/"provider").string();ir.imported_interfaces={external};ir.emit=EmitKind::LLVM;Success(Compile(ir));CheckIR(root/"provider.ll",symbols,global_policy);
 const auto interface=root/"FunctionPolicies.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);assert(fs::remove(source));
 for(const char *name:{"a","b"}){
  const auto input=root/(std::string(name)+".mm");
  const std::string linkage=c?"":"extern \"C\" ";
  Write(input,"static int shadow_step(int n){return n+1;}static int shadow_builtin(void){int(*abs)(int)=shadow_step;return abs(41);}"
      "static __attribute__((enforce_tcb(\"native-policy\"))) int relay(int n){return policy_leaf(n);}"+linkage+"int check_"+name+R"witness((void){int value=37;
 if(policy_builtins(-36)!=42 || policy_all_builtins(-42)!=42)return 1;
 if(policy_convergent(35)!=42 || policy_nonconvergent(35)!=42)return 2;
 if(policy_hardened(&value)!=37 || policy_unhardened(&value)!=37)return 3;
 if(policy_patched(41)!=42 || policy_cleared(40)!=42 || policy_trusted(39)!=42 || relay(39)!=42 || policy_section(37)!=42)return 4;
 if(policy_out_of_line(-42)!=42 || shadow_builtin()!=42)return 5;
#if __cplusplus
#if __cplusplus >= 202302L
 if(PolicyTemplates::standard_closure(-42)!=42)return 7;
#endif
 PolicyTemplates::Box<int> box;
 if(box.seed!=42 || PolicyTemplates::stored(-42)!=42 || PolicyTemplates::defaults()!=42 || policy_standard(-42)!=42 || box.apply(-42)!=42 || PolicyTemplates::apply(-42)!=42 || PolicyTemplates::local(-42)!=42 || PolicyTemplates::closure(-42)!=42)return 6;
#endif
 return 0;})witness");
  auto consumer=request(input,root/(std::string(name)+".o"));InterfaceImportInput dependency;dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;consumer.imported_interfaces={external,dependency};Success(Compile(consumer));
  consumer.emit=EmitKind::LLVM;consumer.output_path=(root/(std::string(name)+".ll")).string();Success(Compile(consumer));
  if(c) {
   // ISO C noinline consumers call the provider; CodeGen need not emit the
   // available inline bodies. Their full typed policy identity was validated
   // on import, and the provider's emitted definitions are checked above.
   const auto text=Read(consumer.output_path);
   for(const char *function:{"policy_builtins","policy_all_builtins","policy_convergent","policy_nonconvergent","policy_hardened","policy_unhardened","policy_patched","policy_section","policy_cleared","policy_trusted"})
    assert(text.find("@"+symbols.at(function)+"(")!=std::string::npos);
  } else CheckIR(consumer.output_path,symbols,global_policy);

 }
 if(c) {
  const auto naked=root/"naked.m";Write(naked,"int naked(void){return (int)strlen(\"policy\");}");auto missing=request(naked,root/"naked.o");auto absent=Compile(missing);assert(!absent.success);
  assert(std::any_of(absent.diagnostics.begin(),absent.diagnostics.end(),[](const auto &d){return d.message.find("undeclared library function")!=std::string::npos;}));
  const auto hidden=root/"hidden.m";Write(hidden,"int hidden(void){return (int)labs(-42);}");auto private_request=request(hidden,root/"hidden.o");private_request.imported_interfaces={external};auto inaccessible=Compile(private_request);assert(!inaccessible.success);
  if(ownership){
   auto directed=[&](const fs::path &input,const fs::path &output){auto value=request(input,output);value.source_directed_imports=true;value.interface_search_paths={root.string()};InterfaceModuleRequest item;item.canonical_name=external.locked_identity.canonical_name;item.locked_identity=external.locked_identity;item.initially_visible=false;value.module_imports={item};return value;};
   const auto before=root/"before.m";Write(before,"int before(void){return abs(-42);}@import fixture.PolicyExternal;");assert(!Compile(directed(before,root/"before.o")).success);
   const auto after=root/"directive.m";Write(after,"@import fixture.PolicyExternal;int check_directive(void){return abs(-42)!=42 || strlen(\"policy\")!=6 || @fixture.PolicyExternal.abs(-42)!=42;}");Success(Compile(directed(after,root/"directive.o")));
  }
 }
 std::vector<llvm::StringRef> older;for(auto cap:reader)if(cap!=kNativeFunctionPolicySemantic)older.push_back(cap);
 auto old=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,older);assert(!old);llvm::consumeError(old.takeError());
 auto missing=*decoded;std::erase(missing.required_semantics,kNativeFunctionPolicySemantic);auto invalid=EncodeSemanticInterfaceSidecar(missing,reader);assert(!invalid);llvm::consumeError(invalid.takeError());
 // Rehashing valid text cannot conceal a changed native policy contract.
 auto tampered=artifact.source_bytes;const std::string before="patchable_function_entry(5, 2)",after="patchable_function_entry(6, 2)";
 unsigned changed=0;for(std::size_t at=0;(at=tampered.find(before,at))!=std::string::npos;at+=after.size()){tampered.replace(at,before.size(),after);++changed;}assert(changed);
 auto forged=*decoded;forged.canonical_source_sha256=ComputeInterfaceSourceSha256(tampered);auto forged_json=EncodeSemanticInterfaceSidecar(forged,reader);assert(forged_json);
 const auto forged_interface=root/"AlteredPolicies.objc3i";Write(forged_interface,tampered);Write(forged_interface.string()+".json",*forged_json);
 const auto forged_source=root/"altered.mm";Write(forged_source,"int altered(void){return policy_patched(1);}");auto altered=request(forged_source,root/"altered.o");
 InterfaceImportInput forged_input;forged_input.source_path=forged_interface.string();forged_input.locked_identity=artifact.identity;altered.imported_interfaces={external,forged_input};assert(!Compile(altered).success);
 const auto bad=root/"bad.mm";Write(bad,"__attribute__((enforce_tcb(\"other-policy\"))) int forbidden(int n){return policy_leaf(n);}");
 auto violation=request(bad,root/"bad.o");InterfaceImportInput dependency;dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;violation.imported_interfaces={external,dependency};auto rejected=Compile(violation);assert(!rejected.success);
 assert(std::any_of(rejected.diagnostics.begin(),rejected.diagnostics.end(),[](const auto &d){return d.message.find("trusted computing base")!=std::string::npos;}));
 if(global_policy)return; // This additional configuration checks matched IR, not execution.
 const auto main=root/"main.cpp";
 const bool directive=c && ownership;
 Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();extern \"C\" int policy_external(int n){return n+7;}"+std::string(directive?"extern \"C\" int check_directive();":"")+"int main(){return check_a() || check_b()"+(directive?" || check_directive();}":";}"));
 auto entry=Request(main,root/"main.o","c++23",0,optimization,{});Success(Compile(entry));std::vector<std::string> libraries;if(directive)libraries.push_back((root/"directive.o").string());LinkAndExecute(root,entry.output_path,ownership!=0,libraries);
}
}
int main(){
 test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-function-policies",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(const char *standard:{"c17","c++20","c++23"})for(int ownership:{0,1,2})for(const char *optimization:{"-O0","-O2"}){
  const auto name=std::string(standard)+"-"+std::to_string(ownership)+optimization;cases.Run(name,[&]{Run(root/name,standard,ownership,optimization);});}
 if(llvm::StringRef(OBJC3_TEST_NATIVE_LINUX_TARGET).starts_with("x86_64"))
  cases.Run("c++23-2-global-IR",[&]{Run(root/"global-ir","c++23",2,"-O2",true);});
 return cases.Finish();
}

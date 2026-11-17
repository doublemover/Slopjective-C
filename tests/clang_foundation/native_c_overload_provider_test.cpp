#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "clang_foundation/interface_module_resolver.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>
#include <set>

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
constexpr const char *Source=R"overloads(
struct Plain {int value;};
int choose(int value) __attribute__((overloadable)){return value+10;}
int choose(double value) __attribute__((overloadable)){return (int)value+20;}
int fallback(int value) __attribute__((overloadable)){return value+40;}
int fallback(struct Plain value){return value.value+50;}
int bounded(int value) __attribute__((overloadable,enable_if(value>=0,"nonnegative"))){return value+60;}
int bounded(int value) __attribute__((overloadable,enable_if(value<0,"negative"))){return -value+70;}
int cross_bound(int value) __attribute__((overloadable,enable_if(value>=0,"cross nonnegative"))){return value+100;}
int ranked(int value) __attribute__((overloadable,enable_if(value>0,"positive"))){return value+80;}
int ranked(int value) __attribute__((overloadable,enable_if(value>0,"positive"),enable_if(value<10,"small"))){return value+90;}
unsigned long measured(const char *const value __attribute__((pass_object_size(0)))) __attribute__((overloadable)){return __builtin_object_size(value,0);}
unsigned long measured(const void *value) __attribute__((overloadable)){(void)value;return 999;}
inline int inline_choose(int value) __attribute__((overloadable)){return choose(value)+1;}
inline int inline_choose(double value) __attribute__((overloadable)){return choose(value)+2;}
int missing(int value) __attribute__((overloadable,unavailable("entry unavailable"))){return value;}
)overloads";
constexpr const char *Extra=R"extra(
int choose(const char *value) __attribute__((overloadable)){return 30+*value-'A';}
int cross_bound(int value) __attribute__((overloadable,enable_if(value<0,"cross negative"))){return -value+110;}
)extra";
constexpr const char *Witness=R"witness(
static int witness(void){
 int (*volatile integer)(int)=choose;int (*volatile floating)(double)=choose;
 char bytes[7]={0};
 if(choose(1)!=11 || choose(2.0)!=22 || choose("A")!=30)return 1;
 if(integer(3)!=13 || floating(4.0)!=24)return 2;
 if(fallback(2)!=42 || fallback((struct Plain){3})!=53)return 3;
 if(bounded(3)!=63 || bounded(-3)!=73)return 4;
 if(cross_bound(3)!=103 || cross_bound(-3)!=113)return 5;
 if(ranked(4)!=94 || ranked(20)!=100)return 6;
 if(measured(bytes)!=7 || measured((const void *)bytes)!=999)return 7;
 if(inline_choose(1)!=12 || inline_choose(2.0)!=24)return 8;
 return 0;
}
)witness";

void Run(const fs::path &root,int ownership,const char *optimization){
 fs::create_directories(root/"provider");
 auto request=[&](const fs::path &input,const fs::path &output,int mode){
  CompileRequest value;value.input_path=input.string();value.output_path=output.string();value.emit=EmitKind::Object;
  value.language=mode?BaseLanguage::ObjectiveC:BaseLanguage::C;value.objc3=mode!=0;value.arc=mode==2;
  if(mode)value.objc_runtime="gnustep-2.2";value.target_triple=OBJC3_TEST_NATIVE_LINUX_TARGET;
  value.arguments={"-std=c17",optimization,"-resource-dir=" OBJC3_STANDARD_LIBRARY_TEST_RESOURCE_DIR};
  auto flags=Instrumentation();value.arguments.insert(value.arguments.end(),flags.begin(),flags.end());return value;
 };
 auto emit=[&](const std::string &text,const char *module,const char *object,bool owned=true){
  const auto source=root/"provider/module.m";Write(source,text);auto value=request(source,root/object,ownership);
  value.module_name=module;value.package_identity="c-overload-v1";value.emit_semantic_interface=true;value.module_owned_names=owned;
  if(owned)value.module_source_root=(root/"provider").string();auto result=Compile(value);Success(result);
  assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;
 };
 const auto artifact=emit(Source,"fixture.COverloads","provider.o");
 const auto replay=emit(artifact.source_bytes,"fixture.COverloads","replay.o",false);
 assert(artifact.source_bytes==replay.source_bytes && artifact.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
 const auto extra=emit(Extra,"fixture.CExtra","extra.o");
 const auto &reader=SupportedSemanticInterfaceCapabilities();auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(decoded);
 assert(std::find(decoded->required_semantics.begin(),decoded->required_semantics.end(),kCOverloadableSemantic)!=decoded->required_semantics.end());
 std::set<std::string> identities,symbols;unsigned marked=0;
 for(const auto &d:decoded->declarations)if(d.kind=="Function"){
  assert(identities.insert(d.stable_id).second && symbols.insert(d.native_symbol_name).second);
  marked+=HasCOverloadableIdentity(d);
 }
 assert(marked==13);
 assert(fs::remove(root/"provider/module.m"));
 auto persist=[&](const InterfaceEmissionArtifact &value,const char *file){
  const auto path=root/file;Write(path,value.source_bytes);Write(path.string()+".json",value.sidecar_json);
  InterfaceImportInput input;input.source_path=path.string();input.locked_identity=value.identity;return input;
 };
 const auto first=persist(artifact,"COverloads.objc3i"),second=persist(extra,"CExtra.objc3i");
 for(const char *name:{"a","b"}){
  auto source=root/(std::string(name)+".m");Write(source,std::string(Witness)+"int check_"+name+"(void){return witness();}");
  auto value=request(source,root/(std::string(name)+".o"),ownership);value.imported_interfaces={first,second};Success(Compile(value));
 }
 unsigned ordinal=0;
 for(const auto &[text,message]:std::vector<std::pair<std::string,std::string>>{
     {"int bad(int value){return bounded(value);}","no matching function"},
     {"int bad(void){return choose((struct Plain){1});}","no matching function"},
     {"int bad(void){return missing(1);}","entry unavailable"}}){
  auto source=root/("bad-"+std::to_string(ordinal++)+".m");Write(source,text);auto value=request(source,root/"bad.o",ownership);value.imported_interfaces={first,second};
  auto result=Compile(value);const bool rejected=!result.success && std::any_of(result.diagnostics.begin(),result.diagnostics.end(),[&](const auto &d){return d.message.find(message)!=std::string::npos;});
  if(!rejected)test::ReportFailure(result);assert(rejected);
 }
 if(ownership){
  const auto source=root/"directive.m";Write(source,"@import fixture.COverloads;\n@import fixture.CExtra;\n"+std::string(Witness)+
      "int qualified(void){int(*one)(int)=@fixture.COverloads.choose;return one(1)!=11 || @fixture.COverloads.choose(1)!=11 || @fixture.CExtra.choose(\"A\")!=30 || @fixture.COverloads.ranked(4)!=94;}\nint check_directive(void){return witness() || qualified();}");
  auto value=request(source,root/"directive.o",ownership);value.source_directed_imports=true;value.interface_search_paths={root.string()};
  for(const auto *identity:{&artifact.identity,&extra.identity}){InterfaceModuleRequest item;item.canonical_name=identity->canonical_name;item.locked_identity=*identity;item.initially_visible=false;value.module_imports.push_back(item);}
  Success(Compile(value));
 }
 // A marked candidate must not make unrelated incompatible ordinary C entries valid.
 const auto plain_a=emit("int ordinary_conflict(int);","fixture.PlainA","plain-a.o");
 const auto plain_b=emit("double ordinary_conflict(double);","fixture.PlainB","plain-b.o");
 const auto conflict_source=root/"conflict.m";Write(conflict_source,"int unused(void){return 0;}");
 auto conflict=request(conflict_source,root/"conflict.o",ownership);conflict.imported_interfaces={first,persist(plain_a,"PlainA.objc3i"),persist(plain_b,"PlainB.objc3i")};
 const auto rejected=Compile(conflict);assert(!rejected.success && std::any_of(rejected.interface_import_diagnostics.begin(),rejected.interface_import_diagnostics.end(),[](const auto &d){return d.code=="O3IFC132";}));
 std::vector<llvm::StringRef> older;for(auto cap:reader)if(cap!=kCOverloadableSemantic)older.push_back(cap);
 auto old=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,older);assert(!old);llvm::consumeError(old.takeError());
 auto missing=*decoded;std::erase(missing.required_semantics,kCOverloadableSemantic);auto invalid=EncodeSemanticInterfaceSidecar(missing,reader);assert(!invalid);llvm::consumeError(invalid.takeError());
 const auto main=root/"main.c";
 Write(main,"int check_a(void);int check_b(void);"+std::string(ownership?"int check_directive(void);":"")+
     "int main(void){return check_a() || check_b()"+(ownership?" || check_directive();}":";}") );
 auto entry=request(main,root/"main.o",0);Success(Compile(entry));
 std::vector<std::string> objects={(root/"extra.o").string()};if(ownership)objects.push_back((root/"directive.o").string());
 LinkAndExecute(root,entry.output_path,ownership!=0,objects);
}
}
int main(){
 test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-c-overloads",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(int ownership:{0,1,2})for(const char *optimization:{"-O0","-O2"}){
  const auto name=std::string(ownership?"objc":"c")+"-"+std::to_string(ownership)+optimization;
  cases.Run(name,[&]{Run(root/name,ownership,optimization);});}
 return cases.Finish();
}

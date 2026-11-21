#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>
using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
template<class T>bool Fails(llvm::Expected<T> value){if(value)return false;llvm::consumeError(value.takeError());return true;}
constexpr const char *Source=R"availability(
__attribute__((always_inline)) inline int availability_builtin(int *visits){
 if(__builtin_available(macOS 1000.2.3,iOS 1001.2,*)){++*visits;return 42;}
 --*visits;return -1;
}
#ifdef __OBJC__
__attribute__((always_inline)) inline int availability_objective(int *visits){
 if(@available(macOS 1000.2.3,iOS 1001.2,*)){++*visits;return 42;}
 --*visits;return -1;
}
#endif
#ifdef __cplusplus
template<class T>__attribute__((always_inline)) inline T availability_template(T value){
 if(__builtin_available(macOS 1000.2.3,*))return value+1;
 return T{};
}
__attribute__((always_inline)) inline int availability_default(int value=__builtin_available(*)?42:-1){return value;}
#endif
)availability";
constexpr const char *Consumer=R"availability(
#include <assert.h>
static int execute(void){
 int visits=0;assert(availability_builtin(&visits)==42 && visits==1);
#ifdef __OBJC__
 assert(availability_objective(&visits)==42 && visits==2);
#endif
#ifdef __cplusplus
 assert(availability_template(41)==42 && availability_template(41L)==42L);
 assert(availability_default()==42 && availability_default(43)==43);
#endif
 return 0;
}
)availability";
void Run(const fs::path &root,const char *standard,int ownership){
 fs::create_directories(root/"provider");
 auto request=[&](const fs::path &input,const fs::path &output){auto value=Request(input,output,standard,ownership,"-O2",{});
  if(std::string(standard)=="c17")value.language=ownership?BaseLanguage::ObjectiveC:BaseLanguage::C;return value;};
 const auto source=root/"provider/module.mm";
 auto publish=[&](const std::string &text,bool owned=true){Write(source,text);auto value=request(source,root/"provider.o");value.module_name="fixture.Availability";value.package_identity="availability-v2";
  value.module_owned_names=owned;if(owned)value.module_source_root=(root/"provider").string();value.emit_semantic_interface=true;
  auto result=Compile(value);Success(result);assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;};
 const auto artifact=publish(Source),replay=publish(artifact.source_bytes,false);
 assert(artifact.source_bytes==replay.source_bytes && artifact.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
 assert(artifact.source_bytes.find("__builtin_available(*)")!=std::string::npos && artifact.source_bytes.find("1000.2.3")==std::string::npos && artifact.source_bytes.find("available(...)")==std::string::npos);
 const auto &reader=SupportedSemanticInterfaceCapabilities();auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(decoded);
 assert(std::find(decoded->required_semantics.begin(),decoded->required_semantics.end(),kNativeObjectExpressionSemantic)!=decoded->required_semantics.end());
 auto older=std::vector<llvm::StringRef>(reader.begin(),reader.end());std::erase(older,kNativeObjectExpressionSemantic);older.push_back("objc3.native-object-expressions.v1");assert(Fails(DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,older)));
 auto missing=*decoded;std::erase(missing.required_semantics,kNativeObjectExpressionSemantic);assert(Fails(EncodeSemanticInterfaceSidecar(missing,reader)));
 const auto hidden_source=root/"hidden.mm";Write(hidden_source,"int hidden_availability(void){if(__builtin_available(*))return 42;return -1;}");
 auto hidden=request(hidden_source,root/"hidden.o");hidden.module_name="fixture.HiddenAvailability";hidden.package_identity="hidden-availability-v2";hidden.emit_semantic_interface=true;
 auto hidden_result=Compile(hidden);Success(hidden_result);assert(hidden_result.interface_artifact && hidden_result.interface_artifact->success);
 auto hidden_metadata=DecodeSemanticInterfaceSidecar(hidden_result.interface_artifact->sidecar_json,hidden_result.interface_artifact->source_bytes,reader);assert(hidden_metadata);
 assert(std::find(hidden_metadata->required_semantics.begin(),hidden_metadata->required_semantics.end(),kNativeObjectExpressionSemantic)==hidden_metadata->required_semantics.end());
 publish(Source);assert(fs::remove(source));const auto interface=root/"Availability.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 InterfaceImportInput dependency;dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;
 for(const char *name:{"a","b"}){const auto input=root/(std::string(name)+".mm");auto text=std::string(Consumer);
  if(std::string(standard)!="c17")text+="extern \"C\" ";text+="int check_"+std::string(name)+"(void){return execute();}\n";
  Write(input,text);auto value=request(input,root/(std::string(name)+".o"));value.imported_interfaces={dependency};Success(Compile(value));}
 // A valid source rehash cannot conceal a changed branch result.
 const auto changed=EditCanonicalInterface(artifact.source_bytes,"return 42;","return 43;");
 auto mutation=*decoded;mutation.canonical_source_sha256=ComputeInterfaceSourceSha256(changed);auto metadata=EncodeSemanticInterfaceSidecar(mutation,reader);assert(metadata);
 const auto altered=root/"Altered.objc3i";Write(altered,changed);Write(altered.string()+".json",*metadata);Write(root/"mutation.mm","int mutation_control(void){return 0;}");
 auto bad=request(root/"mutation.mm",root/"altered.o");auto altered_dependency=dependency;altered_dependency.source_path=altered.string();bad.imported_interfaces={altered_dependency};auto rejected=Compile(bad);assert(!rejected.success);
 RejectedImport(rejected,"O3IFC139");
 Write(root/"main.mm",std::string(std::string(standard)=="c17"?"":"extern \"C\" ")+"int check_a(void);\n"+(std::string(standard)=="c17"?"":"extern \"C\" ")+"int check_b(void);int main(void){return check_a()||check_b();}\n");
 auto main=request(root/"main.mm",root/"main.o");Success(Compile(main));LinkAndExecute(root,main.output_path,ownership!=0,{});
 if(ownership){
  assert(fs::remove(altered));assert(fs::remove(altered.string()+".json"));
  InterfaceModuleRequest item;item.canonical_name=artifact.identity.canonical_name;item.locked_identity=artifact.identity;item.initially_visible=false;
  for(const char *name:{"a","b"}){const auto input=root/(std::string("directed-")+name+".mm");auto text=std::string("@import fixture.Availability;\n")+Consumer;
   if(std::string(standard)!="c17")text+="extern \"C\" ";text+="int check_"+std::string(name)+"(void){return execute();}\n";
   Write(input,text);auto value=request(input,root/(std::string(name)+".o"));value.source_directed_imports=true;value.interface_search_paths={root.string()};value.module_imports={item};Success(Compile(value));}
  LinkAndExecute(root,main.output_path,true,{});
 }
}
}
int main(){test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-availability",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(const char *standard:{"c17","c++20","c++23"})for(int ownership:{0,1,2}){const auto name=std::string(standard)+"-"+std::to_string(ownership);cases.Run(name,[&]{Run(root/name,standard,ownership);});}return cases.Finish();}

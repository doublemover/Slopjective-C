#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>
using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
template<class T> bool Fails(llvm::Expected<T> result){if(result)return false;llvm::consumeError(result.takeError());return true;}
constexpr const char *Source=R"lifetime(
struct LifetimeView;
struct [[gsl::Owner(int)]] LifetimeOwner {int value;void bind(LifetimeView &target) const [[clang::lifetime_capture_by(target)]];};
struct [[gsl::Pointer(int)]] LifetimeView {const int *pointer;};
void LifetimeOwner::bind(LifetimeView &renamed) const {renamed.pointer=&value;}
struct [[gsl::Owner]] LifetimeErasedOwner {int value;};
struct [[gsl::Pointer]] LifetimeErasedView {const int *pointer;};
void lifetime_capture(const LifetimeOwner &source [[clang::lifetime_capture_by(destination)]],LifetimeView &destination);
void lifetime_capture(const LifetimeOwner &renamed,LifetimeView &result){result.pointer=&renamed.value;}
const int *lifetime_global_pointer;
void lifetime_global(const LifetimeOwner &source [[clang::lifetime_capture_by_global]]){lifetime_global_pointer=&source.value;}
void lifetime_unknown(const LifetimeOwner &source [[clang::lifetime_capture_by_unknown]]){(void)source;}
void lifetime_shadow_global(const LifetimeOwner &source [[clang::lifetime_capture_by_global]],int global){(void)source;(void)global;}
void lifetime_shadow_unknown(const LifetimeOwner &source [[clang::lifetime_capture_by_unknown]],int unknown){(void)source;(void)unknown;}
void lifetime_first(LifetimeView &target,const LifetimeOwner &source [[clang::lifetime_capture_by(target)]]){target.pointer=&source.value;}
void lifetime_clear(){lifetime_global_pointer=nullptr;}
void lifetime_unnamed(const LifetimeOwner &source [[clang::lifetime_capture_by(destination)]],LifetimeView &destination);
void lifetime_unnamed(const LifetimeOwner &,LifetimeView &){}
struct [[gsl::Pointer(int)]] LifetimeHolder {
 const int *pointer;
 void put(const LifetimeOwner &source [[clang::lifetime_capture_by_this]]){pointer=&source.value;}
 static void put_static(const LifetimeOwner &source [[clang::lifetime_capture_by(destination)]],LifetimeView &destination){destination.pointer=&source.value;}
};
namespace LifetimeTemplates {
 template<class T>struct [[gsl::Owner(T)]] Box{T value;};
 template<class T>struct [[gsl::Pointer(T)]] View{const T *pointer;};
 template<class T>void capture(const Box<T> &source [[clang::lifetime_capture_by(destination)]],View<T> &destination);
 template<class T>void capture(const Box<T> &renamed,View<T> &result){result.pointer=&renamed.value;}
 template<class T>void unnamed(const Box<T> &source [[clang::lifetime_capture_by(destination)]],View<T> &destination);
 template<class T>void unnamed(const Box<T> &,View<T> &){}
 inline int local(int value){struct [[gsl::Owner(int)]] Local{int value;};return Local{value}.value;}
 inline auto closure(){return [](const LifetimeOwner &source [[clang::lifetime_capture_by(destination)]],LifetimeView &destination){destination.pointer=&source.value;};}
 inline auto generic_closure(){return [](const auto &source [[clang::lifetime_capture_by(destination)]],auto &destination){destination.pointer=&source.value;};}
#if __cplusplus >= 202302L
 inline auto static_closure(){return [](const LifetimeOwner &source [[clang::lifetime_capture_by(destination)]],LifetimeView &destination) static {destination.pointer=&source.value;};}
 inline auto explicit_closure(){return [](this const auto &,const LifetimeOwner &source [[clang::lifetime_capture_by(destination)]],LifetimeView &destination){destination.pointer=&source.value;};}
#endif
}
)lifetime";
void Run(const fs::path &root,const char *standard,int ownership){
 fs::create_directories(root/"provider");
 auto request=[&](const fs::path &input,const fs::path &output){return Request(input,output,standard,ownership,"-O2",{"-Werror=dangling-capture"});};
 const auto source=root/"provider/module.mm";
 auto publish=[&](const std::string &text,bool owned=true){
  Write(source,text);auto value=request(source,root/"provider.o");value.module_name="fixture.LifetimeAnnotations";value.package_identity="lifetime-annotations-v1";
  value.module_owned_names=owned;if(owned)value.module_source_root=(root/"provider").string();value.emit_semantic_interface=true;
  auto result=Compile(value);Success(result);assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;
 };
 const auto artifact=publish(Source),replay=publish(artifact.source_bytes,false);
 assert(artifact.source_bytes==replay.source_bytes && artifact.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
 const auto &reader=SupportedSemanticInterfaceCapabilities();auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(decoded);
 assert(std::find(decoded->required_semantics.begin(),decoded->required_semantics.end(),kNativeLifetimeAnnotationSemantic)!=decoded->required_semantics.end());
 std::vector<llvm::StringRef> old(reader.begin(),reader.end());std::erase(old,kNativeLifetimeAnnotationSemantic);assert(Fails(DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,old)));
 auto missing=*decoded;std::erase(missing.required_semantics,kNativeLifetimeAnnotationSemantic);assert(Fails(EncodeSemanticInterfaceSidecar(missing,reader)));
 for(const char *kind:{"Namespace","Enum","EnumConstant"}) {
  auto forged=*decoded;auto owner=forged.declarations.front();owner.kind=kind;owner.stable_id="forged-lifetime-owner";owner.qualified_name=owner.export_root="forged_lifetime_owner";
  owner.canonical_type_identity="forged;native-lifetime-annotations:v1;";forged.declarations.push_back(owner);assert(Fails(EncodeSemanticInterfaceSidecar(forged,reader)));
 }
 publish(Source);assert(fs::remove(source));
 const auto interface=root/"LifetimeAnnotations.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 InterfaceImportInput dependency;dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;
 for(const char *name:{"a","b"}) {
  const auto input=root/(std::string(name)+".mm");Write(input,"extern \"C\" int check_"+std::string(name)+R"consumer((){
 LifetimeOwner owner{42};LifetimeView view{nullptr};lifetime_capture(owner,view);if(!view.pointer || *view.pointer!=42)return 1;
 lifetime_global(owner);if(!lifetime_global_pointer || *lifetime_global_pointer!=42)return 2;lifetime_clear();lifetime_unknown(owner);lifetime_unnamed(owner,view);
 LifetimeHolder holder{};holder.put(owner);if(*holder.pointer!=42)return 3;LifetimeHolder::put_static(owner,view);if(*view.pointer!=42)return 4;
 LifetimeTemplates::Box<int> box{42};LifetimeTemplates::View<int> reference{};LifetimeTemplates::capture(box,reference);if(*reference.pointer!=42)return 5;
 LifetimeTemplates::unnamed(box,reference);owner.bind(view);if(*view.pointer!=42)return 7;
 lifetime_first(view,owner);lifetime_shadow_global(owner,0);lifetime_shadow_unknown(owner,0);
 auto closure=LifetimeTemplates::closure();closure(owner,view);if(*view.pointer!=42 || LifetimeTemplates::local(42)!=42)return 6;
 LifetimeTemplates::generic_closure()(owner,view);if(*view.pointer!=42)return 8;
#if __cplusplus >= 202302L
 LifetimeTemplates::static_closure()(owner,view);LifetimeTemplates::explicit_closure()(owner,view);if(*view.pointer!=42)return 9;
#endif
 return 0;
})consumer");
  auto value=request(input,root/(std::string(name)+".o"));value.imported_interfaces={dependency};Success(Compile(value));
 }
 for(const char *body:{
     "void rejected(LifetimeView &view){lifetime_capture(LifetimeOwner{42},view);}",
     "void rejected(){lifetime_global(LifetimeOwner{42});}",
     "void rejected(){lifetime_unknown(LifetimeOwner{42});}",
     "void rejected(){lifetime_shadow_global(LifetimeOwner{42},0);}",
     "void rejected(){lifetime_shadow_unknown(LifetimeOwner{42},0);}",
     "void rejected(LifetimeView &view){LifetimeOwner{42}.bind(view);}",
     "void rejected(LifetimeView &view){lifetime_first(view,LifetimeOwner{42});}",
     "void rejected(LifetimeView &view){auto closure=LifetimeTemplates::closure();closure(LifetimeOwner{42},view);}",
     "void rejected(LifetimeView &view){LifetimeTemplates::generic_closure()(LifetimeOwner{42},view);}",
     "#if __cplusplus >= 202302L\nvoid rejected(LifetimeView &view){LifetimeTemplates::static_closure()(LifetimeOwner{42},view); }\n#else\nvoid rejected(LifetimeView &view){lifetime_capture(LifetimeOwner{42},view); }\n#endif",
     "#if __cplusplus >= 202302L\nvoid rejected(LifetimeView &view){LifetimeTemplates::explicit_closure()(LifetimeOwner{42},view); }\n#else\nvoid rejected(LifetimeView &view){lifetime_capture(LifetimeOwner{42},view); }\n#endif",
     "void rejected(LifetimeTemplates::View<int> &view){LifetimeTemplates::unnamed(LifetimeTemplates::Box<int>{42},view);}",
     "void rejected(LifetimeHolder &holder){holder.put(LifetimeOwner{42});}",
     "void rejected(LifetimeView &view){LifetimeHolder::put_static(LifetimeOwner{42},view);}",
     "void rejected(LifetimeTemplates::View<int> &view){LifetimeTemplates::capture(LifetimeTemplates::Box<int>{42},view);}"}) {
  const auto input=root/"bad.mm";Write(input,body);auto value=request(input,root/"bad.o");value.imported_interfaces={dependency};auto result=Compile(value);if(result.success)std::cerr<<"Expected dangling-capture rejection for: "<<body<<"\n";assert(!result.success);
  assert(std::any_of(result.diagnostics.begin(),result.diagnostics.end(),[](const auto &d){return d.message.find("captured")!=std::string::npos && d.message.find("full-expression")!=std::string::npos;}));
 }
 const auto erased=root/"erased.mm";
 Write(erased,"struct ErasedOwner{int value;};struct ErasedView{const int *pointer;};using ErasedCapture=void(*)(const ErasedOwner &source [[clang::lifetime_capture_by(target)]],ErasedView &target);");
 auto erased_request=request(erased,root/"erased.o");erased_request.module_name="fixture.ErasedCapture";erased_request.package_identity="erased-capture-v1";erased_request.emit_semantic_interface=true;
 auto erased_result=Compile(erased_request);assert(erased_result.interface_artifact && !erased_result.interface_artifact->success);
 assert(std::any_of(erased_result.interface_artifact->diagnostics.begin(),erased_result.interface_artifact->diagnostics.end(),[](const auto &d){return d.code=="O3IFC112";}));
 const auto local_only=root/"lambda-only.mm";
 Write(local_only,"inline auto lifetime_only(){return [](const int &source [[clang::lifetime_capture_by_global]]){(void)source;};}");
 auto local_request=request(local_only,root/"lambda-only.o");local_request.module_name="fixture.LifetimeOnly";local_request.package_identity="lifetime-only-v1";local_request.emit_semantic_interface=true;
 auto local_result=Compile(local_request);Success(local_result);assert(local_result.interface_artifact && local_result.interface_artifact->success);
 auto local_metadata=DecodeSemanticInterfaceSidecar(local_result.interface_artifact->sidecar_json,local_result.interface_artifact->source_bytes,reader);assert(local_metadata);
 assert(std::find(local_metadata->required_semantics.begin(),local_metadata->required_semantics.end(),kNativeLifetimeAnnotationSemantic)!=local_metadata->required_semantics.end());
 const auto hidden=root/"hidden.mm";Write(hidden,"struct HiddenLifetime{};struct [[gsl::Pointer(HiddenLifetime)]] PublicView{HiddenLifetime *pointer;};");
 auto hidden_request=request(hidden,root/"hidden.o");hidden_request.module_name="fixture.HiddenLifetime";hidden_request.package_identity="hidden-lifetime-v1";hidden_request.emit_semantic_interface=true;hidden_request.interface_exports={"PublicView"};
 auto excluded=Compile(hidden_request);assert(excluded.interface_artifact && !excluded.interface_artifact->success);
 assert(std::any_of(excluded.interface_artifact->diagnostics.begin(),excluded.interface_artifact->diagnostics.end(),[](const auto &d){return d.code=="O3IFC156" || d.code=="O3IFC112";}));
 // A well-formed changed capture promise must fail at the semantic identity gate.
 const auto changed=EditCanonicalInterface(artifact.source_bytes,"lifetime_capture_by_global","lifetime_capture_by_unknown");
 auto mutation=*decoded;mutation.canonical_source_sha256=ComputeInterfaceSourceSha256(changed);auto metadata=EncodeSemanticInterfaceSidecar(mutation,reader);assert(metadata);
 const auto altered=root/"Altered.objc3i";Write(altered,changed);Write(altered.string()+".json",*metadata);Write(root/"mutation.mm","int mutation_control(){return 42;}");
 auto bad=request(root/"mutation.mm",root/"altered.o");auto altered_dependency=dependency;altered_dependency.source_path=altered.string();bad.imported_interfaces={altered_dependency};auto altered_result=Compile(bad);assert(!altered_result.success);
 RejectedImport(altered_result,"O3IFC139");
 const auto main=root/"main.cpp";Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();int main(){return check_a()||check_b();}");
 auto entry=request(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,ownership!=0,{});
}
}
int main(){
 test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-lifetime-annotations",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(const char *standard:{"c++20","c++23"})for(int ownership:{0,1,2}){
  const auto name=std::string(standard)+"-"+std::to_string(ownership);cases.Run(name,[&]{Run(root/name,standard,ownership);});}
 return cases.Finish();
}

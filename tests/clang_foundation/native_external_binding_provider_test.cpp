#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>
using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
template<class T>bool Fails(llvm::Expected<T> value){if(value)return false;llvm::consumeError(value.takeError());return true;}
constexpr const char *Source=R"external(
#ifdef __cplusplus
extern "C" {
#endif
int external_first(int) __asm__("ffi_first");
int external_second(int) __asm__("ffi_second");
int external_dotted(int) __asm__("ffi.dotted");
extern int external_counter __asm__("ffi_counter");
extern int (*external_callback)(int) __asm__("ffi_callback");
#ifdef __cplusplus
}
#endif
inline int external_apply(int value){++external_counter;return external_callback(value)+external_second(0);}
#ifdef __OBJC__
__attribute__((objc_runtime_name("FFIExplicitProtocol")))
@protocol ExternalProtocol
- (int)value;
@end
__attribute__((objc_root_class,objc_runtime_name("FFIExplicitObject")))
@interface ExternalObject<ExternalProtocol> {Class isa;}
+ (id)new;
- (int)value;
@end
#ifdef __cplusplus
extern "C" {
#endif
void external_release(void *) __asm__("objc_release");
extern int external_destroyed __asm__("ffi_destroyed");
#ifdef __cplusplus
}
#endif
__attribute__((always_inline)) inline int external_object_value(void){
 ExternalObject *object=[ExternalObject new];int result=[object value];
#if !__has_feature(objc_arc)
 external_release((__bridge void *)object);
#endif
 return result;
}
#endif
#ifdef __cplusplus
struct ExternalBinding {
 int value;
 static int direct(int) __asm__("ffi_method");
 int method(int) const __asm__("ffi_instance");
};
namespace ExternalNamespace {inline int apply(int value){return external_apply(value);}}
#endif
)external";
constexpr const char *Consumer=R"external(
#include <assert.h>
#ifdef __OBJC__
#ifdef __cplusplus
extern "C"
#endif
void *objc_getProtocol(const char *);
#endif
static int execute(void){
 external_counter=0;
 assert(external_apply(40)==42 && external_counter==1);
 assert(owned_external_apply(40)==42 && owned_external_counter()==&external_counter);
 assert(external_first(41)==42 && external_dotted(41)==42 && external_callback==&external_first);
#ifdef __OBJC__
 external_destroyed=0;assert(external_object_value()==42 && external_destroyed==1);
 assert((__bridge void *)@protocol(ExternalProtocol)==objc_getProtocol("FFIExplicitProtocol"));
#endif
#ifdef __cplusplus
 ExternalBinding value{40};assert(value.method(2)==42 && ExternalBinding::direct(41)==42);
 assert(ExternalNamespace::apply(40)==42 && external_counter==2);
#endif
 return 0;
}
)external";
constexpr const char *Main=R"external(
#include <assert.h>
#ifdef __cplusplus
extern "C" {
#endif
int ffi_counter=0;
#ifdef __OBJC__
int ffi_destroyed=0;
id class_createInstance(Class,__SIZE_TYPE__) __attribute__((ns_returns_retained));
id object_dispose(__unsafe_unretained id);
#endif
int ffi_first(int value){return value+1;}
int ffi_second(int value){return value+1;}
int actual_dotted(int) __asm__("ffi.dotted");
int actual_dotted(int value){return value+1;}
int (*ffi_callback)(int)=ffi_first;
int ffi_method(int value){return value+1;}
int ffi_instance(const void *object,int value){return *(const int *)object+value;}
int check_a(void);int check_b(void);void *address_a(void);void *address_b(void);
#ifdef __cplusplus
}
#endif
#ifdef __OBJC__
@implementation ExternalObject
+ (id)new{return class_createInstance(self,0);}
- (int)value{return 42;}
- (void)dealloc{++ffi_destroyed;object_dispose(self);}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease{}
#endif
@end
#endif
int main(void){assert(address_a()==&ffi_counter && address_b()==&ffi_counter);return check_a()||check_b();}
)external";
void Run(const fs::path &root,const char *standard,int ownership){
 fs::create_directories(root/"provider");
 auto request=[&](const fs::path &input,const fs::path &output){auto value=Request(input,output,standard,ownership,"-O2",{});
  if(std::string(standard)=="c17")value.language=ownership?BaseLanguage::ObjectiveC:BaseLanguage::C;return value;};
 const auto source=root/"provider/module.mm";
 auto publish=[&](const std::string &text){Write(source,text);auto value=request(source,root/"provider.o");value.module_name="fixture.ExternalBindings";value.package_identity="external-bindings-v1";value.emit_semantic_interface=true;
  auto result=Compile(value);Success(result);assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;};
 const auto artifact=publish(Source),replay=publish(artifact.source_bytes);
 assert(artifact.source_bytes==replay.source_bytes && artifact.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
 const auto &reader=SupportedSemanticInterfaceCapabilities();auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(decoded);
 assert(std::find(decoded->required_semantics.begin(),decoded->required_semantics.end(),kNativeExternalBindingSemantic)!=decoded->required_semantics.end());
 std::vector<llvm::StringRef> old(reader.begin(),reader.end());std::erase(old,kNativeExternalBindingSemantic);assert(Fails(DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,old)));
 auto missing=*decoded;std::erase(missing.required_semantics,kNativeExternalBindingSemantic);assert(Fails(EncodeSemanticInterfaceSidecar(missing,reader)));
 for(const char *kind:{"Namespace","Enum","EnumConstant"}){
  auto forged=*decoded;auto row=std::find_if(forged.declarations.begin(),forged.declarations.end(),HasNativeExternalBindingIdentity);assert(row!=forged.declarations.end());row->kind=kind;
  assert(Fails(EncodeSemanticInterfaceSidecar(forged,reader)));
 }
 for(unsigned shape=0;shape!=5;++shape){
  auto forged=*decoded;auto entry=std::find_if(forged.declarations.begin(),forged.declarations.end(),HasNativeExternalBindingIdentity);assert(entry!=forged.declarations.end());
  if(shape==0)entry->canonical_type_identity=NativeExternalBindingIdentity(entry->native_symbol_name)+entry->canonical_type_identity;
  if(shape==1)entry->canonical_type_identity=NativeExternalBindingIdentity(entry->native_symbol_name);
  if(shape==2)entry->canonical_type_identity=std::string(kNativeExternalBindingMarker)+"999:"+entry->native_symbol_name+";int";
  if(shape==3){entry->native_symbol_name="bad\nsymbol";entry->canonical_type_identity=NativeExternalBindingIdentity(entry->native_symbol_name)+"int";}
  if(shape==4){entry->native_symbol_name="__objc3_m_other_owner";entry->canonical_type_identity=NativeExternalBindingIdentity(entry->native_symbol_name)+"int";}
  assert(Fails(EncodeSemanticInterfaceSidecar(forged,reader)));
 }
 auto reserved=*decoded;auto row=std::find_if(reserved.declarations.begin(),reserved.declarations.end(),HasNativeExternalBindingIdentity);assert(row!=reserved.declarations.end());
 row->native_symbol_name="__objc3_m_forged_other_owner";assert(Fails(EncodeSemanticInterfaceSidecar(reserved,reader)));
 publish(Source);assert(fs::remove(source));
 const auto interface=root/"ExternalBindings.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 InterfaceImportInput dependency;dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;
 fs::create_directories(root/"adapter");
 const auto adapter_source=root/"adapter/module.mm";
 Write(adapter_source,"__attribute__((always_inline)) inline int owned_external_apply(int value){return external_callback(value)+external_second(0); }\n__attribute__((always_inline)) inline int *owned_external_counter(void){return &external_counter;}\n");
 auto adapter_request=request(adapter_source,root/"adapter.o");adapter_request.imported_interfaces={dependency};adapter_request.module_name="fixture.ExternalAdapter";adapter_request.package_identity="external-adapter-v1";
 adapter_request.module_owned_names=true;adapter_request.module_source_root=(root/"adapter").string();adapter_request.emit_semantic_interface=true;
 auto adapter_built=Compile(adapter_request);Success(adapter_built);assert(adapter_built.interface_artifact && adapter_built.interface_artifact->success);assert(fs::remove(adapter_source));
 const auto adapter_path=root/"ExternalAdapter.objc3i";Write(adapter_path,adapter_built.interface_artifact->source_bytes);Write(adapter_path.string()+".json",adapter_built.interface_artifact->sidecar_json);
 InterfaceImportInput adapter;adapter.source_path=adapter_path.string();adapter.locked_identity=adapter_built.interface_artifact->identity;
 for(const char *name:{"a","b"}){
  const auto input=root/(std::string(name)+".mm");std::string text=Consumer;
  if(std::string(standard)!="c17")text+="extern \"C\" {\n";
  text+="int check_"+std::string(name)+"(void){return execute();}void *address_"+name+"(void){return &external_counter;}\n";
  if(std::string(standard)!="c17")text+="}\n";
  Write(input,text);auto value=request(input,root/(std::string(name)+".o"));value.imported_interfaces={dependency,adapter};Success(Compile(value));
 }
 // Explicit foreign bindings cannot impersonate names owned by this module.
 Write(source,Source);auto owned=request(source,root/"owned.o");owned.module_name="fixture.OwnedBindings";owned.package_identity="owned-bindings-v1";owned.module_owned_names=true;owned.module_source_root=(root/"provider").string();owned.emit_semantic_interface=true;
 auto rejected=Compile(owned);assert(!rejected.success);assert(std::any_of(rejected.diagnostics.begin(),rejected.diagnostics.end(),[](const auto &d){return d.message.find("explicit native name conflicts")!=std::string::npos;}));
 assert(fs::remove(source));
 auto changed=artifact.source_bytes;const auto at=changed.find("ffi_first");assert(at!=std::string::npos);changed.replace(at,9,"ffi_other");
 auto mutation=*decoded;mutation.canonical_source_sha256=ComputeInterfaceSourceSha256(changed);auto metadata=EncodeSemanticInterfaceSidecar(mutation,reader);assert(metadata);
 const auto altered=root/"Altered.objc3i";Write(altered,changed);Write(altered.string()+".json",*metadata);Write(root/"mutation.mm","int mutation_control(void){return 42;}");
 auto bad=request(root/"mutation.mm",root/"altered.o");auto altered_dependency=dependency;altered_dependency.source_path=altered.string();bad.imported_interfaces={altered_dependency};auto altered_result=Compile(bad);assert(!altered_result.success);
 assert(std::any_of(altered_result.interface_import_diagnostics.begin(),altered_result.interface_import_diagnostics.end(),[](const auto &d){return d.code=="O3IFC139";}));
 const auto main=root/"main.mm";Write(main,Main);auto entry=request(main,root/"main.o");if(ownership)entry.imported_interfaces={dependency};Success(Compile(entry));LinkAndExecute(root,entry.output_path,ownership!=0,{});
}
}
int main(){
 test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-external-bindings",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(const char *standard:{"c17","c++20","c++23"})for(int ownership:{0,1,2}){
  const auto name=std::string(standard)+"-"+std::to_string(ownership);cases.Run(name,[&]{Run(root/name,standard,ownership);});}
 return cases.Finish();
}

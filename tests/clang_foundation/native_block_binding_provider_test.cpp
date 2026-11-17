#include "standard_library_test_support.h"
#include "clang_foundation/interface_module_resolver.h"
#include "llvm/ADT/ScopeExit.h"

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
constexpr const char *External=R"blocks(
#include <objc3c/runtime/objc3_runtime_services.h>
@protocol Sendable @end
__attribute__((objc_root_class)) @interface BindingUnsafe @end

typedef void (^BindingWork)(void) __objc3_async_frame;
extern "C" objc3_task_t *binding_start(BindingWork,objc3_executor_t *) __attribute__((objc_task_detached));
extern "C" void binding_pause(void) __objc3_async_frame;
extern "C" void binding_release(void *);
)blocks";
constexpr const char *Source=R"blocks(
__attribute__((objc_root_class)) @interface BindingOwner<Sendable> {
 Class isa;
@public
 unsigned *destroyed;
 unsigned value;
}
+ (id)new;
@end
namespace BindingModule {
inline BindingWork project_reference(BindingWork __strong &work){return work;}
inline BindingWork project(BindingWork work){return work;}
inline BindingWork choose_work(unsigned choose,BindingWork left,BindingWork right){if(choose)return left;return right;}
inline BindingWork nested_work(unsigned choose,BindingWork left,BindingWork right){const BindingWork work=choose_work(choose,left,right);return choose?project(work):right;}
inline BindingWork const global_work=^void(void) __objc3_async_frame {};
inline objc3_task_t *start(objc3_executor_t *executor,unsigned choose,unsigned shape,
                           unsigned *destroyed,unsigned *ran,unsigned *deferred) {
 BindingOwner *one=[BindingOwner new],*two=[BindingOwner new];
 one->destroyed=destroyed;one->value=1;two->destroyed=destroyed;two->value=2;
 BindingWork first=^void(void) __objc3_async_frame {
  defer {++*deferred;}*ran+=one->value;__objc3_await binding_pause();
  if(one->value!=1)__builtin_trap();
 };
 BindingWork second=^void(void) __objc3_async_frame {
  defer {++*deferred;}*ran+=two->value;__objc3_await binding_pause();
  if(two->value!=2)__builtin_trap();
 };
 BindingWork selected=first;if(!choose)selected=second;
 BindingWork alias=selected;
 for(unsigned n=0;n!=2;++n)alias=choose?alias:selected;
 selected=alias;
 void (^callback)(void)=^{*ran+=one->value;};
 if(!choose)callback=^{*ran+=two->value;};
 BindingWork nested=^void(void) __objc3_async_frame {
  defer {++*deferred;}__objc3_await binding_pause();callback();
 };
 BindingWork work=alias;if(shape==1)work=nested;
 if(shape==2)work=choose?first:second;
 if(shape==3){work=(BindingWork)0;work=selected;}
 if(shape==4)work=choose_work(choose,first,second);
 if(shape==5)work=nested_work(choose,first,second);
 if(shape==6){const BindingWork immutable=project(work);work=project(immutable);}
 if(shape==7)work=project_reference(selected);
 if(shape==8)work=global_work;
 objc3_task_t *task=binding_start(work,executor);
#if !__has_feature(objc_arc)
 binding_release((__bridge void *)one);binding_release((__bridge void *)two);
#endif
 return task;
}
}
)blocks";
constexpr const char *Consumer=R"blocks(
#include <objc3c/runtime/objc3_runtime_services.h>
#include <assert.h>
static int execute(void) {
 objc3_executor_t *executor=nullptr;assert(objc3_executor_create(1,&executor)==OBJC3_SERVICE_OK);
 for(unsigned choose=0;choose!=2;++choose)for(unsigned shape=0;shape!=9;++shape)for(unsigned cancel=0;cancel!=2;++cancel){
  unsigned destroyed=0,ran=0,deferred=0;
  objc3_task_t *task=BindingModule::start(executor,choose,shape,&destroyed,&ran,&deferred);
  assert(task && destroyed==(shape==8?2u:1u) && ran==0 && deferred==0);
  if(cancel)objc3_task_request_cancel(task);
  uint32_t executed=0;assert(objc3_executor_pump(executor,UINT32_MAX,&executed)==OBJC3_SERVICE_OK);
  objc3_task_group_t *group=nullptr;assert(objc3_task_group_create(nullptr,&group)==OBJC3_SERVICE_OK);
  assert(objc3_task_group_add(group,task)==OBJC3_SERVICE_OK);assert(objc3_task_group_close(group)==OBJC3_SERVICE_OK);
  assert(objc3_task_group_join(group)==OBJC3_SERVICE_OK);
  objc3_completion completion={sizeof(completion),OBJC3_SERVICE_ABI_VERSION};assert(objc3_task_get_completion(task,&completion)==OBJC3_SERVICE_OK);
  assert(completion.kind==(cancel?OBJC3_COMPLETION_CANCELLED:OBJC3_COMPLETION_SUCCESS));
  assert(destroyed==2 && ran==(cancel || shape==8?0u:choose?1u:2u) && deferred==(cancel || shape==8?0u:1u));
  objc3_task_release(task);objc3_task_group_release(group);
 }
 objc3_executor_release(executor);return 0;
}
)blocks";
constexpr const char *Main=R"blocks(
#include <assert.h>
extern "C" void objc_release(void *);
extern "C" void binding_release(void *object){objc_release(object);}
extern "C" id class_createInstance(Class,__SIZE_TYPE__) __attribute__((ns_returns_retained));
extern "C" id object_dispose(__unsafe_unretained id);
@implementation BindingOwner
+ (id)new{return class_createInstance(self,0);}
- (void)dealloc{++*destroyed;object_dispose(self);}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease{}
#endif
@end
extern "C" void binding_pause(void) __objc3_async_frame {}
extern "C" objc3_task_t *binding_start(BindingWork work,objc3_executor_t *executor) {
 objc3_async_frame frame=work();objc3_task_t *task=nullptr;
 if(objc3_async_frame_start(&frame,executor,nullptr,&task)!=OBJC3_SERVICE_OK)(void)objc3_async_frame_dispose(&frame);
 return task;
}
extern "C" int check_a();extern "C" int check_b();extern "C" int check_c();extern "C" int check_d();
int main(){return check_a()||check_b()||check_c()||check_d();}
)blocks";
void Run(const fs::path &root,const char *standard,int ownership) {
 fs::create_directories(root/"provider");fs::create_directories(root/"include/objc3c");
 fs::create_directory_symlink(OBJC3_BLOCK_BINDING_RUNTIME_HEADERS,root/"include/objc3c/runtime");
 auto request=[&](const fs::path &input,const fs::path &output){
  auto value=Request(input,output,standard,ownership,"-O2",{"-fblocks","-I"+(root/"include").string(),"-Wno-nullability-completeness"});
  value.profile=SemanticProfile::StrictConcurrency;return value;
 };
 const auto external_source=root/"external.mm";Write(external_source,External);auto external_request=request(external_source,root/"external.o");
 external_request.module_name="fixture.BlockBindingABI";external_request.package_identity="block-binding-abi-v1";external_request.emit_semantic_interface=true;
 auto external_built=Compile(external_request);Success(external_built);assert(external_built.interface_artifact && external_built.interface_artifact->success);
 const auto external_path=root/"BlockBindingABI.objc3i";Write(external_path,external_built.interface_artifact->source_bytes);Write(external_path.string()+".json",external_built.interface_artifact->sidecar_json);
 InterfaceImportInput external;external.source_path=external_path.string();external.locked_identity=external_built.interface_artifact->identity;
 const auto include=root/"provider"/BuildInterfaceIncludePath(external.locked_identity.canonical_name);fs::create_directories(include.parent_path());Write(include,external_built.interface_artifact->source_bytes);
 const auto source=root/"provider/module.mm";
 auto publish=[&](const std::string &text,bool owned=true){
  Write(source,text);auto value=request(source,root/"provider.o");value.imported_interfaces={external};value.module_name="fixture.BlockBindings";value.package_identity="block-bindings-v1";
  value.emit_semantic_interface=true;value.module_owned_names=owned;if(owned)value.module_source_root=(root/"provider").string();
  value.interface_exports={"BindingOwner","BindingModule"};
  auto result=Compile(value);Success(result);assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;
 };
 const auto artifact=publish(Source),replay=publish(artifact.source_bytes,false);
 assert(artifact.source_bytes==replay.source_bytes && artifact.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
 publish(Source);assert(fs::remove(source));
 const auto interface=root/"BlockBindings.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 InterfaceImportInput dependency;dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;
 for(const char *name:{"a","b","c","d"}){
  const bool directed=*name=='c' || *name=='d';
  const auto input=root/(std::string(name)+".mm");
  std::string prefix;
  if(directed)prefix="#include <objc3c/runtime/objc3_runtime_services.h>\n@import fixture.BlockBindingABI;\n#include \"provider/"+BuildInterfaceIncludePath(external.locked_identity.canonical_name)+"\"\n@import fixture.BlockBindings;\n";
  Write(input,prefix+Consumer+"extern \"C\" int check_"+name+"(){return execute();}");
  auto value=request(input,root/(std::string(name)+".o"));
  if(!directed)value.imported_interfaces={external,dependency};
  else {
   value.source_directed_imports=true;value.interface_search_paths={root.string()};
   for(const auto &identity:{external.locked_identity,dependency.locked_identity}){
    InterfaceModuleRequest module;module.canonical_name=identity.canonical_name;module.locked_identity=identity;module.initially_visible=false;value.module_imports.push_back(module);
   }
  }
  Success(Compile(value));
 }
 for(const char *body:{
  "void rejected(unsigned choose,BindingUnsafe *object){BindingWork bad=^void(void) __objc3_async_frame {(void)object;};BindingWork safe=^void(void) __objc3_async_frame {};binding_start(BindingModule::choose_work(choose,bad,safe),nullptr);}",
  "void rejected(unsigned choose,BindingUnsafe *object){BindingWork bad=^void(void) __objc3_async_frame {(void)object;};BindingWork safe=^void(void) __objc3_async_frame {};binding_start(BindingModule::nested_work(choose,safe,bad),nullptr);}",
  "void rejected(BindingWork unknown){binding_start(BindingModule::project(unknown),nullptr);}"}) {
  const auto input=root/"bad.mm";Write(input,body);auto value=request(input,root/"bad.o");value.imported_interfaces={external,dependency};auto result=Compile(value);assert(!result.success);
  bool matched=false;for(const auto &diagnostic:result.diagnostics)matched|=diagnostic.message.find("non-Sendable type")!=std::string::npos || diagnostic.message.find("cannot prove Sendable captures")!=std::string::npos;assert(matched);
 }
 const auto main=root/"main.mm";Write(main,Main);auto entry=request(main,root/"main.o");entry.imported_interfaces={external,dependency};Success(Compile(entry));
 LinkAndExecute(root,entry.output_path,true,{(root/"c.o").string(),(root/"d.o").string(),OBJC3_BLOCK_BINDING_RUNTIME_LIBRARY,"-pthread"});
}
}
int main(){
 test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-block-bindings",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(const char *standard:{"c++20","c++23"})for(int ownership:{1,2}){
  const auto name=std::string(standard)+"-"+std::to_string(ownership);cases.Run(name,[&]{Run(root/name,standard,ownership);});}
 return cases.Finish();
}

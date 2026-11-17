#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "clang_foundation/interface_module_resolver.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
constexpr const char *External=R"boundary(
#include <objc3c/runtime/objc3_runtime_services.h>
typedef int BoundaryHandle __attribute__((objc_unsafe_sendable));
extern "C" BoundaryHandle boundary_open(void) noexcept;
extern "C" void boundary_close(BoundaryHandle) noexcept;
extern "C" int boundary_target(BoundaryHandle) noexcept __attribute__((objc_executor(named("boundary.serial"))));
extern "C" int boundary_finish(int) noexcept;
extern "C" int boundary_number(int) noexcept __attribute__((objc_executor(named("boundary.serial"))));
extern "C" void boundary_begin(objc3_executor_t *) noexcept;
extern "C" void boundary_completed(void) noexcept;
)boundary";
constexpr const char *Source=R"boundary(
#include <objc3c/runtime/objc3_runtime_services.h>
#include <utility>
__attribute__((objc_root_class)) @interface BoundaryRoot @end
actor class BoundaryActor:BoundaryRoot
- (const char *_Nonnull)view __attribute__((objc_returns_inner_pointer));
- (int)stable __attribute__((objc_nonisolated));
@end
namespace BoundaryModule {
inline int ordinary_move(int value) __objc3_async_frame {
 return __objc3_await boundary_number(std::move(value));
}
inline int ordinary_views(int value) __objc3_async_frame {
 int &alias=std::forward<int &>(value);
 const int &view=std::as_const(alias);
 return __objc3_await boundary_number(std::forward<const int &>(view));
}
inline int transfer(void) __objc3_async_frame {
 int descriptor __attribute__((objc_resource(close=boundary_close,invalid=-1)))=boundary_open();
 int copied=0;copied=descriptor;
 BoundaryHandle admitted=copied;
 int result=__objc3_await boundary_target(admitted);
 return boundary_finish(result);
}
}
)boundary";
constexpr const char *Consumer=R"boundary(
#include <objc3c/runtime/objc3_runtime_services.h>
#include <assert.h>
static int execute(void){
 objc3_executor_t *caller=0,*destination=0;
 assert(objc3_executor_create(1,&caller)==OBJC3_SERVICE_OK);
 assert(objc3_executor_get_named("boundary.serial",&destination)==OBJC3_SERVICE_OK);
 boundary_begin(caller);objc3_async_frame frame=BoundaryModule::transfer();objc3_task_t *task=0;
 assert(objc3_async_frame_start(&frame,caller,0,&task)==OBJC3_SERVICE_OK);
 objc3_completion completion={sizeof(completion),OBJC3_SERVICE_ABI_VERSION};bool complete=false;
 for(unsigned step=0;step!=128;++step){
  uint32_t count=0;assert(objc3_executor_pump(destination,32,&count)==OBJC3_SERVICE_OK);
  assert(objc3_executor_pump(caller,32,&count)==OBJC3_SERVICE_OK);
  auto status=objc3_task_get_completion(task,&completion);
  if(status==OBJC3_SERVICE_OK){complete=true;break;}assert(status==OBJC3_SERVICE_NOT_READY);
 }
 assert(complete && completion.kind==OBJC3_COMPLETION_SUCCESS && completion.has_payload && completion.payload);
 assert(*static_cast<const int *>(completion.payload)==42);boundary_completed();
 objc3_task_release(task);
 for(unsigned operation=0;operation!=2;++operation){
  auto ordinary=operation?BoundaryModule::ordinary_views(41):BoundaryModule::ordinary_move(41);task=0;
  assert(objc3_async_frame_start(&ordinary,caller,0,&task)==OBJC3_SERVICE_OK);complete=false;
  for(unsigned step=0;step!=128;++step){uint32_t count=0;
   assert(objc3_executor_pump(destination,32,&count)==OBJC3_SERVICE_OK && objc3_executor_pump(caller,32,&count)==OBJC3_SERVICE_OK);
   if(objc3_task_get_completion(task,&completion)==OBJC3_SERVICE_OK){complete=true;break;}
  }
  assert(complete && completion.kind==OBJC3_COMPLETION_SUCCESS && completion.has_payload && *static_cast<const int *>(completion.payload)==42);
  objc3_task_release(task);
 }
 objc3_executor_release(destination);objc3_executor_release(caller);return 0;
}
)boundary";
constexpr const char *Main=R"boundary(
#include <objc3c/runtime/objc3_runtime_services.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
static objc3_executor_t *destination=0,*caller=0;
static unsigned opened=0,closed=0;static int last_descriptor=-1;
extern "C" int boundary_open(void) noexcept {
 int ends[2];assert(pipe(ends)==0);assert(close(ends[1])==0);++opened;last_descriptor=ends[0];return ends[0];
}
extern "C" void boundary_close(int descriptor) noexcept {
 assert(descriptor==last_descriptor && opened==1 && closed==0);assert(close(descriptor)==0);++closed;
}
extern "C" int boundary_target(int descriptor) noexcept {
 assert(objc3_executor_is_current(destination));assert(opened==1 && closed==0 && descriptor==last_descriptor);
 assert(fcntl(descriptor,F_GETFD)>=0);return 42;
}
extern "C" int boundary_number(int value) noexcept {assert(objc3_executor_is_current(destination) && value==41);return value+1;}
extern "C" int boundary_finish(int value) noexcept {
 assert(objc3_executor_is_current(caller));assert(value==42 && opened==1 && closed==0);
 assert(fcntl(last_descriptor,F_GETFD)>=0);return value;
}
extern "C" void boundary_begin(objc3_executor_t *value) noexcept {caller=value;opened=closed=0;last_descriptor=-1;}
extern "C" void boundary_completed(void) noexcept {
 assert(opened==1 && closed==1);errno=0;assert(fcntl(last_descriptor,F_GETFD)==-1 && errno==EBADF);
}
extern "C" int check_a();extern "C" int check_b();
int main(){
 assert(objc3_executor_create(1,&destination)==OBJC3_SERVICE_OK);
 assert(objc3_executor_register_named("boundary.serial",destination)==OBJC3_SERVICE_OK);
 const int result=check_a() || check_b();
 assert(objc3_executor_unregister_named("boundary.serial")==OBJC3_SERVICE_OK);objc3_executor_release(destination);return result;
}
)boundary";
void Run(const fs::path &root,const char *standard,int ownership){
 fs::create_directories(root/"provider");fs::create_directories(root/"include/objc3c");
 fs::create_directory_symlink(OBJC3_BOUNDARY_RUNTIME_HEADERS,root/"include/objc3c/runtime");
 auto request=[&](const fs::path &input,const fs::path &output){
  auto value=Request(input,output,standard,ownership,"-O2",{"-I"+(root/"include").string(),"-Wno-nullability-completeness"});
  value.profile=SemanticProfile::StrictConcurrency;return value;
 };
 const auto external_source=root/"external.mm";Write(external_source,External);auto external_request=request(external_source,root/"external.o");
 external_request.module_name="fixture.BoundaryABI";external_request.package_identity="boundary-abi-v1";external_request.emit_semantic_interface=true;
 auto external_built=Compile(external_request);Success(external_built);assert(external_built.interface_artifact && external_built.interface_artifact->success);
 const auto external_path=root/"BoundaryABI.objc3i";Write(external_path,external_built.interface_artifact->source_bytes);Write(external_path.string()+".json",external_built.interface_artifact->sidecar_json);
 InterfaceImportInput external;external.source_path=external_path.string();external.locked_identity=external_built.interface_artifact->identity;
 const auto replay_dependency=root/"provider"/BuildInterfaceIncludePath(external.locked_identity.canonical_name);
 fs::create_directories(replay_dependency.parent_path());Write(replay_dependency,external_built.interface_artifact->source_bytes);
 const auto source=root/"provider/module.mm";
 auto publish=[&](const std::string &text,bool owned=true){
  Write(source,text);auto value=request(source,root/"provider.o");value.imported_interfaces={external};value.module_name="fixture.BoundaryResources";value.package_identity="boundary-resources-v1";
  value.emit_semantic_interface=true;value.module_owned_names=owned;if(owned)value.module_source_root=(root/"provider").string();value.interface_exports={"BoundaryRoot","BoundaryActor","BoundaryModule"};
  auto result=Compile(value);Success(result);assert(result.interface_artifact && result.interface_artifact->success);return *result.interface_artifact;
 };
 const auto artifact=publish(Source),replay=publish(artifact.source_bytes,false);
 assert(artifact.source_bytes==replay.source_bytes && artifact.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
 publish(Source);assert(fs::remove(source));
 const auto interface=root/"BoundaryResources.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 InterfaceImportInput dependency;dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;
 for(const char *name:{"a","b"}){
  const auto input=root/(std::string(name)+".mm");Write(input,std::string(Consumer)+"extern \"C\" int check_"+name+"(){return execute();}");
  auto value=request(input,root/(std::string(name)+".o"));value.imported_interfaces={external,dependency};Success(Compile(value));
 }
 for(const auto &[text,message]:std::vector<std::pair<std::string,std::string>>{
     {"int rejected(void) __objc3_async_frame {int h __attribute__((objc_resource(close=boundary_close,invalid=-1)))=boundary_open();return __objc3_await boundary_target(h);}","cleanup-managed concurrent value"},
     {"int rejected(void) __objc3_async_frame {int h __attribute__((objc_resource(close=boundary_close,invalid=-1)))=boundary_open();int copy=h;return __objc3_await boundary_target(copy);}","cleanup-managed concurrent value"},
     {"int rejected(void) __objc3_async_frame {int h __attribute__((objc_resource(close=boundary_close,invalid=-1)))=boundary_open();int copy=0;copy=h;return __objc3_await boundary_target(copy);}","cleanup-managed concurrent value"},
     {"#include <utility>\nint rejected(void) __objc3_async_frame {int h __attribute__((objc_resource(close=boundary_close,invalid=-1)))=boundary_open();int copy=0;copy=h;return __objc3_await boundary_target(std::move(copy));}","cleanup-managed concurrent value"},
     {"#include <utility>\nint rejected(void) __objc3_async_frame {int h __attribute__((objc_resource(close=boundary_close,invalid=-1)))=boundary_open();int copy=0;int &alias=std::forward<int &>(copy);alias=h;return __objc3_await boundary_target(std::as_const(copy));}","cleanup-managed concurrent value"},
     {"int rejected(int copy) __objc3_async_frame {int h __attribute__((objc_resource(close=boundary_close,invalid=-1)))=boundary_open();copy=h;return __objc3_await boundary_target(copy);}","cleanup-managed concurrent value"},
     {"int rejected(int choose) __objc3_async_frame {int h __attribute__((objc_resource(close=boundary_close,invalid=-1)))=boundary_open();int first=0,second=first;first=second;second=choose?first:h;first=second;return __objc3_await boundary_target(first);}","cleanup-managed concurrent value"},
     {"int rejected(void) __objc3_async_frame {int h __attribute__((objc_resource(close=boundary_close,invalid=-1)))=boundary_open();int copy=0;copy=h;int &view=copy;return __objc3_await boundary_target(view);}","cleanup-managed concurrent value"},
     {"void may_modify(int *) noexcept;int rejected(void) __objc3_async_frame {int copy=0;may_modify(&copy);return __objc3_await boundary_target(copy);}","cleanup-managed concurrent value"},
     {"struct NativeManagedPair{int value;};void close_pair(NativeManagedPair *) noexcept;int rejected(void) __objc3_async_frame {NativeManagedPair owner __attribute__((cleanup(close_pair)))={42};auto [copy]=owner;return __objc3_await boundary_target(copy);}","cleanup-managed concurrent value"},
     {"template<class W>int bad_spawn(W) __attribute__((objc_task_spawn));int rejected(void){int h __attribute__((objc_resource(close=boundary_close,invalid=-1)))=boundary_open();int copy=0;copy=h;return bad_spawn([copy]()->int __objc3_async_frame {return copy;});}","cleanup-managed concurrent value"},
     {"@interface BoundaryActor (RejectedView) - (const char *_Nonnull)bad __attribute__((objc_nonisolated,objc_returns_inner_pointer));@end","nonisolated actor member cannot transfer a receiver-owned interior view"}}){
  const auto input=root/"bad.mm";Write(input,text);auto value=request(input,root/"bad.o");value.imported_interfaces={external,dependency};auto result=Compile(value);assert(!result.success);
  assert(std::any_of(result.diagnostics.begin(),result.diagnostics.end(),[&](const auto &d){return d.message.find(message)!=std::string::npos;}));
 }
 const auto main=root/"main.cpp";Write(main,Main);auto entry=request(main,root/"main.o");Success(Compile(entry));
 LinkAndExecute(root,entry.output_path,true,{OBJC3_BOUNDARY_RUNTIME_LIBRARY,"-pthread"});
}
}
int main(){
 test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-boundary-resources",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(const char *standard:{"c++20","c++23"})for(int ownership:{1,2}){
  const auto name=std::string(standard)+"-"+std::to_string(ownership);cases.Run(name,[&]{Run(root/name,standard,ownership);});}
 return cases.Finish();
}

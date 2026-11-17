#include "standard_library_test_support.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/ADT/ScopeExit.h"
#include <algorithm>

using namespace objc3::clang_foundation;
using namespace objc3::clang_foundation::standard_library_test;
namespace {
constexpr const char *Source=R"actor(
#include <objc3c/runtime/objc3_runtime_services.h>
#include <assert.h>
#include <receiver_runtime.h>
__attribute__((objc_root_class)) @interface ReceiverBase {Class isa;}
+ (instancetype _Nullable)new;
- (instancetype _Nullable)init;
@end
actor class ReceiverBound:ReceiverBase {int _value;}
@property(nonatomic) int value;
- (int)read __attribute__((nothrow));
- (int)aliases:(int)flag __attribute__((nothrow));
- (instancetype _Nullable)init __attribute__((objc_nonisolated));
- (instancetype _Nonnull)identity __attribute__((objc_nonisolated));
@end
static void *opaque(id object){
#if __has_feature(objc_arc)
 return (__bridge void *)object;
#else
 return (void *)object;
#endif
}
@implementation ReceiverBase
+ (instancetype _Nullable)new {
 id _Nullable created=class_createInstance(self,0);
 guard let object=created else {return 0;}
 return [object init];
}
- (instancetype _Nullable)init{return self;}
- (void)dealloc{object_dispose(self);}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease{}
#endif
@end
@implementation ReceiverBound
@synthesize value=_value;
- (int)read {
 objc3_current_actor_facts facts={sizeof(facts),OBJC3_ACTOR_SERVICE_VERSION};
 assert(objc3_actor_current_facts(&facts)==OBJC3_SERVICE_OK);
 objc3_actor_t *actor=0;assert(objc3_actor_get_for_object(opaque(self),&actor)==OBJC3_SERVICE_OK);
 assert(facts.actor_identity==objc3_actor_identity(actor));objc3_actor_release(actor);return _value;
}
- (int)aliases:(int)flag {
 ReceiverBound *_Nonnull const first=self;
 ReceiverBound *_Nonnull const second=first;
 ReceiverBound *_Nonnull const choice=flag?second:self;
 ReceiverBound *_Nonnull const *const cell=&choice;
 ReceiverBound *_Nonnull const *const *const nested=&cell;
 ReceiverBound *_Nonnull const &reference=self;
 ReceiverBound *_Nonnull __unsafe_unretained const view=self;
 first.value=17;choice.value+=1;
 if([first read]!=18 || [second read]!=18 || [choice read]!=18 || reference.value!=18 || [view read]!=18)return 1;
 if([*cell read]!=18 || [**nested read]!=18 || [*(&self) read]!=18)return 2;
 if([(flag?first:second) read]!=18 || [((void)flag,self) read]!=18)return 3;
 return 42;
}
- (instancetype _Nullable)init __attribute__((objc_nonisolated)){self=[super init];return self;}
- (instancetype _Nonnull)identity __attribute__((objc_nonisolated)){return self;}
@end
)actor";
constexpr const char *Consumer=R"actor(
#include <objc3c/runtime/objc3_runtime_services.h>
#include <assert.h>
extern "C" void release_receiver(void *) __asm__("objc_release");
static int invoke(ReceiverBound *_Nonnull receiver,int flag,objc3_executor_t *caller) __objc3_async_frame {
 int result=__objc3_await [receiver aliases:flag];
 assert(objc3_executor_is_current(caller));return result;
}
static int run(void){
 @autoreleasepool {
 ReceiverBound *_Nullable created=[ReceiverBound new];guard let object=created else {return 1;}
 if([object identity]!=object)return 2;
 void *raw;
#if __has_feature(objc_arc)
 raw=(__bridge void *)object;
#else
 raw=(void *)object;
#endif
 objc3_executor_t *caller=0,*destination=0;
 assert(objc3_executor_create(1,&caller)==OBJC3_SERVICE_OK);
 assert(objc3_executor_create(1,&destination)==OBJC3_SERVICE_OK);
 objc3_actor_t *actor=0;assert(objc3_actor_create(destination,&actor)==OBJC3_SERVICE_OK);
 assert(objc3_actor_bind_object(raw,actor)==OBJC3_SERVICE_OK);
 for(int flag=0;flag!=2;++flag){
  objc3_async_frame frame=invoke(object,flag,caller);objc3_task_t *task=0;
  assert(objc3_async_frame_start(&frame,caller,0,&task)==OBJC3_SERVICE_OK);
  objc3_completion completion={sizeof(completion),OBJC3_SERVICE_ABI_VERSION};bool complete=false;
  for(unsigned step=0;step!=128;++step){
   uint32_t count=0;assert(objc3_executor_pump(destination,32,&count)==OBJC3_SERVICE_OK);
   assert(objc3_executor_pump(caller,32,&count)==OBJC3_SERVICE_OK);
   auto status=objc3_task_get_completion(task,&completion);
   if(status==OBJC3_SERVICE_OK){complete=true;break;}assert(status==OBJC3_SERVICE_NOT_READY);
  }
  assert(complete && completion.kind==OBJC3_COMPLETION_SUCCESS && completion.has_payload && completion.payload);
  assert(*static_cast<const int *>(completion.payload)==42);objc3_task_release(task);
 }
 assert(objc3_actor_close(actor)==OBJC3_SERVICE_OK);assert(objc3_actor_join(actor)==OBJC3_SERVICE_OK);
 assert(objc3_actor_unbind_object(raw)==OBJC3_SERVICE_OK);objc3_actor_release(actor);
#if !__has_feature(objc_arc)
 release_receiver(raw);
#endif
 objc3_executor_release(destination);objc3_executor_release(caller);return 0;
 }
}
)actor";
void Run(const fs::path &root,const char *standard,int ownership){
 fs::create_directories(root/"provider");fs::create_directories(root/"include/objc3c");
 fs::create_directory_symlink(OBJC3_ACTOR_BINDING_RUNTIME_HEADERS,root/"include/objc3c/runtime");
 Write(root/"include/receiver_runtime.h",R"(extern "C" id _Nullable class_createInstance(Class,unsigned long) __attribute__((ns_returns_retained));
extern "C" id _Nullable object_dispose(__unsafe_unretained id);
)");
 auto request=[&](const fs::path &input,const fs::path &output){
  auto value=Request(input,output,standard,ownership,"-O2",{});value.profile=SemanticProfile::StrictConcurrency;
  value.arguments.insert(value.arguments.end(),{"-fblocks","-fexceptions","-fobjc-exceptions","-fobjc-arc-exceptions","-Wno-nullability-completeness","-I"+(root/"include").string()});return value;
 };
 const auto source=root/"provider/module.mm";
 auto publish=[&](const std::string &text,bool owned=true){
  Write(source,text);auto value=request(source,root/"provider.o");value.module_name="fixture.ActorReceiver";value.package_identity="actor-receiver-v1";
  value.module_owned_names=owned;if(owned)value.module_source_root=(root/"provider").string();value.emit_semantic_interface=true;value.interface_exports={"ReceiverBase","ReceiverBound"};
  auto built=Compile(value);Success(built);assert(built.interface_artifact && built.interface_artifact->success);return *built.interface_artifact;
 };
 const auto artifact=publish(Source);const auto replay=publish(artifact.source_bytes,false);
 assert(artifact.source_bytes==replay.source_bytes && artifact.identity.semantic_fingerprint==replay.identity.semantic_fingerprint);
 publish(Source);assert(fs::remove(source));
 const auto interface=root/"ActorReceiver.objc3i";Write(interface,artifact.source_bytes);Write(interface.string()+".json",artifact.sidecar_json);
 const auto &reader=SupportedSemanticInterfaceCapabilities();auto decoded=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,reader);assert(decoded);
 assert(std::find(decoded->required_semantics.begin(),decoded->required_semantics.end(),kActorReceiverBindingSemantic)!=decoded->required_semantics.end());
 for(const char *name:{"a","b"}){
  const auto input=root/(std::string(name)+".mm");Write(input,std::string(Consumer)+"extern \"C\" int check_"+name+"(){return run();}");
  auto value=request(input,root/(std::string(name)+".o"));InterfaceImportInput dependency;dependency.source_path=interface.string();dependency.locked_identity=artifact.identity;value.imported_interfaces={dependency};Success(Compile(value));
 }
 std::vector<llvm::StringRef> old_reader;for(auto cap:reader)if(cap!=kActorReceiverBindingSemantic)old_reader.push_back(cap);
 auto old=DecodeSemanticInterfaceSidecar(artifact.sidecar_json,artifact.source_bytes,old_reader);assert(!old);llvm::consumeError(old.takeError());
 auto missing=*decoded;std::erase(missing.required_semantics,kActorReceiverBindingSemantic);auto invalid=EncodeSemanticInterfaceSidecar(missing,reader);assert(!invalid);llvm::consumeError(invalid.takeError());
 const auto main=root/"main.cpp";Write(main,"extern \"C\" int check_a();extern \"C\" int check_b();int main(){return check_a() || check_b();}");
 auto entry=request(main,root/"main.o");Success(Compile(entry));LinkAndExecute(root,entry.output_path,true,{OBJC3_ACTOR_BINDING_RUNTIME_LIBRARY,"-pthread"});
}
}
int main(){
 test::IsolatedCases cases;llvm::SmallString<128> temporary;assert(!llvm::sys::fs::createUniqueDirectory("objc3-actor-bindings",temporary));
 const fs::path root(temporary.c_str());auto cleanup=llvm::scope_exit([&]{fs::remove_all(root);});
 for(const auto &[standard,ownership]:std::vector<std::pair<const char *,int>>{{"c++20",1},{"c++20",2},{"c++23",1},{"c++23",2}}){
  const auto name=std::string(standard)+"-"+std::to_string(ownership);cases.Run(name,[&]{Run(root/name,standard,ownership);});}
 return cases.Finish();
}
